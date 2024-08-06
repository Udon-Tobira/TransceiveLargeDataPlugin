// Fill out your copyright notice in the Description page of Project Settings.

#include "BeaconMulticastGameModeBase.h"

#include "LogMulticastBeaconGameModeBase.h"

ABPOnlineBeaconMulticastHostObject*
    ABeaconMulticastGameModeBase::GetOnlineBeaconMulticastHostObject() const {
	return OnlineBeaconMulticastHostObject;
}

void ABeaconMulticastGameModeBase::PreInitializeComponents() {
	Super::PreInitializeComponents();

	// get World
	UWorld* World = GetWorld();

	// check World
	check(World != nullptr);

	// create spawn parameter
	FActorSpawnParameters SpawnParam_Transient;
	SpawnParam_Transient.Instigator = GetInstigator();
	SpawnParam_Transient.ObjectFlags |= RF_Transient; // We never want to save

	// spawn OnlineBeaconMulticastHostObject
	OnlineBeaconMulticastHostObject =
	    World->SpawnActor<ABPOnlineBeaconMulticastHostObject>(
	        OnlineBeaconMulticastHostObjectClass, SpawnParam_Transient);

	// check OnlineBeaconMulticastHostObject
	check(OnlineBeaconMulticastHostObject != nullptr);

	// spawn OnlineBeaconHost
	OnlineBeaconHost = World->SpawnActor<AOnlineBeaconHost>(SpawnParam_Transient);

	// check OnlineBeaconHost
	check(OnlineBeaconHost != nullptr);

	// initialize OnlineBeaconHost
	const auto& SuccessInitOnlineBeaconHost = OnlineBeaconHost->InitHost();

	// if failed to initialize OnlineBeaconHost
	if (!SuccessInitOnlineBeaconHost) {
		// log as Error
		UE_LOG(LogMulticastBeaconGameModeBase, Error,
		       TEXT("failed to initialize OnlineBeaconHost"));

		// finish
		return;
	}

	// register host object of OnlineBeaconHost
	OnlineBeaconHost->RegisterHost(OnlineBeaconMulticastHostObject);

	// start beacon host
	OnlineBeaconHost->PauseBeaconRequests(false);
}

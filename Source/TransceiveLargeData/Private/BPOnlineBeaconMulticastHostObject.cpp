// Fill out your copyright notice in the Description page of Project Settings.

#include "BPOnlineBeaconMulticastHostObject.h"

TArray<ABPOnlineBeaconMulticastClient*>
    ABPOnlineBeaconMulticastHostObject::GetClientActors() const {
	return static_cast<TArray<ABPOnlineBeaconMulticastClient*>>(ClientActors);
}

ABPOnlineBeaconMulticastClient*
    ABPOnlineBeaconMulticastHostObject::GetServerOnlyClientActor() const {
	return ServerOnlyClientActor;
}

ABPOnlineBeaconMulticastClient*
    ABPOnlineBeaconMulticastHostObject::SpawnBeaconActor(
        UNetConnection* ClientConnection) {
	// spawn beacon multicast client
	const auto& SpawnedBeaconMulticastClient =
	    CastChecked<ABPOnlineBeaconMulticastClient>(
	        Super::SpawnBeaconActor(ClientConnection));

	// broad cast that client is spawned
	OnBeaconMulticastClientSpawnedDelegate.Broadcast(
	    SpawnedBeaconMulticastClient);

	// return spawned client
	return SpawnedBeaconMulticastClient;
}

ABPOnlineBeaconMulticastHostObject::ABPOnlineBeaconMulticastHostObject(
    const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) {
	// set ClientBeaconActorClass
	ClientBeaconActorClass = ABPOnlineBeaconMulticastClient::StaticClass();

	// set BeaconTypeName
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

void ABPOnlineBeaconMulticastHostObject::PostActorCreated() {
	// get World
	const auto& World = GetWorld();

	// check World
	check(nullptr != World);

	// spawn client actor only used for server
	ServerOnlyClientActor =
	    GetWorld()->SpawnActor<ABPOnlineBeaconMulticastClient>();

	// set owner
	ServerOnlyClientActor->SetBeaconOwner(this);
}

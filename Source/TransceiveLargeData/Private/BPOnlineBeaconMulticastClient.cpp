// Fill out your copyright notice in the Description page of Project Settings.

#include "BPOnlineBeaconMulticastClient.h"

#include "BPOnlineBeaconMulticastHostObject.h"
#include "BeaconMulticastGameModeBase.h"
#include "LogBPOnlineBeaconMulticastClient.h"

void ABPOnlineBeaconMulticastClient::Multicast(const TArray<uint8>& Data) {
	// get broadcast target beacon multicast client
	const auto& BroadcastTargetBeaconMulticastClient =
	    GetBroadcastTargetBeaconMulticastClient();

	// broadcast to the broadcast target beacon multicast client
	BroadcastTargetBeaconMulticastClient->EventReceivedDataDelegate.Broadcast(
	    Data);

	// if I'm on client
	if (ENetMode::NM_Client == GetNetMode()) {
		// send data to server
		TransceiveLargeDataComponent->SendData(
		    Data, ETransceiveLargeDataDirection::Server);
	}
	// if I'm on server
	else {
		// get all beacon multicast clients
		const auto& BeaconMulticastClients = GetBeaconMulticastClients();

		// for each beacon multicast clients
		for (const auto& BeaconMulticastClient : BeaconMulticastClients) {
			// send data to its owning client
			BeaconMulticastClient->TransceiveLargeDataComponent->SendData(
			    Data, ETransceiveLargeDataDirection::Client);
		}
	}
}

ABPOnlineBeaconMulticastClient::ABPOnlineBeaconMulticastClient()
    : TransceiveLargeDataComponent(
          CreateDefaultSubobject<UTransceiveLargeDataComponent>(
              TEXT("TransceiveLargeDataComponent"))) {
	// create default scene root
	RootComponent =
	    CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
}

void ABPOnlineBeaconMulticastClient::OnConstruction(
    const FTransform& Transform) {
	// bind on received data on
	TransceiveLargeDataComponent->EventReceivedDataDelegate.AddDynamic(
	    this, &ABPOnlineBeaconMulticastClient::OnReceivedData);

	// if I'm on server
	if (ENetMode::NM_Client != GetNetMode()) {
		// bind on received data on server
		TransceiveLargeDataComponent->EventReceivedDataDelegate.AddDynamic(
		    this, &ABPOnlineBeaconMulticastClient::OnReceivedDataOnServer);
	}
}

void ABPOnlineBeaconMulticastClient::OnReceivedData(const TArray<uint8>& Data) {
	// get broadcast target beacon multicast client
	const auto& BroadcastTargetBeaconMulticastClient =
	    GetBroadcastTargetBeaconMulticastClient();

	// broadcast to the broadcast target beacon multicast client
	BroadcastTargetBeaconMulticastClient->EventReceivedDataDelegate.Broadcast(
	    Data);
}

void ABPOnlineBeaconMulticastClient::OnReceivedDataOnServer(
    const TArray<uint8>& Data) {
	// check I'm on server
	check(ENetMode::NM_Client != GetNetMode());

	// get all beacon multicast clients
	const auto& BeaconMulticastClients = GetBeaconMulticastClients();

	// for each beacon multicast clients
	for (const auto& BeaconMulticastClient : BeaconMulticastClients) {
		// If it's not the client who sent the data
		if (BeaconMulticastClient != this) {
			// send data to its owning client
			TransceiveLargeDataComponent->SendData(
			    Data, ETransceiveLargeDataDirection::Client);
		}
	}
}

TArray<ABPOnlineBeaconMulticastClient*>
    ABPOnlineBeaconMulticastClient::GetBeaconMulticastClients() const {
	const auto& BeaconMulticastHostObject = GetBeaconMulticastHostObject();

	// check MulticastHostObject
	check(nullptr != BeaconMulticastHostObject);

	// get all beacon multicast clients
	const auto& BeaconMulticastClients =
	    BeaconMulticastHostObject->GetClientActors();

	// return all beacon multicast clients
	return BeaconMulticastClients;
}

ABPOnlineBeaconMulticastHostObject*
    ABPOnlineBeaconMulticastClient::GetBeaconMulticastHostObject() const {
	// check I'm on server
	check(ENetMode::NM_Client != GetNetMode());

	// get World
	const auto& World = GetWorld();

	// check World
	check(nullptr != World);

	// get GameMode
	const auto& GameMode = World->GetAuthGameMode();

	// check GameMode
	check(nullptr != GameMode);

	// cast
	const auto& BeaconMulticastGameMode =
	    Cast<ABeaconMulticastGameModeBase>(GameMode);

	// check Cast
	checkf(nullptr != BeaconMulticastGameMode,
	       TEXT("To use this class, the game mode must be "
	            "ABeaconMulticastGameModeBase or a class derived from it."));

	// get beacon MulticastHostObject
	const auto& BeaconMulticastHostObject =
	    BeaconMulticastGameMode->GetOnlineBeaconMulticastHostObject();

	return BeaconMulticastHostObject;
}

const ABPOnlineBeaconMulticastClient*
    ABPOnlineBeaconMulticastClient::GetBroadcastTargetBeaconMulticastClient()
        const {
	// if I'm on client
	if (ENetMode::NM_Client == GetNetMode()) {
		return this;
	}
	// if I'm on server
	else {
		const auto& BeaconMulticastHostObject = GetBeaconMulticastHostObject();

		// check MulticastHostObject
		check(nullptr != BeaconMulticastHostObject);

		// get ServerOnlyClientActor
		const auto& ServerOnlyClientActor =
		    BeaconMulticastHostObject->GetServerOnlyClientActor();

		return ServerOnlyClientActor;
	}
}

ABPOnlineBeaconMulticastClient*
    ABPOnlineBeaconMulticastClient::GetBroadcastTargetBeaconMulticastClient() {
	const auto* const ConstThis = this;
	return const_cast<ABPOnlineBeaconMulticastClient*>(
	    ConstThis->GetBroadcastTargetBeaconMulticastClient());
}

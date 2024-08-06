// Fill out your copyright notice in the Description page of Project Settings.

#include "BPOnlineBeaconMulticastClient.h"

#include "BPOnlineBeaconMulticastHostObject.h"
#include "BeaconMulticastGameModeBase.h"
#include "LogBPOnlineBeaconMulticastClient.h"
#include "Net/UnrealNetwork.h"

void ABPOnlineBeaconMulticastClient::Multicast(const TArray<uint8>& Data,
                                               const FName& ChannelName) {
	// get broadcast target beacon multicast client
	const auto& BroadcastTargetBeaconMulticastClient =
	    GetBroadcastTargetBeaconMulticastClient();

	// broadcast to the broadcast target beacon multicast client
	BroadcastTargetBeaconMulticastClient->OnReceivedDataDynamicDelegate.Broadcast(
	    Data, ChannelName);

	// get index of Channel
	const auto& ChannelIndex = ChannelNames.Find(ChannelName);

	// assert specified channel exists
	check(INDEX_NONE != ChannelIndex);

	// if I'm on client
	if (ENetMode::NM_Client == GetNetMode()) {
		// get TransceiveLargeDataComponent for the specified ChannelName
		const auto& TransceiveLargeDataComponent =
		    TransceiveLargeDataComponents[ChannelIndex];

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
			// get TransceiveLargeDataComponent for the specified ChannelName
			const auto& TransceiveLargeDataComponent =
			    BeaconMulticastClient->TransceiveLargeDataComponents[ChannelIndex];

			// send data to its owning client
			TransceiveLargeDataComponent->SendData(
			    Data, ETransceiveLargeDataDirection::Client);
		}
	}
}

ABPOnlineBeaconMulticastClient::ABPOnlineBeaconMulticastClient() {
	// create default scene root
	RootComponent =
	    CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

	// replicate
	bReplicates = true;
}

void ABPOnlineBeaconMulticastClient::OnConstruction(
    const FTransform& Transform) {
	Super::OnConstruction(Transform);

	// if I'm on server
	if (ENetMode::NM_Client != GetNetMode()) {
		// for each ChannelNames
		for (const auto& ChannelName : ChannelNames) {
			// name "TransceiveLargeDataComponent_ChannelName"
			const FName TransceiveLargeDataComponentName =
			    FName(UTransceiveLargeDataComponent::StaticClass()->GetName() + "_" +
			          ChannelName.ToString());

			// create TransceiveLargeDataComponent
			const auto& TransceiveLargeDataComponent =
			    NewObject<UTransceiveLargeDataComponent>(
			        this, TransceiveLargeDataComponentName);

			// register GC that owner is this BeaconMulticastClient
			TransceiveLargeDataComponent->RegisterComponent();

			// bind OnReceivedDataOnServer
			TransceiveLargeDataComponent->OnReceivedDataDelegate.AddLambda(
			    [this, ChannelName](const auto& Data) {
				    OnReceivedDataOnServer(Data, ChannelName);
			    });

			// add component to TransceiveLargeDataComponents
			TransceiveLargeDataComponents.Add(TransceiveLargeDataComponent);
		}

		// call function that TransceiveLargeDataComponents is created
		OnRep_TransceiveLargeDataComponents();
	}
}

void ABPOnlineBeaconMulticastClient::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// replicate TransceiveLargeDataComponents
	DOREPLIFETIME(ABPOnlineBeaconMulticastClient, TransceiveLargeDataComponents);
}

void ABPOnlineBeaconMulticastClient::OnReceivedData(const TArray<uint8>& Data,
                                                    const FName& ChannelName) {
	// get broadcast target beacon multicast client
	const auto& BroadcastTargetBeaconMulticastClient =
	    GetBroadcastTargetBeaconMulticastClient();

	// broadcast to the broadcast target beacon multicast client
	BroadcastTargetBeaconMulticastClient->OnReceivedDataDynamicDelegate.Broadcast(
	    Data, ChannelName);
}

void ABPOnlineBeaconMulticastClient::OnReceivedDataOnServer(
    const TArray<uint8>& Data, const FName& ChannelName) {
	// check I'm on server
	check(ENetMode::NM_Client != GetNetMode());

	// get all beacon multicast clients
	const auto& BeaconMulticastClients = GetBeaconMulticastClients();

	// for each beacon multicast clients
	for (const auto& BeaconMulticastClient : BeaconMulticastClients) {
		// If it's not the client who sent the data
		if (BeaconMulticastClient != this) {
			// get channel index
			const auto& ChannelIndex = ChannelNames.Find(ChannelName);

			// assert channel exists
			check(INDEX_NONE != ChannelIndex);

			// send data to its owning client
			BeaconMulticastClient->TransceiveLargeDataComponents[ChannelIndex]
			    ->SendData(Data, ETransceiveLargeDataDirection::Client);
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

void ABPOnlineBeaconMulticastClient::OnRep_TransceiveLargeDataComponents() {
	// get number of channels
	const auto& ChannelNum = ChannelNames.Num();

	for (auto i = decltype(ChannelNum){0}; i < ChannelNum; ++i) {
		// get TransceiveLargeDataComponent
		const auto& TransceiveLargeDataComponent = TransceiveLargeDataComponents[i];

		// when the array has been replicated but the component has not yet been
		// replicated
		if (nullptr == TransceiveLargeDataComponent) {
			// ends with nothing.
			break;
		}

		// get ChannelName
		const auto& ChannelName = ChannelNames[i];

		// if TransceiveLargeDataComponent received data
		TransceiveLargeDataComponent->OnReceivedDataDelegate.AddLambda(
		    [this, ChannelName](const auto& Data) {
			    // call OnlineBeaconMulticastClient's OnReceivedData
			    OnReceivedData(Data, ChannelName);
		    });
	}
}

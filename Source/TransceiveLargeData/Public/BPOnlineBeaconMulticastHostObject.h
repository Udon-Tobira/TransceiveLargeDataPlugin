// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BPOnlineBeaconMulticastClient.h"
#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"

#include "BPOnlineBeaconMulticastHostObject.generated.h"

// delegate called after a client is spawned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnBeaconMulticastClientSpawnedDynamicDelegate,
    ABPOnlineBeaconMulticastClient*, SpawnedClient);

/**
 * ABPOnlineBeaconMulticastHostObject class available from blueprint.
 * It should exist only on the server side.
 * Spawn OnlineBeaconClients and manage them.
 * It also allows a beacon client to send data to all other beacon clients.
 */
UCLASS(BlueprintType, Blueprintable, transient, config = Engine, notplaceable)
class TRANSCEIVELARGEDATA_API ABPOnlineBeaconMulticastHostObject
    : public AOnlineBeaconHostObject {
	GENERATED_BODY()

public:
	//~=============================================================================
	// Accessors for beacon host object and beacon host.

	// The class of OnlineBeaconMulticastClient.
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<ABPOnlineBeaconMulticastClient> OnlineBeaconMulticastClientClass =
	    ABPOnlineBeaconMulticastClient::StaticClass();

	UFUNCTION(BlueprintCallable)
	TArray<ABPOnlineBeaconMulticastClient*> GetClientActors() const;

	UFUNCTION(BlueprintCallable)
	ABPOnlineBeaconMulticastClient* GetServerOnlyClientActor() const;

	/**
	 * spawn beacon client.
	 * @return   spawned new beacon client
	 */
	virtual ABPOnlineBeaconMulticastClient*
	    SpawnBeaconActor(UNetConnection* ClientConnection) override;

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Beacon Spawned"))
	FOnBeaconMulticastClientSpawnedDynamicDelegate OnBeaconSpawnedDynamicDelegate;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	TObjectPtr<ABPOnlineBeaconMulticastClient> ServerOnlyClientActor;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BPOnlineBeaconMulticastHostObject.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineBeaconHost.h"

#include "BeaconMulticastGameModeBase.generated.h"

/**
 * A game mode base extension class that adds the ability to spawn multicast
 * beacon.
 */
UCLASS(config = Game, notplaceable, BlueprintType, Blueprintable, Transient,
       hideCategories = (Info, Rendering, MovementReplication, Replication,
                         Actor),
       MinimalAPI)
class ABeaconMulticastGameModeBase: public AGameModeBase {
	GENERATED_BODY()

public:
	//~=============================================================================
	// Accessors for beacon host object and beacon host.

	// OnlineBeaconMulticastHostObject controls
	// OnlineBeaconHost and OnlineBeaconClient.
	UPROPERTY(Transient)
	TObjectPtr<ABPOnlineBeaconMulticastHostObject>
	    OnlineBeaconMulticastHostObject;

	UFUNCTION(BlueprintCallable)
	ABPOnlineBeaconMulticastHostObject*
	    GetOnlineBeaconMulticastHostObject() const;

public:
	// initialize this game mode object
	virtual void PreInitializeComponents() override;

public:
	// OnlineBeaconHost listens on server and wait for connecting from
	// OnlineBeaconClient.
	TObjectPtr<AOnlineBeaconHost> OnlineBeaconHost;
};

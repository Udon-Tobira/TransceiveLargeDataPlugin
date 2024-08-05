// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BPOnlineBeaconClient.h"
#include "CoreMinimal.h"
#include "TransceiveLargeDataComponent.h"

#include "BPOnlineBeaconMulticastClient.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, transient, config = Engine, notplaceable)
class TRANSCEIVELARGEDATA_API ABPOnlineBeaconMulticastClient
    : public ABPOnlineBeaconClient {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Multicast(const TArray<uint8>& Data);

public:
	UPROPERTY(BlueprintAssignable)
	FEventReceivedDataDelegate EventReceivedDataDelegate;

public:
	ABPOnlineBeaconMulticastClient();

	//~ Begin AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(
	    TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

private:
	UFUNCTION()
	void OnReceivedData(const TArray<uint8>& Data);
	UFUNCTION()
	void OnReceivedDataOnServer(const TArray<uint8>& Data);
	TArray<ABPOnlineBeaconMulticastClient*> GetBeaconMulticastClients() const;
	class ABPOnlineBeaconMulticastHostObject*
	    GetBeaconMulticastHostObject() const;
	const ABPOnlineBeaconMulticastClient*
	    GetBroadcastTargetBeaconMulticastClient() const;
	ABPOnlineBeaconMulticastClient* GetBroadcastTargetBeaconMulticastClient();

private:
	UPROPERTY(ReplicatedUsing = OnRep_TransceiveLargeDataComponent)
	UTransceiveLargeDataComponent* TransceiveLargeDataComponent;

	UFUNCTION()
	void OnRep_TransceiveLargeDataComponent();
};

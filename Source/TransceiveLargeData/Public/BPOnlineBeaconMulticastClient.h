// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BPOnlineBeaconClient.h"
#include "CoreMinimal.h"
#include "TransceiveLargeDataComponent.h"

#include "BPOnlineBeaconMulticastClient.generated.h"

// delegate for blueprint called when a data is received
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnOnlineBeaconMulticastClientReceivedDataDynamicDelegate,
    const TArray<uint8>&, Data, const FName&, ChannelName);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, transient, config = Engine, notplaceable,
       Abstract)
class TRANSCEIVELARGEDATA_API ABPOnlineBeaconMulticastClient
    : public ABPOnlineBeaconClient {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Multicast(const TArray<uint8>& Data, const FName& ChannelName);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere,
	          meta = (TitleProperty = "Channel Name"), Category = "Network")
	TArray<FName> ChannelNames;

public:
	UPROPERTY(BlueprintAssignable)
	FOnOnlineBeaconMulticastClientReceivedDataDynamicDelegate
	    OnReceivedDataDynamicDelegate;

public:
	ABPOnlineBeaconMulticastClient();

	//~ Begin AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(
	    TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor Interface

private:
	UFUNCTION()
	void OnReceivedData(const TArray<uint8>& Data, const FName& ChannelName);
	UFUNCTION()
	void OnReceivedDataOnServer(const TArray<uint8>& Data,
	                            const FName&         ChannelName);
	TArray<ABPOnlineBeaconMulticastClient*> GetBeaconMulticastClients() const;
	class ABPOnlineBeaconMulticastHostObject*
	    GetBeaconMulticastHostObject() const;
	const ABPOnlineBeaconMulticastClient*
	    GetBroadcastTargetBeaconMulticastClient() const;
	ABPOnlineBeaconMulticastClient* GetBroadcastTargetBeaconMulticastClient();

private:
	UPROPERTY(ReplicatedUsing = OnRep_TransceiveLargeDataComponents)
	TArray<UTransceiveLargeDataComponent*> TransceiveLargeDataComponents;

	UFUNCTION()
	void OnRep_TransceiveLargeDataComponents();
};

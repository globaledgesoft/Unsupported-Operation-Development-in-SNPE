//==============================================================================
//
// Copyright (c) 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once
#include "SnpeUdo/UdoImplDsp.h"

typedef struct OpFactory_t {
    SnpeUdo_String_t opType;
} OpFactory;

// struct for operation instances
typedef struct OpParams_t {
    SnpeUdo_OpFactory_t opFactory;
    uint32_t numInputParams;
    SnpeUdo_TensorParam_t* InputParams;
    uint32_t numOutputParams;
    SnpeUdo_TensorParam_t* outputParams;
    SnpeUdo_HexNNv2OpInfra_t opInfra;
} OpParams;


//declaration of pointers to functions
typedef SnpeUdo_ErrorType_t (*fptrQueryOperation)(SnpeUdo_String_t , uint32_t,
                                                  const SnpeUdo_Param_t* , uint32_t*,
                                                  SnpeUdo_QuantizationType_t** ,
                                                  SnpeUdo_HexNNTensorLayout_t**,
                                                  uint32_t*, SnpeUdo_QuantizationType_t**,
                                                  SnpeUdo_HexNNTensorLayout_t**);
typedef SnpeUdo_ErrorType_t (*fptrValidateOperation)(SnpeUdo_String_t, uint32_t,
                                                     const SnpeUdo_Param_t*);
typedef SnpeUdo_ErrorType_t (*fptrCreateOpFactory)(SnpeUdo_HexNNv2GlobalInfra_t*,
                                                   SnpeUdo_CoreType_t, void*,
                                                   SnpeUdo_String_t, uint32_t,
                                                   SnpeUdo_Param_t*, SnpeUdo_OpFactory_t*) ;
typedef SnpeUdo_ErrorType_t (*fptrExecuteOp)(SnpeUdo_HexNNv2GlobalInfra_t*, SnpeUdo_Operation_t,
                                             bool, const uint32_t, SnpeUdo_ExternalNotify_t) ;

typedef struct UdoDspShared_t
{
    fptrQueryOperation     QueryOp;
    fptrValidateOperation  ValidateOp;
    fptrCreateOpFactory    CreateOp;
    fptrExecuteOp          ExecuteOp;
} UdoDspShared;

typedef struct SnpeUdo_OpTypesList_t
{
    char *opType;
    UdoDspShared *opFunctionPtr;
    struct SnpeUdo_OpTypesList_t *next;
} SnpeUdo_OpTypesList;


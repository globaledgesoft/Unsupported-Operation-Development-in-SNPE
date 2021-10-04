//==============================================================================
//
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <vector>
#include "UdoOperation.hpp"
#include "SnpeUdo/UdoImplCpu.h"

namespace UdoUtil {

class UdoCpuOperation : public UdoOperation
{
public:
    UdoCpuOperation(SnpeUdo_TensorParam_t* inputs,
                    uint32_t numOfInputs,
                    SnpeUdo_TensorParam_t* outputs,
                    uint32_t numOfOutputs,
                    SnpeUdo_CpuInfrastructure_t* infrastructure,
                    uint32_t numOfStaticParams = 0,
                    SnpeUdo_Param_t* params =  nullptr);

  SnpeUdo_ErrorType_t
  snpeUdoExecute(bool blocking,
                 uint32_t id,
                 SnpeUdo_ExternalNotify_t notifyFunc) override = 0;

  SnpeUdo_ErrorType_t
  snpeUdoSetIo(SnpeUdo_TensorParam_t* inputs, SnpeUdo_TensorParam_t* outputs) override ;

  SnpeUdo_ErrorType_t snpeUdoProfile(uint32_t* executionTime) override ;

  ~UdoCpuOperation() override;

protected:
    SnpeUdo_CpuInfrastructure_t*  m_PerOpFactoryInfrastructure;
};
}


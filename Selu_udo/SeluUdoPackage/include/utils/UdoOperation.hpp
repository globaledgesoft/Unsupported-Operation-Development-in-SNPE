//==============================================================================
//
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include "SnpeUdo/UdoBase.h"
#include "SnpeUdo/UdoImpl.h"
#include <string>
#include <map>
#include <vector>

namespace UdoUtil {

class UdoOperation
{
public:
  virtual SnpeUdo_ErrorType_t
  snpeUdoExecute(bool blocking, uint32_t id, SnpeUdo_ExternalNotify_t notifyFunc) = 0;

  virtual SnpeUdo_ErrorType_t
  snpeUdoSetIo(SnpeUdo_TensorParam_t* inputs, SnpeUdo_TensorParam_t* outputs) = 0;

  virtual SnpeUdo_ErrorType_t snpeUdoProfile(uint32_t* executionTime) = 0;

  virtual ~UdoOperation() = default;

protected:
    std::vector<SnpeUdo_TensorParam_t*> m_Inputs;
    std::vector<SnpeUdo_TensorParam_t*> m_Outputs;
    uint32_t m_ExecutionTime;
    uint32_t m_NumOfStaticParams;
    std::map<std::string, SnpeUdo_Param_t*> m_Params;
};

}

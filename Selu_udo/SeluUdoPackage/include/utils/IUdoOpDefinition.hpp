//==============================================================================
//
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <memory>

#include "utils/UdoOperation.hpp"
#include "SnpeUdo/UdoBase.h"

namespace UdoUtil {
class IUdoOpDefinition
{
public:
  virtual std::unique_ptr<UdoOperation>
  createOp(void* perOpInfrastucture,
           uint32_t numOfInputs,
           SnpeUdo_TensorParam_t* inputs,
           uint32_t numOfOutputs,
           SnpeUdo_TensorParam_t* outputs,
           uint32_t numStaticParams,
           SnpeUdo_Param_t* params) = 0;

  virtual const char* getOperationType() const = 0;

  virtual ~IUdoOpDefinition() = default;
};
}

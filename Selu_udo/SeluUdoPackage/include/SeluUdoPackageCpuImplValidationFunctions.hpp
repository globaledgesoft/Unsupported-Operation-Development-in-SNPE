//==============================================================================
// Auto Generated Code for SeluUdoPackage
//==============================================================================

#pragma once

#include "utils/UdoUtil.hpp"

class SeluCpuValidationFunction : public UdoUtil::ImplValidationFunction {
public:

    SeluCpuValidationFunction()
            : ImplValidationFunction() {}

    SnpeUdo_ErrorType_t
    validateOperation(SnpeUdo_OpDefinition_t* def) override;
};

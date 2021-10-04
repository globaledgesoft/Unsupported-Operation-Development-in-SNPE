//==============================================================================
// Auto Generated Code for SeluUdoPackage
//==============================================================================

#include "SnpeUdo/UdoBase.h"
#include "SeluUdoPackageCpuImplValidationFunctions.hpp"
#include <string.h>

using namespace UdoUtil;

SnpeUdo_ErrorType_t
SeluCpuValidationFunction::validateOperation(SnpeUdo_OpDefinition_t* def) {
    /**
     * add code here
     */

    SnpeUdo_ErrorType_t status = SNPE_UDO_NO_ERROR;

    if (def == nullptr)
    {
        return SNPE_UDO_INVALID_ARGUMENT;
    }

    if (strcmp(def->operationType, "Selu"))
        return SNPE_UDO_WRONG_OPERATION;

    if (def->numOfStaticParams != 0)
        return SNPE_UDO_WRONG_OPERATION;


    if (def->numOfInputs != 1 || def->numOfOutputs != 1)
        return SNPE_UDO_WRONG_OPERATION;

    return SNPE_UDO_NO_ERROR;
}


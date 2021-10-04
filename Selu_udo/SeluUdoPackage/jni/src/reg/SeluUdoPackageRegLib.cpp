//==============================================================================
// Auto Generated Code for SeluUdoPackage
//==============================================================================
#include <iostream>
#include "utils/UdoUtil.hpp"
#include "SeluUdoPackageCpuImplValidationFunctions.hpp"

#ifndef UDO_LIB_NAME_CPU
#define UDO_LIB_NAME_CPU "libUdoSeluUdoPackageImplCpu.so"
#endif

extern "C"
{

std::unique_ptr<UdoUtil::UdoVersion> regLibraryVersion;
std::unique_ptr<UdoUtil::UdoRegLibrary> regLibraryInfo;

SnpeUdo_ErrorType_t
SnpeUdo_initRegLibrary(void)
{
    regLibraryInfo.reset(new UdoUtil::UdoRegLibrary("SeluUdoPackage",
                                                   SNPE_UDO_CORETYPE_CPU));

    regLibraryVersion.reset(new UdoUtil::UdoVersion);

    regLibraryVersion->setUdoVersion(1, 0, 0);


    /*
    ** User should fill in implementation library path here as needed.
    ** Note: The Implementation library path set here is relative, meaning each library to be used
    ** must be discoverable by the linker.
    */
    regLibraryInfo->addImplLib(UDO_LIB_NAME_CPU, SNPE_UDO_CORETYPE_CPU); //adding implementation libraries

    //==============================================================================
    // Auto Generated Code for Selu
    //==============================================================================
    auto SeluInfo = regLibraryInfo->addOperation("Selu", SNPE_UDO_CORETYPE_CPU, 1, 1);

    SeluInfo->addCoreInfo(SNPE_UDO_CORETYPE_CPU, SNPE_UDO_DATATYPE_FLOAT_16 | SNPE_UDO_DATATYPE_FLOAT_32); //adding core info



    //inputs and outputs need to be added as tensor params

    SeluInfo->addInputTensorInfo("Placeholder", {{SNPE_UDO_CORETYPE_CPU, SNPE_UDO_DATATYPE_FLOAT_32},}, SNPE_UDO_LAYOUT_NHWC, 0, 0); //adding tensor info

    //adding outputs
    SeluInfo->addOutputTensorInfo("Output", {{SNPE_UDO_CORETYPE_CPU, SNPE_UDO_DATATYPE_FLOAT_32},}, SNPE_UDO_LAYOUT_NHWC, 0); //adding tensor info

    // adding validation functions
    UDO_VALIDATE_RETURN_STATUS(regLibraryInfo->registerValidationFunction("Selu",
                                                SNPE_UDO_CORETYPE_CPU,
                                                std::unique_ptr<SeluCpuValidationFunction>
                                                    (new SeluCpuValidationFunction())))

    UDO_VALIDATE_RETURN_STATUS(regLibraryInfo->createRegInfoStruct())

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
SnpeUdo_getVersion(SnpeUdo_LibVersion_t** version) {

    UDO_VALIDATE_RETURN_STATUS(regLibraryVersion->getLibraryVersion(version))

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
SnpeUdo_getRegInfo(SnpeUdo_RegInfo_t** registrationInfo) {

    UDO_VALIDATE_RETURN_STATUS(regLibraryInfo->getLibraryRegInfo(registrationInfo))

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
SnpeUdo_terminateRegLibrary(void) {
    regLibraryInfo.reset();
    regLibraryVersion.reset();

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
SnpeUdo_validateOperation(SnpeUdo_OpDefinition_t* opDefinition) {
    UDO_VALIDATE_RETURN_STATUS(regLibraryInfo->snpeUdoValidateOperation(opDefinition))

    return SNPE_UDO_NO_ERROR;
}
}; //extern C

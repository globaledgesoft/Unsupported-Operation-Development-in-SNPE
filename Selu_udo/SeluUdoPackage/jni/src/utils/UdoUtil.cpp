//==============================================================================
//
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#include "utils/UdoUtil.hpp"

using namespace UdoUtil;

#define UDO_CHECK_POINTER(ptr) {if (ptr == nullptr) return false;}

// Utility function to initialize all fields of SnpeUdo_RegInfo_t structure
bool
initUdoRegInfoStruct(std::shared_ptr<SnpeUdo_RegInfo_t> &regInfoStruct,
                     const std::string &packageName,
                     SnpeUdo_Bitmask_t supportedCoreTypes,
                     uint32_t numOfImplementationLibValue,
                     SnpeUdo_LibraryInfo_t* implementationLibValue,
                     const std::string &operationsString,
                     uint32_t numOfOperationsValue,
                     SnpeUdo_OperationInfo_t* operationsInfoValue) {
    UDO_CHECK_POINTER(regInfoStruct);

    regInfoStruct->packageName = const_cast<char*>(packageName.c_str());
    regInfoStruct->supportedCoreTypes = supportedCoreTypes;
    regInfoStruct->numOfImplementationLib = numOfImplementationLibValue;
    regInfoStruct->implementationLib = implementationLibValue;
    regInfoStruct->operationsString = const_cast<char*>(operationsString.c_str());
    regInfoStruct->numOfOperations = numOfOperationsValue;
    regInfoStruct->operationsInfo = operationsInfoValue;

    return true;
}

SnpeUdo_ErrorType_t createTensorInfoStruct(const std::string& tensorName,
                                           std::vector<SnpeUdo_PerCoreDatatype_t> perCoreDatatype,
                                           SnpeUdo_TensorLayout_t layout,
                                           bool repeated,
                                           bool isStatic,
                                           std::unique_ptr<SnpeUdo_TensorInfo_t>& tensorInfo)
{
    if (tensorInfo == nullptr || perCoreDatatype.empty())
    {
        return SNPE_UDO_INVALID_ARGUMENT;
    }
    else
    {

        tensorInfo->tensorName = const_cast<char*>(tensorName.c_str());
        tensorInfo->perCoreDatatype = new SnpeUdo_PerCoreDatatype_t[perCoreDatatype.size()];
        for(std::size_t i=0; i<perCoreDatatype.size(); i++){
            tensorInfo->perCoreDatatype[i] = perCoreDatatype[i];
        }
        tensorInfo->layout = layout;
        tensorInfo->repeated = repeated;
        tensorInfo->isStatic = isStatic;
    }

    return SNPE_UDO_NO_ERROR;
}

bool
setVersionStruct(SnpeUdo_Version_t* versionStruct,
                 uint32_t majorValue,
                 uint32_t minorValue,
                 uint32_t patchValue) {
    UDO_CHECK_POINTER(versionStruct);

    versionStruct->major = majorValue;
    versionStruct->minor = minorValue;
    versionStruct->teeny = patchValue;
    return true;
}


UdoVersion::UdoVersion() {
    // Get API version from UdoBase header file
    setVersionStruct(&m_Version.apiVersion,
                     API_VERSION_MAJOR, API_VERSION_MINOR, API_VERSION_TEENY);
    // Set Library version to zero.
    setVersionStruct(&m_Version.libVersion, 0, 0, 0);
}

void
UdoVersion::setUdoVersion(uint32_t libMajor, uint32_t libMinor, uint32_t libPatch) {
    setVersionStruct(&m_Version.apiVersion,
                     API_VERSION_MAJOR, API_VERSION_MINOR, API_VERSION_TEENY);
    setVersionStruct(&m_Version.libVersion, libMajor, libMinor, libPatch);
}

SnpeUdo_ErrorType_t
UdoLibraryInfo::copyLibraryInfo(SnpeUdo_LibraryInfo_t* libraryInfoPtr) {
    UDO_VALIDATE_MSG(libraryInfoPtr == nullptr,
                 SNPE_UDO_INVALID_ARGUMENT,
                 "The provided library info struct is null")

    libraryInfoPtr->libraryName = const_cast<char*>(m_LibraryName.c_str());
    libraryInfoPtr->udoCoreType = m_CoreType;
    return SNPE_UDO_NO_ERROR;
}


UdoOperationInfo::UdoOperationInfo(std::string &&operationType,
                                   SnpeUdo_Bitmask_t supportedCores,
                                   int numOfInputs,
                                   int numOfOutputs) {
    m_OperationType = operationType;
    m_SupportedCoreTypes = supportedCores;
    m_NumOfInputs = numOfInputs;
    m_NumOfOutputs = numOfOutputs;
}

void
UdoOperationInfo::addScalarParam(const char* name,
                                 SnpeUdo_DataType_t type) {
    auto* localUdoParam = new SnpeUdo_Param_t;
    localUdoParam->paramName = const_cast<char*>(name);
    localUdoParam->scalarParam.dataType = type;
    localUdoParam->paramType = SNPE_UDO_PARAMTYPE_SCALAR;;
    m_Params.emplace_back(localUdoParam);
}

void
UdoOperationInfo::addTensorParam(const char* name,
                                 SnpeUdo_DataType_t type,
                                 SnpeUdo_TensorLayout_t layout) {
    auto* localUdoParam = new SnpeUdo_Param_t;
    localUdoParam->paramName = const_cast<char*>(name);
    localUdoParam->tensorParam.dataType = type;
    localUdoParam->paramType = SNPE_UDO_PARAMTYPE_TENSOR;
    localUdoParam->tensorParam.layout = layout;
    m_Params.emplace_back(localUdoParam);

}

void
UdoOperationInfo::addCoreInfo(SnpeUdo_CoreType_t coreType,
                              SnpeUdo_Bitmask_t calculationTypes) {
    m_CoreInfoVec.emplace_back(new SnpeUdo_OpCoreInfo_t{coreType, calculationTypes});
}

SnpeUdo_ErrorType_t
UdoOperationInfo::copyOperationParams(SnpeUdo_OperationInfo_t* operationInfoPtr) {
    UDO_VALIDATE_MSG(m_Params.empty(),
                 SNPE_UDO_WRONG_NUM_OF_PARAMS,
                 "No params provided")

    // Creating the array of core specific info and populating it
    operationInfoPtr->numOfStaticParams = m_Params.size();
    operationInfoPtr->staticParams = new SnpeUdo_Param_t[m_Params.size()];
    SnpeUdo_Param_t* localParamPtr = operationInfoPtr->staticParams;

    for (auto &paramInfo: m_Params)
    {
        *localParamPtr = *(paramInfo.release());
        localParamPtr++;
    }
    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoOperationInfo::copyOperationCoreInfo(SnpeUdo_OperationInfo_t* operationInfoPtr) {
    UDO_VALIDATE_MSG(m_CoreInfoVec.empty(),
                 SNPE_UDO_WRONG_CORE_TYPE,
                 "No core-types provided")

    // Creating the array of core specific info and populating it
    operationInfoPtr->opPerCoreInfo = new SnpeUdo_OpCoreInfo_t[m_CoreInfoVec.size()];
    SnpeUdo_OpCoreInfo_t* localCoreInfoPtr = operationInfoPtr->opPerCoreInfo;

    for (auto &coreInfo: m_CoreInfoVec)
    {
        // doing deep copy
        (*localCoreInfoPtr) = *(coreInfo.release());
        localCoreInfoPtr++;
    }
    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoOperationInfo::copyOperationInfo(SnpeUdo_OperationInfo_t* operationInfoPtr) {
    // populate operation info with basic information

    operationInfoPtr->operationType = const_cast<char*>(m_OperationType.c_str());
    operationInfoPtr->supportedByCores = m_SupportedCoreTypes;
    operationInfoPtr->numOfInputs = m_NumOfInputs;
    operationInfoPtr->numOfOutputs = m_NumOfOutputs;

    if(!m_Params.empty())
    {
        auto paramStatus = copyOperationParams(operationInfoPtr);
        UDO_VALIDATE_MSG(paramStatus != SNPE_UDO_NO_ERROR,
                     paramStatus,
                     "Attempt to copy params failed for operation " << operationInfoPtr->operationType)
    }

    auto tensorInfoStatus = copyOperationTensorInfos(operationInfoPtr);
    UDO_VALIDATE_MSG(tensorInfoStatus != SNPE_UDO_NO_ERROR,
                 tensorInfoStatus,
                 "Attempt to copy tensor infos failed for operation " << operationInfoPtr->operationType)

    auto coreStatus = copyOperationCoreInfo(operationInfoPtr);
    UDO_VALIDATE_MSG(coreStatus != SNPE_UDO_NO_ERROR,
                 coreStatus,
                 "Attempt to copy core types failed for operation " << operationInfoPtr->operationType)

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoOperationInfo::copyOperationTensorInfos(SnpeUdo_OperationInfo_t* operationInfoPtr)
{
    if (operationInfoPtr == nullptr)
    {
        return SNPE_UDO_INVALID_ARGUMENT;
    }

    if (m_InputInfos.empty() || m_NumOfInputs != (int) m_InputInfos.size())
    {
        return SNPE_UDO_WRONG_NUM_OF_INPUTS;
    }
    else
    {
        operationInfoPtr->inputInfos = new SnpeUdo_TensorInfo_t[m_InputInfos.size() + 1];
        SnpeUdo_TensorInfo_t* localInputTensorInfoPtr = operationInfoPtr->inputInfos;

        for (auto& inputInfo: m_InputInfos)
        {
            *localInputTensorInfoPtr = *inputInfo.release();
            localInputTensorInfoPtr++;

        }
    }
    if (m_OutputInfos.empty() || m_NumOfOutputs != (int) m_OutputInfos.size())
    {
        return SNPE_UDO_WRONG_NUM_OF_OUTPUTS;
    }
    else
    {
        operationInfoPtr->outputInfos = new SnpeUdo_TensorInfo_t[m_OutputInfos.size() + 1];
        SnpeUdo_TensorInfo_t* localOutputTensorInfoPtr = operationInfoPtr->outputInfos;

        for (auto& outputInfo: m_OutputInfos)
        {
            *localOutputTensorInfoPtr = *outputInfo.release();
            localOutputTensorInfoPtr++;

        }
    }
    return SNPE_UDO_NO_ERROR;
}

void UdoOperationInfo::addInputTensorInfo(const std::string& tensorName,
                                          std::vector<SnpeUdo_PerCoreDatatype_t>&& perCoreDatatype,
                                          SnpeUdo_TensorLayout_t layout, bool repeated, bool isStatic)
{
    std::unique_ptr<SnpeUdo_TensorInfo_t> tensorInfo;
    tensorInfo.reset(new SnpeUdo_TensorInfo_t());
    createTensorInfoStruct(tensorName, std::move(perCoreDatatype), layout, repeated, isStatic, tensorInfo);
    m_InputInfos.emplace_back(std::move(tensorInfo));
}

void UdoOperationInfo::addOutputTensorInfo(const std::string& tensorName,
                                           std::vector<SnpeUdo_PerCoreDatatype_t>&& perCoreDatatype,
                                           SnpeUdo_TensorLayout_t layout, bool repeated)
{
    std::unique_ptr<SnpeUdo_TensorInfo_t> tensorInfo;
    tensorInfo.reset(new SnpeUdo_TensorInfo_t());
    createTensorInfoStruct(tensorName, std::move(perCoreDatatype), layout, repeated, false, tensorInfo);
    m_OutputInfos.emplace_back(std::move(tensorInfo));
}

UdoRegLibrary::UdoRegLibrary(const std::string &packageName,
                             SnpeUdo_Bitmask_t supportedCoreTypes)
        : m_LibraryInfo(nullptr), m_SupportedCoreTypes(supportedCoreTypes) {
    m_RegInfo.reset(new SnpeUdo_RegInfo_t);
    m_PackageName = packageName;
    initUdoRegInfoStruct(m_RegInfo,
                         m_PackageName,
                         SNPE_UDO_CORETYPE_UNDEFINED,
                         0,
                         nullptr,
                         m_OperationsString,
                         0,
                         nullptr);
}

void
UdoRegLibrary::addImplLib(std::string &&libName, SnpeUdo_CoreType_t udoCoreType) {
    m_UdoImplLibs.emplace_back(new UdoLibraryInfo(std::move(libName), udoCoreType));
}

std::shared_ptr<UdoOperationInfo>
UdoRegLibrary::addOperation(std::string &&operationType,
                            SnpeUdo_Bitmask_t supportedCores,
                            int numOfInputs,
                            int numOfOutputs) {
    m_UdoOperations.emplace_back(new UdoOperationInfo(std::move(operationType),
                                                      supportedCores,
                                                      numOfInputs,
                                                      numOfOutputs));
    return m_UdoOperations.back();
}

SnpeUdo_ErrorType_t
UdoRegLibrary::createImplLibInfo() {
    UDO_VALIDATE_MSG(m_UdoImplLibs.empty(),
                 SNPE_UDO_UNKNOWN_ERROR,
                 "No implementation libraries present in package: " << m_PackageName)

    // m_libraryInfo marks the start of the array, need local pointer to travel...
    m_LibraryInfo = new SnpeUdo_LibraryInfo_t[m_UdoImplLibs.size()];
    SnpeUdo_LibraryInfo_t* localLiInfoPtr = m_LibraryInfo;

    for (auto &&implLibPtr: m_UdoImplLibs)
    {
        UDO_VALIDATE_RETURN_STATUS(implLibPtr->copyLibraryInfo(localLiInfoPtr))
        localLiInfoPtr++; // advance the pointer to next struct entry
    }

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoRegLibrary::createOperationInfo() {
    UDO_VALIDATE_MSG(m_UdoOperations.empty(),
                 SNPE_UDO_UNKNOWN_ERROR,
                 "No operations were found in package: " << m_PackageName)

    // allocate memory for array of m_OperationInfo structures
    std::ostringstream combinedOperationsString;
    m_OperationInfo = new SnpeUdo_OperationInfo_t[m_UdoOperations.size()];
    // m_OperationInfo marks the start of the array, need local pointer to travel...
    SnpeUdo_OperationInfo_t* localOperationPtr = m_OperationInfo;

    for (auto &operationVecPtr: m_UdoOperations)
    {
        operationVecPtr->copyOperationInfo(localOperationPtr);
        combinedOperationsString << localOperationPtr->operationType;
        combinedOperationsString << ' '; // add spaces between operations
        localOperationPtr++; // advance the pointer to next struct entry
    }
    m_OperationsString = combinedOperationsString.str();

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoRegLibrary::createRegInfoStruct() {
    UDO_VALIDATE_RETURN_STATUS(createImplLibInfo());
    UDO_VALIDATE_RETURN_STATUS(createOperationInfo());
    UDO_VALIDATE_MSG(!initUdoRegInfoStruct(m_RegInfo,                 // Registration info Struct
                                         m_PackageName,             // Package name
                                         m_SupportedCoreTypes,      // Supported Cores
                                         m_UdoImplLibs.size(),      // Number of Implementation libraries
                                         m_LibraryInfo,             // Pointer to array of Impl Libraries Info
                                         m_OperationsString,        // Support Operations string
                                         m_UdoOperations.size(),    // Number of operation info structs
                                         m_OperationInfo),         // Pointer to array of Operation,
                                         SNPE_UDO_UNKNOWN_ERROR, "Unknown failure while create the regInfo for package"<<m_PackageName)

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoRegLibrary::registerValidationFunction(const std::string &name,
                                          const SnpeUdo_CoreType_t &coreType,
                                          std::unique_ptr<ImplValidationFunction> &&validateFunction) {
    auto validationKey = std::make_pair(name, coreType);
    auto pos = m_ValidateFunctions.find(validationKey);

    UDO_VALIDATE_MSG(pos != m_ValidateFunctions.end(),
                 SNPE_UDO_INVALID_ARGUMENT,
                 "Validation for op: " << name << " with core-type: "<<coreType<<
                 " is already registered")


        m_ValidateFunctions.emplace(std::move(validationKey), std::move(validateFunction));
        return SNPE_UDO_NO_ERROR;
}

ImplValidationFunction*
UdoRegLibrary::resolveValidationFunction(const char* operationType,
                                         const SnpeUdo_CoreType_t &coreType) {
    auto pos = m_ValidateFunctions.find(std::make_pair(operationType, coreType));

    if (pos != m_ValidateFunctions.end())
    {
        return pos->second.get();
    }
    return nullptr;
}

SnpeUdo_ErrorType_t
UdoRegLibrary::snpeUdoValidateOperation(SnpeUdo_OpDefinition_t* opDefinition) {
    auto def = resolveValidationFunction(opDefinition->operationType, opDefinition->udoCoreType);

    UDO_VALIDATE_MSG(def == nullptr,
                 SNPE_UDO_WRONG_OPERATION,
                 "Could not retrieve validation function for op: " << opDefinition->operationType)

    return def->validateOperation(opDefinition);

}

SnpeUdo_ErrorType_t
UdoImplementationLib::createOpFactory(const char* operationType,
                                      void* perOpFactoryInfrastructure,
                                      uint32_t numOfStaticParams,
                                      SnpeUdo_Param_t* staticParams,
                                      SnpeUdo_OpFactory_t* opFactory) {
    auto* definition = resolveOperation(operationType);

     UDO_VALIDATE_MSG(definition == nullptr,
                 SNPE_UDO_WRONG_OPERATION,
                 "Could not retrieve operation definition for op: " << operationType)

    auto* factory = new _SnpeUdo_OpFactory_t();

    factory->definition = definition;
    factory->infrastructure = perOpFactoryInfrastructure;
    factory->staticParams = staticParams;
    factory->numOfStaticParams = numOfStaticParams;

    *opFactory = factory;

    return SNPE_UDO_NO_ERROR;
}

UdoImplementationLib::UdoImplementationLib() {
    m_ImplInfo.udoCoreType = SNPE_UDO_CORETYPE_UNDEFINED;
    m_ImplInfo.packageName = nullptr;
    m_ImplInfo.operationsString = nullptr;
    m_ImplInfo.numOfOperations = 0;
}

UdoImplementationLib::UdoImplementationLib(SnpeUdo_CoreType_t coreType,
                                           const std::string &packageName)

        : UdoImplementationLib() {
    configure(coreType, packageName);
}

void
UdoImplementationLib::configure(SnpeUdo_CoreType_t coreType,
                                const std::string &packageName) {
    m_PackageName = packageName;
    m_CoreType = coreType;
    m_ImplInfo.udoCoreType = m_CoreType;
    m_ImplInfo.packageName = const_cast<char*>(m_PackageName.c_str());
}

SnpeUdo_ErrorType_t
UdoImplementationLib::registerOpDefinition(const std::string &name,
                                           std::unique_ptr<IUdoOpDefinition> &&definition) {
    auto pos = m_Definitions.find(name);

    UDO_VALIDATE_MSG(pos != m_Definitions.end(),
                 SNPE_UDO_WRONG_OPERATION,
                 "Operation definition for op: " << name <<
                 " is already registered in package: "<<m_PackageName)

    m_Definitions.emplace(name, std::move(definition));
    m_OperationTypes.emplace_back(name);

    return SNPE_UDO_NO_ERROR;
}

IUdoOpDefinition*
UdoImplementationLib::resolveOperation(const char* operationType) {
    auto pos = m_Definitions.find(std::string(operationType));

    if (pos != m_Definitions.end())
    {
        return pos->second.get();
    }
    return nullptr;
}

SnpeUdo_ErrorType_t
UdoImplementationLib::getImplementationInfo(SnpeUdo_ImpInfo_t** info) {

    UDO_VALIDATE_MSG(info == nullptr,
                     SNPE_UDO_INVALID_ARGUMENT,
                     "The implementation info provided to the function is null")

        UDO_VALIDATE_MSG(m_ImplInfo.numOfOperations > m_Definitions.size(),
                 SNPE_UDO_WRONG_OPERATION,
                 "The number of operations is more than the registered operation definitions."
                 "Expected at most: " << m_Definitions.size() <<
                 " instead got: "<<m_ImplInfo.numOfOperations)


        std::ostringstream strm;
        auto iter = m_Definitions.begin();

        while (iter != m_Definitions.end())
        {
            strm << iter->second->getOperationType();
            iter++;
            if (iter != m_Definitions.end())
            {
                strm << ' ';
            }
        }
        m_OperationsString = strm.str();
        m_ImplInfo.operationsString = const_cast<char*>(m_OperationsString.c_str());
        m_ImplInfo.numOfOperations = m_Definitions.size();

    *info = &m_ImplInfo;
    return SNPE_UDO_NO_ERROR;
}

std::unique_ptr<UdoImplementationLib> libInstance;
std::mutex libInstanceMutex;

SnpeUdo_ErrorType_t
UdoImplementationLib::getVersion(SnpeUdo_LibVersion_t** version) {
    UDO_VALIDATE_MSG(version == nullptr,
                 SNPE_UDO_INVALID_ARGUMENT,
                 "The provided version info struct is null")

    *version = &m_Version;

    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoImplementationLib::setVersion(uint32_t libMajor, uint32_t libMinor, uint32_t libPatch) {
    setVersionStruct(&m_Version.apiVersion,
                     API_VERSION_MAJOR, API_VERSION_MINOR, API_VERSION_TEENY);
    setVersionStruct(&m_Version.libVersion, libMajor, libMinor, libPatch);
    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoUtil::setImplementation(SnpeUdo_CoreType_t coreType, const std::string &packageName)
{
    std::lock_guard<std::mutex> lock(libInstanceMutex);
    UDO_VALIDATE_MSG(libInstance != nullptr,
                     SNPE_UDO_INVALID_ARGUMENT,
                     "Implementation library instance has already been set")

    libInstance.reset(new UdoImplementationLib(coreType, packageName));

    return SNPE_UDO_NO_ERROR;
}

UdoImplementationLib&
UdoUtil::getImplementation()
{
    bool libInstanceIsNull = libInstance == nullptr;
    UDO_ASSERT_MSG(libInstanceIsNull,
                   SNPE_UDO_UNKNOWN_ERROR,
                   "No Implementation set in function")

    return *libInstance; // #TODO: this will segfault if libInstance is null,
    // workaround needed as throwing exceptions is not allowed
}

SnpeUdo_ErrorType_t
UdoUtil::deleteImplementationInstance()
{
    auto implLibptr = libInstance.release();
    delete implLibptr;

    return SNPE_UDO_NO_ERROR;
}


//==============================================================================
//
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "SnpeUdo/UdoReg.h"
#include "SnpeUdo/UdoBase.h"
#include "UdoOperation.hpp"
#include "IUdoOpDefinition.hpp"
#include "utils/UdoMacros.hpp"

extern "C"
{

struct _SnpeUdo_Operation_t
{
  std::unique_ptr<UdoUtil::UdoOperation> operation;
};

struct _SnpeUdo_OpFactory_t
{
  UdoUtil::IUdoOpDefinition* definition;
  void* infrastructure;
  SnpeUdo_Param_t* staticParams;
  uint32_t numOfStaticParams;
};

} // extern "C"

namespace UdoUtil {

/**
* @brief A helper class to create the UDO version struct
* This class automatically fills the API version from the UDO Base header,
* and allows a developer to specify a library version.
*
*/
class UdoVersion
{
public:
  SnpeUdo_LibVersion_t m_Version;

  /**
   * \brief Base constructor. initializes the API version based on headers.
   * Library version is set to 0.0.0.0
   */
  UdoVersion();

  /**
   * \brief A function to set the library version, in case the default constructor was used
   * API version is not touched
   * Library version is set based on values passed
   */
  void setUdoVersion(uint32_t libMajor, uint32_t libMinor, uint32_t libPatch);

  /**
   * \brief A function to return the Library version struct
   * Note that it returns a pointer to the m_version class member.
   */
  SnpeUdo_ErrorType_t
  getLibraryVersion(SnpeUdo_LibVersion_t** version)
  {
    *version =  &m_Version;
    return SNPE_UDO_NO_ERROR;
  }
};

/**
 * @brief A helper class to create the Library information struct in the
 * registration object.
 * This class provides easy creation of library info using C++ constructor.
 * Once created, the class provides a copy method to serialize the info.
 */
class UdoLibraryInfo
{
public:
  std::string m_LibraryName;

  UdoLibraryInfo(std::string&& libraryName,
         SnpeUdo_CoreType_t udoCoreType)
    : m_LibraryName(libraryName), m_CoreType(udoCoreType) {}

  SnpeUdo_ErrorType_t copyLibraryInfo(SnpeUdo_LibraryInfo_t* libraryInfoPtr);

  SnpeUdo_CoreType_t getCoreType() { return m_CoreType; }

private:
  SnpeUdo_CoreType_t m_CoreType;

};

/**
* @brief A helper class to create Operation information struct
* This class provides the following :
* - Creation basic Operation info by using C++ constructor
* - Provide "add" methods for params and Core-specific info
* - Provide a serialization function ("copy") which allocates a C struct
*   and copies all information to it
*/
class UdoOperationInfo
{
public:
  std::string m_OperationType;

  UdoOperationInfo(std::string&& operationType,
           SnpeUdo_Bitmask_t supportedCores,
           int numOfInputs,
           int numOfOutputs);

  // signature for adding scalar param for registration info, no dataValue
  void
  addScalarParam(const char* name, SnpeUdo_DataType_t type);

  // signature for adding tensor param
  void
  addTensorParam(const char* name,
         SnpeUdo_DataType_t type,
         SnpeUdo_TensorLayout_t layout);


  /**
  * \brief Populates a tensor info object within an Operation Info. The intended use is to
  * describe inputs
  * @param tensorName: The name of the input tensor
  * @param perCoreDatatype: A structure containing a tensor's datatype and the associated core-type
  * @param layout : A struct indicating how data will be arranged in the tensor
  * @param repeated : A boolean indicating that there will be more than one tensor with the above
  * params.
  */
  void
  addInputTensorInfo(const std::string& tensorName,
                     std::vector<SnpeUdo_PerCoreDatatype_t>&& perCoreDatatype,
                     SnpeUdo_TensorLayout_t layout,
                     bool repeated, bool isStatic);

  /**
  * \brief Populates a tensor info object within an Operation Info. The intended use is to
  * describe inputs
  * @param tensorName: The name of the input tensor
  * @param perCoreDatatype: A structure containing a tensor's datatype and the associated core-type
  * @param layout : A struct indicating how data will be arranged in the tensor
  * @param repeated : A boolean indicating that there will be more than one tensor with the above
  * params.
  */
  void
  addOutputTensorInfo(const std::string& tensorName,
                      std::vector<SnpeUdo_PerCoreDatatype_t>&& perCoreDatatype,
                      SnpeUdo_TensorLayout_t layout,
                      bool repeated);

  void
  addCoreInfo(SnpeUdo_CoreType_t coreType,
          SnpeUdo_Bitmask_t calculationTypes);
  /**
   * \brief Populates a SnpeUdo_OperationInfo_t using its private member fields
   * and methods.
   * @param operationInfoPtr ptr where the operation info will be copied into
   * @return
   */
  SnpeUdo_ErrorType_t
  copyOperationInfo(SnpeUdo_OperationInfo_t* operationInfoPtr);

private:
  int m_NumOfInputs;
  int m_NumOfOutputs;
  SnpeUdo_Bitmask_t m_SupportedCoreTypes;
  std::vector<std::unique_ptr<SnpeUdo_Param_t>> m_Params;
  std::vector<std::unique_ptr<SnpeUdo_TensorInfo_t>> m_InputInfos;
  std::vector<std::unique_ptr<SnpeUdo_TensorInfo_t>> m_OutputInfos;
  std::vector<std::unique_ptr<SnpeUdo_OpCoreInfo_t>> m_CoreInfoVec;
  SnpeUdo_ErrorType_t
  copyOperationParams(SnpeUdo_OperationInfo_t* operationInfoPtr);
  SnpeUdo_ErrorType_t
  copyOperationCoreInfo(SnpeUdo_OperationInfo_t* operationInfoPtr);
  SnpeUdo_ErrorType_t
  copyOperationTensorInfos(SnpeUdo_OperationInfo_t* operationInfoPtr);
};

using validationFunction =
std::function<SnpeUdo_ErrorType_t(SnpeUdo_OpDefinition_t* opDefinition)>;
class ImplValidationFunction
{
public:
    ImplValidationFunction() = default;

    virtual SnpeUdo_ErrorType_t
    validateOperation(SnpeUdo_OpDefinition_t* def) = 0;

    virtual ~ImplValidationFunction() = default;

};

class UdoRegLibrary
{
public:
  std::string m_PackageName; // Pointer to hold the package name string
  SnpeUdo_LibraryInfo_t* m_LibraryInfo; // pointer to hold array of library info
  SnpeUdo_OperationInfo_t* m_OperationInfo; // pointer to hold array of operation info

  UdoRegLibrary(const std::string &packageName, SnpeUdo_Bitmask_t supportedCoreTypes);

  /**
   * \brief This function creates a new implementation library info
   * object, and adds it to internal vector
   */
  void
  addImplLib(std::string&& libName, SnpeUdo_CoreType_t udoCoreType);

  /**
   * \brief
   * adds an object of UdoImplementationLib to an internal map
   * of implementation libraries
   *
   */
  SnpeUdo_ErrorType_t registerValidationFunction(const std::string &name,
                                  const SnpeUdo_CoreType_t& m_coreType,
                                  std::unique_ptr<ImplValidationFunction>&& validateFunction);

  /**
   * \brief
   * registers a validation function with a registration library instance, which will be wrapped
   * by SnpeUdoValidateOperation
   */
  SnpeUdo_ErrorType_t
  snpeUdoValidateOperation(SnpeUdo_OpDefinition_t* opDefinition);

  /**
   * \brief This function creates a new Operation info
   * object, and adds it to internal vector.
   * The function returns the info object so that the user can add parameters and
   * per-core information
   */
  std::shared_ptr<UdoOperationInfo>
  addOperation(std::string&& operationType,
               SnpeUdo_Bitmask_t supportedCores,
               int numOfInputs,
               int numOfOutputs);

  /**
   * \brief This function creates the registration struct and populate it
   * with the libraries information, operations information etc.
   * The function should be called after all libraries and operations were
   * added to the registration object using "addImplLib", "addOperation" etc.
   */
  SnpeUdo_ErrorType_t
  createRegInfoStruct();

  /**
   * \brief
   *  Populates the registration library info pointer passed with the private regInfo struct
   *  for a UdoRegLibrary instance. This function should be called after CreateRegInfoStruct.
   */
  SnpeUdo_ErrorType_t
  getLibraryRegInfo(SnpeUdo_RegInfo_t** regInfo) {
    *regInfo = m_RegInfo.get();
    return SNPE_UDO_NO_ERROR;
  }

private:
  SnpeUdo_ErrorType_t createImplLibInfo();
  SnpeUdo_ErrorType_t createOperationInfo();
  SnpeUdo_Bitmask_t m_SupportedCoreTypes;
  std::string m_OperationsString;
  std::shared_ptr<SnpeUdo_RegInfo_t> m_RegInfo; // struct pointer to hold reglibinfo
  std::vector<std::unique_ptr<UdoLibraryInfo>> m_UdoImplLibs;
  std::vector<std::shared_ptr<UdoOperationInfo>> m_UdoOperations;
  ImplValidationFunction* resolveValidationFunction(const char* operationType, const SnpeUdo_CoreType_t& coreType);
  std::map<std::pair<std::string, SnpeUdo_CoreType_t>, std::unique_ptr<ImplValidationFunction>> m_ValidateFunctions;
};

/**\brief
 *  Helper class that wraps creation of an implementation info struct,
 *  creation of an opFactory instance and registering of a SnpeUdoOpDefinition.
 *  The class utilizes an internal map to store all the operations and opDefinitions in
 *  in an implementation library, and provides a simple entry point to SnpeUdo methods that
 *  are called when an implementation library is opened.
 */
class UdoImplementationLib
{
public:
  UdoImplementationLib();

  std::vector<std::string> m_OperationTypes;

  UdoImplementationLib(SnpeUdo_CoreType_t coreType, const std::string &packageName);

  /**
   *\brief Sets the core type and package name for this library
   * when the default constructor is used.
   */
  void
  configure(SnpeUdo_CoreType_t coreType, const std::string &packageName);

  /**
   * \brief Registers an op definition in with an Implementation Lib instance, using its name and
   * a unique ptr to an IUdoOpDefinition object.
   *
   * @param name A constant string reference
   *
   * @param definition An object of IUdoOpDefinition which is registered in a std::map member of an
   *        ImplLib instance.
   *
   *@return SNPE_UDO_WRONG_OPERATION if the definition was already registered
   */
  SnpeUdo_ErrorType_t
  registerOpDefinition(const std::string &name, std::unique_ptr<IUdoOpDefinition>&& definition);

  /**
   * @brief A function to create an operation factory.
   *        The function receives the operation type, and an array of static parameters,
   *        and returns operation factory handler
   *
   * @param[in] operationType A string containing Operation type. for example "MY_CONV"
   *
   * @param[in] perOpFactoryInfrastructure The infrastructure needed for all ops created by this factory
   *
   * @param[in] numOfStaticParams The number of static parameters.
   *
   * @param[in] staticParams Array of static parameters
   *
   * @param[in,out] OpFactory Handler to Operation Factory, to be used when creating operations
   *
   * @return Error Code
   */
  SnpeUdo_ErrorType_t createOpFactory(const char* operationType,
                                      void* perOpFactoryInfrastructure,
                                      uint32_t numOfStaticParams,
                                      SnpeUdo_Param_t* staticParams,
                                      SnpeUdo_OpFactory_t* opFactory);

  SnpeUdo_ErrorType_t
  getVersion(SnpeUdo_LibVersion_t** version);

  SnpeUdo_ErrorType_t
  setVersion(uint32_t libMajor, uint32_t libMinor, uint32_t libPatch);

  /**
   * @brief A function to query the info on the UDO implementation library.
   *        The function returns a UdoImpInfo struct, which contains information on the operations
   *        which are part of this library
   *
   * @param[in, out] implementationInfo A pointer to struct which contains information on the UDO
   *        set
   *
   * @return error code
   *
   */
  SnpeUdo_ErrorType_t
  getImplementationInfo(SnpeUdo_ImpInfo_t** info);

private:
  IUdoOpDefinition* resolveOperation(const char* operationType);
  std::map<std::string, std::unique_ptr<IUdoOpDefinition>> m_Definitions;
  SnpeUdo_ImpInfo_t m_ImplInfo;
  std::string m_PackageName;
  SnpeUdo_LibVersion_t m_Version;
  SnpeUdo_CoreType_t m_CoreType;
  std::string m_OperationsString;
};
 UdoImplementationLib&
 getImplementation();

  SnpeUdo_ErrorType_t
  setImplementation(SnpeUdo_CoreType_t coreType, const std::string &packageName);

  SnpeUdo_ErrorType_t
  deleteImplementationInstance();
}


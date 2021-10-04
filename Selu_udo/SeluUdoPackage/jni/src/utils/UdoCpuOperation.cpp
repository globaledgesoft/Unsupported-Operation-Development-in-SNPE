//==============================================================================
//
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

//==============================================================================

#include <utils/UdoCpuOperation.hpp>
#include "utils/UdoMacros.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <numeric>

using namespace UdoUtil;

SnpeUdo_ErrorType_t
copyTensorParam(const SnpeUdo_TensorParam_t &srcParam, SnpeUdo_TensorParam_t &destParam, bool copyData=false) {
    destParam.dataType = srcParam.dataType;
    destParam.layout = srcParam.layout;
    destParam.quantizeParams = srcParam.quantizeParams;
    destParam.tensorRank = srcParam.tensorRank;


    UDO_VALIDATE_MSG(srcParam.maxDimensions == nullptr || srcParam.currDimensions == nullptr,
                     SNPE_UDO_INVALID_ARGUMENT,
                     "Provided dimensions are null")

    auto dimSize = srcParam.tensorRank;
    auto dimByteSize = dimSize * sizeof(uint32_t);
    destParam.currDimensions = new uint32_t[srcParam.tensorRank];
    destParam.maxDimensions = new uint32_t[srcParam.tensorRank];

    std::memcpy(destParam.maxDimensions,
                srcParam.maxDimensions,
                dimByteSize);
    std::memcpy(destParam.currDimensions,
                srcParam.currDimensions,
                dimByteSize);

    const std::map<SnpeUdo_DataType_t, size_t> dataTypeToSize = {
            {SNPE_UDO_DATATYPE_INT_8, 1},
            {SNPE_UDO_DATATYPE_INT_16, 2},
            {SNPE_UDO_DATATYPE_INT_32, 4},
            {SNPE_UDO_DATATYPE_UINT_8, 1},
            {SNPE_UDO_DATATYPE_UINT_16, 2},
            {SNPE_UDO_DATATYPE_UINT_32, 4},
            {SNPE_UDO_DATATYPE_FLOAT_16, 2},
            {SNPE_UDO_DATATYPE_FLOAT_32, 4},
            {SNPE_UDO_DATATYPE_FIXED_4, 1},
            {SNPE_UDO_DATATYPE_FIXED_8, 1},
            {SNPE_UDO_DATATYPE_FIXED_16, 2},
            {SNPE_UDO_DATATYPE_FIXED_32, 4}
    };

    if (copyData)
    {
        auto dataDimSize = std::accumulate(srcParam.currDimensions,
                                           srcParam.currDimensions + dimSize,
                                           dataTypeToSize.find(srcParam.dataType)->second,
                                           std::multiplies<size_t>());

        // logic here is based on data being received as a uint8_t from Param Span
        destParam.tensorData = (uint8_t*)malloc(dataDimSize);
        std::memcpy(destParam.tensorData,
                    reinterpret_cast<uint8_t*>(srcParam.tensorData),
                    dataDimSize);
    }else {
        destParam.tensorData = srcParam.tensorData; // this is done for inputs and outputs, which cannot be copied here
    }

    return SNPE_UDO_NO_ERROR;
}

UdoCpuOperation::UdoCpuOperation(SnpeUdo_TensorParam_t* inputs,
                                 uint32_t numOfInputs,
                                 SnpeUdo_TensorParam_t* outputs,
                                 uint32_t numOfOutputs,
                                 SnpeUdo_CpuInfrastructure_t* infrastructure,
                                 uint32_t numOfStaticParams,
                                 SnpeUdo_Param_t* params) {

    UDO_ASSERT_MSG(inputs == nullptr || numOfInputs <= 0,
                   SNPE_UDO_WRONG_NUM_OF_INPUTS,
                   "Input provided to function is null")

    UDO_ASSERT_MSG(outputs == nullptr || numOfOutputs <= 0,
                   SNPE_UDO_WRONG_NUM_OF_OUTPUTS,
                   "Output provided to function is null")

    UDO_ASSERT_MSG(infrastructure == nullptr,
                   SNPE_UDO_INVALID_ARGUMENT,
                   "Infrastructure provided to function is null")

    m_Inputs.resize(numOfInputs);
    m_Outputs.resize(numOfOutputs);

    for (uint32_t idx = 0; idx < m_Inputs.size(); idx++)
    {
        m_Inputs[idx] =  new SnpeUdo_TensorParam_t();
        copyTensorParam(inputs[idx], *m_Inputs[idx]);
    }

    for (std::size_t idx = 0; idx < m_Outputs.size(); idx++)
    {
        m_Outputs[idx] =  new SnpeUdo_TensorParam_t();
        copyTensorParam(outputs[idx], *m_Outputs[idx]);
    }

    m_PerOpFactoryInfrastructure = infrastructure;
    m_NumOfStaticParams = numOfStaticParams;
    if (numOfStaticParams > 0 && params != nullptr)
    {
        for (std::size_t idx = 0; idx < numOfStaticParams; idx++)
        {
            std::string paramName = const_cast<char*>(params[idx].paramName);
            m_Params[paramName] =  new SnpeUdo_Param_t();
            m_Params[paramName]->paramType = params[idx].paramType;
            m_Params[paramName]->paramName = params[idx].paramName;
            switch (params[idx].paramType)
            {
                case SNPE_UDO_PARAMTYPE_TENSOR:
                    copyTensorParam(params[idx].tensorParam, m_Params[paramName]->tensorParam, true);
                    break;
                case SNPE_UDO_PARAMTYPE_STRING:
                    m_Params[paramName]->stringParam = params[idx].stringParam;
                    break;
                case SNPE_UDO_PARAMTYPE_SCALAR:
                    m_Params[paramName]->scalarParam = params[idx].scalarParam;
                    break;
                default:
                {
                    std::cerr << "ERROR: Function: "
                              << __FUNCTION__ << " Unknown param type for param: " << paramName
                              << std::endl;
                }
            }
        }
    }
}

SnpeUdo_ErrorType_t
UdoCpuOperation::snpeUdoProfile(uint32_t* executionTime) {
    UDO_VALIDATE_MSG(executionTime == nullptr,
                     SNPE_UDO_INVALID_ARGUMENT,
                     "Execution time provided is not valid")

    *executionTime = m_ExecutionTime;
    return SNPE_UDO_NO_ERROR;
}

SnpeUdo_ErrorType_t
UdoCpuOperation::snpeUdoSetIo(SnpeUdo_TensorParam_t* inputs, SnpeUdo_TensorParam_t* outputs) {
    //TODO: This function is never used, we should consider removing it
    UDO_VALIDATE_MSG(inputs == nullptr || m_Inputs.empty(),
                     SNPE_UDO_WRONG_NUM_OF_INPUTS,
                     "Input provided to function is null")

    UDO_VALIDATE_MSG(outputs == nullptr || m_Outputs.empty(),
                     SNPE_UDO_WRONG_NUM_OF_INPUTS,
                     "Output provided to function is null")


    for (std::size_t idx = 0; idx < m_Inputs.size(); idx++)
    {
        m_Inputs[idx] = &inputs[idx];
    }

    for (std::size_t idx = 0; idx < m_Outputs.size(); idx++)
    {
        m_Outputs[idx] = &outputs[idx];
    }

    return SNPE_UDO_NO_ERROR;
}

void freeUdoTensorParam(SnpeUdo_TensorParam_t &tensorParam, bool deleteData = false) {
    if (!tensorParam.maxDimensions || !tensorParam.currDimensions || !tensorParam.tensorData)
    {
        return;
    }
    if (deleteData)
    {
        free(reinterpret_cast<uint8_t*>(tensorParam.tensorData));
    }
    delete tensorParam.maxDimensions;
    delete tensorParam.currDimensions;
}

UdoCpuOperation::~UdoCpuOperation() {
    std::for_each(m_Inputs.begin(), m_Inputs.end(), [](SnpeUdo_TensorParam_t*& param) -> void
    { freeUdoTensorParam(*param);  delete param;});
    std::for_each(m_Outputs.begin(), m_Outputs.end(), [](SnpeUdo_TensorParam_t*&param) -> void
    { freeUdoTensorParam(*param); delete param;});

    for (auto it = m_Params.begin(); it != m_Params.end(); it++)
    {
        freeUdoTensorParam(it->second->tensorParam, true);
        delete it->second;
    }
}
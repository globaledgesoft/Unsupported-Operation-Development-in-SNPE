//==============================================================================
// Auto Generated Code for SeluUdoPackage
//==============================================================================

#pragma once
#include "utils/UdoCpuOperation.hpp"
#include "utils/IUdoOpDefinition.hpp"

class SeluOp : public UdoUtil::UdoCpuOperation
{
public:
    SeluOp(SnpeUdo_TensorParam_t* inputs, uint32_t numOfInputs, SnpeUdo_TensorParam_t* outputs,
                   uint32_t numOfOutputs, SnpeUdo_CpuInfrastructure_t* infrastructure, uint32_t numOfStaticParams,
                   SnpeUdo_Param_t* params)
           : UdoCpuOperation(inputs, numOfInputs, outputs, numOfOutputs, infrastructure, numOfStaticParams,  params) {}

    SnpeUdo_ErrorType_t
    snpeUdoExecute(bool blocking, uint32_t ID, SnpeUdo_ExternalNotify_t notifyFunc) override;
};

class SeluOpDef : public UdoUtil::IUdoOpDefinition
{
public:
    SeluOpDef() = delete;
    SeluOpDef(const char *operationType, uint32_t numOfInputs,uint32_t numOfOutputs)
    :m_OperationType(operationType)
    ,m_NumOfInputs(numOfInputs)
    ,m_NumOfOutputs(numOfOutputs)
    {}

    std::unique_ptr<UdoUtil::UdoOperation>
    createOp(void *perOpInfrastucture,
             uint32_t numOfInputs,
             SnpeUdo_TensorParam_t *inputs,
             uint32_t numOfOutputs,
             SnpeUdo_TensorParam_t *outputs,
             uint32_t numOfStaticParams,
             SnpeUdo_Param_t* params) override;

    const char *getOperationType() const override { return m_OperationType; }

private:
    const char *m_OperationType;
    uint32_t m_NumOfInputs;
    uint32_t m_NumOfOutputs;
};

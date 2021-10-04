//==============================================================================
// Auto Generated Code for SeluUdoPackage
//==============================================================================

#include "SeluImplLibCpu.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <string>

std::unique_ptr<UdoUtil::UdoOperation>
SeluOpDef::createOp(void *perOpInfrastructure,
                    uint32_t numOfInputs,
                    SnpeUdo_TensorParam_t *inputs,
                    uint32_t numOfOutputs,
                    SnpeUdo_TensorParam_t *outputs,
                    uint32_t numOfStaticParams,
                    SnpeUdo_Param_t* params)
{
    /**
      * add code here
      * This method should
      * 1.) Validate Static Params
      * 2.) Create Op
      * 3.) Convert arguments and use appropriate constructor in Selu.hpp
      */

    return std::unique_ptr<UdoUtil::UdoCpuOperation>
          (new SeluOp(inputs, numOfInputs, outputs, numOfOutputs,
                         static_cast<SnpeUdo_CpuInfrastructure_t*>(perOpInfrastructure),
                         numOfStaticParams, params));
   // return nullptr;
}

SnpeUdo_ErrorType_t
SeluOp::snpeUdoExecute(bool blocking, const uint32_t ID, SnpeUdo_ExternalNotify_t notifyFunc)
{
    /**
      * add code here
      */

    auto startTime = std::chrono::high_resolution_clock::now();
    if(!blocking) { return SNPE_UDO_UNSUPPORTED_FEATURE; }
    if(m_Inputs.empty() || m_Outputs.empty() || !m_PerOpFactoryInfrastructure) { return SNPE_UDO_INVALID_ARGUMENT; }

    const uint32_t rank = m_Outputs[0]->tensorRank;
    const size_t depth = m_Outputs[0]->currDimensions[rank - 1];

    uint32_t tensorLength = 1;

    for(uint32_t j = 0; j < rank; ++j)
    {
       tensorLength *= (uint32_t)(m_Outputs[0]->currDimensions[j]);

    }
    const size_t numPixels = tensorLength/depth;

    for( size_t pix = 0; pix < numPixels; ++pix )
   {
      const float* in = (float*)m_PerOpFactoryInfrastructure->getData(m_Inputs[0]->tensorData)+pix*depth;
      float* out = (float*)m_PerOpFactoryInfrastructure->getData(m_Outputs[0]->tensorData)+pix*depth;

      
      // find the max element for max subtraction
      float maxElt = std::numeric_limits<float>::lowest();
      for( size_t i = 0; i < depth; ++i )
      {
         maxElt = std::max( maxElt, in[i] );
      }
      
      // compute exponentiations
      float expSum = 0.0;
      const float scale = 1.05070098;
      const float alpha = 1.67326324;
      for( size_t i = 0; i < depth; ++i )
      {
       // const float ei = expf( in[i] - maxElt );
        const float x = in[i] - maxElt;
        
        if (x>0) {
          out[i] = scale * x;
        }

        if (x<0) {
          out[i] = scale * alpha * (exp(x) - 1);
        }

      }
      /*
      // normalize
      for( size_t i = 0; i < depth; ++i )
      {
         out[i] = out[i] / expSum;
      }
      */
   }

    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    uint32_t elapsedTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    m_ExecutionTime = elapsedTimeUs;
    return SNPE_UDO_NO_ERROR;
}

//==============================================================================
//
// Copyright (c) 2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <string>
#include <sstream>
#include <iostream>


#define UDO_ERROR_MSG(status, msg)                              \
   {                                                            \
      std::ostringstream oss ## __LINE__;                       \
      oss ## __LINE__ << msg;                                   \
      std::cerr<<"ERROR: "<<oss ##__LINE__.str()                \
      <<"; UDO error code = "<<(int)status<< "; File: "<<__FILE__<<       \
      "; Function: "<<__FUNCTION__<<" Line : "<<__LINE__<<std::endl; \
   }
#define UDO_ASSERT_MSG(condition, status, msg)                  \
   if((condition)) {                                              \
      UDO_ERROR_MSG(status, msg)                                \
   }
#define UDO_VALIDATE_MSG(condition, status, msg)                \
   if((condition)) {                                           \
      UDO_ERROR_MSG(status, msg)                                \
      return status;                                            \
   }
#define UDO_VALIDATE_RETURN_STATUS(status)                       \
   if(status != SNPE_UDO_NO_ERROR) {                             \
      return status;                                             \
   }

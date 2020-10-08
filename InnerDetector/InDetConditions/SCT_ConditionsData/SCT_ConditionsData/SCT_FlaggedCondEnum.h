// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedCondEnum.h
 * @brief header file for enum for SCT_Clusterization and SCT_FlaggedConditionTool.
 * @author Susumu Oda
 * @date 20/08/2020
 **/

#ifndef SCT_FLAGGEDCONDENUM_H
#define SCT_FLAGGEDCONDENUM_H

#include <vector>

namespace SCT_FlaggedCondEnum {
  /**
   * @enum ErrorType
   * @brief SCT flagged condition error type enums used in SCT_Clusterization,
   * SCT_FlaggedConditionTool.
   +*/
  enum ErrorType {
    ExceedMaxFiredStrips, // SCT_Clusterization: Exceeds max fired strips
    ExceedMaxRDOs, // SCT_TrgClusterization: Exceeds max RDOs
    NUM_ERROR_TYPES
  };
  static const std::vector<std::string> ErrorTypeDescription = {
    "SCT_Clusterization: Exceeds max fired strips", // ExceedMaxFiredStrips
    "SCT_TrgClusterization: Exceeds max RDOs" // ExceedMaxRDOs
  };
}

#endif // SCT_FLAGGEDCONDENUM_H

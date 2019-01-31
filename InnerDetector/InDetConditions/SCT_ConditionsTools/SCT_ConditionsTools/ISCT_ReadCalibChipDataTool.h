/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file ISCT_ReadCalibDataTool.h Interface file for SCT_ReadCalibDataTool.
 */

// Multiple inclusion protection
#ifndef ISCT_READ_CALIB_CHIP_DATA_TOOL
#define ISCT_READ_CALIB_CHIP_DATA_TOOL

//STL includes
#include <vector>

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

class ISCT_ReadCalibChipDataTool: virtual public ISCT_ConditionsTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~ISCT_ReadCalibChipDataTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ReadCalibChipDataTool, 1, 0);

  // Methods to return calibration data 
  //PJ change to wafer hash id?!
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const =0; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) const =0; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const =0; //!<Get NoiseOccupancy data wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) const =0; //!<Get NoiseOccupancy data wafer

};

//---------------------------------------------------------------------- 
#endif // ISCT_READ_CALIB_CHIP_DATA_TOOL

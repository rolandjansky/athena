/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file ISCT_ReadCalibDataTool.h Interface file for SCT_ReadCalibDataTool.
 */

// Multiple inclusion protection
#ifndef ISCT_READ_CALIB_DATA_TOOL
#define ISCT_READ_CALIB_DATA_TOOL

//STL includes
#include <vector>
#include <list>
#include <string>

// Include SCT calibration data map objects
//needed for typedef
#include "SCT_ConditionsData/SCT_CalibDefectData.h"

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

class Identifier;
class IdentifierHash;
class StatusCode;

class ISCT_ReadCalibDataTool: virtual public ISCT_ConditionsTool {

 public:
  
  typedef struct { 
    std::vector<std::string> scan;
    std::vector<std::string> defect;
    std::vector<float> value;
  } CalibDefectType;

  //----------Public Member Functions----------//
  // Structors
  virtual ~ISCT_ReadCalibDataTool() = default; //!< Destructor
  
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ReadCalibDataTool, 1, 0);

  // Methods to return calibration defect type and summary
  virtual ISCT_ReadCalibDataTool::CalibDefectType defectType(const Identifier& stripId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const = 0; //!<Return summary of defect type and values for a strip
  virtual ISCT_ReadCalibDataTool::CalibDefectType defectType(const Identifier& stripId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const = 0; //!<Return summary of defect type and values for a strip
  virtual SCT_CalibDefectData::CalibModuleDefects defectsSummary(const Identifier& moduleId, const std::string& scan, const EventContext& ctx) const = 0; //!<Returns module summary of defect
  virtual SCT_CalibDefectData::CalibModuleDefects defectsSummary(const Identifier& moduleId, const std::string& scan) const = 0; //!<Returns module summary of defect
  virtual std::list<Identifier> defectList(const std::string& defect, const EventContext& ctx) const = 0; //!<Returns module summary of defect
  virtual std::list<Identifier> defectList(const std::string& defect) const = 0; //!<Returns module summary of defect
  
};

//---------------------------------------------------------------------- 
#endif // ISCT_READ_CALIB_DATA_TOOL

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy)=0;
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier&, InDetConditions::Hierarchy)=0;
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& /*hashId*/)=0;
};

//---------------------------------------------------------------------- 
#endif // ISCT_READ_CALIB_DATA_TOOL

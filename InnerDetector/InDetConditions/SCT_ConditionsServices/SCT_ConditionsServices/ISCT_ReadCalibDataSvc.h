/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ISCT_ReadCalibDataTool.h Interface file for SCT_ReadCalibDataSvc.
 */

// Multiple inclusion protection
#ifndef ISCT_READ_CALIB_DATA_SVC
#define ISCT_READ_CALIB_DATA_SVC

//STL includes
#include <vector>
#include <list>
#include <string>

// Include SCT calibration data map objects
//needed for typedef
#include "SCT_ConditionsData/SCT_CalibDefectData.h"

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

class ISCT_ReadCalibDataSvc: virtual public ISCT_ConditionsSvc {

 public:
  
  typedef struct { 
    std::vector<std::string> scan;
    std::vector<std::string> defect;
    std::vector<float> value;
  } CalibDefectType;

  //----------Public Member Functions----------//
  // Structors
  virtual ~ISCT_ReadCalibDataSvc() {} //!< Destructor
  
  // Retrive interface ID
  static const InterfaceID& interfaceID();
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy)=0;
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier&, InDetConditions::Hierarchy)=0;
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& /*hashId*/)=0;
  // Fill the data structures
  StatusCode fillData()=0;
  // Fill the data structures from a Callback 
  StatusCode fillData(int&, std::list<std::string>&)=0;
  // Report whether the map was filled
  bool filled() const =0;
  // Report whether the service can fill its data during the initialize phase
  virtual bool canFillDuringInitialize()=0;
};

inline const InterfaceID& ISCT_ReadCalibDataSvc::interfaceID() {
  static const InterfaceID IID_SCT_ReadCalibDataSvc{"SCT_ReadCalibDataSvc", 1, 0};
  return IID_SCT_ReadCalibDataSvc;
}

//---------------------------------------------------------------------- 
#endif // ISCT_READ_CALIB_DATA_SVC

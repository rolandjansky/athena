/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ISCT_ReadCalibDataTool.h Interface file for SCT_ReadCalibDataSvc.
 */

// Multiple inclusion protection
#ifndef ISCT_READ_CALIB_CHIP_DATA_SVC
#define ISCT_READ_CALIB_CHIP_DATA_SVC

//STL includes
#include <vector>
#include <list>
#include <string>

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class ISCT_ReadCalibChipDataSvc: virtual public ISCT_ConditionsSvc {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~ISCT_ReadCalibChipDataSvc() {} //!< Destructor
  
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

  // Methods to return calibration data 
  //PJ change to wafer hash id?!
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype)=0; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype)=0; //!<Get NoiseOccupancy data wafer

};

inline const InterfaceID & ISCT_ReadCalibChipDataSvc::interfaceID() {
  static const InterfaceID IID_SCT_ReadCalibChipDataSvc{"SCT_ReadCalibChipDataSvc", 1, 0};
  return IID_SCT_ReadCalibChipDataSvc;
}

//---------------------------------------------------------------------- 
#endif // ISCT_READ_CALIB_CHIP_DATA_SVC

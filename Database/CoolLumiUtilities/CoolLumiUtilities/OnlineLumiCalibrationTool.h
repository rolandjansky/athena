/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class OnlineLumiCalibrationTool
   @brief Tool to provide access to LVL1 OnlineLumiCalibrationContent COOL folder
   @brief Implementing IOnlineLumiCalibrationTool interface
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_ONLINELUMICALIBRATIONTOOL_H
#define COOLLUMIUTILITIES_ONLINELUMICALIBRATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "CoolLumiUtilities/IOnlineLumiCalibrationTool.h"
#include "CoolLumiUtilities/OnlineLumiCalibrator.h"

#include <string>
#include <vector>
#include <map>

class OnlineLumiCalibrationTool : public AthAlgTool, virtual public IOnlineLumiCalibrationTool {

 public:

  OnlineLumiCalibrationTool(const std::string& type,
		const std::string& name,
		const IInterface* parent);

  // ---------------- user interface -------------------

  // Tool functions
  StatusCode initialize();

  // Returns -1 if channel doesn't exist
  float getMuToLumi(unsigned int channel) const;

  // Returns false on error
  bool calibrateLumi(unsigned int channel, float raw, float& lumi) const;
  bool calibrateMu(unsigned int channel, float raw, float& mu) const;

  // Callback function called when cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

 private:

  // COOL folder
  std::string m_calibrationFolderName;

  // Data
  std::map<unsigned int, OnlineLumiCalibrator> m_cali;

};

#endif

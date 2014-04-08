/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LuminosityTool
   @brief Tool to provide access to luminosity information from COOL
   @brief Implementing ILuminosityTool interface
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LuminosityTool_H
#define LUMIBLOCKCOMPS_LuminosityTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "LumiBlockComps/ILuminosityTool.h"
#include "LumiCalibrator.h"

#include <string>
#include <vector>
#include <map>


// Usually this will be called to retrieve the mu value directly from EventInfo
// For data which doesn't have this (or to write it there in the first place)
// this can be configured to read the mu value from COOL instead.
//
class LuminosityTool: public AthAlgTool, virtual public ILuminosityTool {

 public:

  LuminosityTool(const std::string& type,
		 const std::string& name,
		 const IInterface* parent);

  // ---------------- user interface -------------------

  // Instantaneous luminosity information from /TRIGGER/LUMI/LBLESTONL or LBLESTOFL
  float lbAverageLuminosity() const;  // In ub-1 s-1 = 10^30 cm-2 s-1
  float lbAverageInteractionsPerCrossing() const;  
  uint32_t lbAverageValid() const; // Validity flag from COOL

  // Luminosity block time (in seconds) from /TRIGGER/LUMI/LBLB
  // Multiply by lbAverageLuminosity to get integrated luminosity in the current lumi block
  double lbDuration() const; // Luminosity block time (in seconds)

  // Instantaneous luminosity per BCID
  // Returned as vector which can be indexed by BCID number
  // Non-zero values only filled for BCIDs in the physics bunch group, or found to have collisions by the bunch currents
  const std::vector<float>& lbLuminosityPerBCIDVector();    // In ub-1 s-1 = 10^30 cm-2 s-1

  // Return current BCID luminosity based on EventInfo bcid number
  float lbLuminosityPerBCID();

  // Return specific BCID luminosity
  float lbLuminosityPerBCID(unsigned int bcid);

  // Conversion factor from OLC.  lumiPerBCID/muToLumi = interactionsPerCrossingPerBCID
  float muToLumi() const;

  // Functions
  StatusCode initialize();
  StatusCode finalize();

  // Callback function called when any cached data is invalidated, 
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

 private:

  // Callback functions
  virtual StatusCode updateAvgLumi(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode updateCalibrations(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode updateFillparams(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode updateBunchgroup(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode updateLBLB(IOVSVC_CALLBACK_ARGS);

  // Per-BCID Luminosity update function
  void recalculatePerBCIDLumi();

  // Data from LBLESTOFL
  float m_LBAvInstLumi; // in ub-1 s-1 => 10^30 cm-2 s-1
  float m_LBAvEvtsPerBX;
  uint32_t m_Valid;

  // Lumi block length from /TRIGGER/LUMI/LBLB
  double m_LBDuration; // in seconds

  // Luminosity per beam crossing (for a specific BCID)
  std::vector<float> m_LBInstLumi;

  // Conversion factor from calibrations
  float m_MuToLumi;

  // DB parameters
  std::string m_lumiFolderName; // Default to /TRIGGER/OFLLUMI/LBLESTOFL 
  unsigned long m_lumiChannel;  // Default to 0 - preferred

  // Folders for per-BCID calculation
  std::string m_calibrationFolderName;
  std::string m_fillparamsFolderName;
  std::string m_bunchlumisFolderName;
  std::string m_bunchgroupFolderName;
  std::string m_lblbFolderName;

  // Flag to indicate that cached data has changed
  bool m_recalcPerBCIDLumi;

  // Flag to indicate preferred channel
  unsigned int m_preferredChannel;

  // Fillparams data
  unsigned int m_luminousBunches;
  std::vector<unsigned int> m_luminousBunchesVec;

  // Bunchgroup data
  unsigned int m_bgBunches;
  std::vector<unsigned int> m_bgBunchesVec;

  // Calibration data
  std::map<unsigned int, LumiCalibrator> m_cali;

};


#endif

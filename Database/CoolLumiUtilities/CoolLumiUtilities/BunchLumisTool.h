/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class BunchLumisTool
   @brief Tool to provide access to BUNCHLUMIS COOL folder
   @brief Implementing IBunchLumisTool interface
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_BUNCHLUMISTOOL_H
#define COOLLUMIUTILITIES_BUNCHLUMISTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ToolHandle.h"
#include "CoolLumiUtilities/IBunchLumisTool.h"
#include "CoolLumiUtilities/IFillParamsTool.h"

#include <string>
#include <vector>

class BunchLumisTool : public AthAlgTool, virtual public IBunchLumisTool {

 public:

  BunchLumisTool(const std::string& type,
		const std::string& name,
		const IInterface* parent);

  // ---------------- user interface -------------------

  // Set luminosity channel to access
  void setChannel(unsigned int channel);

  // Raw luminosity per BCID (must access each event to ensure updates)
  const std::vector<float>& rawLuminosity();
  float rawLuminosity(unsigned int bcid);
  float averageRawLuminosity();

  // Callback function called when cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

  // Tool functions
  StatusCode initialize();

 private:

  // COOL folder
  std::string m_bunchLumisFolderName;

  // Reload raw lumi array
  bool m_reloadRawLumi;

  // Channel
  unsigned int m_channel;

  // Data
  float m_avgRawLumi;
  std::vector<float> m_rawLumi;

  // FillParamsTool (needed to unpack certain schemes)
  ToolHandle<IFillParamsTool> m_fillParamsTool;

  // Function to recalculate lumi
  void reloadRawLumi();

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class FillParamsTool
   @brief Tool to provide access to FILLPARAMS COOL folder
   @brief Implementing IFillParamsTool interface
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_FILLPARAMSTOOL_H
#define COOLLUMIUTILITIES_FILLPARAMSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "CoolLumiUtilities/IFillParamsTool.h"

#include <string>
#include <vector>

class FillParamsTool : public AthAlgTool, virtual public IFillParamsTool {

 public:

  FillParamsTool(const std::string& type,
		const std::string& name,
		const IInterface* parent);

  // ---------------- user interface -------------------

  // Number of filled bunches (length of vectors below)
  unsigned int nBeam1Bunches() const;
  unsigned int nBeam2Bunches() const;
  unsigned int nLuminousBunches() const;

  // Arrays with BCID numbers for filled bunches
  const std::vector<unsigned int>& beam1Bunches() const;
  const std::vector<unsigned int>& beam2Bunches() const;
  const std::vector<unsigned int>& luminousBunches() const;

  // Callback function called when cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

  // Tool functions
  StatusCode initialize();

  // Interface
 private:

  // COOL folder
  std::string m_fillParamsFolderName;

  // Data
  std::vector<unsigned int> m_beam1Bunches;
  std::vector<unsigned int> m_beam2Bunches;
  std::vector<unsigned int> m_luminousBunches;

};

#endif

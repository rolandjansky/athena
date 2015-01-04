/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class BunchGroupTool
   @brief Tool to provide access to LVL1 BunchGroupContent COOL folder
   @brief Implementing IBunchGroupTool interface
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_BUNCHGROUPTOOL_H
#define COOLLUMIUTILITIES_BUNCHGROUPTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "CoolLumiUtilities/IBunchGroupTool.h"

#include <string>
#include <vector>

class BunchGroupTool : public AthAlgTool, virtual public IBunchGroupTool {

 public:

  BunchGroupTool(const std::string& type,
		const std::string& name,
		const IInterface* parent);

  // ---------------- user interface -------------------

  // Number of filled bunches (length of vectors below)
  unsigned int nBunchGroup0() const;
  unsigned int nBunchGroup1() const;
  unsigned int nBunchGroup2() const;
  unsigned int nBunchGroup3() const;
  unsigned int nBunchGroup4() const;
  unsigned int nBunchGroup5() const;
  unsigned int nBunchGroup6() const;
  unsigned int nBunchGroup7() const;

  // Arrays with BCID numbers for filled bunches
  const std::vector<unsigned int>& bunchGroup0() const;
  const std::vector<unsigned int>& bunchGroup1() const;
  const std::vector<unsigned int>& bunchGroup2() const;
  const std::vector<unsigned int>& bunchGroup3() const;
  const std::vector<unsigned int>& bunchGroup4() const;
  const std::vector<unsigned int>& bunchGroup5() const;
  const std::vector<unsigned int>& bunchGroup6() const;
  const std::vector<unsigned int>& bunchGroup7() const;

  // Callback function called when cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS);

  // Tool functions
  StatusCode initialize();

  // Interface
 private:

  // COOL folder
  std::string m_bunchGroupFolderName;

  // Data
  std::vector<unsigned int> m_bg0;
  std::vector<unsigned int> m_bg1;
  std::vector<unsigned int> m_bg2;
  std::vector<unsigned int> m_bg3;
  std::vector<unsigned int> m_bg4;
  std::vector<unsigned int> m_bg5;
  std::vector<unsigned int> m_bg6;
  std::vector<unsigned int> m_bg7;
};

#endif

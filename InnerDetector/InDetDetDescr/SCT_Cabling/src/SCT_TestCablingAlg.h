// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_TestCablingAlg_h
#define SCT_TestCablingAlg_h
/**
 * @file SCT_TestCablingAlg.h
 * Header file for Algorithm to test cabling
 * @author Shaun Roe
 * @date 20 October, 2008
 **/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

//package includes
#include "SCT_Cabling/ISCT_CablingTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>

class ISvcLocator;
class StatusCode;
class SCT_ID;

/**
 * @class SCT_TestCablingAlg
 * SCT_TestCablingAlg exercises the routines of the SCT cabling tool
 **/
class SCT_TestCablingAlg:public AthReentrantAlgorithm {
 public:
  SCT_TestCablingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_TestCablingAlg() = default;
  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode execute(const EventContext& ctx) const override; //!< Gaudi executer
  virtual StatusCode finalize() override; //!< Gaudi finaliser
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
  const SCT_ID* m_idHelper{nullptr}; //!< helper for offlineId/hash conversions
  std::string coordString(const Identifier& offlineId) const;
  UnsignedIntegerProperty m_POSIXtime{this, "POSIXtime", 0, "POSIX time for database"};

};
#endif // SCT_TestCablingAlg_h

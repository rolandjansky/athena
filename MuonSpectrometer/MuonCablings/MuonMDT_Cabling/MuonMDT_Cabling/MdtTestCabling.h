/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTTESTCABLING_H
#define MUONMDT_CABLING_MDTTESTCABLING_H

/***********************************************
 *
 * @class MdtTestCabling
 *
 * Testing algorithm for the Muon MDT mapping class
 *
 * @author Stefano Rosati Stefano.Rosati@roma1.infn.it
 *
 ***********************************************/

#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// old cabling service
//#include "MDTcabling/IMDTcablingSvc.h"


class MdtTestCabling : public AthAlgorithm {

 public:

  MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdtTestCabling();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

 private:

  ServiceHandle<MuonMDT_CablingSvc> m_cablingSvc;

  const MdtIdHelper* m_mdtIdHelper;

  // test initialize function
  bool initTestMap();

  bool testMapTiming();

  bool testMap();

  // the chrono service
  ServiceHandle<IChronoStatSvc> m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;

};


#endif     // MUONMDT_CABLING_MDTTESTCABLING_H

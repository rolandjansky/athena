/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"


class MdtTestCabling : public AthAlgorithm {

 public:

  MdtTestCabling(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdtTestCabling()=default;

  virtual StatusCode initialize();
  virtual StatusCode execute();

 private:

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

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

  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

};


#endif     // MUONMDT_CABLING_MDTTESTCABLING_H

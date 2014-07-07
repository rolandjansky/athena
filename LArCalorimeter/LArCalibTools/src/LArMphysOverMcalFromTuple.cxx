/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArMphysOverMcalFromTuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArElecCalib/ILArMphysOverMcal.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>


LArMphysOverMcalFromTuple::LArMphysOverMcalFromTuple (const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{  
  declareProperty("FileNames", m_root_file_names);
  declareProperty("StoreKey", m_store_key="FROMTUPLE");
}

LArMphysOverMcalFromTuple::~LArMphysOverMcalFromTuple() 
{}

StatusCode LArMphysOverMcalFromTuple::initialize() 
{
  return StatusCode::SUCCESS ;
}


StatusCode LArMphysOverMcalFromTuple::stop()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "... in finalize()" << endreq ;
  
  // Get access to the Detector Store
  StoreGateSvc* detStore;  
  StatusCode sc = service("DetectorStore",detStore);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }

  // get LArOnlineID helper
  const LArOnlineID* onlineHelper;
  sc = detStore->retrieve(onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID" << endreq;
    return sc;
  }

  TChain* outfit = new TChain("outfit");
  for ( std::vector<std::string>::const_iterator it = m_root_file_names.begin();
	it != m_root_file_names.end(); it++ ) {
    outfit->Add(it->c_str());
  }

  // variable names as in the Ntuple 
  Int_t           BarAC; // 0 = barrel C   1 = barrel A
  Int_t           FT; // feedthrough 0 - 31
  Int_t           Channel; // 0 - 14
  Int_t           Slot; // 1 -14
  Double_t        PhysOCal; // Mphys/Mcal
  int gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  
  outfit->SetBranchAddress("Channel", &Channel);
  outfit->SetBranchAddress("Slot", &Slot);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("BarAC", &BarAC);
  outfit->SetBranchAddress("PhysOCal", &PhysOCal);


  // Create new LArMphysOverMcalComplete
  LArMphysOverMcalComplete* container = new LArMphysOverMcalComplete();
//  container->setGroupingType(LArConditionsContainerBase::FeedThroughGrouping);
  container->setGroupingType(LArConditionsContainerBase::ExtendedFTGrouping);
  container->initialize();

  


  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Int_t i = 0; i < nentries; i++ )
  {
    outfit->GetEntry(i);
    HWIdentifier id = onlineHelper->channel_Id(0, BarAC, FT, Slot, Channel);
    container->set(id,gain,PhysOCal);
  }


  // store 
  sc=detStore->record(container,m_store_key);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot record LArMphysOverMcalComplete to StoreGate with key = " << m_store_key << endreq;
    return sc;
  } 
  // Symlink LArMphysOverMcalComplete to ILArMphysOverMcal for further use
  ILArMphysOverMcal *larMphysOverMcal = NULL;
  sc = detStore->symLink(container,larMphysOverMcal);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not symlink ILArMphysOverMcal with LArMphysOverMcalComplete." << endreq;
    return StatusCode::FAILURE;
  } 


 log << MSG::INFO << "LArMphysOverMcalyFromTuple finalized!" << endreq;  
  
  return StatusCode::SUCCESS;
}

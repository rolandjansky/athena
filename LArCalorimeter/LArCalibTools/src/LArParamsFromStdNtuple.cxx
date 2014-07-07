/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArParamsFromStdNtuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>


LArParamsFromStdNtuple::LArParamsFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{  
  declareProperty("FileNames", m_root_file_names);
  declareProperty("NtupleName", m_ntuple_name="PARAMS");
  declareProperty("StoreKey_Cali", m_store_key_cali="FromStdNtuple");
  declareProperty("StoreKey_Det", m_store_key_det="FromStdNtuple");
  declareProperty("GroupingType", m_groupingType="FeedThrough");
}

LArParamsFromStdNtuple::~LArParamsFromStdNtuple() 
{}

StatusCode LArParamsFromStdNtuple::initialize() 
{
  return StatusCode::SUCCESS ;
}


StatusCode LArParamsFromStdNtuple::stop()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "... in stop()" << endreq ;
  
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

  TChain* outfit = new TChain(m_ntuple_name.c_str());
  for ( std::vector<std::string>::const_iterator it = m_root_file_names.begin();
	it != m_root_file_names.end(); it++ ) {
    outfit->Add(it->c_str());
  }


  Int_t           channelId;
  Int_t           FT, slot, channel;
  Double_t        Tcal;
  Double_t        Fstep;
  Double_t        Offset;
  Double_t        dTimeCal;
  Int_t           nCB;
  Double_t        Omega0;
  Double_t        Taur;


  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  outfit->SetBranchAddress("channelId", &channelId);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("slot", &slot);
  outfit->SetBranchAddress("channel", &channel);
  outfit->SetBranchAddress("gain", &gain);
  outfit->SetBranchAddress("Tcal", &Tcal);
  outfit->SetBranchAddress("Fstep", &Fstep);
  outfit->SetBranchAddress("Offset", &Offset);
  outfit->SetBranchAddress("dTimeCal", &dTimeCal);
  outfit->SetBranchAddress("nCB", &nCB);
  outfit->SetBranchAddress("Omega0", &Omega0);
  outfit->SetBranchAddress("Taur", &Taur);

  // Create new objects
  LArCaliPulseParamsComplete *larCaliPulseParams = new LArCaliPulseParamsComplete();
  larCaliPulseParams->setGroupingType(m_groupingType, log);
  larCaliPulseParams->initialize();
  LArDetCellParamsComplete *larDetCellParams = new LArDetCellParamsComplete();
  larDetCellParams->setGroupingType(m_groupingType, log);
  larDetCellParams->initialize();


  unsigned int hwid;
  //unsigned int uflag;
  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t i = 0; i < nentries; i++ )
  {
    outfit->GetEvent(i);
    log << MSG::INFO << " Chan " <<  std::hex << channelId << std::dec << endreq;
    hwid = channelId;
    HWIdentifier id(hwid);
    if(FT != onlineHelper->feedthrough(id) || slot != onlineHelper->slot(id) || channel != onlineHelper->channel(id)) {
       log << MSG::ERROR << "Inconsistency in decoding HWID !!!!" <<endreq;
       log << MSG::ERROR << FT << " - " << onlineHelper->feedthrough(id) << endreq;
       log << MSG::ERROR << slot << " - " << onlineHelper->slot(id) << endreq;
       log << MSG::ERROR << channel << " - " << onlineHelper->channel(id) << endreq;
       log << MSG::ERROR << "Not creating PhysWave !!!!" << endreq;
       continue;
    }
	  
    // Add to objects
    larCaliPulseParams->set(id,gain,Tcal,Fstep,Offset,dTimeCal,nCB);
    larDetCellParams->set(id,gain,Omega0,Taur);
  }


  // store 
  sc=detStore->record(larCaliPulseParams,m_store_key_cali);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot record larCaliPulseParams to StoreGate with key = " << m_store_key_cali << endreq;
    return sc;
  }

  sc=detStore->record(larDetCellParams,m_store_key_det);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot record larDetCellParams to StoreGate with key = " << m_store_key_det << endreq;
    return sc;
  }

  // and symlink
  ILArCaliPulseParams *ilarCaliPulse = NULL;
  sc = detStore->symLink(larCaliPulseParams,ilarCaliPulse);
  if (sc.isFailure()) {
       log << MSG::FATAL << "Could not symlink ILArCaliPulseParams with LArCaliPulseParamsComplete." << endreq;
       return StatusCode::FAILURE;
  }

  ILArDetCellParams *ilarDetCell = NULL;
  sc = detStore->symLink(larDetCellParams,ilarDetCell);
  if (sc.isFailure()) {
     log << MSG::FATAL << "Could not symlink ILArDetCellParams with LArDetCellParamsComplete." << endreq;
     return StatusCode::FAILURE;
  }


  log << MSG::INFO << "LArParamsFromStdNtuple finalized!" << endreq;  

  return StatusCode::SUCCESS;
}

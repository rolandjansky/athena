/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArOFCBin_PhysCaliTdiffFromStdNtuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>


LArOFCBin_PhysCaliTdiffFromStdNtuple::LArOFCBin_PhysCaliTdiffFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{  
  declareProperty("FileNames", m_root_file_names);
  declareProperty("NtupleName", m_ntuple_name="PARAMS");
  declareProperty("Store_OFC", m_fillofc=false);
  declareProperty("StoreKey_OFC", m_store_key_ofcbin="LArOFC");
  declareProperty("Store_Tdiff", m_filltdiff=false);
  declareProperty("StoreKey_Tdiff", m_store_key_tdiff="LArPhysCaliTdiff");
  declareProperty("GroupingType", m_groupingType="FeedThrough");
}

LArOFCBin_PhysCaliTdiffFromStdNtuple::~LArOFCBin_PhysCaliTdiffFromStdNtuple() 
{}

StatusCode LArOFCBin_PhysCaliTdiffFromStdNtuple::initialize() 
{
  return StatusCode::SUCCESS ;
}


StatusCode LArOFCBin_PhysCaliTdiffFromStdNtuple::stop()
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
  Int_t           bin;
  Double_t        tdiff;


  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  outfit->SetBranchAddress("channelId", &channelId);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("slot", &slot);
  outfit->SetBranchAddress("channel", &channel);
  outfit->SetBranchAddress("gain", &gain);
  if(m_filltdiff) {
   outfit->SetBranchAddress("Tdiff", &tdiff);
  }
  if(m_fillofc) {
   outfit->SetBranchAddress("OFCBin", &bin);
  }

  // Create new objects
  LArOFCBinComplete *larOFCParams = new LArOFCBinComplete();
  larOFCParams->setGroupingType(m_groupingType, log);
  larOFCParams->initialize();
  LArPhysCaliTdiffComplete *larTdiffParams = new LArPhysCaliTdiffComplete();
  larTdiffParams->setGroupingType(m_groupingType, log);
  larTdiffParams->initialize();


  unsigned int hwid;
  //unsigned int uflag;
  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t i = 0; i < nentries; i++ )
  {
    outfit->GetEvent(i);
    log << MSG::INFO << " Chan " <<  std::hex << channelId << std::dec << endreq;
    log << MSG::INFO << bin << " | " << tdiff << endreq;
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
    if(m_fillofc) larOFCParams->set(id,gain,bin);
    if(m_filltdiff) larTdiffParams->set(id,gain,tdiff);
  }


  // store 
  if(m_fillofc) {
    sc=detStore->record(larOFCParams,m_store_key_ofcbin);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Cannot record larOFCParams to StoreGate with key = " << m_store_key_ofcbin << endreq;
      return sc;
    }
  }

  if(m_filltdiff) {
    sc=detStore->record(larTdiffParams,m_store_key_tdiff);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Cannot record larTdiffParams to StoreGate with key = " << m_store_key_tdiff << endreq;
      return sc;
    }
  }

  // and symlink

  if(m_filltdiff) {
     ILArPhysCaliTdiff *ilarTdiff = NULL;
     sc = detStore->symLink(larTdiffParams,ilarTdiff);
     if (sc.isFailure()) {
        log << MSG::FATAL << "Could not symlink ILArPhysCaliTdiff with LArTdiffParamsComplete." << endreq;
        return StatusCode::FAILURE;
     }
  }


  log << MSG::INFO << "LArOFCBin_PhysCaliTdiffFromStdNtuple finalized!" << endreq;  

  return StatusCode::SUCCESS;
}

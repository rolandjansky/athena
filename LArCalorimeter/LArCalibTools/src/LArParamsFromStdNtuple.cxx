/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


LArParamsFromStdNtuple::LArParamsFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator)
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
  ATH_MSG_INFO ( "... in stop()" );
  
  // get LArOnlineID helper
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );

  TChain* outfit = new TChain(m_ntuple_name.c_str());
  for (const std::string& s : m_root_file_names) {
    outfit->Add(s.c_str());
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
  ATH_CHECK ( larCaliPulseParams->setGroupingType(m_groupingType, msg()) );
  ATH_CHECK ( larCaliPulseParams->initialize() );
  LArDetCellParamsComplete *larDetCellParams = new LArDetCellParamsComplete();
  ATH_CHECK ( larDetCellParams->setGroupingType(m_groupingType, msg()) );
  ATH_CHECK ( larDetCellParams->initialize() );


  unsigned int hwid;
  //unsigned int uflag;
  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t i = 0; i < nentries; i++ )
  {
    outfit->GetEvent(i);
    ATH_MSG_INFO ( " Chan " <<  std::hex << channelId << std::dec );
    hwid = channelId;
    HWIdentifier id(hwid);
    if(FT != onlineHelper->feedthrough(id) || slot != onlineHelper->slot(id) || channel != onlineHelper->channel(id)) {
       ATH_MSG_ERROR ( "Inconsistency in decoding HWID !!!!" );
       ATH_MSG_ERROR ( FT << " - " << onlineHelper->feedthrough(id) );
       ATH_MSG_ERROR ( slot << " - " << onlineHelper->slot(id) );
       ATH_MSG_ERROR ( channel << " - " << onlineHelper->channel(id) );
       ATH_MSG_ERROR ( "Not creating PhysWave !!!!" );
       continue;
    }
	  
    // Add to objects
    larCaliPulseParams->set(id,gain,Tcal,Fstep,Offset,dTimeCal,nCB);
    larDetCellParams->set(id,gain,Omega0,Taur);
  }

  ATH_CHECK( detStore()->record(larCaliPulseParams,m_store_key_cali) );
  ATH_CHECK( detStore()->record(larDetCellParams,m_store_key_det) );

  // and symlink
  ILArCaliPulseParams *ilarCaliPulse = NULL;
  ATH_CHECK( detStore()->symLink(larCaliPulseParams,ilarCaliPulse) );

  ILArDetCellParams *ilarDetCell = NULL;
  ATH_CHECK( detStore()->symLink(larDetCellParams,ilarDetCell) );

  ATH_MSG_INFO ( "LArParamsFromStdNtuple finalized!" );
  return StatusCode::SUCCESS;
}

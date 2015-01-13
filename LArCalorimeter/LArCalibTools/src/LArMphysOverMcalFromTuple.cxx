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


LArMphysOverMcalFromTuple::LArMphysOverMcalFromTuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator)
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
  ATH_MSG_INFO ( "... in finalize()" );
  
  // get LArOnlineID helper
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );

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
  ATH_CHECK( detStore()->record(container,m_store_key) );
  // Symlink LArMphysOverMcalComplete to ILArMphysOverMcal for further use
  ILArMphysOverMcal *larMphysOverMcal = NULL;
  ATH_CHECK( detStore()->symLink(container,larMphysOverMcal) );

  ATH_MSG_INFO ( "LArMphysOverMcalyFromTuple finalized!" );
  
  return StatusCode::SUCCESS;
}

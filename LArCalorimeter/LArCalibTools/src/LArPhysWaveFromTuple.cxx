/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArPhysWaveFromTuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArPhysWaveContainer.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>


LArPhysWaveFromTuple::LArPhysWaveFromTuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator)
{  
  declareProperty("NPoints", m_NPoints = 800);
  declareProperty("SkipPoints", m_skipPoints = 0);
  declareProperty("PrefixPoints", m_prefixPoints = 0);
  declareProperty("deltaT", m_deltaT = 1.04e-9);
  declareProperty("LArWaveFlag", m_flag = 20);
  declareProperty("FileNames", m_root_file_names);
  declareProperty("StoreKey", m_store_key="FROMTUPLE");
  declareProperty("GroupingType", m_groupingType="FeedThrough");
}

LArPhysWaveFromTuple::~LArPhysWaveFromTuple() 
{}

StatusCode LArPhysWaveFromTuple::initialize() 
{
  return StatusCode::SUCCESS ;
}


StatusCode LArPhysWaveFromTuple::stop()
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

  // This algorithm assumes the input NTuple in contains less than 2000 
  // points.  If the NTuple contains less than 2000 points, the 
  // remaining points are automatically initialized to 0.
  // Catch potential array index out of range error.
  if ( m_NPoints > 2000 ) {
    ATH_MSG_ERROR ( " Too many points specified vs the expected content of the ntuple ! " );
    return StatusCode::FAILURE;
  }

  // variable names as in the Ntuple
  Int_t           BarEC; // 0 = Barrel   1 = EndCap
  Int_t           Side; // 0 = side C   1 = side A
  Int_t           FT; // feedthrough 0 - 31
  Int_t           Channel; // 0 - 127
  Int_t           Slot; // 1 -14
  Double_t        tphys[2000]; // The function
  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  outfit->SetBranchAddress("Channel", &Channel);
  outfit->SetBranchAddress("Slot", &Slot);
  outfit->SetBranchAddress("FT", &FT);
  if(outfit->FindBranch("BarAC")){ // Handle changed naming scheme
    ATH_MSG_INFO ( "'Side' is prefered branch name to 'BarAC'." );
    outfit->SetBranchAddress("BarAC", &Side);
  } else {
    outfit->SetBranchAddress("Side", &Side);
  }
  if(!outfit->FindBranch("BarEC")){ // Handle 
    ATH_MSG_INFO ( "'BarEC' branch not found. Assuming barrel data." );
    BarEC = 0;
  } else {
    outfit->SetBranchAddress("BarEC", &BarEC);
  }
  outfit->SetBranchAddress("Gain", &gain);
  outfit->SetBranchAddress("tphys", tphys);

  // Create new LArPhysWaveContainer
  LArPhysWaveContainer* larPhysWaveContainerNew = new LArPhysWaveContainer();
  larPhysWaveContainerNew->setGroupingType(m_groupingType, msg());
  larPhysWaveContainerNew->initialize();

  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t i = 0; i < nentries; i++ )
  {
    outfit->GetEvent(i);
    ATH_MSG_INFO ( BarEC << " Side " << Side << " FT " << FT << " slot " << Slot << " Chan " << Channel );
    HWIdentifier id = onlineHelper->channel_Id(BarEC, Side, FT, Slot, Channel);
    ATH_MSG_INFO ( std::hex << id << std::dec );

    std::vector<double> wave(m_NPoints);
    for ( unsigned int i = 0; i < m_NPoints; i++ ) wave[i]=0.;
    unsigned int skipped = 0;
    unsigned int limit = m_NPoints;
    if ( m_skipPoints < m_prefixPoints ) limit = m_NPoints+m_skipPoints-m_prefixPoints;
    for ( unsigned int i = 0; i < limit; i++ )
    {
      if ( skipped >= m_skipPoints ) 
      {
	wave[i-m_skipPoints+m_prefixPoints]=tphys[i];
      }
      else skipped++;
    }
 
    LArPhysWave newLArPhysWave(wave, m_deltaT, m_flag);
	  
    // Add physics wave to container
    larPhysWaveContainerNew->setPdata(id, newLArPhysWave, (CaloGain::CaloGain)gain);
  }

  ATH_CHECK( detStore()->record(larPhysWaveContainerNew,m_store_key) );
  ATH_MSG_INFO ( "LArPhysWaveFromTuple finalized!" );
  return StatusCode::SUCCESS;
}

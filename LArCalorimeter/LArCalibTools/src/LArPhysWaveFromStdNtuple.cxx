/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArPhysWaveFromStdNtuple.h"

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


LArPhysWaveFromStdNtuple::LArPhysWaveFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator)
{  
  declareProperty("SkipPoints", m_skipPoints = 0);
  declareProperty("PrefixPoints", m_prefixPoints = 0);
  declareProperty("FileNames", m_root_file_names);
  declareProperty("NtupleName", m_ntuple_name="PhysWave");
  declareProperty("StoreKey", m_store_key="FromStdNtuple");
  declareProperty("GroupingType", m_groupingType="FeedThrough");
}

LArPhysWaveFromStdNtuple::~LArPhysWaveFromStdNtuple() 
{}

StatusCode LArPhysWaveFromStdNtuple::initialize() 
{
  return StatusCode::SUCCESS ;
}


StatusCode LArPhysWaveFromStdNtuple::stop()
{
  ATH_MSG_INFO ( "... in stop()" );
  
  // get LArOnlineID helper
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );

  TChain* outfit = new TChain(m_ntuple_name.c_str());
  for ( std::vector<std::string>::const_iterator it = m_root_file_names.begin();
	it != m_root_file_names.end(); it++ ) {
    outfit->Add(it->c_str());
  }


  Int_t           timeIndex;
  Int_t           flag;
  Double_t          Dt;
  Double_t        timeOffset;
  Int_t           channelId;
  Int_t           FT, slot, channel;

  Double_t        Amplitude[2000]; // The function
  Double_t        Error[2000]; // The function
  Int_t          Triggers[2000]; // The function
  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  outfit->SetBranchAddress("channelId", &channelId);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("slot", &slot);
  outfit->SetBranchAddress("channel", &channel);
  outfit->SetBranchAddress("timeIndex", &timeIndex);
  outfit->SetBranchAddress("Dt", &Dt);
  outfit->SetBranchAddress("timeOffset", &timeOffset);
  outfit->SetBranchAddress("flag", &flag);
  outfit->SetBranchAddress("gain", &gain);
  outfit->SetBranchAddress("Amplitude", Amplitude);
  outfit->SetBranchAddress("Error", Error);
  outfit->SetBranchAddress("Triggers", Triggers);

  // Create new LArPhysWaveContainer
  LArPhysWaveContainer* larPhysWaveContainerNew = new LArPhysWaveContainer();
  larPhysWaveContainerNew->setGroupingType(m_groupingType, msg());
  larPhysWaveContainerNew->initialize();

  unsigned int hwid;
  unsigned int uflag;
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

    // Catch potential array index out of range error.
    if ( timeIndex >= 2000 ) {
      ATH_MSG_ERROR ( " Too many points specified vs the expected content of the ntuple ! " );
      ATH_MSG_ERROR ( "Not creating PhysWave !!!!" );
      continue;
    }
    std::vector<double> wave(timeIndex);
    std::vector<double> wave_err(timeIndex);
    std::vector<int> wave_trig(timeIndex);
    for ( int i = 0; i < timeIndex; i++ ) {
       wave[i]=0.;
       wave_err[i]=0.;
       wave_trig[i]=0.;
    }
    unsigned int skipped = 0;
    unsigned int limit = timeIndex;
    if ( m_skipPoints < m_prefixPoints ) limit = timeIndex+m_skipPoints-m_prefixPoints;
    for ( unsigned int i = 0; i < limit; i++ )
    {
      if ( skipped >= m_skipPoints ) 
      {
	wave[i-m_skipPoints+m_prefixPoints]=Amplitude[i];
	wave_err[i-m_skipPoints+m_prefixPoints]=Error[i];
	wave_trig[i-m_skipPoints+m_prefixPoints]=Triggers[i];
      }
      else skipped++;
    }
 
    uflag = flag;
    LArPhysWave newLArPhysWave(wave, wave_err, wave_trig, Dt, timeOffset, uflag);
	  
    // Add physics wave to container
    larPhysWaveContainerNew->setPdata(id, newLArPhysWave, (CaloGain::CaloGain)gain);
  }

  ATH_CHECK( detStore()->record(larPhysWaveContainerNew,m_store_key) );
  ATH_MSG_INFO ( "LArPhysWaveFromStdNtuple finalized!" );
  return StatusCode::SUCCESS;
}

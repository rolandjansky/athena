/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAutoCorrFromStdNtuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArAutoCorrComplete.h"

#include "LArTools/LArMCSymTool.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include "TFile.h"
#include "TBranch.h"
#include "TTree.h"
#include "TChain.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

// sFcal hardcoded numbers
const float sFcalcovr[31][3]={
   { 0.572496, 0.655057, 0.731093},
   {   0.075488, 0.104297, 0.214769},
   {    -0.148836, -0.187077, -0.136677},
   {   -0.196002, -0.24895, -0.252711},
   {   -0.177228, -0.21831, -0.245412},
   {   -0.143346, -0.168434, -0.201137},
   {   -0.11199, -0.123737, -0.154998},
   {   -0.0864405, -0.0892498, -0.116567},
   {   -0.0660753, -0.0641608, -0.0867919},
   {   -0.050389, -0.0458778, -0.0642172},
   {   -0.0385136, -0.032897, -0.047445},
   {   -0.0288351, -0.023311, -0.0351371},
   {   -0.0218402, -0.0163931, -0.0260872},
   {   -0.0165734, -0.0114467, -0.019315},
   {   -0.0125223, -0.00795447, -0.0141427},
   {   -0.00894386, -0.00551898, -0.0101339},
   {   -0.00576509, -0.00396242, -0.00705192},
   {   -0.00364762, -0.00298286, -0.00479298},
   {   -0.00188369, -0.00202063, -0.00284983},
   {   -0.000721197, -0.000986272, -0.00117402},
   {   2.62603e-05, -0.000216376, -0.000234439},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06},
   {   0.000133223, -5.59111e-06, 6.85495e-06}
       };

LArAutoCorrFromStdNtuple::LArAutoCorrFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_larmcsym("LArMCSymTool")
{  
  declareProperty("Nsamples",    m_nsamples=7);
  declareProperty("FileNames", m_root_file_names);
  declareProperty("NtupleName", m_ntuple_name="AUTOCORR");
  declareProperty("StoreKey", m_store_key="FromStdNtuple");
  declareProperty("GroupingType", m_groupingType="ExtendedSubDetector");
  declareProperty("isComplete",   m_isComplete=false);
  declareProperty("doSFcal",   m_sFcal=false);

}

LArAutoCorrFromStdNtuple::~LArAutoCorrFromStdNtuple() 
{}

StatusCode LArAutoCorrFromStdNtuple::initialize() 
{
  ATH_CHECK ( m_larmcsym.retrieve() ); 
  return StatusCode::SUCCESS ;
}


StatusCode LArAutoCorrFromStdNtuple::stop()
{

  ATH_MSG_INFO ( "... in stop()" );
  
  // get LArOnlineID helper
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );
  // and cabling service
  LArCablingService *larCablingSvc;
  ATH_CHECK( toolSvc()->retrieveTool("LArCablingService",larCablingSvc) );
  // and helper for FCAL
  const CaloIdManager* caloId_mgr;
  ATH_CHECK( detStore()->retrieve(caloId_mgr, "CaloIdManager") );
  const LArFCAL_ID* fcal_id = caloId_mgr->getFCAL_ID();


  TChain* outfit = new TChain(m_ntuple_name.c_str());
  for ( std::vector<std::string>::const_iterator it = m_root_file_names.begin();
	it != m_root_file_names.end(); it++ ) {
    outfit->Add(it->c_str());
  }


  Int_t           det;
  Int_t           channelId;
  Int_t           FT, slot, channel;

  Float_t         covr[100]; // The function
  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,

  outfit->SetBranchAddress("channelId", &channelId);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("slot", &slot);
  outfit->SetBranchAddress("channel", &channel);
  outfit->SetBranchAddress("detector", &det);
  outfit->SetBranchAddress("gain", &gain);
  outfit->SetBranchAddress("covr", covr);

  // Create new LArAutocorrContainer
  LArAutoCorrComplete* larAutoCorrComplete = NULL ;
  LArAutoCorrMC* larAutoCorrMC = NULL ;

  if(m_isComplete) {
    larAutoCorrComplete = new LArAutoCorrComplete();
    ATH_CHECK( larAutoCorrComplete->setGroupingType(m_groupingType, msg()) );
    ATH_CHECK( larAutoCorrComplete->initialize() );
  } else {
    larAutoCorrMC = new LArAutoCorrMC();
    ATH_CHECK( larAutoCorrMC->setGroupingType(m_groupingType, msg()) );
    ATH_CHECK( larAutoCorrMC->initialize() );
  } 

  typedef std::vector<float> AutoCorrVec;
  std::map<std::pair<unsigned int,int>, AutoCorrVec> AutoCorr;
  unsigned int hwid;
  int nsamples=m_nsamples;
  if(nsamples>100) nsamples=100;
  ATH_MSG_INFO( "Using " << nsamples << " samples" );
  // loop over entries in the Tuple, one entry = one channel
  // first create a map from existing entries, will fill container later
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t iev = 0; iev < nentries; ++iev )
  {
    outfit->GetEvent(iev);
    ATH_MSG_DEBUG ( " Chan " <<  std::hex << channelId << " det. "<< det << std::dec );

    hwid = channelId;
    HWIdentifier id(hwid);
    
      if(FT != onlineHelper->feedthrough(id) || slot != onlineHelper->slot(id) || channel != onlineHelper->channel(id)) {
        ATH_MSG_ERROR ( "Inconsistency in decoding HWID !!!!" );
        ATH_MSG_ERROR ( FT << " - " << onlineHelper->feedthrough(id) );
        ATH_MSG_ERROR ( slot << " - " << onlineHelper->slot(id) );
        ATH_MSG_ERROR ( channel << " - " << onlineHelper->channel(id) );
        continue;
    }

    if(!m_isComplete) {
       if (id != m_larmcsym->symOnline(id) ) {
           ATH_MSG_DEBUG( "Symmetrized, not stored" );
           continue;
       }
    } 

    AutoCorrVec av(nsamples);
    for(int i=0;i<nsamples; ++i) {av[i]=0.; }
    for (int i = 0; i < nsamples; ++i ) {
       av[i]=covr[i];
    }

    AutoCorr[std::make_pair(hwid,gain)]=av;

    ATH_MSG_DEBUG ( "after reading size " << AutoCorr[std::make_pair(hwid,gain)].size() );

  }

  ATH_MSG_INFO ( "Ntuple read out, going to store. Have " << AutoCorr.size() << " numbers" );

  // could fill only now
  std::map<std::pair<unsigned int,int>, AutoCorrVec>::iterator ibeg = AutoCorr.begin();
  std::map<std::pair<unsigned int,int>, AutoCorrVec>::iterator iend = AutoCorr.end();
  unsigned int count=0;
  for(;ibeg != iend; ++ibeg) {
     ATH_MSG_DEBUG ( " Chan " <<  std::hex << (ibeg->first).first  << std::dec );
     if(m_sFcal) {
        HWIdentifier id((ibeg->first).first);
        // doesn't work:
        //if(onlineHelper->isFCALchannel(id) && onlineHelper->feedthrough(id) == 6) {
        if(onlineHelper->barrel_ec(id)==1 && onlineHelper->feedthrough(id) == 6) {
           ATH_MSG_INFO ( "Not storing FCAL channel " << onlineHelper->channel_name(id) );
           continue;
        }
     }
        
    ATH_MSG_DEBUG( "Storing length " << (ibeg->second).size() ); 
    if(!m_isComplete) {
      larAutoCorrMC->set(HWIdentifier((ibeg->first).first),(ibeg->first).second, ibeg->second);
    } else {    
     larAutoCorrComplete->set(HWIdentifier((ibeg->first).first),(ibeg->first).second, ibeg->second);
    }
    ++count;
  }

  ATH_MSG_INFO ( "Storing old values done...." << count << " values copied..." );

  count = 0;
  if(m_sFcal) { // now add sFcal
      std::vector<HWIdentifier>::const_iterator itOnId = onlineHelper->channel_begin();
      std::vector<HWIdentifier>::const_iterator itOnIdEnd = onlineHelper->channel_end();
      int limit = nsamples;
      if(limit > 31) limit = 31;
      AutoCorrVec myvec(limit);
      ATH_MSG_INFO( "Using limit " << limit );
      for(; itOnId!=itOnIdEnd;++itOnId){
          const HWIdentifier chid = *itOnId;
          if(!m_isComplete) {
             if (chid != m_larmcsym->symOnline(chid) ) {
                 ATH_MSG_DEBUG( "Symmetrized, not stored" );
                 continue;
             }
          }
          //if(onlineHelper->isFCALchannel(chid)) {
          if(onlineHelper->barrel_ec(chid)==1 && onlineHelper->feedthrough(chid) >= 25 && onlineHelper->feedthrough(chid) < 28 ) {
             ATH_MSG_DEBUG ( "Adding sFCAL channel " << onlineHelper->channel_name(chid) );
             const int mod=fcal_id->module(larCablingSvc->cnvToIdentifier(chid));
             if(mod<0 || mod > 3) {
                ATH_MSG_ERROR ( "Wrong FCAL module: " << mod << " ignored !!" );
                continue;
             }
             for(int mygain=0; mygain<3; ++mygain) {
                for(int i=0;i<limit;++i) myvec[i]=sFcalcovr[i][mod-1];
                if(m_isComplete) {
                   larAutoCorrComplete->set(chid,mygain,myvec);
                } else {
                   larAutoCorrMC->set(chid,mygain,myvec);
                }
             }
             ATH_MSG_DEBUG ( "sFcal storing size " << myvec.size() );
             ++count;
          }
      }
      ATH_MSG_INFO ( "Adding " << count << " sFcal values ..." );

  }

  if(m_isComplete) {
     ATH_CHECK( detStore()->record(larAutoCorrComplete,m_store_key) );
     ATH_CHECK( detStore()->symLink(larAutoCorrComplete,(ILArAutoCorr*)larAutoCorrComplete) );
  } else {
     ATH_CHECK( detStore()->record(larAutoCorrMC,m_store_key) );
     ATH_CHECK( detStore()->symLink(larAutoCorrMC,(ILArAutoCorr*)larAutoCorrMC) );
     ATH_MSG_INFO ( "Stored container " << larAutoCorrMC->nGains() << "gains, " << larAutoCorrMC->totalNumberOfConditions() << " conditions, key: " << m_store_key );
  }

  return StatusCode::SUCCESS;
}

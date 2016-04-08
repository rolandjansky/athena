/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArShapeFromStdNtuple.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LArShapeComplete.h"

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


LArShapeFromStdNtuple::LArShapeFromStdNtuple (const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), m_larmcsym("LArMCSymTool")
{  
  declareProperty("SkipPoints", m_skipPoints = 0);
  declareProperty("PrefixPoints", m_prefixPoints = 0);
  declareProperty("FileNames", m_root_file_names);
  declareProperty("NtupleName", m_ntuple_name="SHAPE");
  declareProperty("StoreKey", m_store_key="FromStdNtuple");
  declareProperty("GroupingType", m_groupingType="ExtendedSubDetector");
  declareProperty("isComplete",   m_isComplete=false);

  m_done=false;
}

LArShapeFromStdNtuple::~LArShapeFromStdNtuple() 
{}

StatusCode LArShapeFromStdNtuple::initialize() 
{
  ATH_CHECK ( m_larmcsym.retrieve() ); 
  return StatusCode::SUCCESS ;
}


StatusCode LArShapeFromStdNtuple::stop()
{
  if(m_done) return StatusCode::SUCCESS;

  ATH_MSG_INFO ( "... in stop()" );
  
  // get LArOnlineID helper
  const LArOnlineID* onlineHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(onlineHelper, "LArOnlineID") );

  TChain* outfit = new TChain(m_ntuple_name.c_str());
  for ( std::vector<std::string>::const_iterator it = m_root_file_names.begin();
	it != m_root_file_names.end(); it++ ) {
    outfit->Add(it->c_str());
  }


  Int_t           phase=0;
  Int_t           det=0;
  Float_t         phaseTime=0;
  Float_t         timeOffset=0;
  Int_t           channelId=0;
  Int_t           FT=0, slot=0, channel=0;

  Float_t         Shape[2000]; // The function
  Float_t         ShapeDer[2000]; // The function
  Int_t           gain = 0; // LARHIGHGAIN = 0, LARMEDIUMGAIN = 1,  LARLOWGAIN = 2,
  Int_t           nsamples=0;

  outfit->SetBranchAddress("channelId", &channelId);
  outfit->SetBranchAddress("FT", &FT);
  outfit->SetBranchAddress("slot", &slot);
  outfit->SetBranchAddress("channel", &channel);
  outfit->SetBranchAddress("detector", &det);
  if(m_isComplete) {
    outfit->SetBranchAddress("PhaseTime", &phaseTime);
    outfit->SetBranchAddress("Phase", &phase);
    outfit->SetBranchAddress("timeOffset", &timeOffset);
  }
  outfit->SetBranchAddress("nSamples", &nsamples);
  outfit->SetBranchAddress("Gain", &gain);
  outfit->SetBranchAddress("Shape", Shape);
  outfit->SetBranchAddress("ShapeDer", ShapeDer);

  Float_t timeBinWidth=-1, prevTime=-1, timeOff=-1;
  Int_t prevPhase=-1;

  // Create new LArShapeContainer
  LArShapeComplete* larShapeComplete = NULL ;
  LArShape32MC* larShapeMC = NULL ;

  if(m_isComplete) {
    larShapeComplete = new LArShapeComplete();
    ATH_CHECK( larShapeComplete->setGroupingType(m_groupingType, msg()) );
    ATH_CHECK( larShapeComplete->initialize() );
  } else {
    larShapeMC = new LArShape32MC();
    ATH_CHECK( larShapeMC->setGroupingType(m_groupingType, msg()) );
    ATH_CHECK( larShapeMC->initialize() );
  } 

  std::vector<float> shapemc;
  std::vector<float> shape_dermc;
  typedef std::vector<std::vector<float> > wave2d;
  std::map<std::pair<unsigned int,int>, wave2d> shape;
  std::map<std::pair<unsigned int,int>, wave2d> shape_der;
  unsigned int hwid;
  // loop over entries in the Tuple, one entry = one channel
  Long64_t nentries = outfit->GetEntries();
  for ( Long64_t iev = 0; iev < nentries; ++iev )
  {
    outfit->GetEvent(iev);
    ATH_MSG_INFO ( " Chan " <<  std::hex << channelId << " det. "<< det << std::dec );

    hwid = channelId;
    //if (det != 4) hwid = (channelId<<4);
    HWIdentifier id(hwid);
    
    /*
    if(FT != onlineHelper->feedthrough(id) || slot != onlineHelper->slot(id) || channel != onlineHelper->channel(id)) {
      ATH_MSG_ERROR ( "Inconsistency in decoding HWID !!!!" );
      ATH_MSG_INFO  ( "Trying to fix..." );
      hwid = (channelId<<4);
      id=HWIdentifier(hwid);
    */
      if(FT != onlineHelper->feedthrough(id) || slot != onlineHelper->slot(id) || channel != onlineHelper->channel(id)) {
        ATH_MSG_ERROR ( "Inconsistency in decoding HWID !!!!" );
        ATH_MSG_ERROR ( FT << " - " << onlineHelper->feedthrough(id) );
        ATH_MSG_ERROR ( slot << " - " << onlineHelper->slot(id) );
        ATH_MSG_ERROR ( channel << " - " << onlineHelper->channel(id) );
        //if(det == 4) {
        //   ATH_MSG_ERROR ( "Ignoring for sFcal" );
        //} else {   
        //   ATH_MSG_ERROR ( "Not creating Shape !!!!" );
        //   continue;
        // }
        continue;
     /* }
      ATH_MSG_INFO  ( "Fixed....." );
     */ 
    }

    // Catch potential array index out of range error.
    if ( nsamples >= 2000 ) {
      ATH_MSG_ERROR ( " Too many points specified vs the expected content of the ntuple ! " );
      ATH_MSG_ERROR ( "Not creating Shape !!!!" );
      continue;
    } else {
      ATH_MSG_DEBUG ( "Working with " << nsamples << " samples" ); 
    }
    if(timeBinWidth < 0) {
       if(prevTime < 0) {
          prevTime=phaseTime;
          prevPhase=phase;
       } else {
          if(abs(phase-prevPhase) == 1) {
            timeBinWidth=fabs(phaseTime - prevTime);
          }
       }
    }

    if(timeOff < 0) timeOff=timeOffset;

    if(m_isComplete) { 
       if(shape[std::make_pair(hwid,gain)].size()==0) shape[std::make_pair(hwid,gain)].reserve(50);
       if(shape_der[std::make_pair(hwid,gain)].size()==0) shape_der[std::make_pair(hwid,gain)].reserve(50);
       shape[std::make_pair(hwid,gain)][phase].reserve(nsamples);
       shape_der[std::make_pair(hwid,gain)][phase].reserve(nsamples);
       for(int i=0;i<nsamples; ++i) {shape[std::make_pair(hwid,gain)][phase][i]=0.;
                                 shape_der[std::make_pair(hwid,gain)][phase][i]=0.;}
    } else {
       shapemc.resize(nsamples);
       shape_dermc.resize(nsamples);
       for(int i=0;i<nsamples; ++i) {shapemc[i]=0.; shape_dermc[i]=0.;}
    }
    unsigned int skipped = 0;
    unsigned int limit = nsamples;
    if ( m_skipPoints < m_prefixPoints ) limit = nsamples+m_skipPoints-m_prefixPoints;
    for ( unsigned int i = 0; i < limit; i++ ) {
      if ( skipped >= m_skipPoints ) {
       if(m_isComplete) { // accumulate into map
	shape[std::make_pair(hwid,gain)][phase][i-m_skipPoints+m_prefixPoints]=Shape[i];
	shape_der[std::make_pair(hwid,gain)][phase][i-m_skipPoints+m_prefixPoints]=ShapeDer[i];

       } else {
	shapemc[i-m_skipPoints+m_prefixPoints]=Shape[i];
	shape_dermc[i-m_skipPoints+m_prefixPoints]=ShapeDer[i];
       }
      } else skipped++;
    }

    if(!m_isComplete) {
       if (id != m_larmcsym->symOnline(id) ) {
           ATH_MSG_INFO( "Symmetrized, not stored" );
       } else {

           ATH_MSG_INFO( "Storing shape with length: " << shapemc.size() );

           //LArShapeP1 t(shapemc,shape_dermc);

           //larShapeMC->setPdata(id, LArShapeP1(shapemc,shape_dermc),gain);
           larShapeMC->set(id, gain, shapemc, shape_dermc);
           ATH_MSG_INFO( larShapeMC->Shape(id,gain).size() << " " << larShapeMC->ShapeDer(id,gain).size() );
           ATH_MSG_INFO( "Shape[2] =" << larShapeMC->Shape(id,gain)[2] << "shapemc[2] =" << shapemc[2] );
       }
    }
 
  }
  // for complete, could fill only now
  if(m_isComplete){
      std::map<std::pair<unsigned int,int>, wave2d>::iterator ibeg = shape.begin();
      std::map<std::pair<unsigned int,int>, wave2d>::iterator iend = shape.end();
      for(;ibeg != iend; ++ibeg) {
        larShapeComplete->set(HWIdentifier((ibeg->first).first),(ibeg->first).second,
                              ibeg->second, shape_der[std::make_pair((ibeg->first).first, (ibeg->first).second)],
                              timeOff,timeBinWidth);
      }
  }

  const ILArShape* larShape = NULL ;
  if(m_isComplete) {
     ATH_CHECK( detStore()->record(larShapeComplete,m_store_key) );
     ATH_CHECK( detStore()->symLink(larShapeComplete,larShape) );
  } else {
     ATH_CHECK( detStore()->record(larShapeMC,m_store_key) );
     ATH_CHECK( detStore()->symLink(larShapeMC,larShape) );
  }
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"
#include "muonEvent/MuonContainer.h"
#include "MissingETGoodness/MuonGoodiesFiller.h"
#include "MissingETGoodness/MuonSpShowerHelper.h"
///#include "MuonTimingInfoToolsInterfaces/IMuonTRTTimingTool.h"

using namespace std;

MET::Goodies& MuonGoodiesFiller::s_goodies(MET::Goodies::instance());


//------------------------------------------------------------------------------
MuonGoodiesFiller::MuonGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("MuonSpShowerContainerKey",     m_muonSpShowerContainerKey= "MuonSpShowers");
  declareProperty("MuonContainerKey",     m_muonContainerKey= "StacoMuonCollection");
  ///declareProperty("IMuonTRTTimingTool", p_ITRTTimingTool );
}


//------------------------------------------------------------------------------
StatusCode 
MuonGoodiesFiller::initialize() 
{
  ATH_MSG_DEBUG( "MuonGoodiesFiller initialize() has been called"  );
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode 
MuonGoodiesFiller::execute() 
{
  ATH_MSG_DEBUG( "execute() has been called"  );

  /* ------------------------------------------- */
  /* Find container in storegate                */
  /* ------------------------------------------- */
  
  const Rec::MuonSpShowerContainer *MuonSpShowers=0;
  //Retrieve muon shower container
  if (evtStore()->contains<Rec::MuonSpShowerContainer>(m_muonSpShowerContainerKey)) {
    ATH_CHECK( evtStore()->retrieve(MuonSpShowers, m_muonSpShowerContainerKey) );
  } else {
    ATH_MSG_DEBUG( "No MuonSpShowerContainer found in StoreGate, key:" << m_muonSpShowerContainerKey << ". Skipping."  );
    return StatusCode::SUCCESS;
  }

  //Retrieve muon container
  const Analysis::MuonContainer *muons=0;
  ATH_CHECK( evtStore()->retrieve( muons, m_muonContainerKey) );

/*
  //Retrieve p_ITRTTimingTool
  if ( p_ITRTTimingTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve tool " << p_ITRTTimingTool << endmsg;
    p_ITRTTimingTool = 0 ;
  }else{
    msg(MSG::DEBUG) << "Retrieved tool " << p_ITRTTimingTool << endmsg;
  }
*/
  
  /* ------------------------------------------- */
  /* Calculate and fill variables                */
  /* ------------------------------------------- */
  
  // sum for each event (if any), Fill effective sum, per event  
  Rec::MuonSpShower TotalSpShower;

  int JetTotMuonSpHits=-1; 
  if (MuonSpShowerHelper::SumSpShowers(TotalSpShower,MuonSpShowers)){
     JetTotMuonSpHits =MuonSpShowerHelper::TotalSpHits( &TotalSpShower);     
  }
  s_goodies.setValue("MuonSpTotHits"    , JetTotMuonSpHits   );

  int MuonSp0Hits=-1; 
  float MuonSp0Eta=0;
  float MuonSp0Phi=0;
  int   MuonSp1Hits=-1; 
  float MuonSp1Eta=0;
  float MuonSp1Phi=0;

  std::vector<int> indexsorted= MuonSpShowerHelper::SortListSpShower(MuonSpShowers);
  if(indexsorted.size()){     
     Rec::MuonSpShower mSP ;
     if(!MuonSpShowerHelper::GetSpShower(mSP,MuonSpShowers,indexsorted.at(0))){
       ATH_MSG_DEBUG( "Error getting muon sp shower"  );
     } else{          
        MuonSp0Hits=MuonSpShowerHelper::TotalSpHits(&mSP);
        MuonSp0Eta=mSP.eta();
        MuonSp0Phi=mSP.phi();
     }     
  }
  if(indexsorted.size()>=2){     
     Rec::MuonSpShower mSP ;
     if(!MuonSpShowerHelper::GetSpShower(mSP,MuonSpShowers,indexsorted.at(1))){
       ATH_MSG_DEBUG( "Error getting muon sp shower"  );
     } else{          
        MuonSp1Hits=MuonSpShowerHelper::TotalSpHits(&mSP);
        MuonSp1Eta=mSP.eta();
        MuonSp1Phi=mSP.phi();
     }     
  }
  s_goodies.setValue("MuonSp0Hits",MuonSp0Hits);
  s_goodies.setValue("MuonSp0Eta" ,MuonSp0Eta);
  s_goodies.setValue("MuonSp0Phi" ,MuonSp0Phi);
  s_goodies.setValue("MuonSp1Hits",MuonSp1Hits);
  s_goodies.setValue("MuonSp1Eta" ,MuonSp1Eta);
  s_goodies.setValue("MuonSp1Phi" ,MuonSp1Phi);
  
  //Loop over muons
  // TRT Timing info
  s_goodies.setValue("MuonTRTTiming", -999.);

/*
  double muonptmax(-999);
  for(Analysis::MuonContainer::const_iterator muonsItr = muons->begin(); muonsItr != muons->end(); ++muons){
    if ((*muonsItr)->hlv().perp() > muonptmax) {
      muonptmax = (*muonsItr)->hlv().perp() ;
      if (p_ITRTTimingTool != 0) {
        s_goodies.setValue("MuonTRTTiming"    , p_ITRTTimingTool->muonMuonTRTTiming( *(*muonsItr) )    );
      }
    }
  }
*/

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode 
MuonGoodiesFiller::finalize() 
{
  ATH_MSG_DEBUG( "MuonGoodiesFiller::finalize() has been called"  );
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
MuonGoodiesFiller::~MuonGoodiesFiller() {}


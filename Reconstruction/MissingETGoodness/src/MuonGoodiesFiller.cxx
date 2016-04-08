/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"
#include "muonEvent/MuonContainer.h"
#include "MissingETGoodness/MuonGoodiesFiller.h"
#include "MissingETGoodness/MuonSpShowerHelper.h"
///#include "MuonTimingInfoToolsInterfaces/IMuonTRTTimingTool.h"

using namespace std;

MET::Goodies& MuonGoodiesFiller::goodies(MET::Goodies::instance());


//------------------------------------------------------------------------------
MuonGoodiesFiller::MuonGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_storeGate(0)
{
  declareProperty("MuonSpShowerContainerKey",     _muonSpShowerContainerKey= "MuonSpShowers");
  declareProperty("MuonContainerKey",     _muonContainerKey= "StacoMuonCollection");
  ///declareProperty("IMuonTRTTimingTool", p_ITRTTimingTool );
  mLog = 0;
}


//------------------------------------------------------------------------------
StatusCode 
MuonGoodiesFiller::initialize() 
{
  if (!mLog) mLog = new MsgStream(msgSvc(), name() );

  *mLog << MSG::DEBUG << "MuonGoodiesFiller initialize() has been called" << endreq;

  /** get a handle of StoreGate for access to the Event Store */
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     *mLog << MSG::ERROR
          << "Unable to retrieve pointer to StoreGateSvc"
          << endreq;
     return sc;
  }

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode 
MuonGoodiesFiller::execute() 
{
  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  *mLog << MSG::DEBUG << "execute() has been called" << endreq;

  /* ------------------------------------------- */
  /* Find container in storegate                */
  /* ------------------------------------------- */
  
  const Rec::MuonSpShowerContainer *MuonSpShowers=0;
  //Retrieve muon shower container
  if (m_storeGate->contains<Rec::MuonSpShowerContainer>(_muonSpShowerContainerKey)) {
    sc = m_storeGate->retrieve(MuonSpShowers, _muonSpShowerContainerKey);
    if ( !sc.isSuccess() ) { 
      *mLog << MSG::WARNING << "Could not retrieve MuonSpShowerContainer from StoreGate, key:" << _muonSpShowerContainerKey << endreq;
      return sc; 
    }
  } else {
    *mLog << MSG::DEBUG << "No MuonSpShowerContainer found in StoreGate, key:" << _muonSpShowerContainerKey << ". Skipping." << endreq; 
    return sc;
  }

  //Retrieve muon container
  const Analysis::MuonContainer *muons=0;
  sc=m_storeGate->retrieve( muons, _muonContainerKey);
  if ( !sc.isSuccess() ) { 
    *mLog << MSG::WARNING << "Could not retrieve MuonContainer from StoreGate, key:" << _muonContainerKey << endreq;
    return sc; 
  }

/*
  //Retrieve p_ITRTTimingTool
  if ( p_ITRTTimingTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve tool " << p_ITRTTimingTool << endreq;
    p_ITRTTimingTool = 0 ;
  }else{
    msg(MSG::DEBUG) << "Retrieved tool " << p_ITRTTimingTool << endreq;
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
  goodies.setValue("MuonSpTotHits"    , JetTotMuonSpHits   );

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
        *mLog << MSG::DEBUG << "Error getting muon sp shower" <<  endreq;
     } else{          
        MuonSp0Hits=MuonSpShowerHelper::TotalSpHits(&mSP);
        MuonSp0Eta=mSP.eta();
        MuonSp0Phi=mSP.phi();
     }     
  }
  if(indexsorted.size()>=2){     
     Rec::MuonSpShower mSP ;
     if(!MuonSpShowerHelper::GetSpShower(mSP,MuonSpShowers,indexsorted.at(1))){
        *mLog << MSG::DEBUG << "Error getting muon sp shower" <<  endreq;
     } else{          
        MuonSp1Hits=MuonSpShowerHelper::TotalSpHits(&mSP);
        MuonSp1Eta=mSP.eta();
        MuonSp1Phi=mSP.phi();
     }     
  }
  goodies.setValue("MuonSp0Hits",MuonSp0Hits);
  goodies.setValue("MuonSp0Eta" ,MuonSp0Eta);
  goodies.setValue("MuonSp0Phi" ,MuonSp0Phi);
  goodies.setValue("MuonSp1Hits",MuonSp1Hits);
  goodies.setValue("MuonSp1Eta" ,MuonSp1Eta);
  goodies.setValue("MuonSp1Phi" ,MuonSp1Phi);
  
  //Loop over muons
  // TRT Timing info
  goodies.setValue("MuonTRTTiming", -999.);

/*
  double muonptmax(-999);
  for(Analysis::MuonContainer::const_iterator muonsItr = muons->begin(); muonsItr != muons->end(); ++muons){
    if ((*muonsItr)->hlv().perp() > muonptmax) {
      muonptmax = (*muonsItr)->hlv().perp() ;
      if (p_ITRTTimingTool != 0) {
        goodies.setValue("MuonTRTTiming"    , p_ITRTTimingTool->muonMuonTRTTiming( *(*muonsItr) )    );
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
  *mLog << MSG::DEBUG << "MuonGoodiesFiller::finalize() has been called" << endreq;

  delete mLog;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
MuonGoodiesFiller::~MuonGoodiesFiller() {}


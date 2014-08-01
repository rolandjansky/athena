/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewEventSelectionTool.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

static const InterfaceID IID_IMboyViewEventSelectionTool("MboyViewEventSelectionTool", 1, 0);

const InterfaceID& MboyViewEventSelectionTool::interfaceID()
{
  return IID_IMboyViewEventSelectionTool;
}

MboyViewEventSelectionTool::MboyViewEventSelectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewEventSelectionTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyViewEventSelectionTool::~MboyViewEventSelectionTool(){}

// Initialize
StatusCode MboyViewEventSelectionTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO( "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyViewEventSelectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewEventSelectionTool::EventSelection(
               std::string MuonRecDigiStoreTDSLocation,
               std::string TrkTrackCollectionLocation,
               int EvtSelectionType,
               int EvtSelectionThreshold
){

  if (   EvtSelectionType != 2
      && EvtSelectionType != 3
      && EvtSelectionType != 4
     ) return StatusCode::SUCCESS;

//Access digits
  if (!(evtStore()->contains<MuonRecDigiStoreTDS>(MuonRecDigiStoreTDSLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << MuonRecDigiStoreTDSLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  StatusCode sc = StatusCode::SUCCESS;

  const DataHandle<MuonRecDigiStoreTDS> pStoreDigitsTDS ;
  sc = evtStore()->retrieve(pStoreDigitsTDS,MuonRecDigiStoreTDSLocation);
  if (sc.isFailure()) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
     ATH_MSG_WARNING( " Selection: pStoreDigitsTDS not found at " << MuonRecDigiStoreTDSLocation ) ; 
    }
    return( StatusCode::SUCCESS);
  }
  const MuonRecDigiStoreAth* pStoreDigits = &(pStoreDigitsTDS->m_MuonRecDigiStore);
  int TotalNberOfDigi1st = pStoreDigits->NberOfDigiMDT() + pStoreDigits->NberOfDigiCSC();

//Access tracks
  if (!(evtStore()->contains<TrackCollection>(TrkTrackCollectionLocation))) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_INFO( " Not existing " << TrkTrackCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS );
  }

  const DataVector<Trk::Track>* pContainerTracks ;
  sc = evtStore()->retrieve(pContainerTracks, TrkTrackCollectionLocation);
  if (sc.isFailure()){
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
     ATH_MSG_WARNING( " Selection: pContainerTracks not found at " << TrkTrackCollectionLocation ) ;
    }
    return( StatusCode::SUCCESS);
  }
  int TotalNberOfTrack = pContainerTracks->size() ;

  if ( EvtSelectionType == 2 && TotalNberOfTrack <= EvtSelectionThreshold && TotalNberOfDigi1st != 0 ) return( StatusCode::SUCCESS); 
  if ( EvtSelectionType == 3 && TotalNberOfTrack >= EvtSelectionThreshold && TotalNberOfDigi1st != 0 ) return( StatusCode::SUCCESS);
  if ( EvtSelectionType == 4 && TotalNberOfTrack == EvtSelectionThreshold && TotalNberOfDigi1st != 0 ) return( StatusCode::SUCCESS);

  return StatusCode::FAILURE;

}


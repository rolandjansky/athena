/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrackRecord/TrackRecordCollection.h"

#include "GaudiKernel/SystemOfUnits.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyGenTrackTool.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MboyGenTrackStore.h"

MboyGenTrackTool::MboyGenTrackTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyGenTrackTool>(this);

  m_KounterErrorMessage    =  0; 
  m_KounterErrorMessageMax = 10;   

}

MboyGenTrackTool::~MboyGenTrackTool(){}

// Initialize
StatusCode MboyGenTrackTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }


//Create MboyGenTrackStore
  p_MboyGenTrackStore = new MboyGenTrackStore ;


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

StatusCode MboyGenTrackTool::finalize(){

 delete p_MboyGenTrackStore;
 return StatusCode::SUCCESS;
 
}

StatusCode MboyGenTrackTool::GetMboyGenTrackStore(
               MboyGenTrackStore*& pMboyGenTrackStore,
               std::string TrackRecordCollectionLocation
){

  StatusCode sc = StatusCode::SUCCESS;


//Set MboyGenTrackStore
  p_MboyGenTrackStore->Reset();
  const TrackRecordCollection* pTrackRecordCollection(0);
  sc = evtStore()->retrieve(pTrackRecordCollection, TrackRecordCollectionLocation);
  if ( sc.isFailure() ) {
    m_KounterErrorMessage = m_KounterErrorMessage + 1 ;
    if (m_KounterErrorMessageMax >= m_KounterErrorMessage){
      ATH_MSG_WARNING( "Cannot retrieve Trackrecord collection " 
          ) ;
    }
  }else{
    for (TrackRecordCollection::const_iterator trkit=pTrackRecordCollection->begin(); trkit != pTrackRecordCollection->end() ; ++trkit) {
      double Pt = (*trkit)->GetEnergy()
           /sqrt(1.+sinh((*trkit)->GetMomentum().eta())*sinh((*trkit)->GetMomentum().eta()) );
      int    PDGCode = (*trkit)->GetPDGCode()                 ;
      double PosX    = (*trkit)->GetPosition().x()            ;
      double PosY    = (*trkit)->GetPosition().y()            ;
      double PosZ    = (*trkit)->GetPosition().z()            ;
      double PX      = Pt*cos((*trkit)->GetMomentum().phi())  ;
      double PY      = Pt*sin((*trkit)->GetMomentum().phi())  ;
      double PZ      = Pt*sinh((*trkit)->GetMomentum().eta()) ;
      MboyGenTrack aMboyGenTrack;
      aMboyGenTrack.SetPDGCode(PDGCode); 
      aMboyGenTrack.SetPosX   (PosX   );
      aMboyGenTrack.SetPosY   (PosY   );
      aMboyGenTrack.SetPosZ   (PosZ   );
      aMboyGenTrack.SetPX     (PX     );
      aMboyGenTrack.SetPY     (PY     ); 
      aMboyGenTrack.SetPZ     (PZ     );
      p_MboyGenTrackStore->Add(aMboyGenTrack);
    }
  }
  pMboyGenTrackStore = p_MboyGenTrackStore ;


  return StatusCode::SUCCESS;

}

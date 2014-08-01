/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrackRecord/TrackRecordCollection.h"
#include "TrackRecord/TrackRecord.h"

/////////////////////////////////////////////////////////
#include "MboyView/MboyViewGenEventSelectionTool.h"

#include "MboyAthEvt/MuonRecDigiStoreTDS.h"
#include "MboyAthEvt/MuonRecDigiStoreAth.h"

static const InterfaceID IID_IMboyViewGenEventSelectionTool("MboyViewGenEventSelectionTool", 1, 0);

const InterfaceID& MboyViewGenEventSelectionTool::interfaceID()
{
  return IID_IMboyViewGenEventSelectionTool;
}

MboyViewGenEventSelectionTool::MboyViewGenEventSelectionTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<MboyViewGenEventSelectionTool>(this);

}

MboyViewGenEventSelectionTool::~MboyViewGenEventSelectionTool(){}

// Initialize
StatusCode MboyViewGenEventSelectionTool::initialize(){

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

StatusCode MboyViewGenEventSelectionTool::finalize(){return StatusCode::SUCCESS;}

StatusCode MboyViewGenEventSelectionTool::EventSelection(
               std::vector<std::string> TrackRecordCollectionLocationList,
               int GenEvtSelectionType,
               float GenEvtSelectionEtaMin,
               float GenEvtSelectionEtaMax
){

  if (   GenEvtSelectionType != 2
     ) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;

  int TrackRecordCollectionLocationListSize =  TrackRecordCollectionLocationList.size() ;
  for (int TrackRecordCollectionLocationListItem=0; TrackRecordCollectionLocationListItem<TrackRecordCollectionLocationListSize ; TrackRecordCollectionLocationListItem++){
    std::string TheCurrentTrackRecordCollectionLocation = TrackRecordCollectionLocationList[TrackRecordCollectionLocationListItem] ;
    if (evtStore()->contains<TrackRecordCollection>(TheCurrentTrackRecordCollectionLocation)) {
      const TrackRecordCollection *pTrackRecordCollection(0);
      sc = evtStore()->retrieve(pTrackRecordCollection, TheCurrentTrackRecordCollectionLocation);
      if ( sc.isFailure() ) {
    	ATH_MSG_INFO( " pTrackRecordCollection not found at existing " << TheCurrentTrackRecordCollectionLocation  ) ;
    	return( StatusCode::SUCCESS );
      }
      for (TrackRecordCollection::const_iterator trkit=pTrackRecordCollection->begin();trkit != pTrackRecordCollection->end() ; ++trkit) {
        double AbsEta = fabs( (*trkit)->GetMomentum().eta() ) ;
        if ( GenEvtSelectionEtaMin <= AbsEta  && AbsEta <= GenEvtSelectionEtaMax ) return StatusCode::SUCCESS;
      }
    }
  }

  return StatusCode::FAILURE;

}


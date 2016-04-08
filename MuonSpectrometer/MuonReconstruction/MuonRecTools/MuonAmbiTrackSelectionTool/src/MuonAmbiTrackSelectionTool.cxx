/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonAmbiTrackSelectionTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonAmbiTrackSelectionTool/MuonAmbiTrackSelectionTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include <iostream>
#include <stdio.h>

#include <ext/functional>
//================ Constructor =================================================

Muon::MuonAmbiTrackSelectionTool::MuonAmbiTrackSelectionTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_assoTool("Trk::PRD_AssociationTool/PRD_AssociationTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool") 
{
  declareInterface<IAmbiTrackSelectionTool>(this);

  //  template for property decalration
  declareProperty("AssociationTool"      , m_assoTool);

  declareProperty("MaxOverlapFraction"   , m_maxOverlapFraction = 0.1 );
  declareProperty("KeepPartialOverlaps"  , m_keepPartial = true );
  declareProperty("KeepMoreThanOneShare" , m_keepMoreThanOne = true );
}

//================ Destructor =================================================

Muon::MuonAmbiTrackSelectionTool::~MuonAmbiTrackSelectionTool()
{}


//================ Initialisation =================================================

StatusCode Muon::MuonAmbiTrackSelectionTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;


  sc = m_assoTool.retrieve();
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR("Failed to retrieve tool " << m_assoTool);
      return StatusCode::FAILURE;
    } 
  else
    ATH_MSG_DEBUG("Retrieved tool " << m_assoTool);

  sc = m_printer.retrieve();
  if (sc.isSuccess()){
    ATH_MSG_DEBUG("Retrieved " << m_printer);
  }else{
    ATH_MSG_ERROR("Could not get " << m_printer); 
    return sc;
  }

  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::MuonAmbiTrackSelectionTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//============================================================================================
const Trk::Track* Muon::MuonAmbiTrackSelectionTool::getCleanedOutTrack(const Trk::Track* ptrTrack, const Trk::TrackScore /*score*/) 
{

  unsigned int  numshared     = 0;
  unsigned int  numhits       = 0;
  
  if( !ptrTrack ) return 0;

  // get all TSOS
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  
  
  ATH_MSG_VERBOSE("New Track " << m_printer->print(*ptrTrack));

  std::map<Muon::MuonStationIndex::StIndex,int> sharedPrecisionPerLayer;
  std::map<Muon::MuonStationIndex::StIndex,int> sharedTriggerEtaPerLayer;
  std::map<Muon::MuonStationIndex::StIndex,int> sharedPhiPerLayer;
  std::map<Muon::MuonStationIndex::StIndex,int> precisionPerLayer;
  std::map<Muon::MuonStationIndex::StIndex,int> triggerEtaPerLayer;
  std::map<Muon::MuonStationIndex::StIndex,int> phiPerLayer;
  
  // loop over TSOS
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
  for ( ; iTsos != iTsosEnd ; ++iTsos) {
      
    // get measurment from TSOS
    const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();
    if (!meas) continue;

    // only look at measurements
    if( !(*iTsos)->type(Trk::TrackStateOnSurface::Measurement ) ) continue;
      

    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
    if (!rot) {
      // could be a Pseudo-Measurement
      const Trk::CompetingRIOsOnTrack* competingROT = dynamic_cast <const Trk::CompetingRIOsOnTrack*> (meas);
      if (competingROT) {
	bool measPhi = false;
	++numhits;
	const unsigned int numROTs = competingROT->numberOfContainedROTs();
	bool overlap = false;
	Muon::MuonStationIndex::StIndex stIndex = Muon::MuonStationIndex::StUnknown;
	for( unsigned int i=0;i<numROTs;++i ){
	  const Trk::RIO_OnTrack* rot = &competingROT->rioOnTrack(i);
	  if( !rot || !rot->prepRawData() || !m_idHelperTool->isMuon(rot->identify()) ) continue;
	  stIndex = m_idHelperTool->stationIndex(rot->identify());
	  measPhi = m_idHelperTool->measuresPhi(rot->identify());

	  if ( m_assoTool->isUsed(*(rot->prepRawData()))) {
	    ATH_MSG_VERBOSE("Track overlap found! " << m_idHelperTool->toString(rot->identify()));
	    overlap = true;
	    break;
	  }
	}
	
	if( overlap ){
	  ++numshared;
	  if( measPhi ) ++sharedPhiPerLayer[stIndex];
	  else          ++sharedTriggerEtaPerLayer[stIndex];
	}else{
	  if( measPhi ) ++phiPerLayer[stIndex];
	  else          ++triggerEtaPerLayer[stIndex];
	}

      }
    }else{

      if(!m_idHelperTool->isMuon(rot->identify())) continue;

      ++numhits;

      Muon::MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(rot->identify());
      bool measPhi = m_idHelperTool->measuresPhi(rot->identify());
      bool isTrigger = m_idHelperTool->isTrigger(rot->identify());
      if( measPhi ) {
	++phiPerLayer[stIndex];
      }else{
	if( isTrigger ) ++triggerEtaPerLayer[stIndex];
	else            ++precisionPerLayer[stIndex];
      }
      // allow no overlap
      if ( m_assoTool->isUsed(*(rot->prepRawData()))) {
	ATH_MSG_VERBOSE("Track overlap found! " << m_idHelperTool->toString(rot->identify()));
	++numshared;

	if( measPhi ) {
	  ++sharedPhiPerLayer[stIndex];
	}else{
	  if( isTrigger ) ++sharedTriggerEtaPerLayer[stIndex];
	  else            ++sharedPrecisionPerLayer[stIndex];
	}
      }
    }
  }
  if( numhits == 0 ) {
    ATH_MSG_WARNING("Got track without Muon hits " << m_printer->print(*ptrTrack));
    return 0;
  }
  double overlapFraction = (double)numshared/(double)numhits;

  ATH_MSG_DEBUG("Track "<< ptrTrack<<" has "<< numhits <<" hit, shared " << numshared 
		<< " overlap fraction " << overlapFraction << " layers " << precisionPerLayer.size() 
		<< " shared " << sharedPrecisionPerLayer.size() );

  if( overlapFraction > m_maxOverlapFraction ) {
    if( m_keepPartial ){
      if( sharedPrecisionPerLayer.empty() ) {
	ATH_MSG_DEBUG("Track is not sharing precision hits, keeping ");
	return ptrTrack;
      }
      if( overlapFraction < 0.25 && precisionPerLayer.size() > 2 && precisionPerLayer.size() - sharedPrecisionPerLayer.size() == 1 ) {
	ATH_MSG_DEBUG("Three station track differing by one precision layer, keeping ");
	return ptrTrack;
      }
      if( overlapFraction < 0.35 && precisionPerLayer.size() > 2 && precisionPerLayer.size() - sharedPrecisionPerLayer.size() > 1 && m_keepMoreThanOne) {
	ATH_MSG_DEBUG("Three station track differing by more than one precision layer, keeping ");
	return ptrTrack;
      }
    }
    return 0;
  }
   
  return ptrTrack;
}


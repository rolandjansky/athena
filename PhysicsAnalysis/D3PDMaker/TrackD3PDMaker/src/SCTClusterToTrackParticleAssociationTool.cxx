/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTClusterToTrackParticleAssociationTool.h"
 
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include <map>

#include "TrkTrack/TrackStateOnSurface.h"
#include "Identifier/Identifier.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODEventInfo/EventInfo.h"

 
namespace D3PD {
 
  SCTClusterToTrackParticleAssociationTool::SCTClusterToTrackParticleAssociationTool (const std::string& type,
								  const std::string& name,
								  const IInterface* parent)
    : Base (type, name, parent),
      m_tr(0),
      m_eventNumber(0)
  {
    declareProperty ("TrackParticleContainerSGKey", m_TrackParticleContainer = "TrackParticleCandidate");
  }

 
  StatusCode SCTClusterToTrackParticleAssociationTool::initialize()
  {
 
    CHECK( Base::initialize() );

    m_tr = 0;
    m_eventNumber = -1;

    return StatusCode::SUCCESS;
  }
 
 
  StatusCode SCTClusterToTrackParticleAssociationTool::book ()
  {
 
    CHECK( Base::book() );

    return StatusCode::SUCCESS;
  }


  StatusCode SCTClusterToTrackParticleAssociationTool::reset(const InDet::SCT_Cluster& cluster)
  {
 
    //REPORT_MESSAGE (MSG::DEBUG) << "EnterReset";

    // Get Track Collections
    // Should be using Getter tools intead of StoreGate
    // -- Tracks

    m_trackParticleForAssoc.clear();
  
    StatusCode sc = StatusCode::SUCCESS;

    const xAOD::EventInfo* eventInfo;
    sc = evtStore()->retrieve(eventInfo);
    if (sc.isFailure())
      {
        REPORT_MESSAGE  (MSG::ERROR) << "Could not retrieve event info";
      }
    int eventNumber = (int)eventInfo->eventNumber();


    // FIXME: Obviously bogus! Should catch an incident.
    if (m_eventNumber != eventNumber ){ // tretrieve only once!

      m_eventNumber = eventNumber;
     
      m_trackParticleMap.clear();

      sc = evtStore()->retrieve(m_tr, m_TrackParticleContainer);
      if(sc.isFailure() || !m_tr) {
	REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve TrackParticleContainer";
	return StatusCode::FAILURE;
      }
      else {

        
	Rec::TrackParticleContainer::const_iterator trItr = m_tr->begin();
	for( ; trItr!=m_tr->end(); trItr++){


	  const Trk::Track *track = (*trItr)->originalTrack();
	  if ( !track ){
	    //REPORT_MESSAGE (MSG::DEBUG) << "Could not access original track";
	    continue;
	  }
	  std::vector< Identifier> sctClusterIdentifier;

	  const DataVector<const Trk::TrackStateOnSurface>* trackHits = track->trackStateOnSurfaces();

	  if (!trackHits){
	    //REPORT_MESSAGE (MSG::DEBUG) << "REVERSE TESTASSOC for current track measurementsOntrack == Null, no clusters will be associated with this track";
	    continue;
	  }

	  DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackHits->begin();

	  for ( ; it != trackHits->end(); it++){

	    const InDet::SiClusterOnTrack *clus = dynamic_cast<const InDet::SiClusterOnTrack*>((*it)->measurementOnTrack());

	    if (clus){

	      const Trk::TrkDetElementBase* detectorElement = clus->detectorElement();
	      // Dynamic cast to Silicon element and do Pixel / SCT element test
	      const InDetDD::SiDetectorElement* siliconDetectorElement = dynamic_cast< const InDetDD::SiDetectorElement* >(detectorElement);
	      if ( siliconDetectorElement ){

		// Get Pointer to prepRawDataObject
		const InDet::SCT_Cluster *RawDataClus = dynamic_cast<const InDet::SCT_Cluster*>(clus->prepRawData());

		// check if Cluster is in SCT ...
		if (RawDataClus!=0){
		  if (RawDataClus->detectorElement()->isSCT()){

		    //REPORT_MESSAGE (MSG::DEBUG) << "REVERSE TESTASSOC SiHIT via prepRawData";

		    Identifier sctClusterID = RawDataClus->identify();
		    sctClusterIdentifier.push_back(sctClusterID);
		    //REPORT_MESSAGE (MSG::DEBUG) << "(2)REVERSE TESTASSOC TSoS-Type is: "<<   (*it)->types().to_ulong();

		  } // isSCT
		} // RawDataClus
	      } // siliconDetectorElement
	      else {
                //REPORT_MESSAGE (MSG::DEBUG) << "REVERSE TESTASSOC can't cast to siliconDetElement!!!!";
              }
	    } // clus
	  } // TSoS loop

	  m_trackParticleMap.insert(std::pair<const Rec::TrackParticle*, std::vector<Identifier> >(*trItr,sctClusterIdentifier));

	} // track loop 
      } // if trackParticle is properly retrieved
    } // if !m_tr

    // reverse logic compare to TrackSCTClusterAssociationTool

    Identifier clusterId = cluster.identify();

    std::map< const Rec::TrackParticle*, std::vector<Identifier> >::const_iterator mapIt = m_trackParticleMap.begin();
    for( ; mapIt!=m_trackParticleMap.end(); mapIt++){


      if ( find(mapIt->second.begin(),mapIt->second.end(),clusterId)!= mapIt->second.end() ){
	m_trackParticleForAssoc.push_back(mapIt->first);
      }
    }

    m_pItr = m_trackParticleForAssoc.begin();
    m_pEnd = m_trackParticleForAssoc.end();
      
    //REPORT_MESSAGE (MSG::DEBUG) << "LeavingReset";

    return StatusCode::SUCCESS;
  }
  //---------------------------------------------------------------------------------------------
  const Rec::TrackParticle* SCTClusterToTrackParticleAssociationTool::next(){

    //REPORT_MESSAGE (MSG::DEBUG) << "EnterNext";

    if(m_pItr==m_pEnd){

      //m_trackParticleMap.clear();
      //m_trackParticleForAssoc.clear();
      //m_tr=0;

      return 0;
    }

    const Rec::TrackParticle *trackParticle = *m_pItr;

    m_pItr++;

    //REPORT_MESSAGE (MSG::DEBUG) << "LeavingNext";
    return trackParticle;
  }

 
} // namespace D3PD

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPTDBasicTool.cxx
/// @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Implementation file for AFPTDBasicTool used in tracks reconstruction.

// STL includes
#include<list>
#include<sstream>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// xAOD includes
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFTrack.h"
#include "xAODForward/AFPToFTrackContainer.h"
#include "xAODForward/AFPToFTrackAuxContainer.h"

// AFP_LocReco includes
#include "AFP_LocReco/AFPTDBasicTool.h"

AFPTDBasicTool::AFPTDBasicTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent )
{
  declareInterface< IAFPTDLocRecoTrackAlgTool >(this);

  declareProperty( "hitContainerName", m_hitContainerName = "AFPToFHitContainer", "Name of the container ToF hits from which tracks are to be reconstructed" );
  declareProperty( "tracksContainerName", m_tracksContainerName = "AFPToFTrackContainer", "Name of the container in which ToF tracks are saved");
  declareProperty( "stationID", m_stationID = 0, "ID number of station for which tracks should be reconstructed");
  declareProperty( "maxAllowedLength", m_maxAllowedLength = 100, "Maximal length of the bar signal at which bar can be joined to the track");
  declareProperty( "minHitsNumber", m_minHitsNumber = 3, "Minimal number of bars in track. If there are less bars track is rejected"); 
  declareProperty( "TimeOffset", m_TimeOffset, "Array of bar time offsets with respect to first bar");
  declareProperty( "BarWeight", m_BarWeight, "Array of bar weights used to calculate average time");

}

StatusCode AFPTDBasicTool::initialize()
{

  // print information about initialised stations

  ATH_MSG_INFO("Station with ID="<<m_stationID <<" will have minimum number of "<<m_minHitsNumber <<" bars.");
  ATH_MSG_INFO("Maximal length of signal  at which bar can be joined to the track  m_maxAllowedLength = "<<m_maxAllowedLength);

  return StatusCode::SUCCESS;
}

StatusCode AFPTDBasicTool::finalize()
{
  return StatusCode::SUCCESS;
}


void AFPTDBasicTool::fillTrainWithBars()
{
  // retrieve ToF bars
  if (evtStore()->retrieve(m_hitContainer, m_hitContainerName).isSuccess()) {
    try {
      // fill station with ToF hits
      for (const xAOD::AFPToFHit* theHit : *m_hitContainer)
	if (theHit->stationID() == m_stationID) // check if hit is from the correct station
          m_trainBars[theHit->trainID()].push_back(theHit);
    }
    catch (const std::out_of_range& outOfRange) {
      ATH_MSG_WARNING("Bar with station outside expected range. Aborting track reconstruction.");
      clearTrains();
    }
  }
  else
    ATH_MSG_WARNING("Failed to retrieve ToF container.");
}


StatusCode AFPTDBasicTool::reconstructTracks(xAOD::AFPToFTrackContainer* outputContainer)
{

  // prepare list for storing  reconstructed ToF tracks

  std::list<AFPTDBasicToolTrack> reconstructedTracks;

  clearTrains();
  fillTrainWithBars();


  // ===== do tracks reconstruction =====

  for (int k=0; k<4; k++)
	{	
	int TrSize = m_trainBars[k].size();
	double TrTime = 0.;
        double weight = 0;
	int TrSat = 0;
	if ( TrSize>0 )
		{
		for (int l=0; l<TrSize; l++)
		    {
                    if ( m_trainBars[k].at(l)->pulseLength() < m_maxAllowedLength) 
		      {
                      TrTime += (m_trainBars[k].at(l)->time()-m_TimeOffset[m_trainBars[k].at(l)->barInTrainID()])*
                                m_BarWeight[m_trainBars[k].at(l)->barInTrainID()];
                                weight += m_BarWeight[m_trainBars[k].at(l)->barInTrainID()];
                      }
                    else
                      {
                      TrSat++;
                      }
		    }
		// time average
		if( TrSize!=TrSat) TrTime /= weight;					
					
      reconstructedTracks.emplace_back(m_stationID,k,TrTime, TrSize, TrSat);
      AFPTDBasicToolTrack& theTrack = reconstructedTracks.back();					
      for( int l=0; l<TrSize; l++) theTrack.addBar(m_trainBars[k].at(l));
      }
  }

  // === Save result to xAOD ===
  // save to xAOD

  for (const AFPTDBasicToolTrack& track : reconstructedTracks)
    saveToXAOD(track, outputContainer);  
  return StatusCode::SUCCESS;
}

void AFPTDBasicTool::saveToXAOD (const AFPTDBasicToolTrack& recoTrack, xAOD::AFPToFTrackContainer* containerToFill) const
{
  xAOD::AFPToFTrack* track = new xAOD::AFPToFTrack;
  containerToFill->push_back(track);
  
//  const xAOD::AFPToFHit* firstHit = recoTrack.hitInTrack().front();
    track->setStationID(recoTrack.m_stationID);
    track->setTrainID(recoTrack.m_TrainID);
//  track->setXLocal(firstPoint[0]);
//  track->setYLocal(firstPoint[2]);
//  track->setZLocal(firstCluster->zLocal());
//  track->setXSlope(firstPoint[1]);
//  track->setYSlope(firstPoint[3]);
//  track->setNClusters(recoTrack.clustersInTrack().size());
//  track->setNHoles(recoTrack.holes());
//  track->setChi2(recoTrack.trkChi2NDFSmooth());

// add links to clusters
//  ATH_MSG_DEBUG("Track position: (x="<<track->xLocal()<<", y="<<track->yLocal()<<", z="<<track->zLocal()<<")   slope: (dx="<<track->xSlope()<<", dy="<<track->ySlope()<<")   chi2="<<track->chi2());
//  for (const xAOD::AFPSiHitsCluster* theCluster : recoTrack.clustersInTrack()) {
//    ElementLink< xAOD::AFPSiHitsClusterContainer >* clusterLink = new ElementLink< xAOD::AFPSiHitsClusterContainer >; // will be taken over by the xAODCluster and deleted
//    clusterLink->toContainedElement(*m_hitsClusterContainer, theCluster);
//    track->addCluster(*clusterLink);
//
//    ATH_MSG_DEBUG("cluster position: (x="<<theCluster->xLocal()<<", y="<<theCluster->yLocal()<<", z="<<theCluster->zLocal()<<")");
//  }

}


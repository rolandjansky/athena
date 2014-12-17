/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_MUONTRACKSLIMMER_H
#define REC_MUONTRACKSLIMMER_H
/*****************************************************************************
Name    : MuonTrackSlimmer.h
Package : offline/PhysicsAnalysis/MuonID/MuonAlgs
Author  : Ketevi A. Assamagan
Created : July 2007
Note    : Based on the work by Ed Moyse who created the track slimming tool

DESCRIPTION:

This class slims Trk::Tracks. The slimmed tracks are put in the AOD. The muon 
TrackParticles in the AOD points to the slimmed Tracks. On the ESD, the muon 
TrackParticles would point to the full Trk::Track.

There are 6 muon track collections, 3 for MOORE and 3 for Muonboy, in addition
to the Inner Detector one. This algorithm creates one slimmed Track collection 
from all of those; in the process I remove the overlaps.

*****************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ObjectVector.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "muonEvent/MuonContainer.h"

#include <vector>
#include <string>

class IThinningSvc;

namespace Rec {

  /** @class MuonTrackSlimmer
      @brief Algorithm to treat the tracks associated to all muons across
             various MuonContainers, with slimming (remove redundant pars
             and material) and thinning (overlap removal and resetting trackparticle pointers)

      @author Ketevi A Assamagan
  */

  class MuonTrackSlimmer : public AthAlgorithm {
  public:
    MuonTrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonTrackSlimmer();
  
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
  
  private:

  /** Do the track slimming and fill the containers - you pass the AOD muon container to use, the
      container where to store the slimmed Track and whether to check for overlap or not */
    void doTrackSlimming ( const Analysis::MuonContainer * muonTES,
			   TrackCollection* slimmedTracks,
			   std::vector<bool>& selectedInDetTracks,
			   const bool checkOverlap,
			   const bool doSelection=false);

  /** check for overlap (through means of track pointer) and return "true" if
      there is an overlap - if the track was already processed then it will
      not be slimmed and added to the collection again */
    bool checkForOverlap ( const Trk::Track * track );

 private:
  
    /** TrackSlimmer Tool - From Ed Moyse */
    ToolHandle<Trk::ITrackSlimmingTool> m_muonTrackSlimmerTool;

    /** The thinning service */
    typedef ServiceHandle<IThinningSvc> IThinningSvc_t;
    IThinningSvc_t m_thinningSvc;

    /** Slimmed Track output container name */
    std::string m_muonSlimTrackContainerName;

    /** Inner Detector Track container to slim */
    std::string m_inDetSlimTrackContainerName;

    /** Muon container names in the AOD */
    std::vector<std::string>  m_muonContainerNames;

    /** temporay Track collection for overlap checking. It is cleared at
	the beginng of the event delete and the end of the run */
    TrackCollection * m_tempTrackCollection;

    /** CaloLikelihood cut */
    float m_caloLikelihood;

    /** option to take all refitted tracks */
    bool m_takeAllMuGirlRefittedTrack;

    /** option to slim muon tracks or not */
    bool m_doMuonTrackSlimming;
 
    /** append to an existing slimmed track collection */
    bool m_doAppendToExisting;
 
  };

}

#endif // REC_MUONTRACKSLIMMER_H

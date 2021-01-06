/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  TrigMuonEFInfoCnvTestAlg.h
//
//  Created by Mark Owen on 20/03/2014.
//

#ifndef xAODTrigMuonCnv_TrigMuonEFInfoCnvTestAlg_h
#define xAODTrigMuonCnv_TrigMuonEFInfoCnvTestAlg_h

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// xAOD includes
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODMuon/Muon.h"

// fwd declare some clases
class TrigMuonEFInfoTrack;
class TrigMuonEFTrack;

    /**
     *  @short Algorithm to test conversion of EFMuons to xAOD objects
     *
     *         This algorithm tests the results of converting
     *         from TrigMuonEFInfo objects to xAOD muons.
     *
     * @author Mark Owen <markowen@cern.ch>
     *
     */
class TrigMuonEFInfoCnvTestAlg : public AthAlgorithm {
        
    public:
        /// Regular algorithm constructor
        TrigMuonEFInfoCnvTestAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function executing the algorithm
        virtual StatusCode execute();

	/// compare extrapolated tracks
	StatusCode compareExtrapolatedTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon);
	/// compare combined tracks
	StatusCode compareCombinedTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon);
	/// compare primary tracks
	StatusCode comparePrimaryTracks(const TrigMuonEFInfoTrack* infotrk, const xAOD::Muon* muon);

	/// compare tracks
	StatusCode compareTracks(const TrigMuonEFTrack* eftrack, const xAOD::TrackParticle* xaodtrack);

    private:
        /// The key of the TrigMuonEFInfoContainer
        std::string m_aodContainerName;
        /// The key for the xAOD::MuonContainer
        std::string m_xaodMuonContainerName;
};//class TrigMuonEFInfoCnvTestAlg

#endif

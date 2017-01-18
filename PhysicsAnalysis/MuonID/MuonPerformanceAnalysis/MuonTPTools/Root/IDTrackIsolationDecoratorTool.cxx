/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IDTrackIsolationDecoratorTool.cxx
 *
 *  Created on: Sep 29, 2015
 *      Author: goblirsc
 */

#include "MuonTPTools/IDTrackIsolationDecoratorTool.h"


IDTrackIsolationDecoratorTool::IDTrackIsolationDecoratorTool(
        std::string myname) :
        AsgTool(myname),
        m_dec_ptcone40("ptcone40"),
        m_dec_ptcone30("ptcone30"),
        m_dec_ptvarcone40("ptvarcone40"),
        m_dec_ptvarcone30("ptvarcone30"),
        m_dec_topoetcone40("topoetcone40"),
        m_dec_topoetcone20("topoetcone20"),
        m_dec_etcore("etcore_correction"),

        m_ptcone40_acc("MUON_ptcone40"),
        m_ptcone30_acc("MUON_ptcone30"),
        m_ptvarcone40_acc("MUON_ptvarcone40"),
        m_ptvarcone30_acc("MUON_ptvarcone30"),
        m_topoetcone40_acc("MUON_topoetcone40"),
        m_topoetcone20_acc("MUON_topoetcone20")
        {
#ifndef XAOD_ANALYSIS
    declareProperty("TrackIsolationTool",      m_track_iso_tool);
    declareProperty("CaloIsolationTool",      m_calo_iso_tool);
#endif

}

StatusCode IDTrackIsolationDecoratorTool::initialize() {


#ifndef XAOD_ANALYSIS
    // setup the corrections to apply (taken from MUON1 derivation)
    m_trk_corr.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
    m_calo_corr.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone));
    m_calo_corr.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));

    // isolation types to run. The ptcones each also imply the respective ptvarcone.
    m_trk_isos =  {xAOD::Iso::ptcone40, xAOD::Iso::ptcone30, xAOD::Iso::ptcone20  };
    m_calo_isos =  {xAOD::Iso::topoetcone40, xAOD::Iso::topoetcone30, xAOD::Iso::topoetcone20  };

    // and retrieve the tools
    ATH_CHECK(m_track_iso_tool.retrieve());
    ATH_CHECK(m_calo_iso_tool.retrieve());
#endif
    return StatusCode::SUCCESS;

}

StatusCode IDTrackIsolationDecoratorTool::decorate(
        const xAOD::IParticle* part) const {

    static SG::AuxElement::Decorator< bool > appliedDec( "AppliedIso" );
    // remember if we already decorated a track, saves CPU time
    if (part->isAvailable<bool>("AppliedIso") && part->auxdataConst<bool>("AppliedIso")){
        ATH_MSG_DEBUG("Already decorated this track!");
        return StatusCode::SUCCESS;
    }

    // DAOD: Take the existing MUON1 decorations
//    if (m_ptvarcone30_acc.isAvailable(*part)){
//        ATH_MSG_DEBUG("Using existing isolation decoration from MUON1 DAOD");
//        try{
//            m_dec_ptcone40(*part) = m_ptcone40_acc(*part);
//            m_dec_ptcone30(*part) = m_ptcone30_acc(*part);
//            m_dec_ptvarcone40(*part) = m_ptvarcone40_acc(*part);
//            m_dec_ptvarcone30(*part) = m_ptvarcone30_acc(*part);
//            m_dec_topoetcone40(*part) = m_topoetcone40_acc(*part);
//            m_dec_topoetcone20(*part) = m_topoetcone20_acc(*part);
//            appliedDec(*part) = true;
//            return StatusCode::SUCCESS;
//        }
//        catch(SG::ExcBadAuxVar & bad){
//            ATH_MSG_DEBUG("Unable to retrieve MUON1 isolation decoration for a TrackParticle! Recomputing and -decorating...");
//            return recompute_and_decorate(part);
//        }
//    }
//    // otherwise recompute
//    else {
        return recompute_and_decorate(part);
//    }


//    return StatusCode::SUCCESS;
}

StatusCode IDTrackIsolationDecoratorTool::recompute_and_decorate(const xAOD::IParticle* part) const{

    static SG::AuxElement::Decorator< bool > appliedDec( "AppliedIso" );

    if (m_topoetcone20_acc.isAvailable(*part)){

        ATH_MSG_DEBUG("Using the DAOD isolation vars");
        appliedDec(*part) = true;
        try{
			m_dec_ptcone40(*part) = m_ptcone40_acc(*part);
			m_dec_ptcone30(*part) = m_ptcone30_acc(*part);

			m_dec_ptvarcone40(*part) = m_ptvarcone40_acc(*part);
			m_dec_ptvarcone30(*part) = m_ptvarcone30_acc(*part);

			m_dec_topoetcone40(*part) = m_topoetcone40_acc(*part);
			m_dec_topoetcone20(*part) = m_topoetcone20_acc(*part);
        }
        catch (SG::ExcBadAuxVar & ){
			ATH_MSG_WARNING("Unable to read DAOD track isolation!!");
        }
    }
    else{

#ifndef XAOD_ANALYSIS
		ATH_MSG_DEBUG("Recomputing isolation by hand");
		xAOD::TrackIsolation result;
		xAOD::CaloIsolation resultCalo;
		if (   !m_track_iso_tool->trackIsolation(result, *part, m_trk_isos,m_trk_corr) ||
				!m_calo_iso_tool->caloTopoClusterIsolation(resultCalo, *part, m_calo_isos,m_calo_corr)){
			appliedDec(*part) = false;
			ATH_MSG_WARNING("Track bitset: " << m_trk_corr.trackbitset);
			ATH_MSG_WARNING("Calo bitset: " << m_calo_corr.calobitset);
			ATH_MSG_WARNING("Unable to update ID track isolation!!");
			return StatusCode::RECOVERABLE;
		}
		appliedDec(*part) = true;
		m_dec_ptcone40(*part) = result.ptcones[0];
		m_dec_ptcone30(*part) = result.ptcones[1];

		m_dec_ptvarcone40(*part) = result.ptvarcones_10GeVDivPt[0];
		m_dec_ptvarcone30(*part) = result.ptvarcones_10GeVDivPt[1];

		m_dec_topoetcone40(*part) =resultCalo.etcones[0];
		m_dec_topoetcone20(*part) =resultCalo.etcones[2];

		m_dec_etcore(*part) = resultCalo.coreCorrections[xAOD::Iso::coreCone][xAOD::Iso::coreEnergy];
#else
		ATH_MSG_WARNING("Iso decoration can not be recomputed in AthAnalysisBase! Filling a zero dummy value");
		appliedDec(*part) = true;
		m_dec_ptcone40(*part) = 0;
		m_dec_ptcone30(*part) = 0;

		m_dec_ptvarcone40(*part) = 0;
		m_dec_ptvarcone30(*part) = 0;

		m_dec_topoetcone40(*part) = 0;
		m_dec_topoetcone20(*part) = 0;

		m_dec_etcore(*part) = 0;
#endif
    }
    return StatusCode::SUCCESS;
}

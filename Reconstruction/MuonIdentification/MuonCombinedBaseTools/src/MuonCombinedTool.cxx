/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedTool
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonCombinedTool.h"

#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/ConcurrencyFlags.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "TrkCaloExtension/CaloExtension.h"
namespace MuonCombined {

    MuonCombinedTool::MuonCombinedTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCombinedTool>(this);
    }
    void MuonCombinedTool::fill_debugging(const MuonCandidateCollection& muonCandidates,
                                          const InDetCandidateCollection& inDetCandidates) const {
        if (!m_runMuonCombinedDebugger) return;
        MuonCombinedDebuggerTool* debugger_tool ATLAS_THREAD_SAFE = const_cast<MuonCombinedDebuggerTool*>(m_muonCombDebugger.get());
        debugger_tool->fillBranches(muonCandidates, inDetCandidates);
    }
    StatusCode MuonCombinedTool::initialize() {
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_muonCombinedTagTools.retrieve());
        // debug tree, only for running with 1 thread
        if (Gaudi::Concurrency::ConcurrencyFlags::numThreads() > 1) { m_runMuonCombinedDebugger = false; }
        if (m_runMuonCombinedDebugger) {
            ATH_CHECK(m_muonCombDebugger.retrieve());
        } else
            m_muonCombDebugger.disable();

        return StatusCode::SUCCESS;
    }

    void MuonCombinedTool::combine(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates,
                                   std::vector<InDetCandidateToTagMap*> tagMaps, TrackCollection* combinedTracks, TrackCollection* METracks,
                                   const EventContext& ctx) const {
        // check that there are tracks in both systems
        if (inDetCandidates.empty()) return;
        if (muonCandidates.empty()) return;
        if (tagMaps.size() != m_muonCombinedTagTools.size()) {
            ATH_MSG_ERROR("Number of tag maps does not match number of tag tools");
            return;
        }

        // debug tree
        fill_debugging(muonCandidates, inDetCandidates);
        // loop over muon track particles
        for (const MuonCandidate* muonCandidate : muonCandidates) {
            const Trk::Track& muonTrack =
                muonCandidate->extrapolatedTrack() ? *muonCandidate->extrapolatedTrack() : muonCandidate->muonSpectrometerTrack();
            ATH_MSG_DEBUG("MuonCandidate " << m_printer->print(muonTrack) << std::endl << m_printer->printStations(muonTrack));
            // preselect ID candidates close to the muon
            std::vector<const InDetCandidate*> associatedIdCandidates;
            associate(*muonCandidate, inDetCandidates, associatedIdCandidates);
            if (associatedIdCandidates.empty()) continue;
            ATH_MSG_DEBUG("Associated ID candidates " << associatedIdCandidates.size());
            // build combined muons
            int count = 0;
            
            for (const auto& tool : m_muonCombinedTagTools) {
                tool->combine(*muonCandidate, associatedIdCandidates, *(tagMaps.at(count)), combinedTracks, METracks, ctx);
                ++count;
            }
        }
    }

    void MuonCombinedTool::associate(const MuonCandidate& muonCandidate, const InDetCandidateCollection& inDetCandidates,
                                     std::vector<const InDetCandidate*>& associatedIdCandidates) const {
        associatedIdCandidates.clear();
        for (const InDetCandidate* indetCand : inDetCandidates) {
            if (!pass_prematching(muonCandidate, *indetCand)) continue;
            associatedIdCandidates.push_back(indetCand);
        }
    }
    bool MuonCombinedTool::pass_prematching(const MuonCandidate& muonCandidate, const InDetCandidate& idCandidate) const{
        const bool hasExtr = muonCandidate.extrapolatedTrack() != nullptr;
        const Trk::Track* ms_trk =  &muonCandidate.muonSpectrometerTrack();
        const Trk::Track* msoe_trk = muonCandidate.extrapolatedTrack();
        const Trk::Track* id_trk = idCandidate.indetTrackParticle().track();
        if (!ms_trk && !msoe_trk){
            ATH_MSG_WARNING("Muon candidate without any Trk::Track");
            return false;
        }

        const Trk::TrackParameters* muonPars = (msoe_trk? msoe_trk : ms_trk)->perigeeParameters();
          if (!muonPars) {
            ATH_MSG_WARNING("MuonCandidate without Perigee, skipping");
            return false;
        }
      
        const Trk::TrackStateOnSurface* id_exit{nullptr}, *dummy{nullptr}, *ms_entrance{nullptr}, *msoe_entrance{nullptr};
        //// The MuonSystem extension tool extrapolated already the ID candidate to the MS
        const Trk::CaloExtension* calo_extension = idCandidate.getCaloExtension();
        const Trk::TrackParameters* id_extension = calo_extension ? calo_extension->muonEntryLayerIntersection() : nullptr;
        /// Use the alignment uncertainty tool to find the last ID measurement and the 
        /// first MS measurement
        if (!m_alignUncertTool.empty()){
            if (id_trk && !id_extension) m_alignUncertTool->get_track_state_measures(id_trk, id_exit, dummy, dummy, dummy);
            if (ms_trk) m_alignUncertTool->get_track_state_measures(ms_trk, dummy, dummy, dummy, ms_entrance);
            if (msoe_trk) m_alignUncertTool->get_track_state_measures(msoe_trk, dummy, dummy, dummy, msoe_entrance);
            
            if (!msoe_entrance) msoe_entrance = ms_entrance;
            if (!ms_entrance) ms_entrance = msoe_entrance;
        }
        
        /// One of the two is not found use the old approach as a fallback
        if ( (!id_exit && !id_extension) || !ms_entrance){
            /// The reason is that the IDTrackParticle does not have an associated Trk::Track. That should
            /// never happen during reconstruction 
            if (!id_trk) ATH_MSG_WARNING("Trk::Track not found for InDetTrack candidate!");
            const Amg::Vector3D muon_p3 = hasExtr?  muonPars->momentum() : muonPars->position();
            const Amg::Vector3D idtr_p3 = idCandidate.indetTrackParticle().perigeeParameters().momentum();

            const double deltaPhi = std::abs(xAOD::P4Helpers::deltaPhi(muon_p3.phi(), idtr_p3.phi()));
            if (deltaPhi > m_deltaPhiPreSelection) return false;

            const double deltaEta = std::abs(muon_p3.eta() - idtr_p3.eta());
            if (deltaEta > m_deltaEtaPreSelection) return false;

            const double muonPt = muon_p3.perp();
            const double indetPt = idtr_p3.perp();
            const double ptBal = muonPt > 0. ?  std::abs(muonPt - indetPt)/ muonPt : 1.;
            if (m_ptBalance>0. && ptBal >m_ptBalance) return false;
        } else {
            /// Retrieve the position of the ID exit and the MS entrance
            const Amg::Vector3D id_ex_pos = (id_extension ? id_extension : id_exit->trackParameters())->position();
            const Amg::Vector3D ms_en_pos = ms_entrance->trackParameters()->position();
            const Amg::Vector3D msoe_en_pos = msoe_entrance->trackParameters()->position();
            
            const double deltaPhi = std::abs(xAOD::P4Helpers::deltaPhi(id_ex_pos.phi(), ms_en_pos.phi()));
            const double deltaEta = std::abs(id_ex_pos.eta() - ms_en_pos.eta());
          
            const double deltaPhiMSOE = std::abs(xAOD::P4Helpers::deltaPhi(id_ex_pos.phi(), msoe_en_pos.phi()));
            const double deltaEtaMSOE = std::abs(id_ex_pos.eta() - msoe_en_pos.eta());
          
            /// Check that both tracklets are actually out of range. That helps us
            /// to recover tracks in the endcaps where the MSOE vertex constraint 
            /// worsens the compability
            if ( 
                (deltaPhi > m_deltaPhiPreSelection || deltaEta> m_deltaEtaPreSelection) &&
                (deltaPhiMSOE > m_deltaPhiPreSelection || deltaEtaMSOE> m_deltaEtaPreSelection)
                
                )return false;

            const double muonPt = muonPars->momentum().perp();
            const double indetPt = idCandidate.indetTrackParticle().pt();
            const double ptBal = muonPt > 0. ?  std::abs(muonPt - indetPt)/ muonPt : 1.;
            if (m_ptBalance>0. && ptBal >m_ptBalance) return false;
            
         
        }

        return true;
    }
        

}  // namespace MuonCombined

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * DiMuonTPSelectionTool.cxx
 *
 *  Created on: Jun 18, 2015
 *      Author: goblirsc
 */

#include "MuonTPTools/DiMuonTPSelectionTool.h"

DiMuonTPSelectionTool::DiMuonTPSelectionTool(std::string myname)
:  MuonTPSelectionTool(myname),
   m_muonTPextrapolator("MuonTPExtrapolationTool/MuonTPExtrapolationTool")
{
    declareProperty("UseIDProbes",      m_IDProbe = false);
    declareProperty("UseMSProbes",      m_MSProbe = false);
    declareProperty("UseCBProbes",      m_CBProbe = false);
    declareProperty("UseCaloProbes",    m_CaloProbe = false);
    declareProperty("UseTruthProbes",   m_TruthProbe = false);
    declareProperty("UseTruthMatchedProbes", m_TruthMatchedProbe = false);

    declareProperty("DoTagIsolation",     m_tag_iso=false);
    declareProperty("DoProbeIsolation",     m_probe_iso=false);

    declareProperty("DoTagAntiIsolation",     m_tag_antiiso=false);
    declareProperty("DoProbeAntiIsolation",     m_probe_antiiso=false);

    declareProperty("AcceptSameCharge",  m_accept_sameCharge = false);
    declareProperty("AcceptOppCharge",   m_accept_oppCharge = true);

    declareProperty("DeltaPhiCut",       m_deltaPhiCut = -1.00);
    declareProperty("DeltaEtaCut",       m_deltaEtaCut = -1.00);
    declareProperty("DeltaRTriggerPivot", m_deltaRTriggerPivot = 0.0);  // default to off - only decorate the value for offline use

    declareProperty("DoOnlyAside",       m_only_A_side = false);
    declareProperty("DoOnlyCside",       m_only_C_side = false);
    declareProperty("ProbeAbsEtaMin",    m_probe_abseta_min = -1.00);
    declareProperty("ProbeAbsEtaMax",    m_probe_abseta_max = 100.00);

    declareProperty("TagD0Cut",          m_tag_d0 = -1.00);
    declareProperty("TagD0SignCut",      m_tag_d0Sign = -1.00);
    declareProperty("TagZ0Cut",          m_tag_z0 = -1.00);
    declareProperty("ProbeD0Cut",        m_probe_d0 = -1.00);
    declareProperty("ProbeD0SignCut",    m_probe_d0Sign = -1.00);
    declareProperty("ProbeZ0Cut",        m_probe_z0 = -1.00);

    declareProperty("ProbeIDHitCut",     m_probe_ID_hits = false);

    declareProperty("doProbeChargeSys",  m_doProbeChargeSys = false); // Next time you turn this on by default for *EVERYONE*,
    declareProperty("ProbeCharge",       m_ProbeCharge = "positive"); // at least write an email :)

    declareProperty("UseVeryLooseProbes",m_VeryLooseProbe = false);
    declareProperty("UseLooseProbes",    m_LooseProbe     = false);
    declareProperty("UseMediumProbes",   m_MediumProbe    = false);
    declareProperty("UseTightProbes",    m_TightProbe     = false);
    declareProperty("UseHighPtProbes",   m_HighPtProbe    = false);
    declareProperty("SkipTriggerPivotPlaneDr", m_skip_dRTriggerPivotPlane=false);

    declareProperty("MuonTPExtrapolationTool", m_muonTPextrapolator);
    declareProperty("TagIsolationTool", m_tag_isotool);
    declareProperty("ProbeIsolationTool", m_probe_isotool);
}

StatusCode DiMuonTPSelectionTool::initialize()
{
    ATH_CHECK(MuonTPSelectionTool::initialize());
    // ATH_CHECK(m_muonSelectionTool.retrieve());
    //   ATH_CHECK(m_selection_tool.retrieve());
    ATH_CHECK(m_muonTPextrapolator.retrieve());
    if (m_tag_iso || m_tag_antiiso) ATH_CHECK(m_tag_isotool.retrieve());
    if (m_probe_iso || m_probe_antiiso) {
        ATH_CHECK(m_probe_isotool.retrieve());
        ATH_CHECK(m_probe_isotool->setIParticleCutsFrom(xAOD::Type::Muon));
    }
    return StatusCode::SUCCESS;
}


//**********************************************************************

ProbeContainer* DiMuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
{
    // get event weight
    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }

    float evtWeight = 1.0;
    if (info->eventType(xAOD::EventInfo::IS_SIMULATION)){
        evtWeight = info->mcEventWeight();
    }

    // prepare probe selection, log processed events
    ProbeContainer* probeCont = new ProbeContainer();
    FillCutFlows("Processed",evtWeight);

    // check GRL
    if (!passGRL(info)) return probeCont;
    FillCutFlows("GRL",evtWeight);

    bool have_tag = false;
    bool have_probe = false;
    // truth probes are selected separately
    if (m_TruthProbe){
        FillCutFlows("Trigger",evtWeight);
        have_tag = true;
        for(auto probe : *probes) {

            FillCutFlows("TagCandidates",evtWeight);
            if (!isFinalStateTruthMuon(probe)) {
                continue;
            }
            FillCutFlows("TagQuality",evtWeight);
            FillCutFlows("TagPt",evtWeight);
            FillCutFlows("TagEta",evtWeight);
            FillCutFlows("TagTrigger",evtWeight);
            FillCutFlows("TagIP",evtWeight);
            FillCutFlows("TagIsolation",evtWeight);



            FillCutFlows("ProbeCandidates",evtWeight);
            FillCutFlows("ProbeInvMass",evtWeight);
            // Here, we only need to apply the kinematic selection
            if (!PassProbeKinematics(probe)) continue;
            FillCutFlows("ProbeKinematics",evtWeight);
            FillCutFlows("ProbeQuality",evtWeight);
            FillCutFlows("ProbeIsolation",evtWeight);
            FillCutFlows("ProbeIP",evtWeight);
            FillCutFlows("ProbeCharge",evtWeight);
            FillCutFlows("ProbeDeltaPhi",evtWeight);
            FillCutFlows("ProbeDeltaEta",evtWeight);
            // in this case, fill the 'tag' slot of the probe with the probe - there is no tag for the truth efficiencies.
            probeCont->push_back( new Probe(*probe, *probe) );
            have_probe = true;
        }
    }

    // truth matched muon probes are selected separately (used for truth-closure by isolation and trigger TP)
    else if (m_TruthMatchedProbe){
        FillCutFlows("Trigger",evtWeight);
        have_tag = true;
        for(const xAOD::IParticle* probe : *probes) {

            FillCutFlows("TagCandidates",evtWeight);
            FillCutFlows("TagQuality",evtWeight);
            if (!isTruthMatched(probe)) {
                continue;
            }
            FillCutFlows("TagPt",evtWeight);
            FillCutFlows("TagEta",evtWeight);
            FillCutFlows("TagTrigger",evtWeight);
            FillCutFlows("TagIP",evtWeight);
            FillCutFlows("TagIsolation",evtWeight);

            // Add trigger matching info
            const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*> (probe);
            if(probemu) {
                this->TagTriggerMatch(probemu);
                this->TagTriggerMatch_RerunMode(probemu);
            }

            // Here, we only need to apply the kinematic selection
            if (!PassProbeKinematics(probe)) continue;

            FillCutFlows("ProbeCandidates",evtWeight);
            FillCutFlows("ProbeInvMass",evtWeight);
            FillCutFlows("ProbeKinematics",evtWeight);
            FillCutFlows("ProbeQuality",evtWeight);
            FillCutFlows("ProbeIsolation",evtWeight);
            FillCutFlows("ProbeIP",evtWeight);
            FillCutFlows("ProbeCharge",evtWeight);
            FillCutFlows("ProbeDeltaPhi",evtWeight);
            FillCutFlows("ProbeDeltaEta",evtWeight);

         // add energy deopsition in calo layers as decoration
         if(!m_is_on_DAOD && m_addCaloDeposits)
           if ( m_trk_calodep_tool->decorate(probe).isFailure() )           
          ATH_MSG_WARNING("Unable to decorare probe with calo energy deposits!");
         
            // in this case, fill the 'tag' slot of the probe with the probe - there is no tag for the truth efficiencies.
            probeCont->push_back( new Probe(*probe, *probe) );
            have_probe = true;
        }
    }

    // "Real" Tag&Probe pairs
    else {

        // Event Trigger
        if (!Event_Trigger()) return probeCont;
        FillCutFlows("Trigger",evtWeight);

        // loop over tag container
        for(auto tag : *tags) {
            FillCutFlows("TagCandidates",evtWeight);
//
//            if (tag->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon){
//                 ATH_MSG_DEBUG ("SAF Muon - primary pt "<<
//                           tag->primaryTrackParticle()->pt()<<
//                              ", pt() pt "<<tag->pt()<<
//                              ", CB pt "<<(*(tag->combinedTrackParticleLink()))->pt()<<
//                              ", ID pt "<<(*(tag->inDetTrackParticleLink()))->pt()
//                 );
//            }
//
//            if (tag->muonType() == xAOD::Muon::SegmentTagged){
//                 ATH_MSG_DEBUG ("Segment-Tagged Muon - primary pt "<<
//                           tag->primaryTrackParticle()->pt()<<
//                              ", pt() pt "<<tag->pt()<<
////                              ", CB pt "<<(*(tag->combinedTrackParticleLink()))->pt()<<
//                              ", ID pt "<<(*(tag->inDetTrackParticleLink()))->pt()
//                 );
//            }
//
//            if (tag->muonType() == xAOD::Muon::CaloTagged){
//                 ATH_MSG_DEBUG ("Calo-Tagged Muon - primary pt "<<
//                           tag->primaryTrackParticle()->pt()<<
//                              ", pt() pt "<<tag->pt()<<
////                              ", CB pt "<<(*(tag->combinedTrackParticleLink()))->pt()<<
//                              ", ID pt "<<(*(tag->inDetTrackParticleLink()))->pt()
//                 );
//            }

            // select good muons (combined for the time being)
            // A muon selection tool should be used, but the tool
            // should be ASG dual-tool
            if (m_selection_tool->getQuality(*tag) > xAOD::Muon::Medium)  continue;
            // DAOD: Tag *must* be combined muon
            if (tag->muonType() != xAOD::Muon::Combined) continue;
            if (!m_selection_tool->passedIDCuts(*tag)) continue;
            FillCutFlows("TagQuality",evtWeight);

            if(tag->pt() < m_tagPtCut) continue;
            FillCutFlows("TagPt",evtWeight);

            if(fabs(tag->eta()) > m_tagEtaCut) continue;
            FillCutFlows("TagEta",evtWeight);

            if(!this->TagTriggerMatch(tag)) continue;
            FillCutFlows("TagTrigger",evtWeight);

            // Fill rerun triggerd decision bits
            this->TagTriggerMatch_RerunMode(tag);

            if (!PassTagIPCuts(tag)) continue;
            FillCutFlows("TagIP",evtWeight);

            if (!PassTagIsoCuts(tag)) continue;
            FillCutFlows("TagIsolation",evtWeight);

            have_tag = true;
            // for each selected tag, loop over probes
            for(auto probe : *probes) {

                FillCutFlows("ProbeCandidates",evtWeight);

                // Cut on tag-probe invariant mass
                double mtp = (tag->p4()+probe->p4()).M();
                if(mtp<m_lowMassWindow || mtp>m_highMassWindow) continue;
                FillCutFlows("ProbeInvMass",evtWeight);

                // Cut on probe eta, pT
                if (!PassProbeKinematics(probe)) continue;
                FillCutFlows("ProbeKinematics",evtWeight);

                // select good probe tracks,
                // again a selecton ASG duel-tool could be used
                // tba
                // check the probe type
                if (!isRightType(probe)) continue;

                // remove the probe track matched to the tag
                if(isTag(tag, probe)) continue;
                // ID hits
                const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*> (probe);
                if (m_probe_ID_hits && probemu) {
                    if (!m_selection_tool->passedIDCuts(*probemu)) continue;
                }
                if (m_IDProbe && m_probe_ID_hits){
                    const xAOD::TrackParticle *trk = dynamic_cast<const xAOD::TrackParticle*>(probe);
                    if (!m_selection_tool->passedIDCuts(*trk)) continue;
                }
                FillCutFlows("ProbeQuality",evtWeight);

                if (!PassProbeIsoCuts(probe)) continue;
                FillCutFlows("ProbeIsolation",evtWeight);

                if (!PassProbeIPCuts(probe)) continue;
                FillCutFlows("ProbeIP",evtWeight);

                if(m_doProbeChargeSys)
                {
                    if(m_ProbeCharge=="positive")
                    {
                        // case of a muon probe
                        const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
                        if (muprobe && (muprobe->trackParticle(xAOD::Muon::Primary)->charge() < 0) ) continue;

                        // case of an ID probe
                        const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(probe);
                        if (trkprobe && (trkprobe->charge() < 0) )  continue;
                    }
                    if(m_ProbeCharge=="negative")
                    {
                        // case of a muon probe
                        const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
                        if (muprobe && (muprobe->trackParticle(xAOD::Muon::Primary)->charge() > 0) ) continue;

                        // case of an ID probe
                        const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(probe);
                        if (trkprobe && (trkprobe->charge() > 0) )  continue;
                    }
                }

                // Charge cut
                //if(m_oppositeCharge &&
                //      tag->trackParticle(xAOD::Muon::Primary)->charge()
                int cp = ChargeProd(tag,probe);
                if (cp == 1 && !m_accept_sameCharge) continue;
                if (cp == -1 && !m_accept_oppCharge) continue;
                FillCutFlows("ProbeCharge",evtWeight);    // does this work?
                //         if (m_MSProbe && m_accept_oppCharge && m_probe_antiEtConeIso < 0 && !probemu){
                //             const xAOD::TrackParticle* mutrk = dynamic_cast <const xAOD::TrackParticle*> (probe);
                //             uint8_t nPhiHits = 0;
                //             uint8_t nLayers = 0;
                //             uint8_t nHoleLayers = 0;
                //             uint8_t nPhiHoleLayers = 0;
                //             if (mutrk
                //                 && mutrk->summaryValue(nPhiHits,xAOD::numberOfPhiLayers)
                //                 && mutrk->summaryValue(nLayers,xAOD::numberOfPrecisionLayers)
                //                 && mutrk->summaryValue(nHoleLayers,xAOD::numberOfPrecisionHoleLayers)
                //                 && mutrk->summaryValue(nPhiHoleLayers,xAOD::numberOfPhiHoleLayers)
                //             )
                //             ATH_MSG_WARNING("MS probe has "<<(int)nPhiHits<<" phi hits, "<<(int)nLayers<<" precision layers, "<<(int)nHoleLayers<<" precision holes and "<< (int)nPhiHoleLayers<<" Phi holes");
                //         }

                // Delta Phi cut
                if (DeltaPhiTP(tag,probe) < m_deltaPhiCut) continue;
                FillCutFlows("ProbeDeltaPhi",evtWeight);

                // Delta Eta cut
                if (fabs(tag->eta()-probe->eta()) < m_deltaEtaCut) continue;
                FillCutFlows("ProbeDeltaEta",evtWeight);

                // Cut on probe-tag dR in trigger pivot plane
                // Shift this back in the selection to save CPU time (expensive!)
                if (!m_skip_dRTriggerPivotPlane){
                    float dR = m_muonTPextrapolator->dROnTriggerPivotPlane(*tag, probe);
                    // dR < 0 is assigned when an ID probe does not make it into the MS.
                    // In this case, we can safely accept the probe since there can 
                    // be no trigger bias 
                    if (dR >= 0 && dR < m_deltaRTriggerPivot ) continue;
                    // note to keen-eyed svn diff readers: Yes, the sign of the cut switched in this commit.
                    // It should have been a "<" all the time (minimum separation)....
                    // Good that no one noticed the last 2 years. 
                }
                FillCutFlows("ProbeDR_exTP",evtWeight);


          // add energy deopsition in calo layers as decoration
          if(!m_is_on_DAOD && m_addCaloDeposits)
            if ( m_trk_calodep_tool->decorate(probe).isFailure() )
              ATH_MSG_WARNING("Unable to decorare probe with calo energy deposits!");
          
                // for each selected probe build a Probe object
                probeCont->push_back( new Probe(*tag, *probe) );
                have_probe = true;
            }
        }
    }
    ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

    if (have_tag) FillCutFlows("HaveTag",evtWeight);
    if (have_probe) FillCutFlows("HaveProbe",evtWeight);
    return probeCont;
}
//**********************************************************************



bool DiMuonTPSelectionTool::PassTagIsoCuts (const xAOD::Muon* tag) const{
    if (m_tag_iso){
        return  (m_tag_isotool->accept(*tag));
    }
    else if (m_tag_antiiso){
        return (!m_tag_isotool->accept(*tag));
    }
    else {
        return true;
    }
}

bool DiMuonTPSelectionTool::PassProbeIsoCuts (const xAOD::IParticle* probe) const{
    const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*>(probe);
    // ID probes: check ptcone 
    if (!probemu){
        ATH_CHECK(m_trk_iso_tool->decorate(probe));
    }
    if (m_probe_iso){
        if (!probemu){
            return  (m_probe_isotool->accept(*probe));
//            return (ptcone_rel < 0.06 && etcone_rel < 0.2);
        }
        if (probemu->author() == xAOD::Muon::MuidSA){
            ATH_MSG_DEBUG("Standalone muon. Outcome is "<<m_probe_isotool->accept(*probemu));
        }
        return  (m_probe_isotool->accept(*probemu));
    }
    else if (m_probe_antiiso){
        if (!probemu){
            return  (!m_probe_isotool->accept(*probe));
//            return (ptcone_rel > 0.35 || etcone_rel > 1.0);
        }
        return (!m_probe_isotool->accept(*probemu));
    }
    else {
        return true;
    }
}


bool DiMuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
    return (tag->p4().DeltaR(probe->p4()) < 0.01);
}

bool DiMuonTPSelectionTool::PassProbeIPCuts(const xAOD::IParticle* probe) const{

    const xAOD::TrackParticle* tp = 0;
    const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
    if (muprobe) {
        tp = muprobe->primaryTrackParticle();
    }
    else {
        tp = dynamic_cast<const xAOD::TrackParticle*>(probe);
    }
    if (!tp){
        ATH_MSG_WARNING("Unable to find a track particle to cut on!");
    }
    return PassIPCuts(tp,m_probe_d0, m_probe_d0Sign, m_probe_z0);
}
bool DiMuonTPSelectionTool::PassTagIPCuts(const xAOD::Muon* tag) const{

    const xAOD::TrackParticle* tp = tag->primaryTrackParticle();
    if (!tp){
        ATH_MSG_WARNING("Unable to find the tag muon's track particle to cut on!");
    }
    return PassIPCuts(tp,m_tag_d0, m_tag_d0Sign, m_tag_z0);
}

bool DiMuonTPSelectionTool::isRightType(const xAOD::IParticle* probe) const{

    const xAOD::Muon* muprobe = dynamic_cast <const xAOD::Muon*> (probe);

    // if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
    //  This covers the ID and MS probe use cases
    if (! muprobe) return true;

    // otherwise, we have to manually pick the right probe

    // for VeryLoose/Loose/Medium/Tight selection
    if (m_VeryLooseProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::VeryLoose);
    }
    if (m_LooseProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Loose);
    }
    if (m_MediumProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Medium);
    }
    if (m_TightProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Tight);
    }
    if (m_HighPtProbe)
    {
        return (muprobe && m_selection_tool->passedHighPtCuts(*muprobe));
    }


    // MS probe if we are running on the muon collection
    if (m_MSProbe){
        //         static int n_zerophi = 0;
        //         uint8_t naux = 0;
        //         if (muprobe->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)){
        //             if (!muprobe->summaryValue(naux, xAOD::numberOfPhiLayers) ||  naux < 1 || !muprobe->summaryValue(naux, xAOD::numberOfPrecisionHoleLayers) || naux > 0 ){
        //                 n_zerophi++;
        //                 ATH_MSG_WARNING("Found a low phi hit probe. This is number "<<n_zerophi);
        //                 return false;
        //             }
        //         }
        return (muprobe &&  muprobe->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle)!=0);
    }
    // CT Probe
    if (m_CaloProbe){
        return (muprobe &&  muprobe->isAuthor(xAOD::Muon::CaloTag) && m_selection_tool->passedCaloTagQuality(*muprobe));
    }
    // CB probe
    if (m_CBProbe){
        return (muprobe->muonType() == xAOD::Muon::Combined && muprobe->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
    }
    return false;

}

bool DiMuonTPSelectionTool::PassProbeKinematics (const xAOD::IParticle* probe) const{

    if(probe->pt() < m_probePtCut) return false;
    if(fabs(probe->eta()) > m_probeEtaCut) return false;
    if (fabs(probe->eta()) < m_probe_abseta_min) return false;
    if (fabs(probe->eta()) > m_probe_abseta_max) return false;
    if (m_only_A_side && probe->eta() < 0) return false;
    if (m_only_C_side && probe->eta() > 0) return false;
    return true;
}

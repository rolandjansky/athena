/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/IBTagTrackAssociation.h"

#include "xAODBTagging/BTagging.h"

#include "xAODJet/Jet.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "ParticleJetTools/ParticleToJetAssociator.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"


namespace Analysis {

    BTagTrackAssociation::BTagTrackAssociation(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p)
    {
        declareInterface<IBTagTrackAssociation>(this);

        // if this option is set, run tagging on the named collection
        // already attached to the jet instead of performing the
        // association.
        declareProperty("AssociatedTrackLinks", m_associatedTrackLinks=std::string());
        declareProperty("AssociatedMuonLinks", m_associatedMuonLinks=std::string());
        declareProperty("TrackContainerName", m_trackContainerName=std::string());
        declareProperty("MuonContainerName", m_muonContainerName=std::string());
        declareProperty("TrackAssociationName", m_trackAssociationName=std::string());
        declareProperty("MuonAssociationName", m_muonAssociationName=std::string());
    }

    BTagTrackAssociation::~BTagTrackAssociation() {
    }

    StatusCode BTagTrackAssociation::initialize()
    {
        return StatusCode::SUCCESS;
    }

    StatusCode BTagTrackAssociation::finalize()
    {
        return StatusCode::SUCCESS;
    }

    StatusCode BTagTrackAssociation::BTagTrackAssociation_exec(jetcollection_t* theJets) const {

        StatusCode sc;

        for (const xAOD::Jet* jet : *theJets) {
            xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(jet->btagging());
            if (!tagInfo) {
                ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
                return StatusCode::FAILURE;
            }

            // track association
            SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > acctrack(m_associatedTrackLinks);
            std::vector< ElementLink< xAOD::IParticleContainer > > tmptrks = acctrack(*jet);

            const xAOD::TrackParticleContainer * tpContainer(0);
            sc = evtStore()->retrieve( tpContainer, m_trackContainerName);
            if ( sc.isFailure() || tpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles through name: " << m_trackContainerName);
                return StatusCode::FAILURE;
            }

            // we have to convert the IParticle*s to TrackParticle*s
            std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks;
            for( const auto& link : tmptrks ) {
                const xAOD::IParticle* ipart = *link;

                if( ipart->type() != xAOD::Type::TrackParticle ) {
                    ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::TrackParticle ");
                }
                const xAOD::TrackParticle* tpart = static_cast< const xAOD::TrackParticle* >( ipart );
                ElementLink<xAOD::TrackParticleContainer> EL;
                EL.toContainedElement(*tpContainer, tpart);
                tpLinks.push_back(EL);
            }

            tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_trackAssociationName) = tpLinks;

            ATH_MSG_INFO("Attached tracks to jet as " + m_trackAssociationName);

            // muon association
            // this is optional. currently (2018 03 21) online b-tagging has no
            // muons, so we continue if there is no muon container name passed.
            if (m_muonContainerName == "") {
              ATH_MSG_WARNING("#BTAG# no muon container name given to the association tool. this is interpreted to mean no muon association should take place.");
              continue;
            }

            SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > accmu(m_associatedMuonLinks);
            std::vector< ElementLink< xAOD::IParticleContainer > > tmpmus = accmu(*jet);


            const xAOD::MuonContainer * mpContainer( 0 );
            sc = evtStore()->retrieve( mpContainer, m_muonContainerName);
            if ( sc.isFailure() || mpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons through name: " << m_muonContainerName);
                return StatusCode::FAILURE;
            }

            // we have to convert the IParticle*s to Muon*s
            std::vector< ElementLink< xAOD::MuonContainer > > mpLinks;
            for( const auto& link : tmpmus ) {
                const xAOD::IParticle* ipart = *link;
                if( ipart->type() != xAOD::Type::Muon ) {
                    ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::Muon ");
                }

                const xAOD::Muon* tpart = static_cast< const xAOD::Muon* >( ipart );
                ElementLink<xAOD::MuonContainer> EL;
                EL.toContainedElement(*mpContainer, tpart);
                mpLinks.push_back(EL);
            }

            tagInfo->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_muonAssociationName) = mpLinks;

            ATH_MSG_INFO("Attached mouns to jet as " + m_muonAssociationName);

            continue;
        }

        return StatusCode::SUCCESS;
    }

} // namespace

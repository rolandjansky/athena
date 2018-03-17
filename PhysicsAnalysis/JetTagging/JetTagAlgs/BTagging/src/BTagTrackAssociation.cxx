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
        declareProperty("AssociatedTrackLinks", m_AssociatedTrackLinks=std::string());
        declareProperty("AssociatedMuonLinks", m_AssociatedMuonLinks=std::string());
        declareProperty("TrackContainerName", m_TrackContainerName=std::string());
        declareProperty("MuonContainerName", m_MuonContainerName=std::string());
        declareProperty("TrackAssociationName", m_TrackAssociationName=std::string());
        declareProperty("MuonAssociationName", m_MuonAssociationName=std::string());
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
            SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > acctrack(m_AssociatedTrackLinks);
            std::vector< ElementLink< xAOD::IParticleContainer > > tmptrks = acctrack(*jet);

            const xAOD::TrackParticleContainer * tpContainer(0);
            sc = evtStore()->retrieve( tpContainer, m_TrackContainerName);
            if ( sc.isFailure() || tpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles through name: " << m_TrackContainerName);
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

            tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_TrackAssociationName) = tpLinks;

            ATH_MSG_INFO("Attached tracks to jet as " + m_TrackAssociationName);

            // muon association
            SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer> > > accmu(m_AssociatedMuonLinks);
            std::vector< ElementLink< xAOD::IParticleContainer > > tmpmus = accmu(*jet);

            const xAOD::MuonContainer * mpContainer( 0 );
            sc = evtStore()->retrieve( mpContainer, m_MuonContainerName);
            if ( sc.isFailure() || mpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons through name: " << m_MuonContainerName);
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

            tagInfo->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_MuonAssociationName) = mpLinks;

            ATH_MSG_INFO("Attached mouns to jet as " + m_MuonAssociationName);
        }

        return StatusCode::SUCCESS;
    }

} // namespace

/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/IBTagTrackAssociation.h"

#include "xAODBTagging/BTagging.h"

#include "xAODJet/Jet.h"
// #include "JetEvent/JetCollection.h"

#include "xAODTracking/TrackParticleContainer.h"
// #include "Particle/TrackParticle.h"
//#include "Particle/TrackParticleContainer.h"

#include "ParticleJetTools/ParticleToJetAssociator.h"

// // // For Soft Lepton Tag
// #include "egammaEvent/ElectronAssociation.h"
// #include "egammaEvent/PhotonAssociation.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"


namespace Analysis {

    BTagTrackAssociation::BTagTrackAssociation(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p),
        m_BTagAssociation(true)
    {
        declareInterface<IBTagTrackAssociation>(this);
        declareProperty( "BTagAssociation",                     m_BTagAssociation);

        declareProperty( "TrackToJetAssociatorList",            m_TrackToJetAssociatorList);
        declareProperty( "ElectronToJetAssociatorList",         m_ElectronToJetAssociatorList );
        declareProperty( "MuonToJetAssociatorList",             m_MuonToJetAssociatorList );

        declareProperty( "TrackToJetAssocNameList",             m_TrackToJetAssocNameList);
        declareProperty( "ElectronToJetAssocNameList",          m_ElectronToJetAssocNameList);
        declareProperty( "PhotonToJetAssocNameList",            m_PhotonToJetAssocNameList);
        declareProperty( "MuonToJetAssocNameList",              m_MuonToJetAssocNameList);

        declareProperty( "TrackContainerNameList",              m_TrackContainerNameList);
        declareProperty( "ElectronContainerNameList",           m_ElectronContainerNameList ); //for SoftEl
        declareProperty( "PhotonContainerNameList",             m_PhotonContainerNameList);   //for SoftEl
        declareProperty( "MuonContainerNameList",               m_MuonContainerNameList );     //for SoftMu


        // if this option is set, run tagging on the named collection
        // already attached to the jet instead of performing the
        // association.
        declareProperty( "ParticlesToAssociateList",            m_ParticlesToAssociateList=std::vector<std::string>() );
        declareProperty( "MuonsToAssociateList",            m_MuonsToAssociateList=std::vector<std::string>() );
    }

    BTagTrackAssociation::~BTagTrackAssociation() {
    }

    StatusCode BTagTrackAssociation::initialize()
    {

        /* ----------------------------------------------------------------------------------- */
        /*                        RETRIEVE SERVICES FROM STOREGATE                             */
        /* ----------------------------------------------------------------------------------- */

        if(m_TrackToJetAssociatorList.size() != m_TrackToJetAssocNameList.size()){
            ATH_MSG_FATAL("Size mismatch between track assoc tools("
                    << m_TrackToJetAssociatorList.size() << ") and names( "
                    << m_TrackToJetAssocNameList.size() << ")");
            return StatusCode::FAILURE;
        }
        if(m_TrackToJetAssociatorList.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tools " << m_TrackToJetAssociatorList);
            return StatusCode::FAILURE;
        } else {
            if (m_TrackToJetAssociatorList.size() == 0) {
                ATH_MSG_WARNING(" No tool for track to jet association");
            }
            else {
                ATH_MSG_INFO("Retrieved tools " << m_TrackToJetAssociatorList);
            }
        }

        if(m_MuonToJetAssociatorList.size() != m_MuonContainerNameList.size()){
            ATH_MSG_FATAL("Size mismatch between muon assoc tools("
                    << m_MuonToJetAssociatorList.size() << ") and names( "
                    << m_MuonContainerNameList.size() << ")");
            return StatusCode::FAILURE;
        }
        if(m_MuonToJetAssociatorList.retrieve().isFailure() ) {
            ATH_MSG_FATAL("Failed to retrieve tools " << m_MuonToJetAssociatorList);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Retrieved tools " << m_MuonToJetAssociatorList);
        }


        return StatusCode::SUCCESS;
    }

    StatusCode BTagTrackAssociation::finalize()
    {
        return StatusCode::SUCCESS;
    }

    StatusCode BTagTrackAssociation::BTagTrackAssociation_exec(jetcollection_t* theJets, const xAOD::TrackParticleContainer* tracks) const {

        /* ----------------------------------------------------------------------------------- */
        /*               Particle to Jet Associations                                          */
        /* ----------------------------------------------------------------------------------- */

        // track association
        StatusCode sc;
        int i = 0;
        for (const std::string& trackColName : m_ParticlesToAssociateList) {

            for (const xAOD::Jet* jet : *theJets) {
                xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(jet->btagging());
                if (!tagInfo) {
                    ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
                    return StatusCode::FAILURE;
                }

                std::vector< ElementLink< xAOD::IParticleContainer > > tmp;
                if (!jet->getAttribute(trackColName, tmp)) {
                    ATH_MSG_FATAL("Unable to read track collection " + trackColName + " from jets for b-tagging.");
                    return StatusCode::FAILURE;
                }

                std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
                const xAOD::TrackParticleContainer * tpContainer( 0 );
                sc = evtStore()->retrieve( tpContainer, m_TrackContainerNameList[i]);
                if ( sc.isFailure() || tpContainer==0) {
                    ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles through name: " << m_TrackContainerNameList[i]);
                    return StatusCode::FAILURE;
                }

                for( const auto& link : tmp ) {
                    const xAOD::IParticle* ipart = *link;

                    if( ipart->type() != xAOD::Type::TrackParticle ) {
                        ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::TrackParticle ");
                    }
                    const xAOD::TrackParticle* tpart = static_cast< const xAOD::TrackParticle* >( ipart );
                    ElementLink<xAOD::TrackParticleContainer> EL;
                    EL.toContainedElement(*tpContainer, tpart);
                    associationLinks.push_back(EL);

                }

                tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(m_TrackToJetAssocNameList[i]) = associationLinks;
            }

            i++;
        }

        // muon association
        i = 0;
        for (const std::string& muonColName : m_MuonsToAssociateList) {

            for (const xAOD::Jet* jet : *theJets) {
                xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>(jet->btagging());
                if (!tagInfo) {
                    ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
                    return StatusCode::FAILURE;
                }

                std::vector< ElementLink< xAOD::IParticleContainer > > tmp;
                if (!jet->getAttribute(muonColName, tmp)) {
                    ATH_MSG_FATAL("Unable to read track collection " + muonColName + " from jets for b-tagging.");
                    return StatusCode::FAILURE;
                }

                std::vector< ElementLink< xAOD::MuonContainer > > associationLinks;

                const xAOD::MuonContainer * tpContainer( 0 );
                sc = evtStore()->retrieve( tpContainer, m_MuonContainerNameList[i]);
                if ( sc.isFailure() || tpContainer==0) {
                    ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons through name: " << m_MuonContainerNameList[i]);
                    return StatusCode::SUCCESS;
                }

                for( const auto& link : tmp ) {
                    const xAOD::IParticle* ipart = *link;
                    if( ipart->type() != xAOD::Type::Muon ) {
                        ATH_MSG_ERROR("#BTAG# Failed to change xAOD::IParticle type to xAOD::Type::Muon ");
                    }
                    const xAOD::Muon* tpart = static_cast< const xAOD::Muon* >( ipart );
                    ElementLink<xAOD::MuonContainer> EL;
                    EL.toContainedElement(*tpContainer, tpart);
                    associationLinks.push_back(EL);
                }

                tagInfo->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_MuonToJetAssocNameList[i]) = associationLinks;

            }

            i++;
        }


        return sc;
    }

} // namespace

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
    using namespace xAOD;

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

        for (const Jet* jet : *theJets) {
            // We need a const_cast here, as the Jet does not have a method providing a pointer to a non-const BTagging object
            // A *much* cleaner solution would be to ask the ElementLink (btaggingLink())
            // for the container name and index of the BTagging object, retrieve
            // the container from StoreGate using a non-const pointer, and get the
            // non-const pointer to the BTagging object like that. That would keep
            // const-correctness intact, as StoreGate could tell us whether it's allowed
            // to modify the BTagging object or not.
            BTagging* tagInfo = const_cast<BTagging*>(jet->btagging());
            if (!tagInfo) {
                ATH_MSG_FATAL("The pointer from Jet to BTagging object is invalid");
                return StatusCode::FAILURE;
            }

            // track association
            SG::AuxElement::ConstAccessor<std::vector<ElementLink<IParticleContainer> > > acctrack(m_associatedTrackLinks);
            std::vector< ElementLink< IParticleContainer > > tmptrks = acctrack(*jet);

            const TrackParticleContainer * tpContainer(0);
            sc = evtStore()->retrieve( tpContainer, m_trackContainerName);
            if ( sc.isFailure() || tpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles through name: " << m_trackContainerName);
                return StatusCode::FAILURE;
            }

            // we have to convert the IParticle*s to TrackParticle*s
            std::vector< ElementLink< TrackParticleContainer > > tpLinks;
            for( const auto& link : tmptrks ) {
                const IParticle* ipart = *link;

                if( ipart->type() != Type::TrackParticle ) {
                    ATH_MSG_ERROR("#BTAG# Failed to change IParticle type to Type::TrackParticle ");
                }
                const TrackParticle* tpart = static_cast< const TrackParticle* >( ipart );
                ElementLink<TrackParticleContainer> EL;
                EL.toContainedElement(*tpContainer, tpart);
                tpLinks.push_back(EL);
            }

            tagInfo->auxdata<std::vector<ElementLink<TrackParticleContainer> > >(m_trackAssociationName) = tpLinks;

            ATH_MSG_INFO("Attached tracks to jet as " + m_trackAssociationName);

            // muon association
            // this is optional. currently (2018 03 21) online b-tagging has no
            // muons, so we continue if there is no muon container name passed.
            if (m_muonContainerName == "") {
              ATH_MSG_WARNING("#BTAG# no muon container name given to the association tool. this is interpreted to mean no muon association should take place.");
              continue;
            }

            SG::AuxElement::ConstAccessor<std::vector<ElementLink<IParticleContainer> > > accmu(m_associatedMuonLinks);
            std::vector< ElementLink< IParticleContainer > > tmpmus = accmu(*jet);


            const MuonContainer * mpContainer( 0 );
            sc = evtStore()->retrieve( mpContainer, m_muonContainerName);
            if ( sc.isFailure() || mpContainer==0) {
                ATH_MSG_ERROR("#BTAG# Failed to retrieve Muons through name: " << m_muonContainerName);
                return StatusCode::FAILURE;
            }

            // we have to convert the IParticle*s to Muon*s
            std::vector< ElementLink< MuonContainer > > mpLinks;
            for( const auto& link : tmpmus ) {
                const IParticle* ipart = *link;
                if( ipart->type() != Type::Muon ) {
                    ATH_MSG_ERROR("#BTAG# Failed to change IParticle type to Type::Muon ");
                }

                // explicit xAOD namespace due to ambiguity
                const xAOD::Muon* tpart = static_cast< const xAOD::Muon* >( ipart );
                ElementLink<MuonContainer> EL;
                EL.toContainedElement(*mpContainer, tpart);
                mpLinks.push_back(EL);
            }

            tagInfo->auxdata<std::vector<ElementLink<MuonContainer> > >(m_muonAssociationName) = mpLinks;

            ATH_MSG_INFO("Attached mouns to jet as " + m_muonAssociationName);

            continue;
        }

        return StatusCode::SUCCESS;
    }

} // namespace

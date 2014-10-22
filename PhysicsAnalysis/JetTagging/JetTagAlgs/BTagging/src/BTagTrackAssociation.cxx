/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/IBTagTrackAssociation.h"

#include "xAODBTagging/BTagging.h"

#include "xAODJet/Jet.h"
// #include "JetEvent/JetCollection.h"
// #include "JetTagEvent/TrackAssociation.h"

#include "xAODTracking/TrackParticleContainer.h"
// #include "Particle/TrackParticle.h" 
//#include "Particle/TrackParticleContainer.h"

#include "ParticleJetTools/ParticleToJetAssociator.h"

// // // For Soft Lepton Tag
// #include "egammaEvent/ElectronAssociation.h"
// #include "MuonIDEvent/MuonAssociation.h"
// #include "egammaEvent/PhotonAssociation.h"

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
  }

  BTagTrackAssociation::~BTagTrackAssociation() {
  }

  StatusCode BTagTrackAssociation::initialize()
  {

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if(m_TrackToJetAssociatorList.size() != m_TrackToJetAssocNameList.size()){
      msg( MSG::FATAL ) << "Size mismatch between track assoc tools("
			<<m_TrackToJetAssociatorList.size()<<") and names( "
			<<m_TrackToJetAssocNameList.size()<<")"  << endreq;
      return StatusCode::FAILURE;
    }
    if(m_TrackToJetAssociatorList.retrieve().isFailure() ) {
      msg( MSG::FATAL ) << "Failed to retrieve tools " << m_TrackToJetAssociatorList << endreq;
      return StatusCode::FAILURE;
    } else {
      if (m_TrackToJetAssociatorList.size() == 0) {
	msg( MSG::ERROR ) << " No tool for track to jet association" << endreq;
      }
      else {
        msg( MSG::INFO ) << "Retrieved tools " << m_TrackToJetAssociatorList << endreq;
      }
    }

    // FF: comment out everything but the bare track association for now
    // if(2*m_ElectronToJetAssociatorList.size() != m_ElectronContainerNameList.size() + m_PhotonContainerNameList.size()){ // because use the same tool for electrons and photons
    //   msg( MSG::FATAL ) << "Size mismatch between electron/photon assoc tools("
    // 			<<m_ElectronToJetAssociatorList.size()<<") and names( "
    // 			<<m_ElectronContainerNameList.size()<< "+" << m_PhotonContainerNameList.size()<<  ")"  << endreq;
    //   return StatusCode::FAILURE;
    // }
    // if(m_ElectronToJetAssociatorList.retrieve().isFailure() ) {
    //   msg( MSG::FATAL ) << "Failed to retrieve tools " << m_ElectronToJetAssociatorList << endreq;
    //   return StatusCode::FAILURE;
    // } else {
    //   msg( MSG::INFO ) << "Retrieved tools " << m_ElectronToJetAssociatorList << endreq;
    // }

    // if(m_MuonToJetAssociatorList.size() != m_MuonContainerNameList.size()){
    //   msg( MSG::FATAL ) << "Size mismatch between muon assoc tools("
    // 			<<m_MuonToJetAssociatorList.size()<<") and names( "
    // 			<<m_MuonContainerNameList.size()<<")"  << endreq;
    //   return StatusCode::FAILURE;
    // }
    // if(m_MuonToJetAssociatorList.retrieve().isFailure() ) {
    //   msg( MSG::FATAL ) << "Failed to retrieve tools " << m_MuonToJetAssociatorList << endreq;
    //   return StatusCode::FAILURE;
    // } else {
    //   msg( MSG::INFO ) << "Retrieved tools " << m_MuonToJetAssociatorList << endreq;
    // }

    return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAssociation::finalize()
  {
      return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAssociation::BTagTrackAssociation_exec(jetcollection_t* theJets) {
  //StatusCode BTagTrackAssociation::BTagTrackAssociation_exec(std::vector<xAOD::Jet*>* theJets) {

    /* ----------------------------------------------------------------------------------- */
    /*               Particle to Jet Associations                                          */
    /* ----------------------------------------------------------------------------------- */

    StatusCode sc;
    // ----- associate tracks
    const xAOD::TrackParticleContainer * tpContainer( 0 );
    ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator tAssocIter = m_TrackToJetAssociatorList.begin();
    ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator tAssocEnd  = m_TrackToJetAssociatorList.end();
    std::vector< std::string >::iterator tNameIter = m_TrackContainerNameList.begin();
    std::vector< std::string >::iterator tAssocNameIter = m_TrackToJetAssocNameList.begin();

    //if (MyjetBasis == "Cells" || (MyjetBasis == "Tracks" && m_BTagAssociation) ) {
      for (; tAssocIter!=tAssocEnd; ++tAssocIter) {
        sc = evtStore()->retrieve( tpContainer, *tNameIter );
        if ( sc.isFailure() || tpContainer==0) {
          ATH_MSG_ERROR("#BTAG# Failed to retrieve TrackParticles " << *tNameIter);
          return StatusCode::SUCCESS;
        }
        ATH_MSG_VERBOSE("#BTAG# Number of TrackParticles in event: " << tpContainer->size());

	// compute the associations
	// NB we are doing away with the Analysis::TrackAssociation class for this.
	// std::vector<Analysis::TrackAssociation> assocs = 
	//   (*tAssocIter)->associateParticlesToJets<Analysis::TrackAssociation, xAOD::TrackParticleContainer>( theJets, tpContainer, *tAssocNameIter );
	std::vector<std::vector<const xAOD::TrackParticle*>*> assocs = 
	  (*tAssocIter)->associateParticlesToJets<std::vector<const xAOD::TrackParticle*>, xAOD::TrackParticleContainer>( theJets, tpContainer, *tAssocNameIter );

	// then store them in the BTagging objects. Note that for this to work, the ElementLink from Jet to BTagging object must exist
        unsigned int i = 0;
	for (jetcollection_t::iterator jetIter = theJets->begin(); jetIter != theJets->end(); ++jetIter) {
	  // We need a const_cast here, as the Jet does not have a method providing a pointer to a non-const BTagging object
	  // A *much* cleaner solution would be to ask the ElementLink (btaggingLink())
          // for the container name and index of the BTagging object, retrieve
          // the container from StoreGate using a non-const pointer, and get the
          // non-const pointer to the BTagging object like that. That would keep
          // const-correctness intact, as StoreGate could tell us whether it's allowed
          // to modify the BTagging object or not.
          xAOD::BTagging* tagInfo = const_cast<xAOD::BTagging*>((*jetIter)->btagging());
	  std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
	  for (std::vector<const xAOD::TrackParticle*>::const_iterator trkIter = assocs[i]->begin(); trkIter != assocs[i]->end(); ++trkIter) {
	    ElementLink<xAOD::TrackParticleContainer> EL;
	    EL.toContainedElement(*tpContainer, *trkIter);
	    associationLinks.push_back(EL);
	  }
	  tagInfo->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(*tAssocNameIter) = associationLinks;
          ++i;
	}
        //delete pointer created in associateParticlesToJets
        for (i=0; i < assocs.size(); i++) {
	  delete assocs[i];
        }
        ++tNameIter;
        ++tAssocNameIter;
      }

//    } else {
//      int nTotalTrackAssoc = 0;
//
//      for ( jetcollection_t::iterator jetOfCol = theJets->begin();
//	  jetOfCol != theJets->end(); jetOfCol++ ) {
//	Analysis::TrackAssociation* tAssoc = new Analysis::TrackAssociation(MyjetBasis);
//	Jet::const_iterator itjc  = (*jetOfCol)->begin();
//	Jet::const_iterator itjcE = (*jetOfCol)->end();
//	for ( ; itjc != itjcE; itjc++) {
//	  const Rec::TrackParticle *tp = dynamic_cast<const Rec::TrackParticle*>(*itjc);
//	  if (tp) {
//            ATH_MSG_VERBOSE("#BTAG# A TrackParticle " << tp << " " << tp->pt());
//	    tAssoc->set_association(tpContainer, tp, 0.);
//	    nTotalTrackAssoc++;
//	  }
//	}
//	(*jetOfCol)->setAssociation( tAssoc );
      //}  //end if
//      if(0==nTotalTrackAssoc) {
//	ATH_MSG_WARNING("#BTAG# There seems to be a problem, no tracks are associated to the jets...Stopping execute() now.");
//        return StatusCode::SUCCESS;
//     }
//    }

    // // ----- associate electrons and photons
    // const ElectronContainer* elContainer(0);
    // const PhotonContainer* phContainer(0);
    // ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator elAssocIter = m_ElectronToJetAssociatorList.begin();
    // ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator elAssocEnd = m_ElectronToJetAssociatorList.end();
    // std::vector< std::string >::iterator elNameIter = m_ElectronContainerNameList.begin();
    // std::vector< std::string >::iterator phNameIter = m_PhotonContainerNameList.begin();
    // std::vector< std::string >::iterator elAssocNameIter = m_ElectronToJetAssocNameList.begin();
    // std::vector< std::string >::iterator phAssocNameIter = m_PhotonToJetAssocNameList.begin();

    // ATH_MSG_VERBOSE("#BTAG# Now associate the electrons and the photons");

    // for (; elAssocIter!=elAssocEnd; ++elAssocIter) {
    //   if (*elNameIter != "none") {
    //     sc = evtStore()->retrieve( elContainer, *elNameIter );
    //     if ( sc.isFailure() || elContainer==0) {
    //       ATH_MSG_WARNING("#BTAG# Electron container " << *elNameIter << " not found.");
    //     } else {
    //     (*elAssocIter)->associateParticlesToJets<ElectronAssociation>( theJets, elContainer, *elAssocNameIter );
    //     }
    //     if (*phNameIter != "none") {
    //       sc = evtStore()->retrieve( phContainer, *phNameIter );
    //       if ( sc.isFailure() || phContainer==0) {
    //         ATH_MSG_WARNING("#BTAG# Photon container " << *phNameIter << " not found.");
    //       } else {
    //       (*elAssocIter)->associateParticlesToJets<PhotonAssociation>( theJets, phContainer, *phAssocNameIter );
    //       }
    //     }
    //     ++elNameIter;
    //     ++phNameIter;
    //     ++elAssocNameIter;
    //     ++phAssocNameIter;
    //   }
    // }

    // // ----- associate muons
    // const MuonContainer* muContainer(0);
    // ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator muassocIter = m_MuonToJetAssociatorList.begin();
    // ToolHandleArray< Analysis::ParticleToJetAssociator >::iterator muassocEnd = m_MuonToJetAssociatorList.end();
    // std::vector< std::string >::iterator muNameIter = m_MuonContainerNameList.begin();
    // std::vector< std::string >::iterator muAssocNameIter = m_MuonToJetAssocNameList.begin();

    // ATH_MSG_VERBOSE("#BTAG# Now associate the muons");

    // for (; muassocIter!=muassocEnd; ++muassocIter) {
    //   if (*muNameIter != "none") {
    //     sc = evtStore()->retrieve( muContainer, *muNameIter );
    //     if ( sc.isFailure() || muContainer==0) {
    //       ATH_MSG_WARNING("#BTAG# Muon container " << *muNameIter << " not found.");
    //     } else {
    //       (*muassocIter)->associateParticlesToJets<Analysis::MuonAssociation>( theJets, muContainer, *muAssocNameIter ); 
    //     }
    //     ++muNameIter;
    //     ++muAssocNameIter;
    //   }
    // }

    return StatusCode::SUCCESS;
   }

} // namespace

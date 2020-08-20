///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// METTruthAssociator.cxx 
// Implementation file for class METTruthAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete, MA Klein
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METTruthAssociator.h"

#include "StoreGate/DataHandle.h"

// xAOD EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "AthContainers/ConstDataVector.h"

// Helpers
#include "xAODEgamma/EgammaxAODHelpers.h"
//#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "TruthUtils/TruthParticleHelpers.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using namespace xAOD;

  static SG::AuxElement::ConstAccessor<std::vector<ElementLink<IParticleContainer> > > cacc_ghosttruth("GhostTruth");

  // Constructors
  ////////////////
  METTruthAssociator::METTruthAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {}

  // Destructor
  ///////////////
  METTruthAssociator::~METTruthAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTruthAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    //initialise ReadHandleKeys
    ATH_CHECK( m_recoElKey.initialize());
    ATH_CHECK( m_recoJetKey.initialize());
    ATH_CHECK( m_recoMuKey.initialize());
    ATH_CHECK( m_recoGamKey.initialize());
    ATH_CHECK( m_recoTauKey.initialize());
    ATH_CHECK( m_truthEventKey.initialize());

    return StatusCode::SUCCESS;
  }

  StatusCode METTruthAssociator::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
  // executeTool
  ////////////////
  StatusCode METTruthAssociator::executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    ATH_CHECK( associateJets(metMap) );

    SG::ReadHandle<xAOD::ElectronContainer> electronCont(m_recoElKey);
    if (!electronCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_recoElKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    if(fillAssocMap(metMap,electronCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with electron container " << m_recoElKey.key());
      return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::PhotonContainer> photonCont(m_recoGamKey);
    if (!photonCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input photon container " << m_recoGamKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved photon collection");
    if(fillAssocMap(metMap,photonCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with photon container " << m_recoGamKey.key());
      return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::MuonContainer> muonCont(m_recoMuKey);
    if (!muonCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input muon container " << m_recoMuKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully retrieved muon collection");
    if(fillAssocMap(metMap,muonCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with muon container " << m_recoMuKey.key());
      return StatusCode::FAILURE;
    }

    SG::ReadHandle<xAOD::TauJetContainer> tauCont(m_recoTauKey);
    if (!tauCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input tau container " << m_recoTauKey.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved tau collection");
    if(fillAssocMap(metMap,tauCont.cptr()).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with tau container " << m_recoTauKey.key());
      return StatusCode::FAILURE;
    }

    if( computeSoftTerms(metCont,metMap).isFailure() ) {
      ATH_MSG_WARNING("Unable to build truth core soft terms.");
      return StatusCode::FAILURE;      
    }

    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Build jet and soft contributions

  StatusCode METTruthAssociator::associateJets(xAOD::MissingETAssociationMap* metMap) const
  {
    // Retrieve the jet container
    SG::ReadHandle<xAOD::JetContainer> jetCont(m_recoJetKey);
    if (!jetCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input jet container " << m_recoJetKey.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved jet collection");

    // Create jet associations
    std::vector<const IParticle*> chargedTruth;
    std::vector<const IParticle*> jetTruth;
    std::vector<ElementLink<IParticleContainer> > jetconst;
    jetTruth.reserve(20);
    for(const auto& jet : *jetCont) {
      jetTruth.clear();
      jetconst.clear();
      MissingETBase::Types::constvec_t trkvec;
      MissingETBase::Types::constvec_t truthvec;
      if(!cacc_ghosttruth.isAvailable(*jet)) {
	ATH_MSG_WARNING("Failed to extract ghost truth particles from jet");
      } else {
	for (const auto& el : cacc_ghosttruth(*jet)) {
	  if(el.isValid()) {
	    const xAOD::TruthParticle *truth = static_cast<const xAOD::TruthParticle*>(*el);
	    ATH_MSG_VERBOSE("Jet contains truth particle with pt " << truth->pt()
			    << " status " << truth->status()
			    << " pdgId " << truth->pdgId() );
	    if(fabs(truth->charge())>1e-6) {
	      trkvec += *truth;
	    }
	    if(!truth->isMuon()) {
	      jetconst.push_back(el);
	      jetTruth.push_back(truth);
	      truthvec += *truth;
	    }
	  }
	}
      }

      MissingETComposition::add(metMap,jet,jetconst,trkvec);
      MissingETComposition::insert(metMap,jet,jet,jetTruth);
      ATH_MSG_VERBOSE("Added association " << metMap->findIndex(jet) << " pointing to jet " << jet);
      ATH_MSG_VERBOSE("Jet pt, eta, phi = " << jet->pt() << ", " << jet->eta() << "," << jet->phi() );
      ATH_MSG_VERBOSE("Jet truthvec pt = " << truthvec.cpt() );
      ATH_MSG_VERBOSE("Jet trkvec pt = " << trkvec.cpt() );
    }

    MissingETComposition::addMiscAssociation(metMap);
    ATH_MSG_DEBUG("Added miscellaneous association");

    return StatusCode::SUCCESS;
  }

  StatusCode METTruthAssociator::computeSoftTerms(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {
    // Add MET terms to the container
    // Always do this in order that the terms exist even if the method fails
    MissingET* metCoreAllTruth = new MissingET(0.,0.,0.,"SoftTruthAllCore",MissingETBase::Source::softEvent() | MissingETBase::Source::truthInt() | MissingETBase::Source::Calo);
    metCont->push_back(metCoreAllTruth);
    MissingET* metCoreChargedTruth = new MissingET(0.,0.,0.,"SoftTruthChargedCore",MissingETBase::Source::softEvent() | MissingETBase::Source::truthInt() | MissingETBase::Source::ID);
    metCont->push_back(metCoreChargedTruth);
    MissingET* metCoreChargedCentralTruth = new MissingET(0.,0.,0.,"SoftTruthChargedCentralCore",MissingETBase::Source::softEvent() | MissingETBase::Source::truthInt(MissingETBase::Source::Central) | MissingETBase::Source::ID);
    metCont->push_back(metCoreChargedCentralTruth);

    ATH_MSG_VERBOSE("Added core terms.");

    // Retrieve the truth container
    SG::ReadHandle<xAOD::TruthEventContainer> truthEventCont(m_truthEventKey);
    if (!truthEventCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input truthEvent container " << m_truthEvent);
      return StatusCode::FAILURE;
    }

    // First truth event is the hard scatter
    const TruthEvent* hsevent = truthEventCont->front();
    ConstDataVector<TruthParticleContainer> truthParticleCont(SG::VIEW_ELEMENTS);
    for(size_t itp=0; itp<hsevent->nTruthParticles(); ++itp) {
      const xAOD::TruthParticle* tp = hsevent->truthParticle(itp);
      if(tp) {
	truthParticleCont.push_back(tp);
	ATH_MSG_VERBOSE("Extracted truth particle with index " << tp->index() );
      }
    }

    const IParticleContainer* uniqueTruth = metMap->getUniqueSignals(truthParticleCont.asDataVector(),MissingETBase::UsageHandler::TruthParticle);
    ATH_MSG_VERBOSE("Extracted " << uniqueTruth->size() << "/" << truthParticleCont.size()
		    << " unique truth particles.");
    for(const auto& part : *uniqueTruth) {
      const xAOD::TruthParticle* truth = static_cast<const xAOD::TruthParticle*>(part);
      // stable
      if(!MC::isGenStable(truth->status(),truth->barcode())) continue;
      // interacting
      if(MC::isNonInteracting(truth->pdgId())) continue;
      if(truth->pt()<1 || fabs(truth->eta())>5) continue;
      ATH_MSG_VERBOSE("Accepted soft truth particle with pt " << truth->pt()
		      << " status " << truth->status()
		      << " pdgId " << truth->pdgId() );
      if(fabs(truth->charge())>1e-6) {
	// in ID acceptance
	if(fabs(truth->eta())<2.5) *metCoreChargedCentralTruth += truth;
	*metCoreChargedTruth += truth;
      }
      if(!truth->isMuon()) *metCoreAllTruth += truth;
    }
    delete uniqueTruth;
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METTruthAssociator::extractTruthParticles(const xAOD::IParticle* obj,
						       std::vector<const xAOD::IParticle*>& truthlist) const
  {
    switch(obj->type()) {
    case xAOD::Type::Electron:
      return extractTruthFromElectron(obj,truthlist);
    case xAOD::Type::Photon:
      return extractTruthFromPhoton(obj,truthlist);
    case xAOD::Type::Muon:
      return extractTruthFromMuon(obj,truthlist);
    case xAOD::Type::Tau:
      return extractTruthFromTau(obj,truthlist);
    default:
      ATH_MSG_WARNING("Invalid object type " << obj->type() << " passed to extractTruthParticle");
      break;
    }

    return StatusCode::FAILURE;
  }

  StatusCode METTruthAssociator::extractTruthFromElectron(const xAOD::IParticle* obj,
						      std::vector<const xAOD::IParticle*>& truthlist) const
  {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(obj);
    // El --> TruthParticles
    const xAOD::TruthParticle* eltruth = TruthHelpers::getTruthParticle( *el );
    if(eltruth && MC::isGenStable(eltruth->status(),eltruth->barcode()))
      truthlist.push_back(eltruth);

    // for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
    //   const TrackParticle* eltrk = EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
    //   const xAOD::IParticle* truth(0);
    //   if(eltrk) truth = TruthHelpers::getTruthParticle( *eltrk );
    //   if(truth && truth!=eltruth) truthlist.push_back(truth);
    // }

    // Retrieve the truth container
    SG::ReadHandle<xAOD::TruthEventContainer> truthEventCont(m_truthEventKey);
    if (!truthEventCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input truthEvent container " << m_truthEvent);
      return StatusCode::SUCCESS;
    }


    // First truth event is the hard scatter
    const TruthEvent* hsevent = truthEventCont->front();
    ConstDataVector<TruthParticleContainer> truthParticleCont(SG::VIEW_ELEMENTS);
    for(size_t itp=0; itp<hsevent->nTruthParticles(); ++itp) {
      const xAOD::TruthParticle* tp = hsevent->truthParticle(itp);
      if(tp) {
	truthParticleCont.push_back(tp);
      }
    }

    for(const auto& truth : truthParticleCont) {
      if(truth->pt()<1) continue;
      // stable
      if(!MC::isGenStable(truth->status(),truth->barcode())) continue;
      // interacting
      if(MC::isNonInteracting(truth->pdgId())) continue;
      float etasize = 0.025/2;
      float phisize = 0.025/2;
      bool isSuperCluster = false;
      float Rsize = 0.;
      switch(el->caloCluster()->clusterSize()) {
      case xAOD::CaloCluster::SW_55ele:
	etasize *= 5; phisize *=5;
	break;
      case xAOD::CaloCluster::SW_35ele:	
	etasize *= 3; phisize *=5;
	break;
      case xAOD::CaloCluster::SW_37ele:
	etasize *= 3; phisize *=7;
	break;
      case xAOD::CaloCluster::SuperCluster:
	isSuperCluster = true;
	Rsize = el->caloCluster()->getMomentValue( xAOD::CaloCluster::SECOND_R );
	if (Rsize<0.) {ATH_MSG_WARNING("Invalid SECOND_R moment retrieved!");}
	break;
      default:
	ATH_MSG_WARNING("Unexpected electron cluster size " << el->caloCluster()->clusterSize() << " received!");
	continue;
      }
      float deltaEta(fabs(truth->eta()-el->caloCluster()->eta()));
      float deltaPhi(fabs(truth->p4().DeltaPhi(el->caloCluster()->p4())));
      if( (isSuperCluster && (deltaEta*deltaEta+deltaPhi*deltaPhi)<Rsize*Rsize ) ||
	  (deltaEta<etasize && deltaPhi<phisize) ) {
	bool skip(false);
	for(const auto& truthobj : truthlist) {
	  const xAOD::TruthParticle *truth2 = static_cast<const xAOD::TruthParticle*>(truthobj);
	  if(truth==truth2) { skip = true; break;}
	}
	if(!skip) {
	  truthlist.push_back(truth);	  
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METTruthAssociator::extractTruthFromPhoton(const xAOD::IParticle* obj,
						      std::vector<const xAOD::IParticle*>& truthlist) const
  {
    const xAOD::Photon* ph = static_cast<const xAOD::Photon*>(obj);
    // Ph --> TruthParticles
    const xAOD::TruthParticle* phtruth = TruthHelpers::getTruthParticle( *ph );
    if(phtruth && MC::isGenStable(phtruth->status(),phtruth->barcode()))
      truthlist.push_back(phtruth);

    // std::vector<const xAOD::TrackParticle*> phtrks;
    // for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
    //   const xAOD::Vertex* phvx = ph->vertex(iVtx);
    //   for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
    // 	const xAOD::TrackParticle* phtrk = EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
    // 	bool duplicate = false;
    // 	for(const auto& gamtrk : phtrks) {
    // 	  if( (duplicate = (phtrk == gamtrk)) ) {
    // 	    ATH_MSG_VERBOSE("Veto duplicate track");
    // 	    break;
    // 	  }
    // 	}
    // 	if(!duplicate) {
    // 	  const xAOD::IParticle* truth(0);
    // 	  if(phtrk) truth = TruthHelpers::getTruthParticle( *phtrk );
    // 	  if(truth && truth!=phtruth) truthlist.push_back(truth);
    // 	}
    //   }
    // }

    // Retrieve the truth container
    SG::ReadHandle<xAOD::TruthEventContainer> truthEventCont(m_truthEventKey);
    if (!truthEventCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input truthEvent container " << m_truthEvent);
      return StatusCode::SUCCESS;
    }

    // First truth event is the hard scatter
    const TruthEvent* hsevent = truthEventCont->front();
    ConstDataVector<TruthParticleContainer> truthParticleCont(SG::VIEW_ELEMENTS);
    for(size_t itp=0; itp<hsevent->nTruthParticles(); ++itp) {
      const xAOD::TruthParticle* tp = hsevent->truthParticle(itp);
      if(tp) {
	truthParticleCont.push_back(tp);
      }
    }

    for(const auto& truth : truthParticleCont) {
      if(!truth || truth->pt()<1) continue;
      // stable
      if(!MC::isGenStable(truth->status(),truth->barcode())) continue;
      // interacting
      if(MC::isNonInteracting(truth->pdgId())) continue;
      float etasize(0.025/2);
      float phisize(0.025/2);
      bool isSuperCluster = false;
      float Rsize = 0.;
      switch(ph->caloCluster()->clusterSize()) {
      case xAOD::CaloCluster::SW_55gam:
      case xAOD::CaloCluster::SW_55Econv:
	etasize *= 5; phisize *=5;
	break;
      case xAOD::CaloCluster::SW_35ele:	
      case xAOD::CaloCluster::SW_35gam:	
      case xAOD::CaloCluster::SW_35Econv:	
	etasize *= 3; phisize *=5;
	break;
      case xAOD::CaloCluster::SW_37gam:
      case xAOD::CaloCluster::SW_37Econv:
	etasize *= 3; phisize *=7;
	break;
      case xAOD::CaloCluster::SuperCluster:
	isSuperCluster = true;
	Rsize = ph->caloCluster()->getMomentValue( xAOD::CaloCluster::SECOND_R );
	if (Rsize<0.) {ATH_MSG_WARNING("Invalid SECOND_R moment retrieved!");}
	break;
      default:
	ATH_MSG_WARNING("Unexpected photon cluster size " << ph->caloCluster()->clusterSize() << " received!");
	continue;
      }
      float deltaEta(fabs(truth->eta()-ph->caloCluster()->eta()));
      float deltaPhi(fabs(truth->p4().DeltaPhi(ph->caloCluster()->p4())));
      if( (isSuperCluster && (deltaEta*deltaEta+deltaPhi*deltaPhi)<Rsize*Rsize ) ||
	  (deltaEta<etasize && deltaPhi<phisize) ) {
	bool skip(false);
	for(const auto& truthobj : truthlist) {
	  const xAOD::TruthParticle *truth2 = static_cast<const xAOD::TruthParticle*>(truthobj);
	  if(truth==truth2) { skip = true; break;}
	}
	if(!skip) {
	  truthlist.push_back(truth);	  
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METTruthAssociator::extractTruthFromMuon(const xAOD::IParticle* obj,
						      std::vector<const xAOD::IParticle*>& truthlist) const
  {
    const xAOD::Muon* mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* trk = mu->primaryTrackParticle();
    const xAOD::TruthParticle* truth(0);
    if(trk) truth = TruthHelpers::getTruthParticle( *trk );
    if(truth && MC::isGenStable(truth->status(),truth->barcode()))
      truthlist.push_back(truth);
    return StatusCode::SUCCESS;
  }

  StatusCode METTruthAssociator::extractTruthFromTau(const xAOD::IParticle* obj,
						     std::vector<const xAOD::IParticle*>& truthlist) const
  {
    const TauJet* tau = static_cast<const TauJet*>(obj);
    const Jet* seedjet = *tau->jetLink();
    std::vector<ElementLink<IParticleContainer> > jetconst = cacc_ghosttruth(*seedjet);
    for(const auto& truth : jetconst) {
      if(truth) {
	// TEMP: use jet seed axis
	//       taus will provide an accessor
	if(!xAOD::P4Helpers::isInDeltaR(*seedjet,**truth,0.2,m_useRapidity)) continue;
	truthlist.push_back(*truth);
      }
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Override fill map method to use the extracted truth particles

  StatusCode METTruthAssociator::fillAssocMap(xAOD::MissingETAssociationMap* metMap,
					      const xAOD::IParticleContainer* hardObjs) const
  //					 std::vector<const xAOD::IParticle*>& mutracks)
  {
    std::vector<const IParticle*> constlist;
    constlist.reserve(20);
    std::vector<const IParticle*> hardObjs_tmp;
    for(const auto& obj : *hardObjs) {
      hardObjs_tmp.push_back(obj);
    }
    std::sort(hardObjs_tmp.begin(),hardObjs_tmp.end(),greaterPt);

    for(const auto& obj : hardObjs_tmp) {
      if(obj->pt()<5e3 && obj->type()!=xAOD::Type::Muon) continue;
      constlist.clear();
      ATH_MSG_VERBOSE( "Object type, pt, eta, phi = " << obj->type() << ", " << obj->pt() << ", " << obj->eta() << "," << obj->phi() );
      ATH_CHECK( this->extractTruthParticles(obj,constlist) );

      // MissingETBase::Types::constvec_t chrgvec;
      // MissingETBase::Types::constvec_t truthvec;

      for(const auto& truthobj : constlist) {
	const xAOD::TruthParticle *truth = static_cast<const xAOD::TruthParticle*>(truthobj);
	ATH_MSG_VERBOSE("Matched truth particle with pt " << truth->pt()
			<< " status " << truth->status()
			<< " pdgId " << truth->pdgId()
			<< " charge " << truth->charge());
	ATH_MSG_VERBOSE("DeltaR to hard object: " << xAOD::P4Helpers::deltaR(*truth,*obj,m_useRapidity));

	// if(fabs(truth->charge())>1e-9)
	//   chrgvec += MissingETBase::Types::constvec_t(*truth);
	// if(!truth->isMuon())
	//   truthvec += MissingETBase::Types::constvec_t(*truth);
      }
      // ATH_MSG_VERBOSE("Charged truth constvec: sumpt " << chrgvec.sumpt());
      // ATH_MSG_VERBOSE("Calo truth constvec: sumpt " << truthvec.sumpt());

      MissingETComposition::insert(metMap,obj,constlist);
      ATH_MSG_VERBOSE("\n\n");
    }
    return StatusCode::SUCCESS;
  }

}

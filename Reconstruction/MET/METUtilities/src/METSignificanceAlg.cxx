/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSignificanceAlg.cxx

#include "METSignificanceAlg.h"
#include "METInterface/IMETSignificance.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"

#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
#include "TauAnalysisTools/ITauSelectionTool.h"

using std::string;
using namespace xAOD;

namespace met {

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");

  //**********************************************************************

  METSignificanceAlg::METSignificanceAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
    m_muonSelTool(""),
    m_elecSelLHTool(""),
    m_photonSelIsEMTool(""),
    m_tauSelTool("")
 {
    declareProperty( "Signficiance",   m_metsignif                       );
    declareProperty( "METMapName",     m_mapname   = "METAssoc"          );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty( "METName",        m_outname   = "MET_Reference"     );

    declareProperty( "METSoftClName",  m_softclname  = "SoftClus"        );
    declareProperty( "METSoftTrkName", m_softtrkname = "PVSoftTrk"       );

    declareProperty( "InputJets",      m_jetColl   = "AntiKt4LCTopoJets" );
    declareProperty( "InputElectrons", m_eleColl   = "Electrons"         );
    declareProperty( "InputPhotons",   m_gammaColl = "Photons"           );
    declareProperty( "InputTaus",      m_tauColl   = "TauJets"           );
    declareProperty( "InputMuons",     m_muonColl  = "Muons"             );

    declareProperty( "MuonSelectionTool",        m_muonSelTool           );
    declareProperty( "ElectronLHSelectionTool",  m_elecSelLHTool         );
    declareProperty( "PhotonIsEMSelectionTool" , m_photonSelIsEMTool     );
    declareProperty( "TauSelectionTool",         m_tauSelTool            );

    declareProperty( "DoTruthLeptons", m_doTruthLep = false              );

  }

  //**********************************************************************

  METSignificanceAlg::~METSignificanceAlg() { }

  //**********************************************************************

  StatusCode METSignificanceAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // retrieve tools
    if( m_metsignif.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_metsignif->name());
      return StatusCode::FAILURE;
    };

    if( m_muonSelTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_muonSelTool->name());
      return StatusCode::FAILURE;
    };

    if( m_elecSelLHTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_elecSelLHTool->name());
      return StatusCode::FAILURE;
    };

    if( m_photonSelIsEMTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_photonSelIsEMTool->name());
      return StatusCode::FAILURE;
    };

    if( m_tauSelTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_tauSelTool->name());
      return StatusCode::FAILURE;
    };

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METSignificanceAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METSignificanceAlg::execute() {
    ATH_MSG_VERBOSE("Executing " << name() << "...");

    // Create a MissingETContainer with its aux store
    MissingETContainer* newMet = new MissingETContainer();
    if( evtStore()->record(newMet, m_outname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_outname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_outname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_outname+"Aux.");
      return StatusCode::SUCCESS;
    }
    newMet->setStore(metAuxCont);

    const MissingETAssociationMap* metMap = 0;
    if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    metMap->resetObjSelectionFlags();

    // Retrieve containers ***********************************************

    /// MET
    const MissingETContainer* coreMet(0);
    if( evtStore()->retrieve(coreMet, m_corename).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_corename);
      return StatusCode::SUCCESS;
    }

    /// Jets
    const JetContainer* jetCont(0);
    if( evtStore()->retrieve(jetCont, m_jetColl).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input jet container: " << m_jetColl);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved jet collection");

    /// Electrons
    const ElectronContainer* elCont(0);
    if(!m_eleColl.empty()) {
      if( evtStore()->retrieve(elCont, m_eleColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve input electron container: " << m_eleColl);
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("Successfully retrieved electron collection");
    }

    /// Photons
    const PhotonContainer* phCont(0);
    if(!m_gammaColl.empty()) {
      if( evtStore()->retrieve(phCont, m_gammaColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve input photon container: " << m_gammaColl);
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("Successfully retrieved photon collection");
    }

    /// Taus
    const TauJetContainer* tauCont(0);
    if(!m_tauColl.empty()) {
      if( evtStore()->retrieve(tauCont, m_tauColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve input tau container: " << m_tauColl);
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("Successfully retrieved tau collection");
    }

    /// Muons
    const MuonContainer* muonCont(0);
    if(!m_muonColl.empty()) {
      if( evtStore()->retrieve(muonCont, m_muonColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve input muon container: " << m_muonColl);
	return StatusCode::SUCCESS;
      }
      ATH_MSG_DEBUG("Successfully retrieved muon collection");
    }

    // Select and flag objects for final MET building ***************************

    MissingETBase::UsageHandler::Policy objScale = MissingETBase::UsageHandler::PhysicsObject;
    if(m_doTruthLep) objScale = MissingETBase::UsageHandler::TruthParticle;
    // Electrons
    if(!m_eleColl.empty()) {
      ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *elCont) {
    	if(accept(el)) {
    	  metElectrons.push_back(el);
    	}
      }
    }
    // Photons
    if(!m_gammaColl.empty()) {
      ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
      for(const auto& ph : *phCont) {
    	if(accept(ph)) {
    	  metPhotons.push_back(ph);
    	}
      }
      ATH_MSG_DEBUG("Selected " << metPhotons.size() << " MET photons. "
    		    << acc_constitObjLinks(*(*newMet)["RefGamma"]).size() << " are non-overlapping.");
    }

    // Taus
    if(!m_tauColl.empty()) {
      ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
      for(const auto& tau : *tauCont) {
    	if(accept(tau)) {
    	  metTaus.push_back(tau);
    	}
      }
      ATH_MSG_DEBUG("Selected " << metTaus.size() << " MET taus. "
    		    << acc_constitObjLinks(*(*newMet)["RefTau"]).size() << " are non-overlapping.");
    }

    // Muons
    if(!m_muonColl.empty()) {
      ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
      for(const auto& mu : *muonCont) {
    	if(accept(mu)) {
    	  metMuons.push_back(mu);
    	}
      }
      
      if(m_doTruthLep) objScale = MissingETBase::UsageHandler::OnlyTrack;
      ATH_MSG_DEBUG("Selected " << metMuons.size() << " MET muons. "
    		    << acc_constitObjLinks(*(*newMet)["Muons"]).size() << " are non-overlapping.");
    }

    ATH_MSG_DEBUG("Of " << jetCont->size() << " jets, "
		  << acc_constitObjLinks(*(*newMet)["RefJet"]).size() << " are non-overlapping, "
		  << acc_constitObjLinks(*(*newMet)[m_softtrkname]).size() << " are soft");

    MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
    if((*newMet)[m_softtrkname]) trksource = (*newMet)[m_softtrkname]->source();
    MissingETBase::Types::bitmask_t clsource = MissingETBase::Source::LCTopo;
    if((*newMet)[m_softclname]) clsource = (*newMet)[m_softclname]->source();

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  bool METSignificanceAlg::accept(const xAOD::Muon* mu)
  {
    if( mu->pt()<2.5e3 || mu->pt()/cosh(mu->eta())<4e3 ) return false;
    return m_muonSelTool->accept(*mu);
  }

  bool METSignificanceAlg::accept(const xAOD::Electron* el)
  {
    if( fabs(el->eta())>2.47 || el->pt()<10e3 ) return false;
    return m_elecSelLHTool->accept(*el);
  }

  bool METSignificanceAlg::accept(const xAOD::Photon* ph)
  {
    if( !(ph->author()&20) || fabs(ph->eta())>2.47 || ph->pt()<10e3 ) return false;
    return m_photonSelIsEMTool->accept(ph);
  }

  bool METSignificanceAlg::accept(const xAOD::TauJet* tau)
  { return m_tauSelTool->accept( *tau ); }

}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// METMakerAlg.cxx

#include "METMakerAlg.h"
#include "METInterface/IMETMaker.h"

#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAssociationHelper.h"

#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "PATCore/AcceptData.h"
#include "TauAnalysisTools/ITauSelectionTool.h"

using std::string;
using namespace xAOD;

namespace met {

  typedef ElementLink<xAOD::IParticleContainer> iplink_t;
  static const SG::AuxElement::ConstAccessor< std::vector<iplink_t > > acc_constitObjLinks("ConstitObjectLinks");

  //**********************************************************************

  METMakerAlg::METMakerAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
    m_ElectronContainerKey(""),
    m_PhotonContainerKey(""),
    m_TauJetContainerKey(""),
    m_MuonContainerKey(""),
    m_JetContainerKey(""), 
    m_CoreMetKey(""),
    m_metKey(""),
    m_metMap("METAssoc"),
    m_muonSelTool(""),
    m_elecSelLHTool(""),
    m_photonSelIsEMTool(""),
    m_tauSelTool("")
 {
    declareProperty( "Maker",          m_metmaker                        );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty("METName",         m_metKey = std::string("MET_Reference"),"MET container");
    declareProperty("METMapName",      m_metMap );

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

  METMakerAlg::~METMakerAlg() { }

  //**********************************************************************

  StatusCode METMakerAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");

    // retrieve tools
    if( m_metmaker.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve tool: " << m_metmaker->name());
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
    ATH_CHECK( m_ElectronContainerKey.assign(m_eleColl) );
    ATH_CHECK( m_ElectronContainerKey.initialize() );
    ATH_CHECK( m_PhotonContainerKey.assign(m_gammaColl) );
    ATH_CHECK( m_PhotonContainerKey.initialize() );
    ATH_CHECK( m_TauJetContainerKey.assign(m_tauColl) );
    ATH_CHECK( m_TauJetContainerKey.initialize() );
    ATH_CHECK( m_MuonContainerKey.assign(m_muonColl) );
    ATH_CHECK( m_MuonContainerKey.initialize() );
    ATH_CHECK( m_JetContainerKey.assign(m_jetColl) );
    ATH_CHECK( m_JetContainerKey.initialize() );
    ATH_CHECK( m_CoreMetKey.assign(m_corename) );
    ATH_CHECK( m_CoreMetKey.initialize() );
    ATH_CHECK( m_metKey.initialize() );

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METMakerAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METMakerAlg::execute() {
    ATH_MSG_VERBOSE("Executing " << name() << "...");

    // Create a MissingETContainer with its aux store
    auto ctx = getContext();
    auto metHandle= SG::makeHandle (m_metKey,ctx);
    ATH_CHECK( metHandle.record (std::make_unique<xAOD::MissingETContainer>(),                      std::make_unique<xAOD::MissingETAuxContainer>()) );
    xAOD::MissingETContainer* newMet=metHandle.ptr();

    ATH_CHECK( m_metMap.isValid() );

    MissingETAssociationMap* metMap = new MissingETAssociationMap((*m_metMap));
    MissingETAssociationHelper metHelper(metMap);
    // Retrieve containers ***********************************************

    /// MET
    SG::ReadHandle<xAOD::MissingETContainer> coreMet(m_CoreMetKey);
    if (!coreMet.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MissingETContainer: " << m_CoreMetKey.key());
      return StatusCode::FAILURE;
    }


    /// Jets
    SG::ReadHandle<xAOD::JetContainer> Jets(m_JetContainerKey);
    if (!Jets.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve JetContainer: " << Jets.key());
      return StatusCode::FAILURE;
    }

    /// Electrons
    SG::ReadHandle<xAOD::ElectronContainer> Electrons(m_ElectronContainerKey);
    if (!Electrons.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve ElectronContainer: " << Electrons.key());
      return StatusCode::FAILURE;
    }

    /// Photons
    SG::ReadHandle<xAOD::PhotonContainer> Gamma(m_PhotonContainerKey);
    if (!Gamma.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve GammaContainer: " << Gamma.key());
      return StatusCode::FAILURE;
    }

    /// Taus
    SG::ReadHandle<xAOD::TauJetContainer> TauJets(m_TauJetContainerKey);
    if (!TauJets.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve TauJetContainer: " << TauJets.key());
      return StatusCode::FAILURE;
    }

    /// Muons
    SG::ReadHandle<xAOD::MuonContainer> Muons(m_MuonContainerKey);
    if (!Muons.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve MuonContainer: "  << Muons.key());
      return StatusCode::FAILURE;
    }

    // Select and flag objects for final MET building ***************************

    MissingETBase::UsageHandler::Policy objScale = MissingETBase::UsageHandler::PhysicsObject;
    if(m_doTruthLep) objScale = MissingETBase::UsageHandler::TruthParticle;
    // Electrons
    if(!m_eleColl.empty()) {
      ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *Electrons) {
    	if(accept(el)) {
    	  metElectrons.push_back(el);

    	}
      }
      if( m_metmaker->rebuildMET("RefEle", xAOD::Type::Electron, newMet,
    				 metElectrons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build electron term.");
      }
      ATH_MSG_DEBUG("Selected " << metElectrons.size() << " MET electrons. "
    		    << acc_constitObjLinks(*(*newMet)["RefEle"]).size() << " are non-overlapping.");
    }

    // Photons
    if(!m_gammaColl.empty()) {
      ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
      for(const auto& ph : *Gamma) {
    	if(accept(ph)) {
    	  metPhotons.push_back(ph);
    	}
      }
      if( m_metmaker->rebuildMET("RefGamma", xAOD::Type::Photon, newMet,
    				 metPhotons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build photon term.");
      }
      ATH_MSG_DEBUG("Selected " << metPhotons.size() << " MET photons. "
    		    << acc_constitObjLinks(*(*newMet)["RefGamma"]).size() << " are non-overlapping.");
    }

    // Taus
    if(!m_tauColl.empty()) {
      ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
      for(const auto& tau : *TauJets) {
    	if(accept(tau)) {
    	  metTaus.push_back(tau);
    	}
      }
      if( m_metmaker->rebuildMET("RefTau", xAOD::Type::Tau, newMet,
    				 metTaus.asDataVector(),
    				 &metHelper, objScale).isFailure() ){
    	ATH_MSG_WARNING("Failed to build tau term.");
      }
      ATH_MSG_DEBUG("Selected " << metTaus.size() << " MET taus. "
    		    << acc_constitObjLinks(*(*newMet)["RefTau"]).size() << " are non-overlapping.");
    }

    // Muons
    if(!m_muonColl.empty()) {
      ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
      for(const auto& mu : *Muons) {
    	if(accept(mu)) {
    	  metMuons.push_back(mu);
    	}
      }
      
      if(m_doTruthLep) objScale = MissingETBase::UsageHandler::OnlyTrack;
      if( m_metmaker->rebuildMET("Muons", xAOD::Type::Muon, newMet,
    				 metMuons.asDataVector(),
    				 &metHelper, objScale).isFailure() ) {
    	ATH_MSG_WARNING("Failed to build muon term.");
      }
      ATH_MSG_DEBUG("Selected " << metMuons.size() << " MET muons. "
    		    << acc_constitObjLinks(*(*newMet)["Muons"]).size() << " are non-overlapping.");
    }

    if( m_metmaker->rebuildJetMET("RefJet", m_softclname, m_softtrkname, newMet,
				  Jets.cptr(), coreMet.cptr(), &metHelper, false ).isFailure() ) {
      ATH_MSG_WARNING("Failed to build jet and soft terms.");
    }
    ATH_MSG_DEBUG("Of " << Jets.cptr()->size()  << " jets, "
		  << acc_constitObjLinks(*(*newMet)["RefJet"]).size() << " are non-overlapping, "
		  << acc_constitObjLinks(*(*newMet)[m_softtrkname]).size() << " are soft");

    MissingETBase::Types::bitmask_t trksource = MissingETBase::Source::Track;
    if((*newMet)[m_softtrkname]) trksource = (*newMet)[m_softtrkname]->source();
    if( m_metmaker->buildMETSum("FinalTrk", newMet, trksource).isFailure() ){
      ATH_MSG_WARNING("Building MET FinalTrk sum failed.");
    }
    MissingETBase::Types::bitmask_t clsource = MissingETBase::Source::LCTopo;
    if((*newMet)[m_softclname]) clsource = (*newMet)[m_softclname]->source();
    if( m_metmaker->buildMETSum("FinalClus", newMet, clsource).isFailure() ) {
      ATH_MSG_WARNING("Building MET FinalClus sum failed.");
    }

    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  bool METMakerAlg::accept(const xAOD::Muon* mu)
  {
    if( mu->pt()<2.5e3 || mu->pt()/cosh(mu->eta())<4e3 ) return false;
    return static_cast<bool>(m_muonSelTool->accept(*mu));
  }

  bool METMakerAlg::accept(const xAOD::Electron* el)
  {
    if( fabs(el->eta())>2.47 || el->pt()<10e3 ) return false;
    return static_cast<bool> (m_elecSelLHTool->accept(el));
  }

  bool METMakerAlg::accept(const xAOD::Photon* ph)
  {
    if( !(ph->author()&20) || fabs(ph->eta())>2.47 || ph->pt()<10e3 ) return false;
    return static_cast<bool> (m_photonSelIsEMTool->accept(ph));
  }

  bool METMakerAlg::accept(const xAOD::TauJet* tau)
  { 
  // std::cout<<"Just checking this works -> tau pt is "<<tau->pt()<<std::endl;

    return static_cast<bool>(m_tauSelTool->accept( *tau ));
  }

}

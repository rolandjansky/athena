/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMakerAlg.cxx

#include "METMakerAlg.h"
#include "METInterface/IMETMaker.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"

#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"

using std::string;
using namespace xAOD;

namespace met {

  //**********************************************************************

  METMakerAlg::METMakerAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty( "Maker",          m_metmaker                       );
    declareProperty( "METMapName",     m_mapname   = "METAssoc"          );
    declareProperty( "METCoreName",    m_corename  = "MET_Core"          );
    declareProperty( "METName",        m_outname   = "MET_Reference"     );

    declareProperty( "InputJets",      m_jetColl   = "AntiKt4LCTopoJets" );
    declareProperty( "InputElectrons", m_eleColl   = "Electrons"         );
    declareProperty( "InputPhotons",   m_gammaColl = "Photons"           );
    declareProperty( "InputTaus",      m_tauColl   = "TauJets"           );
    declareProperty( "InputMuons",     m_muonColl  = "Muons"             );
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

    const MissingETAssociationMap* metMap = 0;
    if( evtStore()->retrieve(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }

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

    std::vector<const xAOD::IParticle*> uniques;
    // Select and flag objects for final MET building ***************************

    // Electrons
    if(!m_eleColl.empty()) {
      MissingET* metEle = new MissingET(0.,0.,0.,"RefEle",MissingETBase::Source::electron());
      newMet->push_back(metEle);
      uniques.clear();
      ConstDataVector<ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *elCont) {
	if(accept(el)) {
	  metElectrons.push_back(el);
	}
      }
      ATH_CHECK( m_metmaker->rebuildMET(metEle, metElectrons.asDataVector(), metMap, uniques) );
      ATH_MSG_DEBUG("Selected " << metElectrons.size() << " MET electrons. "
		    << uniques.size() << " are non-overlapping.");
    }

    // Photons
    if(!m_gammaColl.empty()) {
      MissingET* metGamma = new MissingET(0.,0.,0.,"RefGamma",MissingETBase::Source::photon());
      newMet->push_back(metGamma);
      uniques.clear();
      ConstDataVector<PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
      for(const auto& ph : *phCont) {
	if(accept(ph)) {
	  metPhotons.push_back(ph);
	}
      }
      ATH_CHECK( m_metmaker->rebuildMET(metGamma, metPhotons.asDataVector(), metMap, uniques) );
      ATH_MSG_DEBUG("Selected " << metPhotons.size() << " MET photons. "
		    << uniques.size() << " are non-overlapping.");
    }

    // Taus
    if(!m_tauColl.empty()) {
      MissingET* metTau = new MissingET(0.,0.,0.,"RefTau",MissingETBase::Source::tau());
      newMet->push_back(metTau);
      uniques.clear();
      ConstDataVector<TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
      for(const auto& tau : *tauCont) {
	if(accept(tau)) {
	  metTaus.push_back(tau);
	}
      }
      ATH_CHECK( m_metmaker->rebuildMET(metTau, metTaus.asDataVector(), metMap, uniques) );
      ATH_MSG_DEBUG("Selected " << metTaus.size() << " MET taus. "
		    << uniques.size() << " are non-overlapping.");
    }

    // Muons
    if(!m_muonColl.empty()) {
      MissingET* metMuon = new MissingET(0.,0.,0.,"Muons",MissingETBase::Source::muon());
      newMet->push_back(metMuon);
      uniques.clear();
      ConstDataVector<MuonContainer> metMuons(SG::VIEW_ELEMENTS);
      for(const auto& mu : *muonCont) {
	if(accept(mu)) {
	  metMuons.push_back(mu);
	}
      }
      ATH_CHECK( m_metmaker->rebuildMET(metMuon, metMuons.asDataVector(), metMap, uniques) );
      ATH_MSG_DEBUG("Selected " << metMuons.size() << " MET muons. "
		    << uniques.size() << " are non-overlapping.");
    }

    MissingET* metJet = new MissingET(0.,0.,0.,"RefJet",MissingETBase::Source::jet());
    MissingET* metSoftCl = new MissingET(0.,0.,0.,"SoftClus",
					 MissingETBase::Source::softEvent()|MissingETBase::Source::cluster());
    MissingET* metSoftTrk = new MissingET(0.,0.,0.,"PVSoftTrk",
					  MissingETBase::Source::softEvent()|MissingETBase::Source::idTrack());
    newMet->push_back(metJet);
    newMet->push_back(metSoftTrk);
    newMet->push_back(metSoftCl);
    ATH_CHECK( m_metmaker->rebuildJetMET(metJet, jetCont, metMap, uniques,
					 metSoftCl, (*coreMet)["SoftClusCore"],
					 metSoftTrk, (*coreMet)["PVSoftTrkCore"]) );
    ATH_MSG_DEBUG("Of " << jetCont->size() << " jets, "
		 << uniques.size() << " are non-overlapping.");    

    ATH_CHECK( m_metmaker->buildMETSum("FinalTrk", newMet, MissingETBase::Source::Track) );
    ATH_CHECK( m_metmaker->buildMETSum("FinalClus", newMet, MissingETBase::Source::LCTopo) );

    return StatusCode::SUCCESS;
  }
    
  //**********************************************************************

  bool METMakerAlg::accept(const xAOD::Muon* mu)
  {

    if(mu->pt()<2.5e3 || mu->pt()/cosh(mu->eta())<4e3) return false;
    if(mu->muonType()==xAOD::Muon::MuonStandAlone) {
      // only take forward SA -- need a max eta cut?
      if(fabs(mu->eta())<2.5) return false;
      uint8_t nPrecision=0;
      if(!mu->primaryTrackParticleLink().isValid()) return false;
      mu->primaryTrackParticle()->summaryValue(nPrecision,xAOD::numberOfPrecisionLayers);
      if(nPrecision<3) return false;
    } // selection for StandAlone muons
    else if(mu->muonType()==xAOD::Muon::Combined || mu->muonType()==xAOD::Muon::SegmentTagged) {
      if(fabs(mu->eta())>2.5) return false;
      
      // could add some error checking to make sure we successfully read the details
      uint8_t nPixHits(0), nSctHits(0);
      if(!mu->primaryTrackParticleLink().isValid()) return false;
      mu->primaryTrackParticle()->summaryValue(nPixHits,xAOD::numberOfPixelHits);
      mu->primaryTrackParticle()->summaryValue(nSctHits,xAOD::numberOfSCTHits);

      if(nPixHits<3) return false;
      if(nPixHits+nSctHits<5) return false;
    } // selection for SegmentTagged and Combined muons
    else {return false;} // don't accept forward muons or calo tagged

    return true;
  }

  bool METMakerAlg::accept(const xAOD::Electron* el)
  {

    ATH_MSG_VERBOSE("Test electron quality." 
		    << " pT = " << el->pt()
		    << " eta = " << el->eta()
		    << " phi = " << el->phi());

    bool testPID = 0;
    el->passSelection(testPID,"Medium");
    ATH_MSG_VERBOSE("Electron PID \"Medium\" tests " << (testPID ? " GOOD" : "BAD") );
    if( !testPID ) return false;

    ATH_MSG_VERBOSE("Electron author = " << el->author() << " test " << (el->author()&17));
    if( !(el->author()&17) ) return false;

    if( el->pt()<10e3 ) return false;
    if( fabs(el->eta())>2.47 ) return false;

    // if( m_el_rejectCrack ) {
    //   if( fabs(el->eta())>1.37 &&
    // 	  fabs(el->eta())<1.52 ) return false;
    // }

    ATH_MSG_VERBOSE("Accepted this electron");

    return true;
  }

  bool METMakerAlg::accept(const xAOD::Photon* ph)
  {

    ATH_MSG_VERBOSE("Test photon quality." 
		    << " pT = " << ph->pt()
		    << " eta = " << ph->eta()
		    << " phi = " << ph->phi());

    bool testPID = 0;
    ph->passSelection(testPID,"Tight");
    ATH_MSG_VERBOSE("Photon PID \"Tight\" tests " << (testPID ? " GOOD" : "BAD") );
    if( !testPID ) return false;

    ATH_MSG_VERBOSE("Photon author = " << ph->author() << " test " << (ph->author()&20));
    if( !(ph->author()&20) ) return false;

    if( ph->pt()<10e3 ) return false;
    if( fabs(ph->eta())>2.47 ) return false;

    ATH_MSG_VERBOSE("Accepted this photon");

    return true;
  }

  bool METMakerAlg::accept(const xAOD::TauJet* tau)
  {
    ATH_MSG_VERBOSE("Testing tau with pt " << tau->pt() << ", eta " << tau->eta());
    ATH_MSG_VERBOSE("Tau ID discriminants:"
		    << " jet " << tau->discriminant(xAOD::TauJetParameters::BDTJetScore)
		    << " el " << tau->discriminant(xAOD::TauJetParameters::BDTEleScore)
		    << " mu " << tau->flag(xAOD::TauJetParameters::MuonFlag));

    if(tau->pt()<20e3 || fabs(tau->eta())>2.5) return false;
    // need to accommodate more than one of these?
    if(!tau->isTau( xAOD::TauJetParameters::IsTauFlag(xAOD::TauJetParameters::JetBDTSigMedium) )) return false;
    if(tau->isTau( xAOD::TauJetParameters::IsTauFlag(xAOD::TauJetParameters::EleBDTMedium) )) return false;
    if(tau->isTau( xAOD::TauJetParameters::IsTauFlag(xAOD::TauJetParameters::MuonVeto) )) return false;

    return true;
  }

}


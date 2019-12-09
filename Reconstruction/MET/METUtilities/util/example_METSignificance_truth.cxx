/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//Author: Doug Schaefer
//Email : schae@cern.ch
//Date  : Nov 2019
#ifndef XAOD_ANALYSIS
int main() {
  std::cout << "Examples are only defined in the analysis release." << std::endl;
  return 0;
}
#else

#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#else
#include "POOLRootAccess/TEvent.h"
#include "StoreGate/StoreGateSvc.h"
#endif

// FrameWork includes
#include "AsgTools/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "JetInterface/IJetModifier.h"

#include <memory>
#undef NDEBUG
#include <cassert>
#include "TFile.h"
#include "TSystem.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSignificance.h"
#include "METUtilities/CutsMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

static const SG::AuxElement::Decorator< float > dec_uniqueFrac("UniqueFrac");
static const SG::AuxElement::Decorator< float > dec_METPtDiff("METPtDiff");
static const SG::AuxElement::Decorator< float > dec_Jvt("Jvt");
static const SG::AuxElement::Decorator< float > dec_fJvt("fJvt");

using namespace asg::msgUserCode;
using xAOD::IParticle;
typedef ElementLink<xAOD::IParticleContainer> iplink_t;
typedef DataVector< IParticle > IParticleContainer;

int main( int argc, char* argv[] ){std::cout << __PRETTY_FUNCTION__ << std::endl;
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();
#else
  IAppMgrUI* app = POOL::Init(); //important to do this first!
#endif

  std::string jetType = "AntiKt4EMTopo";
  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  size_t evtmax = 100;
  std::string jetAux = "";
  bool isDeriv=false;
  bool debug(false);
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-evtmax" && i+1<argc) {
      evtmax = atoi(argv[i+1]);
    }else if (std::string(argv[i]) == "-jetaux" && i+1<argc) {
      jetAux = argv[i+1];
    } else if (std::string(argv[i]) == "-debug") {
      debug = true;
    } else if (std::string(argv[i]) == "-isDeriv") {
      isDeriv = true;
    }
  }

  std::string config = "JES_data2017_2016_2015_Recommendation_Aug2018_rel21.config";
  std::string calibSeq = "JetArea_Residual_EtaJES_GSC";
  std::string calibArea = "00-04-81";
  if(jetType=="AntiKt4EMPFlow"){
    config = "JES_data2017_2016_2015_Recommendation_PFlow_Aug2018_rel21.config";
    calibSeq = "JetArea_Residual_EtaJES_GSC";
    calibArea = "00-04-81";	    
  }

  asg::AnaToolHandle<IJetCalibrationTool> jetCalibrationTool;
  ANA_CHECK( ASG_MAKE_ANA_TOOL( jetCalibrationTool, JetCalibrationTool ) );
  jetCalibrationTool.setName("jetCalibTool");
  ANA_CHECK( jetCalibrationTool.setProperty("JetCollection", jetType) );
  ANA_CHECK( jetCalibrationTool.setProperty("ConfigFile", config) );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibSequence", calibSeq) );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibArea", calibArea) );
  ANA_CHECK( jetCalibrationTool.setProperty("IsData", false) );
  ANA_CHECK( jetCalibrationTool.retrieve() );

  asg::AnaToolHandle<IJetModifier> m_jetFwdJvtTool;
  m_jetFwdJvtTool.setTypeAndName("JetForwardJvtTool/JetForwardJvtTool");
  ANA_CHECK( m_jetFwdJvtTool.setProperty("OutputDec", "passFJvt") ); //Output decoration
  // fJVT WPs depend on the MET WP, see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EtmissRecommendationsRel21p2#fJVT_and_MET
  ANA_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", true) ); // Tight
  ANA_CHECK( m_jetFwdJvtTool.setProperty("EtaThresh", 2.5) );   //Eta dividing central from forward jets
  ANA_CHECK( m_jetFwdJvtTool.setProperty("ForwardMaxPt", 120.0e3) ); //Max Pt to define fwdJets for JVT
  ANA_CHECK( m_jetFwdJvtTool.retrieve() );

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::cout << "Opening: " << fileName << std::endl;
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object to read from file and a transient store in which to place items
#ifdef XAOD_STANDALONE
  std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kClassAccess ) );
  //std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kAthenaAccess ) );
  std::unique_ptr<xAOD::TStore> store(new xAOD::TStore());
#else // Athena "Store" is the same StoreGate used by the TEvent
  std::unique_ptr<POOL::TEvent> event(new POOL::TEvent( POOL::TEvent::kClassAccess ));
  //std::unique_ptr<POOL::TEvent> event(new POOL::TEvent( POOL::TEvent::kAthenaAccess ));
  ServiceHandle<StoreGateSvc>& store = event->evtStore();
#endif
  ANA_CHECK( event->readFrom( ifile.get() ) );

  // declare METSignificance
  
  asg::AnaToolHandle<IMETSignificance> metSignif;
  metSignif.setTypeAndName("met::METSignificance/metSignif");
  ANA_CHECK( metSignif.setProperty("SoftTermParam", met::Random) );
  ANA_CHECK( metSignif.setProperty("TreatPUJets",   true) );
  ANA_CHECK( metSignif.setProperty("DoPhiReso",     true) );
  ANA_CHECK( metSignif.setProperty("IsDataJet",     false) );
  ANA_CHECK( metSignif.setProperty("DoJerForEMu",   false) );
  ANA_CHECK( metSignif.setProperty("JetCollection", jetType) );
  if(jetAux!="")
    ANA_CHECK( metSignif.setProperty("JetResoAux", jetAux) );
  if(debug) ANA_CHECK( metSignif.setProperty("OutputLevel", MSG::VERBOSE) );
  ANA_CHECK( metSignif.retrieve() );
  
  // reconstruct the MET
  asg::AnaToolHandle<IMETMaker> metMaker;
  metMaker.setTypeAndName("met::METMaker/metMaker");
  ANA_CHECK( metMaker.setProperty("DoMuonEloss", false) );
  ANA_CHECK( metMaker.setProperty("DoRemoveMuonJets", true) );
  ANA_CHECK( metMaker.setProperty("DoSetMuonJetEMScale", true) );
  ANA_CHECK( metMaker.retrieve() );

  for(size_t ievent = 0;  ievent < std::min(size_t(event->getEntries()), evtmax); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    ANA_CHECK( event->getEntry(ievent) >= 0 );

    const xAOD::EventInfo* eventinfo = 0;
    ANA_CHECK( event->retrieve( eventinfo, "EventInfo" ) );    

    //---------------------------
    // Truth Containers
    //---------------------------
    const xAOD::JetContainer * truthJets = nullptr;
    xAOD::TruthParticleContainer* truthElectrons = nullptr;
    xAOD::TruthParticleContainer* truthMuons = nullptr;
    const xAOD::TruthParticleContainer* truthTaus = nullptr;
    const xAOD::TruthParticleContainer* truthPhotons = nullptr;
    const xAOD::MissingETContainer* truthMET = nullptr;
    xAOD::MissingETContainer* newMetContainer = new xAOD::MissingETContainer();//nullptr;
    xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
    newMetContainer->setStore(newMetAuxContainer);
    newMetContainer->reserve(50);
    const xAOD::TruthParticleContainer* truthParticles = nullptr;

    //
    // Truth jets
    //
    ANA_CHECK( event->retrieve( truthJets, "AntiKt4TruthJets" ) );

    if(isDeriv){
      //---------------------------
      // Retrieve Truth Electrons
      //---------------------------
      ANA_CHECK( event->retrieve( truthElectrons, "TruthElectrons" ) );
      
      //---------------------------
      // Retrieve Truth Muons
      //---------------------------
      ANA_CHECK( event->retrieve( truthMuons, "TruthMuons" ) );
      
      //---------------------------
      // Retrieve Truth Taus
      //---------------------------
      ANA_CHECK( event->retrieve( truthTaus, "TruthTaus" ) );
      
      //---------------------------
      // Retrieve Truth Photons
      //---------------------------
      ANA_CHECK( event->retrieve( truthPhotons, "TruthPhotons" ) );
    }
    //---------------------------
    // Retrieve Truth MET
    //---------------------------
    ANA_CHECK( event->retrieve( truthMET, "MET_Truth" ) );
      
    //---------------------------
    // Retrieve Truth Particles
    //---------------------------
    ANA_CHECK( event->retrieve( truthParticles, "TruthParticles" ) );
 
    const xAOD::IParticleContainer* truthParticlesCont = static_cast<const IParticleContainer*>(truthParticles);
    //ANA_CHECK( store->record(truthParticlesCont, "TruthPartForMET") );
    //NonInt - all stable, non-interacting particles including neutrinos, SUSY LSPs, Kaluza-Klein particles etc.
    //Int - all stable, interacting particles within detector acceptance (|eta|<5) excluding muons (approximate calo term)
    //IntOut - all stable, interacting particles outside detector acceptance (|eta|>5)
    //IntMuons - all final state muons in |eta|<5 and with pt>6 GeV

    // For testing only - the 4-vector of the neutrino(s)
    const xAOD::MissingET * met_NonInt = (*truthMET)["NonInt"];
    const xAOD::MissingET * met_Int = (*truthMET)["Int"];
    const xAOD::MissingET * met_IntMuons = (*truthMET)["IntMuons"];

    TLorentzVector MET_Vec_Nu, MET_Vec;
    MET_Vec_Nu.SetPtEtaPhiM(met_NonInt->met()*1e-3,0.0,met_NonInt->phi(),0.0);

    TLorentzVector MET_Vec_Int;//Int: all interacting particles till abs(eta) <= 5 (muons excluded) 
    TLorentzVector MET_Vec_IntMuons;// Muons: all truth muons 

    MET_Vec_Int.SetPtEtaPhiM(met_Int->met()*1e-3,0.0,met_Int->phi(),0.0);
    MET_Vec_IntMuons.SetPtEtaPhiM(met_IntMuons->met()*1e-3,0.0,met_IntMuons->phi(),0.0);

    // Add the calo. term and the muon term.
    // Has to be done this way or MET_Vec has an artificial inv. mass!
    TVector3 MET_ThreeVec = MET_Vec_Int.Vect() + MET_Vec_IntMuons.Vect();
    MET_Vec.SetVectM(MET_ThreeVec,0.0);

    // MET Significance. Define the MET that you want. 
    TLorentzVector MET_for_sig = MET_Vec_Int + MET_Vec_IntMuons; // start with interacting met
    //TLorentzVector MET_for_sig = MET_Vec_Nu; //Here we start with the neutrino MET
    if(debug) std::cout << "MET_for_sig: " << MET_for_sig.Pt() << std::endl;

    double sumet = 0;

    // overlap vector. very basic overlap is applied. should be handled by the analysis teams
    std::vector<TLorentzVector> overlap;

    // declare the MET containers
    static SG::AuxElement::Decorator< std::vector<iplink_t> > dec_constitObjLinks("ConstitObjectLinks");
    xAOD::MissingET* met_muo = new xAOD::MissingET();
    newMetContainer->push_back(met_muo);
    met_muo->setName("RefMuon");
    met_muo->setSource(MissingETBase::Source::muon());
    dec_constitObjLinks(*met_muo) = std::vector<iplink_t>(0); // declare
    std::vector<iplink_t>& uniqueLinksMuo = dec_constitObjLinks(*met_muo);
    uniqueLinksMuo.reserve(20);
    xAOD::MissingET* met_electron = new xAOD::MissingET();
    newMetContainer->push_back(met_electron);
    met_electron->setName("RefEle");
    met_electron->setSource(MissingETBase::Source::electron());
    dec_constitObjLinks(*met_electron) = std::vector<iplink_t>(0); // declare
    std::vector<iplink_t>& uniqueLinksEle = dec_constitObjLinks(*met_electron);
    uniqueLinksEle.reserve(20);
    xAOD::MissingET* met_tau = new xAOD::MissingET();
    newMetContainer->push_back(met_tau);
    met_tau->setName("RefTau");
    dec_constitObjLinks(*met_tau) = std::vector<iplink_t>(0); // declare
    std::vector<iplink_t>& uniqueLinksTau = dec_constitObjLinks(*met_tau);  
    uniqueLinksTau.reserve(20);  
    met_tau->setSource(MissingETBase::Source::tau());
    xAOD::MissingET* met_photons = new xAOD::MissingET();
    newMetContainer->push_back(met_photons);
    met_photons->setName("RefPhotons");
    met_photons->setSource(MissingETBase::Source::photon());
    dec_constitObjLinks(*met_photons) = std::vector<iplink_t>(0); // declare
    std::vector<iplink_t>& uniqueLinksPh = dec_constitObjLinks(*met_photons);
    uniqueLinksPh.reserve(20);  
    // Loading the physics objects into the MissingET container
    if(isDeriv){
      SG::sgkey_t objkeyMuo = iplink_t(*static_cast<const xAOD::IParticleContainer*>(truthMuons->front()->container()),0).key();
      for (const auto truth_muon: *truthMuons){
	const xAOD::TruthParticle *muon = static_cast<const xAOD::TruthParticle*>(truth_muon);
	sumet += muon->pt();
	uniqueLinksMuo.emplace_back( iplink_t(objkeyMuo,muon->index()) );
      }
      SG::sgkey_t objkeyEle = iplink_t(*static_cast<const xAOD::IParticleContainer*>(truthElectrons->front()->container()),0).key();
      for (const auto truth_electron: *truthElectrons){
	const xAOD::TruthParticle *electron = static_cast<const xAOD::TruthParticle*>(truth_electron);
	sumet += electron->pt();
	uniqueLinksEle.emplace_back( iplink_t(objkeyEle,electron->index()) );
	overlap.push_back(electron->p4());
      }
      SG::sgkey_t objkeyTau = iplink_t(*static_cast<const xAOD::IParticleContainer*>(truthTaus->front()->container()),0).key();
      for (const auto truth_tau: *truthTaus){
	const xAOD::TruthParticle *tau = static_cast<const xAOD::TruthParticle*>(truth_tau);
	sumet += tau->pt();
	uniqueLinksTau.emplace_back( iplink_t(objkeyTau,tau->index()) );
	overlap.push_back(tau->p4());
      }
      SG::sgkey_t objkeyPh = iplink_t(*static_cast<const xAOD::IParticleContainer*>(truthPhotons->front()->container()),0).key();
      for (const auto truth_photon: *truthPhotons){
	const xAOD::TruthParticle *photon = static_cast<const xAOD::TruthParticle*>(truth_photon);
	sumet += photon->pt();
	uniqueLinksPh.emplace_back( iplink_t(objkeyPh,photon->index()) );
	overlap.push_back(photon->p4());
      }
    }else{ // read in the truth particles
      SG::sgkey_t objkey = iplink_t(*static_cast<const xAOD::IParticleContainer*>(truthParticles->front()->container()),0).key();
      for (const auto truth_part: *truthParticles){
	const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(truth_part);
	if(part && part->status()==1 && part->pt()>3.0e3){
	  if(debug) std::cout << "particle: " << part->pdgId() << " " << part->pt() << std::endl;
	  if(fabs(part->pdgId())==11 && fabs(part->eta())<2.37 && part->pt()>5.0e3){
	    if(debug) std::cout << "ELE particle: " << part->pdgId() << " " << part->pt() 
				<< " index: " << part->index() << " size: " << truthParticles->size() 
				<< std::endl;
	    if(debug) std::cout << "   valid pointer? " << truthParticles->at(part->index()) << std::endl;
	    sumet += part->pt();
	    uniqueLinksEle.emplace_back( iplink_t(objkey,part->index()) );
	    overlap.push_back(part->p4());
	  }
	  if(fabs(part->pdgId())==13 && fabs(part->eta())<2.7 && part->pt()>5.0e3){
	    if(debug) std::cout << "MUO particle: " << part->pdgId() << " " << part->pt() << std::endl;
	    sumet += part->pt();
	    if(debug) std::cout << "   valid pointer muo? " << truthParticlesCont->at(truth_part->index()) 
				<< " index: " << part->index() << " size: " << truthParticles->size() 
				<< std::endl;
	    uniqueLinksMuo.emplace_back( iplink_t(objkey,part->index()) );
	    if(debug) std::cout << "Added MUO particle: " << part->pdgId() << " " << part->pt() << std::endl;
	  }
	  if(fabs(part->pdgId())==15 && fabs(part->eta())<2.4 && part->pt()>5.0e3){
	    if(debug) std::cout << "TAU particle: " << part->pdgId() << " " << part->pt() << std::endl;
	    sumet += part->pt();
	    uniqueLinksTau.emplace_back( iplink_t(objkey,part->index()) );
	    overlap.push_back(part->p4());
	  }
	  if(fabs(part->pdgId())==22 && fabs(part->eta())<2.37 && part->pt()>20.0e3){
	    if(debug) std::cout << "TAU particle: " << part->pdgId() << " " << part->pt() << std::endl;
	    sumet += part->pt();
	    uniqueLinksPh.emplace_back( iplink_t(objkey,part->index()) );
	    overlap.push_back(part->p4());
	  }
	}
      }
    }

    //Re-creating object links defining fake containers for each reconstructed object 
    xAOD::MissingET* met_jet = new xAOD::MissingET();
    newMetContainer->push_back(met_jet);
    met_jet->setName("RefJet");
    met_jet->setSource(MissingETBase::Source::jet());
    dec_constitObjLinks(*met_jet) = std::vector<iplink_t>(0); // declare
    std::vector<iplink_t>& uniqueLinks = dec_constitObjLinks(*met_jet);
    for (const auto truth_jet: *truthJets){
      const xAOD::Jet *jet = static_cast<const xAOD::Jet*>(truth_jet);
      bool isOverlapping=false;
      if(!(fabs(jet->eta())<4.5 && jet->pt()>20.0e3)) continue;
      for(unsigned o=0; o<overlap.size(); ++o){ if(overlap.at(o).DeltaR(jet->p4())<0.3) isOverlapping=true; break; }
      if(isOverlapping) continue;
      dec_Jvt(*jet)=1.0;
      dec_fJvt(*jet)=0.0;
      sumet += jet->pt();
      uniqueLinks.emplace_back( iplink_t(*static_cast<const IParticleContainer*>(jet->container()),jet->index()) );
    }

    xAOD::MissingET* met_tot_trk = new xAOD::MissingET();
    newMetContainer->push_back(met_tot_trk);
    met_tot_trk->setName("FinalTrk");
    met_tot_trk->setSource(MissingETBase::Source::total());
    met_tot_trk->add(MET_for_sig.Px()*1000,MET_for_sig.Py()*1000,MET_for_sig.Pt()*1000); 
    met_tot_trk->setSumet(sumet);

    xAOD::MissingET* met_tot_soft = new xAOD::MissingET();
    newMetContainer->push_back(met_tot_soft);
    met_tot_soft->setName("PVSoftTrk");
    met_tot_soft->setSource(MissingETBase::Source::softEvent()+MissingETBase::Source::Track);
    met_tot_soft->add(1.0,0,1.0);//VF: Setting Soft Term to 1 MeV (Doug's suggestion)
  
    // Run MET significance    
    ANA_CHECK( metSignif->varianceMET(newMetContainer, eventinfo->averageInteractionsPerCrossing(), "RefJet", "PVSoftTrk","FinalTrk"));

    if(debug){
      if(newMetContainer->find("Muons")!=newMetContainer->end())
	std::cout << "Muon term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("Muons")))->met()
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("Muons")))->phi() 
		  << std::endl;
      if(newMetContainer->find("MuonEloss")!=newMetContainer->end())
	std::cout << "MuonEloss term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("MuonEloss")))->met()
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("MuonEloss")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefJet")!=newMetContainer->end())
	std::cout << "Jet term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefJet")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefJet")))->phi() 
		  << std::endl;
      
      if(newMetContainer->find("RefEle")!=newMetContainer->end())
	std::cout << "Ele term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefEle")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefEle")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefTau")!=newMetContainer->end())
	std::cout << "Tau term: "   << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefTau")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefTau")))->phi() 
		  << std::endl;
      if(newMetContainer->find("RefPhoton")!=newMetContainer->end())
	std::cout << "Gamma term: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefPhoton")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("RefPhoton")))->phi() 
		  << std::endl;
      if(newMetContainer->find("PVSoftTrk")!=newMetContainer->end())
	std::cout << "Soft term: "  << static_cast<xAOD::MissingET*>(*(newMetContainer->find("PVSoftTrk")))->met() 
		  << " phi: " << static_cast<xAOD::MissingET*>(*(newMetContainer->find("PVSoftTrk")))->phi() 
		  << std::endl;

      // Print the METSignificance terms (e.g. jet, muon, ele, pho, etc)
      // NOTE::: these are not currently rotated if the MET is rotated
      std::cout << "  jet   VarL: " << metSignif->GetTermVarL(met::ResoJet) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoJet) << " GeV" << std::endl;
      std::cout << "  muon   VarL: " << metSignif->GetTermVarL(met::ResoMuo) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoMuo) << " GeV" << std::endl;
      std::cout << "  electron   VarL: " << metSignif->GetTermVarL(met::ResoEle) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoEle) << " GeV" << std::endl;
      std::cout << "  photon   VarL: " << metSignif->GetTermVarL(met::ResoPho) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoPho) << " GeV" << std::endl;
      std::cout << "  tau   VarL: " << metSignif->GetTermVarL(met::ResoTau) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoTau) << " GeV" << std::endl;
      std::cout << "  Soft term   VarL: " << metSignif->GetTermVarL(met::ResoSoft) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoSoft) << " GeV" << std::endl;
      std::cout << "  other/bug   VarL: " << metSignif->GetTermVarL(met::ResoNone) << " GeV VarT: " << metSignif->GetTermVarT(met::ResoNone) << " GeV" << std::endl;

    }
      
    // extracting the MET significance
    std::cout << "MET significance: " << metSignif->GetSignificance() << std::endl;

    if(debug){
      // Try a rotation to a new lambda parameter
      std::cout << " Lambda Test Before: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
      metSignif->SetLambda(0.0, 0.0);
      std::cout << " Lambda Test 0: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
      metSignif->SetLambda(10.0, 10.0);
      std::cout << " Lambda Test 10: " << metSignif->GetSignificance() << " VarL: " << metSignif->GetVarL() << " VarT: " << metSignif->GetVarT() << std::endl;
    }

    ANA_CHECK(store->record( newMetContainer,    "FinalMETContainer"    ));
    ANA_CHECK(store->record( newMetAuxContainer, "FinalMETContainerAux."));

#ifdef XAOD_STANDALONE // POOL::TEvent should handle this when changing events
    //fill the containers stored in the event
    //to the output file and clear the transient store
    event->fill();
    store->clear();
#endif
  }

#ifndef XAOD_STANDALONE // POOL::TEvent should handle this when changing events
  app->finalize();
#endif

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
 }

#endif

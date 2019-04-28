/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : February 2015
#ifndef XAOD_ANALYSIS
#include <cstdio>
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

#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODBase/IParticleHelpers.h"

#include <memory>
#undef NDEBUG
#include <cassert>
#include "TFile.h"
#include "TSystem.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METInterface/IMETSystematicsTool.h"
#include "METInterface/IMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "PATInterfaces/SystematicRegistry.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace asg::msgUserCode;

int main( int argc, char* argv[]) {std::cout << __PRETTY_FUNCTION__ << std::endl;
  // Initialize the application
#ifdef XAOD_STANDALONE
  
  //enable status code failures
  CP::CorrectionCode::enableFailure();
  CP::SystematicCode::enableFailure();
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();
  xAOD::Init() ;
#else
  IAppMgrUI* app = POOL::Init(); //important to do this first!
#endif

  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  std::string jetType = "AntiKt4EMTopo";
  bool debug = false;
  bool calibjets = true;
  size_t evtmax = 100;
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-nocalib") { // useful for checking smart slimming content
      calibjets = false;
    } else if (std::string(argv[i]) == "-evtmax" && i+1<argc) {
      evtmax = atoi(argv[i+1]);
    } else if (std::string(argv[i]) == "-debug") {
      debug = true;
    }
  }

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object to read from file and a transient store in which to place items
#ifdef XAOD_STANDALONE
  std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kClassAccess ) );
  std::unique_ptr<xAOD::TStore> store(new xAOD::TStore());
#else // Athena "Store" is the same StoreGate used by the TEvent
  std::unique_ptr<POOL::TEvent> event(new POOL::TEvent( POOL::TEvent::kClassAccess ));
  ServiceHandle<StoreGateSvc>& store = event->evtStore();
#endif
  ANA_CHECK( event->readFrom( ifile.get() ) );

  // creation and set properties of the tools
  // if you need to set properties, you should do so before initialization

  asg::AnaToolHandle<IJetCalibrationTool> jetCalibrationTool;
  ANA_CHECK( ASG_MAKE_ANA_TOOL( jetCalibrationTool, JetCalibrationTool ) );
  jetCalibrationTool.setName("jetCalibTool");
  ANA_CHECK( jetCalibrationTool.setProperty("JetCollection", jetType) );
  ANA_CHECK( jetCalibrationTool.setProperty("ConfigFile", "JES_data2017_2016_2015_Recommendation_Feb2018_rel21.config") );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibSequence", "JetArea_Residual_EtaJES_GSC") );
  ANA_CHECK( jetCalibrationTool.setProperty("IsData", false) );
  ANA_CHECK( jetCalibrationTool.retrieve() );

  asg::AnaToolHandle<IMETSystematicsTool> metSystTool;
  metSystTool.setTypeAndName("met::METSystematicsTool/metSystTool");
  // ANA_CHECK( metSystTool.setProperty("UseDevArea"      ,true )); // To get the configs from GROUPDATA/dev/METUtilities
  ANA_CHECK( metSystTool.retrieve() );

  asg::AnaToolHandle<IMETMaker> metMaker;
  metMaker.setTypeAndName("met::METMaker/metMaker");
  ANA_CHECK( metMaker.setProperty("DoMuonEloss", false) ); // currently under investigation. recommend false unless you see too many jets being removed
  ANA_CHECK( metMaker.setProperty("DoRemoveMuonJets", true) );
  ANA_CHECK( metMaker.setProperty("DoSetMuonJetEMScale", true) );
  ANA_CHECK( metMaker.retrieve() );

  for(size_t ievent = 0;  ievent < std::min(size_t(event->getEntries()), evtmax); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    ANA_CHECK( event->getEntry(ievent) >= 0 );

    //retrieve the original containers

    //this contains all constituents not associated to a jet/misc term
    const xAOD::MissingETContainer* coreMet  = nullptr;
    std::string coreMetKey = "MET_Core_" + jetType;
    ANA_CHECK( event->retrieve(coreMet, coreMetKey) );
    if(debug) std::cout << "Using core MET " << coreMet << std::endl;

    const xAOD::ElectronContainer* electrons = nullptr;
    ANA_CHECK( event->retrieve(electrons, "Electrons") );

    const xAOD::MuonContainer* muons = nullptr;
    ANA_CHECK( event->retrieve(muons, "Muons") );

    const xAOD::PhotonContainer* photons = nullptr;
    ANA_CHECK( event->retrieve(photons, "Photons"));

    const xAOD::TauJetContainer* taus = nullptr;
    ANA_CHECK( event->retrieve(taus, "TauJets"));

    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* uncalibJets = nullptr;
    ANA_CHECK( event->retrieve(uncalibJets, jetType+"Jets"));//this retrieves and applies the correction
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > calibJetsPair = xAOD::shallowCopyContainer( *uncalibJets );//make a shallow copy to calibrate
    xAOD::JetContainer *& calibJets = calibJetsPair.first;//create a reference to the first element of the pair (i.e. the JetContainer)

    met::addGhostMuonsToJets(*muons, *calibJets);
    if(calibjets) {
      for ( const auto& jet : *calibJets ) {
	//Shallow copy is needed (see links below)
	if(!jetCalibrationTool->applyCalibration(*jet))//apply the calibration
	  return 1;
      }
    }
    if(!xAOD::setOriginalObjectLink(*uncalibJets, *calibJets)){//tell calib container what old container it matches
      if(debug) std::cout << "Failed to set the original object links" << std::endl;
      return 1;
    }
    ANA_CHECK( store->record(calibJets, "CalibJets") );

    //retrieve the MET association map
    const xAOD::MissingETAssociationMap* metMap = nullptr;
    std::string metAssocKey = "METAssoc_" + jetType;
    ANA_CHECK( event->retrieve(metMap, metAssocKey) );

    //get the set of recommended systematics
    const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    const CP::SystematicSet&      recSysList  = registry.recommendedSystematics();

    //start the loop over systematics
    //For each systematic, we create an output MissingET container
    for(CP::SystematicSet::const_iterator isys = recSysList.begin();
	isys != recSysList.end();
	++isys) {	// print the systematics on the first event
      // When only using the METSystematicsTool, this will be the list of recommended systematics for that tool
      if(ievent == 0) 
	if(debug) std::cout << "Doing systematic : " << (*isys).name() << std::endl;

      // Create a MissingETContainer with its aux store for each systematic
      xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
      xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
      newMetContainer->setStore(newMetAuxContainer);

      // It is necessary to reset the selected objects before every MET calculation
      metMap->resetObjSelectionFlags();

      //here we apply some basic cuts and rebuild the met at each step
      //Electrons
      ConstDataVector<xAOD::ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *electrons) {
	if(el->pt()>20e3 && el->eta()<2.47) metElectrons.push_back(el);
      }
      ANA_CHECK( metMaker->rebuildMET("RefEle",                   //name of metElectrons in metContainer
				 xAOD::Type::Electron,       //telling the rebuilder that this is electron met
				 newMetContainer,            //filling this met container
				 metElectrons.asDataVector(),//using these metElectrons that accepted our cuts
				 metMap)                     //and this association map
	     );

      //Photons
      ConstDataVector<xAOD::PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
      for(const auto& ph : *photons) {
	if(ph->pt()>20e3 && ph->eta()<2.47) metPhotons.push_back(ph);
      }
      ANA_CHECK( metMaker->rebuildMET("RefPhoton",
				 xAOD::Type::Photon,
				 newMetContainer,
				 metPhotons.asDataVector(),
				 metMap)
	     );

      //Taus
      ConstDataVector<xAOD::TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
      for(const auto& tau : *taus) {
	if(tau->pt()>20e3 && tau->eta()<2.5) metTaus.push_back(tau);
      }
      ANA_CHECK( metMaker->rebuildMET("RefTau",
				 xAOD::Type::Tau,
				 newMetContainer,
				 metTaus.asDataVector(),
				 metMap)
	     );

      //Muons
      ConstDataVector<xAOD::MuonContainer> metMuons(SG::VIEW_ELEMENTS);
      for(const auto& mu : *muons) {
	if(mu->pt()>20e3 && mu->eta()<2.4) metMuons.push_back(mu);
      }
      ANA_CHECK( metMaker->rebuildMET("Muons",
				 xAOD::Type::Muon,
				 newMetContainer,
				 metMuons.asDataVector(),
				 metMap)
	     );

      //Now time to rebuild jetMet and get the soft term
      //This adds the necessary soft term for both CST and TST
      //these functions create an xAODMissingET object with the given names inside the container
      ANA_CHECK( metMaker->rebuildJetMET("RefJet",        //name of jet met
				     "SoftClus",      //name of soft cluster term met
				     "PVSoftTrk",     //name of soft track term met
				     newMetContainer, //adding to this new met container
				     calibJets,       //using this jet collection to calculate jet met
				     coreMet,         //core met container
				     metMap,          //with this association map
				      true            //apply jet jvt cut
				     )
	     );

      //this is kind of annoying, but applySystematicVariation only takes a SystematicSet, but *isys is a SystematicVariation.
      //We use the SystematicSet constructor which just takes a SystematicVariation
      CP::SystematicSet iSysSet( (*isys).name());
      if(debug) std::cout << "Syst: " << (*isys).name() << std::endl;
      //tell the tool that we are using this SystematicSet (of one SystematicVariation for now)
      //after this call, when we use applyCorrection, the given met term will be adjusted with this systematic applied
      ANA_CHECK( metSystTool->applySystematicVariation(iSysSet) );

      //get the soft cluster term, and applyCorrection
      xAOD::MissingET * softClusMet = (*newMetContainer)["SoftClus"];
      if(debug) std::cout << "Soft cluster met term met " << softClusMet->met() << std::endl;
      ANA_CHECK( softClusMet != nullptr); //check we retrieved the clust term
      ANA_CHECK( metSystTool->applyCorrection(*softClusMet) );

      xAOD::MissingET * softTrkMet = (*newMetContainer)["PVSoftTrk"];
      if(debug) std::cout << "Soft track met term met before " << softTrkMet->met() << std::endl;
      ANA_CHECK( softTrkMet != nullptr); //check we retrieved the soft trk
      ANA_CHECK( metSystTool->applyCorrection(*softTrkMet) );
      if(debug) std::cout << "Soft track met term met after " << softTrkMet->met() << std::endl;

      //this builds the final track or cluster met sums, using systematic varied container
      //In the future, you will be able to run both of these on the same container to easily output CST and TST
      ANA_CHECK( metMaker->buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
      //ANA_CHECK( metMaker->buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

      //we record the container to the store, with a systematic indicated name
      ANA_CHECK( store->record( newMetContainer,    "FinalMETContainer_" + iSysSet.name()      ));
      ANA_CHECK( store->record( newMetAuxContainer, "FinalMETContainer_" + iSysSet.name() + "Aux."));

      if(debug) {
	std::cout << "total MET: " << (*newMetContainer)["FinalTrk"]->met() << std::endl;

	xAOD::MissingET * jetMet = (*newMetContainer)["RefJet"];
	const std::vector<float>& jetweights = jetMet->auxdataConst<std::vector<float> >("ConstitObjectWeights");
	const std::vector<ElementLink<xAOD::IParticleContainer> >& constitjets = jetMet->auxdataConst<std::vector<ElementLink<xAOD::IParticleContainer> > >("ConstitObjectLinks");
	for(size_t iconstit=0; iconstit < jetweights.size(); ++iconstit) {
	  const xAOD::Jet* constjet = static_cast<const xAOD::Jet*>(*constitjets[iconstit]);
	  const float jetweight = jetweights[iconstit];
	  std::cout << "RefJet jet " << constjet->index() << ", weight " << jetweight << ", pt: " << constjet->pt() << std::endl;
	}
      }
    }

#ifdef XAOD_STANDALONE
    //fill the containers stored in the event
    //to the output file and clear the transient store
    assert( event->fill());
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

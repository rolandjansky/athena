/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : May 2015
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
#include "xAODMissingET/MissingETAssociationHelper.h"

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

#include "METInterface/IMETMaker.h"
#include "METUtilities/CutsMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace asg::msgUserCode;

int main( int argc, char* argv[] ){std::cout << __PRETTY_FUNCTION__ << std::endl;
#ifdef XAOD_STANDALONE
  //enable status code failures
  //  CP::CorrectionCode::enableFailure();
  //  CP::SystematicCode::enableFailure();
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();
  xAOD::Init() ;
#else
  IAppMgrUI* app = POOL::Init(); //important to do this first!
#endif

  std::string jetType = "AntiKt4EMTopo";
  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  size_t evtmax = 100;
  bool debug(false);
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-evtmax" && i+1<argc) {
      evtmax = atoi(argv[i+1]);
    } else if (std::string(argv[i]) == "-debug") {
      debug = true;
    }
  }

  asg::AnaToolHandle<IJetCalibrationTool> jetCalibrationTool;
  ANA_CHECK( ASG_MAKE_ANA_TOOL( jetCalibrationTool, JetCalibrationTool ) );
  jetCalibrationTool.setName("jetCalibTool");
  ANA_CHECK( jetCalibrationTool.setProperty("JetCollection", jetType) );
  ANA_CHECK( jetCalibrationTool.setProperty("ConfigFile", "JES_MC15cRecommendation_May2016_rel21.config") );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibSequence", "JetArea_Residual_EtaJES_GSC") );
  ANA_CHECK( jetCalibrationTool.setProperty("IsData", false) );
  ANA_CHECK( jetCalibrationTool.retrieve() );

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
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

  asg::AnaToolHandle<IMETMaker> metMaker;
  metMaker.setTypeAndName("met::METMaker/metMaker");
  ANA_CHECK( metMaker.setProperty("DoMuonEloss", true) );
  ANA_CHECK( metMaker.setProperty("DoRemoveMuonJets", true) );
  ANA_CHECK( metMaker.setProperty("DoSetMuonJetEMScale", true) );
  ANA_CHECK( metMaker.retrieve() );

  for(size_t ievent = 0;  ievent < std::min(size_t(event->getEntries()), evtmax); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    ANA_CHECK( event->getEntry(ievent) >= 0 );

    //retrieve the original containers
    const xAOD::MissingETContainer* coreMet  = nullptr;
    std::string coreMetKey = "MET_Core_" + jetType;
    ANA_CHECK( event->retrieve(coreMet, coreMetKey) );
    if(debug) std::cout << "Using core MET " << coreMet << std::endl;

    //if you wanted to make a particle invisible to MET, i.e., remove the particle and
    //its clusters/tracks from the MET calculation, you can make a container of invisible particles
    //and then use selectIfNoOverlaps (below)
    //in this example, we will treat electrons as invisible
    const xAOD::ElectronContainer* invisibleElectrons = nullptr;
    ANA_CHECK( event->retrieve(invisibleElectrons, "Electrons") );

    const xAOD::MuonContainer* muons = nullptr;
    ANA_CHECK( event->retrieve(muons, "Muons") );

    const xAOD::PhotonContainer* photons = nullptr;
    ANA_CHECK( event->retrieve(photons, "Photons"));

    const xAOD::TauJetContainer* taus = nullptr;
    ANA_CHECK( event->retrieve(taus, "TauJets"));

    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* jets = nullptr;
    ANA_CHECK( event->retrieve(jets, jetType+"Jets"));

    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *jets );
    ANA_CHECK(store->record( jets_shallowCopy.first,  "CalibJets"    ));
    ANA_CHECK(store->record( jets_shallowCopy.second, "CalibJetsAux."));
    //this is a non-const copy of the jet collection that you can calibrate.
    xAOD::JetContainer* calibJets = jets_shallowCopy.first;
    xAOD::setOriginalObjectLink(*jets,*calibJets);
    //Shallow copy is needed (see links below)
    if(jetCalibrationTool->applyCalibration(*calibJets).isFailure())//apply the calibration
      return 1;

    //retrieve the MET association map
    const xAOD::MissingETAssociationMap* metMap = nullptr;
    std::string metAssocKey = "METAssoc_" + jetType;
    ANA_CHECK( event->retrieve(metMap, metAssocKey) );

    xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
    xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
    newMetContainer->setStore(newMetAuxContainer);

    xAOD::MissingETAssociationHelper metHelper(metMap);

    //here we apply some basic cuts and rebuild the met at each step
    //InvisibleElectrons
    if(!invisibleElectrons->empty()){
      ConstDataVector<xAOD::ElectronContainer> metInvisibleElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *invisibleElectrons) {
	if(CutsMETMaker::accept(el)){
	  metInvisibleElectrons.push_back(el);
	}
      }
      //this line will mark the electron as invisible if it passes the (inv) electron selection cut
      //this removes the particle and associated clusters from the jet and soft term calculations
      ANA_CHECK( metMaker->markInvisible(metInvisibleElectrons.asDataVector(),&metHelper,newMetContainer) );
      // NOTE: Objects marked as invisible should not also be added as part
      // of another term! However, you can e.g. mark some electrons invisible
      // and compute RefEle with others.
    }

    //Photons
    ConstDataVector<xAOD::PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
    for(const auto& ph : *photons) {
      if(CutsMETMaker::accept(ph)) metPhotons.push_back(ph);
    }
    ANA_CHECK(metMaker->rebuildMET("RefPhoton",
			       xAOD::Type::Photon,
			       newMetContainer,
			       metPhotons.asDataVector(),
			       &metHelper)
	   );
    //Taus
    ConstDataVector<xAOD::TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
    for(const auto& tau : *taus) {
      if(CutsMETMaker::accept(tau)) metTaus.push_back(tau);
    }
    ANA_CHECK(metMaker->rebuildMET("RefTau",
			       xAOD::Type::Tau,
			       newMetContainer,
			       metTaus.asDataVector(),
			       &metHelper)
	   );
    
    //Muons
    ConstDataVector<xAOD::MuonContainer> metMuons(SG::VIEW_ELEMENTS);
    for(const auto& mu : *muons) {
      //      if(CutsMETMaker::accept(mu)) metMuons.push_back(mu);
      if(mu->muonType()==xAOD::Muon::Combined && mu->pt()>10e3) metMuons.push_back(mu);
    }
    ANA_CHECK(metMaker->rebuildMET("Muons",
			       xAOD::Type::Muon,
			       newMetContainer,
			       metMuons.asDataVector(),
			       &metHelper)
	   );
    
    met::addGhostMuonsToJets(*muons, *calibJets);

    //Now time to rebuild jetMet and get the soft term
    //these functions create an xAODMissingET object with the given names inside the container
    ANA_CHECK( metMaker->rebuildJetMET("RefJet",        //name of jet met
				    "SoftClus",      //name of soft cluster term met
				    "PVSoftTrk",     //name of soft track term met
				    newMetContainer, //adding to this new met container
				    calibJets,       //using this jet collection to calculate jet met
				    coreMet,         //core met container
				    &metHelper,          //with this association map
				    false            //don't apply jet jvt cut
				    )
	     );

    ANA_CHECK( metMaker->rebuildTrackMET("RefJetTrk",    //name of jet track met
				      "PVSoftTrk",	  //name of soft track term met
				      newMetContainer,//adding to this new met container
				      calibJets,	  //using this jet collection to calculate jet track met
				      coreMet,	  //core met container
				      &metHelper,	  //with this association map
				      false		  //don't apply jet jvt cut
				      )
	     );


    //this builds the final track and cluster met sums, using systematic varied container
    ANA_CHECK( met::buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
    ANA_CHECK( met::buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

    ANA_CHECK(store->record( newMetContainer,    "FinalMETContainer"    ));
    ANA_CHECK(store->record( newMetAuxContainer, "FinalMETContainerAux."));

#ifdef XAOD_STANDALONE // POOL::TEvent should handle this when changing events
    //fill the containers stored in the event
    //to the output file and clear the transient store
    assert(event->fill());
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

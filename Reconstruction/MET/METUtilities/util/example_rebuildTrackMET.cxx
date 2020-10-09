/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : February 2015
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
#include "AsgMessaging/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationHelper.h"

#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"

#undef NDEBUG
#include <cassert>
#include <memory>
#include "TFile.h"
#include "TSystem.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METUtilities/METSystematicsTool.h"
#include "METUtilities/METMaker.h"
#include "METUtilities/METHelpers.h"

#include "PATInterfaces/SystematicRegistry.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace asg::msgUserCode;

int main( int argc, char* argv[]) {std::cout << __PRETTY_FUNCTION__ << std::endl;
  ANA_CHECK_SET_TYPE (int);

#ifdef XAOD_STANDALONE
  ANA_CHECK (xAOD::Init()) ;
  //enable status code failures
  CP::CorrectionCode::enableFailure();
  CP::SystematicCode::enableFailure();
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();
#else
  IAppMgrUI* app = POOL::Init(); //important to do this first!
#endif

  bool invisEle(false);
  bool debug(false);
  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  std::string jetType = "AntiKt4EMTopo";
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-invisEle") {
      invisEle=true;
    } else if (std::string(argv[i]) == "-debug") {
      debug = true;
    }
  }

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object to read from file and a transient store in which to place items
#ifdef XAOD_STANDALONE
  std::unique_ptr<xAOD::TEvent> event(new xAOD::TEvent( xAOD::TEvent::kClassAccess ) );
  std::unique_ptr<xAOD::TStore> store (new xAOD::TStore());
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
  ANA_CHECK( jetCalibrationTool.setProperty("ConfigFile", "JES_MC15cRecommendation_May2016_rel21.config") );
  ANA_CHECK( jetCalibrationTool.setProperty("CalibSequence", "JetArea_Residual_EtaJES_GSC") );
  ANA_CHECK( jetCalibrationTool.setProperty("IsData", false) );
  ANA_CHECK( jetCalibrationTool.retrieve() );

  asg::AnaToolHandle<IMETSystematicsTool> metSystTool;
  metSystTool.setTypeAndName("met::METSystematicsTool/metSystTool");
  ANA_CHECK( metSystTool.setProperty("ConfigJetTrkFile","JetTrackSyst.config" ));//this is needed to do jet track systematics
  // ANA_CHECK( metSystTool.setProperty("UseDevArea"      ,true )); // To get the configs from GROUPDATA/dev/METUtilities
  ANA_CHECK( metSystTool.retrieve() );

  asg::AnaToolHandle<IMETMaker> metMaker;
  metMaker.setTypeAndName("met::METMaker/metMaker");
  ANA_CHECK( metMaker.setProperty("DoMuonEloss", true) );
  ANA_CHECK( metMaker.retrieve() );  

  for(Long64_t ievent = 0;  ievent < std::min(int(event->getEntries()), 100); ++ievent){
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

    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* uncalibJets = nullptr;
    ANA_CHECK( event->retrieve(uncalibJets, jetType+"Jets"));//this retrieves and applies the correction
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > calibJetsPair = xAOD::shallowCopyContainer( *uncalibJets );//make a shallow copy to calibrate
    xAOD::JetContainer *& calibJets = calibJetsPair.first;//create a reference to the first element of the pair (i.e. the JetContainer)
    //Shallow copy is needed (see links below)
    if(jetCalibrationTool->applyCalibration(*calibJets).isFailure())//apply the calibration
      return 1;
    if(!xAOD::setOriginalObjectLink(*uncalibJets, *calibJets)){//tell calib container what old container it matches
      std::cout << "Failed to set the original object links" << std::endl;
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
      //      if(ievent == 0) 

      // Create a MissingETContainer with its aux store for each systematic
      xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
      xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
      newMetContainer->setStore(newMetAuxContainer);

      xAOD::MissingETAssociationHelper metHelper(metMap);
      //here we apply some basic cuts and rebuild the met at each step
      //Electrons
      ConstDataVector<xAOD::ElectronContainer> invisElectrons(SG::VIEW_ELEMENTS);
      ConstDataVector<xAOD::ElectronContainer> metElectrons(SG::VIEW_ELEMENTS);
      for(const auto& el : *electrons) {
	if(el->pt()>20e3 && el->eta()<2.47) {
	  if(invisEle) {
	    invisElectrons.push_back(el);
	  }// else {
	  metElectrons.push_back(el);
	    //}
	}
      }
      if(!invisElectrons.empty()){
	ANA_CHECK( metMaker->markInvisible(invisElectrons.asDataVector(),&metHelper,newMetContainer) );
      }

      ANA_CHECK( metMaker->rebuildMET("TrkEle",                   //name of metElectrons in metContainer
				 xAOD::Type::Electron,       //telling the rebuilder that this is electron met
				 newMetContainer,            //filling this met container
				 metElectrons.asDataVector(),//using these metElectrons that accepted our cuts
				 &metHelper)                     //and this association map
	     );

      //Muons
      ConstDataVector<xAOD::MuonContainer> metMuons(SG::VIEW_ELEMENTS);
      for(const auto& mu : *muons) {
	if(mu->pt()>20e3 && mu->eta()<2.4) metMuons.push_back(mu);
      }
      ANA_CHECK( metMaker->rebuildMET("TrkMuons",
				 xAOD::Type::Muon,
				 newMetContainer,
				 metMuons.asDataVector(),
				 &metHelper)
	     );

      // for rebuilding track MET
      ANA_CHECK(  metMaker->rebuildTrackMET("TrkJet",    //name of jet track met
      				  "PVSoftTrk",	  //name of soft track term met
      				  newMetContainer,//adding to this new met container
      				  calibJets,	  //using this jet collection to calculate jet track met
      				  coreMet,	  //core met container
      				  &metHelper,	  //with this association map
      				  false		  //don't apply jet jvt cut
      				  )
      	 );

      //this is kind of annoying, but applySystematicVariation only takes a SystematicSet, but *isys is a SystematicVariation.
      //We use the SystematicSet constructor which just takes a SystematicVariation
      CP::SystematicSet iSysSet( (*isys).name());
      //tell the tool that we are using this SystematicSet (of one SystematicVariation for now)
      //after this call, when we use applyCorrection, the given met term will be adjusted with this systematic applied
      ANA_CHECK( metSystTool->applySystematicVariation(iSysSet) );

      metSystTool->setRandomSeed( int(1e6*(*newMetContainer)["PVSoftTrk"]->phi()) );

      xAOD::MissingET * softTrkMet = (*newMetContainer)["PVSoftTrk"];
      ANA_CHECK( softTrkMet != nullptr); //check we retrieved the soft trk
      ANA_CHECK( metSystTool->applyCorrection(*softTrkMet) );
      if(debug) std::cout << "Soft track met: " << softTrkMet->met();

      // when doing track MET
      xAOD::MissingET * jetTrkMet = (*newMetContainer)["TrkJet"];
      ANA_CHECK( jetTrkMet != nullptr);
      ANA_CHECK( metSystTool->applyCorrection(*jetTrkMet, &metHelper));//for jetTrkMet correction, we need the METAssociationMap
      if(debug) std::cout << "Jet track met: " << jetTrkMet->met();

      //this builds the final track or cluster met sums, using systematic varied container
      //In the future, you will be able to run both of these on the same container to easily output CST and TST
      ANA_CHECK( met::buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
      //ANA_CHECK( met::buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

      //we record the container to the store, with a systematic indicated name
      ANA_CHECK( store->record( newMetContainer,    "FinalMETContainer_" + iSysSet.name()      ));
      ANA_CHECK( store->record( newMetAuxContainer, "FinalMETContainer_" + iSysSet.name() + "Aux."));
    }

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

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : February 2015

//this will only run in RootCore
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// FrameWork includes
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"

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

#include "assert.h"
#include "TFile.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METUtilities/METSystematicsTool.h"
#include "METUtilities/METMaker.h"
#include "METUtilities/CutsMETMaker.h"

#include "PATInterfaces/SystematicRegistry.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

JetCalibrationTool jetCalibrationTool("JetCalibTool");

//jets which are given to METMaker should be calibrated
StatusCode calibrateJetCollection(xAOD::JetContainer* & jets ){
  for ( xAOD::JetContainer::iterator ijet = jets->begin();
	ijet < jets->end();
	++ijet
	){
    //Shallow copy is needed (see links below)
    if(!jetCalibrationTool.applyCalibration(**ijet))//apply the calibration
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

int main( int argc, char* argv[]) {std::cout << __PRETTY_FUNCTION__ << std::endl;
   // Initialize the application
  xAOD::Init() ;

  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  std::string jetType = "AntiKt4EMTopoJets";
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    }
  }

  //enable status code failures
  CP::CorrectionCode::enableFailure();
  CP::SystematicCode::enableFailure();
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object.
  xAOD::TEvent * event = new xAOD::TEvent( xAOD::TEvent::kClassAccess );
  assert( event->readFrom( ifile.get(), xAOD::TEvent::kClassAccess  ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;

  // creation and set properties of the tools
  // if you need to set properties, you should do so before initialization
  met::METSystematicsTool metSystTool("METSystematicsTool");
  assert(metSystTool.setProperty("ConfigJetTrkFile","JetTrackSyst.config" ));//this is needed to do jet track systematics
  assert(metSystTool.setProperty("JetColl"         ,jetType               ));//this should be the same type as you give to rebuildJetMet
  //  metSystTool.msg().setLevel(MSG::VERBOSE);                              //If you want more messages, you can turn this up

  met::METMaker metMaker("METMaker");
  // metMaker.msg().setLevel(MSG::VERBOSE);//If you want more messages, you can turn this up

  if( (!metSystTool.initialize()) &&
      (!metMaker.initialize())    &&
      (!jetCalibrationTool.initialize())
      ){
    std::cout << "initialization of the met tools failed.  Exiting" << std::endl;
    return 0;
  }

  // output xAOD
  TFile *outfile = new TFile("example_METUtilities.root", "RECREATE");
  assert(event->writeTo(outfile));

  for(Long64_t ievent = 0;  ievent < std::min(int(event->getEntries()), 100); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    assert(    event->getEntry(ievent) >= 0 );

    //retrieve the original containers

    //this contains all constituents not associated to a jet/misc term
    const xAOD::MissingETContainer* coreMet  = nullptr;
    std::string coreMetKey = "MET_Core_" + jetType;
    coreMetKey.erase(coreMetKey.length() - 4);//this removes the Jets from the end of the jetType
    assert( event->retrieve(coreMet, coreMetKey) );

    const xAOD::ElectronContainer* electrons = nullptr;
    assert( event->retrieve(electrons, "Electrons") );

    const xAOD::MuonContainer* muons = nullptr;
    assert( event->retrieve(muons, "Muons") );

    const xAOD::PhotonContainer* photons = nullptr;
    assert( event->retrieve(photons, "Photons"));

    const xAOD::TauJetContainer* taus = nullptr;
    assert( event->retrieve(taus, "TauJets"));

    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* uncalibJets = nullptr;
    assert( event->retrieve(uncalibJets, jetType));//this retrieves and applies the correction
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > calibJetsPair = xAOD::shallowCopyContainer( *uncalibJets );//make a shallow copy to calibrate
    xAOD::JetContainer *& calibJets = calibJetsPair.first;//create a reference to the first element of the pair (i.e. the JetContainer)
    if( !calibrateJetCollection(calibJets)){//we calibrate the JetContainer
      std::cout << "initialization of the met tools failed.  Exiting" << std::endl;
      return 0;
    }
    if(!xAOD::setOriginalObjectLink(*uncalibJets, *calibJets)){//tell calib container what old container it matches
      std::cout << "Failed to set the original object links" << std::endl;
      return 0;
    }

    //retrieve the MET association map
    const xAOD::MissingETAssociationMap* metMap = nullptr;
    std::string metAssocKey = "METAssoc_" + jetType;
    metAssocKey.erase(metAssocKey.length() - 4 );//this removes the Jets from the end of the jetType
    assert( event->retrieve(metMap, metAssocKey) );

    //get the set of recommended systematics
    const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    const CP::SystematicSet&      recSysList  = registry.recommendedSystematics();

    //start the loop over systematics
    //For each systematic, we create an output MissingET container
    for(CP::SystematicSet::const_iterator isys = recSysList.begin();
	isys != recSysList.end();
	++isys)
      {	// print the systematics on the first event
	// When only using the METSystematicsTool, this will be the list of recommended systematics for that tool
	if(ievent == 0) std::cout << "Doing systematic : " << (*isys).name() << std::endl;

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
	  if(CutsMETMaker::accept(el)) metElectrons.push_back(el);
	}
	assert(metMaker.rebuildMET("RefEle",                   //name of metElectrons in metContainer
				   xAOD::Type::Electron,       //telling the rebuilder that this is electron met
				   newMetContainer,            //filling this met container
				   metElectrons.asDataVector(),//using these metElectrons that accepted our cuts
				   metMap)                     //and this association map
	       );

	//Photons
	ConstDataVector<xAOD::PhotonContainer> metPhotons(SG::VIEW_ELEMENTS);
	for(const auto& ph : *photons) {
	  if(CutsMETMaker::accept(ph)) metPhotons.push_back(ph);
	}
	assert(metMaker.rebuildMET("RefPhoton",
				   xAOD::Type::Photon,
				   newMetContainer,
				   metPhotons.asDataVector(),
				   metMap)
	       );

	//Taus
	ConstDataVector<xAOD::TauJetContainer> metTaus(SG::VIEW_ELEMENTS);
	for(const auto& tau : *taus) {
	  if(CutsMETMaker::accept(tau)) metTaus.push_back(tau);
	}
	assert(metMaker.rebuildMET("RefTau",
				   xAOD::Type::Tau,
				   newMetContainer,
				   metTaus.asDataVector(),
				   metMap)
	       );

	//Muons
	ConstDataVector<xAOD::MuonContainer> metMuons(SG::VIEW_ELEMENTS);
	for(const auto& mu : *muons) {
	  if(CutsMETMaker::accept(mu)) metMuons.push_back(mu);
	}
	assert(metMaker.rebuildMET("RefMuon",
				   xAOD::Type::Muon,
				   newMetContainer,
				   metMuons.asDataVector(),
				   metMap)
	       );

	//Now time to rebuild jetMet and get the soft term
	//This adds the necessary soft term for both CST and TST
	//these functions create an xAODMissingET object with the given names inside the container
	assert(  metMaker.rebuildJetMET("RefJet",        //name of jet met
					"SoftClus",      //name of soft cluster term met
					"PVSoftTrk",     //name of soft track term met
					newMetContainer, //adding to this new met container
					calibJets,       //using this jet collection to calculate jet met
					coreMet,         //core met container
					metMap,          //with this association map
					false            //don't apply jet jvt cut
					)
		 );

	// for rebuilding track MET
	// assert(  metMaker.rebuildTrackMET("RefJetTrk",    //name of jet track met
	// 				  "PVSoftTrk",	  //name of soft track term met
	// 				  newMetContainer,//adding to this new met container
	// 				  calibJets,	  //using this jet collection to calculate jet track met
	// 				  coreMet,	  //core met container
	// 				  metMap,	  //with this association map
	// 				  false		  //don't apply jet jvt cut
	// 				  )
	// 	 );


	//this is kind of annoying, but applySystematicVariation only takes a SystematicSet, but *isys is a SystematicVariation.
	//We use the SystematicSet constructor which just takes a SystematicVariation
	CP::SystematicSet iSysSet( (*isys).name());
	//tell the tool that we are using this SystematicSet (of one SystematicVariation for now)
	//after this call, when we use applyCorrection, the given met term will be adjusted with this systematic applied
	assert(	metSystTool.applySystematicVariation(iSysSet) );

	//get the soft cluster term, and applyCorrection
	xAOD::MissingET * softClusMet = (*newMetContainer)["SoftClus"];
	//std::cout << "Soft cluster met term met " << softClusMet->met() << std::endl;
	assert( softClusMet != nullptr); //check we retrieved the clust term
	assert( metSystTool.applyCorrection(*softClusMet) );

	xAOD::MissingET * softTrkMet = (*newMetContainer)["PVSoftTrk"];
	//std::cout << "Soft track met term met " << softTrkMet->met() << std::endl;
	assert( softTrkMet != nullptr); //check we retrieved the soft trk
	assert( metSystTool.applyCorrection(*softTrkMet) );

	// when doing track MET
	// xAOD::MissingET * jetTrkMet = (*newMetContainer)["RefJetTrk"];
	// //std::cout << "Jet track met term met " << jetTrkMet->met() << std::endl;
	// assert( jetTrkMet != nullptr);
	// assert( metSystTool.applyCorrection(*jetTrkMet, metMap));//for jetTrkMet correction, we need the METAssociationMap

	//this builds the final track or cluster met sums, using systematic varied container
	//In the future, you will be able to run both of these on the same container to easily output CST and TST
	assert( metMaker.buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
	//assert( metMaker.buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

	//we record the container to the store, with a systematic indicated name
	assert(event->record( newMetContainer,    "FinalMETContainer_"    + iSysSet.name()      ));
	assert(event->record( newMetAuxContainer, "FinalMETAuxContainer_" + iSysSet.name() + "."));

      }

    //fill the containers stored in the store
    //to the output file
    assert(event->fill());
  }

  assert(event->finishWritingTo(outfile) );
  outfile->Close();

  delete outfile;
  delete event;

  xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

  return 0;
 }



#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : May 2015

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
#include "xAODMissingET/MissingETComposition.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODBase/IParticleHelpers.h"

#include "assert.h"
#include <stdlib.h>
#include "TFile.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "METUtilities/METMaker.h"
#include "METUtilities/CutsMETMaker.h"
#include "METUtilities/METHelpers.h"

#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

//#include "PATInterfaces/SystematicRegistry.h"

int main( int argc, char* argv[] ){std::cout << __PRETTY_FUNCTION__ << std::endl;
   // Initialize the application
  xAOD::Init() ;

  //enable status code failures
  CP::CorrectionCode::enableFailure();
  //  CP::SystematicCode::enableFailure();
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();

  std::string jetType = "AntiKt4EMTopoJets";
  TString fileName = gSystem->Getenv("ASG_TEST_FILE_MC");
  Long64_t evtmax = 100;
  for (int i=0; i<argc; ++i) {
    if (std::string(argv[i]) == "-filen" && i+1<argc) {
      fileName = argv[i+1];
    } else if (std::string(argv[i]) == "-jetcoll" && i+1<argc) {
      jetType = argv[i+1];
    } else if (std::string(argv[i]) == "-evtmax" && i+1<argc) {
      evtmax = atoi(argv[i+1]);
    }
  }

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object.
  xAOD::TEvent * event = new xAOD::TEvent( xAOD::TEvent::kClassAccess );
  assert( event->readFrom( ifile.get(), xAOD::TEvent::kClassAccess  ) );

  // Create a transient object store. Needed for the tools.
  xAOD::TStore store;
  met::METMaker metMaker("METMaker");
  metMaker.setProperty("DoRemoveMuonJets", true).ignore();
  metMaker.setProperty("DoSetMuonJetEMScale", true).ignore();

  // metMaker.msg().setLevel(MSG::VERBOSE);//If you want more messages, you can turn this up

  // std::string const  jetCalibName = ;
  JetCalibrationTool jetCalibTool("JetCalibTool" );

  if( (!metMaker.initialize())    &&
      (!jetCalibTool.initialize())
      ){
    std::cout << "initialization of the met tools failed.  Exiting" << std::endl;
    return 0;
  }

  // output xAOD
  TFile *outfile = new TFile("example_METUtilities_advanced.root", "RECREATE");
  assert(event->writeTo(outfile));

  for(Long64_t ievent = 0;  ievent < std::min(int(event->getEntries()), 100); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl;
    assert(    event->getEntry(ievent) >= 0 );

    //retrieve the original containers
    const xAOD::MissingETContainer* coreMet  = nullptr;
    std::string coreMetKey = "MET_Core_" + jetType;
    coreMetKey.erase(coreMetKey.length() - 4);//this removes the Jets from the end of the jetType
    assert( event->retrieve(coreMet, coreMetKey) );

    //if you wanted to make a particle invisible to MET, i.e., remove the particle and
    //its clusters/tracks from the MET calculation, you can make a container of invisible particles
    //and then use selectIfNoOverlaps (below)
    //in this example, we will treat electrons as invisible
    const xAOD::ElectronContainer* invisibleElectrons = nullptr;
    assert( event->retrieve(invisibleElectrons, "Electrons") );

    const xAOD::MuonContainer* muons = nullptr;
    assert( event->retrieve(muons, "Muons") );

    const xAOD::PhotonContainer* photons = nullptr;
    assert( event->retrieve(photons, "Photons"));

    const xAOD::TauJetContainer* taus = nullptr;
    assert( event->retrieve(taus, "TauJets"));


    //this should probably be a calibrated jet container.  See the METUtilities twiki for more info
    const xAOD::JetContainer* jets = nullptr;
    assert( event->retrieve(jets, jetType));

    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *jets );
    //this is a non-const copy of the jet collection that you can calibrate.
    xAOD::JetContainer* calibJets = jets_shallowCopy.first;
    xAOD::setOriginalObjectLink(*jets,*calibJets);

    //retrieve the MET association map
    const xAOD::MissingETAssociationMap* metMap = nullptr;
    std::string metAssocKey = "METAssoc_" + jetType;
    metAssocKey.erase(metAssocKey.length() - 4 );//this removes the Jets from the end of the jetType
    assert( event->retrieve(metMap, metAssocKey) );
    metMap->resetObjSelectionFlags();

    xAOD::MissingETContainer*    newMetContainer    = new xAOD::MissingETContainer();
    xAOD::MissingETAuxContainer* newMetAuxContainer = new xAOD::MissingETAuxContainer();
    newMetContainer->setStore(newMetAuxContainer);

    // It is necessary to reset the selected objects before every MET calculation
    metMap->resetObjSelectionFlags();

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
      assert( metMaker.markInvisible(metInvisibleElectrons.asDataVector(),metMap) );
      // NOTE: Objects marked as invisible should not also be added as part
      // of another term! However, you can e.g. mark some electrons invisible
      // and compute RefEle with others.
    }

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
      //      if(CutsMETMaker::accept(mu)) metMuons.push_back(mu);
      if(mu->muonType()==xAOD::Muon::Combined && mu->pt()>10e3) metMuons.push_back(mu);
    }
    assert(metMaker.rebuildMET("RefMuon",
			       xAOD::Type::Muon,
			       newMetContainer,
			       metMuons.asDataVector(),
			       metMap)
	   );
    
    met::addGhostMuonsToJets(*muons, *calibJets);

    //Now time to rebuild jetMet and get the soft term
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

    assert(  metMaker.rebuildTrackMET("RefJetTrk",    //name of jet track met
				      "PVSoftTrk",	  //name of soft track term met
				      newMetContainer,//adding to this new met container
				      calibJets,	  //using this jet collection to calculate jet track met
				      coreMet,	  //core met container
				      metMap,	  //with this association map
				      false		  //don't apply jet jvt cut
				      )
	     );


    // //get the soft cluster term, and applyCorrection
    // xAOD::MissingET * softClusMet = (*newMetContainer)["SoftClus"];
    // xAOD::MissingET * softTrkMet = (*newMetContainer)["PVSoftTrk"];
    // xAOD::MissingET * jetTrkMet = (*newMetContainer)["RefJetTrk"];

    //this builds the final track and cluster met sums, using systematic varied container
    assert( metMaker.buildMETSum("FinalTrk" , newMetContainer, MissingETBase::Source::Track ) );
    assert( metMaker.buildMETSum("FinalClus", newMetContainer, MissingETBase::Source::LCTopo) );

    assert(event->record( newMetContainer,    "FinalMETContainer"    ));
    assert(event->record( newMetAuxContainer, "FinalMETAuxContainer."));

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

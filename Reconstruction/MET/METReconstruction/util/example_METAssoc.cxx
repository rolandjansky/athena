/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Matt Klein
//Email : matthew.henry.klein@cern.ch
//Date  : March 2015

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

#include "assert.h"
#include "TFile.h"

#include "METReconstruction/METAssociationTool.h"
#include "METReconstruction/METElectronAssociator.h"
#include "METReconstruction/METPhotonAssociator.h"
#include "METReconstruction/METTauAssociator.h"
#include "METReconstruction/METMuonAssociator.h"
#include "METReconstruction/METJetAssocTool.h"
#include "METReconstruction/METSoftAssociator.h"

#include "PFlowUtils/RetrievePFOTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

//The arguments can be used to specify which MET type we want to calculate:
//arg 1: Tag name
//arg 2: Jet container name
//arg 3: isPFlow
//arg 4: signal state of clusters (EM=0,LC=1)
//Examples: 
//To run default LC Reco, call "./example_METAssoc AntiKt4LCTopo AntiKt4LCTopoJets" (default if no args are provided)
//To run default EM Reco, call "./example_METAssoc AntiKt4EMTopo AntiKt4EMTopoJets 0 0"
//To run default PF Reco, call "./example_METAssoc AntiKt4EMPFlow AntiKt4EMPFlowJets 1"


int main(int argc,char *argv[]){

   // Initialize the application
  xAOD::Init() ;
  
  //enable status code failures
  StatusCode::enableFailure();
  xAOD::TReturnCode::enableFailure();

  int signalstate = argc >4 ? std::atoi(argv[4]) : 1;
  int signalstate_matching = argc >5 ? std::atoi(argv[5]) : signalstate; //only set for experimental purposes
  bool ispflow     = argc >3 ? std::atoi(argv[3]) : false;
  std::string jettype = argc >2 ? argv[2] : "AntiKt4LCTopoJets";
  std::string tag     = argc >1 ? argv[1] : "AntiKt4LCTopo";

  //this test file should work.  Feel free to contact me if there is a problem with the file.
  TString const fileName = "/afs/cern.ch/work/m/maklein/public/mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD.e2928_s1982_s2008_r6114_r6104_tid04859512_00/AOD.04859512._000001.pool.root";
  std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
  assert( ifile.get() );

  // Create a TEvent object.
  xAOD::TEvent * event = new xAOD::TEvent( xAOD::TEvent::kClassAccess );
  assert( event->readFrom( ifile.get(), xAOD::TEvent::kClassAccess  ) );

  // Create a transient object store. Needed for the tools.  
  xAOD::TStore store;

  CP::RetrievePFOTool *pfotool = new CP::RetrievePFOTool("PFOtool_RCREC");
  assert(pfotool->initialize());
  ToolHandle<CP::IRetrievePFOTool> hpfotool(pfotool);

  InDet::InDetTrackSelectionTool *trackTool = new InDet::InDetTrackSelectionTool("trackSel_RCREC");
  assert(trackTool->setProperty("CutLevel","TightPrimary"));
  assert(trackTool->setProperty("maxZ0SinTheta",1.5));
  assert(trackTool->setProperty("maxD0overSigmaD0",3.0));
  assert(trackTool->initialize());
  ToolHandle<InDet::IInDetTrackSelectionTool> htrackTool(trackTool);

  met::METElectronAssociator *electronTool = new met::METElectronAssociator("ElectronTool_RCREC");
  assert(electronTool->setProperty("InputCollection","Electrons"));
  assert(electronTool->setProperty("TrackSelectorTool",htrackTool));
  assert(electronTool->setProperty("PFlow",ispflow));
  assert(electronTool->setProperty("PFOTool",hpfotool));
  assert(electronTool->initialize());
  ToolHandle<IMETAssocToolBase> helectronTool(electronTool);

  met::METPhotonAssociator *photonTool = new met::METPhotonAssociator("PhotonTool_RCREC");
  assert(photonTool->setProperty("InputCollection","Photons"));
  assert(photonTool->setProperty("TrackSelectorTool",htrackTool));
  assert(photonTool->setProperty("PFlow",ispflow));
  assert(photonTool->setProperty("PFOTool",hpfotool));
  assert(photonTool->initialize());
  ToolHandle<IMETAssocToolBase> hphotonTool(photonTool);

  met::METMuonAssociator *muonTool = new met::METMuonAssociator("MuonTool_RCREC");
  assert(muonTool->setProperty("InputCollection","Muons"));
  assert(muonTool->setProperty("TrackSelectorTool",htrackTool));
  assert(muonTool->setProperty("PFlow",ispflow));
  assert(muonTool->setProperty("PFOTool",hpfotool));
  assert(muonTool->initialize());
  ToolHandle<IMETAssocToolBase> hmuonTool(muonTool);

  met::METTauAssociator *tauTool = new met::METTauAssociator("TauTool_RCREC");
  assert(tauTool->setProperty("InputCollection","TauJets"));
  assert(tauTool->setProperty("TrackSelectorTool",htrackTool));
  assert(tauTool->setProperty("PFlow",ispflow));
  assert(tauTool->setProperty("PFOTool",hpfotool));
  assert(tauTool->initialize());
  ToolHandle<IMETAssocToolBase> htauTool(tauTool);

  met::METJetAssocTool *jetTool = new met::METJetAssocTool("JetTool_RCREC");
  assert(jetTool->setProperty("InputCollection",jettype));
  assert(jetTool->setProperty("TrackSelectorTool",htrackTool));
  assert(jetTool->setProperty("PFlow",ispflow));
  assert(jetTool->setProperty("PFOTool",hpfotool));
  assert(jetTool->initialize());
  ToolHandle<IMETAssocToolBase> hjetTool(jetTool);

  met::METSoftAssociator *softTool = new met::METSoftAssociator("SoftTool_RCREC");
  assert(softTool->setProperty("TrackSelectorTool",htrackTool));
  assert(softTool->setProperty("PFlow",ispflow));
  assert(softTool->setProperty("PFOTool",hpfotool));
  assert(softTool->initialize());
  ToolHandle<IMETAssocToolBase> hsoftTool(softTool);

  // output xAOD
  std::string outputString = "example_METAssoc_"+tag+".root";
  TFile *outfile = new TFile(outputString.c_str(), "RECREATE");
  assert(event->writeTo(outfile));

  for(Long64_t ievent = 0;  ievent < std::min(int(event->getEntries()), 100); ++ievent){
    if(ievent % 10 == 0) std::cout << "event number: " << ievent << std::endl; 
    assert(    event->getEntry(ievent) >= 0 );

    xAOD::MissingETAssociationMap* metMap = new xAOD::MissingETAssociationMap();
    xAOD::MissingETAuxAssociationMap* metAuxMap = new xAOD::MissingETAuxAssociationMap();
    metMap->setStore(metAuxMap);

    xAOD::MissingETContainer* metCont = new xAOD::MissingETContainer();
    xAOD::MissingETAuxContainer* metAuxCont = new xAOD::MissingETAuxContainer();
    metCont->setStore(metAuxCont);


    CaloClusterChangeSignalStateList stateHelperList;    
    const xAOD::CaloClusterContainer* clusters(0);
    assert(event->retrieve( clusters,    "CaloCalTopoClusters"    )); 

    for(const auto& clus : *clusters) stateHelperList.add(clus,xAOD::CaloCluster::State(signalstate_matching));
    assert(hjetTool->execute(metCont,metMap));
    assert(helectronTool->execute(metCont,metMap));
    assert(hphotonTool->execute(metCont,metMap));
    assert(htauTool->execute(metCont,metMap));
    assert(hmuonTool->execute(metCont,metMap));
    for(const auto& clus : *clusters) stateHelperList.add(clus,xAOD::CaloCluster::State(signalstate));
    assert(hsoftTool->execute(metCont,metMap));
    metMap->identifyOverlaps();

    assert(event->record( metMap,    "METAssoc_" + tag   )); 
    assert(event->record( metAuxMap, "METAssoc_" + tag + "Aux.")); 

    assert(event->record( metCont,    "MET_Core_" + tag  )); 
    assert(event->record( metAuxCont, "MET_Core_" + tag + "Aux.")); 

    assert(event->copy(jettype));
    assert(event->copy("Electrons"));
    assert(event->copy("Photons"));
    assert(event->copy("Muons"));
    assert(event->copy("TauJets"));
    assert(event->copy("EventInfo"));
    assert(event->copy("MET_Truth"));
    assert(event->copy("CombinedMuonTrackParticles"));
    assert(event->copy("ExtrapolatedMuonTrackParticles"));
    assert(event->copy("MuonSpectrometerTrackParticles"));
    assert(event->copy("InDetTrackParticles"));

    //fill the containers stored in the store
    //to the output file
    assert(event->fill());
  }

  assert(event->finishWritingTo(outfile) );
  outfile->Close();

  delete outfile;
  delete event;

  return 0;
 }

#endif

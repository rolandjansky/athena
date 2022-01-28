/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <utility>
#include <ctime>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>
#include <TEnv.h>

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include <xAODCore/ShallowCopy.h>
#include <xAODBase/ObjectType.h>
#include <xAODBase/IParticleHelpers.h>

// Tools
#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "IsolationSelection/TestMacroHelpers.h"
#include "IsolationSelection/IsolationSelectionTool.h"

#include "AsgMessaging/MessageCheck.h"
#include "AsgTools/AnaToolHandle.h"

ANA_MSG_HEADER(TestCB)
ANA_MSG_SOURCE(TestCB, "testIsolationCloseByCorrectionTool")
using namespace TestCB;

std::string MuonIso(""), ElectronIso(""), PhotonIso("");

float MuonPt(0),  ElectronPt(0),  PhotonPt(0);
float MuonEta(0), ElectronEta(0), PhotonEta(0);

StatusCode setConfigWP(TString conf){
  TEnv env;
  if(env.ReadFile(conf, kEnvAll) != 0){
    ANA_MSG_INFO("Cannot read config file: " << conf);
    return StatusCode::FAILURE;
  }
  ANA_MSG_INFO("Reading config file: " << conf);
  MuonIso     = env.GetValue("MuonIso",     "PflowTight_FixedRad");
  MuonPt      = env.GetValue("MuonPt",      7000.);
  MuonEta     = env.GetValue("MuonEta",     2.5);

  ElectronIso = env.GetValue("ElectronIso", "PLImprovedTight");
  ElectronPt  = env.GetValue("ElectronPt",  7000.);
  ElectronEta = env.GetValue("ElectronEta", 2.47);

  PhotonIso   = env.GetValue("PhotonIso",   "FixedCutTight");
  PhotonPt    = env.GetValue("PhotonPt",    7000.);
  PhotonEta   = env.GetValue("PhotonEta",   2.47);

  env.PrintEnv();
  return StatusCode::SUCCESS;
}

template<typename Container> StatusCode RetrieveContainer(xAOD::TEvent &Ev, const std::string &Key, Container* &C, xAOD::ShallowAuxContainer* &Aux) {
    if (Aux) delete Aux;
    if (C) delete C;
    const Container* InCont(0);
    if (!Ev.retrieve(InCont, Key).isSuccess()) {
      ANA_MSG_ERROR("RetrieveContainer() Could not retrieve " << Key.c_str());
      return StatusCode::FAILURE;
    }
    typename std::pair<Container*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*InCont);
    Aux = shallowcopy.second;
    C = shallowcopy.first;
    if (!xAOD::setOriginalObjectLink(*InCont, *C)) {
      ANA_MSG_ERROR("RetrieveContainer() Failed to set Object links to " << Key.c_str());
      delete Aux;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

int main(int argc, char** argv){
    ANA_CHECK_SET_TYPE (int);
    gErrorIgnoreLevel = 0;

    const char* APP_NAME = argv[ 0 ];
    if( argc < 3 ) {
      ANA_MSG_ERROR("No input file name or WP config specified!");
      ANA_MSG_ERROR("Usage: %s <WPconfig> <xAOD file> <NEvents>"); 
      return EXIT_FAILURE;
    }
    ANA_CHECK( xAOD::Init( APP_NAME ) );
    auto start = std::time(nullptr);
    ANA_MSG_INFO("Initialized " << std::ctime(&start));

    TString outputTree = "testIsoCloseByTool_out.root";
    ANA_MSG_INFO("Name of output file: " << outputTree);

    // Open the input file:
    const TString fileName = argv[ 2 ];
    ANA_MSG_INFO("Opening file: " << fileName.Data());
    std::unique_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
    if( !ifile.get() ) return EXIT_FAILURE;
    
    const TString configFile = argv[ 1 ];
    ANA_CHECK( setConfigWP(configFile) );

    // Create a TEvent object:
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    ANA_CHECK( event.readFrom(ifile.get()) );
    ANA_MSG_INFO("Number of events in the file " << static_cast<int>(event.getEntries()));

    // Create a transient object store. Needed for the tools.
    xAOD::TStore store;

    ANA_MSG_INFO("Initialize instance of the IsolationSelectionTool");
    asg::AnaToolHandle<CP::IIsolationSelectionTool> IsoSelectionTool("CP::IsolationSelectionTool/IsoSelectionTool");
  
    ANA_CHECK( IsoSelectionTool.setProperty("MuonWP",      MuonIso) );
    ANA_CHECK( IsoSelectionTool.setProperty("ElectronWP",  ElectronIso) );
    ANA_CHECK( IsoSelectionTool.setProperty("PhotonWP",    PhotonIso) );
    ANA_CHECK( IsoSelectionTool.setProperty("OutputLevel", MSG::FATAL) );
    ANA_CHECK( IsoSelectionTool.retrieve() );

    ANA_MSG_INFO("Initialize instance of the IsolationCloseByCorrectionTool");
    asg::AnaToolHandle<CP::IIsolationCloseByCorrectionTool> IsoCloseByTool("CP::IsolationCloseByCorrectionTool/IsoCorrectionTool");
    
    ANA_CHECK( IsoCloseByTool.setProperty("IsolationSelectionTool", IsoSelectionTool.getHandle()) ); //Pass the instance of the created isolation tool
    ANA_CHECK( IsoCloseByTool.setProperty("SelectionDecorator",    "isCloseByObject") ); //Name of the quality decorator defining all nearby particles used to correct the isolation of a given particle

    //ANA_CHECK(m_isoCloseByTool.setProperty("PassOverlapDecorator","passOR")); //Usingnly using partilces after the OR (auxdata<char>("passOR") == 1)
    ANA_CHECK( IsoCloseByTool.setProperty("IsolationSelectionDecorator", "correctedIsol") );   //The name of the final isolation decorator. The tool internally calls P->auxdata<char>("CorrectedIsol") = m_IsoTool->accept(*P)

    //By default all particles in the container are corrected. For the purpose of saving processing time one can optionally
    //define this property. Then the isolation of the particle is only corrected only if the particle passes the input quality or if this decorator is set to true
    //ANA_CHECK(m_isoCloseByTool.setProperty("CorrectIsolationOf", "CorrectTheThing"));

    //The closeByIsoCorrectionTool accesses the default variables of a particle via the original container links
    //Optionally one can backup the isolation values before correction. Then the tool creates an auxelement called <BackupPrefix>_<IsoVariable> This might be interesting if the people are interested in writing
    //out the default values using CxAODs
    ANA_CHECK( IsoCloseByTool.setProperty("OutputLevel", MSG::DEBUG) );
    ANA_CHECK( IsoCloseByTool.setProperty("BackupPrefix", "default") );
    ANA_CHECK( IsoCloseByTool.retrieve() );

    //Define  the output
    TFile* ofile;
    ofile = TFile::Open(outputTree, "RECREATE");

    TTree *tree = new TTree("otree", "otree");
    CP::IsoCorrectionTestHelper eleBranches(tree, "Electrons", IsoSelectionTool->getElectronWPs());
    CP::IsoCorrectionTestHelper muoBranches(tree, "Muons",     IsoSelectionTool->getMuonWPs());
    CP::IsoCorrectionTestHelper phoBranches(tree, "Photons",   IsoSelectionTool->getPhotonWPs());

    Long64_t maxEVT = atoll( argv[ 3 ] );
    Long64_t entries = event.getEntries();
    if ((entries < maxEVT) || (maxEVT <= 0)) {
        maxEVT = entries;
    }
    ANA_MSG_INFO(Form("%lld events found, %lld events will be processed.", entries, maxEVT));

    const int INTERVAL = maxEVT > 20000 ? 10000 : maxEVT / 10;

    xAOD::ShallowAuxContainer *AuxMuons(nullptr), *AuxElectrons(nullptr), *AuxPhotons(nullptr);

    xAOD::MuonContainer* Muons(nullptr);
    xAOD::ElectronContainer* Electrons(nullptr);
    xAOD::PhotonContainer* Photons(nullptr);

    SG::AuxElement::Decorator<char> dec_PassQuality("isCloseByObject");
    SG::AuxElement::Decorator<char> dec_PassIsol("defaultIso");

    for (Long64_t entry(0); entry < maxEVT; ++entry) {
        event.getEntry(entry);

        const xAOD::EventInfo* ei(0);
        ANA_CHECK(event.retrieve(ei, "EventInfo"));

        if (entry % INTERVAL == 0)
	  ANA_MSG_INFO(Form("--> Event %lld \t  EventNumber: %llu RunNumber: %u", entry, ei->eventNumber(), ei->runNumber()));

        //Retrieve the Containers and create the ShallowAux links
        ANA_CHECK( RetrieveContainer(event, "Muons", Muons, AuxMuons) );
        ANA_CHECK( RetrieveContainer(event, "Electrons", Electrons, AuxElectrons).isSuccess() );
        ANA_CHECK( RetrieveContainer(event, "Photons", Photons, AuxPhotons).isSuccess() );

        for (const auto el : *Electrons) { 
            //Store if the electron passes the isolation
            dec_PassIsol(*el) = IsoSelectionTool->accept(*el);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*el) = el->pt() > ElectronPt && std::abs(el->eta()) < ElectronEta;
	}
        for (const auto ph : *Photons) {
            //Store if the photon passes the isolation (only needed for later comparisons)
            dec_PassIsol(*ph) = IsoSelectionTool->accept(*ph);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*ph) = ph->pt() > PhotonPt && std::abs(ph->eta()) < PhotonEta;
        }
        for (const auto mu : *Muons) {
            //Store if the muon passes the isolation
            dec_PassIsol(*mu) = IsoSelectionTool->accept(*mu);
            //Quality criteria only baseline kinematic selection
            dec_PassQuality(*mu) = mu->pt() > MuonPt && std::abs(mu->eta()) < MuonEta;
	}

        //Pass the things  towards the IsoCorrectionTool
        if (IsoCloseByTool->getCloseByIsoCorrection(Electrons, Muons, Photons).code() == CP::CorrectionCode::Error){ 
	  return EXIT_FAILURE;
	}

        // The isoCorrectionTool has now corrected everything using close-by objects satisfiyng the dec_PassQuality criteria
        // The name of the decorator is set via the 'SelectionDecorator' property of the tool
        // Optionally one can also define that the tool shall only objects surviving the overlap removal without  changing the initial decorator
        // Use therefore the 'PassOverlapDecorator' property to define the decorators name
        // If you define  the 'BackupPrefix' property then the original values are stored before correction <Prefix>_<IsolationCone>
        // The final result  whether the object  passes the isolation criteria now can be stored in the 'IsolationSelectionDecorator' e.g. 'CorrectedIso'

        //Store everything in the final ntuples
        ANA_CHECK( eleBranches.Fill(Electrons) );
        ANA_CHECK( muoBranches.Fill(Muons) );
        ANA_CHECK( phoBranches.Fill(Photons) );
        tree->Fill();

    }
    ofile->cd();
    tree->Write();
    ofile->Close();

    auto end = std::time(nullptr);
    ANA_MSG_INFO(Form("Ran on %i event for testing %s",(int)maxEVT, std::ctime(&end)));

    return EXIT_SUCCESS;
}

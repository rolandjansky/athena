/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// Create a histogram with offline rings information
//

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/RingSetConfContainer.h"
#include "xAODCaloRings/RingSetConfAuxContainer.h"
#include "xAODCaloRings/tools/getCaloRingsDecorator.h"
#include "RingerSelectorTools/tools/cxx/errorcheck.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"

#include <iostream>

using namespace std;
using namespace xAOD;

// Config
const char* APP_NAME = "ringsHist";
const char* OUTPUT_FILE = "histRings.root";

// Helpers
TChain *getChain(int argc, char* argv[], const char *chainName = "CollectionTree");

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getChain(argc, argv,"CollectionTree");
  if (chain == nullptr) {
    return 1;
  }

  const auto& ringsReader = getCaloRingsReader();

  //
  // Add initialization code for tools here
  //

  // The TEvent object
  TEvent event(TEvent::kClassAccess);
  RETURN_CHECK( APP_NAME, event.readFrom(chain));

  // Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }

  auto hRingsE = new TH1F("RingsE", "Rings energy dist", 30, 0.0, 1.0);
  hRingsE->GetXaxis()->SetCanExtend(kTRUE);
  hRingsE->SetStats(false);

  auto hOutput = new TH1F("NeuralRingerOffline", "Neural Ringer Offline", 30, -1.0, 1.0);
  hOutput->SetStats(false);

  auto hNNvrEta = new TH2F("NNOutput vs rEta",
        "NNOutput w.r.t rEta;rEta;NNOutput",
        50, 0, 1.2, 50, -1.00, 1.00);
  hNNvrEta->SetOption("COLZ");
  hNNvrEta->SetStats(false);

  const xAOD::ElectronContainer *electronCont;

  std::vector<float> ringsE;

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {

    event.getEntry(entry);

    const xAOD::EventInfo* event_info = 0;
    RETURN_CHECK( APP_NAME, event.retrieve( event_info, "EventInfo" ) );

    if ( event.retrieve(electronCont, "Electrons").isSuccess() ){
      for ( const Electron *electron : *electronCont ){
        const xAOD::CaloRingsLinks &caloRingsELVec = ringsReader(*electron);
        for ( const ElementLink<xAOD::CaloRingsContainer> &clRingsEL : caloRingsELVec ){
          (*clRingsEL)->exportRingsTo(ringsE);
          for ( auto ringE : ringsE ){
            hRingsE->Fill( ringE );
          }
        }
        // Loose Output = Medium Output = Tight Output
        SG::AuxElement::ConstAccessor<float> looseOutput("ToolSvc.ElectronRingerSelector_TestLoose_output");
        float output = looseOutput( *electron );
        hOutput->Fill( output );
        hNNvrEta->Fill( electron->showerShapeValue( EgammaParameters::Reta ) ,
                        output );
      }
    } else {
      std::cout << "Could not retrieve electrons container" << std::endl;
    }
  }

  auto metaChain = getChain(argc, argv,"MetaData");

  //// The TEvent object
  TEvent metaEvent( TEvent::kBranchAccess );
  RETURN_CHECK( APP_NAME, metaEvent.readFrom(metaChain));

  const xAOD::RingSetConfContainer *electronRingSetConf;

  size_t nEntriesMeta = metaChain->GetEntries();
  for (size_t entry = 0; entry < nEntriesMeta; entry++) {
    metaEvent.getEntry(entry);
    std::cout << " :: Entry :: " << entry << "/" << nEntriesMeta << std::endl;
    std::cout << "----------------- Reading RingSetConfs ------------------- " << std::endl;
    if ( metaEvent.retrieve(electronRingSetConf,"ElectronRingSetsConf").isSuccess() ){
      try {
        for ( const xAOD::RingSetConf *ringsConf : *electronRingSetConf ){
          ringsConf->print(std::cout);
        }
        std::cout << "----------------- Printing its RawConfCollection ------------------- " << std::endl;
        xAOD::RingSetConf::RawConfCollection rawConfCol;
        xAOD::RingSetConf::getRawConfCol( rawConfCol, electronRingSetConf );
        xAOD::RingSetConf::print( rawConfCol, std::cout );
      } catch (const std::runtime_error &e) {
        std::cout << "Coudln't read RingSetConfs, reason: " << e.what() << std::endl;
      }
    } else {
      std::cout << "ElectronRingSetsConf not available." << std::endl;
    }
  }

  // Draw
  auto canvas = new TCanvas("RingsEnergyHist");
  hRingsE->Draw();
  canvas->SaveAs( "ringsE.gif");
  canvas->SaveAs( "ringsE.eps");
  canvas = new TCanvas("NNOutput");
  hOutput->Draw();
  canvas->SaveAs( "nnOutput.gif");
  canvas->SaveAs( "nnOutput.eps");
  canvas = new TCanvas("NNOutVsReta");
  hNNvrEta->Draw();
  canvas->SaveAs( "nnWrtReta.gif");
  canvas->SaveAs( "nnWrtReta.eps");

  outputFile->Write();
  outputFile->Close();

}


TChain *getChain(int argc, char* argv[], const char *chainName)
{
  if (argc < 2) {
    ::Error(APP_NAME, "Usage: %s <xAOD file> [xAOD file2]...", APP_NAME);
    return nullptr;
  }

  auto chain = new TChain( chainName );
  for (int i = 1; i < argc; i++) {
    chain->Add(argv[i]);
  }

  return chain;
}


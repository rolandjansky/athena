/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// Print out the rings information and its metadata.
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
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"

#include <iostream>

using namespace std;
using namespace xAOD;

// Config
const char* APP_NAME = "dumpRings";
//const char* OUTPUT_FILE = "dumpRings.root";

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

  //// Get an output file open, book events.
  //auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  //if (outputFile == nullptr || !outputFile->IsOpen()) {
  //  return 1;
  //}

  //auto hEventCounter = new TH1F("eventCounter", "Number of Events Processed", 1, 0.0, 1.0);

  const xAOD::CaloRingsContainer *electronCaloRings(nullptr);
  const xAOD::ElectronContainer *electronCont(nullptr);

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {

    event.getEntry(entry);
    std::cout << " :: Entry :: " << entry << std::endl;

    const xAOD::EventInfo* event_info = 0;
    RETURN_CHECK( APP_NAME, event.retrieve( event_info, "EventInfo" ) );

    if ( event.retrieve(electronCont, "Electrons").isSuccess() ){
      std::cout << "The electron size is : " << electronCont->size() << std::endl;
      for ( const xAOD::Electron *electron : *electronCont ){
        std::cout << "----------------- CaloRings info ------------------- " << std::endl;
        std::cout << "isAvailable is : " << ringsReader.isAvailable(*electron) << std::endl;
        std::cout << "(eta,phi) is : (" << electron->eta() << "," << electron->phi() << ") | cluster pt is : " << electron->caloCluster()->pt() << std::endl;
        if ( ringsReader.isAvailable(*electron) ) {
          //std::cout << "isAvailableWritable is : " << ringsReader->isAvailableWritable(*electron) << std::endl;
          const xAOD::CaloRingsLinks &caloRingsELVec = ringsReader(*electron);
          std::cout << "The vectorEL size is : " << caloRingsELVec.size() << std::endl;
          for ( const ElementLink<xAOD::CaloRingsContainer> &clRingsEL : caloRingsELVec ){
            if ( clRingsEL.isValid() ) {
              (*clRingsEL)->print(std::cout);
            } else {
              std::cout << "Retrieved invalid link!" << std::endl;
            }
          }
          const xAOD::CaloRingsLinks vec = electron->auxdata< xAOD::CaloRingsLinks >("caloRingsLinks");
          std::cout << "The vector size is : " << vec.size() << std::endl;
        }
        std::cout << "----------------- Testing cluster ------------------- " << std::endl;
        SG::AuxElement::ConstAccessor<char> looseDec("ToolSvc.ElectronRingerSelector_TestLoose");
        if ( looseDec.isAvailable( *electron) ){
          bool dec = looseDec( *electron );
          std::cout << "Loose Decision is: " << std::boolalpha
            << dec << std::noboolalpha << std::endl;
          SG::AuxElement::ConstAccessor<char> mediumDec("ToolSvc.ElectronRingerSelector_TestMedium");
          dec = mediumDec( *electron );
          std::cout << "Medium Decision is: " << std::boolalpha
            << dec << std::noboolalpha << std::endl;
          SG::AuxElement::ConstAccessor<char> tightDec("ToolSvc.ElectronRingerSelector_TestTight");
          dec = tightDec( *electron );
          std::cout << "Tight Decision is: " << std::boolalpha
            << dec << std::noboolalpha << std::endl;
          SG::AuxElement::ConstAccessor<float> looseOutput("ToolSvc.ElectronRingerSelector_TestLoose_output");
          float output = looseOutput( *electron );
          std::cout << "Loose Output is: " << output << std::endl;
          SG::AuxElement::ConstAccessor<float> mediumOutput("ToolSvc.ElectronRingerSelector_TestMedium_output");
          output = mediumOutput( *electron );
          std::cout << "Medium Output is: " << output << std::endl;
          SG::AuxElement::ConstAccessor<float> tightOutput("ToolSvc.ElectronRingerSelector_TestTight_output");
          output = tightOutput( *electron );
          std::cout << "Tight Output is: " << output << std::endl;
        }
      }
    } else {
      std::cout << "Could not retrieve electrons container" << std::endl;
    }

    std::cout << "----------------- Reading directly from ElectronCaloRings ------------------- " << std::endl;

    if ( event.retrieve(electronCaloRings,"ElectronCaloRings").isSuccess() ){
      //unsigned int counter = 0;
      for ( const xAOD::CaloRings *rings : *electronCaloRings ){
        rings->print(std::cout);
      }
    } else {
      std::cout << "ElectronCaloRings. nao funciona" << std::endl;
    }
  }

  auto metaChain = getChain(argc, argv,"MetaData");

  std::cout << "MetaEvent:" << std::endl;
  metaChain->Print();

  //// The TEvent object
  TEvent metaEvent( TEvent::kClassAccess );
  RETURN_CHECK( APP_NAME, metaEvent.readFrom(metaChain));

  const xAOD::RingSetConfContainer *electronRingSetConf, *photonRingSetConf;

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
    if ( metaEvent.retrieve(photonRingSetConf,"PhotonRingSetsConf").isSuccess() ){
      try {
        for ( const xAOD::RingSetConf *ringsConf : *photonRingSetConf ){
          ringsConf->print(std::cout);
        }
        std::cout << "----------------- Printing its RawConfCollection ------------------- " << std::endl;
        xAOD::RingSetConf::RawConfCollection rawConfCol;
        xAOD::RingSetConf::getRawConfCol( rawConfCol, photonRingSetConf );
        xAOD::RingSetConf::print( rawConfCol, std::cout );
      } catch (const std::runtime_error &e) {
        std::cout << "Coudln't read RingSetConfs, reason: " << e.what() << std::endl;
      }
    } else {
      std::cout << "PhotonRingSetsConf not available." << std::endl;
    }
  }

  std::cout << "before close " << std::endl;

  //outputFile->Write();
  //outputFile->Close();

  std::cout << "Finishing..." << std::endl;
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


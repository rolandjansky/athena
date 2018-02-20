/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file OverlapRemovalTester.cxx
/// @brief Contains RootCore testing code for the overlap removal tools.
///
/// At the beginning you will find some global accessors/decorators and
/// selection functions. These are just for convenience and roughly simulate
/// a "standard" object selection.
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///


// System includes
#include <memory>

// Boost includes
#include "boost/program_options.hpp"

// ROOT includes
#include "TFile.h"
#include "TError.h"
#include "TString.h"

// Infrastructure includes
#ifdef ROOTCORE
#  include "xAODRootAccess/Init.h"
#  include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// Local includes
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/OverlapRemovalDefs.h"

using namespace ORUtils;
namespace po = boost::program_options;

//-----------------------------------------------------------------------------
// Error checking macro
//-----------------------------------------------------------------------------
#define CHECK( ARG )                                 \
  do {                                               \
    const bool result = ARG;                         \
    if(!result) {                                    \
      ::Error(APP_NAME, "Failed to execute: \"%s\"", \
              #ARG );                                \
      return 1;                                      \
    }                                                \
  } while( false )


//-----------------------------------------------------------------------------
// Global accessors and decorators
//-----------------------------------------------------------------------------
const bool outputPassValue = false; // overlap objects are 'true'
const std::string inputLabel = "selected";
const std::string outputLabel = outputPassValue? "passOR" : "overlaps";
const std::string bJetLabel = "isBJet";
const ort::inputAccessor_t selectAcc(inputLabel);
const ort::inputDecorator_t selectDec(inputLabel);
const ort::outputAccessor_t overlapAcc(outputLabel);
const ort::inputDecorator_t bJetDec(bJetLabel);
const ort::objLinkAccessor_t objLinkAcc("overlapObject");

//-----------------------------------------------------------------------------
// Functions for applying a dumb object selection
//-----------------------------------------------------------------------------
template<class ContainerType>
void selectObjects(const ContainerType* container)
{
  for(auto obj : *container){
    if(obj->pt() > 10000. && fabs(obj->eta()) < 2.5)
      selectDec(*obj) = true;
    else selectDec(*obj) = false;
  }
}
//-----------------------------------------------------------------------------
template<> void selectObjects<xAOD::JetContainer>
(const xAOD::JetContainer* jets)
{
  for(auto jet : *jets){
    bool pass = (jet->pt() > 20000. && fabs(jet->eta()) < 2.5);
    selectDec(*jet) = pass;
    // Label bjets
    double mv2c10 = 0.;
    if(!jet->btagging()->MVx_discriminant("MV2c10", mv2c10))
      throw std::runtime_error("MV2c10 unavailable");
    // This is the 85% efficiency working point
    bJetDec(*jet) = (mv2c10 > -0.1416);
  }
}
//-----------------------------------------------------------------------------
template<> void selectObjects<xAOD::TauJetContainer>
(const xAOD::TauJetContainer* taus)
{
  for(auto tau : *taus){
    bool pass = (tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) &&
                 tau->isTau(xAOD::TauJetParameters::EleBDTLoose) &&
                 tau->pt() > 20000. && fabs(tau->eta()) < 2.5);
    selectDec(*tau) = pass;
  }
}

//-----------------------------------------------------------------------------
// Function for printing object results
//-----------------------------------------------------------------------------
void printObj(const char* APP_NAME, const char* type,
              const xAOD::IParticle* obj)
{

  // Safety check
  if(!overlapAcc.isAvailable(*obj)){
    Error(APP_NAME, "Overlap decoration missing for object");
    abort();
  }

  // Print selected objects
  if(selectAcc(*obj)){
    Info(APP_NAME, "  %s pt %6.2f eta %5.2f phi %5.2f selected %i %s %i",
                   type, obj->pt()*0.001, obj->eta(), obj->phi(),
                   selectAcc(*obj), outputLabel.c_str(), overlapAcc(*obj));
    // Check for overlap object link
    if(objLinkAcc.isAvailable(*obj) && objLinkAcc(*obj).isValid()){
      const xAOD::IParticle* overlapObj = *objLinkAcc(*obj);
      std::stringstream ss; ss << overlapObj->type();
      Info(APP_NAME, "    Overlap: type %s pt %6.2f",
                     ss.str().c_str(), overlapObj->pt()*0.001);
    }
  }
}

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  // The application's name
  const char* APP_NAME = argv[0];

  po::options_description optDesc("Allowed options");
  optDesc.add_options()
    ("help,h", "produce help message")
    ("input-file,i", po::value<std::string>()->required(),
     "input xAOD file name")
    ("num-events,n", po::value<Long64_t>()->default_value(-1ll),
     "number of events to process")
    ("working-point,w", po::value<std::string>()->default_value("standard"),
     "OR working point");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optDesc), vm);
  if(vm.count("help")) {
    std::cout << optDesc << std::endl;
    return 1;
  }
  try {
    po::notify(vm);
  }
  catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  // Initialise the application
  CHECK( xAOD::Init(APP_NAME) );
  StatusCode::enableFailure();

  // Open the input file
  const auto fileName = vm["input-file"].as<std::string>();
  Info(APP_NAME, "Opening file: %s", fileName.c_str());
  TFile inputFile( fileName.c_str() );

  // Create a TEvent object
  xAOD::TEvent event(xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  CHECK( event.readFrom(&inputFile) );

  // Decide how many events to run over
  Long64_t entries = event.getEntries();
  Long64_t optEntries = vm["num-events"].as<Long64_t>();
  Info(APP_NAME, "Number of events in the file: %lli", entries);
  if(optEntries >= 0) entries = std::min(entries, optEntries);
  Info(APP_NAME, "Processing %lli entries", entries);

  // Overlap removal tool configuration flags
  ORUtils::ORFlags orFlags("OverlapRemovalTool", inputLabel, outputLabel);

  // Parse the requested working point
  const auto wp = vm["working-point"].as<std::string>();
  if(wp == "standard") {
    // use all defaults
  } else if(wp == "HF") {
    orFlags.bJetLabel = bJetLabel;
  } else if(wp == "boosted") {
    orFlags.boostedLeptons = true;
  } else if(wp == "boostedHF") {
    orFlags.bJetLabel = bJetLabel;
    orFlags.boostedLeptons = true;
  } else if(wp == "photonFavored") {
    orFlags.doTaus = false;
  } else {
    Error(APP_NAME, "Unsupported working-point: %s", wp.c_str());
    return 1;
  }
  orFlags.outputPassValue = outputPassValue;

  // Get the recommended tool configuration
  ORUtils::ToolBox toolBox;
  CHECK( ORUtils::recommendedTools(orFlags, toolBox) );

  // Special settings for photon-favored OR
  if(wp == "photonFavored") {
    CHECK( toolBox.muJetORT.setProperty("UseGhostAssociation", false) );
    CHECK( toolBox.muJetORT.setProperty("InnerDR", 0.) );
    CHECK( toolBox.phoEleORT.setProperty("SwapContainerPrecedence", true) );
    CHECK( toolBox.phoMuORT.setProperty("SwapContainerPrecedence", true) );
  }

  // Set message level for all tools
  //CHECK( toolBox.setGlobalProperty("OutputLevel", MSG::DEBUG) );

  // Connect and initialize all tools
  CHECK( toolBox.initialize() );
  auto& orTool = toolBox.masterTool;

  // Some object and event counters to help roughly
  // evaluate the effects of changes in the OR tool.
  unsigned int nTotalElectrons = 0;
  unsigned int nTotalMuons = 0;
  unsigned int nTotalJets = 0;
  unsigned int nTotalTaus = 0;
  unsigned int nTotalPhotons = 0;
  unsigned int nSelectedElectrons = 0;
  unsigned int nSelectedMuons = 0;
  unsigned int nSelectedJets = 0;
  unsigned int nSelectedTaus = 0;
  unsigned int nSelectedPhotons = 0;
  unsigned int nOverlapElectrons = 0;
  unsigned int nOverlapMuons = 0;
  unsigned int nOverlapJets = 0;
  unsigned int nOverlapTaus = 0;
  unsigned int nOverlapPhotons = 0;

  // Loop over the events
  std::cout << "Starting loop" << std::endl;
  for(Long64_t entry = 0; entry < entries; ++entry){

    event.getEntry(entry);

    // Print some event information for fun
    const xAOD::EventInfo* evtInfo = nullptr;
    CHECK( event.retrieve(evtInfo, "EventInfo") );
    Info(APP_NAME, "===>>>  Processing entry %lli, run %u, event %llu  <<<===",
         entry, evtInfo->runNumber(), evtInfo->eventNumber());

    // Get electrons
    const xAOD::ElectronContainer* electrons = nullptr;
    CHECK( event.retrieve(electrons, "Electrons") );
    // Get muons
    const xAOD::MuonContainer* muons = nullptr;
    CHECK( event.retrieve(muons, "Muons") );
    // Get jets
    const xAOD::JetContainer* jets = nullptr;
    CHECK( event.retrieve(jets, "AntiKt4EMTopoJets") );
    // Get taus
    const xAOD::TauJetContainer* taus = nullptr;
    CHECK( event.retrieve(taus, "TauJets") );
    // Get photons
    const xAOD::PhotonContainer* photons = nullptr;
    CHECK( event.retrieve(photons, "Photons") );

    // Set input decorations
    selectObjects(electrons);
    selectObjects(muons);
    selectObjects(jets);
    selectObjects(taus);
    selectObjects(photons);

    Info(APP_NAME, "nEle %lu, nMuo %lu, nJet %lu, nTau %lu, nPho %lu",
         electrons->size(), muons->size(),
         jets->size(), taus->size(),
         photons->size());

    // Apply the overlap removal to all objects
    CHECK( orTool->removeOverlaps(electrons, muons, jets, taus, photons) );

    //
    // Now, dump all of the results
    // TODO: Add counts of selected objects
    //

    // electrons
    Info(APP_NAME, "Now dumping the electrons");
    for(auto electron : *electrons){
      if(selectAcc(*electron)) {
        printObj(APP_NAME, "ele", electron);
        if(overlapAcc(*electron) != outputPassValue) nOverlapElectrons++;
        nSelectedElectrons++;
      }
      nTotalElectrons++;
    }

    // muons
    Info(APP_NAME, "Now dumping the muons");
    for(auto muon : *muons){
      if(selectAcc(*muon)) {
        printObj(APP_NAME, "muo", muon);
        if(overlapAcc(*muon) != outputPassValue) nOverlapMuons++;
        nSelectedMuons++;
      }
      nTotalMuons++;
    }

    // jets
    Info(APP_NAME, "Now dumping the jets");
    for(auto jet : *jets){
      if(selectAcc(*jet)) {
        printObj(APP_NAME, "jet", jet);
        if(overlapAcc(*jet) != outputPassValue) nOverlapJets++;
        nSelectedJets++;
      }
      nTotalJets++;
    }

    // taus
    Info(APP_NAME, "Now dumping the taus");
    for(auto tau : *taus){
      if(selectAcc(*tau)) {
        printObj(APP_NAME, "tau", tau);
        if(overlapAcc(*tau) != outputPassValue) nOverlapTaus++;
        nSelectedTaus++;
      }
      nTotalTaus++;
    }

    // photons
    Info(APP_NAME, "Now dumping the photons");
    for(auto photon : *photons){
      if(selectAcc(*photon)) {
        printObj(APP_NAME, "pho", photon);
        if(overlapAcc(*photon) != outputPassValue) nOverlapPhotons++;
        nSelectedPhotons++;
      }
      nTotalPhotons++;
    }

  }
  Info(APP_NAME, "=====================================");
  Info(APP_NAME, "End of event processing");
  Info(APP_NAME, "Object count summaries: nOverlap / nSelected / nTotal");
  Info(APP_NAME, "Number overlap elecs:   %5i / %5i / %5i",
       nOverlapElectrons, nSelectedElectrons, nTotalElectrons);
  Info(APP_NAME, "Number overlap muons:   %5i / %5i / %5i",
       nOverlapMuons, nSelectedMuons, nTotalMuons);
  Info(APP_NAME, "Number overlap jets:    %5i / %5i / %5i",
       nOverlapJets, nSelectedJets, nTotalJets);
  Info(APP_NAME, "Number overlap taus:    %5i / %5i / %5i",
       nOverlapTaus, nSelectedTaus, nTotalTaus);
  Info(APP_NAME, "Number overlap photons: %5i / %5i / %5i",
       nOverlapPhotons, nSelectedPhotons, nTotalPhotons);

  Info(APP_NAME, "Application finished successfully");

  return 0;
}

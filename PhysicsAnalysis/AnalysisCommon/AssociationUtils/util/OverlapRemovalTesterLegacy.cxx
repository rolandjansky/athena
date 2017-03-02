/// @file OverlapRemovalTesterLegacy.cxx
/// @brief Contains RootCore testing code for the legacy OverlapRemovalTool.
/// It is recommended to use instead the more advanced set of tools
/// demonstrated in OverlapRemovalTester.cxx
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///

// System includes
#include <memory>
#include <cstdlib>

// ROOT includes
#include "TFile.h"
#include "TError.h"
#include "TString.h"

// Infrastructure includes
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// Local includes
#include "AssociationUtils/OverlapRemovalTool.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

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
ort::inputAccessor_t selectAcc("selected");
ort::inputDecorator_t selectDec("selected");
ort::outputAccessor_t overlapAcc("overlaps");
ort::inputDecorator_t bJetDec("isBJet");
ort::objLinkAccessor_t objLinkAcc("overlapObject");

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
    double mv2c20 = 0.;
    if(!jet->btagging()->MVx_discriminant("MV2c20", mv2c20))
      throw std::runtime_error("MV2c20 unavailable");
    // This is the 80% efficiency working point
    bJetDec(*jet) = (mv2c20 > -0.5911);
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
  if(selectDec(*obj)){
    Info(APP_NAME, "  %s pt %6.2f eta %5.2f phi %5.2f selected %i overlaps %i",
                   type, obj->pt()*0.001, obj->eta(), obj->phi(),
                   selectAcc(*obj), overlapAcc(*obj));
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
int main( int argc, char* argv[] )
{

  // The application's name
  const char* APP_NAME = argv[ 0 ];

  // Check if we received a file name
  if(argc < 2) {
    Error( APP_NAME, "No file name received!" );
    Error( APP_NAME, "  Usage: %s [xAOD file name] [num events]", APP_NAME );
    return 1;
  }

  // Initialise the application
  CHECK( xAOD::Init(APP_NAME) );
  StatusCode::enableFailure();

  // Open the input file
  const TString fileName = argv[ 1 ];
  Info(APP_NAME, "Opening file: %s", fileName.Data());
  std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
  CHECK( ifile.get() );

  // Create a TEvent object
  xAOD::TEvent event(xAOD::TEvent::kAthenaAccess);
  CHECK( event.readFrom(ifile.get()) );
  Info(APP_NAME, "Number of events in the file: %i",
       static_cast<int>(event.getEntries()));

  // Decide how many events to run over
  Long64_t entries = event.getEntries();
  if(argc > 2) {
    const Long64_t e = atoll(argv[2]);
    entries = std::min(e, entries);
  }

  // Create and configure the tool
  OverlapRemovalTool orTool("OverlapRemovalTool");
  CHECK( orTool.setProperty("InputLabel", "selected") );
  // Turn on the object links for debugging
  CHECK( orTool.setProperty("LinkOverlapObjects", true) );
  // Turn on special b-jet handling for e-jet OR
  CHECK( orTool.setProperty("BJetLabel", "isBJet") );
  //CHECK( orTool.setProperty("OutputLevel", MSG::DEBUG) );

  // Initialize the tool
  CHECK( orTool.initialize() );

  // Some object and event counters to help roughly
  // evaluate the effects of changes in the OR tool.
  unsigned int nInputElectrons = 0;
  unsigned int nInputMuons = 0;
  unsigned int nInputJets = 0;
  unsigned int nInputTaus = 0;
  unsigned int nInputPhotons = 0;
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
    const xAOD::EventInfo* evtInfo = 0;
    CHECK( event.retrieve(evtInfo, "EventInfo") );
    Info(APP_NAME, "===>>>  Processing entry %lli, run %u, event %llu  <<<===",
         entry, evtInfo->runNumber(), evtInfo->eventNumber());

    // Get electrons
    const xAOD::ElectronContainer* electrons = 0;
    CHECK( event.retrieve(electrons, "Electrons") );
    // Get muons
    const xAOD::MuonContainer* muons = 0;
    CHECK( event.retrieve(muons, "Muons") );
    // Get jets
    const xAOD::JetContainer* jets = 0;
    CHECK( event.retrieve(jets, "AntiKt4EMTopoJets") );
    // Get taus
    const xAOD::TauJetContainer* taus = 0;
    CHECK( event.retrieve(taus, "TauJets") );
    // Get photons
    const xAOD::PhotonContainer* photons = 0;
    CHECK( event.retrieve(photons, "Photons") );

    // Set input decorations
    selectObjects(electrons);
    selectObjects(muons);
    selectObjects(jets);
    selectObjects(taus);
    selectObjects(photons);

    Info(APP_NAME,
         "nEle %lu, nMuo %lu, nJet %lu, nTau %lu, nPho %lu",
         electrons->size(), muons->size(),
         jets->size(), taus->size(),
         photons->size());

    // Apply the overlap removal to all "selected" objects
    CHECK( orTool.removeOverlaps(electrons, muons, jets, taus, photons) );

    //
    // Now, dump all of the results
    //

    // electrons
    Info(APP_NAME, "Now dumping the electrons");
    for(auto electron : *electrons){
      printObj(APP_NAME, "ele", electron);
      if(overlapAcc(*electron)) nOverlapElectrons++;
      nInputElectrons++;
    }

    // muons
    Info(APP_NAME, "Now dumping the muons");
    for(auto muon : *muons){
      printObj(APP_NAME, "muo", muon);
      if(overlapAcc(*muon)) nOverlapMuons++;
      nInputMuons++;
    }

    // jets
    Info(APP_NAME, "Now dumping the jets");
    for(auto jet : *jets){
      printObj(APP_NAME, "jet", jet);
      if(overlapAcc(*jet)) nOverlapJets++;
      nInputJets++;
    }

    // taus
    Info(APP_NAME, "Now dumping the taus");
    for(auto tau : *taus){
      printObj(APP_NAME, "tau", tau);
      if(overlapAcc(*tau)) nOverlapTaus++;
      nInputTaus++;
    }

    // photons
    Info(APP_NAME, "Now dumping the photons");
    for(auto photon : *photons){
      printObj(APP_NAME, "pho", photon);
      if(overlapAcc(*photon)) nOverlapPhotons++;
      nInputPhotons++;
    }

  }
  Info(APP_NAME, "=====================================");
  Info(APP_NAME, "End of event processing");
  Info(APP_NAME, "Number overlap elecs:   %i / %i", nOverlapElectrons, nInputElectrons);
  Info(APP_NAME, "Number overlap muons:   %i / %i", nOverlapMuons, nInputMuons);
  Info(APP_NAME, "Number overlap jets:    %i / %i", nOverlapJets, nInputJets);
  Info(APP_NAME, "Number overlap taus:    %i / %i", nOverlapTaus, nInputTaus);
  Info(APP_NAME, "Number overlap photons: %i / %i", nOverlapPhotons, nInputPhotons);

  Info(APP_NAME, "Application finished successfully");

  return 0;

}

#pragma GCC diagnostic pop

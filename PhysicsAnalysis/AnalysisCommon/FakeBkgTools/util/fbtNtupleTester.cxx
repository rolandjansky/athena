/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <AsgMessaging/MessageCheck.h>
#include <AsgTools/StandaloneToolHandle.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <numeric>

#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TSystem.h>

#ifdef XAOD_STANDALONE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

#include "AsgTools/AnaToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "FakeBkgTools/ApplyFakeFactor.h"

#ifndef READ_TREE_ADDRESSES
#define READ_TREE_ADDRESSES(typeName, name)			\
  typeName name = 0; chain->SetBranchAddress(#name, &name)
#endif

#ifndef READ_TREE_ADDRESSES_VEC
#define READ_TREE_ADDRESSES_VEC(typeName, name)			\
  typeName* name = 0; chain->SetBranchAddress(#name, &name)
#endif



// messaging
ANA_MSG_HEADER(Test)
ANA_MSG_SOURCE(Test, "fbtNtupleTester")
using namespace Test;

// an example input file is /afs/cern.ch/user/j/jreicher/public/fbtTestData.root
int main(int argc, char* argv[])
{
  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  if(argc < 2){
    std::cerr << "No input file specified! Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  TString fileNameFullPath = argv[1];
  TString fileName = fileNameFullPath;
  
#ifdef XAOD_STANDALONE

  
  TFile *inFile = TFile::Open(fileName, "READ");
  if(!inFile){
    std::cerr << "Invalid root file! Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  // Name of the tree in the file
  TString sample = "tree_NoSys";

  std::cout << "Running sample: " << sample << std::endl;

  // Create instance of the tools and set the FF file names + the histogram names
  CP::ApplyFakeFactor* ffTool = new CP::ApplyFakeFactor("ApplyFF");

  // Path to the file of FF histograms
  std::vector<std::string> ff_file = { "FakeBkgTools/testValuesFF.root" };

  ANA_CHECK( ffTool->setProperty("InputFiles", ff_file) );
  ANA_CHECK( ffTool->setProperty("EnergyUnit", "GeV") );
  ANA_CHECK( ffTool->setProperty("SkipUncertainties", true) );
  ANA_CHECK( ffTool->setProperty("OutputLevel", MSG::FATAL) );

  // ntuple values are in GeV, but xAOD::IParticles that 
  // we create will be in MeV
  float convertToMeV = 1000.;

  // Initialize tools
  ANA_CHECK( ffTool->initialize() );
  TChain* chain = new TChain("chain");
  std::vector<TString> datasets = {};

  datasets = {sample};

  for(TString dataset : datasets){
    chain->Add(fileNameFullPath+"/"+dataset);
  }

  READ_TREE_ADDRESSES(unsigned long long, EventNumber);
  READ_TREE_ADDRESSES(int, DatasetNumber);
  READ_TREE_ADDRESSES(int, nLep_base);
  READ_TREE_ADDRESSES(int, nLep_signal);
  READ_TREE_ADDRESSES(int, nLep_antiID);

  READ_TREE_ADDRESSES(float, lep1Pt);
  READ_TREE_ADDRESSES(float, lep1Eta);
  READ_TREE_ADDRESSES(float, lep1Phi);
  READ_TREE_ADDRESSES(float, lep1D0Sig);
  READ_TREE_ADDRESSES(float, lep1Z0SinTheta);
  READ_TREE_ADDRESSES(int, lep1Flavor);
  READ_TREE_ADDRESSES(int, lep1Charge);
  READ_TREE_ADDRESSES(int, lep1PassOR);
  READ_TREE_ADDRESSES(int, lep1PassBL);
  READ_TREE_ADDRESSES(bool, lep1Loose);
  READ_TREE_ADDRESSES(bool, lep1Medium);
  READ_TREE_ADDRESSES(bool, lep1TruthMatched);
  READ_TREE_ADDRESSES(bool, lep1Signal);
  READ_TREE_ADDRESSES(bool, lep1AntiID);
  READ_TREE_ADDRESSES(int, lep1Type);

  READ_TREE_ADDRESSES(float, lep2Pt);
  READ_TREE_ADDRESSES(float, lep2Eta);
  READ_TREE_ADDRESSES(float, lep2Phi);
  READ_TREE_ADDRESSES(float, lep2D0Sig);
  READ_TREE_ADDRESSES(float, lep2Z0SinTheta);
  READ_TREE_ADDRESSES(int, lep2Flavor);
  READ_TREE_ADDRESSES(int, lep2Charge);
  READ_TREE_ADDRESSES(bool, lep2PassOR);
  READ_TREE_ADDRESSES(bool, lep2PassBL);
  READ_TREE_ADDRESSES(bool, lep2Loose);
  READ_TREE_ADDRESSES(bool, lep2Medium);
  READ_TREE_ADDRESSES(bool, lep2TruthMatched);
  READ_TREE_ADDRESSES(bool, lep2Signal);
  READ_TREE_ADDRESSES(bool, lep2AntiID);
  READ_TREE_ADDRESSES(int, lep2Type);

  READ_TREE_ADDRESSES(float, lep3Pt);
  READ_TREE_ADDRESSES(float, lep3Eta);
  READ_TREE_ADDRESSES(float, lep3Phi);
  READ_TREE_ADDRESSES(float, lep3D0Sig);
  READ_TREE_ADDRESSES(float, lep3Z0SinTheta);
  READ_TREE_ADDRESSES(int, lep3Flavor);
  READ_TREE_ADDRESSES(int, lep3Charge);
  READ_TREE_ADDRESSES(bool, lep3PassOR);
  READ_TREE_ADDRESSES(bool, lep3PassBL);
  READ_TREE_ADDRESSES(bool, lep3Loose);
  READ_TREE_ADDRESSES(bool, lep3Medium);
  READ_TREE_ADDRESSES(bool, lep3TruthMatched);
  READ_TREE_ADDRESSES(bool, lep3Signal);
  READ_TREE_ADDRESSES(bool, lep3AntiID);
  READ_TREE_ADDRESSES(int, lep3Type);

  READ_TREE_ADDRESSES(double, FFWeight);

  // Turn on all branches. Our cloned tree will share
  // the references to the variables which we wish to modify!
  chain->SetBranchStatus("*",1);

  TFile* outFile_nominal = new TFile("antiIDTimesFF_"+sample+"_nominal.root","RECREATE");
  TTree* outTree_nominal = chain->CloneTree(0);
  outTree_nominal->SetTitle("fakes");
  outTree_nominal->SetName("fakes");

  // We need to make a TEvent and TStore object so that the FBT can look
  // for the evtStore()
  xAOD::TEvent* tEvent = new xAOD::TEvent();
  xAOD::TStore* tStore = new xAOD::TStore();

  // For creating an EventInfo object that the tool will look for
  std::unique_ptr<xAOD::EventInfo> eventInfo = std::make_unique<xAOD::EventInfo>();
  std::unique_ptr<xAOD::EventAuxInfo> eventAuxInfo = std::make_unique<xAOD::EventAuxInfo>();

  eventInfo->setStore(eventAuxInfo.get());
  ANA_CHECK( ffTool->evtStore()->record(eventInfo.get(),    "EventInfo") );
  ANA_CHECK( ffTool->evtStore()->record(eventAuxInfo.get(), "EventInfoAux.") );;

  // we can clear this vector in each loop, rather than allocate
  // the memory for each event
  xAOD::IParticleContainer particles;

  // Loop over events!
  int nEventsProcessed(0);
  int nEventsPassed(0);

  Long64_t nentries = chain->GetEntries();
  for(Long64_t i(0); i < nentries; ++i){
    std::cout << "processed " << nEventsProcessed << " events" << std::endl;
    ++nEventsProcessed;

    int getEntry = chain->GetEntry(i);
    if(getEntry < 1){
      std::cerr << "GetEntry() error!! Either an I/O issue, or an invalid entry. Aborting." << std::endl;
      abort();
    }

    // just as an example for an EventInfo decor that one could set...
    eventInfo->auxdecor<unsigned int>("RandomRunNumber") = 507;

    std::vector<float> lepPt = {lep1Pt, lep2Pt, lep3Pt};
    std::vector<float> lepEta = {lep1Eta, lep2Eta, lep3Eta};
    std::vector<float> lepPhi = {lep1Phi, lep2Phi, lep3Phi};
    std::vector<float> lepD0Sig = {lep1D0Sig, lep2D0Sig, lep3D0Sig};
    std::vector<float> lepZ0SinTheta = {lep1Z0SinTheta, lep2Z0SinTheta, lep3Z0SinTheta};
    std::vector<int> lepFlavor = {lep1Flavor, lep2Flavor, lep3Flavor};
    std::vector<int> lepCharge = {lep1Charge, lep2Charge, lep3Charge};
    std::vector<int> lepPassOR = {lep1PassOR, lep2PassOR, lep3PassOR};
    std::vector<int> lepPassBL = {lep1PassBL, lep2PassBL, lep3PassBL};
    std::vector<int> lepLoose = {lep1Loose, lep2Loose, lep3Loose};
    std::vector<int> lepMedium = {lep1Medium, lep2Medium, lep3Medium};
    std::vector<int> lepSignal = {lep1Signal, lep2Signal, lep3Signal};
    std::vector<int> lepTruthMatched = {lep1TruthMatched, lep2TruthMatched, lep3TruthMatched};

    //// Reset some variables
    FFWeight = 1;
    nLep_antiID = 0;
    lep1AntiID = 0;
    lep2AntiID = 0;
    lep3AntiID = 0;

    // Right now, we have up to 3 leptons saved in our ntuples
    int nTotalLeps = 3;

    // Look at ID and isolation to fill whether a given lepton passes
    // the signal ID or antiID
    std::vector<int> lepPassSigID(nTotalLeps, 0);
    std::vector<int> lepPassAntiID(nTotalLeps, 0);

    // clear the vector of IParticle objects
    for(auto part : particles){
      delete part;
    }
    particles.clear();

    // Loop over the events to assign ID vs. antiID leptons
    for(int i = 0; i < nTotalLeps; ++i){

      bool passesSignal = false;
      bool passesAntiID = false;
      xAOD::Type::ObjectType lepType = xAOD::Type::Other;

      // Fill ID and SF vectors below:
      if( (lepFlavor).at(i) == 1 ){ // electrons

        //double z0sinTheta = fabs( (lepZ0SinTheta).at(i) );

        passesSignal = (lepSignal).at(i);
        passesAntiID = !(lepSignal).at(i);
        //passesAntiID = (lepLoose.at(i) && lepPassBL.at(i) && (z0sinTheta < 0.5) && (lepPassOR).at(i)) && !(lepSignal.at(i));
          
        // Now fill the vectors
        lepPassSigID.at(i) = passesSignal;
        lepPassAntiID.at(i) = passesAntiID;

        lepType = xAOD::Type::Electron;
      }
      else if( (lepFlavor).at(i) == 2){ // muons

        //double z0sinTheta = fabs( (lepZ0SinTheta).at(i) );

        passesSignal = (lepSignal).at(i);
        passesAntiID = !(lepSignal).at(i);
        //passesAntiID = (lepMedium.at(i) && (z0sinTheta < 0.5) && !(lepSignal).at(i));

        // Now fill the vectors
        lepPassSigID.at(i) = passesSignal;
        lepPassAntiID.at(i) = passesAntiID;

        lepType = xAOD::Type::Muon;
      }

      // if this lepton isn't relevant, we can skip
      if(!passesSignal && !passesAntiID) continue;

      // FIXME need to be sure all pT/eta/phi values are in MeV, and then just need to
      // unpack the FFs into MeV
      if (lepType == xAOD::Type::Electron){
        xAOD::Electron* particle = new xAOD::Electron();
        particle->makePrivateStore();
        particle->setP4(lepPt.at(i)*convertToMeV,lepEta.at(i),lepPhi.at(i),0.511);
        particle->setCharge(lepCharge.at(i));
        particle->auxdata<char>("Tight") = passesSignal;
        particles.push_back(static_cast<xAOD::IParticle*>(particle));
      }
      else if(lepType == xAOD::Type::Muon){
        xAOD::Muon* particle = new xAOD::Muon();
        particle->makePrivateStore();
        particle->setP4(lepPt.at(i)*convertToMeV,lepEta.at(i),lepPhi.at(i));
        particle->setCharge(lepCharge.at(i));
        particle->auxdata<char>("Tight") = passesSignal;
        particles.push_back(static_cast<xAOD::IParticle*>(particle));
      }
      else{
        std::cerr << "invalid lepton type!" << std::endl;
        continue;
      }

      // Here we'll redefine "signal" lepton for our outputs, because
      // we're about to promote our antiID leptons to signal leptons. 
      if(passesAntiID){

        ++nLep_signal;
        ++nLep_antiID;

        if(i == 0){
          lep1Signal = 1;
          lep1AntiID = 1;
        }
        else if(i == 1){
          lep2Signal = 1;
          lep2AntiID = 1;
        }
        else if(i == 2){
          lep3Signal = 1;
          lep3AntiID = 1;
        }
      }

    } // end loop over nTotalLeps

    // Event is not relevant for fake estimation!
    if(nLep_antiID == 0) continue;

    // Now actually compute the weight!
    float fbtWeight;
    ANA_CHECK( ffTool->addEvent(particles) );;
    ANA_CHECK( ffTool->getEventWeight(fbtWeight, "3T", ">=1F[T]") );

    // FIXME note that the relevant MC events will need their 
    // FFWeight to be multiplied by -1 here, since these are used
    // for the prompt MC subtraction
    FFWeight = fbtWeight;
    std::cout << "nLep_antiID " << nLep_antiID << std::endl;
    std::cout << "FFWeight " << FFWeight << std::endl;      

    // Fill the tree
    outTree_nominal->Fill();

    // just to keep track
    ++nEventsPassed;

  } // end loop over events

  std::cout << "nEventsProcessed is " << nEventsProcessed << " and nEventsPassed is " << nEventsPassed << std::endl;

  outFile_nominal->Write();

  delete chain;
  delete ffTool;
  delete outFile_nominal;
  delete tStore;
  delete tEvent;
  
#endif 

  return EXIT_SUCCESS;
}

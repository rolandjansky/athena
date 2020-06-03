/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// STL include(s):
#include <vector>
#include <string>
#include <assert.h>
#include "cstdlib" //std::getenv

// ROOT include(s):
#include <TChain.h>
#include <TError.h>

// Local include(s):
#include "TrigCostRootAnalysis/TrigConfInterface.h"
#include "TrigCostRootAnalysis/TrigCostData.h"
#include "TrigCostRootAnalysis/Utility.h"
#include "TrigCostRootAnalysis/Config.h"
#include "TrigCostRootAnalysis/TrigCostAtlasStyle.h"
#include "TrigCostRootAnalysis/ROSConfService.h"

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

using namespace TrigCostRootAnalysis;

// This is a simple example of how to use TrigCostRootAnalysis as a library

int main() {
  Info("TrigCostD3PD", "Execute UserSkeleton");
  const std::string treeName = "trig_cost";

  // Master chain of all files to analyse
  TChain chain(treeName.c_str());

  std::vector< std::string > inputFiles;
  // TODO - ADD YOUR FILES
  inputFiles.push_back("/afs/cern.ch/work/t/tamartin/public/NTUP_TRIGCOST.05385661._001604.pool.root");


  for (UInt_t f = 0; f < inputFiles.size(); ++f) {
    chain.Add(inputFiles.at(f).c_str());
    Info("TrigCostD3PD", "Using Input File: %s", inputFiles.at(f).c_str());
  }

  // 'event' is very important, it's used pass-by-reference and dictates which entry in the tree we're reading from.
  Long64_t event = 0;

  // If you need L2 or EF data, add extra instances of TrigCostData
  TrigCostData HLTData;
  HLTData.setup(/*currentEvent*/ event, /*branch prefix*/ "TrigCostHLT_", /*TTree (or TChain)*/ &chain);

  // Set location of config tree
  Config::config().set(kConfigPrefix, "trig_costMeta/TrigConfTree", "ConfigPrefix");

  // Set the name of the ROS mapping
  // This is the RootCore environment way.
  Config::config().set(kROSXMLName, "rob-ros-robin-2012.xml");
  const Char_t* env = std::getenv("ROOTCOREBIN");
  if (env != NULL) {
    Config::config().set(kDataDir, std::string(env) + std::string("/data/TrigCostRootAnalysis/"));
    Config::config().set(kROSXMLPath, Config::config().getStr(kDataDir) + Config::config().getStr(kROSXMLName));
  }

  // This is the ATHENA WAY
#ifndef ROOTCORE
  Config::config().set(kIsRootCore, 0, "IsRootCore");
  if (Config::config().getIsSet(kROSXMLName)) {
    std::string locAthena = PathResolverFindDataFile(Config::config().getStr(kROSXMLName));
    if (locAthena == "") Error("Config::parseCLI", "Athena cannot find ROS mapping file %s", Config::config().getStr(kROSXMLName).c_str());
    else {
      Config::config().set(kROSXMLPath, locAthena, "ROSXMLPath");
      Info("Config::parseCLI", "Athena has found the file: %s", Config::config().getStr(kROSXMLPath).c_str());
    }
  }
#endif // not ROOTCORE

  // Set the name of ROI types
  Config::config().Config::config().set(kNoneString, "None");
  Config::config().set(kMuonString, "Muon");
  Config::config().set(kEmTauString, "EMTau");
  Config::config().set(kJetString, "Jet");
  Config::config().set(kJetEtString, "JetEt");
  Config::config().set(kEnergyString, "Energy");

  // Set the "I don't know" string
  Config::config().set(kUnknownString, "UNKNOWN");

  Info("TrigCostD3PD", "Doing some sample output, have a look at TrigCostData.h to see all accessible data calls.");

  const UInt_t maxEvents = 1;

  // Begin event loop
  for (Long64_t masterEvent = 0; masterEvent < chain.GetEntries(); ++masterEvent) {
    // Load correct tree into memory in the chain
    event = chain.LoadTree(masterEvent);
    assert(event >= 0); // -1=Empty, -2=OutOfRange, -3=FileIOProblem, -4=TTreeMissing

    //if start of file, config
    if (event == 0) {
      TrigConfInterface::reset();
      TrigConfInterface::configure(&chain);
    }

    /// CHAINS
    Info("TrigCostD3PD::EventLoop", "Event %lli, Number of Chains: %i", event, HLTData.getNChains());
    for (UInt_t c = 0; c < HLTData.getNChains(); ++c) {
      Info("TrigCostD3PD::EventLoop", " -- Chain:%s, PassedRaw:%s, Time:%.2f ms, Algs:%i",
           TrigConfInterface::getHLTNameFromChainID(HLTData.getChainID(c), HLTData.getChainLevel(c)).c_str(),
           (HLTData.getIsChainPassedRaw(c) ? "YES" : "NO"),
           HLTData.getChainTimerFromSequences(c),
           (HLTData.getChainAlgCalls(c) + HLTData.getChainAlgCaches(c)));
    }

    /// SEQUENCES AND ALGS
    for (UInt_t s = 0; s < HLTData.getNSequences(); ++s) {
      Info("TrigCostD3PD::EventLoop", " -- -- Sequence:%s/%s",
           TrigConfInterface::getHLTNameFromChainID(HLTData.getSequenceChannelCounter(s), HLTData.getSequenceLevel(s)).c_str(),
           TrigConfInterface::getHLTSeqNameFromIndex(HLTData.getSequenceIndex(s)).c_str());

      // Loop over all algorithms in sequence
      for (UInt_t a = 0; a < HLTData.getNSeqAlgs(s); ++a) {
        Int_t seqIndex = HLTData.getSequenceIndex(s);
        Int_t seqAlgPos = HLTData.getSeqAlgPosition(s, a);

        const std::string algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);
        const std::string algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);

        Info("TrigCostD3PD::EventLoop", " -- -- -- Algorithm:%s::%s, Time:%.6f ms, Data:%.2f kB",
             algType.c_str(), algName.c_str(),
             HLTData.getSeqAlgTimer(s, a),
             (HLTData.getSeqAlgROBRetrievalSize(s, a) + HLTData.getSeqAlgROBRequestSize(s, a)));
      }
    }

    /// ROS
    for (UInt_t Rob = 0; Rob < HLTData.getNROBs(); ++Rob) {
      std::pair< Int_t, Int_t > alg = HLTData.getROBAlgLocation(Rob);
      std::string requestingAlg = "NotFound";
      if (alg.first > -1 && alg.second > -1) {
        Int_t seqIndex = HLTData.getSequenceIndex(alg.first);
        Int_t seqAlgPos = HLTData.getSeqAlgPosition(alg.first, alg.second);
        requestingAlg = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);
      }
      Info("TrigCostD3PD::EventLoop", " -- ROS Request by '%s': Start:%i.%i Stop:%i.%i Time:%.6f",
           requestingAlg.c_str(),
           HLTData.getROBTimeStartSec(Rob),
           HLTData.getROBTimeStartMicroSec(Rob),
           HLTData.getROBTimeStopSec(Rob),
           HLTData.getROBTimeStopMicroSec(Rob),
           HLTData.getROBTimer(Rob));
      for (UInt_t RobData = 0; RobData < HLTData.getROBDataN(Rob); ++RobData) {
        Int_t RobId = HLTData.getROBDataID(Rob, RobData);
        const std::string RobinName = ROSConfService::rosConfService().getRobinNameFromId((UInt_t) RobId);
        const std::string RosName = ROSConfService::rosConfService().getRosNameFromFromRobinName(RobinName);

        Bool_t cached = HLTData.getIsROBDataCached(Rob, RobData);
        Info("TrigCostD3PD::EventLoop", " -- -- ROB:(%s | %s) : Size:%.6f kb : %s",
             RosName.c_str(),
             RobinName.c_str(),
             HLTData.getROBDataSize(Rob, RobData),
             (cached ? "Cached" : "Retrieved"));
      }
    }

    /// ROI
    for (UInt_t r = 0; r < HLTData.getNRoIs(); ++r) {
      std::string ROIType = HLTData.getRoITypeString(r);
      Info("TrigCostD3PD::EventLoop", " -- ROI ID:%i Type:%s at Eta:%.2f Phi:%.2f",
           HLTData.getRoIID(r),
           ROIType.c_str(),
           HLTData.getRoIEta(r),
           HLTData.getRoIPhi(r));
    }

    if (masterEvent >= maxEvents) break;
  }

  Info("TrigCostD3PD", "Terminating");
  return 0;
}

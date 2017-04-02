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
  const std::string _treeName = "trig_cost";

  // Master chain of all files to analyse
  TChain _chain(_treeName.c_str());

  std::vector< std::string > _inputFiles;
  // TODO - ADD YOUR FILES
  _inputFiles.push_back("/afs/cern.ch/work/t/tamartin/public/NTUP_TRIGCOST.05385661._001604.pool.root");


  for (UInt_t f = 0; f < _inputFiles.size(); ++f) {
    _chain.Add(_inputFiles.at(f).c_str());
    Info("TrigCostD3PD", "Using Input File: %s", _inputFiles.at(f).c_str());
  }

  // 'event' is very important, it's used pass-by-reference and dictates which entry in the tree we're reading from.
  Long64_t _event = 0;

  // If you need L2 or EF data, add extra instances of TrigCostData
  TrigCostData _HLTData;
  _HLTData.setup(/*currentEvent*/ _event, /*branch prefix*/ "TrigCostHLT_", /*TTree (or TChain)*/ &_chain);

  // Set location of config tree
  Config::config().set(kConfigPrefix, "trig_costMeta/TrigConfTree", "ConfigPrefix");

  // Set the name of the ROS mapping
  // This is the RootCore environment way.
  Config::config().set(kROSXMLName, "rob-ros-robin-2012.xml");
  const Char_t* _env = std::getenv("ROOTCOREBIN");
  if (_env != NULL) {
    Config::config().set(kDataDir, std::string(_env) + std::string("/data/TrigCostRootAnalysis/"));
    Config::config().set(kROSXMLPath, Config::config().getStr(kDataDir) + Config::config().getStr(kROSXMLName));
  }

  // This is the ATHENA WAY
#ifndef ROOTCORE
  Config::config().set(kIsRootCore, 0, "IsRootCore");
  if (Config::config().getIsSet(kROSXMLName)) {
    std::string _locAthena = PathResolverFindDataFile(Config::config().getStr(kROSXMLName));
    if (_locAthena == "") Error("Config::parseCLI", "Athena cannot find ROS mapping file %s", Config::config().getStr(kROSXMLName).c_str());
    else {
      Config::config().set(kROSXMLPath, _locAthena, "ROSXMLPath");
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

  const UInt_t _maxEvents = 1;

  // Begin event loop
  for (Long64_t _masterEvent = 0; _masterEvent < _chain.GetEntries(); ++_masterEvent) {
    // Load correct tree into memory in the chain
    _event = _chain.LoadTree(_masterEvent);
    assert(_event >= 0); // -1=Empty, -2=OutOfRange, -3=FileIOProblem, -4=TTreeMissing

    //if start of file, config
    if (_event == 0) {
      TrigConfInterface::reset();
      TrigConfInterface::configure(&_chain);
    }

    /// CHAINS
    Info("TrigCostD3PD::EventLoop", "Event %lli, Number of Chains: %i", _event, _HLTData.getNChains());
    for (UInt_t _c = 0; _c < _HLTData.getNChains(); ++_c) {
      Info("TrigCostD3PD::EventLoop", " -- Chain:%s, PassedRaw:%s, Time:%.2f ms, Algs:%i",
           TrigConfInterface::getHLTNameFromChainID(_HLTData.getChainID(_c), _HLTData.getChainLevel(_c)).c_str(),
           (_HLTData.getIsChainPassedRaw(_c) ? "YES" : "NO"),
           _HLTData.getChainTimerFromSequences(_c),
           (_HLTData.getChainAlgCalls(_c) + _HLTData.getChainAlgCaches(_c)));
    }

    /// SEQUENCES AND ALGS
    for (UInt_t _s = 0; _s < _HLTData.getNSequences(); ++_s) {
      Info("TrigCostD3PD::EventLoop", " -- -- Sequence:%s/%s",
           TrigConfInterface::getHLTNameFromChainID(_HLTData.getSequenceChannelCounter(_s), _HLTData.getSequenceLevel(_s)).c_str(),
           TrigConfInterface::getHLTSeqNameFromIndex(_HLTData.getSequenceIndex(_s)).c_str());

      // Loop over all algorithms in sequence
      for (UInt_t _a = 0; _a < _HLTData.getNSeqAlgs(_s); ++_a) {
        Int_t _seqIndex = _HLTData.getSequenceIndex(_s);
        Int_t _seqAlgPos = _HLTData.getSeqAlgPosition(_s, _a);

        const std::string _algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);
        const std::string _algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);

        Info("TrigCostD3PD::EventLoop", " -- -- -- Algorithm:%s::%s, Time:%.6f ms, Data:%.2f kB",
             _algType.c_str(), _algName.c_str(),
             _HLTData.getSeqAlgTimer(_s, _a),
             (_HLTData.getSeqAlgROBRetrievalSize(_s, _a) + _HLTData.getSeqAlgROBRequestSize(_s, _a)));
      }
    }

    /// ROS
    for (UInt_t _Rob = 0; _Rob < _HLTData.getNROBs(); ++_Rob) {
      std::pair< Int_t, Int_t > _alg = _HLTData.getROBAlgLocation(_Rob);
      std::string _requestingAlg = "NotFound";
      if (_alg.first > -1 && _alg.second > -1) {
        Int_t _seqIndex = _HLTData.getSequenceIndex(_alg.first);
        Int_t _seqAlgPos = _HLTData.getSeqAlgPosition(_alg.first, _alg.second);
        _requestingAlg = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);
      }
      Info("TrigCostD3PD::EventLoop", " -- ROS Request by '%s': Start:%i.%i Stop:%i.%i Time:%.6f",
           _requestingAlg.c_str(),
           _HLTData.getROBTimeStartSec(_Rob),
           _HLTData.getROBTimeStartMicroSec(_Rob),
           _HLTData.getROBTimeStopSec(_Rob),
           _HLTData.getROBTimeStopMicroSec(_Rob),
           _HLTData.getROBTimer(_Rob));
      for (UInt_t _RobData = 0; _RobData < _HLTData.getROBDataN(_Rob); ++_RobData) {
        Int_t _RobId = _HLTData.getROBDataID(_Rob, _RobData);
        const std::string _RobinName = ROSConfService::rosConfService().getRobinNameFromId((UInt_t) _RobId);
        const std::string _RosName = ROSConfService::rosConfService().getRosNameFromFromRobinName(_RobinName);

        Bool_t _cached = _HLTData.getIsROBDataCached(_Rob, _RobData);
        Info("TrigCostD3PD::EventLoop", " -- -- ROB:(%s | %s) : Size:%.6f kb : %s",
             _RosName.c_str(),
             _RobinName.c_str(),
             _HLTData.getROBDataSize(_Rob, _RobData),
             (_cached ? "Cached" : "Retrieved"));
      }
    }

    /// ROI
    for (UInt_t _r = 0; _r < _HLTData.getNRoIs(); ++_r) {
      std::string _ROIType = _HLTData.getRoITypeString(_r);
      Info("TrigCostD3PD::EventLoop", " -- ROI ID:%i Type:%s at Eta:%.2f Phi:%.2f",
           _HLTData.getRoIID(_r),
           _ROIType.c_str(),
           _HLTData.getRoIEta(_r),
           _HLTData.getRoIPhi(_r));
    }

    if (_masterEvent >= _maxEvents) break;
  }

  Info("TrigCostD3PD", "Terminating");
  return 0;
}

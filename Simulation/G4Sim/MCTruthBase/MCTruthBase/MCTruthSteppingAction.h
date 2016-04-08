/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthSteppingAction_H
#define MCTruthSteppingAction_H

#include <map>
#include <string>
#include "G4AtlasTools/UserActionBase.h"

class SensitiveDetectorEntry;
class G4VSensitiveDetector;

/// Why are these global? Why specify the comparator?
/// TODO: move them into MCTruthSteppingAction
typedef std::map<std::string, int, std::less<std::string> > VolName2LevelMap;
typedef std::map<std::string,std::string, std::less<std::string> > VolName2SDNameMap;
typedef std::map<std::string,G4VSensitiveDetector*, std::less<std::string> > VolName2SDPtrMap;
typedef VolName2LevelMap::const_iterator VolName2LevelMapIterator;

/// @class MCTruthSteppingAction
/// @brief Action which pushes hits into SDs via steps in assigned volumes
///
class MCTruthSteppingAction final: public UserActionBase
{
  /// Grant private access to the TruthStrategyManager
  friend class TruthStrategyManager;

 public:

  /// Standard constructor
  MCTruthSteppingAction(const std::string& type, const std::string& name,
                        const IInterface* parent);

  /// Process one particle step. If volume is applicable,
  /// passes the step to the assigned SD To process a hit.
  virtual void Step(const G4Step* aStep) override;

  /// Gaudi boilerplate
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:

  VolName2LevelMap m_volumeMap;
  VolName2SDNameMap m_sdNameMap;
  VolName2SDPtrMap m_sdPtrMap;

  /// Assign a volume to a sensitive detector.
  void AddRecordingEnvelope(std::string,int,std::string sdName="");
  /// Print the list of recording envelopes.
  void ListRecordingEnvelopes();
  /// Retrieve the sensitive detector pointers from the assigned names.
  void GetSensitiveDetectors();
  /// Apparently not used...?
  /// TODO: remove this.
  SensitiveDetectorEntry* sde;
  bool m_needToInitialize;
};

#endif

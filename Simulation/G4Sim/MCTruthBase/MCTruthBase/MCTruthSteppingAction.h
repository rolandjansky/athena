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

typedef std::map<std::string, int, std::less<std::string> > VolName2LevelMap;
typedef std::map<std::string,std::string, std::less<std::string> > VolName2SDNameMap;
typedef std::map<std::string,G4VSensitiveDetector*, std::less<std::string> > VolName2SDPtrMap;
typedef VolName2LevelMap::const_iterator VolName2LevelMapIterator;

class MCTruthSteppingAction final: public UserActionBase
{
  friend class TruthStrategyManager;
 public:
  MCTruthSteppingAction(const std::string& type, const std::string& name, const IInterface* parent);
  virtual void Step(const G4Step* aStep) override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:
  VolName2LevelMap m_volumeMap;
  VolName2SDNameMap m_sdNameMap;
  VolName2SDPtrMap m_sdPtrMap;
  void AddRecordingEnvelope(std::string,int,std::string sdName="");
  void ListRecordingEnvelopes();
  void GetSensitiveDetectors();

  SensitiveDetectorEntry *sde;
  bool m_needToInitialize;
};

#endif

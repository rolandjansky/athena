/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_VolumeDebugger_H
#define G4DEBUGGINGTOOLS_VolumeDebugger_H

#include "G4AtlasTools/UserActionBase.h"

#include <string>

class G4LogicalVolume;

class VolumeDebugger final: public UserActionBase {

 public:
  VolumeDebugger(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void EndOfRun(const G4Run*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;


 private:
  std::string m_path;
  std::string m_targetVolume;
  int m_res;
  bool m_verbose;
  float m_tol;
  bool m_volumeCheck;

  void PullVolumes( G4LogicalVolume* v ) const;

};

#endif //G4DEBUGGINGTOOLS_VolumeDebugger_H

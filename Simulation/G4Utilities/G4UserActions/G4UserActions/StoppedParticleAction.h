/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StoppedParticleAction_h
#define StoppedParticleAction_h


#include "G4AtlasTools/UserActionBase.h"


#include <string>
class TrackFastSimSD;

class StoppedParticleAction final: public UserActionBase {
 public:
 StoppedParticleAction(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
    m_fsSD(0) , m_init(false) {}

  virtual void Step(const G4Step*) override;

  virtual StatusCode initialize() override;
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 protected: 

  bool isSUSYParticle(const int) const;
  TrackFastSimSD * m_fsSD;  
  bool m_init;

};

#endif


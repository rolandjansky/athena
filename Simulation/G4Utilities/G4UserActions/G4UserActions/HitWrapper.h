/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HitWrapper_H
#define HitWrapper_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class HitWrapper final: public UserActionBase {

  public:
  HitWrapper(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void EndOfEvent(const G4Event*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:

  double m_time; //!< Time over which to wrap the hits
  

  
};

#endif

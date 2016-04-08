/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollower_H
#define GeantFollower_H

#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>

/** @class GeantFollower

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class IGeantFollowerHelper;
}

class StoreGateSvc;

class GeantFollower final: public UserActionBase {

 public:
  /** Standard UserAction Constructor*/
  GeantFollower(const std::string& type, const std::string& name, const IInterface* parent);

  /** All G4 interface methods */
  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void Step(const G4Step*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  virtual StatusCode initialize() override;

 private:
  ToolHandle<Trk::IGeantFollowerHelper>     m_helper;
  mutable const Trk::IGeantFollowerHelper*  m_helperPointer;


};

#endif

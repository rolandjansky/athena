/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoSvc_H
#define AGDD2GeoSvc_H

#include "AGDDControl/IAGDD2GeoSvc.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/IAGDDToolBase.h"
#include "AGDDControl/AGDDController.h"
#include "AthenaBaseComps/AthService.h"

#include <vector>
#include <mutex>

class ISvcLocator;
class IToolSvc;
class IRDBAccessSvc;
class IGeoModelSvc;
class GeoFullPhysVol;

class AGDDtoGeoSvc : public extends<AthService, IAGDDtoGeoSvc> {

public:

  virtual StatusCode initialize() override final;

  virtual void addHandler(XMLHandler* v) override final;
  virtual LockedController getController() override final;

  // Standard Constructor
  AGDDtoGeoSvc(const std::string& name, ISvcLocator* svc);

  // Standard Destructor
  virtual ~AGDDtoGeoSvc() = default;

private:

  void localInitialization();

  std::vector<XMLHandler*> m_handlerVector;

  ToolHandleArray<IAGDDToolBase> m_builders; // public ToolHandleArray
  AGDDController m_controller;
  std::recursive_mutex m_mutex;
};

#endif

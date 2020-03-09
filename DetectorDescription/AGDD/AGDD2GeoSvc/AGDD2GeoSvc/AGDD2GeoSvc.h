/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoSvc_H
#define AGDD2GeoSvc_H

#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/IAGDDToolBase.h"
#include "AthenaBaseComps/AthService.h"

#include "AGDDKernel/AGDDDetectorStore.h"

#include <vector>

class ISvcLocator;
class IToolSvc;
class IRDBAccessSvc;
class IGeoModelSvc;
class GeoFullPhysVol;

class AGDDtoGeoSvc : public extends<AthService, IAGDDtoGeoSvc> {

public:

  virtual StatusCode initialize() override final;

  virtual void addHandler(XMLHandler* v) override final { m_handlerVector.push_back(v);}

  // Standard Constructor
  AGDDtoGeoSvc(const std::string& name, ISvcLocator* svc);

  // Standard Destructor
  virtual ~AGDDtoGeoSvc() = default;

private:

  void localInitialization();

  std::vector<XMLHandler*> m_handlerVector;

  ToolHandleArray<IAGDDToolBase> m_builders; // public ToolHandleArray

};

#endif

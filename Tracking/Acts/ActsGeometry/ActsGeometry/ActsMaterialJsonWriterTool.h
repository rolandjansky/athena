/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSMATERIALJSONWRITERTOOL_H
#define ACTSGEOMETRY_ACTSMATERIALJSONWRITERTOOL_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"  /*no forward decl: typedef*/

#include "ActsGeometryInterfaces/IActsMaterialJsonWriterTool.h"

#include "Acts/Material/ISurfaceMaterial.hpp"
#include "Acts/Material/IVolumeMaterial.hpp"



class ActsMaterialJsonWriterTool : public extends<AthAlgTool, IActsMaterialJsonWriterTool>
{

public:

  using SurfaceMaterialMap
  = std::map<Acts::GeometryID, std::shared_ptr<const Acts::ISurfaceMaterial>>;

  using VolumeMaterialMap
  = std::map<Acts::GeometryID, std::shared_ptr<const Acts::IVolumeMaterial>>;

  using DetectorMaterialMaps = std::pair<SurfaceMaterialMap, VolumeMaterialMap>;

  StatusCode initialize() override;

  ActsMaterialJsonWriterTool(const std::string &type, const std::string &name,
                                const IInterface *parent);

  virtual
  void
  write(const DetectorMaterialMaps& detMaterial) const override;

private:

  std::string m_fileName;

};
#endif


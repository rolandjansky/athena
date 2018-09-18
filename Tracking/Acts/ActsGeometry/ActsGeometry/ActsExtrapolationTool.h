/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H

// ATHENA
#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/Property.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// PACKAGE
#include "ActsGeometry/IExtrapolationTool.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ACTS
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"

namespace Acts {
class ExtrapolationCode;
class Surface;
class BoundaryCheck;
class ExtrapolationEngine;
}

class ActsExtrapolationTool : public Acts::IExtrapolationTool
{

public:
  virtual StatusCode initialize() override;

  ActsExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  virtual Acts::ExtrapolationCode
  extrapolate(Acts::ExCellCharged&       ecCharged,
              const Acts::Surface*       sf,
              const Acts::BoundaryCheck& bcheck) const;

  virtual Acts::ExtrapolationCode
  extrapolate(Acts::ExCellNeutral&       ecNeutral,
              const Acts::Surface*       sf,
              const Acts::BoundaryCheck& bcheck) const;

  virtual
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine() const;

  void
  prepareAlignment() const;

private:

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ToolHandle<ActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  std::shared_ptr<Acts::ExtrapolationEngine> m_exEngine;

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}};
  Gaudi::Property<bool> m_matEngineDoELossCorrection{this, "MatEngineDoELossCorrection", true};

};


inline
void
ActsExtrapolationTool::prepareAlignment() const 
{
  m_trackingGeometryTool->prepareAlignment();
}

#endif

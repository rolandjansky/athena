/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ACTS
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp" // for ExCell*

namespace MagField {
  class IMagFieldSvc;
}

namespace Acts {
class ExtrapolationCode;
class Surface;
class BoundaryCheck;
class ExtrapolationEngine;
}

static const InterfaceID IID_ActsExtrapolationTool("ActsExtrapolationTool", 1, 0);

class ActsExtrapolationTool : public AthAlgTool
{

public:
  virtual StatusCode initialize() override;

  ActsExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  Acts::ExtrapolationCode
  extrapolate(Acts::ExCellCharged&       ecCharged,
              const Acts::Surface*       sf = nullptr,
              const Acts::BoundaryCheck& bcheck = true) const;

  Acts::ExtrapolationCode
  extrapolate(Acts::ExCellNeutral&       ecNeutral,
              const Acts::Surface*       sf = nullptr,
              const Acts::BoundaryCheck& bcheck = true) const;

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



#endif

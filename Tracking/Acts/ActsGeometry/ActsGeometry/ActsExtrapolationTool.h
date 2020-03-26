/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Propagator/detail/StandardAborters.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Utilities/Result.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/Helpers.hpp"

#include <cmath>

namespace MagField {
  class IMagFieldSvc;
}

namespace Acts {
class Surface;
class BoundaryCheck;
}


namespace ActsExtrapolationDetail {
  class VariantPropagator;
}


class ActsExtrapolationTool : public extends<AthAlgTool, IActsExtrapolationTool>
{

public:
  virtual StatusCode initialize() override;

  ActsExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  ~ActsExtrapolationTool();

private:
  // set up options for propagation
  using SteppingLogger = Acts::detail::SteppingLogger;
  using DebugOutput = Acts::detail::DebugOutputActor;
  using EndOfWorld = Acts::detail::EndOfWorldReached;
  using ResultType = Acts::Result<std::pair<std::vector<Acts::detail::Step>,
                                            DebugOutput::result_type>>;

public:
  virtual
  std::vector<Acts::detail::Step>
  propagate(const EventContext& ctx,
            const Acts::BoundParameters& startParameters,
            double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const override
  {
    return m_trackingGeometryTool.get();
  }


private:
  std::unique_ptr<ActsExtrapolationDetail::VariantPropagator> m_varProp;

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS", "Either ATLAS or Constant"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}, "Constant field value to use if FieldMode == Constant"};

  Gaudi::Property<double> m_ptLoopers{this, "PtLoopers", 300, "PT loop protection threshold. Will be converted to Acts MeV unit"};

  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10, "Max step size in Acts m unit"};
};



#endif

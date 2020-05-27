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
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/DebugOutputActor.hpp"
#include "Acts/Propagator/StandardAborters.hpp"
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
  using DebugOutput = Acts::DebugOutputActor;
  using EndOfWorld = Acts::EndOfWorldReached;
  using ResultType = Acts::Result<std::pair<Acts::PropagationOutput,
                                            DebugOutput::result_type>>;


public:
  virtual
  Acts::PropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundParameters& startParameters,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  std::unique_ptr<const Acts::CurvilinearParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundParameters& startParameters,
            Acts::NavigationDirection navDir = Acts::forward,
            double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  Acts::PropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundParameters& startParameters,
                   const Acts::Surface& target,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  std::unique_ptr<const Acts::BoundParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundParameters& startParameters,
            const Acts::Surface& target,
            Acts::NavigationDirection navDir = Acts::forward,
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

  Gaudi::Property<bool> m_interactionMultiScatering{this, "InteractionMultiScatering", true, "Whether to consider multiple scattering in the interactor"};
  Gaudi::Property<bool> m_interactionEloss{this, "InteractionEloss", true, "Whether to consider energy loss in the interactor"};
  Gaudi::Property<bool> m_interactionRecord{this, "InteractionRecord", true, "Whether to record all material interactions"};
};



#endif

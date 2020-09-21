/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H
#define ACTSGEOMETRY_ACTSEXTRAPOLATIONTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"

// Need to include this early; otherwise, we run into errors with
// ReferenceWrapperAnyCompat in clang builds due the is_constructable
// specialization defined there getting implicitly instantiated earlier.
#include "Acts/Propagator/Propagator.hpp"

// PACKAGE
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

// ACTS
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/StandardAborters.hpp"
#include "Acts/Propagator/SurfaceCollector.hpp"
#include "Acts/Utilities/Result.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Logger.hpp"

#include <cmath>

namespace Acts {
class Surface;
class BoundaryCheck;
class Logger;
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
  using EndOfWorld = Acts::EndOfWorldReached;
  using ResultType = Acts::Result<ActsPropagationOutput>;


public:
  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  std::unique_ptr<const Acts::CurvilinearParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            Acts::NavigationDirection navDir = Acts::forward,
            double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   const Acts::Surface& target,
                   Acts::NavigationDirection navDir = Acts::forward,
                   double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  std::unique_ptr<const Acts::BoundTrackParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            const Acts::Surface& target,
            Acts::NavigationDirection navDir = Acts::forward,
            double pathLimit = std::numeric_limits<double>::max()) const override;

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const override
  {
    return m_trackingGeometryTool.get();
  }


  virtual
  Acts::MagneticFieldContext
  getMagneticFieldContext(const EventContext& ctx) const override;


private:
  std::unique_ptr<ActsExtrapolationDetail::VariantPropagator> m_varProp;
  std::unique_ptr<const Acts::Logger> m_logger{nullptr};

  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS", "Either ATLAS or Constant"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}, "Constant field value to use if FieldMode == Constant"};

  Gaudi::Property<double> m_ptLoopers{this, "PtLoopers", 300, "PT loop protection threshold. Will be converted to Acts MeV unit"};
  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10, "Max step size in Acts m unit"};

  // Material inteaction option
  Gaudi::Property<bool> m_interactionMultiScatering{this, "InteractionMultiScatering", false, "Whether to consider multiple scattering in the interactor"};
  Gaudi::Property<bool> m_interactionEloss{this, "InteractionEloss", false, "Whether to consider energy loss in the interactor"};
  Gaudi::Property<bool> m_interactionRecord{this, "InteractionRecord", false, "Whether to record all material interactions"};
};



#endif

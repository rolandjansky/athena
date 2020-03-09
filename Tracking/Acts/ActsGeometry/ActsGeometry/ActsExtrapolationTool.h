/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Propagator/detail/StandardAborters.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Utilities/Result.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/Helpers.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cmath>

namespace MagField {
  class IMagFieldSvc;
}

namespace Acts {
class Surface;
class BoundaryCheck;
}


class ActsExtrapolationTool : public extends<AthAlgTool, IActsExtrapolationTool>
{

public:
  virtual StatusCode initialize() override;

  ActsExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

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
            double pathLimit = std::numeric_limits<double>::max()) const override
  {
    using namespace Acts::UnitLiterals;
    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

    Acts::MagneticFieldContext mctx;
    const ActsGeometryContext& gctx
      = m_trackingGeometryTool->getGeometryContext(ctx);

    auto anygctx = gctx.any();

    Options options(anygctx, mctx);
    options.pathLimit = pathLimit;
    bool debug = msg().level() == MSG::VERBOSE;
    options.debug = debug;

    options.loopProtection
      = (Acts::VectorHelpers::perp(startParameters.momentum())
          < m_ptLoopers * 1_MeV);
    options.maxStepSize = m_maxStepSize * 1_m;

    std::vector<Acts::detail::Step> steps;
    DebugOutput::result_type debugOutput;

    auto res = boost::apply_visitor([&](const auto& propagator) -> ResultType {
        auto result = propagator.propagate(startParameters, options);
        if (!result.ok()) {
        return result.error();
        }
        auto& propRes = *result;

        auto steppingResults = propRes.template get<SteppingLogger::result_type>();
        auto debugOutput = propRes.template get<DebugOutput::result_type>();
        // try to force return value optimization, not sure this is necessary
        return std::make_pair(std::move(steppingResults.steps), std::move(debugOutput));
        }, *m_varProp);

    if (!res.ok()) {
      ATH_MSG_ERROR("Got error during propagation:" << res.error()
          << ". Returning empty step vector.");
      return {};
    }
    std::tie(steps, debugOutput) = std::move(*res);

    if(debug) {
      ATH_MSG_VERBOSE(debugOutput.debugString);
    }

    ATH_MSG_VERBOSE("Collected " << steps.size() << " steps");
    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " end");

    return steps;
  }

  virtual
  const IActsTrackingGeometryTool*
  trackingGeometryTool() const override
  {
    return m_trackingGeometryTool.get();
  }


private:
  // Action list and abort list
  using ActionList = Acts::ActionList<SteppingLogger, DebugOutput>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;

  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

  using VariantPropagator = boost::variant<
    Acts::Propagator<Acts::EigenStepper<ATLASMagneticFieldWrapper>, Acts::Navigator>,
    Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>, Acts::Navigator>
  >;

  std::unique_ptr<VariantPropagator> m_varProp;

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS", "Either ATLAS or Constant"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}, "Constant field value to use if FieldMode == Constant"};

  Gaudi::Property<double> m_ptLoopers{this, "PtLoopers", 300, "PT loop protection threshold. Will be converted to Acts MeV unit"};

  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10, "Max step size in Acts m unit"};
};



#endif

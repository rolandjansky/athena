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
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Propagator/detail/StandardAborters.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "Acts/MagneticField/ConstantBField.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

namespace MagField {
  class IMagFieldSvc;
}

namespace Acts {
class Surface;
class BoundaryCheck;
}

static const InterfaceID IID_ActsExtrapolationTool("ActsExtrapolationTool", 1, 0);

class ActsExtrapolationTool : public AthAlgTool
{

public:
  virtual StatusCode initialize() override;

  ActsExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  template <typename parameters_t>
  std::vector<Acts::detail::Step>
  propagate(const parameters_t& startParameters,
            double pathLimit = std::numeric_limits<double>::max()) const
  {
    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

    Options options;
    options.pathLimit = pathLimit;
    bool debug = msg().level() == MSG::VERBOSE;
    options.debug = debug;

    options.loopProtection
      = (Acts::VectorHelpers::perp(startParameters.momentum())
          < m_ptLoopers * Acts::units::_MeV);

    options.maxStepSize = m_maxStepSize * Acts::units::_m;

    PropagatorVisitor<parameters_t, Options> visitor(startParameters, std::move(options));

    std::vector<Acts::detail::Step> steps;
    DebugOutput::result_type debugOutput;
    std::tie(steps, debugOutput) = boost::apply_visitor(visitor, *m_varProp);

    if(debug) {
      ATH_MSG_VERBOSE(debugOutput.debugString);
    }

    ATH_MSG_VERBOSE("Collected " << steps.size() << " steps");
    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " end");

    return steps;
  }

  void
  prepareAlignment() const;

private:
  // set up options for propagation
  using SteppingLogger = Acts::detail::SteppingLogger;
  using DebugOutput = Acts::detail::DebugOutputActor;
  using EndOfWorld = Acts::detail::EndOfWorldReached;

  // Action list and abort list
  using ActionList = Acts::ActionList<SteppingLogger, DebugOutput>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;

  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

  using VariantPropagator = boost::variant<
    Acts::Propagator<Acts::EigenStepper<ATLASMagneticFieldWrapper>, Acts::Navigator>,
    Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>, Acts::Navigator>
  >;

  std::unique_ptr<VariantPropagator> m_varProp;

  template <typename parameters_t, typename options_t>
  struct PropagatorVisitor 
  : boost::static_visitor<std::pair<std::vector<Acts::detail::Step>, 
                                    DebugOutput::result_type>> {

    PropagatorVisitor(const parameters_t& parameters, options_t options)
      : m_parameters(parameters), m_options(std::move(options))
    {}

    template <typename propagator_t>
    std::pair<std::vector<Acts::detail::Step>, DebugOutput::result_type>
    operator()(const propagator_t& propagator) const
    {
      const auto& result = propagator.propagate(m_parameters, m_options);
      auto steppingResults = result.template get<SteppingLogger::result_type>();
      auto debugOutput = result.template get<DebugOutput::result_type>();
      // try to force return value optimization, not sure this is necessary
      return {std::move(steppingResults.steps), std::move(debugOutput)};
    }

    const parameters_t& m_parameters;
    options_t m_options;

  };
  

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ToolHandle<ActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};

  Options m_propagationOptions;

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}};

  Gaudi::Property<double> m_ptLoopers{this, "PtLoopers", 300, "PT loop protection threshold. Will be converted to Acts MeV unit"};

  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10, "Max step size in Acts m unit"};
};



#endif

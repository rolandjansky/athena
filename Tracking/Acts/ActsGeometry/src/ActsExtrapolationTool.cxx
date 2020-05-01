/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExtrapolationTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ACTS
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/BoundaryCheck.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

// STL
#include <iostream>
#include <memory>


namespace ActsExtrapolationDetail {
  using VariantPropagatorBase = boost::variant<
    Acts::Propagator<Acts::EigenStepper<ATLASMagneticFieldWrapper>, Acts::Navigator>,
    Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>, Acts::Navigator>
  >;

  class VariantPropagator : public VariantPropagatorBase
  {
  public:
    using VariantPropagatorBase::VariantPropagatorBase;
  };

}


using ActsExtrapolationDetail::VariantPropagator;


ActsExtrapolationTool::ActsExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent),
    m_fieldServiceHandle("AtlasFieldSvc", name)
{

}


ActsExtrapolationTool::~ActsExtrapolationTool()
{
}


StatusCode
ActsExtrapolationTool::initialize()
{
  using namespace std::literals::string_literals;


  ATH_MSG_INFO("Initializing ACTS extrapolation");

  ATH_CHECK( m_trackingGeometryTool.retrieve() );
  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
    = m_trackingGeometryTool->trackingGeometry();

  Acts::Navigator navigator(trackingGeometry);

  if (m_fieldMode == "ATLAS"s) {
    // we need the field service
    ATH_CHECK( m_fieldServiceHandle.retrieve() );
    ATH_MSG_INFO("Using ATLAS magnetic field service");
    using BField_t = ATLASMagneticFieldWrapper;
    BField_t bField(m_fieldServiceHandle.get());
    auto stepper = Acts::EigenStepper<BField_t>(std::move(bField));
    auto propagator = Acts::Propagator<decltype(stepper), Acts::Navigator>(std::move(stepper), 
                                                                      std::move(navigator));
    m_varProp = std::make_unique<VariantPropagator>(propagator);
  }
  else if (m_fieldMode == "Constant") {
    std::vector<double> constantFieldVector = m_constantFieldVector;
    double Bx = constantFieldVector.at(0);
    double By = constantFieldVector.at(1);
    double Bz = constantFieldVector.at(2);
    ATH_MSG_INFO("Using constant magnetic field: (Bx, By, Bz) = (" << Bx << ", " << By << ", " << Bz << ")");
    using BField_t = Acts::ConstantBField;
    BField_t bField(Bx, By, Bz);
    auto stepper = Acts::EigenStepper<BField_t>(std::move(bField));
    auto propagator = Acts::Propagator<decltype(stepper), Acts::Navigator>(std::move(stepper), 
                                                                      std::move(navigator));
    m_varProp = std::make_unique<VariantPropagator>(propagator);
  }

  ATH_MSG_INFO("ACTS extrapolation successfully initialized");
  return StatusCode::SUCCESS;
}


std::vector<Acts::detail::Step>
ActsExtrapolationTool::propagate(const EventContext& ctx,
                                 const Acts::BoundParameters& startParameters,
                                 double pathLimit /*= std::numeric_limits<double>::max()*/) const
{
  using namespace Acts::UnitLiterals;
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

  Acts::MagneticFieldContext mctx;
  const ActsGeometryContext& gctx
    = m_trackingGeometryTool->getGeometryContext(ctx);

  auto anygctx = gctx.any();

  // Action list and abort list
  using ActionList = Acts::ActionList<SteppingLogger, DebugOutput>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;

  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

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

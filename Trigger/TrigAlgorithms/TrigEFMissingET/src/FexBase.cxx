/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file FexBase.cxx
 *
 * Implementation of base Fex class
 * @author Jon Burr
 *****************************************************************************/

#include "FexBase.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/StatusFlags.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include <memory>
#include <algorithm>
#include <iterator>

namespace {
  // Convert from MeV to GeV if above threshold, else fallback value
  float toLinGeV(float x, float fallback=0, float epsilon=1e-6)
  {
    float xGeV = x / Gaudi::Units::GeV;
    if (std::fabs(xGeV) < epsilon)
      return fallback;
    return xGeV;
  }
  // convert from MeV to GeV and then log10, preserving the sign and the minimum
  // dictated by monitoring histograms
  float toLog10GeV(float x, float fallback=0, float epsilon=1e-6)
  {
    float absXGeV = std::fabs(x / Gaudi::Units::GeV);
    if (absXGeV < epsilon)
      return fallback;
    return std::copysign(std::log10(absXGeV), x);
  }
} //> end anonymous namespace

namespace HLT { namespace MET {
  FexBase::FexBase(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator)
  {
  }

  StatusCode FexBase::initializeBase(
      const std::vector<std::string>& componentNames)
  {
    ATH_MSG_DEBUG("Initialising FexBase base class");
    m_baseInitialised = true;
    m_componentNames = componentNames;
    CHECK( m_metContainerKey.initialize() );
    if (!m_monTool.empty())
      CHECK( m_monTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode FexBase::execute( const EventContext& context ) const
  {
    if (!m_baseInitialised) {
      ATH_MSG_ERROR("Base class was not initialised! This means that the "
          << "derived class was not correctly written!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Executing " << name() << "...");
    Monitored::Timer totalTimer("TIME_Total");
    // Create the output
    auto metCont = std::make_unique<xAOD::TrigMissingETContainer>();
    auto metContAux = std::make_unique<xAOD::TrigMissingETAuxContainer>();
    metCont->setStore(metContAux.get() );

    // Create the actual output object
    metCont->push_back(std::make_unique<xAOD::TrigMissingET>());
    xAOD::TrigMissingET* met = metCont->back();

    // Initialise the components
    met->defineComponents(m_componentNames);
    // We also need to initialise all of the values. This is not done by the EDM
    // class so you can easily get missing aux element errors if you don't do
    // this
    met->setEx(0);
    met->setEy(0);
    met->setEz(0);
    met->setSumEt(0);
    met->setSumE(0);
    met->setFlag(0);
    met->setRoiWord(0);

    MonGroupBuilder monitors;
    CHECK( fillMET(*met, context, monitors) );
    // Add flags
    CHECK( flagMET(*met) );
    // Add extra monitors
    CHECK( monitor(*met, monitors) );
    // Create the actual group and trigger the monitoring
    monitors.build(m_monTool);

    // Output REGTEST information
    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG( "REGTEST " << METComponent(*met) );
      ATH_MSG_DEBUG( "REGTEST flag = " << met->flag() );
      ATH_MSG_DEBUG( "REGTEST Name, status, values: ");
      for (std::size_t ii = 0; ii < met->getNumberOfComponents(); ++ii)
        ATH_MSG_DEBUG( "REGTEST "
            << met->nameOfComponent(ii) << ", "
            << met->statusComponent(ii) << ", "
            << METComponent(ii, *met) );
    }

    // Push this output to the store
    auto handle = SG::makeHandle(m_metContainerKey, context);
    CHECK( handle.record( std::move(metCont), std::move(metContAux) ) );
    return StatusCode::SUCCESS;
  }

  StatusCode FexBase::flagMET(xAOD::TrigMissingET& met) const
  {
    // Start with the components
    // Keep a flag to OR into the main value
    int overall = 0;
    for (std::size_t ii = 0; ii < met.getNumberOfComponents(); ++ii) {
      METComponent component(ii, met);
      if (component.sumEt > 0 &&
          component.met() / component.sumEt > m_maxComponentMETSumEtRatio) {
        component.status |= StatusFlag::ComponentBigMEtSEtRatio;
        overall |= StatusFlag::ComponentBigMEtSEtRatio;
      }
      met.setStatusComponent(ii, component.status);
    }
    METComponent total(met);
    total.status |= overall;
    if (total.sumEt > 0 &&
        total.met() / total.sumEt > m_maxGlobalMETSumEtRatio)
      total.status |= StatusFlag::GlobalBigMEtSEtRatio;
    met.setFlag(total.status);
    return StatusCode::SUCCESS;
  }

  StatusCode FexBase::monitor(
      const xAOD::TrigMissingET& met,
      MonGroupBuilder& monitors) const
  {
    METComponent metComponent(met);
    // Increase the capacity of the vector
    monitors.increaseCapacity(16, true);
    // Add the standard variables
    monitors.add(Monitored::Scalar("EF_MEx_log", toLog10GeV(metComponent.mpx) ) );
    monitors.add(Monitored::Scalar("EF_MEy_log", toLog10GeV(metComponent.mpy) ) );
    monitors.add(Monitored::Scalar("EF_MEz_log", toLog10GeV(metComponent.mpz) ) );
    monitors.add(Monitored::Scalar("EF_MET_log", toLog10GeV(metComponent.met() ) ) );
    monitors.add(Monitored::Scalar("EF_ME_log", toLog10GeV(metComponent.magnitude() ) ) );
    monitors.add(Monitored::Scalar("EF_SumEt_log", toLog10GeV(metComponent.sumEt) ) );
    monitors.add(Monitored::Scalar("EF_SumE_log", toLog10GeV(metComponent.sumE, -9e9) ) );
    monitors.add(Monitored::Scalar("EF_MEx_lin", toLinGeV(metComponent.mpx) ) );
    monitors.add(Monitored::Scalar("EF_MEy_lin", toLinGeV(metComponent.mpy) ) );
    monitors.add(Monitored::Scalar("EF_MEz_lin", toLinGeV(metComponent.mpz) ) );
    monitors.add(Monitored::Scalar("EF_MET_lin", toLinGeV(metComponent.met() ) ) );
    monitors.add(Monitored::Scalar("EF_ME_lin", toLinGeV(metComponent.magnitude() ) ) );
    monitors.add(Monitored::Scalar("EF_SumEt_lin", toLinGeV(metComponent.sumEt) ) );
    monitors.add(Monitored::Scalar("EF_SumE_lin", toLinGeV(metComponent.sumE) ) );
    monitors.add(Monitored::Scalar(
          "EF_XS", toLinGeV(metComponent.met() ) / toLinGeV(metComponent.sumEt, 1) ) );
    monitors.add(Monitored::Scalar("EF_MET_phi", metComponent.phi() ) );
    return StatusCode::SUCCESS;
  }
} } //> end namespace HLT::MET

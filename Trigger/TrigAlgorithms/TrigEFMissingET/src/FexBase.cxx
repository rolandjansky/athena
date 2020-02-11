/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "FexBase.h"
#include "TrigEFMissingET/METMonitor.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/StatusFlags.h"
#include "TrigInterfaces/HLTCheck.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace HLT { namespace MET {

  FexBase::FexBase(
      const std::string& name, ISvcLocator* pSvcLocator) :
    HLT::AllTEAlgo(name, pSvcLocator)
  {
    declareProperty("MissingETOutputKey", m_metOutputKey="TrigEFMissingET",
        "The name of the output TE in the navigation.");
    // Flag-related properties
    declareProperty("MaxComponentMetSumEtRatio", m_maxComponentMetSumEtRatio=1.,
        "The maximum MET/SumEt ratio per component.");
    declareProperty("MaxGlobalMetSumEtRatio", m_maxGlobalMetSumEtRatio=1.,
        "The maximum MET/SumEt ratio for the total value.");
  }

  FexBase::~FexBase() {}

  HLT::ErrorCode FexBase::initializeBase(
      const std::vector<std::string>& componentNames,
      std::unique_ptr<IMETMonitor> monitor)
  {
    m_baseInitialised = true;
    m_componentNames = componentNames;
    if (!monitor)
      // If we didn't provide a monitor then just create the default one
      m_monitor = std::make_unique<METMonitor>(this);
    else
      m_monitor = std::move(monitor);
    m_timer = addTimer(name() + "TotalTimer");
    if (!m_timer)
      // This only happens if the timing is deliberately disabled
      ATH_MSG_DEBUG("Will not time algorithm");
    return HLT::OK;
  }

  HLT::ErrorCode FexBase::flagMET(xAOD::TrigMissingET& met)
  {
    // Start with the components
    // Keep a flag to OR into the main value
    int overall = 0;
    for (std::size_t ii = 0; ii < met.getNumberOfComponents(); ++ii) {
      METComponent component(ii, met);
      if (component.sumEt > 0 &&
          component.met() / component.sumEt > m_maxComponentMetSumEtRatio) {
        component.status |= StatusFlag::ComponentBigMEtSEtRatio;
        overall |= StatusFlag::ComponentBigMEtSEtRatio;
      }
      met.setStatusComponent(ii, component.status);
    }
    METComponent total(met);
    total.status |= overall;
    if (total.sumEt > 0 &&
        total.met() / total.sumEt > m_maxGlobalMetSumEtRatio)
      total.status |= StatusFlag::GlobalBigMEtSEtRatio;
    met.setFlag(total.status);
    return HLT::OK;
  }

  HLT::ErrorCode FexBase::hltExecute(
      std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
      unsigned int type_out)
  {
    ATH_MSG_DEBUG( "Executing MET Fex " << name() );
    if (!m_baseInitialised) {
      ATH_MSG_ERROR("Base class was not initialised!");
      return HLT::ERROR;
    }
    if (m_timer)
      m_timer->start();
    std::unique_ptr<xAOD::TrigMissingET> met = makeEDMObject();
    // Tell the code that we're going to monitor
    // Ignore is safe because it never returns a failure.
    beforeExecMonitors().ignore();
    m_monitor->reset();
    HLT_CHECK( fillMET(*met, tes_in) );
    ATH_MSG_DEBUG("Filled MET, now process flags");
    HLT_CHECK( flagMET(*met) );
    ATH_MSG_DEBUG("Fill monitoring variables");
    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("Total MET calculated: " << METComponent(*met) );
      for (std::size_t ii = 0; ii < met->getNumberOfComponents(); ++ii)
        ATH_MSG_DEBUG("Component " << met->nameOfComponent(ii) << ": "
            << METComponent(ii, *met) );
    }
    HLT_CHECK( m_monitor->monitor(*met) );
    // Ignore is safe because it never returns a failure.
    afterExecMonitors().ignore();
    ATH_MSG_DEBUG("Write output");
    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("REGTEST " << METComponent(*met) );
      ATH_MSG_DEBUG("REGTEST flag = " << met->flag() );
      ATH_MSG_DEBUG("REGTEST Name, status, values:");
      for (std::size_t ii = 0; ii < met->getNumberOfComponents(); ++ii)
        ATH_MSG_DEBUG("REGTEST "
            << met->nameOfComponent(ii) << ", "
            << met->statusComponent(ii) << ", "
            << METComponent(ii, *met) );
    }
    HLT_CHECK( makeOutputTE(tes_in, type_out, std::move(met) ) );
    if (m_timer)
      m_timer->stop();
    return HLT::OK;
  }


  std::unique_ptr<xAOD::TrigMissingET> FexBase::makeEDMObject()
  {
    auto met = std::make_unique<xAOD::TrigMissingET>();
    // Create a private store as it never meets its real auxstore in this class.
    met->makePrivateStore();
    // Define the components that are available in this object.
    met->defineComponents(m_componentNames);

    // By default, the EDM isn't creating *any* of our variables. Therefore we
    // need to set everything to defaults first here or something later down
    // could throw a missing aux element exception
    met->setEx(0);
    met->setEy(0);
    met->setEz(0);
    met->setSumEt(0);
    met->setSumE(0);
    met->setFlag(0);
    met->setRoiWord(0);

    return std::move(met);
  }

  HLT::ErrorCode FexBase::makeOutputTE(
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
      unsigned int type_out,
      std::unique_ptr<xAOD::TrigMissingET> met)
  {
    // "Only count MET as an input TE (for seeding relation of navigation structure)"
    // ** I don't really know what this means. Whichever TE this actually is is
    // very rarely going to be a MET TE.
    //
    // However this is how our navigation has worked since ~forever so a priori
    // I'm not going to change it for this rewrite.
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

    ATH_MSG_DEBUG("Attaching output feature " << m_metOutputKey);
    // Create the output TE
    HLT::TriggerElement* outputTE =
      config()->getNavigation()->addNode(allTEs, type_out);
    HLT::ErrorCode status =
      attachFeature(outputTE, met.release(), m_metOutputKey);
    if (status != HLT::OK) {
      ATH_MSG_ERROR("Failed to write output TE");
      return status;
    }
    return HLT::OK;
  }


} } //> end namespace HLT::MET

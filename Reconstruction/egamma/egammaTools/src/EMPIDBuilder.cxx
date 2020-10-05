/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "EMPIDBuilder.h"
#include "AthenaKernel/errorcheck.h"
#include "PATCore/AcceptData.h"
#include "xAODEgamma/Egamma.h"

EMPIDBuilder::EMPIDBuilder(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : egammaBaseTool(type, name, parent)
{
  // declare interface
  declareInterface<IegammaBaseTool>(this);
}

StatusCode
EMPIDBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing EMPIDBuilder");

  ATH_CHECK(m_electronIsEMselectors.retrieve());

  if (m_electronIsEMselectors.size() !=
      m_electronIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given "
                  "electronIsEMselector names");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_electronLHselectors.retrieve());

  if (m_electronLHselectors.size() != m_electronLHselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given "
                  "electron LH selector names");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_genericIsEMselectors.retrieve());

  if (m_genericIsEMselectors.size() !=
      m_genericIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given "
                  "generic selector names");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_photonIsEMselectors.retrieve());

  if (m_photonIsEMselectors.size() != m_photonIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given "
                  "photon selector names");
    return StatusCode::FAILURE;
  }

  if (m_UselumiBlockMuTool) {
    ATH_MSG_INFO("enabling lumiBlockMuTool");
    if (m_lumiBlockMuTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve Luminosity Tool");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");

  } else {
    ATH_MSG_INFO("disabling lumiBlockMuTool");
    m_lumiBlockMuTool.disable();
  }
  return StatusCode::SUCCESS;
}

StatusCode
EMPIDBuilder::execute(const EventContext& ctx, xAOD::Egamma* eg) const
{

  ATH_MSG_DEBUG("Executing EMPIDBuilder::execute");

  // Protection against bad pointers
  if (eg == nullptr) {
    ATH_MSG_WARNING("NULL egamma object given to execute function");
    return StatusCode::SUCCESS;
  }

  size_t size = m_electronIsEMselectors.size();

  for (size_t i = 0; i < size; ++i) {
    asg::AcceptData accept = m_electronIsEMselectors[i]->accept(ctx, eg);
    // save the bool result
    eg->setPassSelection(static_cast<bool>(accept),
                         m_electronIsEMselectorResultNames[i]);
    // save the isem
    unsigned int isEM = (~0);
    if (m_electronIsEMselectors[i]->execute(ctx, eg, isEM).isFailure()) {
      ATH_MSG_ERROR("problem to get isEM for "
                    << m_electronIsEMselectorResultNames[i]);
      return StatusCode::FAILURE;
    }
    eg->setSelectionisEM(isEM, "isEM" + m_electronIsEMselectorResultNames[i]);
  }

  size_t sizePh = m_photonIsEMselectors.size();

  for (size_t i = 0; i < sizePh; ++i) {
    asg::AcceptData accept = m_photonIsEMselectors[i]->accept(ctx, eg);
    // save the bool result
    eg->setPassSelection(static_cast<bool>(accept),
                         m_photonIsEMselectorResultNames[i]);
    // save the isem
    unsigned int isEM = ~0;
    if (m_photonIsEMselectors[i]->execute(ctx, eg, isEM).isFailure()) {
      ATH_MSG_ERROR("problem to get isEM for "
                    << m_photonIsEMselectorResultNames[i]);
      return StatusCode::FAILURE;
    }
    eg->setSelectionisEM(isEM, "isEM" + m_photonIsEMselectorResultNames[i]);
  }

  size_t sizeLH = m_electronLHselectors.size();

  // negative mu means the default behaviour --> retrieve the one in xAOD
  double mu = -99.;
  double avg_mu = -99.;
  if (m_UselumiBlockMuTool) { //
    mu = m_lumiBlockMuTool->actualInteractionsPerCrossing();
    avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
    ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu);
    ATH_MSG_DEBUG("REGTEST: Average Mu Value   : " << avg_mu);
  }

  for (size_t i = 0; i < sizeLH; ++i) {
    asg::AcceptData accept = m_electronLHselectors[i]->accept(ctx, eg, avg_mu);
    // save the bool result
    eg->setPassSelection(static_cast<bool>(accept),
                         m_electronLHselectorResultNames[i]);
    // save the isem
    eg->setSelectionisEM(
      static_cast<unsigned int>(accept.getCutResultInverted()),
      "isEM" + m_electronLHselectorResultNames[i]);

    // save the LHValue only once
    if (i == 0) {
      eg->setLikelihoodValue(
        static_cast<float>(
          m_electronLHselectors[i]->calculate(ctx, eg, avg_mu)),
        m_LHValueName);
    }
  }

  size_t sizeGen = m_genericIsEMselectors.size();
  for (size_t i = 0; i < sizeGen; ++i) {
    asg::AcceptData accept = m_genericIsEMselectors[i]->accept(eg);
    // save the bool result
    eg->setPassSelection(static_cast<bool>(accept),
                         m_genericIsEMselectorResultNames[i]);
    // save the isem
    eg->setSelectionisEM(
      static_cast<unsigned int>(accept.getCutResultInverted()),
      "isEM" + m_genericIsEMselectorResultNames[i]);
  }

  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "AthenaKernel/errorcheck.h"

#include "EMPIDBuilder.h"
#include "xAODEgamma/Egamma.h"
//#include "LumiBlockComps/LumiBlockMuTool.h"
#include "PATCore/TAccept.h"            // for TAccept
#include "PATCore/TResult.h"            // for TResult

// ===========================================================================
EMPIDBuilder::EMPIDBuilder(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
  : egammaBaseTool(type, name, parent)
{
  //
  // constructor
  //

  // declare interface
  declareInterface<IegammaBaseTool>(this);

}

// ===============================================================
EMPIDBuilder::~EMPIDBuilder()
{ 
  //
  // destructor
  //
}

// ==================================================================
StatusCode EMPIDBuilder::initialize()
{
  //
  // initialize method
  //

  ATH_MSG_DEBUG(" Initializing EMPIDBuilder");
 
  for (const auto& selector : m_electronIsEMselectors) {
    CHECK(selector.retrieve());
  }

  if (m_electronIsEMselectors.size() != m_electronIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given electronIsEMselector names");
    return StatusCode::FAILURE;
  }

  for (const auto& selector : m_electronLHselectors) {
    CHECK(selector.retrieve());
  }

  if (m_electronLHselectors.size() != m_electronLHselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given electron LH selector names");
    return StatusCode::FAILURE;
  }


  for (const auto& selector : m_genericIsEMselectors) {
    CHECK(selector.retrieve());
  }

  if (m_genericIsEMselectors.size() != m_genericIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given generic selector names");
    return StatusCode::FAILURE;
  }


  for (const auto& selector : m_photonIsEMselectors) {
    CHECK(selector.retrieve());
  }

  if (m_photonIsEMselectors.size() != m_photonIsEMselectorResultNames.size()) {
    ATH_MSG_ERROR("The number of selectors does not match the number of given photon selector names");
    return StatusCode::FAILURE;
  }

  if (m_UselumiBlockMuTool) {
    // retrieve the lumi tool
    if (m_lumiBlockMuTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve Luminosity Tool");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
    }
  } else {
    // Switch to using disable when supported by Gaudi version
    m_lumiBlockMuTool = nullptr;
    // m_lumiBlockMuTool.disable();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================
StatusCode EMPIDBuilder::finalize()
{
  //
  // finalize method
  //

  return StatusCode::SUCCESS;
}


// =====================================================================
StatusCode EMPIDBuilder::execute(xAOD::Egamma* eg)
{ 
  
  ATH_MSG_DEBUG("Executing EMPIDBuilder::execute");

  // Protection against bad pointers
  if (eg==0) {
    ATH_MSG_WARNING("NULL egamma object given to execute function");
    return StatusCode::SUCCESS;
  }
  
  size_t size = m_electronIsEMselectors.size();

  for (size_t i = 0; i<size;++i) {
    const Root::TAccept& accept = m_electronIsEMselectors[i]->accept(eg);
    //save the bool result
    eg->setPassSelection(static_cast<bool>(accept), m_electronIsEMselectorResultNames[i]);
    //save the isem
    eg->setSelectionisEM(m_electronIsEMselectors[i]->IsemValue(), "isEM"+m_electronIsEMselectorResultNames[i]);

  }

  size_t sizePh = m_photonIsEMselectors.size();

  for (size_t i = 0; i<sizePh;++i) {
    const Root::TAccept& accept = m_photonIsEMselectors[i]->accept(eg);
    //save the bool result
    eg->setPassSelection(static_cast<bool>(accept), m_photonIsEMselectorResultNames[i]);
    //save the isem
    eg->setSelectionisEM(m_photonIsEMselectors[i]->IsemValue(), "isEM"+m_photonIsEMselectorResultNames[i]);
  }

  size_t sizeLH = m_electronLHselectors.size();

    //negative mu means the default behaviour --> retrieve the one in xAOD 
    double mu = -99.;
    double avg_mu = -99.;
    if(m_UselumiBlockMuTool){ //
      mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
      avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu);
      ATH_MSG_DEBUG("REGTEST: Average Mu Value   : " << avg_mu);
    }

  for (size_t i = 0; i<sizeLH; ++i) {
  
    const Root::TAccept& accept = m_electronLHselectors[i]->accept(eg,avg_mu);
    //save the bool result
    eg->setPassSelection(static_cast<bool>(accept), m_electronLHselectorResultNames[i]);
    //save the isem
    eg->setSelectionisEM(static_cast<unsigned int> (accept.getCutResultInverted()), "isEM"+m_electronLHselectorResultNames[i]); 
    
    //save the LHValue only once
    if(i==0){
      eg->setLikelihoodValue(static_cast<float>(m_electronLHselectors[i]->getTResult().getResult(0)),m_LHValueName);
    }  
  }
  
  size_t sizeGen = m_genericIsEMselectors.size();
  for (size_t i = 0; i<sizeGen;++i) {
    const Root::TAccept& accept = m_genericIsEMselectors[i]->accept(eg);
    //save the bool result
    eg->setPassSelection(static_cast<bool>(accept), m_genericIsEMselectorResultNames[i]);
    //save the isem
    eg->setSelectionisEM(static_cast<unsigned int> (accept.getCutResultInverted()), "isEM"+m_genericIsEMselectorResultNames[i]);
  }

  
  return StatusCode::SUCCESS;
}




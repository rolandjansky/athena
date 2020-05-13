/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGElectronLikelihoodToolWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)

#include "DerivationFrameworkEGamma/EGElectronLikelihoodToolWrapper.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"


namespace DerivationFramework {

  EGElectronLikelihoodToolWrapper::EGElectronLikelihoodToolWrapper(const std::string& t,
								   const std::string& n,
								   const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_cut(""),
    m_sgName(""),
    m_containerName(""),
    m_storeTResult(false)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("EGammaElectronLikelihoodTool", m_tool);
    declareProperty("EGammaFudgeMCTool", m_fudgeMCTool);
    declareProperty("CutType", m_cut);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("ContainerName", m_containerName);
    declareProperty("StoreTResult", m_storeTResult);
  }

  StatusCode EGElectronLikelihoodToolWrapper::initialize()
  {
    if (m_sgName.empty()) {
      ATH_MSG_ERROR("No SG name provided for the output of EGElectronLikelihoodToolWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_containerName!="Photons" && m_containerName!="Electrons") {
      ATH_MSG_ERROR("Wrong container provided!");
      return StatusCode::FAILURE;
    }
    CHECK(m_tool.retrieve());
    if (!(m_fudgeMCTool.name().empty())) CHECK(m_fudgeMCTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode EGElectronLikelihoodToolWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode EGElectronLikelihoodToolWrapper::addBranches() const
  {
    // retrieve container
    const xAOD::IParticleContainer* particles = evtStore()->retrieve< const xAOD::IParticleContainer >( m_containerName );
    if( ! particles ) {
        ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_containerName );
        return StatusCode::FAILURE;
    }

    // Decorator
    SG::AuxElement::Decorator< char > decoratorPass(m_sgName);
    SG::AuxElement::Decorator< unsigned int > decoratorIsEM(m_sgName + "IsEMValue");

    // Write mask for each element and record to SG for subsequent selection
    for (xAOD::IParticleContainer::const_iterator pItr = particles->begin(); pItr!=particles->end(); ++pItr) {

      xAOD::Type::ObjectType type = (*pItr)->type();
      if (type!=xAOD::Type::Electron && type!=xAOD::Type::Photon) {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type (not electron nor photon) being passed to EGElectronLikelihoodToolWrapper");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Electron && m_containerName!="Electrons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGElectronLikelihoodToolWrapper");
	  return StatusCode::FAILURE;
      }
      if (type==xAOD::Type::Photon && m_containerName!="Photons") {
	  ATH_MSG_ERROR ("addBranches(): Wrong particle type being passed to EGElectronLikelihoodToolWrapper");
	  return StatusCode::FAILURE;
      }
      
      const xAOD::IParticle* pCopy = *pItr;

      // this should be computed based on some property of the tool or the existence of the ElectronPhotonShowerShapeFudgeTool
      bool applyFF = (!m_fudgeMCTool.empty());
      if (applyFF) {
	// apply the shower shape corrections
	CP::CorrectionCode correctionCode = CP::CorrectionCode::Ok;
	if (type==xAOD::Type::Electron) {
	    const xAOD::Electron* eg = static_cast<const xAOD::Electron*>(*pItr);
	    xAOD::Electron* el = nullptr;
	    correctionCode = m_fudgeMCTool->correctedCopy(*eg, el);
	    pCopy = el;
	}
	else {
	    const xAOD::Photon* eg = static_cast<const xAOD::Photon*>(*pItr);
	    xAOD::Photon* ph = nullptr;
	    correctionCode = m_fudgeMCTool->correctedCopy(*eg, ph);
	    pCopy = ph;
	}
	if (correctionCode==CP::CorrectionCode::Ok)
	    ;
	else if (correctionCode==CP::CorrectionCode::Error)
	    Error("addBranches()","Error applying fudge factors to current photon");
	else if (correctionCode==CP::CorrectionCode::OutOfValidityRange)
	    Warning("addBranches()","Current object has no valid fudge factors due to out-of-range");
	else
	    Warning("addBranches()","Unknown correction code %d from ElectronPhotonShowerShapeFudgeTool",(int) correctionCode);
      }

      // compute the output of the selector
      asg::AcceptData theAccept(m_tool->accept(pCopy));
      unsigned int isEM = (unsigned int) theAccept.getCutResultInvertedBitSet().to_ulong(); // this should work for both the cut-based and the LH selectors
      double result(0.); // initialise explicitly to avoid compilation warning. It will be overridden in the following block (result is used only if m_storeTResult is true)

      // Lukas Heinrich: interface in master not yet available.
      //      if (m_storeTResult) {
      //	result = double(m_tool->calculate(pCopy));
      //      }
      
      // decorate the original object
      if(m_cut.empty()){
	bool pass_selection = (bool) theAccept;
	if(pass_selection) decoratorPass(**pItr) = 1;
	else decoratorPass(**pItr) = 0;
	decoratorIsEM(**pItr) = isEM;
	if (m_storeTResult) {
	  SG::AuxElement::Decorator< double > decoratorResult(m_sgName + "Result");
	  decoratorResult(**pItr) = result;
	}
      }
      else{
	if (theAccept.getCutResult(m_cut)) {
	  decoratorPass(**pItr) = 1;
	} else {
	  decoratorPass(**pItr) = 0;
	}
	decoratorIsEM(**pItr) = isEM;
	if (m_storeTResult) {
	  SG::AuxElement::Decorator< double > decoratorResult(m_sgName + "Result");
	  decoratorResult(**pItr) = result;
	}
      }

      // delete the particle copy
      if (applyFF) delete pCopy;
    }
    
    return StatusCode::SUCCESS;
  }
}

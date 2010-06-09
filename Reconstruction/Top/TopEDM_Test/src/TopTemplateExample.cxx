/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEDM_Test/TopTemplateExample.h"

#include "StoreGate/StoreGateSvc.h"

#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarLJContainer.h"

#include "JetEvent/JetCollection.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaPIDdefs.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

//================================================================
TopTemplateExample::TopTemplateExample(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("ttbarEJContainerName", m_ttbarEJContainerName="TTbarEJCandidates");
  declareProperty("ttbarMuJContainerName", m_ttbarMuJContainerName="TTbarMuJCandidates");
}

//================================================================
StatusCode TopTemplateExample::initialize()
{
  ATH_MSG_INFO("TopTemplateExample::initialize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopTemplateExample::finalize() 
{
  ATH_MSG_INFO("TopTemplateExample::finalize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopTemplateExample::execute() {
  using namespace TopRec;
  ATH_MSG_DEBUG("TopTemplateExample::execute() begin");

  //----------------------------------------------------------------
  // Get input ttbars
  const TTbarEJContainer *ejContainer(0);
  const TTbarMuJContainer *mujContainer(0);
  if(  sgSvc()->retrieve(ejContainer, m_ttbarEJContainerName).isSuccess()
       && sgSvc()->retrieve(mujContainer, m_ttbarMuJContainerName).isSuccess() 
       )
    { 
      //----------------
      // Run the electron-specific analysis
      ATH_MSG_INFO("Running electron channel analysis");
      electronChannelAnalysis(*ejContainer);

      //----------------
      // Run the common lepton analysis on ttbars in different channels
      ATH_MSG_INFO("Running generic analysis on the electron channel");
      genericLeptonAnalysis(*ejContainer);

      ATH_MSG_INFO("Running generic analysis on the muon channel");
      genericLeptonAnalysis(*mujContainer);
    }
  else {
    ATH_MSG_WARNING("Problem getting input ttbars.");
  }

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("TopTemplateExample::execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
void TopTemplateExample::electronChannelAnalysis(const TopRec::TTbarEJContainer& ttbars) {
  ATH_MSG_INFO("electronChannelAnalysis(): iterating over "<<ttbars.size()<<" ttbars");

  for(unsigned i=0; i<ttbars.size(); ++i) {
    const Analysis::Electron  &electron = ttbars[i].lepton();
    ATH_MSG_INFO(

		 // Here we can access both "generic" properties
		 "electronChannelAnalysis(): electron eta = "<<electron.eta()

		 // and those that are specific to Electron
		 <<", isem(medium) = "<<electron.isem(egammaPID::ElectronMedium)

		 );
  }
}

//================================================================
// The templated analysis method
template<class TopContainer>
void TopTemplateExample::genericLeptonAnalysis(const TopContainer& ttbars) {
  ATH_MSG_INFO("genericLeptonAnalysis(): iterating over "<<ttbars.size()<<" ttbars");

  // Shorter type names for readability
  typedef typename TopContainer::value_type TTbarType; // value_type is defined in std::vector
  typedef typename TTbarType::Lepton LeptonType;

  for(unsigned i=0; i<ttbars.size(); ++i) {
    const LeptonType  &lepton = ttbars[i].lepton();

    // Here we can use only properties that are generic to all our
    // leptons, such as the 4-momentum components.  Attempt to use a
    // method that does not exist for a lepton type we use will lead
    // to a compilation error.

    ATH_MSG_INFO("genericLeptonAnalysis(): lepton eta = "<<lepton.eta());
  }
}

//================================================================

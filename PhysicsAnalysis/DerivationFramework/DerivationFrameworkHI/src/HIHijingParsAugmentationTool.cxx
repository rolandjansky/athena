/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// HIHijingParsAugmentationTool
/////////////////////////////////////////////////////////////////////
//// Author: Soumya Mohapatra (soumya@cern.ch)

 

#include "DerivationFrameworkHI/HIHijingParsAugmentationTool.h"
#include "GeneratorObjects/HijingEventParams.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>
#include <TROOT.h>

namespace DerivationFramework {

HIHijingParsAugmentationTool::HIHijingParsAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("NumHarmonics",m_nhar=6);
    declareProperty("HijingEventParamsKey",m_HijingEventParamsKey="Hijing_event_params");
    declareProperty("EventInfoKey"        ,m_EventInfo_key       ="EventInfo"          );
    gROOT->ProcessLine("gErrorIgnoreLevel = 2002");
  }


 
StatusCode HIHijingParsAugmentationTool::addBranches() const{
      // Set up the decorators
      SG::AuxElement::Decorator< std::vector<float> > decorator("HijingEventParams__Psi_n");

      const xAOD::EventInfo* l_EventInfo;
      if(evtStore()->retrieve(l_EventInfo,m_EventInfo_key).isFailure()){
        ATH_MSG_ERROR("Could not retrieve EventInfo with key"<<m_EventInfo_key);
        return StatusCode::FAILURE;
      }

      const HijingEventParams *hijing_pars;
      if(evtStore()->retrieve(hijing_pars,m_HijingEventParamsKey).isFailure()){
        ATH_MSG_ERROR("Could not retrieve HijingEventParams with key"<<m_HijingEventParamsKey);
        return StatusCode::FAILURE;
      }


      for(int ihar=0;ihar<m_nhar;ihar++){
        float Psi_n=hijing_pars->get_psi(ihar+1);
        decorator(*l_EventInfo).push_back(Psi_n);
      } 


      return StatusCode::SUCCESS;

}

}

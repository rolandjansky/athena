/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class header file
#include "TruthEDDecorator.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"

namespace DerivationFramework {

  static const SG::AuxElement::Accessor<float> acc_Density("Density");

  TruthEDDecorator::TruthEDDecorator(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("EnergyDensityKeys",m_edKeys={"TruthIsoCentralEventShape","TruthIsoForwardEventShape"});
    declareProperty("DecorationSuffix",m_ed_suffix="_rho");
  }


  TruthEDDecorator::~TruthEDDecorator(){}


  StatusCode TruthEDDecorator::initialize(){
    for (size_t i=0;i<m_edKeys.size();++i){
      m_dec_eventShape.emplace_back(m_edKeys[i]+m_ed_suffix );
    }
    return StatusCode::SUCCESS;
  }


  StatusCode TruthEDDecorator::addBranches() const{
    ATH_MSG_VERBOSE("addBranches()");

    // Get the event info that we will decorate onto
    const xAOD::EventInfo* eventInfo(nullptr);
    if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
      ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
      return StatusCode::FAILURE;
    }

    const xAOD::EventShape* eventShape(nullptr);
    for (size_t i=0;i<m_edKeys.size();++i){
      // Get the event shapes from which we'll get the densities
      ATH_CHECK( evtStore()->retrieve(eventShape,m_edKeys[i]) );
      // Decorate the densities onto the event info
      m_dec_eventShape[i](*eventInfo) = acc_Density(*eventShape);
    }

    return StatusCode::SUCCESS;
  }

} /// namespace
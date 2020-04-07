/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// UnassociatedHitsDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Olivier Arnaez ( olivier.arnaez@cern.ch )

#include "DerivationFrameworkInDet/UnassociatedHitsDecorator.h"
#include "DerivationFrameworkInDet/DecoratorUtils.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  UnassociatedHitsDecorator::UnassociatedHitsDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) :
    AthAlgTool(type,name,parent)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode UnassociatedHitsDecorator::initialize()
  {

    if (m_sgName.empty()) {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of UnassociatedHitsDecorator!");
    }

    if (m_eventInfoKey.empty()) {
      ATH_MSG_ERROR("No EventInfo collection provided for UnassociatedHitsDecorator!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_eventInfoKey.initialize() );
    // retrieve PRD association tool
    ATH_CHECK( m_UnassociatedHitsGetterTool.retrieve() );

    {
       std::vector<std::string> names;
       names.resize(kNIntDecor);
       names[knPixelUADecor]        ="nPixelUA";
       names[knBlayerUADecor]       ="nBlayerUA";
       names[knPixelBarrelUADecor]  ="nPixelBarrelUA";
       names[knPixelEndCapAUADecor] ="nPixelEndCapAUA";
       names[knPixelEndCapCUADecor] ="nPixelEndCapCUA";
       names[knSCTUADecor]          ="nSCTUA";
       names[knSCTBarrelUADecor]    ="nSCTBarrelUA";
       names[knSCTEndCapAUADecor]   ="nSCTEndCapAUA";
       names[knSCTEndCapCUADecor]   ="nSCTEndCapCUA";
       names[knTRTUADecor]          ="nTRTUA";
       names[knTRTBarrelUADecor]    ="nTRTBarrelUA";
       names[knTRTEndCapAUADecor]   ="nTRTEndCapAUA";
       names[knTRTEndCapCUADecor]   ="nTRTEndCapCUA";
       createDecoratorKeys(*this,m_eventInfoKey, m_sgName, names, m_intDecorKeys);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode UnassociatedHitsDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode UnassociatedHitsDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding unassociated hits info to EventInfo");
    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey,ctx);
    CHECK( eventInfo.isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE );

    const MinBiasPRDAssociation* prdAssoc = m_UnassociatedHitsGetterTool->get();
    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,int> >
    intDecorators = createDecorators<xAOD::EventInfo,int>(m_intDecorKeys,ctx);
    if (prdAssoc) {
      intDecorators[knPixelUADecor](*eventInfo)        = prdAssoc->nPixelUA;
      intDecorators[knBlayerUADecor](*eventInfo)       = prdAssoc->nBlayerUA;
      intDecorators[knPixelBarrelUADecor](*eventInfo)  = prdAssoc->nPixelBarrelUA;
      intDecorators[knPixelEndCapAUADecor](*eventInfo) = prdAssoc->nPixelEndCapAUA;
      intDecorators[knPixelEndCapCUADecor](*eventInfo) = prdAssoc->nPixelEndCapCUA;
      intDecorators[knSCTUADecor](*eventInfo)          = prdAssoc->nSCTUA;
      intDecorators[knSCTBarrelUADecor](*eventInfo)    = prdAssoc->nSCTBarrelUA;
      intDecorators[knSCTEndCapAUADecor](*eventInfo)   = prdAssoc->nSCTEndCapAUA;
      intDecorators[knSCTEndCapCUADecor](*eventInfo)   = prdAssoc->nSCTEndCapCUA;
      intDecorators[knTRTUADecor](*eventInfo)          = prdAssoc->nTRTUA;
      intDecorators[knTRTBarrelUADecor](*eventInfo)    = prdAssoc->nTRTBarrelUA;
      intDecorators[knTRTEndCapAUADecor](*eventInfo)   = prdAssoc->nTRTEndCapAUA;
      intDecorators[knTRTEndCapCUADecor](*eventInfo)   = prdAssoc->nTRTEndCapCUA;

      ATH_MSG_DEBUG("Decorating unassociated hits with nPixelBarrelUA = ");
      ATH_MSG_DEBUG(prdAssoc->nPixelBarrelUA);

      m_UnassociatedHitsGetterTool->releaseObject(prdAssoc);
    }
    else
      ATH_MSG_WARNING("Could not retrieve Unassociated hits information");
    
    return StatusCode::SUCCESS;
  }  
  
}

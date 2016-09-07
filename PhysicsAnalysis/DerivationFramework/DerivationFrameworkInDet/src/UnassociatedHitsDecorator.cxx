/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// UnassociatedHitsDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Olivier Arnaez ( olivier.arnaez@cern.ch )

#include "DerivationFrameworkInDet/UnassociatedHitsDecorator.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  UnassociatedHitsDecorator::UnassociatedHitsDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) : 
    AthAlgTool(type,name,parent),  
    m_sgName(""),
    m_containerName(""),
    m_UnassociatedHitsGetterTool(this)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("DecorationPrefix",       m_sgName);
    declareProperty("ContainerName",          m_containerName="EventInfo");
    declareProperty("UnassociatedHitsGetter", m_UnassociatedHitsGetterTool, "The UnassociatedHitsGetterTool instance");
  }

  StatusCode UnassociatedHitsDecorator::initialize()
  {

    if (m_sgName=="") {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of UnassociatedHitsDecorator!");
    }
    
    if (m_containerName=="") {
      ATH_MSG_ERROR("No EventInfo collection provided for UnassociatedHitsDecorator!");
      return StatusCode::FAILURE;
    }

    // retrieve PRD association tool
    CHECK( m_UnassociatedHitsGetterTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode UnassociatedHitsDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode UnassociatedHitsDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding unassociated hits info to EventInfo");

    const xAOD::EventInfo* eventInfo;
    CHECK( evtStore()->retrieve( eventInfo, m_containerName ) );

    const MinBiasPRDAssociation* prdAssoc = m_UnassociatedHitsGetterTool->get();
    if (prdAssoc) {
      eventInfo->auxdecor< int >(m_sgName+"nPixelUA")        = prdAssoc->nPixelUA;
      eventInfo->auxdecor< int >(m_sgName+"nBlayerUA")       = prdAssoc->nBlayerUA;
      eventInfo->auxdecor< int >(m_sgName+"nPixelBarrelUA")  = prdAssoc->nPixelBarrelUA;
      eventInfo->auxdecor< int >(m_sgName+"nPixelEndCapAUA") = prdAssoc->nPixelEndCapAUA;
      eventInfo->auxdecor< int >(m_sgName+"nPixelEndCapCUA") = prdAssoc->nPixelEndCapCUA;
      eventInfo->auxdecor< int >(m_sgName+"nSCTUA")          = prdAssoc->nSCTUA;
      eventInfo->auxdecor< int >(m_sgName+"nSCTBarrelUA")    = prdAssoc->nSCTBarrelUA;
      eventInfo->auxdecor< int >(m_sgName+"nSCTEndCapAUA")   = prdAssoc->nSCTEndCapAUA;
      eventInfo->auxdecor< int >(m_sgName+"nSCTEndCapCUA")   = prdAssoc->nSCTEndCapCUA;
      eventInfo->auxdecor< int >(m_sgName+"nTRTUA")          = prdAssoc->nTRTUA;
      eventInfo->auxdecor< int >(m_sgName+"nTRTBarrelUA")    = prdAssoc->nTRTBarrelUA;
      eventInfo->auxdecor< int >(m_sgName+"nTRTEndCapAUA")   = prdAssoc->nTRTEndCapAUA;
      eventInfo->auxdecor< int >(m_sgName+"nTRTEndCapCUA")   = prdAssoc->nTRTEndCapCUA;

      ATH_MSG_DEBUG("Decorating unassociated hits with nPixelBarrelUA = ");
      ATH_MSG_DEBUG(prdAssoc->nPixelBarrelUA);

      m_UnassociatedHitsGetterTool->releaseObject(prdAssoc);
    }
    else
      ATH_MSG_WARNING("Could not retrieve Unassociated hits information");
    
    return StatusCode::SUCCESS;
  }  
  
}

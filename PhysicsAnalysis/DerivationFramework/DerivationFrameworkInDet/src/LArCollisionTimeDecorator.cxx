/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LArCollisionTimeDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Olivier Arnaez ( olivier.arnaez@cern.ch )

#include "DerivationFrameworkInDet/LArCollisionTimeDecorator.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "LArRecEvent/LArCollisionTime.h"


#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  LArCollisionTimeDecorator::LArCollisionTimeDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) : 
    AthAlgTool(type,name,parent),  
    m_sgName(""),
    m_containerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("DecorationPrefix",       m_sgName);
    declareProperty("ContainerName",          m_containerName="EventInfo");
  }

  StatusCode LArCollisionTimeDecorator::initialize()
  {

    if (m_sgName=="") {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of LArCollisionTimeDecorator!");
    }
    
    if (m_containerName=="") {
      ATH_MSG_ERROR("No EventInfo collection provided for LArCollisionTimeDecorator!");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode LArCollisionTimeDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode LArCollisionTimeDecorator::addBranches() const
  {
    ATH_MSG_DEBUG("Adding LAr time info to EventInfo");

    const xAOD::EventInfo* eventInfo;
    CHECK( evtStore()->retrieve( eventInfo, m_containerName ) );

    const  LArCollisionTime* tps;
    CHECK( evtStore()->retrieve(tps,"LArCollisionTime") );
    
    if (tps) {
      eventInfo->auxdecor< int >(m_sgName+"ncellA")     = tps->ncellA();
      eventInfo->auxdecor< int >(m_sgName+"ncellC")     = tps->ncellC();
      eventInfo->auxdecor< float >(m_sgName+"energyA")  = tps->energyA();
      eventInfo->auxdecor< float >(m_sgName+"energyC")  = tps->energyC();
      eventInfo->auxdecor< float >(m_sgName+"timeA")    = tps->timeA();
      eventInfo->auxdecor< float >(m_sgName+"timeC")    = tps->timeC();
      float LArECtimeDiff =   tps->timeA()-tps->timeC();
      ATH_MSG_DEBUG("Decorating LAr time info, LArECTimeDiff = " << LArECtimeDiff);
    }
    else
      ATH_MSG_WARNING("Missing LArCollisionTime information.");
    
    return StatusCode::SUCCESS;
  }  
  
}

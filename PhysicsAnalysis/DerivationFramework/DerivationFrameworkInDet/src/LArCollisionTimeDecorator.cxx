/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LArCollisionTimeDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Olivier Arnaez ( olivier.arnaez@cern.ch )

#include "DerivationFrameworkInDet/LArCollisionTimeDecorator.h"
#include "DerivationFrameworkInDet/DecoratorUtils.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "StoreGate/WriteDecorHandle.h"

#include <vector>
#include <utility>
#include <string>

namespace DerivationFramework {

  LArCollisionTimeDecorator::LArCollisionTimeDecorator(const std::string& type,
      const std::string& name,
      const IInterface* parent) :
    AthAlgTool(type,name,parent)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode LArCollisionTimeDecorator::initialize()
  {

    if (m_sgName.empty()) {
      ATH_MSG_WARNING("No decoration prefix name provided for the output of LArCollisionTimeDecorator!");
    }
    
    if (m_eventInfoKey.key().empty()) {
      ATH_MSG_ERROR("No EventInfo collection provided for LArCollisionTimeDecorator!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_eventInfoKey.initialize() );
    ATH_CHECK(m_larCollisionTimeKey.initialize() );
    {
       std::vector<std::string> names;
       names.resize(kNIntDecor);
       names[kncellA]="ncellA";
       names[kncellC]="ncellC";
       createDecoratorKeys(*this,m_eventInfoKey,"" /* common prefix */,names, m_intDecorKeys);
    }

    {
       std::vector<std::string> names;
       names.resize(kNFloatDecor);
       names[kenergyA] = "energyA";
       names[kenergyC] = "energyC";
       names[ktimeA]   = "timeA";
       names[ktimeC]   = "timeC";
       createDecoratorKeys(*this,m_eventInfoKey,"" /* common prefix */,names, m_floatDecorKeys);
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
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey,ctx);
    CHECK( eventInfo.isValid() ? StatusCode::SUCCESS : StatusCode::FAILURE);

    SG::ReadHandle<LArCollisionTime> tps(m_larCollisionTimeKey,ctx);

    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,int> >   int_decor_handles(createDecorators<xAOD::EventInfo,int>(m_intDecorKeys,ctx));
    std::vector<SG::WriteDecorHandle<xAOD::EventInfo,float> > float_decor_handles(createDecorators<xAOD::EventInfo,float>(m_floatDecorKeys,ctx));
    assert( int_decor_handles.size() == kNIntDecor);
    assert( float_decor_handles.size() == kNFloatDecor);
    if (tps.isValid()) {
      int_decor_handles[kncellA](*eventInfo) = tps->ncellA();
      int_decor_handles[kncellC](*eventInfo)  = tps->ncellC();
      float_decor_handles[kenergyA](*eventInfo)  = tps->energyA();
      float_decor_handles[kenergyC](*eventInfo)  = tps->energyC();
      float_decor_handles[ktimeA](*eventInfo)  = tps->timeA();
      float_decor_handles[ktimeC](*eventInfo)  = tps->timeC();
      float LArECtimeDiff =   tps->timeA()-tps->timeC();
      ATH_MSG_DEBUG("Decorating LAr time info, LArECTimeDiff = " << LArECtimeDiff);
    }
    else {
      ATH_MSG_WARNING("Missing LArCollisionTime information.");
    }

    return StatusCode::SUCCESS;
  }  
  
}

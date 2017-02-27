/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Scaler
 *
 * @brief  :
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: ScalerSvc.cxx,v 1.5 2007-03-01 21:29:48 eifert Exp $
 **********************************************************************************/

#include "TrigSteering/ScalerSvc.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

std::map< std::string, HLT::IScalerFactory* > HLT::ScalerSvc::m_factories;


HLT::ScalerSvc::ScalerSvc( const std::string& name, ISvcLocator* sl )
  : AthService ( name, sl ),
    m_toolSvc("ToolSvc", name)
{
  declareProperty( "DefaultType", m_defaultType = "HLT::RandomScaler");
}


StatusCode HLT::ScalerSvc::initialize()
{
  // Create instances of reusable scalers
  if ( m_toolSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Unable to retrieve ToolSvc");
    return StatusCode::FAILURE;
  }

  for ( std::map<std::string, IScalerFactory*>::iterator iterFact = m_factories.begin();
        iterFact != m_factories.end(); ++iterFact) {
    if ( iterFact->second == NULL ) {// is this the factory of a reusable scaler ?
      IReusableScaler* scaler = 0;

      ATH_MSG_DEBUG("Creating reusable scaler of type " << iterFact->first);

      if (m_toolSvc->retrieveTool(iterFact->first, scaler).isFailure()) {
        ATH_MSG_ERROR("Cannot create tool " << iterFact->first);
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode HLT::ScalerSvc::start()
{
  ATH_MSG_DEBUG("Resetting scalers");
  for ( std::vector< IScaler* >::iterator iterScaler = m_scalers.begin();
        iterScaler != m_scalers.end(); ++iterScaler)
    (*iterScaler)->reset();
  
  return StatusCode::SUCCESS;
}

StatusCode HLT::ScalerSvc::finalize()
{
  for ( std::map<std::string, IScalerFactory*>::iterator iterFact = m_factories.begin();
        iterFact != m_factories.end(); ) {
    delete iterFact->second;
    m_factories.erase( iterFact++ );
  }

  for ( std::vector< IScaler* >::iterator iterScaler = m_scalers.begin();
        iterScaler != m_scalers.end(); ++iterScaler)
    delete *iterScaler;

  m_scalers.clear();

  return StatusCode::SUCCESS;
}


bool HLT::ScalerSvc::registerType( IScalerFactory* factory, const std::string& type)
{
  bool retVal = true;

  std::map<std::string, IScalerFactory*>::iterator findFact
    = m_factories.find(type);

  if ( findFact != m_factories.end() ) {
    // This is a static method. No access to msg() member.
    MsgStream log(Athena::getMessageSvc(), "HLT::ScalerSvc");
    log << MSG::INFO << "A factory was already registered for type"
        << type << ", replacing with this one." << endmsg;

    delete findFact->second;
    retVal = false;
  }

  m_factories[type] = factory;
  return retVal;
}


HLT::IScaler* HLT::ScalerSvc::get(const std::string& type) const
{
  IScaler* scaler = 0;

  std::string theType = ( type.empty() ? m_defaultType : type );
  ATH_MSG_DEBUG("Retrieving scaler of type " << theType);

  std::map<std::string, IScalerFactory*>::iterator findFact
    = m_factories.find( theType );

  if ( findFact == m_factories.end() ) {
    ATH_MSG_ERROR("No factory registered for type " << theType);
    return scaler;
  }

  if ( findFact->second ) {
    ATH_MSG_DEBUG("Scaler is non-reusable, creating a new instance.");
    scaler = findFact->second->create();
    m_scalers.push_back( scaler );
  }
  else {
    ATH_MSG_DEBUG("Scaler is reusable, returning a pointer to the instance.");
    IReusableScaler* rScaler = 0;
    StatusCode sc = m_toolSvc->retrieveTool(theType, rScaler);

    if ( !sc.isSuccess() ||  !rScaler ) {
      ATH_MSG_ERROR("Could not create scaler of type " << theType);
      return 0;
    }

    scaler = static_cast<IScaler*>(rScaler);
  }

  return scaler;
}


StatusCode HLT::ScalerSvc::queryInterface( const InterfaceID& riid, void** ppvIF )
{
  if ( riid == HLT::IScalerSvc::interfaceID() )  {
    *ppvIF = (IScalerSvc*) this;
  } else if ( riid == IProperty::interfaceID() ) {
    *ppvIF = (IProperty*) this;
  } else  {
    return Service::queryInterface( riid, ppvIF );
  }
  addRef();
  return StatusCode::SUCCESS;
}

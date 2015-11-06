/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004EventAction.h"

#include "GaudiKernel/GaudiException.h"
//#include "ParticleGenerator/ParticleManager.h"
#include "LArG4RunControl/LArGeoTB2004Options.h"
#include "CxxUtils/make_unique.h" // For make unique

#include <iomanip>

int LArGeoH62004EventAction::m_evnum=0;

LArGeoH62004EventAction::LArGeoH62004EventAction(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent)
  , m_ev("TBEventInfo")
{
  ATH_MSG_DEBUG ("LArGeoH62004EventAction::LArGeoH62004EventAction constructor");
}

StatusCode LArGeoH62004EventAction::initialize(){
  
  StatusCode status = detStore()->retrieve(m_largeoTB2004Options, "LArGeoTB2004Options");
  if(status.isFailure()) {
    ATH_MSG_WARNING ( "\tCan't access LArGeoTB2004Options, using default values" );
    m_largeoTB2004Options = 0;
  }
  
  return StatusCode::SUCCESS;
}

void LArGeoH62004EventAction::EndOfEvent(const G4Event * /*theEvent*/)
{ 

  ATH_MSG_DEBUG ("LArGeoH62004EventAction::EndOfEventAction");
  StatusCode status;

  //float beamm = PG::ParticleManager::GetInstance()->getEnergy(0);
  float beamm = 0.;
  //float ypos = PG::KinematicManager::GetManager()->getValue(PG::kTargetY);
  float ypos,xpos;
  if(m_largeoTB2004Options) {
    ypos = m_largeoTB2004Options->TableYPosition();
    xpos = m_largeoTB2004Options->CryoXPosition();
  } else {
    xpos = ypos = 0.;
  }
  //int pdg = PG::ParticleManager::GetInstance()->getPDG(0);
  int pdg = 0;
  int evtype = 1; // Physics !!!!
  
  ATH_MSG_DEBUG ( "TBEventInfo: "<<m_evnum<<"/"<<pdg<<"/"<<evtype<<"/"<<0<<"/"<<beamm<<"/"<<""<<"/"<<xpos<<"/"<<0<<"/"<<ypos);

  m_ev = CxxUtils::make_unique<TBEventInfo>(++m_evnum,pdg,evtype,0,beamm,"",xpos,0,ypos);
  return;
}


StatusCode LArGeoH62004EventAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

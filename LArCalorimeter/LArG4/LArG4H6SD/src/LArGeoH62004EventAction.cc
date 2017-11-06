/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004EventAction.h"

#include "GaudiKernel/GaudiException.h"
//#include "ParticleGenerator/ParticleManager.h"
#include "LArG4RunControl/LArGeoTB2004Options.h"
#include "CxxUtils/make_unique.h" // For make unique

#include <iomanip>

namespace G4UA
{
  int LArGeoH62004EventAction::m_evnum=0;

  LArGeoH62004EventAction::LArGeoH62004EventAction(const Config& config)
    : m_ev("TBEventInfo")
    , m_xpos(config.cryoXposition)
    , m_ypos(config.yTable)
  {
    //ATH_MSG_DEBUG ("LArGeoH62004EventAction::LArGeoH62004EventAction constructor");
  }

  void LArGeoH62004EventAction::EndOfEventAction(const G4Event * /*theEvent*/)
  {

    //ATH_MSG_DEBUG ("LArGeoH62004EventAction::EndOfEventAction");

    //float beamm = PG::ParticleManager::GetInstance()->getEnergy(0);
    float beamm = 0.;
    //float ypos = PG::KinematicManager::GetManager()->getValue(PG::kTargetY);
    //int pdg = PG::ParticleManager::GetInstance()->getPDG(0);
    int pdg = 0;
    int evtype = 1; // Physics !!!!

    //ATH_MSG_DEBUG ( "TBEventInfo: "<<m_evnum<<"/"<<pdg<<"/"<<evtype<<"/"<<0<<"/"<<beamm<<"/"<<""<<"/"<<m_xpos<<"/"<<0<<"/"<<m_ypos);

    m_ev = CxxUtils::make_unique<TBEventInfo>(++m_evnum,pdg,evtype,0,beamm,"",m_xpos,0,m_ypos);
    return;
  }

} // namespace G4UA

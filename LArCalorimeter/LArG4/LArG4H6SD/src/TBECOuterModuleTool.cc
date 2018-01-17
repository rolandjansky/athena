/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBECOuterModuleTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "CxxUtils/make_unique.h"

TBECOuterModuleTool::TBECOuterModuleTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl_gapadj("LArHitEMEC_gapadj")
  , m_HitColl_gapold("LArHitEMEC_gapold")
  , m_HitColl_gap_e("LArHitEMEC_gap_e")
  , m_HitColl_gap_s("LArHitEMEC_gap_s")
  , m_HitColl_gap_se("LArHitEMEC_gap_se")
  , m_HitColl_chcoll("LArHitEMEC_chcoll")
  , m_HitColl_ropt("LArHitEMEC_ropt")
  , m_emecoutergadjcalc("EMECPosOuterWheel_ECOR_GADJCalculator", name)
  , m_emecoutergadjoldcalc("EMECPosOuterWheel_ECOR_GADJ_OLDCalculator", name)
  , m_emecoutergadjecalc("EMECPosOuterWheel_ECOR_GADJ_ECalculator", name)
  , m_emecoutergadjscalc("EMECPosOuterWheel_ECOR_GADJ_SCalculator", name)
  , m_emecoutergadjsecalc("EMECPosOuterWheel_ECOR_GADJ_SECalculator", name)
  , m_emecouterchclcalc("EMECPosOuterWheel_ECOR_CHCLCalculator", name)
  , m_emecoutercalc("EMECPosOuterWheelCalculator", name)
  , m_gapadjSD(nullptr)
  , m_gapoldSD(nullptr)
  , m_gap_eSD(nullptr)
  , m_gap_sSD(nullptr)
  , m_gap_seSD(nullptr)
  , m_chcollSD(nullptr)
  , m_roptSD(nullptr)
{
  declareProperty("EMECPosOuterWheel_ECOR_GADJCalculator", m_emecoutergadjcalc);
  declareProperty("EMECPosOuterWheel_ECOR_GADJ_OLDCalculator", m_emecoutergadjoldcalc);
  declareProperty("EMECPosOuterWheel_ECOR_GADJ_ECalculator", m_emecoutergadjecalc);
  declareProperty("EMECPosOuterWheel_ECOR_GADJ_SCalculator", m_emecoutergadjscalc);
  declareProperty("EMECPosOuterWheel_ECOR_GADJ_SECalculator", m_emecoutergadjsecalc);
  declareProperty("EMECPosOuterWheel_ECOR_CHCLCalculator", m_emecouterchclcalc);
  declareProperty("EMECPosOuterWheelCalculator", m_emecoutercalc);
}

StatusCode TBECOuterModuleTool::initializeCalculators()
{
  ATH_CHECK(m_emecoutergadjcalc.retrieve());
  ATH_CHECK(m_emecoutergadjoldcalc.retrieve());
  ATH_CHECK(m_emecoutergadjecalc.retrieve());
  ATH_CHECK(m_emecoutergadjscalc.retrieve());
  ATH_CHECK(m_emecoutergadjsecalc.retrieve());
  ATH_CHECK(m_emecouterchclcalc.retrieve());
  ATH_CHECK(m_emecoutercalc.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TBECOuterModuleTool::initializeSD()
{
  // Multi-threading not yet supported
  if(m_gapadjSD) {
    ATH_MSG_ERROR("TBECOuterModuleTool::initializeSD - SDs already exist. " <<
                  "Are you running an MT job? Not yet supported!");
    return StatusCode::FAILURE;
  }

  m_gapadjSD = new LArG4SimpleSD( "LAr::EMEC::OuterModule::SDout_gapadj" , &*m_emecoutergadjcalc, m_timeBinType , m_timeBinWidth );
  m_gapoldSD = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_gapold", &*m_emecoutergadjoldcalc, m_timeBinType , m_timeBinWidth);
  m_gap_eSD  = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_gap_e", &*m_emecoutergadjecalc, m_timeBinType , m_timeBinWidth);
  m_gap_sSD  = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_gap_s", &*m_emecoutergadjscalc, m_timeBinType , m_timeBinWidth);
  m_gap_seSD = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_gap_se", &*m_emecoutergadjsecalc, m_timeBinType , m_timeBinWidth);
  m_chcollSD = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_chcoll", &*m_emecouterchclcalc, m_timeBinType , m_timeBinWidth);
  m_roptSD   = new LArG4SimpleSD("LAr::EMEC::OuterModule::SDout_ropt", &*m_emecoutercalc, m_timeBinType , m_timeBinWidth);

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_gapadjSD] = &m_volumeNames;
  configuration[m_gapoldSD] = &m_volumeNames;
  configuration[m_gap_eSD]  = &m_volumeNames;
  configuration[m_gap_sSD]  = &m_volumeNames;
  configuration[m_gap_seSD] = &m_volumeNames;
  configuration[m_chcollSD] = &m_volumeNames;
  configuration[m_roptSD]   = &m_volumeNames;
  setupAllSDs(configuration);
    
  // Make sure the helpers are all set up
  setupHelpers(m_gapadjSD);
  setupHelpers(m_gapoldSD);
  setupHelpers(m_gap_eSD);
  setupHelpers(m_gap_sSD);
  setupHelpers(m_gap_seSD);
  setupHelpers(m_chcollSD);
  setupHelpers(m_roptSD);

  return StatusCode::SUCCESS;
}

StatusCode TBECOuterModuleTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl_gapadj.isValid()) m_HitColl_gapadj = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_gapadj");
  m_gapadjSD->EndOfAthenaEvent( &*m_HitColl_gapadj );
  if (!m_HitColl_gapold.isValid()) m_HitColl_gapold = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_gapold");
  m_gapoldSD->EndOfAthenaEvent( &*m_HitColl_gapold );
  if (!m_HitColl_gap_e.isValid()) m_HitColl_gap_e = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_gap_e");
  m_gap_eSD->EndOfAthenaEvent( &*m_HitColl_gap_e );
  if (!m_HitColl_gap_s.isValid()) m_HitColl_gap_s = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_gap_s");
  m_gap_sSD->EndOfAthenaEvent( &*m_HitColl_gap_s );
  if (!m_HitColl_gap_se.isValid()) m_HitColl_gap_se = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_gap_se");
  m_gap_seSD->EndOfAthenaEvent( &*m_HitColl_gap_se );
  if (!m_HitColl_chcoll.isValid()) m_HitColl_chcoll = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_chcoll");
  m_chcollSD->EndOfAthenaEvent( &*m_HitColl_chcoll );
  if (!m_HitColl_ropt.isValid()) m_HitColl_ropt = CxxUtils::make_unique<LArHitContainer>("LArHitEMEC_ropt");
  m_roptSD->EndOfAthenaEvent( &*m_HitColl_ropt );
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H62004MultSD.h"
#include "LArG4H6SD/LArG4H62004CalibSD.h"
#include "LArG4H6SD/LArG4H62004SD.h"
#include "LArG4H6SD/LArG4H6WarmTCSD.h"
#include "LArG4H6SD/LArG4H6BeamSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#undef DEBUG_ME

static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> innerWheelMultSD("EMECInnerModuleMult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> backInnerBarretteMultSD("EMECBackInnerBarretteMult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> fcalMod1MultSD("FCALModule1Mult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> fcalMod2MultSD("FCALModule2Mult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> fcalMod3MultSD("FCALColdTCMult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> hecSliceMultSD("HECMult::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> WarmTCMultXSD("WarmTCMult::X::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> WarmTCMultYSD("WarmTCMult::Y::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> WarmTCMultAbsSD("WarmTCMult::Abs::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> FrontSD("FrontMult::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004MultSD> MoveSD("MoveMult::H6");

LArG4H62004MultSD::LArG4H62004MultSD(G4String a_name)
    : LArG4MultSD(a_name)
{


  LArG4H62004SD* larSD = 0;
  LArG4H62004CalibSD* larCalibSD = 0;
  LArG4H6WarmTCSD* warmtcSD = 0;
  LArG4H6BeamSD* frontSD = 0;
  LArG4H6BeamSD* moveSD = 0;

  if(a_name=="EMECInnerModuleMult::H6")
  {
    larSD = new LArG4H62004SD("EMEC::InnerModule::H6");
    larCalibSD = new LArG4H62004CalibSD("EMEC::InnerModule::Calibration::H6");
  }
  else if(a_name=="EMECBackInnerBarretteMult::H6")
  {
    larSD = new LArG4H62004SD("EMEC::BackInnerBarrette::H6");
    larCalibSD = new LArG4H62004CalibSD("EMEC::BackInnerBarrette::Calibration::H6");
  }
  else if(a_name=="FCALModule1Mult::H6")
  {
    larSD = new LArG4H62004SD("FCAL::Module1::Gap::H6");
    larCalibSD = new LArG4H62004CalibSD("LAr::FCAL::Module1::Gap::Calibration::H6");
  }
  else if(a_name=="FCALModule2Mult::H6")
  {
    larSD = new LArG4H62004SD("FCAL::Module2::Gap::H6");
    larCalibSD = new LArG4H62004CalibSD("LAr::FCAL::Module2::Gap::Calibration::H6");
  }
  else if(a_name=="FCALColdTCMult::H6")
  {
    larSD = new LArG4H62004SD("LAr::FCAL::ColdTC::H6");
    larCalibSD = new LArG4H62004CalibSD("LAr::FCAL::ColdTC::Gap::Calibration::H6");
  }
  else if(a_name=="HECMult::H6")
  {
    larSD = new LArG4H62004SD("HEC::Module::Depth::Slice::Local::H6");
    larCalibSD = new LArG4H62004CalibSD("HEC::Module::Depth::Slice::Local::Calibration::H6");
  }
  else if(a_name=="WarmTCMult::X::H6")
  {
    warmtcSD = new LArG4H6WarmTCSD("LArG4H6WarmTC::X");
    larCalibSD = new LArG4H62004CalibSD("LArDead::Dead::5::H6");
  }
  else if(a_name=="WarmTCMult::Y::H6")
  {
    warmtcSD = new LArG4H6WarmTCSD("LArG4H6WarmTC::Y");
    larCalibSD = new LArG4H62004CalibSD("LArDead::Dead::4::H6");
  }
  else if(a_name=="WarmTCMult::Abs::H6")
  {
    warmtcSD = new LArG4H6WarmTCSD("LArG4H6WarmTC::Abs");
    larCalibSD = new LArG4H62004CalibSD("LArDead::Dead::3::H6");
  }
  else if(a_name=="FrontMult::H6")
  {
    frontSD = new LArG4H6BeamSD("LArG4H6BeamSD::Front");
    larCalibSD = new LArG4H62004CalibSD("LArDead::Dead::2::H6");
  }
  else if(a_name=="MoveMult::H6")
  {
    moveSD = new LArG4H6BeamSD("LArG4H6BeamSD::Movable");
    larCalibSD = new LArG4H62004CalibSD("LArDead::Dead::1::H6");
  }

  if(larSD) {
    larSD->Register();
    addSD(larSD);
  }
  if(warmtcSD){
    warmtcSD->Register();
    addSD(warmtcSD);
  }

  if(frontSD){
    frontSD->Register();
    addSD(frontSD);
  }

  if(moveSD){
    moveSD->Register();
    addSD(moveSD);
  }

  if(larCalibSD) {
    larCalibSD->Register();
    addSD(larCalibSD);
  }

}


LArG4H62004MultSD::~LArG4H62004MultSD()
{
}

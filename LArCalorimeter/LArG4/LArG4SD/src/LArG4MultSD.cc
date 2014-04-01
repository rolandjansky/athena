/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4SD/LArG4MultSD.h"
#include "LArG4SD/LArG4CalibSD.h"
#include "LArG4SD/LArG4SD.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

static FADS::SensitiveDetectorEntryT<LArG4MultSD> stacMultSD("STACMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> presampModuleMultSD("BarrelPresamplerMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> posInnerWheelMultSD("EMECPosInnerWheelMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> posOuterWheelMultSD("EMECPosOuterWheelMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> negInnerWheelMultSD("EMECNegInnerWheelMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> negOuterWheelMultSD("EMECNegOuterWheelMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> backOuterBarretteMultSD("EMECBackOuterBarretteMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> presampLArMultSD("EndcapPresamplerMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> fcalMod1MultSD("FCALModule1Mult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> fcalMod2MultSD("FCALModule2Mult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> fcalMod3MultSD("FCALModule3Mult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> minifcalMultSD("MiniFCALMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> hecSliceMultSD("HECMult");
static FADS::SensitiveDetectorEntryT<LArG4MultSD> hecSliceWheelMultSD("HECWheelMult");


LArG4MultSD::LArG4MultSD(G4String a_name)
  : FadsSensitiveDetector(a_name),
    m_detectorName(a_name)
{

  // Get pointer to the message service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  if(status.isFailure())
    m_msgSvc = 0;

  if(m_msgSvc)
    {
      MsgStream log(m_msgSvc, "LArG4MultSD");
      log << MSG::DEBUG << "Constructing multiple SD " << a_name << endreq;
    }

  LArG4SD* larSD = 0;
  LArG4CalibSD* larCalibSD = 0;

  if(m_detectorName=="STACMult")
    {
      larSD = new LArG4SD("EMB::STAC");
      larCalibSD = new LArG4CalibSD("EMB::STAC::Calibration");
    }
  else if(m_detectorName=="BarrelPresamplerMult")
    {
      larSD = new LArG4SD("Barrel::Presampler::Module");
      larCalibSD = new LArG4CalibSD("Barrel::Presampler::Module::Calibration");
    }

  else if(m_detectorName=="EMECPosInnerWheelMult")
    {
      larSD = new LArG4SD("EMEC::Pos::InnerWheel");
      larCalibSD = new LArG4CalibSD("EMEC::Pos::InnerWheel::Calibration");
    }
  else if(m_detectorName=="EMECPosOuterWheelMult")
    {
      larSD = new LArG4SD("EMEC::Pos::OuterWheel");
      larCalibSD = new LArG4CalibSD("EMEC::Pos::OuterWheel::Calibration");
    }
  else if(m_detectorName=="EMECNegInnerWheelMult")
    {
      larSD = new LArG4SD("EMEC::Neg::InnerWheel");
      larCalibSD = new LArG4CalibSD("EMEC::Neg::InnerWheel::Calibration");
    }
  else if(m_detectorName=="EMECNegOuterWheelMult")
    {
      larSD = new LArG4SD("EMEC::Neg::OuterWheel");
      larCalibSD = new LArG4CalibSD("EMEC::Neg::OuterWheel::Calibration");
    }

  else if(m_detectorName=="EMECBackOuterBarretteMult")
    {
      larSD = new LArG4SD("EMEC::BackOuterBarrette");
      larCalibSD = new LArG4CalibSD("EMEC::BackOuterBarrette::Calibration");
    }

  else if(m_detectorName=="EndcapPresamplerMult")
    {
      larSD = new LArG4SD("Endcap::Presampler::LiquidArgon");
      larCalibSD = new LArG4CalibSD("Endcap::Presampler::LiquidArgon::Calibration");
    }
  else if(m_detectorName=="FCALModule1Mult")
    {
      larSD = new LArG4SD("FCAL::Module1::Gap");
      larCalibSD = new LArG4CalibSD("FCAL::Module1::Gap::Calibration");
    }
  else if(m_detectorName=="FCALModule2Mult")
    {
      larSD = new LArG4SD("FCAL::Module2::Gap");
      larCalibSD = new LArG4CalibSD("FCAL::Module2::Gap::Calibration");
    }
  else if(m_detectorName=="FCALModule3Mult")
    {
      larSD = new LArG4SD("FCAL::Module3::Gap");
      larCalibSD = new LArG4CalibSD("FCAL::Module3::Gap::Calibration");
    }
  else if(m_detectorName=="HECMult")
    {
      larSD = new LArG4SD("HEC::Module::Depth::Slice::Local");
      larCalibSD = new LArG4CalibSD("HEC::Module::Depth::Slice::Local::Calibration");
    }
  else if(m_detectorName=="HECWheelMult")
    {
      larSD = new LArG4SD("HEC::Module::Depth::Slice::Wheel");
      larCalibSD = new LArG4CalibSD("HEC::Module::Depth::Slice::Wheel::Calibration");
    }
  else if(m_detectorName=="MiniFCALMult")
    {
      larSD = new LArG4SD("MiniFCAL::Wafer");
      larCalibSD = new LArG4CalibSD("MiniFCAL::Wafer::Calibration");
    }

  if(larSD)
    {
      larSD->Register();
      addSD(larSD);
    }

  if(larCalibSD)
    {
      larCalibSD->Register();
      addSD(larCalibSD);
    }
}


LArG4MultSD::~LArG4MultSD()
{
}


void LArG4MultSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
}


G4bool LArG4MultSD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  G4bool result = true;
  for(unsigned int i=0; i!=m_sdList.size(); i++)
    result &= m_sdList[i]->Hit(a_step);

  return result;
}


void LArG4MultSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{    
}

void LArG4MultSD::addSD(FadsSensitiveDetector* sd)
{
  m_sdList.push_back(sd);

  if(m_msgSvc)
    {
      MsgStream log(m_msgSvc, "LArG4MultSD");
      log << MSG::DEBUG << "Added SD " << sd->GetName() << endreq;
    }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H6EmecMultSD.h"
#include "LArG4H6SD/LArG4H6EmecSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

using namespace FADS;

static SensitiveDetectorEntryT<LArG4H6EmecMultSD> innerWheelMultSD("TBECInnerModuleMult");
static SensitiveDetectorEntryT<LArG4H6EmecMultSD> outerWheelMultSD("TBECOuterModuleMult");
static SensitiveDetectorEntryT<LArG4H6EmecMultSD> backOuterBarretteMultSD("TBECBackOuterBarretteMult");

LArG4H6EmecMultSD::LArG4H6EmecMultSD(G4String a_name)
  : LArG4MultSD(a_name)
{


  //std::cout<<"===> LArG4H6EmecMultSD is called, a_name="<<a_name<<std::endl;



  LArG4H6EmecSD* larSD = 0;

  if(a_name=="TBECInnerModuleMult")
  {
    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_gapadj");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_gapold");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_gap_e");
    larSD->Register();
    addSD(larSD);
 
    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_gap_s");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_gap_se");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_chcoll");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::InnerModule::SDin_ropt");
    larSD->Register();
    addSD(larSD);
  }
  else if(a_name=="TBECOuterModuleMult")
  {
    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_gapadj");
    larSD->Register();
    addSD(larSD);
 
    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_gapold");
    larSD->Register();
    addSD(larSD);


    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_gap_e");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_gap_s");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_gap_se");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_chcoll");
    larSD->Register();
    addSD(larSD);
 
    larSD = new LArG4H6EmecSD("LAr::EMEC::OuterModule::SDout_ropt");
    larSD->Register();
    addSD(larSD);
  }
  else if(a_name=="TBECBackOuterBarretteMult")
  {
    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gapadj");
    larSD->Register();
    addSD(larSD);
 
    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gapold");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_e");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_s");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_se");
    larSD->Register();
    addSD(larSD);

    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_chcoll");
    larSD->Register();
    addSD(larSD);
 
    larSD = new LArG4H6EmecSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_ropt");
    larSD->Register();
    addSD(larSD);
  }
}

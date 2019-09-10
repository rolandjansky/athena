/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETComponentCopier.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHOR: Gabriel Gallardo
DATE:   May 22, 2019

PURPOSE:  Helper class to copy information from the MET helper to a TrigMissingET object

 ********************************************************************/

#include "EFMissingETComponentCopier.h"

void EFMissingETComponentCopier::addHelperCompToMET(unsigned char helper_i)
{
  TrigEFMissingEtComponent* metComp = m_helper->GetComponent(helper_i);

  float ex =            metComp->m_ex;
  float ey =            metComp->m_ey;
  float ez =            metComp->m_ez;
  float sumE =          metComp->m_sumE;
  float sumEt =         metComp->m_sumEt;
  float c0 =            metComp->m_calib0;
  float c1 =            metComp->m_calib1;
  short sumOfSigns =    metComp->m_sumOfSigns;

  m_met->setEx(       m_met->ex() + sumOfSigns * c0 + c1 * ex );
  m_met->setEy(       m_met->ey() + sumOfSigns * c0 + c1 * ey );
  m_met->setEz(       m_met->ez() + sumOfSigns * c0 + c1 * ez );
  m_met->setSumE(   m_met->sumE() + sumOfSigns * c0 + c1 * sumE );
  m_met->setSumEt( m_met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
  return;
}

void EFMissingETComponentCopier::setMETCompFromHelper(unsigned char met_i, unsigned char helper_i)
{
  TrigEFMissingEtComponent* metComp = m_helper->GetComponent(helper_i);

  m_met->setNameOfComponent(       met_i, metComp->m_name           );
  m_met->setExComponent(           met_i, metComp->m_ex             );
  m_met->setEyComponent(           met_i, metComp->m_ey             );
  m_met->setEzComponent(           met_i, metComp->m_ez             );
  m_met->setSumEtComponent(        met_i, metComp->m_sumEt          );
  m_met->setSumEComponent(         met_i, metComp->m_sumE           );
  m_met->setCalib0Component(       met_i, metComp->m_calib0         );
  m_met->setCalib1Component(       met_i, metComp->m_calib1         );
  m_met->setStatusComponent(       met_i, metComp->m_status         );
  m_met->setSumOfSignsComponent(   met_i, metComp->m_sumOfSigns     );
  m_met->setUsedChannelsComponent( met_i, metComp->m_usedChannels   );
  return;
}

void EFMissingETComponentCopier::addMETCompWithHelper(unsigned char met_i, unsigned char met_add_i, unsigned char helper_i)
{
  TrigEFMissingEtComponent* metComp = m_helper->GetComponent(helper_i);

  m_met->setExComponent(              met_i, metComp->m_ex           + m_met->exComponent(           met_add_i ) );
  m_met->setEyComponent(              met_i, metComp->m_ey           + m_met->eyComponent(           met_add_i ) );
  m_met->setEzComponent(              met_i, metComp->m_ez           + m_met->ezComponent(           met_add_i ) );
  m_met->setSumEtComponent(           met_i, metComp->m_sumEt        + m_met->sumEtComponent(        met_add_i ) );
  m_met->setSumEComponent(            met_i, metComp->m_sumE         + m_met->sumEComponent(         met_add_i ) );
  m_met->setSumOfSignsComponent(      met_i, metComp->m_sumOfSigns   + m_met->sumOfSignsComponent(   met_add_i ) );
  m_met->setUsedChannelsComponent(    met_i, metComp->m_usedChannels + m_met->usedChannelsComponent( met_add_i ) );
  m_met->setStatusComponent(          met_i, metComp->m_status       | m_met->statusComponent(       met_add_i ) );
  m_met->setCalib0Component(          met_i, metComp->m_calib0 );
  m_met->setCalib1Component(          met_i, metComp->m_calib1 );
  return;
}

void EFMissingETComponentCopier::addMETCompWithHelper(unsigned char met_i, unsigned char helper_i)
{
  TrigEFMissingEtComponent* metComp = m_helper->GetComponent(helper_i);

  m_met->setExComponent(              met_i, metComp->m_ex           );
  m_met->setEyComponent(              met_i, metComp->m_ey           );
  m_met->setEzComponent(              met_i, metComp->m_ez           );
  m_met->setSumEtComponent(           met_i, metComp->m_sumEt        );
  m_met->setSumEComponent(            met_i, metComp->m_sumE         );
  m_met->setSumOfSignsComponent(      met_i, metComp->m_sumOfSigns   );
  m_met->setUsedChannelsComponent(    met_i, metComp->m_usedChannels );
  m_met->setStatusComponent(          met_i, metComp->m_status       );
  m_met->setCalib0Component(          met_i, metComp->m_calib0 );
  m_met->setCalib1Component(          met_i, metComp->m_calib1 );
  return;
}

void EFMissingETComponentCopier::setHelperFromMET()
{
  m_helper->SetEx(     m_met->ex()     );
  m_helper->SetEy(     m_met->ey()     );
  m_helper->SetEz(     m_met->ez()     );
  m_helper->SetSumEt(  m_met->sumEt( ) );
  m_helper->SetSumE(   m_met->sumE()   );
  return;
}
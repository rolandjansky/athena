/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromHelperMT.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHOR: Gabriel Gallardo
DATE:   Mar 8, 2019

Based on: EFMissingETFromHelperMT.cxx
AUTHOR:   Diego Casadei
CREATED:  March 12, 2008

PURPOSE:  Updates TrigMissingET using TrigEFMissingEtHelper info for AthenaMT.

Components are left uncorrected when saved into
TrigMissingET as auxiliary information.

 ********************************************************************/

#include "EFMissingETFromHelperMT.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CxxUtils/StrFormat.h"

#include <string>

using CxxUtils::strformat;

void EFMissingETFromHelperMT::ComponentCopier::addHelperCompToMET(unsigned char helper_i)
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

void EFMissingETFromHelperMT::ComponentCopier::setMETCompFromHelper(unsigned char met_i, unsigned char helper_i)
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

void EFMissingETFromHelperMT::ComponentCopier::addMETCompWithHelper(unsigned char met_i, unsigned char met_add_i, unsigned char helper_i)
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

void EFMissingETFromHelperMT::ComponentCopier::addMETCompWithHelper(unsigned char met_i, unsigned char helper_i)
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

void EFMissingETFromHelperMT::ComponentCopier::setHelperFromMET()
{
  m_helper->SetEx(     m_met->ex()     );
  m_helper->SetEy(     m_met->ey()     );
  m_helper->SetEz(     m_met->ez()     );
  m_helper->SetSumEt(  m_met->sumEt( ) );
  m_helper->SetSumE(   m_met->sumE()   );
  return;
}
// ----------------------- //

EFMissingETFromHelperMT::EFMissingETFromHelperMT(const std::string& type,
    const std::string& name,
    const IInterface* parent)
  : base_class( type, name, parent ) 
{

}

StatusCode EFMissingETFromHelperMT::update(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper, const EventContext& /*ctx*/) const
{
    
  ATH_MSG_DEBUG( "EFMissingETFromHelperMT::update() called" );

  if (met==0 || metHelper==0) {
    ATH_MSG_ERROR( "ERROR: null pointers as input!" );
    return StatusCode::FAILURE;
  }

  ComponentCopier copier = ComponentCopier(met, metHelper);

  ATH_MSG_DEBUG( "Found this info in the helper class: " << metHelper->getFormattedValues() );

  met->setFlag( metHelper->GetStatus() );

  uint nMetComp = met->getNumberOfComponents(); // final no. of aux. compon.
  uint nHelperComp = metHelper->GetElements(); // no. of transient aux. compon.
  if (nHelperComp != static_cast<unsigned char>(TrigEFMissingEtComponent::ComponentSize)) {
    ATH_MSG_WARNING( "Found " << nHelperComp << " aux components in the transient helper class.  Not supported!" );
  } else ATH_MSG_DEBUG( "Found " << nHelperComp << " aux components in the transient helper class" );


  // Initialize EDM by setting all components to zero
  met->setEx(0.); met->setEy(0.); met->setEz(0.);
  met->setSumE(0.); met->setSumEt(0.);

  // take info from *metHelper and update *met
  for (uint helper_i=0; helper_i<nHelperComp; ++helper_i){
    switch(static_cast<TrigEFMissingEtComponent::Component>(helper_i))
    {
      case TrigEFMissingEtComponent::PreSamplB:
      case TrigEFMissingEtComponent::EMB1: case TrigEFMissingEtComponent::EMB2: case TrigEFMissingEtComponent::EMB3:

      case TrigEFMissingEtComponent::PreSamplE:
      case TrigEFMissingEtComponent::EME1: case TrigEFMissingEtComponent::EME2: case TrigEFMissingEtComponent::EME3: 

      case TrigEFMissingEtComponent::HEC0: case TrigEFMissingEtComponent::HEC1:
      case TrigEFMissingEtComponent::HEC2: case TrigEFMissingEtComponent::HEC3:

      case TrigEFMissingEtComponent::TileBar0: case TrigEFMissingEtComponent::TileBar1: case TrigEFMissingEtComponent::TileBar2: 
      case TrigEFMissingEtComponent::TileGap1: case TrigEFMissingEtComponent::TileGap2: case TrigEFMissingEtComponent::TileGap3:
      case TrigEFMissingEtComponent::TileExt0: case TrigEFMissingEtComponent::TileExt1: case TrigEFMissingEtComponent::TileExt2:

      case TrigEFMissingEtComponent::FCalEM:
      case TrigEFMissingEtComponent::FCalHad1: case TrigEFMissingEtComponent::FCalHad2:
        copier.addHelperCompToMET(helper_i);
        break;
      default:
        break;
    }
  }

  switch (nMetComp) {
    case 9:
      ATH_MSG_DEBUG("Save summed HAD MET");
      copier.addHelperCompToMET(TrigEFMissingEtComponent::TCLCW);

      copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::TCLCWB1);
      copier.setMETCompFromHelper(1, TrigEFMissingEtComponent::TCLCWB2);
      copier.setMETCompFromHelper(2, TrigEFMissingEtComponent::TCLCWE1);
      copier.setMETCompFromHelper(3, TrigEFMissingEtComponent::TCLCWE2);
      copier.setMETCompFromHelper(4, TrigEFMissingEtComponent::TCEMB1);
      copier.setMETCompFromHelper(5, TrigEFMissingEtComponent::TCEMB2);
      copier.setMETCompFromHelper(6, TrigEFMissingEtComponent::TCEME1);
      copier.setMETCompFromHelper(7, TrigEFMissingEtComponent::TCEME2);
      copier.setMETCompFromHelper(8, TrigEFMissingEtComponent::Muons);
      break;

    case 6:
      ATH_MSG_DEBUG("Save Jets+Mu, with jet components");
      copier.addHelperCompToMET(TrigEFMissingEtComponent::JET);

      copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::JET);
      copier.setMETCompFromHelper(1, TrigEFMissingEtComponent::JETB1);
      copier.setMETCompFromHelper(2, TrigEFMissingEtComponent::JETB2);
      copier.setMETCompFromHelper(3, TrigEFMissingEtComponent::JETE1);
      copier.setMETCompFromHelper(4, TrigEFMissingEtComponent::JETE2);
      copier.setMETCompFromHelper(5, TrigEFMissingEtComponent::Muons);
      break;

    case 2:
      ATH_MSG_DEBUG("Save Jets+Mu, no jet components");
      copier.addHelperCompToMET(TrigEFMissingEtComponent::JET);

      copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::JET);
      copier.setMETCompFromHelper(1, TrigEFMissingEtComponent::Muons);
      break;

    case 5:
      copier.addMETCompWithHelper(0, TrigEFMissingEtComponent::PreSamplB);
      copier.addMETCompWithHelper(0, TrigEFMissingEtComponent::EMB1);
      copier.addMETCompWithHelper(0, TrigEFMissingEtComponent::EMB2);
      copier.addMETCompWithHelper(0, TrigEFMissingEtComponent::EMB3);

      copier.addMETCompWithHelper(1, TrigEFMissingEtComponent::PreSamplE);
      copier.addMETCompWithHelper(1, TrigEFMissingEtComponent::EME1);
      copier.addMETCompWithHelper(1, TrigEFMissingEtComponent::EME2);
      copier.addMETCompWithHelper(1, TrigEFMissingEtComponent::EME3);
      copier.addMETCompWithHelper(1, TrigEFMissingEtComponent::FCalEM);

      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileBar0);
      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileBar1);
      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileBar2);
      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileExt0);
      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileExt1);
      copier.addMETCompWithHelper(2, TrigEFMissingEtComponent::TileExt2);

      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::HEC0);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::HEC1);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::HEC2);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::HEC3);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::TileGap1);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::TileGap2);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::TileGap3);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::FCalHad1);
      copier.addMETCompWithHelper(3, TrigEFMissingEtComponent::FCalHad2);

      copier.addMETCompWithHelper(4, TrigEFMissingEtComponent::Muons);

      break;
    case 3:
      ATH_MSG_DEBUG("Save pufit clusters MET");
      copier.addHelperCompToMET(TrigEFMissingEtComponent::TCPUC);
      copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::TCPUC);
      copier.setMETCompFromHelper(1, TrigEFMissingEtComponent::TCPUCUnc);
      copier.setMETCompFromHelper(2, TrigEFMissingEtComponent::Muons);

      break;
    case 1:
      copier.setMETCompFromHelper(0, TrigEFMissingEtComponent::Muons);
      break;

    default:
      ATH_MSG_WARNING( "Found " << nMetComp << " aux components in TrigMissingET.  Not supported.  NOT SAVING AUX INFO" );
  } ATH_MSG_DEBUG( "Found " << nMetComp << " aux components in TrigMissingET." );
  // also update transient helper class, for monitoring
  copier.setHelperFromMET();




  if(msgLvl(MSG::DEBUG)){
    std::string message;

    message = strformat ("REGTEST Ex =         %10.2f CLHEP::MeV", met->ex());
    ATH_MSG_DEBUG( message );
    message = strformat ("REGTEST Ey =         %10.2f CLHEP::MeV", met->ey());
    ATH_MSG_DEBUG( message );
    message = strformat ("REGTEST Ez =         %10.2f CLHEP::MeV", met->ez());
    ATH_MSG_DEBUG( message );
    message = strformat ("REGTEST SumET =         %10.2f CLHEP::MeV", met->sumEt());
    ATH_MSG_DEBUG( message );
    message = strformat ("REGTEST SumE =       %10.2f CLHEP::MeV", met->sumE());
    ATH_MSG_DEBUG( message );
    message = strformat ("REGTEST Flag =       %d", met->flag());
    ATH_MSG_DEBUG( message );

    message="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
      message+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
    
    if (nMetComp>0) ATH_MSG_DEBUG( message );

    for(uint j = 0; j < nMetComp; j++) 
    {
    	const char* name =               met->nameOfComponent(j).c_str();
    	const short status =             met->statusComponent(j);
    	const unsigned short usedChan =  met->usedChannelsComponent(j);
    	const short sumOfSigns =         met->sumOfSignsComponent(j);
    	const float calib0 =             met->calib0Component(j);
    	const float calib1 =             met->calib1Component(j);
    	const float ex =                 met->exComponent(j);
    	const float ey =                 met->eyComponent(j);
    	const float ez =                 met->ezComponent(j);
    	const float sumE =               met->sumEComponent(j);
    	const float sumEt =              met->sumEtComponent(j);

      message = strformat ("REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
       name, status, usedChan, sumOfSigns, calib1, calib0, ex, ey, ez, sumE, sumEt);
      ATH_MSG_DEBUG( message );
    }
  }
  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromHelper.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Diego Casadei
CREATED:  March 12, 2008

UPDATERS: Gabriel Gallardo, Manfredi Ronzani
DATE:     January 25, 2019

PURPOSE:  Updates TrigMissingET using TrigEFMissingEtHelper info for AthenaMT.

Components are left uncorrected when saved into
TrigMissingET as auxiliary information.

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETFromHelper.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "CxxUtils/StrFormat.h"
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"

#include <string>

using CxxUtils::strformat;

void EFMissingETFromHelper::ComponentCopier::addHelperCompToMET(unsigned char helper_i)
{
  TrigEFMissingEtComponent* metComp = m_helper->GetComponent(helper_i);

  float ex =            metComp->m_ex;
  float ey =            metComp->m_ey;
  float ez =            metComp->m_ez;
  float sumE =          metComp->m_sumE;
  float sumEt =         metComp->m_sumEt;
  float c0 =            metComp->m_calib0;
  float c1 =            metComp->m_calib1;
  // short status =        metComp->m_status;
  short sumOfSigns =    metComp->m_sumOfSigns;
  // unsigned short Ntot = metComp->m_usedChannels;

  m_met->setEx(       m_met->ex() + sumOfSigns * c0 + c1 * ex );
  m_met->setEy(       m_met->ey() + sumOfSigns * c0 + c1 * ey );
  m_met->setEz(       m_met->ez() + sumOfSigns * c0 + c1 * ez );
  m_met->setSumE(   m_met->sumE() + sumOfSigns * c0 + c1 * sumE );
  m_met->setSumEt( m_met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
  return;
}

void EFMissingETFromHelper::ComponentCopier::setMETCompFromHelper(unsigned char met_i, unsigned char helper_i)
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

void EFMissingETFromHelper::ComponentCopier::addMETCompWithHelper(unsigned char met_i, unsigned char met_add_i, unsigned char helper_i)
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

void EFMissingETFromHelper::ComponentCopier::setHelperFromMET()
{
  m_helper->SetEx(     m_met->ex()     );
  m_helper->SetEy(     m_met->ey()     );
  m_helper->SetEz(     m_met->ez()     );
  m_helper->SetSumEt(  m_met->sumEt( ) );
  m_helper->SetSumE(   m_met->sumE()   );
  return;
}
// ----------------------- //

EFMissingETFromHelper::EFMissingETFromHelper(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  EFMissingETBaseTool(type, name, parent)
{
   m_fextype = FexType::OTHER;
}



EFMissingETFromHelper::~EFMissingETFromHelper()
{
}



StatusCode EFMissingETFromHelper::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromHelper::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );
  }
  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  return StatusCode::SUCCESS;
}



StatusCode EFMissingETFromHelper::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromHelper::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromHelper::executeMT(xAOD::TrigMissingET *met, TrigEFMissingEtHelper *metHelper)
{
    return execute(met, metHelper, nullptr, nullptr, nullptr, nullptr, nullptr);
}

StatusCode EFMissingETFromHelper::execute(xAOD::TrigMissingET *met ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer * /* caloCluster */, const xAOD::JetContainer * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{
  ATH_MSG_DEBUG( "EFMissingETFromHelper::execute() called" );

  if (met==0 || metHelper==0) {
    ATH_MSG_ERROR( "ERROR: null pointers as input!" );
    return StatusCode::FAILURE;
  }

  ComponentCopier copier = ComponentCopier(met, metHelper);

  ATH_MSG_DEBUG( "Found this info in the helper class: " << metHelper->getFormattedValues() );

  if(m_timersvc)
    m_glob_timer->start(); // total time

  met->setFlag( metHelper->GetStatus() );

  unsigned int comp = met->getNumberOfComponents(); // final no. of aux. compon.
  unsigned char nHelperComp = metHelper->GetElements(); // no. of transient aux. compon.
  if (nHelperComp!=42) {
    ATH_MSG_WARNING( "Found " << nHelperComp << " aux components in the transient helper class.  Not supported!" );
  } else ATH_MSG_DEBUG( "Found " << nHelperComp << " aux components in the transient helper class" );

  bool skipAuxInfo=false;
  bool save9comp=false;
  bool save6comp=false;
  bool save5comp=false;
  bool save3comp=false;
  bool save2comp=false;
  bool save1comp=false;
  switch (comp) {
    case 25: // default: do nothing
      break;
    case 9:
      save9comp=true;
      break;
    case 6:
      save6comp=true;
      break;
    case 5:
      save5comp=true;
      break;
    case 3:
      save3comp=true;
      break;
    case 2:
      save2comp=true;
      break;
    case 1:
      save1comp=true;
      break;
    default:
      ATH_MSG_WARNING( "Found " << comp << " aux components in TrigMissingET.  Not supported.  NOT SAVING AUX INFO" );
      skipAuxInfo=true;
  }
      ATH_MSG_DEBUG( "Found " << comp << " aux components in TrigMissingET." );

  // Initialize EDM by setting all components to zero
  met->setEx(0.); met->setEy(0.); met->setEz(0.);
  met->setSumE(0.); met->setSumEt(0.);

  // take info from *metHelper and update *met
  for (unsigned char  helper_i=0; helper_i<nHelperComp; ++helper_i) { // loop over transient components

    // basic info - DK calibration
    if (helper_i<nHelperComp-18){  
      ATH_MSG_DEBUG( "skip muon or Had Topo granular or EM Topo correction for all quantities" );
      copier.addHelperCompToMET(helper_i);
    }

    if(save9comp && helper_i == 24) { 
      ATH_MSG_DEBUG( "Save summed HAD MET" );
      copier.addHelperCompToMET(helper_i);
    }

    if( (save2comp || save6comp) && helper_i == 34) { 
      ATH_MSG_DEBUG( "Save JET MET" );
      copier.addHelperCompToMET(helper_i);
    }

    if(save3comp && helper_i == 39) { 
      ATH_MSG_DEBUG( "Save PUC MET" );
      copier.addHelperCompToMET(helper_i);
    }

    if (skipAuxInfo) continue;

    // auxiliary info - uncorrected
    if (comp == unsigned(nHelperComp-17) && helper_i < 24) { 
      ATH_MSG_DEBUG( "finest granularity");
      copier.setMETCompFromHelper(helper_i, helper_i);
    } else if(comp == unsigned(nHelperComp-17) && helper_i == 41) { 
      ATH_MSG_DEBUG( "save muons");
      copier.setMETCompFromHelper(helper_i-17, helper_i);
    } else if (save6comp) {
      if (helper_i>=34 && helper_i < 39) { 
        ATH_MSG_DEBUG( "Central and Forward Jets");
        copier.setMETCompFromHelper(helper_i-34, helper_i);
      }
      if (helper_i==41) { 
        ATH_MSG_DEBUG( "Muons");
        copier.setMETCompFromHelper(5, helper_i);
      }

    } else if (save9comp) {
      if (helper_i > 24 && helper_i < 29 ) { 
        ATH_MSG_DEBUG( "HAD scale quantities" );
        copier.setMETCompFromHelper(helper_i-25, helper_i);
      } else if( helper_i > 29 && helper_i < 34) {     
        ATH_MSG_DEBUG( "EM scale quantities" );
        copier.setMETCompFromHelper(helper_i-25-1, helper_i);
      } else if( helper_i == 41) {    
        ATH_MSG_DEBUG( "Muon" );
        copier.setMETCompFromHelper(helper_i-25-8, helper_i);
      }
    } else if (save5comp) {
      switch (helper_i) {
        case 0: case 1: case 2: case 3: 
          ATH_MSG_DEBUG("LAr, barrel");
          copier.addMETCompWithHelper(0, 1, helper_i);
          break;
        case 4: case 5: case 6: case 7: 
          ATH_MSG_DEBUG("LAr, end-cap");
          /* FALLTHROUGH */
        case 21:                        
          ATH_MSG_DEBUG("+ FCalEM");
          copier.addMETCompWithHelper(1, 2, helper_i);
          break;
        case 12: case 13: case 14: 
          ATH_MSG_DEBUG("Tile, barrel +");
          /* FALLTHROUGH */
        case 18: case 19: case 20: 
          ATH_MSG_DEBUG("Tile, extended barrel");
          copier.addMETCompWithHelper(2, 3, helper_i);
          break;
        case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33:
        case 34: case 35: case 36: case 37: case 38:
        case 39: case 40 :             
          ATH_MSG_DEBUG("Topo. cluster elements or jets - do nothing.");
          break;
        case 41: 
          ATH_MSG_DEBUG("muons");
          copier.setMETCompFromHelper(4, helper_i);
          break;
        default: 
          ATH_MSG_DEBUG("Hadr. end-cap + Tile gap + FCalHad");
          copier.addMETCompWithHelper(3, 4, helper_i);
      }
    } else if (save3comp) {
      switch (helper_i) {
        case 39: 
          ATH_MSG_DEBUG( "Corrected MET" );
          copier.setMETCompFromHelper(0, helper_i);
          break;
        case 40: 
          ATH_MSG_DEBUG( "Original MET" );
          copier.setMETCompFromHelper(1, helper_i);
          break;
        case 41: 
          ATH_MSG_DEBUG( "Muons" );
          copier.setMETCompFromHelper(2, helper_i);
        default:
        ;
      }
    } else if (save2comp) { 
      switch(helper_i){
        case 34:
          ATH_MSG_DEBUG( "Jets+Mu only: Jets");
          copier.setMETCompFromHelper(0, helper_i);
          break;
        case 41:
          ATH_MSG_DEBUG( "Jets+Mu only: Muons");
          copier.setMETCompFromHelper(1, helper_i);
          break;
        default:
        ;
      }
    } else if (save1comp) { 
      if (helper_i==41) { // REPLACE WITH A TEST OVER COMP. NAME
        ATH_MSG_DEBUG("Muons only");
        copier.setMETCompFromHelper(0, helper_i);
      }
    }

  } // loop over transient components

  // also update transient helper class, for monitoring
  copier.setHelperFromMET();

  if(m_timersvc)
    m_glob_timer->stop(); // total time

  // Show the xAOD persistent object
  // Remove me please once done

  std::string s; //char buff[256];
  std::string message;

  if(msgLvl(MSG::DEBUG)){
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
  }

  unsigned int Nc = met->getNumberOfComponents();

  // if (Nc > 0) {
  //   if(msgLvl(MSG::DEBUG)){
        s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
			    s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
        ATH_MSG_DEBUG( s );
   //   }
   // }

   for(uint j = 0; j < Nc; j++) {

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

    // if(msgLvl(MSG::DEBUG)){
      message = strformat ("REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
       name, status, usedChan, sumOfSigns, calib1, calib0, ex, ey, ez, sumE, sumEt);
      ATH_MSG_DEBUG( message );
    // }

  }



  return StatusCode::SUCCESS;
}


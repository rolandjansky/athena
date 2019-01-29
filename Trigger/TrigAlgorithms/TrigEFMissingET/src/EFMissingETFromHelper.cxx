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

void EFMissingETFromHelper::setMET(xAOD::TrigMissingET *const met, TrigEFMissingEtHelper *metHelper, 
                                  const unsigned char comp_i)
{
  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(comp_i);

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

  met->setEx( met->ex() + sumOfSigns * c0 + c1 * ex );
  met->setEy( met->ey() + sumOfSigns * c0 + c1 * ey );
  met->setEz( met->ez() + sumOfSigns * c0 + c1 * ez );
  met->setSumE( met->sumE() + sumOfSigns * c0 + c1 * sumE );
  met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
  return;
}


void EFMissingETFromHelper::setMETComp(xAOD::TrigMissingET *const met, TrigEFMissingEtHelper *metHelper, 
                                        const unsigned char comp_i, const signed char add_i)
{
  TrigEFMissingEtComponent* metComp = metHelper->GetComponent(comp_i);

  float ex =            metComp->m_ex;
  float ey =            metComp->m_ey;
  float ez =            metComp->m_ez;
  float sumE =          metComp->m_sumE;
  float sumEt =         metComp->m_sumEt;
  float c0 =            metComp->m_calib0;
  float c1 =            metComp->m_calib1;
  short status =        metComp->m_status;
  short sumOfSigns =    metComp->m_sumOfSigns;
  unsigned short Ntot = metComp->m_usedChannels;
  char comp_name[10]="";
  strcpy(comp_name, metComp->m_name);

  bool addComponent = (add_i>=0);
  if (addComponent)
  {
    ex         += met->exComponent(add_i);
    ey         += met->eyComponent(add_i);
    ez         += met->ezComponent(add_i);
    sumE       += met->sumEComponent(add_i);
    sumEt      += met->sumEtComponent(add_i);
    sumOfSigns += met->sumOfSignsComponent(add_i);
    status     |= met->statusComponent(add_i);
    Ntot       += met->usedChannelsComponent(add_i);
  }

  if (!strcmp(comp_name, "") && !addComponent) 
    met->setNameOfComponent(        comp_i, comp_name );
  met->setExComponent(              comp_i, ex );
  met->setEyComponent(              comp_i, ey );
  met->setEzComponent(              comp_i, ez );
  met->setSumEtComponent(           comp_i, sumEt );
  met->setSumEComponent(            comp_i, sumE );
  met->setCalib0Component(          comp_i, c0 );
  met->setCalib1Component(          comp_i, c1 );
  met->setStatusComponent(          comp_i, status );
  met->setSumOfSignsComponent(      comp_i, sumOfSigns );
  met->setUsedChannelsComponent(    comp_i, Ntot );
  return;
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

  ATH_MSG_DEBUG( "Found this info in the helper class: " << metHelper->getFormattedValues() );

  if(m_timersvc)
    m_glob_timer->start(); // total time

  met->setFlag( metHelper->GetStatus() );

  unsigned int comp = met->getNumberOfComponents(); // final no. of aux. compon.
  unsigned char elem = metHelper->GetElements(); // no. of transient aux. compon.
  if (elem!=42) {
    ATH_MSG_WARNING( "Found " << elem << " aux components in the transient helper class.  Not supported!" );
  }

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

  // Initialize EDM by setting all components to zero
  met->setEx(0.); met->setEy(0.); met->setEz(0.);
  met->setSumE(0.); met->setSumEt(0.);

  // take info from *metHelper and update *met
  for (unsigned char  i=0; i<elem; ++i) { // loop over transient components

    // basic info - DK calibration
    if (i<elem-18){  // skip muon or Had Topo granular or EM Topo correction for all quantities
      setMET(met, metHelper, i);
    }

    if(save9comp && i == 24) { // Save summed HAD MET
      setMET(met, metHelper, i);
    }

    if( (save2comp || save6comp) && i == 34) { // Save JET MET
      setMET(met, metHelper, i);
    }

    if(save3comp && i == 39) { // Save PUC MET
      setMET(met, metHelper, i);
    }

    if (skipAuxInfo) continue;

    // auxiliary info - uncorrected
    if (comp == unsigned(elem-17) && i < 24) { // finest granularity
      setMETComp(met, metHelper, i);
    } else if(comp == unsigned(elem-17) && i == 41) { // save muons
      setMETComp(met, metHelper, i-17);
    } else if (save6comp) {
      if (i>=34 && i < 39) { // Central and Forward Jets
        setMETComp(met, metHelper, i-34);
      }
      if (i==41) { // Muons
        setMETComp(met, metHelper, 5);
      }

    } else if (save9comp) {
      if (i > 24 && i < 29 ) { // HAD scale quantities
      setMETComp(met, metHelper, i-25);
      } else if( i > 29 && i < 34) {     // EM scale quantities
      setMETComp(met, metHelper, i-25-1);
      } else if( i == 41) {    // Muon
      setMETComp(met, metHelper, i-25-8);
      }
    } else if (save5comp) {
      switch (i) {
        case 0: case 1: case 2: case 3: // LAr, barrel
          setMETComp(met, metHelper, 0, 1);
          break;
        case 4: case 5: case 6: case 7: // LAr, end-cap
        case 21:                        // + FCalEM
          setMETComp(met, metHelper, 1, 2);
          break;
        case 12: case 13: case 14: // Tile, barrel +
        case 18: case 19: case 20: // Tile, extended barrel
          setMETComp(met, metHelper, 2, 3);
          break;
        case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33:
        case 34: case 35: case 36: case 37: case 38:
        case 39: case 40 :             // Topo. cluster elements or jets - do nothing.
          break;
        case 41: // muons
          setMETComp(met, metHelper, 4);
          break;
        default: // Hadr. end-cap + Tile gap + FCalHad
          setMETComp(met, metHelper, 3, 4);
      }
    } else if (save3comp) {
      switch (i) {
        case 39: // Corrected MET
          setMETComp(met, metHelper, 0);
          break;
        case 40: // Original MET
          setMETComp(met, metHelper, 1);
          break;
        case 41: // Muons
          setMETComp(met, metHelper, 2);
      }
    } else if (save2comp) { // Jets + muons only
      if (i==34) { // Jets
        setMETComp(met, metHelper, 0);
      }
      if (i==41) { // Muons
        setMETComp(met, metHelper, 1);
      }
    } else if (save1comp) { // muons only
      if (i==41) { // REPLACE WITH A TEST OVER COMP. NAME
        setMETComp(met, metHelper, 0);
      }
    }

  } // loop over transient components

  // also update transient helper class, for monitoring
  metHelper->SetEx(     met->ex()     );
  metHelper->SetEy(     met->ey()     );
  metHelper->SetEz(     met->ez()     );
  metHelper->SetSumEt(  met->sumEt( ) );
  metHelper->SetSumE(   met->sumE()   );

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

  if (Nc > 0) {
    if(msgLvl(MSG::DEBUG)){
        s="REGTEST __name____status_usedChannels__sumOfSigns__calib1_calib0";
			    s+="/MeV__ex/MeV_____ey/MeV_____ez/MeV___sumE/MeV__sumEt/CLHEP::MeV";
        ATH_MSG_DEBUG( s );
     }
   }

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

     if(msgLvl(MSG::DEBUG)){
        message = strformat ("REGTEST   %s   %6d %12d %10d   %6.2f  %6.3f %10.2f %10.2f %10.2f %10.2f %10.2f",
			   name, status, usedChan, sumOfSigns, calib1, calib0, ex, ey, ez, sumE, sumEt);
        ATH_MSG_DEBUG( message );
      }

    }



  return StatusCode::SUCCESS;
}


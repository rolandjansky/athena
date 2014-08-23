/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromHelper.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Diego Casadei
CREATED:  March 12, 2008

PURPOSE:  Updates TrigMissingET using TrigEFMissingETHelper info.

Components are left uncorrected when saved into
TrigMissingET as auxiliary information.

On the other hand, basic info stored in TrigMissingET
is computed from components using the DK calibration
(by R. Djilkibaev and R. Konoplich).

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETFromHelper.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "GaudiKernel/ThreadGaudi.h"

#include <string>


EFMissingETFromHelper::EFMissingETFromHelper(const std::string& type, 
    const std::string& name, 
    const IInterface* parent):
  EFMissingETBaseTool(type, name, parent)
{
   _fextype = FexType::OTHER; 
}



EFMissingETFromHelper::~EFMissingETFromHelper()
{
}



StatusCode EFMissingETFromHelper::initialize()
{
  if(msgLvl( MSG::DEBUG)) {
    msg(MSG::DEBUG) << "called EFMissingETFromHelper::initialize()" << endreq;
  }

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg(MSG::WARNING) << name() <<
      ": Unable to locate TrigTimer Service" << endreq;
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


StatusCode EFMissingETFromHelper::execute(TrigMissingET *met ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer * /* caloCluster */)
{

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "EFMissingETFromHelper::execute() called" << endreq;
  }

  if (met==0 || metHelper==0) {
    msg(MSG::ERROR) << "ERROR: null pointers as input!" << endreq;
    return StatusCode::FAILURE;
  }

  if(msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Found this info in the helper class: "
      << metHelper->getFormattedValues() << endreq;
  }

  if(m_timersvc)
    m_glob_timer->start(); // total time

  met->clear();

  met->setFlag( metHelper->GetStatus() );

  unsigned int comp = met->getNumOfComponents(); // final no. of aux. compon.
  unsigned char elem = metHelper->GetElements(); // no. of transient aux. compon.
  if (elem!=35) {
    msg(MSG::WARNING) 
      << "Found " << elem
      << " aux components in the transient helper class.  Not supported!" 
      << endreq;
  }

  bool skipAuxInfo=false;
  bool save9comp=false;
  bool save5comp=false;
  bool save4comp=false;
  bool save3comp=false;
  bool save1comp=false;
  switch (comp) {
    case 25: // default: do nothing
      break;
    case 9:
      save9comp=true;
      break;
    case 5:
      save5comp=true;
      break;
    case 4:
      save4comp=true;
      break;
    case 3:
      save3comp=true;
      break;
    case 1:
      save1comp=true;
      break;
    default:
      msg(MSG::WARNING)  
        << "Found " << comp
        << " aux components in TrigMissingET.  Not supported.  NOT SAVING AUX INFO" 
        << endreq;
      skipAuxInfo=true;
  }
  
  // take info from *metHelper and update *met
  for (unsigned char  i=0; i<elem; ++i) { // loop over transient components
    TrigEFMissingEtComponent* metComp = metHelper->GetComponent(i);

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

    // basic info - DK calibration
    if (i<elem-11){  // skip muon or Had Topo granular or EM Topo correction for all quantities
      met->addEx(    sumOfSigns * c0 + c1 * ex );
      met->addEy(    sumOfSigns * c0 + c1 * ey );
      met->addEz(    sumOfSigns * c0 + c1 * ez );
      met->addSumE(  sumOfSigns * c0 + c1 * sumE );
      met->addSumEt( sumOfSigns * c0 + c1 * sumEt );
    }
    
    if(save9comp && i == 24) { // Save summed HAD MET
      met->addEx(    sumOfSigns * c0 + c1 * ex );
      met->addEy(    sumOfSigns * c0 + c1 * ey );
      met->addEz(    sumOfSigns * c0 + c1 * ez );
      met->addSumE(  sumOfSigns * c0 + c1 * sumE );
      met->addSumEt( sumOfSigns * c0 + c1 * sumEt );          	
    }
    
    if (skipAuxInfo) continue; 

    // auxiliary info - uncorrected
    if (comp == unsigned(elem-10) && i < 24) { // finest granularity
      met->setNameOfComponent( i, metComp->m_name );
      met->setExComponent(     i, ex );
      met->setEyComponent(     i, ey );
      met->setEzComponent(     i, ez );
      met->setSumEtComponent(  i, sumEt );
      met->setSumEComponent(   i, sumE );
      met->setComponentCalib0( i, c0 );
      met->setComponentCalib1( i, c1 );
      met->setStatus(          i, status );
      met->setSumOfSigns(      i, sumOfSigns );
      met->setUsedChannels(    i, Ntot );
    } else if(comp == unsigned(elem-10) && i == 34) { // save muons
      met->setNameOfComponent( i-10, metComp->m_name );
      met->setExComponent(     i-10, ex );
      met->setEyComponent(     i-10, ey );
      met->setEzComponent(     i-10, ez );
      met->setSumEtComponent(  i-10, sumEt );
      met->setSumEComponent(   i-10, sumE );
      met->setComponentCalib0( i-10, c0 );
      met->setComponentCalib1( i-10, c1 );
      met->setStatus(          i-10, status );
      met->setSumOfSigns(      i-10, sumOfSigns );
      met->setUsedChannels(    i-10, Ntot );   
    } else if (save9comp) {
      if (i > 24 && i < 29 ) { // HAD scale quantities
      met->setNameOfComponent( i-25, metComp->m_name );
      met->setExComponent(     i-25, ex );
      met->setEyComponent(     i-25, ey );
      met->setEzComponent(     i-25, ez );
      met->setSumEtComponent(  i-25, sumEt );
      met->setSumEComponent(   i-25, sumE );
      met->setComponentCalib0( i-25, c0 );
      met->setComponentCalib1( i-25, c1 );
      met->setStatus(          i-25, status );
      met->setSumOfSigns(      i-25, sumOfSigns );
      met->setUsedChannels(    i-25, Ntot );    
      } else if( i > 29) {     // EM scale quantities 
      met->setNameOfComponent( i-25-1, metComp->m_name );
      met->setExComponent(     i-25-1, ex );
      met->setEyComponent(     i-25-1, ey );
      met->setEzComponent(     i-25-1, ez );
      met->setSumEtComponent(  i-25-1, sumEt );
      met->setSumEComponent(   i-25-1, sumE );
      met->setComponentCalib0( i-25-1, c0 );
      met->setComponentCalib1( i-25-1, c1 );
      met->setStatus(          i-25-1, status );
      met->setSumOfSigns(      i-25-1, sumOfSigns );
      met->setUsedChannels(    i-25-1, Ntot );        		
      } 
    } else if (save5comp) {
      switch (i) {
        case 0: case 1: case 2: case 3: // LAr, barrel
          met->setExComponent(     0, ex         + met->getExComponent(1)    );
          met->setEyComponent(     0, ey         + met->getEyComponent(1)    );
          met->setEzComponent(     0, ez         + met->getEzComponent(1)    );
          met->setSumEtComponent(  0, sumEt      + met->getSumEtComponent(1) );
          met->setSumEComponent(   0, sumE       + met->getSumEComponent(1)  );
          met->setSumOfSigns(      0, sumOfSigns + met->getSumOfSigns(1)     );
          met->setUsedChannels(    0, Ntot       + met->getUsedChannels(1)   );
          met->setStatus(          0, status     | met->getStatus(1)         );
          met->setComponentCalib0( 0, c0 );
          met->setComponentCalib1( 0, c1 );
          break;
        case 4: case 5: case 6: case 7: // LAr, end-cap
        case 21:                        // + FCalEM
          met->setExComponent(     1, ex         + met->getExComponent(2)    );
          met->setEyComponent(     1, ey         + met->getEyComponent(2)    );
          met->setEzComponent(     1, ez         + met->getEzComponent(2)    );
          met->setSumEtComponent(  1, sumEt      + met->getSumEtComponent(2) );
          met->setSumEComponent(   1, sumE       + met->getSumEComponent(2)  );
          met->setSumOfSigns(      1, sumOfSigns + met->getSumOfSigns(2)     );
          met->setUsedChannels(    1, Ntot       + met->getUsedChannels(2)   );
          met->setStatus(          1, status     | met->getStatus(2)         );
          met->setComponentCalib0( 1, c0 );
          met->setComponentCalib1( 1, c1 );
          break;
        case 12: case 13: case 14: // Tile, barrel +
        case 18: case 19: case 20: // Tile, extended barrel
          met->setExComponent(     2, ex         + met->getExComponent(3)    );
          met->setEyComponent(     2, ey         + met->getEyComponent(3)    );
          met->setEzComponent(     2, ez         + met->getEzComponent(3)    );
          met->setSumEtComponent(  2, sumEt      + met->getSumEtComponent(3) );
          met->setSumEComponent(   2, sumE       + met->getSumEComponent(3)  );
          met->setSumOfSigns(      2, sumOfSigns + met->getSumOfSigns(3)     );
          met->setUsedChannels(    2, Ntot       + met->getUsedChannels(3)   );
          met->setStatus(          2, status     | met->getStatus(3)         );
          met->setComponentCalib0( 2, c0 );
          met->setComponentCalib1( 2, c1 );
          break;
        case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33:
        case 34: case 35: case 36: case 37:          // Topo. cluster elements - do nothing. 
          break;  
        case 38: // muons
          met->setExComponent(     4, ex );
          met->setEyComponent(     4, ey );
          met->setEzComponent(     4, ez );
          met->setSumEtComponent(  4, sumEt );
          met->setSumEComponent(   4, sumE  );
          met->setComponentCalib0( 4, c0 );
          met->setComponentCalib1( 4, c1 );
          met->setStatus(          4, status );
          met->setSumOfSigns(      4, sumOfSigns );
          met->setUsedChannels(    4, Ntot );
          break;
        default: // Hadr. end-cap + Tile gap + FCalHad
          met->setExComponent(     3, ex         + met->getExComponent(4)    );
          met->setEyComponent(     3, ey         + met->getEyComponent(4)    );
          met->setEzComponent(     3, ez         + met->getEzComponent(4)    );
          met->setSumEtComponent(  3, sumEt      + met->getSumEtComponent(4) );
          met->setSumEComponent(   3, sumE       + met->getSumEComponent(4)  );
          met->setSumOfSigns(      3, sumOfSigns + met->getSumOfSigns(4)     );
          met->setUsedChannels(    3, Ntot       + met->getUsedChannels(4)   );
          met->setStatus(          3, status     | met->getStatus(4)         );
          met->setComponentCalib0( 3, c0 );
          met->setComponentCalib1( 3, c1 );
      }
    } else if (save3comp) {
      switch (i) {
        case 24:
          met->setNameOfComponent( 0, metComp->m_name );
          met->setExComponent(     0, ex );
          met->setEyComponent(     0, ey );
          met->setEzComponent(     0, ez );
          met->setSumEtComponent(  0, sumEt );
          met->setSumEComponent(   0, sumE );
          met->setComponentCalib0( 0, c0 );
          met->setComponentCalib1( 0, c1 );
          met->setStatus(          0, status );
          met->setSumOfSigns(      0, sumOfSigns );
          met->setUsedChannels(    0, Ntot );
          break;
        case 29: // Had
          met->setNameOfComponent( 1, metComp->m_name );
          met->setExComponent(     1, ex );
          met->setEyComponent(     1, ey );
          met->setEzComponent(     1, ez );
          met->setSumEtComponent(  1, sumEt );
          met->setSumEComponent(   1, sumE );
          met->setComponentCalib0( 1, c0 );
          met->setComponentCalib1( 1, c1 );
          met->setStatus(          1, status );
          met->setSumOfSigns(      1, sumOfSigns );
          met->setUsedChannels(    1, Ntot );
          break;
        case 34: // Muons
          met->setNameOfComponent( 2, metComp->m_name );
          met->setExComponent(     2, ex );
          met->setEyComponent(     2, ey );
          met->setEzComponent(     2, ez );
          met->setSumEtComponent(  2, sumEt );
          met->setSumEComponent(   2, sumE );
          met->setComponentCalib0( 2, c0 );
          met->setComponentCalib1( 2, c1 );
          met->setStatus(          2, status );
          met->setSumOfSigns(      2, sumOfSigns );
          met->setUsedChannels(    2, Ntot );   
      }
    } else if (save1comp) { // muons only
      if (i==34) { // REPLACE WITH A TEST OVER COMP. NAME
        met->setNameOfComponent( 0, metComp->m_name );
        met->setExComponent(     0, ex );
        met->setEyComponent(     0, ey );
        met->setEzComponent(     0, ez );
        met->setSumEtComponent(  0, sumEt );
        met->setSumEComponent(   0, sumE );
        met->setComponentCalib0( 0, c0 );
        met->setComponentCalib1( 0, c1 );
        met->setStatus(          0, status );
        met->setSumOfSigns(      0, sumOfSigns );
        met->setUsedChannels(    0, Ntot );
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

  return StatusCode::SUCCESS;
}

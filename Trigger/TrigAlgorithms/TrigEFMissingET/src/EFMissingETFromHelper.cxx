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



StatusCode EFMissingETFromHelper::execute()
{
  return StatusCode::SUCCESS;
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
    if (i<elem-18){  // skip muon or Had Topo granular or EM Topo correction for all quantities
      met->setEx( met->ex() + sumOfSigns * c0 + c1 * ex );
      met->setEy( met->ey() + sumOfSigns * c0 + c1 * ey );
      met->setEz( met->ez() + sumOfSigns * c0 + c1 * ez );
      met->setSumE( met->sumE() + sumOfSigns * c0 + c1 * sumE );
      met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
    }

    if(save9comp && i == 24) { // Save summed HAD MET
      met->setEx( met->ex() + sumOfSigns * c0 + c1 * ex );
      met->setEy( met->ey() + sumOfSigns * c0 + c1 * ey );
      met->setEz( met->ez() + sumOfSigns * c0 + c1 * ez );
      met->setSumE( met->sumE() + sumOfSigns * c0 + c1 * sumE );
      met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
    }

    if( (save2comp || save6comp) && i == 34) { // Save JET MET
      met->setEx( met->ex() + sumOfSigns * c0 + c1 * ex );
      met->setEy( met->ey() + sumOfSigns * c0 + c1 * ey );
      met->setEz( met->ez() + sumOfSigns * c0 + c1 * ez );
      met->setSumE( met->sumE() + sumOfSigns * c0 + c1 * sumE );
      met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
    }

    if(save3comp && i == 39) { // Save PUC MET
      met->setEx( met->ex() + sumOfSigns * c0 + c1 * ex );
      met->setEy( met->ey() + sumOfSigns * c0 + c1 * ey );
      met->setEz( met->ez() + sumOfSigns * c0 + c1 * ez );
      met->setSumE( met->sumE() + sumOfSigns * c0 + c1 * sumE );
      met->setSumEt( met->sumEt() + sumOfSigns * c0 + c1 * sumEt );
    }

    if (skipAuxInfo) continue;

    // auxiliary info - uncorrected
    if (comp == unsigned(elem-17) && i < 24) { // finest granularity
      met->setNameOfComponent( i, metComp->m_name );
      met->setExComponent(     i, ex );
      met->setEyComponent(     i, ey );
      met->setEzComponent(     i, ez );
      met->setSumEtComponent(  i, sumEt );
      met->setSumEComponent(   i, sumE );
      met->setCalib0Component( i, c0 );
      met->setCalib1Component( i, c1 );
      met->setStatusComponent(          i, status );
      met->setSumOfSignsComponent(      i, sumOfSigns );
      met->setUsedChannelsComponent(    i, Ntot );
    } else if(comp == unsigned(elem-17) && i == 41) { // save muons
      met->setNameOfComponent( i-17, metComp->m_name );
      met->setExComponent(     i-17, ex );
      met->setEyComponent(     i-17, ey );
      met->setEzComponent(     i-17, ez );
      met->setSumEtComponent(  i-17, sumEt );
      met->setSumEComponent(   i-17, sumE );
      met->setCalib0Component( i-17, c0 );
      met->setCalib1Component( i-17, c1 );
      met->setStatusComponent(          i-17, status );
      met->setSumOfSignsComponent(      i-17, sumOfSigns );
      met->setUsedChannelsComponent(    i-17, Ntot );
    } else if (save6comp) {
      if (i>=34 && i < 39) { // Central and Forward Jets
        met->setNameOfComponent( i-34, metComp->m_name );
        met->setExComponent(     i-34, ex );
        met->setEyComponent(     i-34, ey );
        met->setEzComponent(     i-34, ez );
        met->setSumEtComponent(  i-34, sumEt );
        met->setSumEComponent(   i-34, sumE );
        met->setCalib0Component( i-34, c0 );
        met->setCalib1Component( i-34, c1 );
        met->setStatusComponent(          i-34, status );
        met->setSumOfSignsComponent(      i-34, sumOfSigns );
        met->setUsedChannelsComponent(    i-34, Ntot );
      }
      if (i==41) { // Muons
        met->setNameOfComponent( 5, metComp->m_name );
        met->setExComponent(     5, ex );
        met->setEyComponent(     5, ey );
        met->setEzComponent(     5, ez );
        met->setSumEtComponent(  5, sumEt );
        met->setSumEComponent(   5, sumE );
        met->setCalib0Component( 5, c0 );
        met->setCalib1Component( 5, c1 );
        met->setStatusComponent(          5, status );
        met->setSumOfSignsComponent(      5, sumOfSigns );
        met->setUsedChannelsComponent(    5, Ntot );
      }

    } else if (save9comp) {
      if (i > 24 && i < 29 ) { // HAD scale quantities
      met->setNameOfComponent( i-25, metComp->m_name );
      met->setExComponent(     i-25, ex );
      met->setEyComponent(     i-25, ey );
      met->setEzComponent(     i-25, ez );
      met->setSumEtComponent(  i-25, sumEt );
      met->setSumEComponent(   i-25, sumE );
      met->setCalib0Component( i-25, c0 );
      met->setCalib1Component( i-25, c1 );
      met->setStatusComponent(          i-25, status );
      met->setSumOfSignsComponent(      i-25, sumOfSigns );
      met->setUsedChannelsComponent(    i-25, Ntot );
      } else if( i > 29 && i < 34) {     // EM scale quantities
      met->setNameOfComponent( i-25-1, metComp->m_name );
      met->setExComponent(     i-25-1, ex );
      met->setEyComponent(     i-25-1, ey );
      met->setEzComponent(     i-25-1, ez );
      met->setSumEtComponent(  i-25-1, sumEt );
      met->setSumEComponent(   i-25-1, sumE );
      met->setCalib0Component( i-25-1, c0 );
      met->setCalib1Component( i-25-1, c1 );
      met->setStatusComponent(          i-25-1, status );
      met->setSumOfSignsComponent(      i-25-1, sumOfSigns );
      met->setUsedChannelsComponent(    i-25-1, Ntot );
      } else if( i == 41) {    // Muon
      met->setNameOfComponent( i-25-8, metComp->m_name );
      met->setExComponent(     i-25-8, ex );
      met->setEyComponent(     i-25-8, ey );
      met->setEzComponent(     i-25-8, ez );
      met->setSumEtComponent(  i-25-8, sumEt );
      met->setSumEComponent(   i-25-8, sumE );
      met->setCalib0Component( i-25-8, c0 );
      met->setCalib1Component( i-25-8, c1 );
      met->setStatusComponent(          i-25-8, status );
      met->setSumOfSignsComponent(      i-25-8, sumOfSigns );
      met->setUsedChannelsComponent(    i-25-8, Ntot );
      }
    } else if (save5comp) {
      switch (i) {
        case 0: case 1: case 2: case 3: // LAr, barrel
          met->setExComponent(     0, ex         + met->exComponent(1)    );
          met->setEyComponent(     0, ey         + met->eyComponent(1)    );
          met->setEzComponent(     0, ez         + met->ezComponent(1)    );
          met->setSumEtComponent(  0, sumEt      + met->sumEtComponent(1) );
          met->setSumEComponent(   0, sumE       + met->sumEComponent(1)  );
          met->setSumOfSignsComponent(      0, sumOfSigns + met->sumOfSignsComponent(1)     );
          met->setUsedChannelsComponent(    0, Ntot       + met->usedChannelsComponent(1)   );
          met->setStatusComponent(          0, status     | met->statusComponent(1)         );
          met->setCalib0Component( 0, c0 );
          met->setCalib1Component( 0, c1 );
          break;
        case 4: case 5: case 6: case 7: // LAr, end-cap
        case 21:                        // + FCalEM
          met->setExComponent(     1, ex         + met->exComponent(2)    );
          met->setEyComponent(     1, ey         + met->eyComponent(2)    );
          met->setEzComponent(     1, ez         + met->ezComponent(2)    );
          met->setSumEtComponent(  1, sumEt      + met->sumEtComponent(2) );
          met->setSumEComponent(   1, sumE       + met->sumEComponent(2)  );
          met->setSumOfSignsComponent(      1, sumOfSigns + met->sumOfSignsComponent(2)     );
          met->setUsedChannelsComponent(    1, Ntot       + met->usedChannelsComponent(2)   );
          met->setStatusComponent(          1, status     | met->statusComponent(2)         );
          met->setCalib0Component( 1, c0 );
          met->setCalib1Component( 1, c1 );
          break;
        case 12: case 13: case 14: // Tile, barrel +
        case 18: case 19: case 20: // Tile, extended barrel
          met->setExComponent(     2, ex         + met->exComponent(3)    );
          met->setEyComponent(     2, ey         + met->eyComponent(3)    );
          met->setEzComponent(     2, ez         + met->ezComponent(3)    );
          met->setSumEtComponent(  2, sumEt      + met->sumEtComponent(3) );
          met->setSumEComponent(   2, sumE       + met->sumEComponent(3)  );
          met->setSumOfSignsComponent(      2, sumOfSigns + met->sumOfSignsComponent(3)     );
          met->setUsedChannelsComponent(    2, Ntot       + met->usedChannelsComponent(3)   );
          met->setStatusComponent(          2, status     | met->statusComponent(3)         );
          met->setCalib0Component( 2, c0 );
          met->setCalib1Component( 2, c1 );
          break;
        case 24: case 25: case 26: case 27: case 28:
        case 29: case 30: case 31: case 32: case 33:
        case 34: case 35: case 36: case 37: case 38:
        case 39: case 40 :             // Topo. cluster elements or jets - do nothing.
          break;
        case 41: // muons
          met->setExComponent(     4, ex );
          met->setEyComponent(     4, ey );
          met->setEzComponent(     4, ez );
          met->setSumEtComponent(  4, sumEt );
          met->setSumEComponent(   4, sumE  );
          met->setCalib0Component( 4, c0 );
          met->setCalib1Component( 4, c1 );
          met->setStatusComponent(          4, status );
          met->setSumOfSignsComponent(      4, sumOfSigns );
          met->setUsedChannelsComponent(    4, Ntot );
          break;
        default: // Hadr. end-cap + Tile gap + FCalHad
          met->setExComponent(     3, ex         + met->exComponent(4)    );
          met->setEyComponent(     3, ey         + met->eyComponent(4)    );
          met->setEzComponent(     3, ez         + met->ezComponent(4)    );
          met->setSumEtComponent(  3, sumEt      + met->sumEtComponent(4) );
          met->setSumEComponent(   3, sumE       + met->sumEComponent(4)  );
          met->setSumOfSignsComponent(      3, sumOfSigns + met->sumOfSignsComponent(4)     );
          met->setUsedChannelsComponent(    3, Ntot       + met->usedChannelsComponent(4)   );
          met->setStatusComponent(          3, status     | met->statusComponent(4)         );
          met->setCalib0Component( 3, c0 );
          met->setCalib1Component( 3, c1 );
      }
    } else if (save3comp) {
      switch (i) {
        case 39: // Corrected MET
          met->setNameOfComponent( 0, metComp->m_name );
          met->setExComponent(     0, ex );
          met->setEyComponent(     0, ey );
          met->setEzComponent(     0, ez );
          met->setSumEtComponent(  0, sumEt );
          met->setSumEComponent(   0, sumE );
          met->setCalib0Component( 0, c0 );
          met->setCalib1Component( 0, c1 );
          met->setStatusComponent(          0, status );
          met->setSumOfSignsComponent(      0, sumOfSigns );
          met->setUsedChannelsComponent(    0, Ntot );
          break;
        case 40: // Original MET
          met->setNameOfComponent( 1, metComp->m_name );
          met->setExComponent(     1, ex );
          met->setEyComponent(     1, ey );
          met->setEzComponent(     1, ez );
          met->setSumEtComponent(  1, sumEt );
          met->setSumEComponent(   1, sumE );
          met->setCalib0Component( 1, c0 );
          met->setCalib1Component( 1, c1 );
          met->setStatusComponent(          1, status );
          met->setSumOfSignsComponent(      1, sumOfSigns );
          met->setUsedChannelsComponent(    1, Ntot );
          break;
        case 41: // Muons
          met->setNameOfComponent( 2, metComp->m_name );
          met->setExComponent(     2, ex );
          met->setEyComponent(     2, ey );
          met->setEzComponent(     2, ez );
          met->setSumEtComponent(  2, sumEt );
          met->setSumEComponent(   2, sumE );
          met->setCalib0Component( 2, c0 );
          met->setCalib1Component( 2, c1 );
          met->setStatusComponent(          2, status );
          met->setSumOfSignsComponent(      2, sumOfSigns );
          met->setUsedChannelsComponent(    2, Ntot );
      }
    } else if (save2comp) { // Jets + muons only
      if (i==34) { // Jets
        met->setNameOfComponent( 0, metComp->m_name );
        met->setExComponent(     0, ex );
        met->setEyComponent(     0, ey );
        met->setEzComponent(     0, ez );
        met->setSumEtComponent(  0, sumEt );
        met->setSumEComponent(   0, sumE );
        met->setCalib0Component( 0, c0 );
        met->setCalib1Component( 0, c1 );
        met->setStatusComponent(          0, status );
        met->setSumOfSignsComponent(      0, sumOfSigns );
        met->setUsedChannelsComponent(    0, Ntot );
      }
      if (i==41) { // Muons
        met->setNameOfComponent( 1, metComp->m_name );
        met->setExComponent(     1, ex );
        met->setEyComponent(     1, ey );
        met->setEzComponent(     1, ez );
        met->setSumEtComponent(  1, sumEt );
        met->setSumEComponent(   1, sumE );
        met->setCalib0Component( 1, c0 );
        met->setCalib1Component( 1, c1 );
        met->setStatusComponent(          1, status );
        met->setSumOfSignsComponent(      1, sumOfSigns );
        met->setUsedChannelsComponent(    1, Ntot );
      }
    } else if (save1comp) { // muons only
      if (i==41) { // REPLACE WITH A TEST OVER COMP. NAME
        met->setNameOfComponent( 0, metComp->m_name );
        met->setExComponent(     0, ex );
        met->setEyComponent(     0, ey );
        met->setEzComponent(     0, ez );
        met->setSumEtComponent(  0, sumEt );
        met->setSumEComponent(   0, sumE );
        met->setCalib0Component( 0, c0 );
        met->setCalib1Component( 0, c1 );
        met->setStatusComponent(          0, status );
        met->setSumOfSignsComponent(      0, sumOfSigns );
        met->setUsedChannelsComponent(    0, Ntot );
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


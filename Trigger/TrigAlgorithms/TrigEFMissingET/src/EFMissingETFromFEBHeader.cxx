/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromFEBHeader.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev, Ignacio Aracena, Diego Casadei, Chris Young
CREATED:  March 1, 2006

PURPOSE:  Data preparation from FEB Header

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromFEBHeader.h"
#include "TrigMissingEtEvent/TrigMissingET.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "TrigT2CaloCommon/TrigDataAccess.h"

#include <iostream>


EFMissingETFromFEBHeader::EFMissingETFromFEBHeader(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  EFMissingETBaseTool(type, name, parent)
{

  declareProperty("useFullCollection", m_useFullColl = true);
  m_usedfeb = nullptr;

  m_data = nullptr;
  m_cablingSvc = nullptr;
  m_LArOnlineID = nullptr;
  m_CaloCell_ID = nullptr;

  m_fextype = FexType::FEB;

}

EFMissingETFromFEBHeader::~EFMissingETFromFEBHeader()
{
}

StatusCode EFMissingETFromFEBHeader::initialize()
{

  ATH_MSG_DEBUG( " In initialize() of EFMissingETFromFEBHeader " );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );
  }
  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);

    // total time = EM + HEC + Tile + FCal
    // detector time = RegionSelector + LoadCollections + loop
    const char* detName[4] = {"EM", "HEC", "Tile", "FCal"};
    const char* stepName[3] = {"RegionSelector", "LoadCollections", "loop"};

    for (int iDet=0; iDet<4; ++iDet){ // loop on detector
      for (int iStep=0; iStep<3; ++iStep) { // loop on step
        basename=name()+".";
        basename+=detName[iDet];
        basename+=stepName[iStep];
        m_timer[iDet][iStep] = m_timersvc->addItem(basename);
      } // loop on step
    } // loop on detector
  } // if timing service


  if (toolSvc()->retrieveTool("TrigDataAccess", m_data).isFailure()) {
    ATH_MSG_FATAL( "Could not get m_data" );
    return StatusCode::FAILURE;
  }

  if(toolSvc()->retrieveTool("LArCablingLegacyService",m_cablingSvc).isFailure()) {
    ATH_MSG_FATAL( "Could not get LArCablingLegacyService" );
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore = nullptr;
  if (service( "DetectorStore", detStore ).isFailure()) {
    ATH_MSG_FATAL( "Unable to locate DetectorStore" );
    return StatusCode::FAILURE;
  }

  if (detStore->retrieve(m_LArOnlineID, "LArOnlineID").isFailure()) {
    ATH_MSG_FATAL( "Could not get LArOnlineID helper!" );
    return StatusCode::FAILURE;
  }

  if (detStore->retrieve(m_CaloCell_ID, "CaloCell_ID").isFailure()) {
    ATH_MSG_FATAL( "Could not get CaloCell_ID helper!" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromFEBHeader::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromFEBHeader::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromFEBHeader::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper * metHelper,
    const xAOD::CaloClusterContainer * /* caloCluster */, const xAOD::JetContainer  * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "EFMissingETFromFEBHeader::execute() has been called" );

  if(m_timersvc){
    m_glob_timer->start(); // total time

    for (int iDet=0; iDet<4; ++iDet){ // loop on detector
      for (int iStep=0; iStep<3; ++iStep) { // loop on step
        m_timer[iDet][iStep]->start();
        m_timer[iDet][iStep]->pause();
      } // loop on step
    } // loop on detector
  } // if timing service

  // reset components that need to be processed
  for (int k=0; k<metHelper->GetElements(); ++k) {
    TrigEFMissingEtComponent *metComp=metHelper->GetComponent(k);
    if ( (metComp->m_status & m_maskProcessed)==0 ) metComp->Reset();
  }

  m_usedfeb = new std::vector<int>;

  ATH_MSG_DEBUG( "EFMissingETFromFEBHeader::execute() is calling addFebEnergyToHelper()" );
  StatusCode sc = addFebEnergyToHelper(metHelper);
  if(sc.isFailure()){
    ATH_MSG_ERROR( " Failure of addFebEnergy" );
    return sc;
  }

  // move from "processing" to "processed" state
  for (int k=0; k<metHelper->GetElements(); ++k) {
    TrigEFMissingEtComponent *metComp=metHelper->GetComponent(k);
    if ( metComp->m_status & m_maskProcessing ) {
      metComp->m_status ^= m_maskProcessing; // switch off bit
      metComp->m_status |= m_maskProcessed;  // switch on bit
    }
  }

  if(m_timersvc){
    for (int iDet=0; iDet<4; ++iDet){ // loop on detector
      for (int iStep=0; iStep<3; ++iStep) { // loop on step
        m_timer[iDet][iStep]->stop();
      } // loop on step
    } // loop on detector

    m_glob_timer->stop(); // total time
  } // if timing service

  delete m_usedfeb;
  m_usedfeb = nullptr;

  return StatusCode::SUCCESS;
}

//////--

StatusCode EFMissingETFromFEBHeader::addFebEnergyToHelper(TrigEFMissingEtHelper* metHelper){

  ATH_MSG_DEBUG( "addFebEnergyToHelper()" );

  if(m_useFullColl){ // use LoadFullCollections in TrigDataAccess

    ATH_MSG_DEBUG( "EFMissingETFromFEBHeader::addFebEnergyToHelper() is using LoadFullCollections" );

    // Now all TTEM
    StatusCode sc = addFullLArFebEnergyToHelper(metHelper, TTEM, true);
    if (sc.isFailure()) return sc;

    // Now all TTHEC
    sc = addFullLArFebEnergyToHelper(metHelper, TTHEC, true);
    if (sc.isFailure()) return sc;

    // Now all FCALEM
    sc = addFullLArFebEnergyToHelper(metHelper, FCALEM, true);
    if (sc.isFailure()) return sc;

    // Now all FCALHAD
    sc = addFullLArFebEnergyToHelper(metHelper, FCALHAD, true);
    if (sc.isFailure()) return sc;

    // Now all TILE
    sc = addFullTileFebEnergyToHelper(metHelper, TILE, true);
    if (sc.isFailure()) return sc;

  } else { // use RegionSelector() to load collections (slower!)

    ATH_MSG_DEBUG( "EFMissingETFromFEBHeader::addFebEnergyToHelper() is using RegionSelector" );

    const double phimin = -M_PI;    const double phimax =  M_PI;
    const double etamin = -4.8;     const double etamax =  4.8;

    // Now EM
    StatusCode sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, TTEM, 0, true);
    if (sc.isFailure()) return sc;

    //  etamin = -3.2;  etamax = 3.2;
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, TTEM, 1, true);
    if (sc.isFailure()) return sc;

    //  etamin = -3.2;  etamax = 3.2;
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, TTEM, 2, true);
    if (sc.isFailure()) return sc;

    //  etamin = -2.5;  etamax = 2.5;
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, TTEM, 3, true);
    if (sc.isFailure()) return sc;

    // Now HEC (NB: all HEC samplings are accessed from TTHEC,0)
    //  etamin = -3.2;  etamax = 3.2;
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, TTHEC, 0, true);
    if (sc.isFailure()) return sc;

    // Now FCALEM
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, FCALEM, 0, true);
    if (sc.isFailure()) return sc;

    // FCALHAD has 2 layers
    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, FCALHAD, 0, true);
    if (sc.isFailure()) return sc;

    sc = addLArFebEnergyToHelper(etamin, etamax, phimin, phimax, metHelper, FCALHAD, 1, true);
    if (sc.isFailure()) return sc;
  }

  ATH_MSG_DEBUG( metHelper->getFormattedValues() );

  return StatusCode::SUCCESS;

}


//////--

StatusCode EFMissingETFromFEBHeader::addLArFebEnergyToHelper(double etamin, double etamax,
    double phimin, double phimax,
    TrigEFMissingEtHelper* metHelper,
    DETID detectorID, int sampling,
    bool prepare){
  ATH_MSG_DEBUG( "addLArFebEnergyToHelper(DETID=" << detectorID << ", sampling=" << sampling << ")" );

  int iDet=0;
  //CaloSampling::CaloSample samp = CaloSampling::PreSamplerB;
  switch (detectorID) {
    case TTEM:
      iDet=0;
      //samp = CaloSampling::PreSamplerB;
      break;
    case TTHEC:
      iDet=1;
      //samp = CaloSampling::HEC0;
      break;
    case FCALEM:
      iDet=3;
      //samp = CaloSampling::FCAL0;
      break;
    case FCALHAD:
      iDet=3;
      //samp = CaloSampling::FCAL1;
      break;
    default:
      break;
  }

  // Get detector offline ID's for Collections
  if (m_timersvc) m_timer[iDet][0]->resume();
  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, detectorID);
  if (m_timersvc) m_timer[iDet][0]->pause();

  // Prepare data, load collections
  if (m_timersvc) m_timer[iDet][1]->resume();
  StatusCode sc = m_data->LoadCollections(m_iFebBegin,m_iFebEnd, sampling , prepare);
  if (sc.isFailure()) return sc;
  if (m_timersvc) m_timer[iDet][1]->pause();

  // Algorithmic time
  if (m_timersvc) m_timer[iDet][2]->resume();

  // check errors from byte-stream conversion
  bool BSerrors = false;
  if (m_data->report_error()) BSerrors = true;

  for(m_Febit=m_iFebBegin ;m_Febit!=m_iFebEnd; ++m_Febit){ // Main Loop
    // possibly do some noise thresholding

    bool used=false;
    int febid = (*m_Febit)->getFebId();
    int idit = 0;
    int iditend = m_usedfeb->size();
    for(;idit!=iditend; ++idit){ // would be better to use find()...
      if(febid == (*m_usedfeb)[idit] ){
        used = true;
        idit++;
        break;
      }
    }
    m_usedfeb->push_back(febid);
    if(!used){
      // find the sampling
      HWIdentifier febid2(febid);

      Identifier offChId;
      int ichannel=0;
      do {
        HWIdentifier onlChId = m_LArOnlineID->channel_Id(febid2,ichannel);
        offChId = m_cablingSvc->cnvToIdentifier(onlChId);
        ichannel++;
        if (ichannel>127) {
          ATH_MSG_ERROR( "not connected channel found for this FEB: " << febid );
          return StatusCode::RECOVERABLE;
        }
      } while(!offChId.is_valid());

      int caloSamp = m_CaloCell_ID->sampling(offChId);
      HWIdentifier modId = m_cablingSvc->getReadoutModuleID(febid2); //ReadOutModuleId
      int subdet =  m_larROModSvc.em_hec_fcal(modId);  // em=0, hec=1, fcal=2
      int caloId =  m_larROModSvc.barrel_ec(modId);    // barrel:0 or EndCAp:1
      //       int posneg =  m_larROModSvc.pos_neg(modId);      // A(pos:1) or C(neg:0)

      unsigned char k=0;
      bool doHackForHEC=false;
      switch (subdet) {
        case 0: // EM
          k = 4*caloId + caloSamp;
          break;
        case 1: // HEC
          k = 8 + caloSamp;
          doHackForHEC=true;
          break;
        case 2: // FCAL
          k = 20 + caloSamp;
          break;
        default:
          ATH_MSG_FATAL( "Unknown subdetector!" );
          return StatusCode::FAILURE;
      }

      TrigEFMissingEtComponent* metComp = metHelper->GetComponent(k);
      if (metComp==0) {
        ATH_MSG_FATAL("Cannot find calo sampling " << (int)k
                      << ". caloId=" << caloId << ", caloSamp=" << caloSamp
                      << ", subdet=" << subdet << " (febid=" << febid
                      << ", offChId=" << offChId << ")" );
        return StatusCode::FAILURE;
      }
      if (metComp->m_skip) {
        ATH_MSG_DEBUG( "Skipping L.Ar cells from (DETID = " << detectorID
                       << ", sampling = " << sampling << "), i.e. "
                       << metComp->m_name );
        break;
      }
      else if ( metComp->m_status & m_maskProcessed ) // already done
        break;

//       // detect error
//       if (error) {
// 	metComp->m_status |= m_maskErrFEB | m_maskCompErrors;
//       }

      double Ex = (*m_Febit)->getFebEx();
      double Ey = (*m_Febit)->getFebEy();
      double Ez = (*m_Febit)->getFebEz();
      double Et = sqrt(Ex*Ex + Ey*Ey);

      metComp->m_ex -= Ex;
      metComp->m_ey -= Ey;
      metComp->m_ez -= Ez;
      metComp->m_sumEt += Et;
      metComp->m_sumE += sqrt(Et*Et + Ez*Ez);
      metComp->m_usedChannels += 1;
      metComp->m_sumOfSigns += 1; // always positive energy
      metComp->m_status |= m_maskProcessing; // go to "processing" state
      if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
      if (doHackForHEC) { // all HEC FEBs appear to belong to the first sampling
        for (unsigned char jj=0; jj<4; ++jj) {
          metComp = metHelper->GetComponent(8+jj);
          if (metComp) metComp->m_status |= m_maskProcessing;
        }
      }
    }

  } // end loop m_it

  if (m_timersvc) m_timer[iDet][2]->pause();


  return StatusCode::SUCCESS;

}


//////--

StatusCode EFMissingETFromFEBHeader::addFullLArFebEnergyToHelper(TrigEFMissingEtHelper* metHelper,
    DETID detectorID, bool prepare){
  ATH_MSG_DEBUG( "addFullLArFebEnergyToHelper(DETID=" << detectorID << ")" );

  int iDet=0;
  //CaloSampling::CaloSample samp = CaloSampling::PreSamplerB;
  switch (detectorID) {
    case TTEM:
      iDet=0;
      //samp = CaloSampling::PreSamplerB;
      break;
    case TTHEC:
      iDet=1;
      //samp = CaloSampling::HEC0;
      break;
    case FCALEM:
      iDet=3;
      //samp = CaloSampling::FCAL0;
      break;
    case FCALHAD:
      iDet=3;
      //samp = CaloSampling::FCAL1;
      break;
    default:
      break;
  }

  // Prepare data, load collections
  if (m_timersvc) m_timer[iDet][1]->resume();
  StatusCode sc = m_data->LoadFullCollections(m_iFebBegin,m_iFebEnd, detectorID, prepare);
  if (sc.isFailure()) return sc;
  if (m_timersvc) m_timer[iDet][1]->pause();

  // Algorithmic time
  if (m_timersvc) m_timer[iDet][2]->resume();

  // check errors from byte-stream conversion
  bool BSerrors = false;
  if (m_data->report_error()) BSerrors = true;

  for(m_Febit=m_iFebBegin ;m_Febit!=m_iFebEnd; ++m_Febit){ // Main Loop
    // possibly do some noise thresholding

    bool used=false;
    int febid = (*m_Febit)->getFebId();
    int idit = 0;
    int iditend = m_usedfeb->size();
    for(;idit!=iditend; ++idit){
      if(febid == (*m_usedfeb)[idit] ){
        used = true;
        idit++;
        break;
      }
    }
    m_usedfeb->push_back(febid);
    if(!used){
      // find the sampling
      HWIdentifier febid2(febid);

      Identifier offChId;
      int ichannel=0;
      do {
        HWIdentifier onlChId = m_LArOnlineID->channel_Id(febid2,ichannel);
        offChId = m_cablingSvc->cnvToIdentifier(onlChId);
        ichannel++;
        if (ichannel>127) {
          ATH_MSG_ERROR( "not connected channel found for this FEB: " << febid );
          return StatusCode::RECOVERABLE;
        }
      } while(!offChId.is_valid());

      int caloSamp = m_CaloCell_ID->sampling(offChId);
      HWIdentifier modId = m_cablingSvc->getReadoutModuleID(febid2); //ReadOutModuleId
      int subdet = m_larROModSvc.em_hec_fcal(modId);  // em=0, hec=1, fcal=2
      int caloId = m_larROModSvc.barrel_ec(modId);    // barrel:0 or EndCAp:1
      //       int posneg =  m_larROModSvc.pos_neg(modId);      // A(pos:1) or C(neg:0)

      unsigned char k=0;
      bool doHackForHEC=false;
      switch (subdet) {
        case 0: // EM
          k = 4*caloId + caloSamp;
          break;
        case 1: // HEC
          k = 8 + caloSamp;
          doHackForHEC=true;
          break;
        case 2: // FCAL
          k = 20 + caloSamp;
          break;
        default:
          ATH_MSG_FATAL( "Unknown subdetector!" );
          return StatusCode::FAILURE;
      }

      TrigEFMissingEtComponent* metComp = metHelper->GetComponent(k);
      if (metComp==0) {
        ATH_MSG_FATAL( "Cannot find calo sampling " << (int)k
                       << ". caloId=" << caloId << ", caloSamp=" << caloSamp
                       << ", subdet=" << subdet << " (febid=" << febid
                       << ", offChId=" << offChId << ")" );
        return StatusCode::FAILURE;
      }
      if (metComp->m_skip) {
        ATH_MSG_DEBUG( "Skipping L.Ar cells from DETID = " << detectorID
                        << ", i.e. " << metComp->m_name );
        break;
      }
      else if ( metComp->m_status & m_maskProcessed ) // already done
        break;

//       // detect error
//       if (error) {
// 	metComp->m_status |= m_maskErrFEB | m_maskCompErrors;
//       }

      double Ex = (*m_Febit)->getFebEx();
      double Ey = (*m_Febit)->getFebEy();
      double Ez = (*m_Febit)->getFebEz();
      double Et = sqrt(Ex*Ex + Ey*Ey);

      metComp->m_ex -= Ex;
      metComp->m_ey -= Ey;
      metComp->m_ez -= Ez;
      metComp->m_sumEt += Et;
      metComp->m_sumE += sqrt(Et*Et + Ez*Ez);
      metComp->m_usedChannels += 1;
      metComp->m_sumOfSigns += 1; // always positive energy
      metComp->m_status |= m_maskProcessing;
      if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
      if (doHackForHEC) { // all HEC FEBs appear to belong to the first sampling
        for (unsigned char jj=0; jj<4; ++jj) {
          metComp = metHelper->GetComponent(8+jj);
          if (metComp) metComp->m_status |= m_maskProcessing;
        }
      }
    }

  } // end loop m_it

  if (m_timersvc) m_timer[iDet][2]->pause();

  return StatusCode::SUCCESS;

}




//////--

StatusCode EFMissingETFromFEBHeader::addFullTileFebEnergyToHelper(TrigEFMissingEtHelper* metHelper,
    DETID detectorID, bool /*prepare*/ ){
  ATH_MSG_DEBUG( "addFullLArFebEnergyToHelper(DETID=" << detectorID << ")" );

/*  int iDet=0;
  CaloSampling::CaloSample samp = CaloSampling::PreSamplerB;
  switch (detectorID) {
    case TILE:
      iDet=0;
      samp = CaloSampling::TileBar0;
      break;
    default:
      break;
  }
*/
  // Get the Tile part of the Calo info
  TileL2Container::const_iterator draw_it, draw_it_beg, draw_it_end;
  ATH_MSG_DEBUG( "Tile part" );
  StatusCode sc = m_data->LoadFullCollections(draw_it_beg, draw_it_end );
  if (sc.isFailure()) return sc;
  draw_it = draw_it_beg;
  for(;draw_it!=draw_it_end;++draw_it){
    int targetComponent = 12; int sumComponent = 5;
    int id = (*draw_it)->identify();
    if( (id & 0x700) == 0x100  ||  (id & 0x700) == 0x200 ) {
      targetComponent = 12; sumComponent = 5;
    }else if( (id & 0x700) == 0x300  ||  (id & 0x700) == 0x400 ) {
      targetComponent = 18; sumComponent = 2;
    }else{
      ATH_MSG_ERROR( "dont know which part this tile drawer belongs to." );
    }
    TrigEFMissingEtComponent* metComp = metHelper->GetComponent(targetComponent);

    double Ex = (*draw_it)->Ex();
    double Ey = (*draw_it)->Ey();
    double Ez = (*draw_it)->Ez();
    double Et = sqrt(Ex*Ex + Ey*Ey);

    metComp->m_ex -= Ex;
    metComp->m_ey -= Ey;
    metComp->m_ez -= Ez;
    metComp->m_sumEt += Et;
    metComp->m_sumE += sqrt(Et*Et + Ez*Ez);
    metComp->m_usedChannels += 1;
    metComp->m_sumOfSigns += 1; // always positive energy

    metComp->m_status |= m_maskProcessing;

     for (unsigned char jj=1; jj<=sumComponent; jj++) {
       metComp = metHelper->GetComponent(targetComponent+jj);
       if (metComp) metComp->m_status |= m_maskProcessing;
     }



  }

  return StatusCode::SUCCESS;

}

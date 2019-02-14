/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromCells.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev  (updated by Diego Casadei)
CREATED:  March 1, 2006      (March 2008)

PURPOSE: Data preparation from Cells

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromCells.h"
#include "TrigMissingEtEvent/TrigMissingET.h"

#include "GaudiKernel/IToolSvc.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"

// noise suppression
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <Identifier/HWIdentifier.h>
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include <math.h>


EFMissingETFromCells::EFMissingETFromCells(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  EFMissingETBaseTool(type, name, parent)
// , m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault")
{

  declareProperty("DoCellNoiseSuppression", m_doCellNoiseSupp = false ,"switch on/off cell noise suppression");
  declareProperty("useFullCollection",      m_useFullColl = true ,"switch on/off fast access via LoadFullCollection");
  declareProperty("makeRobustness",         m_makeRobustness = true ,"switch on/off robustness checks");
  declareProperty("CaloNoiseTool",          m_noiseTool, "Tool Handle for noise tool");
  declareProperty("CaloNoiseOneSidedCut",   m_rmsOneSided=3.0, "1-sided noise suppression (unit = rms)");
  declareProperty("CaloNoiseTwoSidedCut",   m_rmsTwoSided=2.0, "2-sided noise suppression (unit = rms)");
  declareProperty("MinCellSNratio",         m_MinCellSNratio, "(vector) min cell |energy|/rms for robustness checks");
  declareProperty("MaxLArQ",                m_MaxLArQ = 4000, "Max Q LAr Cell value");
  declareProperty("MaxTileQ",               m_MaxTileQ = 254, "Max Q Tile Cell value");

  /// NB: Trivial limits are set here.
  ///     Look at TrigEFMissingETConfig.py for actual initialization
  unsigned char N=24; // number of calo samplings
  m_MinCellSNratio.reserve(N);
  for (int i=0; i<N; ++i) {
    m_MinCellSNratio.push_back(3.0);
  }
  m_maxThreshold=0;

  m_data = NULL;

  m_fextype = FexType::CELL;

}

EFMissingETFromCells::~EFMissingETFromCells()
{
}

StatusCode EFMissingETFromCells::initialize()
{
  ATH_MSG_DEBUG( " In initialize() of EFMissingETFromCells " );
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


  /// noise suppression
  if(m_doCellNoiseSupp!=0){
    if(m_noiseTool.retrieve().isFailure()){
      ATH_MSG_FATAL( "Unable to find CaloNoiseTool" );
      return StatusCode::FAILURE;
    }

  }

  m_maxThreshold=m_rmsOneSided;
  if (m_rmsOneSided<fabsf(m_rmsTwoSided)) m_maxThreshold=fabsf(m_rmsTwoSided);

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromCells::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromCells::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromCells::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer * /* caloCluster */ , const xAOD::JetContainer  * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "this is EFMissingETFromCells::execute()" );

  if(!m_glob_timer) {
    ATH_MSG_WARNING( "ERROR: cannot get global timer!" );
  }

  if(m_timersvc && m_glob_timer){
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



  ATH_MSG_DEBUG( "EFMissingETFromCells::execute() is calling addAllCellsToHelper()" );
  StatusCode sc = addAllCellsToHelper(metHelper);
  if(sc.isFailure()){
    ATH_MSG_ERROR( " Failure of addAllCellsToHelper" );
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

   if(m_timersvc && m_glob_timer)
    m_glob_timer->stop(); // total time
  } // if timing service

  return StatusCode::SUCCESS;
}

//////--


StatusCode EFMissingETFromCells::addAllCellsToHelper(TrigEFMissingEtHelper* metHelper){
  ATH_MSG_DEBUG( "EFMissingETFromCells::addAllCellsToHelper()" );

  if(m_useFullColl) { // use LoadFullCollections in TrigDataAccess

    ATH_MSG_DEBUG( "EFMissingETFromCells::addAllCellsToHelper() is using LoadFullCollections" );

    /** Load TTEM */
    StatusCode sc = addFullLArCellsToHelper(metHelper, TTEM, true);
    if (sc.isFailure()) return sc;

    /** Load TTHEC */
    sc = addFullLArCellsToHelper(metHelper, TTHEC, true);
    if (sc.isFailure()) return sc;

    /** Load FCALHAD */
    sc = addFullLArCellsToHelper(metHelper, FCALHAD, true);
    if (sc.isFailure()) return sc;

    /** Load FCALEM */
    sc = addFullLArCellsToHelper(metHelper, FCALEM, true);
    if (sc.isFailure()) return sc;

    /** Load Tile */
    sc = addFullTileCellsToHelper(metHelper, 0, true);
    if (sc.isFailure()) return sc;

  } else { // use RegionSelector() to load collections (slower!)

    ATH_MSG_DEBUG( "EFMissingETFromCells::addAllCellsToHelper() is using RegionSelector" );
    const char* samplNames[12] = {"TTEM",  "TTEM",  "TTEM",  "TTEM",
				  "TTHEC", "TTHEC", "TTHEC", "TTHEC",
				  "TILE",
				  "FCALEM",
				  "FCALHAD", "FCALHAD"}; // DEBUG

    const DETID samplDetID[12] = {TTEM,  TTEM,  TTEM,  TTEM,
				  TTHEC, TTHEC, TTHEC, TTHEC,
				  TILE,
				  FCALEM,
				  FCALHAD, FCALHAD};
    const int   sampling[12]   = {0, 1, 2, 3, // TTEM
				  0, 1, 2, 3, // TTHEC
				  0,          // Tile
				  0,          // FCalEM
				  0,1};       // FCalHad
    const double phimin = -M_PI;    const double phimax =  M_PI;
    const double etamin = -4.8;     const double etamax =  4.8;

    for (int i=0; i<12; ++i) { // loop over samplings
      if (i!=8) { // LAr

        ATH_MSG_DEBUG( "EFMissingETFromCells::addLArCellsToHelper(" << samplNames[i] << ", " << sampling[i] << ")" );

	StatusCode sc = addLArCellsToHelper(etamin, etamax, phimin, phimax, metHelper,
				 samplDetID[i], sampling[i], true);
	if (sc.isFailure()) return sc;

      } else { // Tile

        ATH_MSG_DEBUG( "EFMissingETFromCells::addTileCellsToHelper(" << samplNames[i] << ", " << sampling[i] << ")" );

	StatusCode sc = addTileCellsToHelper(etamin, etamax, 0., 2*phimax, metHelper,
				  samplDetID[i], sampling[i], true);
	if (sc.isFailure()) return sc;
      }
    }
  }

  ATH_MSG_DEBUG( metHelper->getFormattedValues() );

  return StatusCode::SUCCESS;

}

//////--

StatusCode EFMissingETFromCells::addLArCellsToHelper(double etamin, double etamax,
    double phimin, double phimax,
    TrigEFMissingEtHelper* metHelper,
    DETID detectorID, int sampling,
    bool prepare){
  ATH_MSG_DEBUG( "EFMissingETFromCells::addLArCellsToHelper(detectorID=" << detectorID << ", sampling=" << sampling << ")" );

  int iDet=0;
  switch (detectorID) {
    case TTEM:
      iDet=0;
      break;
    case TTHEC:
      iDet=1;
      break;
    case FCALEM:
    case FCALHAD:
      iDet=3;
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
  StatusCode sc = m_data->LoadCollections(m_iBegin,m_iEnd, sampling, prepare);
  if (sc.isFailure()) return sc;
  if (m_timersvc) m_timer[iDet][1]->pause();

  // Algorithmic time
  if (m_timersvc) m_timer[iDet][2]->resume();

  // check errors from byte-stream conversion
  bool BSerrors = false;
  if (m_data->report_error()) BSerrors = true;

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // 0. get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    // 1. find component
    TrigEFMissingEtComponent *metComp = metHelper->GetComponent( cDDE->getSampling() );
    if (metComp==0) {
      ATH_MSG_FATAL( "Cannot find calo sampling!" );
      return StatusCode::FAILURE;
    }

    if (metComp->m_skip) {
      ATH_MSG_DEBUG( "Skipping L.Ar cells from (detID = "<< detectorID << ", sampling = " << sampling << "), i.e. " << metComp->m_name );
      break;
    }
    else if ( metComp->m_status & m_maskProcessed ) // already done
      break;

    metComp->m_status |= m_maskProcessing; // go to "processing" state

    // 2. get cell energy
    double E = (*m_it)->e();
    // 3. noise suppression
    if (m_doCellNoiseSupp) {
       double rms = m_noiseTool->getNoise(*m_it, ICalorimeterNoiseTool::TOTALNOISE);
       if (m_rmsOneSided > fabsf(m_rmsTwoSided)) {
          // 1-sided cut
          if (E < m_rmsOneSided * rms) continue;
       } else if (m_rmsOneSided < -fabsf(m_rmsTwoSided)) {
          // mixed case
          if (E < m_rmsOneSided * rms ||     // 1-sided cut
              fabs(E) < m_rmsTwoSided * rms) // 2-sided cut
             continue;
       } else {
          // 2-sided cut: never use this online!
          if (fabs(E) < m_rmsTwoSided * rms) continue;
       }
    }
    // 4. update component
    double et  = E * cDDE->sinTh();
    double ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    metComp->m_ex -= et*cosPhi;
    metComp->m_ey -= et*sinPhi;
    metComp->m_ez -= ez;
    metComp->m_sumEt += et;
    metComp->m_sumE += E;
    metComp->m_usedChannels += 1;
    if( (*m_it)->quality() > m_MaxLArQ ) metComp->m_sumBadEt += et;
    if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,E) + 0.5));

    // 5. auxiliary quantities for robustness checks
    if (!m_makeRobustness) continue;
    if (!m_doCellNoiseSupp || (m_doCellNoiseSupp &&
                               m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {

       if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] *
           m_noiseTool->getNoise(*m_it, ICalorimeterNoiseTool::TOTALNOISE))
          continue;
	    }

    float time = (*m_it)->time() * 1e-3;  // ns
    float quality = (*m_it)->quality();
    if (time < metComp->m_minTime) metComp->m_minTime = time;
    if (time > metComp->m_maxTime) metComp->m_maxTime = time;
    if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
    if (E < metComp->m_minE) metComp->m_minE = E;
    if (E > metComp->m_maxE) metComp->m_maxE = E;

  } // end of Main Loop

  if (m_timersvc) m_timer[iDet][2]->pause();

  return StatusCode::SUCCESS;

}

//////--

StatusCode EFMissingETFromCells::addTileCellsToHelper(double etamin, double etamax,
    double phimin, double phimax,
    TrigEFMissingEtHelper* metHelper,
    DETID detectorID, int sampling,
    bool /*prepare*/ ){
  ATH_MSG_DEBUG( "EFMissingETFromCells::addTileCellsToHelper(detectorID=" << detectorID << ", sampling=" << sampling << ")" );

  int iDet=2;

  // Get detector offline ID's for Collections
  if (m_timersvc) m_timer[iDet][0]->resume();
  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, detectorID);
  if (m_timersvc) m_timer[iDet][0]->pause();

  // Prepare data, load collections
  for( unsigned int iR=0; iR!=m_data->TileContSize();++iR ){ // loop over samplings

    // Time to access Collection
    if (m_timersvc) m_timer[iDet][1]->resume();
    StatusCode sc = m_data->LoadCollections(m_itBegin,m_itEnd, iR, !iR);
    if (sc.isFailure()) return sc;
    if (m_timersvc) m_timer[iDet][1]->pause();

    // Algorithmic time
    if (m_timersvc) m_timer[iDet][2]->resume();

    // check errors from byte-stream conversion
    bool BSerrors = false;
    if (m_data->report_error()) BSerrors = true;

    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { // loop over cells
      // 0. get calo descriptor
      const CaloDetDescrElement* cDDE = (*m_itt)->caloDDE();
      if (cDDE==0) {
        ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_itt)->ID() );
        continue;
      }

      // 1. find component
      TrigEFMissingEtComponent *metComp = metHelper->GetComponent( cDDE->getSampling() );
      if (metComp==0) {
        ATH_MSG_FATAL( "Cannot find calo sampling!" );
        return StatusCode::FAILURE;
      }

      if (metComp->m_skip) {
        ATH_MSG_WARNING( "Skipping Tile cells from " << metComp->m_name );
        break;
      }
      else if ( metComp->m_status & m_maskProcessed ) // already done
        break;

      metComp->m_status |= m_maskProcessing; // go to "processing" state

      double E   = (*m_itt)->e();
      // 2. noise suppression
      if (m_doCellNoiseSupp) {
         if (m_rmsOneSided > fabsf(m_rmsTwoSided)) {
            // 1-sided cut
            if (E < 0 || // the following call implements a 2-sided cut
                ! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsOneSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE)
                ) continue;
         } else if (m_rmsOneSided < -fabsf(m_rmsTwoSided)) {
            // mixed case
            if (   // 1-sided cut
                (E<0 && m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsOneSided),
                                                    ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                                    ICalorimeterNoiseTool::TOTALNOISE))
                || // 2-sided cut
                ! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsTwoSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE)
                   )
               continue;
         } else {
            // 2-sided cut: never use this online!
            if (! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsTwoSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE))
               continue;
         }
      }

      // 3. update component
      double et  = E * cDDE->sinTh();
      double ez  = E * cDDE->cosTh();
      double sinPhi = cDDE->sinPhi();
      double cosPhi = cDDE->cosPhi();
      metComp->m_ex -= et*cosPhi;
      metComp->m_ey -= et*sinPhi;
      metComp->m_ez -= ez;
      metComp->m_sumEt += et;
      metComp->m_sumE += E;
      metComp->m_usedChannels += 1;
      int tileQuality = std::max((*m_itt)->qual1(), (*m_itt)->qual2());
      if( tileQuality > m_MaxTileQ ) metComp->m_sumBadEt += et;
      if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
      metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,E) + 0.5));

      // 4. auxiliary quantities for robustness checks
      if (!m_makeRobustness) continue;
      if (!m_doCellNoiseSupp || (m_doCellNoiseSupp &&
                                 m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {
         double sigma = m_noiseTool->getEffectiveSigma( *m_itt,
                                                        ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                                        ICalorimeterNoiseTool::TOTALNOISE);
         if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] * sigma) continue;
      }

      float time = (*m_itt)->time() * 1e-3;  // ns
      float quality = (*m_itt)->quality();
      if (time < metComp->m_minTime) metComp->m_minTime = time;
      if (time > metComp->m_maxTime) metComp->m_maxTime = time;
      if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
      if (E < metComp->m_minE) metComp->m_minE = E;
      if (E > metComp->m_maxE) metComp->m_maxE = E;

    } // end loop over cells

  } // end loop over samplings

  if (m_timersvc) m_timer[iDet][2]->pause();

  return StatusCode::SUCCESS;

}

//////--

StatusCode EFMissingETFromCells::addFullLArCellsToHelper(TrigEFMissingEtHelper* metHelper,
    DETID detectorID, bool prepare){

  ATH_MSG_DEBUG( "EFMissingETFromCells::addFullLArCellsToHelper(detectorID=" << detectorID << ")" );

  int iDet=0;
  switch (detectorID) {
    case TTEM:
      iDet=0;
      break;
    case TTHEC:
      iDet=1;
      break;
    case FCALEM:
    case FCALHAD:
      iDet=3;
      break;
    default:
      break;
  }

  // Prepare data, load collections
  if (m_timersvc) m_timer[iDet][1]->resume();
  StatusCode sc = m_data->LoadFullCollections(m_iBegin,m_iEnd, detectorID, prepare);
  if (sc.isFailure()) return sc;
  if (m_timersvc) m_timer[iDet][1]->pause();

  // Algorithmic time
  if (m_timersvc) m_timer[iDet][2]->resume();

  // check errors from byte-stream conversion
  bool BSerrors = false;
  if (m_data->report_error()) BSerrors = true;

  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // 0. get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    // 1. find component
    TrigEFMissingEtComponent *metComp = metHelper->GetComponent( cDDE->getSampling() );
    if (metComp==0) {
      ATH_MSG_FATAL( "Cannot find calo sampling!" );
      return StatusCode::FAILURE;
    }

    if (metComp->m_skip) {
      ATH_MSG_DEBUG( "Skipping L.Ar cells from detID = "<< detectorID << ", i.e. " << metComp->m_name );
      break;
    }
    else if ( metComp->m_status & m_maskProcessed ) // already done
      break;

    metComp->m_status |= m_maskProcessing; // go to "processing" state

    // 2. get cell energy
    double E = (*m_it)->e();
    // 3. noise suppression
    if (m_doCellNoiseSupp) {
       double rms = m_noiseTool->getNoise(*m_it, ICalorimeterNoiseTool::TOTALNOISE);
       if (m_rmsOneSided > fabsf(m_rmsTwoSided)) {
          // 1-sided cut
          if (E < m_rmsOneSided * rms) continue;
       } else if (m_rmsOneSided < -fabsf(m_rmsTwoSided)) {
          // mixed case
          if (E < m_rmsOneSided * rms ||     // 1-sided cut
              fabs(E) < m_rmsTwoSided * rms) // 2-sided cut
             continue;
       } else {
          // 2-sided cut: never use this online!
          if (fabs(E) < m_rmsTwoSided * rms) continue;
       }
    }
    // 4. update component
    double et  = E * cDDE->sinTh();
    double ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    metComp->m_ex -= et*cosPhi;
    metComp->m_ey -= et*sinPhi;
    metComp->m_ez -= ez;
    metComp->m_sumEt += et;
    metComp->m_sumE += E;
    metComp->m_usedChannels += 1;
    if( (*m_it)->quality() > m_MaxLArQ ) metComp->m_sumBadEt += et;
    if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,E) + 0.5));

    // 5. auxiliary quantities for robustness checks
    if (!m_makeRobustness) continue;
    if (!m_doCellNoiseSupp || (m_doCellNoiseSupp &&
                               m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {
       if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] *
           m_noiseTool->getNoise(*m_it, ICalorimeterNoiseTool::TOTALNOISE))
          continue;
    }
    float time = (*m_it)->time() * 1e-3;  // ns
    float quality = (*m_it)->quality();
    if (time < metComp->m_minTime) metComp->m_minTime = time;
    if (time > metComp->m_maxTime) metComp->m_maxTime = time;
    if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
    if (E < metComp->m_minE) metComp->m_minE = E;
    if (E > metComp->m_maxE) metComp->m_maxE = E;
  } // end loop

  if (m_timersvc) m_timer[iDet][2]->pause();

  return StatusCode::SUCCESS;

}

//////--

StatusCode EFMissingETFromCells::addFullTileCellsToHelper(TrigEFMissingEtHelper* metHelper,
    int sampling, bool /*prepare*/ ){
  ATH_MSG_DEBUG( "EFMissingETFromCells::addFullTileCellsToHelper(sampling=" << sampling << ")" );

  int iDet=2;

  // Prepare data, load collections
  for( unsigned int iR=0; iR!=m_data->TileFullContSize();++iR ){ // loop over samplings

    // Time to access Collection
    if (m_timersvc) m_timer[iDet][1]->resume();
    StatusCode sc = m_data->LoadFullCollections(m_itBegin,m_itEnd, iR, !iR);
    if (sc.isFailure()) return sc;
    if (m_timersvc) m_timer[iDet][1]->pause();

    // Algorithmic time
    if (m_timersvc) m_timer[iDet][2]->resume();

    // check errors from byte-stream conversion
    bool BSerrors = false;
    if (m_data->report_error()) BSerrors = true;

    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { // loop over cells
      // 0. get calo descriptor
      const CaloDetDescrElement* cDDE = (*m_itt)->caloDDE();
      if (cDDE==0) {
        ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_itt)->ID() );
        continue;
      }

      // 1. find component
      // TrigEFMissingEtComponent *metComp = metHelper->GetComponent( CaloSampling::getSampling( *(*m_itt) ) );
      TrigEFMissingEtComponent *metComp = metHelper->GetComponent( (*m_itt)->caloDDE()->getSampling() );
      if (metComp==0) {
        ATH_MSG_FATAL( "Cannot find calo sampling!" );
        return StatusCode::FAILURE;
      }
      if (metComp->m_skip) {
        ATH_MSG_WARNING( "Skipping Tile cells from " << metComp->m_name );
        break;
      }
      else if ( metComp->m_status & m_maskProcessed ) // already done
        break;

      metComp->m_status |= m_maskProcessing; // go to "processing" state

      double E   = (*m_itt)->e();
      // 2. noise suppression
      if (m_doCellNoiseSupp) {
         if (m_rmsOneSided > fabsf(m_rmsTwoSided)) {
            // 1-sided cut
            if (E < 0 || // the following call implements a 2-sided cut
                ! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsOneSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE)
                ) continue;
         } else if (m_rmsOneSided < -fabsf(m_rmsTwoSided)) {
            // mixed case
            if (   // 1-sided cut
                (E<0 && m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsOneSided),
                                                    ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                                    ICalorimeterNoiseTool::TOTALNOISE))
                || // 2-sided cut
                ! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsTwoSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE)
                   )
               continue;
         } else {
            // 2-sided cut: never use this online!
            if (! m_noiseTool->isEOverNSigma( *m_itt, fabsf(m_rmsTwoSided),
                                              ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                              ICalorimeterNoiseTool::TOTALNOISE))
               continue;
         }
      }

      // 3. update component
      double et  = E * cDDE->sinTh();
      double ez  = E * cDDE->cosTh();
      double sinPhi = cDDE->sinPhi();
      double cosPhi = cDDE->cosPhi();
      metComp->m_ex -= et*cosPhi;
      metComp->m_ey -= et*sinPhi;
      metComp->m_ez -= ez;
      metComp->m_sumEt += et;
      metComp->m_sumE += E;
      metComp->m_usedChannels += 1;
      int tileQuality = std::max((*m_itt)->qual1(), (*m_itt)->qual2());
      if( tileQuality > m_MaxTileQ ) metComp->m_sumBadEt += et;
      if (BSerrors) metComp->m_status |= m_maskErrBSconv; // | m_maskCompErrors;
      metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,E) + 0.5));

      // 4. auxiliary quantities for robustness checks
      if (!m_makeRobustness) continue;
      if (!m_doCellNoiseSupp || (m_doCellNoiseSupp &&
                                 m_MinCellSNratio[cDDE->getSampling()] > m_maxThreshold)) {
         double sigma = m_noiseTool->getEffectiveSigma( *m_itt,
                                                        ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
                                                        ICalorimeterNoiseTool::TOTALNOISE);
         if (fabs(E) < m_MinCellSNratio[cDDE->getSampling()] * sigma) continue;
      }

      float time = (*m_itt)->time() * 1e-3;  // ns
      float quality = (*m_itt)->quality();
      if (time < metComp->m_minTime) metComp->m_minTime = time;
      if (time > metComp->m_maxTime) metComp->m_maxTime = time;
      if (quality > metComp->m_maxQlty) metComp->m_maxQlty = quality;
      if (E < metComp->m_minE) metComp->m_minE = E;
      if (E > metComp->m_maxE) metComp->m_maxE = E;

    } // end loop over cells
  } // end loop over samplings

  if (m_timersvc) m_timer[iDet][2]->pause();

  return StatusCode::SUCCESS;

}


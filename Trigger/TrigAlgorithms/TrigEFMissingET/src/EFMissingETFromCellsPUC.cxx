/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromCellsPUC.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bob Kowalewski, Kenji Hamano
CREATED:  Aug 16, 2019

PURPOSE:  Pile-up fit

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromCellsPUC.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <TMatrixD.h>

#include <cmath>
#include <string>

EFMissingETFromCellsPUC::EFMissingETFromCellsPUC(const std::string& type, 
    const std::string& name, 
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
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

  declareProperty("SubtractPileup", m_subtractpileup = true ,"use fit based pileup subtraction");
  declareProperty("towerWidthInput", m_towerwidthinput = 0.7 ," ");
  declareProperty("EtaRange", m_etarange = 5.0 ,"eta cut");
  declareProperty("aveEclusPU", m_aveecluspu = 10000.0 ,"2016 only - sets scale for variance of masked regions in MeV; not to be interpreted literally as the average PU cluster energy");
  declareProperty("resE", m_rese = 15.81 ,"calo energy resoln in sqrt(MeV)");
  declareProperty("resEfloor", m_resefloor = 50.0 ,"floor for calo energy resoln in MeV");
  declareProperty("nSigma", m_nsigma = 5.0 ,"tower ET significance");
  declareProperty("varRhoScale", m_varrhoscale = 1.0 ,"adjustment factor for weighting rho errors in fit");
  declareProperty("trimFactor", m_trimfactor = 0.90 ,"Fraction of towers used in calculating trimmed mean");

  
  // declare configurables
  /// NB: Trivial limits are set here.
  ///     Look at TrigEFMissingETConfig.py for actual initialization
  unsigned char N=24; // number of calo samplings
  m_MinCellSNratio.reserve(N);
  for (int i=0; i<N; ++i) {
    m_MinCellSNratio.push_back(3.0);
  }
  m_maxThreshold=0;

  m_data = NULL;
  m_cablingSvc = NULL;
  
  m_fextype = FexType::CELL;
  
  m_methelperposition = 3;

  // Determine number of phi & eta bins, and number of towers
  m_nphibins = (TMath::TwoPi()/m_towerwidthinput/2)*2;
  m_netabins = 2* m_etarange/m_towerwidthinput;
  m_ntowers = m_nphibins*m_netabins;

}


EFMissingETFromCellsPUC::~EFMissingETFromCellsPUC()
{
}


StatusCode EFMissingETFromCellsPUC::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFromCellsPUC::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  }


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

    StatusCode sc=toolSvc()->retrieveTool("LArCablingService", m_cablingSvc);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_FATAL( "Failed to retrieve LArCablingService " );
      return StatusCode::FAILURE;
    }
  }

  m_maxThreshold=m_rmsOneSided;
  if (m_rmsOneSided<fabsf(m_rmsTwoSided)) m_maxThreshold=fabsf(m_rmsTwoSided);

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromCellsPUC::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromCellsPUC::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromCellsPUC::finalize()" );

  return StatusCode::SUCCESS;
  
}

StatusCode EFMissingETFromCellsPUC::execute(xAOD::TrigMissingET * /* met */ ,
					    TrigEFMissingEtHelper *metHelper ,
					    const xAOD::CaloClusterContainer * /* caloCluster */, 
					    const xAOD::JetContainer * /*jets*/,
					    const xAOD::TrackParticleContainer * /*trackContainer*/, 
					    const xAOD::VertexContainer * /*vertexContainer*/,
					    const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "called EFMissingETFromCellsPUC::execute()" );

  if(!m_glob_timer) {
    ATH_MSG_WARNING( "ERROR: cannot get global timer!" );
  }

  if(m_timersvc && m_glob_timer) m_glob_timer->start(); // total time

  
  /// fetching the TCPUC component
  TrigEFMissingEtComponent* metComp = nullptr;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch TCPUC component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch TCPUC component!" );
    return StatusCode::FAILURE;
  }
  if(std::string(metComp->m_name).substr(0,2)!="TC"){
    ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the TCPUC component!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );

  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    if(m_timersvc) m_glob_timer->stop(); // total time
    return StatusCode::SUCCESS;
  }

  // set status to Processing
  metComp->m_status |= m_maskProcessing;
  
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch TCPUC component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch TCPUC component!" );
    return StatusCode::FAILURE;
  }
  if(std::string(metComp->m_name).substr(0,2)!="TC"){
    ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the TCPUC component!" );
    return StatusCode::FAILURE;
  }


  // Variables
  double MExEta = 0, MEyEta = 0, MEzEta = 0, MExFull = 0, MEyFull = 0, MEzFull = 0, METEta = 0, MET = 0;

  double sumEtEta = 0, sumEtFull = 0, sumEEta = 0, sumEFull = 0;
  std::vector<double> ExTower(m_ntowers,0), EyTower(m_ntowers,0), EtTower(m_ntowers,0);
  std::vector<double> ExTower0(m_ntowers,0), EyTower0(m_ntowers,0), EtTower0(m_ntowers,0);
  std::vector<double> ExTower1(m_ntowers,0), EyTower1(m_ntowers,0), EtTower1(m_ntowers,0);
  std::vector<double> ExTower2(m_ntowers,0), EyTower2(m_ntowers,0), EtTower2(m_ntowers,0);
  std::vector<double> ExTower3(m_ntowers,0), EyTower3(m_ntowers,0), EtTower3(m_ntowers,0);
  std::vector<double> ExInMask, EyInMask, EtInMask, AreaInMask;

  // Calculate initial energy
  // Loop over TTEM cells
  StatusCode sc = m_data->LoadFullCollections(m_iBegin,m_iEnd, TTEM, true);
  if (sc.isFailure()) return sc;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    //get cell energy
    double E = (*m_it)->e();
    double eta = (*m_it)->eta();
    double phi = (*m_it)->phi();
    double Et  = E * cDDE->sinTh();
    double Ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    double Ex = Et*cosPhi;
    double Ey = Et*sinPhi;

    // noise suppression
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

    // calculate full granularity Ex, Ey
    MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
    // collect clusters into fixed-position towers    
    if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
      MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
      int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins; 
      int binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins;
      int index0 = binEta*m_nphibins + binPhi;
      int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins; 
      int binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins;
      int index1 = binEta1*m_nphibins + binPhi;
      int index2 = binEta *m_nphibins + binPhi1;
      int index3 = binEta1*m_nphibins + binPhi1;
      ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
      ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
      ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
      ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
    }
  } // end loop

  // Loop over TTHEC cells
  sc = m_data->LoadFullCollections(m_iBegin,m_iEnd, TTHEC, true);
  if (sc.isFailure()) return sc;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    //get cell energy
    double E = (*m_it)->e();
    double eta = (*m_it)->eta();
    double phi = (*m_it)->phi();
    double Et  = E * cDDE->sinTh();
    double Ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    double Ex = Et*cosPhi;
    double Ey = Et*sinPhi;

    // noise suppression
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

    // calculate full granularity Ex, Ey
    MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
    // collect clusters into fixed-position towers    
    if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
      MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
      int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
	binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index0 = binEta*m_nphibins + binPhi;
      int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
	binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index1 = binEta1*m_nphibins + binPhi,
	index2 = binEta *m_nphibins + binPhi1,
	index3 = binEta1*m_nphibins + binPhi1;
      ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
      ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
      ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
      ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
    }
  } // end loop

  // Loop over FCALHAD cells
  sc = m_data->LoadFullCollections(m_iBegin,m_iEnd, FCALHAD, true);
  if (sc.isFailure()) return sc;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    //get cell energy
    double E = (*m_it)->e();
    double eta = (*m_it)->eta();
    double phi = (*m_it)->phi();
    double Et  = E * cDDE->sinTh();
    double Ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    double Ex = Et*cosPhi;
    double Ey = Et*sinPhi;

    // noise suppression
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

    // calculate full granularity Ex, Ey
    MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
    // collect clusters into fixed-position towers    
    if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
      MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
      int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
	binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index0 = binEta*m_nphibins + binPhi;
      int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
	binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index1 = binEta1*m_nphibins + binPhi,
	index2 = binEta *m_nphibins + binPhi1,
	index3 = binEta1*m_nphibins + binPhi1;
      ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
      ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
      ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
      ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
    }
  } // end loop

  // Loop over FCALEM cells
  sc = m_data->LoadFullCollections(m_iBegin,m_iEnd, FCALEM, true);
  if (sc.isFailure()) return sc;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) { // Main Loop
    // get calo descriptor
    const CaloDetDescrElement* cDDE = (*m_it)->caloDDE();
    if (cDDE==0) {
      ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_it)->ID() );
      continue;
    }

    //get cell energy
    double E = (*m_it)->e();
    double eta = (*m_it)->eta();
    double phi = (*m_it)->phi();
    double Et  = E * cDDE->sinTh();
    double Ez  = E * cDDE->cosTh();
    double sinPhi = cDDE->sinPhi();
    double cosPhi = cDDE->cosPhi();
    double Ex = Et*cosPhi;
    double Ey = Et*sinPhi;

    // noise suppression
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

    // calculate full granularity Ex, Ey
    MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
    // collect clusters into fixed-position towers    
    if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
      MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
      int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
	binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index0 = binEta*m_nphibins + binPhi;
      int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
	binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	index1 = binEta1*m_nphibins + binPhi,
	index2 = binEta *m_nphibins + binPhi1,
	index3 = binEta1*m_nphibins + binPhi1;
      ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
      ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
      ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
      ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
    }
  } // end loop

  // Loop over TILE cells
  for( unsigned int iR=0; iR!=m_data->TileFullContSize();++iR ){ // loop over samplings
    sc =  m_data->LoadFullCollections(m_itBegin,m_itEnd, iR, !iR);
    if (sc.isFailure()) return sc;

    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) { // loop over cells
      // 0. get calo descriptor
      const CaloDetDescrElement* cDDE = (*m_itt)->caloDDE();
      if (cDDE==0) {
        ATH_MSG_WARNING( "cannot get CaloDetDescrElement from cell " << (*m_itt)->ID() );
        continue;
      }

      // get cell energy
      double E = (*m_itt)->e();
      double eta = (*m_itt)->eta();
      double phi = (*m_itt)->phi();
      double Et  = E * cDDE->sinTh();
      double Ez  = E * cDDE->cosTh();
      double sinPhi = cDDE->sinPhi();
      double cosPhi = cDDE->cosPhi();
      double Ex = Et*cosPhi;
      double Ey = Et*sinPhi;
      
      // noise suppression
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

      // calculate full granularity Ex, Ey
      MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
      // collect clusters into fixed-position towers    
      if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
	MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
	int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
	  binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	  index0 = binEta*m_nphibins + binPhi;
	int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
	  binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
	  index1 = binEta1*m_nphibins + binPhi,
	  index2 = binEta *m_nphibins + binPhi1,
	  index3 = binEta1*m_nphibins + binPhi1;
	ExTower0[index0] += Ex; EyTower0[index0] += Ey; EtTower0[index0] += Et;
	ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
	ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
	ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
      }
    } // end cell loop
  } // end sampling loop


  int iTmax;
  double EtTowerTrimMean = 0, VarEtTower = 0, ptmin_final;
  

  // find median tower energy and calculate sample variance of tower energy using lower 1/2 of distribution
  EtTower = EtTower0;
  std::sort (EtTower.begin(), EtTower.end()); // sorts in ascending order
  int nTrimmed = m_ntowers*(1-m_trimfactor)/2;
  for (int k=nTrimmed; k<m_ntowers-nTrimmed; k++) EtTowerTrimMean += EtTower[k];
  EtTowerTrimMean *= 1.0/(m_ntowers-2*nTrimmed);
  for (int k=0; k<m_ntowers*(1.0+m_trimfactor)/2; k++) { // ignore highest towers
    double term = (EtTowerTrimMean - EtTower[k])*(EtTowerTrimMean - EtTower[k]);
    if (k < m_ntowers*(1.0-m_trimfactor)/2) term *= 2; // replace highest with lowest in variance
    VarEtTower += term;
  }
  VarEtTower *= 1.0/m_ntowers;
  // Set a minimum value to avoid FPEs
  if (VarEtTower < 1.0e6) VarEtTower = 1.0e6;
  ptmin_final = EtTowerTrimMean + m_nsigma*sqrt(VarEtTower);
  
  // find binning that gives largest sumEt from bins above threshold
  std::vector<double> EtMaxSumT(4,0);
  for(unsigned int k=0; k<ExTower0.size(); k++) {
    if ( EtTower0[k]  > ptmin_final ) { EtMaxSumT[0] += EtTower0[k]; }
    if ( EtTower1[k]  > ptmin_final ) { EtMaxSumT[1] += EtTower1[k]; }
    if ( EtTower2[k]  > ptmin_final ) { EtMaxSumT[2] += EtTower2[k]; }
    if ( EtTower3[k]  > ptmin_final ) { EtMaxSumT[3] += EtTower3[k]; }
  }
  std::vector<double>::iterator itmax;
  itmax = std::max_element(EtMaxSumT.begin(), EtMaxSumT.end());
  iTmax = std::distance(EtMaxSumT.begin(), itmax);
  
  // select Tower bins to correspond to iTmax
  if (iTmax == 0) {
    EtTower = EtTower0; ExTower = ExTower0; EyTower = EyTower0;
  } else if (iTmax == 1) {
    EtTower = EtTower1; ExTower = ExTower1; EyTower = EyTower1;
  } else if (iTmax == 2) {
    EtTower = EtTower2; ExTower = ExTower2; EyTower = EyTower2;
  } else if (iTmax == 3) {
    EtTower = EtTower3; ExTower = ExTower3; EyTower = EyTower3;
  }
  // Missing transverse energy from fixed position tower
  METEta = sqrt(MExEta*MExEta + MEyEta*MEyEta); MET = sqrt(MExFull*MExFull + MEyFull*MEyFull);

  // Perform pile-up fit and subtract contributions
  // --------------------------------------------------------------------------------------
  if (m_subtractpileup) {

    for(unsigned int k=0; k<ExTower.size(); k++) {
      if ( EtTower[k] > ptmin_final ) {
	ExInMask.push_back(ExTower[k]); EyInMask.push_back(EyTower[k]); EtInMask.push_back(EtTower[k]);
	AreaInMask.push_back(2*m_etarange/m_netabins*TMath::TwoPi()/m_nphibins);
      }
    }
    
    int nummasks = ExInMask.size();
    if(nummasks > 0) {
      // Form sumEtobs and covEtobs from all towers
      double sumEtobs = 0, varsumEtobs = 0;
      TMatrixD Etobs(2,1), covEtobs(2,2);
      for(unsigned int j = 0; j < ExTower.size(); j++) {
	double E1 = sqrt(ExTower[j]*ExTower[j]+EyTower[j]*EyTower[j]);
	if (E1 < 1.0) continue;
      sumEtobs += E1;
      double E1res2 = m_resefloor*m_resefloor + m_rese*m_rese*E1;
      varsumEtobs += E1res2;
      double cosphi1 = ExTower[j]/E1, sinphi1 = EyTower[j]/E1;
      Etobs[0][0] += E1*cosphi1; Etobs[1][0] += E1*sinphi1;
      covEtobs[0][0] += E1res2*cosphi1*cosphi1;
      covEtobs[1][0] += E1res2*cosphi1*sinphi1;
      covEtobs[1][1] += E1res2*sinphi1*sinphi1;
      }
      covEtobs[0][1] = covEtobs[1][0];
      // record masks, remove towers that are masked from observed quantities
      TMatrixD Etmasked(2,1);
      double arealost = 0;
      //double areatot = (m_etarange*2)*2*M_PI;
      for(int k = 0; k < nummasks; k++) {
	arealost += AreaInMask[k];
	double E1 = sqrt(ExInMask[k]*ExInMask[k]+EyInMask[k]*EyInMask[k]);
	double E1res2 = m_resefloor*m_resefloor + m_rese*m_rese*E1;
	double cosphi1 = ExInMask[k]/E1, sinphi1 = EyInMask[k]/E1;
	Etmasked[0][0] += E1*cosphi1;  Etmasked[1][0] += E1*sinphi1;
	sumEtobs -= E1; varsumEtobs -= E1res2;
	Etobs[0][0] -= E1*cosphi1;  Etobs[1][0] -= E1*sinphi1;
	covEtobs[0][0] -= E1res2*cosphi1*cosphi1;
	covEtobs[1][0] -= E1res2*sinphi1*cosphi1;
	covEtobs[1][1] -= E1res2*sinphi1*sinphi1;
      }  
      covEtobs[0][1] = covEtobs[1][0];
      TMatrixD covEtobsinv(covEtobs); covEtobsinv.Invert();
      
      TMatrixD dXdEab(nummasks,nummasks), dXdEa(nummasks,1);
      std::vector<double> varRhoA;
      for (int k1 = 0; k1<nummasks; k1++) {
	varRhoA.push_back(VarEtTower * m_varrhoscale);
        double ET1inv = 1/EtInMask[k1];
        double cosphi1 = ExInMask[k1]*ET1inv;
        double sinphi1 = EyInMask[k1]*ET1inv;
	dXdEa[k1][0] = -(						\
			 Etobs[0][0]*(covEtobsinv[0][0]*cosphi1+covEtobsinv[1][0]*sinphi1) + \
			 Etobs[1][0]*(covEtobsinv[0][1]*cosphi1+covEtobsinv[1][1]*sinphi1) - \
			 EtTowerTrimMean/varRhoA[k1] );
        for (int k2 = 0; k2<nummasks; k2++) {
          double ET2inv = 1/EtInMask[k2];
          double cosphi2 = ExInMask[k2]*ET2inv;
          double sinphi2 = EyInMask[k2]*ET2inv;
          dXdEab[k1][k2] = (                                            \
                            cosphi1*(covEtobsinv[0][0]*cosphi2+covEtobsinv[1][0]*sinphi2) + \
                            sinphi1*(covEtobsinv[0][1]*cosphi2+covEtobsinv[1][1]*sinphi2) );
          if (k1 == k2) dXdEab[k1][k2] += 1/varRhoA[k1];
        }
      }
      TMatrixD covFit(dXdEab); covFit.Invert(); TMatrixD Evals(covFit*dXdEa);
      TMatrixD ETobscor(Etmasked), ETfitcor(Etmasked);
      for (int k=0; k<nummasks; k++) {
	double cosphi1 = ExInMask[k]/EtInMask[k], sinphi1 = EyInMask[k]/EtInMask[k];
        ETfitcor[0][0] -= Evals[k][0]*cosphi1;
        ETfitcor[1][0] -= Evals[k][0]*sinphi1;
      }
      double METfitcor = sqrt(ETfitcor[0][0]*ETfitcor[0][0]+ETfitcor[1][0]*ETfitcor[1][0]);
      
      ATH_MSG_DEBUG( " METEta = " << METEta << "\t METfitcor = " << METfitcor << "\t MET= " << MET );
      
      ETobscor[0][0]=MExEta; ETobscor[1][0]=MEyEta;
      
      metComp->m_ex = -(float) ETfitcor[0][0];
      metComp->m_ey = -(float) ETfitcor[1][0];  
      // For the rest we add in the default quantities 
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;

      metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET
    
      metComp->m_ex = -(float) ETobscor[0][0];
      metComp->m_ey = -(float) ETobscor[1][0];  
      // For the rest we add in the default quantities 
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;     
      
    } else {
      
      // Just store zero energies for the clusters 
      metComp->m_ex = 0.;
      metComp->m_ey = 0.;
      metComp->m_ey = 0.;
      metComp->m_sumEt = 0.;
      metComp->m_sumE  = 0.;
      metComp->m_usedChannels += 1;
      
      metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET
      
      metComp->m_ex = -MExEta;
      metComp->m_ey = -MEyEta;
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;
      
    }
    
    // --------------------------------------------------------------------------------------
    
    
    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit
    
  } // end container loop.
  
  if(m_timersvc)
    m_glob_timer->stop(); // total time
  
  return StatusCode::SUCCESS;
  
}

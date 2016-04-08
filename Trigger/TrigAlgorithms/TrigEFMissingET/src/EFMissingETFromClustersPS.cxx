/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClustersPS.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bertrand Chapleau, Bob Kowalewski
          Pile-up subtraction by Randall Sobie and Justin Chiu

CREATED:  Feb 7, 2012

PURPOSE:  Updates TrigMissingETHelper using info from topo. clusters
          A modified version of EFMissingETFromClusters with pile-up subtraction

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromClustersPS.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <cmath>
#include <string> 
using namespace std;

EFMissingETFromClustersPS::EFMissingETFromClustersPS(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  declareProperty("SaveUncalibrated", m_saveuncalibrated = false ,"save uncalibrated topo. clusters");
  declareProperty("SubtractPileup", m_subtractpileup = true ,"use ring-based pileup subtraction");
  declareProperty("PileupDebug", m_pileupdebug = false ,"enable pileup subtraction debug statements");
  declareProperty("PileupNumRings", m_pileupnumrings = 10 ,"number of rings to divide calorimeter into");
  declareProperty("PileupNumStdDev", m_pileupnumstddev = 2.0 ,"remove energetic CaloClusters x Std Dev away from mean");
  // m_subtractpileup and m_pileupdebug flags are not implemented in the Cluster (plus pile-up subtraction)
  // standalone module.

  // declare configurables

  _fextype = FexType::TOPO;

  m_methelperposition = 14; 

  //initialization to make coverity happy:
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;

}


EFMissingETFromClustersPS::~EFMissingETFromClustersPS()
{
}


StatusCode EFMissingETFromClustersPS::initialize()
{

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPS::initialize()" << endreq;

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endreq;

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  if(m_saveuncalibrated) m_methelperposition = 9; 

  if(m_saveuncalibrated) m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
   else m_clusterstate = xAOD::CaloCluster_v1::CALIBRATED;


  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersPS::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromClustersPS::finalize()
{
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPS::finalize()" << endreq;

  return StatusCode::SUCCESS;

}

StatusCode EFMissingETFromClustersPS::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */)
{

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPS::execute()" << endreq;

  if(m_timersvc)
    m_glob_timer->start(); // total time

  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = 0;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component
  if (metComp==0) {
    msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endreq;
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name).substr(0,2)!="TC"){
    msg(MSG::ERROR) << "fetched " << metComp->m_name
	     << " instead of the Clusters component!" << endreq;
    return StatusCode::FAILURE;
  }

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "fetched metHelper component \"" << metComp->m_name << "\"" << endreq;


  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    if(m_timersvc) m_glob_timer->stop(); // total time
    return StatusCode::SUCCESS;
  }

  // set status to Processing
  metComp->m_status |= m_maskProcessing;

  msg() << MSG::DEBUG << " Fetch topo cluster component " << endreq;

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

  if (metComp==0) {  msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endreq;  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,2)!="TC"){ msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Clusters component!" << endreq; return StatusCode::FAILURE; }

  for (xAOD::CaloClusterContainer::const_iterator it = caloCluster->begin(); it != caloCluster->end(); ++it ) {

    float phi = (*it)->phi(m_clusterstate);
    float eta = (*it)->eta(m_clusterstate);
    float Et  = (*it)->pt(m_clusterstate);
    float cosPhi, sinPhi;
    sincosf(phi, &sinPhi, &cosPhi);
    float Ex = Et*cosPhi;
    float Ey = Et*sinPhi;
    float Ez = Et*sinhf(eta);
    float E =  (*it)->p4(m_clusterstate).E();  // sqrtf(Et*Et + Ez*Ez);

    metComp->m_ex -= Ex;
    metComp->m_ey -= Ey;
    metComp->m_ez -= Ez;
    metComp->m_sumEt += Et;
    metComp->m_sumE  += E;
    metComp->m_usedChannels += 1;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));
          
   } // end topo. loop -- before PS

  msg() << MSG::DEBUG << " Start pileup subtraction algorithm: " << endreq;

  if (m_subtractpileup) {
     // --------------------
     // Pileup subtraction
     // -------------------
     // Does not store forward- and central-only MET.
     // --------------------
     // Initialization
     // --------------------
     float etaMin = -5.0;
     float etaDelta = 10.0/m_pileupnumrings;
     int *numRingCl = new int[m_pileupnumrings]{};
     int *numRingCl_thresh = new int[m_pileupnumrings]{};
     double *ringE = new double[m_pileupnumrings]{};
     double *ringESq = new double[m_pileupnumrings]{};
     double *ringE_Cl_thresh = new double[m_pileupnumrings]{};
     bool *ringException = new bool[m_pileupnumrings]{};

     msg() << MSG::DEBUG << " Ready for first and second pass " << endreq;

     // --------------------
     // First pass to get average energy and standard deviation
     // --------------------
     for(int ring = 0; ring < m_pileupnumrings; ring++) {
          ringException[ring] = false;
          ringE[ring]=0;
          ringESq[ring]=0;
          ringE_Cl_thresh[ring]=0;
          numRingCl[ring]=0;
          numRingCl_thresh[ring]=0;

          msg() << MSG::DEBUG << " In ring: " << ring << endreq;

         for(xAOD::CaloClusterContainer::const_iterator it_ii = caloCluster->begin(); it_ii != caloCluster->end(); ++it_ii) {
             float eta = (*it_ii)->eta(m_clusterstate);
             float E =  (*it_ii)->p4(m_clusterstate).E(); 
             
             //msg() << MSG::DEBUG << " E = " << E << endreq;
             //msg() << MSG::DEBUG << " eta = " << eta << endreq;


             if(isnan(E)) {
                 msg() << MSG::DEBUG << "isnan E" << endreq;
                 E = 0;
             }
             if(isnan(eta)) {
                 msg() << MSG::DEBUG << "isnan eta" << endreq;
                 eta = 0;
             }
             if(E == 0) { msg() << MSG::DEBUG << "E is zero" << endreq; };
             if(eta == 0) {msg() << MSG::DEBUG << "eta is zero" << endreq; };

             if(E > 0 && eta >= etaMin && eta < etaMin + etaDelta) {
                 numRingCl[ring]++;
                 ringE[ring] += E;
                 ringESq[ring] += E*E;
             }
         }

          msg() << MSG::DEBUG << " After first pass " << ring << endreq;

         // safety check
         double tempEMean;
         double tempESigma;
         if(ringE[ring] == 0 || numRingCl[ring] == 0) {
             msg() << MSG::DEBUG << "zero ring energy or zero number of cl" << endreq;
             ringException[ring] = true;
             tempEMean = 0;
             tempESigma = 0;
         } else {
             tempEMean = ringE[ring] / numRingCl[ring];    
             tempESigma = sqrt( fabs( ringESq[ring] / numRingCl[ring] - tempEMean * tempEMean ) ); // correct std-dev
         };

         //double sqrarg = ringESq[ring] / numRingCl[ring] - tempEMean * tempEMean;
         //if(sqrarg < 0) { msg() << MSG::DEBUG << "argument of sqrt is negative" << endreq; };

     // --------------------
     // Second pass to exclude energetic clusters
     // --------------------
         for(xAOD::CaloClusterContainer::const_iterator it_ii = caloCluster->begin(); it_ii != caloCluster->end(); ++it_ii) {
             float eta = (*it_ii)->eta(m_clusterstate);
             float E = (*it_ii)->p4(m_clusterstate).E(); 

             //msg() << MSG::DEBUG << " E = " << E << endreq;
             //msg() << MSG::DEBUG << " eta = " << eta << endreq;

             if(isnan(E)) {
                 msg() << MSG::DEBUG << "isnan E 2ndpass" << endreq;
                 E = 0;
             }
             if(isnan(eta)) {
                 msg() << MSG::DEBUG << "isnan eta 2ndpass" << endreq;
                 eta = 0;
             }
             if(E == 0) {msg() << MSG::DEBUG << "E is zero 2ndpass" << endreq; };
             if(eta == 0) {msg() << MSG::DEBUG << "eta is zero 2ndpass" << endreq; };

             double threshold = tempEMean + m_pileupnumstddev * tempESigma;
             if(E > 0 && E < threshold && eta >= etaMin && eta < etaMin + etaDelta) {
                 numRingCl_thresh[ring]++;
                 ringE_Cl_thresh[ring] += E;
            }
         }
         etaMin += etaDelta;
         
         msg() << MSG::DEBUG << " After second pass " << ring << endreq;

         
     }
     
     msg() << MSG::DEBUG << " Ready for third pass " << endreq;
     
     // --------------------
     // Third pass to apply correction
     // --------------------
     double ExNew = 0.0;
     double EyNew = 0.0;
     const double PI = 3.141592653589793238463;

     for(xAOD::CaloClusterContainer::const_iterator it_ii = caloCluster->begin(); it_ii != caloCluster->end(); ++it_ii) {
         float phi = (*it_ii)->phi(m_clusterstate);
         float eta = (*it_ii)->eta(m_clusterstate);
         float Et = (*it_ii)->pt(m_clusterstate);
         float cosPhi, sinPhi;
         sincosf(phi, &sinPhi, &cosPhi);
         float Ex = Et*cosPhi;
         float Ey = Et*sinPhi;
         float E = (*it_ii)->p4(m_clusterstate).E(); 
         // Identify which ring the cluster belongs to
         int Iring = -1;
         for(int j = 0; j < m_pileupnumrings; j++) {
             if(eta >= (-5.0+static_cast<double>(j)*etaDelta) && eta < (-5.0+static_cast<double>(j+1)*etaDelta)) Iring = j;
         }
         // Continue only if the cluster is in the allowed eta region
         // Sometimes the second_r and center_mag moments are zero (but the cluster has energy!)
         // Exclude these clusters from being corrected to avoid FPEs
         double second_r;
         double center_mag;
         
         // msg() << MSG::DEBUG << " Fetching second_r & center_mag " << endreq;
         
         (*it_ii)->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r);
         (*it_ii)->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,center_mag);

         
         if(Iring != -1 && second_r != 0 && center_mag != 0) {
      
             // Calculate corrected energy of cluster and the correction factor to be applied to the components
             double clSolidAngle = 4.0 * PI * second_r / ( center_mag * center_mag );
             double etaL = -5.0 + static_cast<double>(Iring)*etaDelta;
             double etaU = etaL + etaDelta;
             //msg() << MSG::INFO << "etaL = " << etaL << endreq;
             //msg() << MSG::INFO << "etaU = " << etaU << endreq;
              double deltaTheta = 2.0 * atan(exp(-etaL)) - 2.0 * atan(exp(-etaU));
             double ringSolidAngle = (2.0 * PI) * deltaTheta;
             double EclNew = E - ringE_Cl_thresh[Iring] * clSolidAngle / ringSolidAngle;
             double corrFactor = 1.0;
             // Only apply the PS to clusters whose original energy is < 0.
             // Also, only apply the PS to clusters if the energy after PS is > 0.
             if(E <= 0 || corrFactor < 0) {
                 corrFactor = 0.0;
             } else {
                 corrFactor = 1.0 - ringE_Cl_thresh[Iring] / E * clSolidAngle / ringSolidAngle;
             }
             // Apply the correction to the components (NB: not the clusters)
             if(EclNew > 0) {
                 ExNew -= Ex * corrFactor;
                 EyNew -= Ey * corrFactor;
             }
         }
     }

     metComp->m_ex = (float) ExNew;
     metComp->m_ey = (float) EyNew;

     delete[] numRingCl;
     delete[] numRingCl_thresh;
     delete[] ringE;
     delete[] ringESq;
     delete[] ringE_Cl_thresh;
     delete[] ringException;

  } // end PS

 // move from "processing" to "processed" state
 metComp->m_status ^= m_maskProcessing; // switch off bit
 metComp->m_status |= m_maskProcessed;  // switch on bit

 if(m_timersvc)
     m_glob_timer->stop(); // total time

 return StatusCode::SUCCESS;
}

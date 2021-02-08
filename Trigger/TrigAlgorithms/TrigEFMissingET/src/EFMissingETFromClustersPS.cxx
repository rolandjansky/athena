/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

  m_fextype = FexType::TOPO;

  m_methelperposition = 18;

  //initialization to make coverity happy:
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;

}


EFMissingETFromClustersPS::~EFMissingETFromClustersPS()
{
}


StatusCode EFMissingETFromClustersPS::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFromClustersPS::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  if(m_saveuncalibrated) m_methelperposition = 13;

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
  ATH_MSG_DEBUG( "called EFMissingETFromClustersPS::finalize()" );

  return StatusCode::SUCCESS;

}

StatusCode EFMissingETFromClustersPS::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "called EFMissingETFromClustersPS::execute()" );

  if(m_timersvc)
    m_glob_timer->start(); // total time

  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = nullptr;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name).substr(0,2)!="TC"){
    ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Clusters component!" );
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

  ATH_MSG_DEBUG( " Fetch topo cluster component " );

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

  if (metComp==0) {  ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,2)!="TC"){ ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Clusters component!" ); return StatusCode::FAILURE; }

  for (const auto clus : *caloCluster) {

    float phi = clus->phi(m_clusterstate);
    float eta = clus->eta(m_clusterstate);
    float Et  = clus->pt(m_clusterstate);
    float cosPhi, sinPhi;
    sincosf(phi, &sinPhi, &cosPhi);
    float Ex = Et*cosPhi;
    float Ey = Et*sinPhi;
    float Ez = Et*sinhf(eta);
    float E =  clus->p4(m_clusterstate).E();  // sqrtf(Et*Et + Ez*Ez);

    metComp->m_ex -= Ex;
    metComp->m_ey -= Ey;
    metComp->m_ez -= Ez;
    metComp->m_sumEt += Et;
    metComp->m_sumE  += E;
    metComp->m_usedChannels += 1;
    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));

   } // end topo. loop -- before PS

  ATH_MSG_DEBUG( " Start pileup subtraction algorithm: " );

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

     ATH_MSG_DEBUG( " Ready for first and second pass " );

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

          ATH_MSG_DEBUG( " In ring: " << ring );

         for (const auto clus : *caloCluster) {
             float eta = clus->eta(m_clusterstate);
             float E =  clus->p4(m_clusterstate).E();

             //ATH_MSG_DEBUG( " E = " << E );
             //ATH_MSG_DEBUG( " eta = " << eta );


             if(isnan(E)) {
                 ATH_MSG_DEBUG( "isnan E" );
                 E = 0;
             }
             if(isnan(eta)) {
                 ATH_MSG_DEBUG( "isnan eta" );
                 eta = 0;
             }
             if(E == 0) { ATH_MSG_DEBUG( "E is zero" ); };
             if(eta == 0) {ATH_MSG_DEBUG( "eta is zero" ); };

             if(E > 0 && eta >= etaMin && eta < etaMin + etaDelta) {
                 numRingCl[ring]++;
                 ringE[ring] += E;
                 ringESq[ring] += E*E;
             }
         }

          ATH_MSG_DEBUG( " After first pass " << ring );

         // safety check
         double tempEMean;
         double tempESigma;
         if(ringE[ring] == 0 || numRingCl[ring] == 0) {
             ATH_MSG_DEBUG( "zero ring energy or zero number of cl" );
             ringException[ring] = true;
             tempEMean = 0;
             tempESigma = 0;
         } else {
             tempEMean = ringE[ring] / numRingCl[ring];
             tempESigma = sqrt( fabs( ringESq[ring] / numRingCl[ring] - tempEMean * tempEMean ) ); // correct std-dev
         };

         //double sqrarg = ringESq[ring] / numRingCl[ring] - tempEMean * tempEMean;
         //if(sqrarg < 0) { ATH_MSG_DEBUG( "argument of sqrt is negative" ); };

     // --------------------
     // Second pass to exclude energetic clusters
     // --------------------
         for (const auto clus : *caloCluster) {
             float eta = clus->eta(m_clusterstate);
             float E = clus->p4(m_clusterstate).E();

             //ATH_MSG_DEBUG( " E = " << E );
             //ATH_MSG_DEBUG( " eta = " << eta );

             if(isnan(E)) {
                 ATH_MSG_DEBUG( "isnan E 2ndpass" );
                 E = 0;
             }
             if(isnan(eta)) {
                 ATH_MSG_DEBUG( "isnan eta 2ndpass" );
                 eta = 0;
             }
             if(E == 0) {ATH_MSG_DEBUG( "E is zero 2ndpass" ); };
             if(eta == 0) {ATH_MSG_DEBUG( "eta is zero 2ndpass" ); };

             double threshold = tempEMean + m_pileupnumstddev * tempESigma;
             if(E > 0 && E < threshold && eta >= etaMin && eta < etaMin + etaDelta) {
                 numRingCl_thresh[ring]++;
                 ringE_Cl_thresh[ring] += E;
            }
         }
         etaMin += etaDelta;

         ATH_MSG_DEBUG( " After second pass " << ring );


     }

     ATH_MSG_DEBUG( " Ready for third pass " );

     // --------------------
     // Third pass to apply correction
     // --------------------
     double ExNew = 0.0;
     double EyNew = 0.0;
     const double PI = 3.141592653589793238463;

     for (const auto clus : *caloCluster) {
         float phi = clus->phi(m_clusterstate);
         float eta = clus->eta(m_clusterstate);
         float Et = clus->pt(m_clusterstate);
         float cosPhi, sinPhi;
         sincosf(phi, &sinPhi, &cosPhi);
         float Ex = Et*cosPhi;
         float Ey = Et*sinPhi;
         float E = clus->p4(m_clusterstate).E();
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

         // ATH_MSG_DEBUG( " Fetching second_r & center_mag " );

         clus->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r);
         clus->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,center_mag);


         if(Iring != -1 && second_r != 0 && center_mag != 0) {

             // Calculate corrected energy of cluster and the correction factor to be applied to the components
             double clSolidAngle = 4.0 * PI * second_r / ( center_mag * center_mag );
             double etaL = -5.0 + static_cast<double>(Iring)*etaDelta;
             double etaU = etaL + etaDelta;
             //ATH_MSG_INFO( "etaL = " << etaL );
             //ATH_MSG_INFO( "etaU = " << etaU );
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

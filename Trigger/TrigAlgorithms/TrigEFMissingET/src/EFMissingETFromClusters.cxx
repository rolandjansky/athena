/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClusters.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bertrand Chapleau, Bob Kowalewski
CREATED:  Feb 7, 2012

PURPOSE:  Updates TrigMissingETHelper using info from topo. clusters

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromClusters.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromClusters::EFMissingETFromClusters(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  declareProperty("SaveUncalibrated", m_saveuncalibrated = false ,"save uncalibrated topo. clusters");
  // declare configurables

  m_fextype = FexType::TOPO;

  m_methelperposition = 18;

  //initialization to make coverity happy:
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;

}


EFMissingETFromClusters::~EFMissingETFromClusters()
{
}


StatusCode EFMissingETFromClusters::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFromClusters::initialize()" );

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


StatusCode EFMissingETFromClusters::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromClusters::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromClusters::finalize()" );

  return StatusCode::SUCCESS;

}

StatusCode EFMissingETFromClusters::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{

  ATH_MSG_DEBUG( "called EFMissingETFromClusters::execute()" );

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

 //--- fetching the topo. cluster component
 double upperlim[4] = {1.5,0,5,-1.5}; double lowerlim[4] = {0,-1.5,1.5,-5};

 for(int i = 0; i < 5; i++) {

  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + i); // fetch Cluster component

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
    float E =  clus->p4(m_clusterstate).E(); //sqrtf(Et*Et + Ez*Ez);


    if(i == 0) {

          metComp->m_ex -= Ex;
          metComp->m_ey -= Ey;
          metComp->m_ez -= Ez;
          metComp->m_sumEt += Et;
          metComp->m_sumE  += E;
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));

    } else if (i > 0) {

       if( eta >= lowerlim[i-1] && eta <= upperlim[i-1]) {
          metComp->m_ex -= Ex;
          metComp->m_ey -= Ey;
          metComp->m_ez -= Ez;
          metComp->m_sumEt += Et;
          metComp->m_sumE  += E;
          metComp->m_usedChannels += 1;
          metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));
       }

     }

   } // end topo. loop.

     // move from "processing" to "processed" state
     metComp->m_status ^= m_maskProcessing; // switch off bit
     metComp->m_status |= m_maskProcessed;  // switch on bit

 } // end container loop.

  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

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

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClusters::initialize()" << endmsg;

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endmsg;

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
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClusters::finalize()" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode EFMissingETFromClusters::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/ )
{

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "called EFMissingETFromClusters::execute()" << endmsg;

  if(m_timersvc)
    m_glob_timer->start(); // total time

  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = 0;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component
  if (metComp==0) {
    msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endmsg;
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name).substr(0,2)!="TC"){
    msg(MSG::ERROR) << "fetched " << metComp->m_name
	     << " instead of the Clusters component!" << endmsg;
    return StatusCode::FAILURE;
  }

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "fetched metHelper component \"" << metComp->m_name << "\"" << endmsg;


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

  if (metComp==0) {  msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endmsg;  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,2)!="TC"){ msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Clusters component!" << endmsg; return StatusCode::FAILURE; }

  for (xAOD::CaloClusterContainer::const_iterator it = caloCluster->begin(); it != caloCluster->end(); ++it ) {

    float phi = (*it)->phi(m_clusterstate);
    float eta = (*it)->eta(m_clusterstate);
    float Et  = (*it)->pt(m_clusterstate);
    float cosPhi, sinPhi;
    sincosf(phi, &sinPhi, &cosPhi);
    float Ex = Et*cosPhi;
    float Ey = Et*sinPhi;
    float Ez = Et*sinhf(eta);
    float E =  (*it)->p4(m_clusterstate).E(); //sqrtf(Et*Et + Ez*Ez);


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

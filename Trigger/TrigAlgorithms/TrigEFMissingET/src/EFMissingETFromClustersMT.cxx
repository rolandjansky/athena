/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClustersMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Kenji Hamano
CREATED:  Feb 8, 2019

PURPOSE:  athenaMT migration

 ********************************************************************/
// Framework includes
#include "AthenaMonitoring/Monitored.h"
#include "GaudiKernel/IToolSvc.h"

#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

// TrigEFMissingET includes
#include "EFMissingETFromClustersMT.h"


#include <cmath>
#include <string>
using namespace std;

EFMissingETFromClustersMT::EFMissingETFromClustersMT(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  base_class(type, name, parent)
{
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
}


EFMissingETFromClustersMT::~EFMissingETFromClustersMT()
{
}


StatusCode EFMissingETFromClustersMT::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFromClustersMT::initialize()" );

  if(m_saveuncalibrated) 
  {
    m_metHelperComp = TrigEFMissingEtComponent::TCEM;
    m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
  }
  else 
  {
    m_metHelperComp = TrigEFMissingEtComponent::TCLCW;
    m_clusterstate = xAOD::CaloCluster_v1::CALIBRATED;
  }

  CHECK( m_clustersKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersMT::update(xAOD::TrigMissingET *met ,
					      TrigEFMissingEtHelper *metHelper, const EventContext& ctx ) const
{
  
  ATH_MSG_DEBUG( "called EFMissingETFromClustersMT::execute()" );

  /* This is a bit opaque but necessary to cooperate with how the MET helper 
     and MissingETFromHelper classes work. This will be cleaned up (ATR-19488).
     - @ggallard
   */
  const std::vector<std::string> vComp = {"TCLCWB1", "TCLCWB2", 
                                          "TCLCWE1", "TCLCWE2", 
                                          "TCEMB1", "TCEMB2", 
                                          "TCEME1", "TCEME2",
                                          "Muons" };
  met->defineComponents(vComp);

  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto caloClustersHandle = SG::makeHandle( m_clustersKey, ctx );

  auto loopTimer = Monitored::Timer( "TIME_Loop" );

  TrigEFMissingEtComponent* metComp = nullptr;
  metComp = metHelper->GetComponent(m_saveuncalibrated?  TrigEFMissingEtComponent::TCEM : TrigEFMissingEtComponent::TCLCW); // fetch Cluster component
  if (metComp==0) {
    ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );

  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    return StatusCode::SUCCESS;
  }

  // set status to Processing
  metComp->m_status |= m_maskProcessing;

  //--- fetching the topocluster components
  const std::map<TrigEFMissingEtComponent::Component, std::pair<float, float> > tcComponents = {
            {m_saveuncalibrated ? TrigEFMissingEtComponent::TCEM   : TrigEFMissingEtComponent::TCLCW  , {  20, -20   }},
            {m_saveuncalibrated ? TrigEFMissingEtComponent::TCEMB1 : TrigEFMissingEtComponent::TCLCWB1, { 1.5,   0   }},
            {m_saveuncalibrated ? TrigEFMissingEtComponent::TCEMB2 : TrigEFMissingEtComponent::TCLCWB2, {   0,  -1.5 }},
            {m_saveuncalibrated ? TrigEFMissingEtComponent::TCEME1 : TrigEFMissingEtComponent::TCLCWE1, {   5,   1.5 }},
            {m_saveuncalibrated ? TrigEFMissingEtComponent::TCEME2 : TrigEFMissingEtComponent::TCLCWE2, {-1.5,  -5   }}   };


  for(auto const& [tcComp, etaLimits] : tcComponents){

    switch(tcComp)
    {
      case TrigEFMissingEtComponent::TCEM    :
      case TrigEFMissingEtComponent::TCEMB1  : case TrigEFMissingEtComponent::TCEMB2  :
      case TrigEFMissingEtComponent::TCEME1  : case TrigEFMissingEtComponent::TCEME2  :
      case TrigEFMissingEtComponent::TCLCW   :
      case TrigEFMissingEtComponent::TCLCWB1 : case TrigEFMissingEtComponent::TCLCWB2 :
      case TrigEFMissingEtComponent::TCLCWE1 : case TrigEFMissingEtComponent::TCLCWE2 :
        break;

      default:
        ATH_MSG_ERROR("You are somehow iterating over this non-tc component: " 
                        << TrigEFMissingEtComponent::ComponentToName(tcComp) 
                        << ". This is not okay, because this is supposed to be MET from clusters!");
        return StatusCode::FAILURE;
    }

    metComp = metHelper->GetComponent(tcComp); // fetch Cluster component

    if (metComp==0) {  
      ATH_MSG_ERROR( "Cannot fetch topocluster component " 
                      << TrigEFMissingEtComponent::ComponentToName(tcComp) << "!" );  
      return StatusCode::FAILURE; 
    }

    for (const auto& clus : *caloClustersHandle) 
    {

      float phi = clus->phi();
      float eta = clus->eta();
      float Et  = clus->pt();
      float cosPhi, sinPhi;
      sincosf(phi, &sinPhi, &cosPhi);
      float Ex = Et*cosPhi;
      float Ey = Et*sinPhi;
      float Ez = Et*sinhf(eta);
      float E =  sqrtf(Et*Et + Ez*Ez);

      if (eta < etaLimits.first && eta > etaLimits.second)
      {
        metComp->m_ex -= Ex;
        metComp->m_ey -= Ey;
        metComp->m_ez -= Ez;
        metComp->m_sumEt += Et;
        metComp->m_sumE  += E;
        metComp->m_usedChannels += 1;
        metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));
      }

     } // end for (cluster : topoclusters)

     // move from "processing" to "processed" state
     metComp->m_status ^= m_maskProcessing; // switch off bit
     metComp->m_status |= m_maskProcessed;  // switch on bit
     
  } // end for (tcComp : tcComponents)

  return StatusCode::SUCCESS;

}

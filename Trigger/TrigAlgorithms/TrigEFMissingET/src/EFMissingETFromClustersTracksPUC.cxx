/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

This code should be edited as necessary and then placed in the TrigEFMissingET/src directory

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromClustersTracksPUC.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include <string>

using namespace std; 

EFMissingETFromClustersTracksPUC::EFMissingETFromClustersTracksPUC(const std::string& type, 
    const std::string& name, 
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  // declare the algorithm configurables here
  // declareProperty("PropertyName", m_property=default_value, "Property Description");
  // (*Do* add a property description...)
}


EFMissingETFromClustersTracksPUC::~EFMissingETFromClustersTracksPUC()
{
}


StatusCode EFMissingETFromClustersTracksPUC::initialize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::initialize()" );

  // Standard initialize step

  // Set up the timer service.
  if( service( "TrigTimerSvc", m_timersvc).isFailure() )
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  // This is the position used by tc_lcw.
  // I think probably this system needs a big rework but I've just not had the time to do it
  // Maybe you should make up your own position? I don't know :(
  m_methelperposition = 3;

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersTracksPUC::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromClustersTracksPUC::finalize()
{
  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::finalize()" );

  return StatusCode::SUCCESS;
  
}

StatusCode EFMissingETFromClustersTracksPUC::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */,
    const xAOD::TrackParticleContainer * /*trackContainer*/, const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{
  // The function signature has to be compatible with all of the algorithms used in the package
  // This is unfortunate and liable to change in the future (certainly for athenaMT) but until then just be aware that that's how it goes
  // The main algorithm works out what types the various trigger elements are that have been given to it and then passes them onto these functions

  ATH_MSG_DEBUG( "called EFMissingETFromClustersTracksPUC::execute()" );

  if(m_timersvc)m_glob_timer->start(); // total time

  // The met Helper is used to create the various components of the output trigger MET
  // Again, this is an old design and could be updated but probably won't be until athenaMT
  // You need to write the met into a position in the helper (and a later tool then compiles this into a met value)
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
  }
  else { // skip if processed
    if(m_timersvc) m_glob_timer->stop(); // total time
    return StatusCode::SUCCESS;
  }

  // The mask statuses are involved in making sure that you don't try and run the same calculation multiple times
  // To be honest I *thought* this was handled by the steering so it probably is something we should check...
  // set status to Processing
  metComp->m_status |= m_maskProcessing;
  
  // Now here you put your algorithm
  // Dummy variables to make sense of the last bit
  float ex;
  float ey;
  float ez;
  float sumEt;
  float sumE;
  
  // At the end make sure that you stored your calculated met values in the component
  metComp->m_ex = ex;
  metComp->m_ey = ey;
  metComp->m_ez = ez;
  metComp->m_sumEt = sumEt;
  metComp->m_sumE  = sumE;
  metComp->m_usedChannels += 1;

  // You *can* store bits in other components (either spreading the met over several components or recording extra information that might be useful)
  // However you need to be very sure that the helper is adding up the right bits
  /****************************************************************************************
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET

  metComp->m_ex = -MExEta;
  metComp->m_ey = -MEyEta;
  metComp->m_ez = -MEzEta;
  metComp->m_sumEt = sumEtEta;
  metComp->m_sumE  = sumEEta;
  metComp->m_usedChannels += 1;
  *****************************************************************************************/
  //}
  
  // --------------------------------------------------------------------------------------

  // move from "processing" to "processed" state
  metComp->m_status ^= m_maskProcessing; // switch off bit
  metComp->m_status |= m_maskProcessed;  // switch on bit

 // end container loop.

  if(m_timersvc) m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;

}

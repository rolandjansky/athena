/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/Monitored.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigL2CaloRingerHypoToolMT.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/Monitored.h"

using namespace Monitored;
using namespace TrigCompositeUtils;
using namespace Ringer;

TrigL2CaloRingerHypoToolMT::TrigL2CaloRingerHypoToolMT( const std::string& type, 
                                                        const std::string& name, 
                                                        const IInterface* parent ) 

  : AthAlgTool( type, name, parent ),
    m_selectorTool(),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_decisionId( HLT::Identifier::fromToolName( name ) )
{
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool, "Luminosity Tool" );
}



StatusCode TrigL2CaloRingerHypoToolMT::initialize()  {

  m_selectorTool.setConstantsCalibPath( m_constantsCalibPath ); 
  m_selectorTool.setThresholdsCalibPath( m_thresholdsCalibPath ); 

  if(m_selectorTool.initialize().isFailure())
    return StatusCode::FAILURE;
  
  if (m_lumiBlockMuTool.retrieve().isFailure())
    return StatusCode::FAILURE;
  
  ATH_MSG_INFO("TrigL2CaloRingerHypo initialization completed successfully.");
  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloRingerHypoToolMT::finalize()  {
 
  if (!m_monTool.empty()){
    ATH_MSG_DEBUG("Retrieving monTool");
    ATH_CHECK(m_monTool.retrieve());
  }else{
    ATH_MSG_INFO("No monTool configured. NO MONITORING");
  }

 
  if(m_selectorTool.finalize().isFailure())
    return StatusCode::FAILURE;
  ATH_MSG_INFO("TrigL2CaloRingerHypo finalization completed successfully.");
  return StatusCode::SUCCESS;
}

TrigL2CaloRingerHypoToolMT::~TrigL2CaloRingerHypoToolMT() {}


bool TrigL2CaloRingerHypoToolMT::decideOnSingleObject( const xAOD::TrigRingerRings* ringerShape) const {
  
 
  auto etMon      =  Monitored::Scalar("Et",-100);
  auto etaMon     =  Monitored::Scalar("Eta",-100);
  auto phiMon     =  Monitored::Scalar("Phi",-100);
  auto rnnOutMon  =  Monitored::Scalar("RnnOut",-100);

 
  auto total_time     = Monitored::Timer("TIME_total");
  auto propagate_time = Monitored::Timer("TIME_propagate");
  auto preproc_time   = Monitored::Timer("TIME_preproc");
  auto decide_time    = Monitored::Timer("TIME_decide");
  auto mon = Monitored::Group(m_monTool,etMon,etaMon,phiMon,rnnOutMon,total_time,propagate_time,preproc_time,decide_time);

  total_time.start();

 

  if(m_acceptAll){
    ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
    return true;
  }


  const xAOD::TrigEMCluster *emCluster = 0;
  if(ringerShape){
    emCluster = ringerShape->emCluster();
    if(!emCluster){
      ATH_MSG_WARNING( "There is no link to xAOD::TrigEMCluster into the Ringer object.");
      return false;
    }
  }else{
    ATH_MSG_WARNING( "There is no xAOD::TrigRingerRings link into the rnnOutput object.");
    return false;
  }

  float eta     = std::fabs(emCluster->eta());
  float et      = emCluster->et() / Gaudi::Units::GeV;
  float avgmu   = m_lumiBlockMuTool->averageInteractionsPerCrossing();
  
  etaMon =  emCluster->eta();
  etMon  = emCluster->et();
  phiMon = emCluster->phi();

  if(eta>2.50) eta=2.50;///fix for events out of the ranger

  ///Et threshold
  if(et < m_emEtCut/Gaudi::Units::GeV){
    ATH_MSG_DEBUG( "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut/Gaudi::Units::GeV);
    return false;
  }

  const std::vector<float> rings = ringerShape->rings();
  std::vector<float> refRings(rings.size());
  refRings.assign(rings.begin(), rings.end());
  
  ATH_MSG_DEBUG("Et = "<< et << " Eta = "<<eta << " mu = " << avgmu << "rsize = "<< refRings.size()); 

  auto output = m_selectorTool.calculate( refRings, et, eta, avgmu, propagate_time, preproc_time );
  rnnOutMon = output;
  ATH_MSG_DEBUG(name()<< " generate as NN output " <<  output );
  
  decide_time.start();
  bool accept = m_selectorTool.accept(output, et,eta,avgmu);
  decide_time.stop();

  total_time.stop();
  return accept;

}


StatusCode TrigL2CaloRingerHypoToolMT::decide(  std::vector<RingerInfo>& input )  const {

    for ( auto i: input ) {
        auto objDecision = decideOnSingleObject( i.ringerShape );
        if ( objDecision == true ) {
          addDecisionID( m_decisionId.numeric(), i.decision );
        }
    }
    return StatusCode::SUCCESS;
}



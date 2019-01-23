/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/Monitored.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigL2CaloRingerHypoToolMT.h"

using namespace TrigCompositeUtils;

TrigL2CaloRingerHypoToolMT::TrigL2CaloRingerHypoToolMT( const std::string& type, 
                        const std::string& name, 
                        const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigL2CaloRingerHypoToolMT::initialize()  {

    	///What is the number of discriminators?
  m_nThresholds = m_thresholds.size();

  if((m_etaBins.size() != m_nThresholds) && (m_etBins.size() != m_nThresholds)){
    ATH_MSG_ERROR("Eta/Et list dont match with the number of thesholds found");
    return StatusCode::FAILURE;
  }

  ///Initialize all discriminators
  for(unsigned i=0; i<m_nThresholds; ++i)
  {
    ///Hold the pointer configuration
    try{
      m_cutDefs.push_back(new TrigL2CaloRingerHypoToolMT::CutDefsHelper(m_thresholds[i],m_etaBins[i][0],
                                                                  m_etaBins[i][1], m_etBins[i][0],m_etBins[i][1]));
    }catch(const std::bad_alloc& xa){
      ATH_MSG_ERROR("Can not alloc cutDefs on memory.");
      return StatusCode::FAILURE;
    }
  }///Loop over discriminators
  
  ATH_MSG_INFO("TrigL2CaloRingerHypo initialization completed successfully.");


  return StatusCode::SUCCESS;
}


TrigL2CaloRingerHypoToolMT::~TrigL2CaloRingerHypoToolMT() {}


bool TrigL2CaloRingerHypoToolMT::decideOnSingleObject( const xAOD::TrigRNNOutput* rnnOutput) const {
  
  

  if(m_acceptAll){
    ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
    return true;
  }

  if(!rnnOutput){
    ATH_MSG_WARNING("There is no xAO::TrigRNNOutput into the TriggerElement.");
    return false;
  }

  const xAOD::TrigEMCluster *emCluster = 0;
  const xAOD::TrigRingerRings *ringerShape = rnnOutput->ringer();
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
  if(eta>2.50) eta=2.50;///fix for events out of the ranger
  float output  = rnnOutput->rnnDecision().at(0);


  ///Et threshold
  if(et < m_emEtCut){
    ATH_MSG_DEBUG( "Event reproved by Et threshold. Et = " << et << ", EtCut = " << m_emEtCut);
    return false;
  }

  
  if(m_nThresholds > 0){
    ///Select the correct threshold for each eta/Et region
    for(unsigned i=0; i<m_nThresholds;++i){
      if((et  > m_cutDefs[i]->etmin()) && (et  <= m_cutDefs[i]->etmax())){
        if((eta > m_cutDefs[i]->etamin()) && (eta <= m_cutDefs[i]->etamax())){
          if(output >= m_cutDefs[i]->threshold()){
              ATH_MSG_DEBUG( "Event information:" );
              //ATH_MSG_DEBUG( "   " << m_cutDefs[i]->etmin() << "< Et ("<<et<<") GeV" << " <=" << m_cutDefs[i]->etmax() );
              //ATH_MSG_DEBUG( "   " << m_cutDefs[i]->etamin() << "< |Eta| ("<<eta<<") " << " <=" << m_cutDefs[i]->etamax() );
              //ATH_MSG_DEBUG( "   rnnOutput: " << output <<  " and threshold: " << m_cutDefs[i]->threshold());      
              return true;
	  }else{
              ATH_MSG_DEBUG( "Event reproved by discriminator threshold" );
          }///Threshold condition
          break;
        }///Loop over eta
      }///Loop over et
    }///Loop over cutDefs
  }else{
    ATH_MSG_DEBUG( "There is no discriminator. Event approved by Et threshold.");   
    return true;
  }///protection
  return false;
}


StatusCode TrigL2CaloRingerHypoToolMT::decide(  std::vector<RNNOutInfo>& input )  const {


    for ( auto i: input ) {
        auto objDecision = decideOnSingleObject( i.rnnOut );
      if ( objDecision == true ) {
        addDecisionID( m_decisionId.numeric(), i.decision );
      }
    }
    return StatusCode::SUCCESS;
}



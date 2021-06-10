
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigL2CaloLayersHypoTool.h"


using namespace TrigCompositeUtils;

TrigL2CaloLayersHypoTool::TrigL2CaloLayersHypoTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
}

StatusCode TrigL2CaloLayersHypoTool::initialize()  {

  
  ATH_MSG_INFO("TrigL2CaloLayersHypoTool initialization completed successfully.");
   
 /*
  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );
 */


  return StatusCode::SUCCESS;
}


StatusCode TrigL2CaloLayersHypoTool::decide( std::vector<ITrigL2CaloLayersHypoTool::CaloLayersInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
    addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

bool TrigL2CaloLayersHypoTool::decide( const ITrigL2CaloLayersHypoTool::CaloLayersInfo& input ) const {

  if (m_acceptAll) return true;
  // no clusters no discussion
  if ( !input.clusters ) return false;
  //bool pass=false;

  // get cluster
  const xAOD::TrigEMCluster* pClus = input.clusters->front();
  //m_preSampFrac=m_preSamp=m_monEta=m_monPhi=m_Energy=-9999.0;
    
  if ( !pClus or (pClus->energy()<0.1) or (fabsf(pClus->eta())>2.1) ) {
    ATH_MSG_DEBUG ("No EM cluster in RoI" );
    return false;
  }

  //m_monEta = pClus->eta();
  //m_monPhi = pClus->phi();

  // increment event counter 
  //m_PassedCuts++; //// the ROI at least contais the cluster

  std::vector<double> fracs;
  for(int i=0;i<4;i++){
    fracs.push_back(0);
    fracs[i] = (pClus->energy( ((CaloSampling::CaloSample)i) )+pClus->energy( ((CaloSampling::CaloSample)(i+4)) ))/pClus->energy();
  }
  //m_Energy = pClus->energy();

  if ( fracs[0] > m_EnergyFracCut[0] ) return false;
  //m_PassedCuts++; //// 
  //m_preSampFrac = fracs[0];
  
  if ( (pClus->energy( ((CaloSampling::CaloSample)0) ) + pClus->energy( ((CaloSampling::CaloSample)4) ) ) > m_EnergyAbsCut[0] ) return false;
  //m_PassedCuts++; //// 
  //m_preSamp = (fracs[0])*pClus->energy();

  ATH_MSG_DEBUG("L2 Calo Layers passed");

  return true;
}

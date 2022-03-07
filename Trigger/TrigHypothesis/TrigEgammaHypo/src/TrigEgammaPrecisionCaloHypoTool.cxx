/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "TrigEgammaPrecisionCaloHypoTool.h"


namespace TCU = TrigCompositeUtils;

TrigEgammaPrecisionCaloHypoTool::TrigEgammaPrecisionCaloHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigEgammaPrecisionCaloHypoTool::initialize()  {
  ATH_MSG_DEBUG( "Initialization completed successfully"   );    
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin      );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    << "( lo )/" << m_eT2thr    << "( hi )" );  
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );
  
   if ( m_etabin.size() == 0 ) {
    ATH_MSG_ERROR(  " There are no cuts set (EtaBins property is an empty list)" );
    return StatusCode::FAILURE;
  }

  unsigned int nEtaBin = m_etabin.size();
#define CHECK_SIZE( __n) if ( m_##__n.size() !=  (nEtaBin - 1) )		\
    { ATH_MSG_DEBUG(" __n size is " << m_##__n.size() << " but needs to be " << (nEtaBin - 1) ); return StatusCode::FAILURE; }

  CHECK_SIZE( eTthr );
  CHECK_SIZE( eT2thr );
#undef CHECK_SIZE



  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  return StatusCode::SUCCESS;
}


bool TrigEgammaPrecisionCaloHypoTool::decide( const ITrigEgammaPrecisionCaloHypoTool::ClusterInfo& input ) const {

  bool pass = false;

  auto mon_dEta         = Monitored::Scalar( "dEta", -1. ); 
  auto mon_dPhi         = Monitored::Scalar( "dPhi", -1. );
  auto mon_eta          = Monitored::Scalar( "Eta", -99. ); 
  auto mon_phi          = Monitored::Scalar( "Phi", -99. );
  auto mon_eT_Cluster   = Monitored::Scalar( "Et_em"   , -1.0 );
  auto PassedCuts       = Monitored::Scalar<int>( "CutCounter", -1 );  
  auto monitorIt        = Monitored::Group( m_monTool, mon_dEta, mon_dPhi, mon_eta, mon_phi, mon_eT_Cluster,PassedCuts );

 // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  float dEta(0), dPhi(0), eta(0), phi(0), eT_Cluster(0);

  auto roiDescriptor = input.roi;


  if ( fabs( roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG( "REJECT The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now" );
      pass=false; // special case       
      return pass;
  } 

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
  		 << ": Eta = " << roiDescriptor->eta()
  		 << ", Phi = " << roiDescriptor->phi() );

  // fill local variables for RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();

  // correct phi the to right range ( probably not needed anymore )   
  if ( fabs( phiRef ) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range


  auto pClus = input.cluster;
  eta = pClus->eta();
  phi = pClus->phi();

  float absEta = fabs( pClus->eta() );
  const int cutIndex = findCutIndex( absEta );
  
  dEta =  pClus->eta() - etaRef;
  //  Deal with angle diferences greater than Pi
  dPhi =  fabs( pClus->phi() - phiRef );
  dPhi = ( dPhi < M_PI ? dPhi : 2*M_PI - dPhi ); // TB why only <
  eT_Cluster  = pClus->et();
  // apply cuts: DeltaEta( clus-ROI )
  ATH_MSG_DEBUG( "CaloCluster: eta="  << pClus->eta()
  		 << " roi eta=" << etaRef << " DeltaEta=" << dEta
  		 << " cut: <"   << m_detacluster          );
  
  if ( fabs(dEta) > m_detacluster ) {
    ATH_MSG_DEBUG("REJECT Cluster dEta cut failed");
    return pass;
  }
  mon_eta  = eta;
  mon_dEta = dEta;
  PassedCuts = PassedCuts + 1; //Deta
  
  // DeltaPhi( clus-ROI )
  ATH_MSG_DEBUG( ": phi="  << pClus->phi()
  		 << " roi phi="<< phiRef    << " DeltaPhi="<< dPhi
  		 << " cut: <"  << m_dphicluster );
  
  if( dPhi > m_dphicluster ) {
    ATH_MSG_DEBUG("REJECT Clsuter dPhi cut failed");
    return pass;
  }
  mon_dPhi = dPhi;
  mon_phi  = phi;
  PassedCuts = PassedCuts + 1; //DPhi

  // eta range
  if ( cutIndex == -1 ) {  // VD
    ATH_MSG_DEBUG( "Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return pass;
  } else { 
    ATH_MSG_DEBUG( "eta bin used for cuts " << cutIndex );
  }
  PassedCuts = PassedCuts + 1; // passed eta cut
  
  // ET_em
  ATH_MSG_DEBUG( "PrecisionCaloCluster: ET_em=" << eT_Cluster << " cut: >"  << m_eTthr[cutIndex] );
  if ( eT_Cluster < m_eTthr[cutIndex] ) {
    ATH_MSG_DEBUG("REJECT et cut failed");
    return pass;
  }
  mon_eT_Cluster  = eT_Cluster;

  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
 
}

int TrigEgammaPrecisionCaloHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}


StatusCode TrigEgammaPrecisionCaloHypoTool::decide( std::vector<ClusterInfo>& input )  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

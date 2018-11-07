/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "TrigL2TauHypoTool.h"

using namespace TrigCompositeUtils;

TrigL2TauHypoTool::TrigL2TauHypoTool( const std::string& type,
						const std::string& name, 
						const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigL2TauHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = " 
		<< ( m_acceptAll==true ? "True" : "False" ) ); 
  //ATH_MSG_DEBUG( "TrackPt              = " << m_trackPt           );
  // Cluster-based pT cut
  ATH_MSG_DEBUG( "LowerPtCut = " << m_lowerPtCut );
  ATH_MSG_DEBUG( "ClusterEnergySumCone = " << m_clusterCone );

  // Cell-based shower requirement
  ATH_MSG_DEBUG( "UseCellCut = " << m_cellCut );
  ATH_MSG_DEBUG( "CellVariableCore = " << m_cellCore );
  ATH_MSG_DEBUG( "CellVariableOuter = "  << m_cellOuter );
  ATH_MSG_DEBUG( "CoreFractionCut = " << m_coreFractionCut );
  ATH_MSG_DEBUG( "HadRadiusCut = " << m_hadRadiusCut );

  m_multiplicity = m_lowerPtCut.size();
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  return StatusCode::SUCCESS;
}


TrigL2TauHypoTool::~TrigL2TauHypoTool() {}

bool TrigL2TauHypoTool::decide( const ITrigL2TauCaloHypoTool::ClusterInfo& input ) const {

  bool pass = false;

  using namespace Monitored;

  auto fCore             = MonitoredScalar::declare( "fCore"          ,  -9999.9 );
  auto hadRadius         = MonitoredScalar::declare( "hadRadius"      ,   9999.9 );
  auto seedPt            = MonitoredScalar::declare( "seedPt"         ,  -9999.9 );
  auto dRBaryCenterRoI   = MonitoredScalar::declare( "dRBaryCenterRoI",  -9999.9 );
  auto PassedCuts        = MonitoredScalar::declare<int>( "CutCounter", -1 );  
  auto monitorIt         = MonitoredScope::declare( m_monTool, 
                      fCore, hadRadius, seedPt, dRBaryCenterRoI, PassedCuts );
  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }
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

  auto pClus = input.clustervector;

  TLorentzVector myCluster;
  TLorentzVector TauBarycenter(0., 0., 0., 0.);

  //xAOD::CaloClusterContainer::const_iterator clusterIt;
  for (auto cluster: *pClus) {
    ATH_MSG_DEBUG(" Cluster (e, eta, phi) : ("<< cluster->e() << " , " <<cluster->eta()<<" , "<<cluster->phi()<< " )" );
      if(cluster->e() < 0){ ATH_MSG_DEBUG(" Negative energy cluster is rejected" ); continue;}
      myCluster.SetPtEtaPhiE(cluster->pt(), cluster->eta(), cluster->phi(), cluster->e());
      TauBarycenter += myCluster;
  }

  ATH_MSG_DEBUG("Jet formed (pT, eta, phi, m): ("<< TauBarycenter.Pt()<<" , "<<TauBarycenter.Eta()<<" , "<<TauBarycenter.Phi()<<" , "<<TauBarycenter.M()<< " )");

  // Determine the LC tau pT at detector axis
  TLorentzVector TauDetectorAxis(0.,0.,0.,0.);

  for (auto cluster: *pClus) {
    myCluster.SetPtEtaPhiE(cluster->pt(), cluster->eta(), cluster->phi(), cluster->e());
    if(TauBarycenter.DeltaR(myCluster) > m_clusterCone[0]) continue;
    TauDetectorAxis += myCluster;
  } // end loop on clusters

  seedPt = TauDetectorAxis.Pt();
  dRBaryCenterRoI = sqrt((etaRef-TauDetectorAxis.Eta())*(etaRef-TauDetectorAxis.Eta()) + (phiRef-TauDetectorAxis.Phi())*(phiRef-TauDetectorAxis.Phi()));

  // Offer quick shortcut if pT cut is not passed:
  if (seedPt < m_lowerPtCut[0])
    {
      ATH_MSG_DEBUG( " REGTEST: The cluster-based tau pT = " << seedPt
              << "did not pass the pt threshold: "
              << m_lowerPtCut
              );
      return pass;
    } else if(m_cellCut[0] == false)
    {
      // Not cutting on cells -> pass straight through
      pass = true;
      ATH_MSG_DEBUG( " REGTEST: TE accepted !! " );
      return pass;
    }

  PassedCuts++;

  //loop over the cells in the cluster collection
  std::bitset<200000> cellSeen;

  TLorentzVector myCell;
  float coreSum   =0;
  float outerSum  =0;
  float outerHad  =0;
  float outerHadW =0;

  for (auto cluster: *pClus){

      CaloClusterCellLink::const_iterator firstcell = cluster->getCellLinks()->begin();
      CaloClusterCellLink::const_iterator lastcell = cluster->getCellLinks()->end();

      //loop over cells and calculate the variables
      for (; firstcell != lastcell; ++firstcell){
	      // use hash code to prevent cell double counting
	      if (cellSeen.test((*firstcell)->caloDDE()->calo_hash()))
	        continue;
	      else
	        cellSeen.set((*firstcell)->caloDDE()->calo_hash());

	      myCell.SetPtEtaPhiM((*firstcell)->et(), (*firstcell)->eta(), (*firstcell)->phi(), 0); 

	      if(TauBarycenter.DeltaR(myCell) < m_cellCore[0])
	        coreSum += myCell.Pt();

	      if(TauBarycenter.DeltaR(myCell) < m_cellOuter[0])
	        outerSum += myCell.Pt();

	      // Are we in the had calorimeter or 3rd layer of the EM?
	      if((*firstcell)->caloDDE()->getSampling() > 6 && (*firstcell)->caloDDE()->getSampling() < 20 && TauBarycenter.DeltaR(myCell) < m_cellOuter[0]){
	        outerHadW += myCell.Pt()*TauBarycenter.DeltaR(myCell);
	        outerHad += myCell.Pt();
	      }
	   }
    }

  if(outerSum >0)
    fCore = coreSum/outerSum;
  if(outerHad >0)
    hadRadius = outerHadW/outerHad;

  // Check the pre-selection shower requirements
  if(fCore < m_coreFractionCut[0] || hadRadius > m_hadRadiusCut[0]){
      ATH_MSG_DEBUG( " REGTEST: The calo-based tau fCore = " << fCore
	           << " and hadRadius = " << hadRadius
              << " did not pass the thresholds: " << m_coreFractionCut[0] << " , " << m_hadRadiusCut[0]);
      pass = false;
      return pass;
  }
  // Success! All done...

  PassedCuts++;

  // got this far => passed!
  pass = true;

  // Reach this point successfully
  ATH_MSG_DEBUG( "pass = " << pass );

  return pass;
}






StatusCode TrigL2TauHypoTool::decide(  std::vector<ClusterInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}



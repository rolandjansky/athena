/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "TrigL2PhotonHypoTool.h"

using namespace TrigCompositeUtils;

TrigL2PhotonHypoTool::TrigL2PhotonHypoTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigL2PhotonHypoTool::initialize()  {
  
  //  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll            = "
		 << ( m_acceptAll==true ? "True" : "False" ) );
  ATH_MSG_DEBUG ( "EtaBins        = " << m_etabin       );
  ATH_MSG_DEBUG( "ETthr          = "
		 << m_eTthr    << "(lo)/" << m_eT2thr    << "(hi)"     );
  ATH_MSG_DEBUG ( "HADETthr       = "
		  << m_hadeTthr << "(lo)/" << m_hadeT2thr << "(hi)"     );
  ATH_MSG_DEBUG( "CARCOREthr     = " << m_carcorethr  );
  ATH_MSG_DEBUG( "CAERATIOthr    = " << m_caeratiothr );
  ATH_MSG_DEBUG( "dPHICLUSTERthr = " << m_dphicluster );
  ATH_MSG_DEBUG( "dETACLUSTERthr = " << m_detacluster );

  unsigned int nEtaBin=m_etabin.size();
  if ( m_eTthr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " etThr size is " <<  m_eTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }


  if ( m_eT2thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " et2Thr size is " <<  m_eT2thr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_hadeTthr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " hadetThr size is " <<  m_hadeTthr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_hadeT2thr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " hadet2Thr size is " <<  m_hadeT2thr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_carcorethr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " carcore size is " <<  m_carcorethr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }

  if ( m_caeratiothr.size() != nEtaBin-1 ) {
    ATH_MSG_ERROR( " caeratio size is " <<  m_caeratiothr.size() << " but needs " << nEtaBin-1 );
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}


TrigL2PhotonHypoTool::~TrigL2PhotonHypoTool() {}


bool TrigL2PhotonHypoTool::decideOnSingleObject( const xAOD::TrigPhoton* photon,
						 size_t cutIndex ) const {
  using namespace Monitored;

  auto cutCounter = MonitoredScalar::declare<int>( "CutCounter", -1 );
  auto cutIndexM  = MonitoredScalar::declare<int>( "CutIndex", cutIndex );
  auto PhEt =  MonitoredScalar::declare( "PhEt", -99. );
  auto PhEta = MonitoredScalar::declare( "PhEta", -99. );
  auto PhPhi = MonitoredScalar::declare( "PhPhi", -99. );
  auto dEta = MonitoredScalar::declare( "dEta", -99. );
  auto dPhi = MonitoredScalar::declare( "dPhi", -99. );
  auto PhRcore = MonitoredScalar::declare( "PhRcore", -99. );
  auto PhEratio = MonitoredScalar::declare( "PhRcore", -99. );
  auto PhHadEt = MonitoredScalar::declare( "PhHadEt", -99. );
  auto PhF1 = MonitoredScalar::declare( "PhF1", -99. );
  auto monitorIt  = MonitoredScope::declare( m_monTool,
                                             cutCounter, cutIndexM,
                                             PhEt,
                                             PhEta, PhPhi,
                                             dEta, dPhi,
                                             PhRcore, PhEratio,
                                             PhHadEt, PhF1 );


  double EmET         = -99.0;
  double HadEmRatio   = -99.0;
  double Reta         = -99.0;
  double Eratio       = -99.0;
  double f1           = -99.0;
  double HadET        = -99.0;

  if ( photon == 0 )
    return  false;
  cutCounter++;

  // Determine which eta bin to apply the cuts                                                                                                                                                              
  double absEta = fabs( photon->eta() );
  int etaBin = -1;
  for (std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++)
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin;

  // getting photon variable                                                                                                                                                                                
  dEta       = photon->dEta();
  dPhi       = photon->dPhi();
  Eratio     = photon->eratio();
  Reta       = photon->rcore();
  EmET       = photon->pt();
  HadET      = photon->etHad();
  f1         = photon->f1();

  if(m_etCutOnly) {
    ATH_MSG_DEBUG ( "EtCutOnly property is set: TrigPhoton: ET_em=" << EmET
		    << " cut in etaBin " << etaBin << " is ET_em >= " << m_eTthr[0] );
  }

  //now use the ratio, not the absolute HadEt (this is not yet persistified in the TrigPhoton)                                                                                                              
  HadEmRatio = (EmET!=0) ? HadET/EmET : -1.0;

  ATH_MSG_VERBOSE( "Cut index " << cutIndex );
  if ( fabs(dEta) < m_detacluster[cutIndex] ){
    ATH_MSG_VERBOSE( "Fails dEta cut " << fabs(dEta) << " < " << m_detacluster[cutIndex] );
    return  false;
  }
  cutCounter++;

  if ( fabs(dPhi) < m_dphicluster[cutIndex] ){
    ATH_MSG_VERBOSE( "Fails dphi cut " << fabs(dPhi) << " < " << m_dphicluster[cutIndex] );
    return  false;
  }
  cutCounter++;

  // eta range                                                                                                                                                                                              
  if ( etaBin==-1 ) {
    ATH_MSG_VERBOSE( "Photon eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return false;
  } else {
    ATH_MSG_VERBOSE( "eta bin used for cuts " << etaBin );
  }
  cutCounter++; // passed eta cut                                                                                                                                                                           
  // Reta (was previously called Rcore)                                                                                                                                                                     
  if ( Reta > m_carcorethr[etaBin][cutIndex] ){
    ATH_MSG_VERBOSE( "TrigPhoton Reta=" << Reta
		     << " cut in etaBin " << etaBin << " is Reta >= "  << m_carcorethr[etaBin][cutIndex]  );
    return  false;
  }
  cutCounter++;
  //  // Eratio           
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
  if ( inCrack || f1<m_F1thr[0] ) {
    ATH_MSG_VERBOSE(  "TrigPhoton: InCrack= " << inCrack << " F1=" << f1
		      << " Eratio cut not being applied" );
  } else {
    if ( Eratio > m_caeratiothr[etaBin][cutIndex] ) return false;
  }
  cutCounter++;
  if(inCrack)  Eratio  = -1; //Set default value in crack for monitoring.                                                                                                                                   

  // ET_em                                                                                                                                                                                                  
  if ( EmET > m_eTthr[etaBin][cutIndex]) {
    ATH_MSG_VERBOSE( "TrigPhoton: ET_em=" << EmET
		     << " not in etaBin " << etaBin << " is ET_em < " << m_eTthr[etaBin][cutIndex] );
    return false;
  }
  cutCounter++;


  // ET_had                                                                                                                                                                                                 
  // find which ET_had to apply : this depends on the ET_em and the eta bin                                                                                                                                 
  std::vector<double> hadET_cut={-1};

  if ( EmET >  m_eT2thr[etaBin][cutIndex] ) {
    hadET_cut[etaBin] = m_hadeT2thr[etaBin][cutIndex] ;
    ATH_MSG_VERBOSE( "ET_em>"     << m_eT2thr[etaBin][cutIndex]
		     << ": use high ET_had cut: <" << hadET_cut[etaBin] );
  } else {
    hadET_cut[etaBin] = m_hadeTthr[etaBin][cutIndex];
    ATH_MSG_VERBOSE( "ET_em<"    << m_eT2thr[etaBin][cutIndex]
		     << ": use low ET_had cut: <" << hadET_cut[etaBin] );
  }

  if ( HadEmRatio < hadET_cut[etaBin] ){
    ATH_MSG_VERBOSE( "TrigPhoton: ET_had=" <<  HadEmRatio
		     << "  not in etaBin " << etaBin << " is ET_had <=" << hadET_cut[etaBin] );
    return false;

  }
  cutCounter++;
  return true;

}

StatusCode TrigL2PhotonHypoTool::inclusiveSelection( std::vector<PhotonInfo>& input ) const {
  for ( auto i: input ) {
    if ( m_respectPreviousDecision 
	 and ( i.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) ) continue; // the decision was negative or not even made in previous stage

    auto objDecision = decideOnSingleObject( i.photon, 0 );
    if ( objDecision == true ) {
      addDecisionID( m_decisionId.numeric(), i.decision );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigL2PhotonHypoTool::markPassing( std::vector<PhotonInfo>& input, const std::set<size_t>& passing ) const {

  for ( auto idx: passing ) 
    addDecisionID( m_decisionId.numeric(), input[idx].decision );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2PhotonHypoTool::decide(  std::vector<PhotonInfo>& input )  const {
  if ( m_multiplicity == 1 ) {
      return inclusiveSelection( input );

  } //else {    
  //    return multiplicitySelection( input );    
  //  }

  return StatusCode::SUCCESS;
}



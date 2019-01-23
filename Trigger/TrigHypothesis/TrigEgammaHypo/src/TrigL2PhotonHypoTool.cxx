
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/Monitored.h"

#include "TrigL2PhotonHypoTool.h"

using namespace TrigCompositeUtils;

TrigL2PhotonHypoTool::TrigL2PhotonHypoTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrigL2PhotonHypoTool::initialize()  {
  
  //  if ( !m_monTool.empty() ) CHECK( m_monTool.rerthrieve() );

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


  std::vector<size_t> sizes( {m_eTthr.size(), m_eT2thr.size(), m_hadeTthr.size(), m_hadeT2thr.size(), m_carcorethr.size(), m_caeratiothr.size() } );   

if ( *std::min_element( sizes.begin(), sizes.end() ) != *std::max_element( sizes.begin(), sizes.end() )  ) {     
  ATH_MSG_ERROR( "Missconfiguration, cut properties listed above ( when DEBUG ) have different dimensions shortest: " <<  *std::min_element( sizes.begin(), sizes.end() ) << " longest " << *std::max_element( sizes.begin(), sizes.end() ) );     
  return StatusCode::FAILURE;   }

  return StatusCode::SUCCESS;
}


TrigL2PhotonHypoTool::~TrigL2PhotonHypoTool() {}


bool TrigL2PhotonHypoTool::decideOnSingleObject( const xAOD::TrigPhoton* photon,size_t cutIndex ) const {

  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );
  auto cutIndexM  = Monitored::Scalar<int>( "CutIndex", cutIndex );
  auto PhEt =  Monitored::Scalar( "PhEt", -99. );
  auto PhEta = Monitored::Scalar( "PhEta", -99. );
  auto PhPhi = Monitored::Scalar( "PhPhi", -99. );
  auto dEta = Monitored::Scalar( "dEta", -99. );
  auto dPhi = Monitored::Scalar( "dPhi", -99. );
  auto PhRcore = Monitored::Scalar( "PhRcore", -99. );
  auto PhEratio = Monitored::Scalar( "PhRcore", -99. );
  auto PhHadEt = Monitored::Scalar( "PhHadEt", -99. );
  auto PhF1 = Monitored::Scalar( "PhF1", -99. );
  auto monitorIt  = Monitored::Group( m_monTool,
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

   if(photon == 0) return false;

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

    
    return true;
  }
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
    return true;
  } else {
    ATH_MSG_VERBOSE( "eta bin used for cuts " << etaBin );
  }
  cutCounter++; // passed eta cut                                                                                                                                                                           
  // Reta (was previously called Rcore)                                                                                                                                                                     
  if ( Reta > m_carcorethr[cutIndex][etaBin] ){
    ATH_MSG_VERBOSE( "TrigPhoton Reta=" << Reta
		     << " cut in etaBin " << etaBin << " is Reta >= "  << m_carcorethr[cutIndex][etaBin]  );
    return  false;
  }
  cutCounter++;
  //  // Eratio           
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
  if ( inCrack || f1<m_F1thr[0] ) {
    ATH_MSG_VERBOSE(  "TrigPhoton: InCrack= " << inCrack << " F1=" << f1
		      << " Eratio cut not being applied" );
  } else {
    if ( Eratio > m_caeratiothr[cutIndex][etaBin] ) return false;
  }
  cutCounter++;
  if(inCrack)  Eratio  = -1; //Set default value in crack for monitoring.                                                                                                                                   

  // ET_em                                                                                                                                                                                                  
  if ( EmET < m_eTthr[cutIndex][etaBin]) {
    ATH_MSG_VERBOSE( "TrigPhoton: ET_em=" << EmET
		     << " not in etaBin " << etaBin << " is ET_em < " << m_eTthr[cutIndex][etaBin] );
    return false;
  }
  cutCounter++;

  // ET_had                                                                                                                                                                                                 
  // find which ET_had to apply : this depends on the ET_em and the eta bin                                                                                                                                 
  std::vector<double> hadET_cut={-1};

  if ( EmET >  m_eT2thr[cutIndex][etaBin] ) {
    hadET_cut[etaBin] = m_hadeT2thr[cutIndex][etaBin] ;
    ATH_MSG_VERBOSE( "ET_em>"     << m_eT2thr[cutIndex][etaBin] );
  } else {
    hadET_cut[etaBin] = m_hadeTthr[cutIndex][etaBin];
    ATH_MSG_VERBOSE( "ET_em<"    << m_eT2thr[cutIndex][etaBin] );
  }

  if ( HadEmRatio < hadET_cut[etaBin] ){
    ATH_MSG_VERBOSE( "TrigPhoton: ET_had=" <<  HadEmRatio
		     << "  not in etaBin " << etaBin );
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



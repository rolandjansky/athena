
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/Combinators.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigEgammaFastPhotonHypoTool.h"

namespace TCU = TrigCompositeUtils;

TrigEgammaFastPhotonHypoTool::TrigEgammaFastPhotonHypoTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent ) 
  : AthAlgTool( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}



StatusCode TrigEgammaFastPhotonHypoTool::initialize()  {
  
  if ( !m_monTool.empty() ) 
    ATH_CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "Initialization completed successfully:" );
  ATH_MSG_DEBUG( "AcceptAll      = " << ( m_acceptAll==true ? "True" : "False" ) );
  ATH_MSG_DEBUG( "EtaBins        = " << m_etabin      );
  ATH_MSG_DEBUG( "ETthr          = " << m_eTthr    << "(lo)/" << m_eT2thr    << "(hi)"     );
  ATH_MSG_DEBUG( "HADETthr       = " << m_hadeTthr << "(lo)/" << m_hadeT2thr << "(hi)"     );
  ATH_MSG_DEBUG( "CARCOREthr     = " << m_carcorethr  );
  ATH_MSG_DEBUG( "CAERATIOthr    = " << m_caeratiothr );


  std::vector<size_t> sizes( {m_eTthr.size(), m_eT2thr.size(), 
                              m_hadeTthr.size(), m_hadeT2thr.size(),
                              m_carcorethr.size(), m_caeratiothr.size() } );   

  if ( *std::min_element( sizes.begin(), sizes.end() ) != *std::max_element( sizes.begin(), sizes.end() )  ) {     
    ATH_MSG_ERROR( "Missconfiguration, cut properties listed above ( when DEBUG ) have different dimensions shortest: " 
                    <<  *std::min_element( sizes.begin(), sizes.end() ) << " longest " 
                    << *std::max_element( sizes.begin(), sizes.end() ) );     
    return StatusCode::FAILURE;   
  }

  return StatusCode::SUCCESS;
}

//==================================================================

StatusCode TrigEgammaFastPhotonHypoTool::decide( std::vector<PhotonInfo>& input)  const {
  for ( auto& i: input ) {
    if ( TCU::passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i.photon ) ) {
        TCU::addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

//==================================================================

bool TrigEgammaFastPhotonHypoTool::decide( const xAOD::TrigPhoton* photon ) const {

  auto cutCounter = Monitored::Scalar<int>( "CutCounter", -1 );
  auto mon_Et     = Monitored::Scalar( "Et", -99. );
  auto mon_Eta    = Monitored::Scalar( "Eta", -99. );
  auto mon_Phi    = Monitored::Scalar( "Phi", -99. );
  auto mon_Rcore  = Monitored::Scalar( "Rcore", -99. );
  auto mon_Eratio = Monitored::Scalar( "Eratio", -99. );
  auto mon_HadEt  = Monitored::Scalar( "Et_had", -99. );
  auto mon_F1     = Monitored::Scalar( "F1", -99. );
  auto mon_HadEmRatio = Monitored::Scalar( "HadEmRatio", -99. );
  auto monitorIt  = Monitored::Group( m_monTool,
                                      cutCounter, 
                                      mon_Et,
                                      mon_Eta, mon_Phi,
                                      mon_Rcore, mon_Eratio,
                                      mon_HadEt, mon_F1,mon_HadEmRatio );

  float EmET         = -99.0;
  float HadEmRatio   = -99.0;
  float Reta         = -99.0;
  float Eratio       = -99.0;
  float f1           = -99.0;
  float HadET        = -99.0;

  if(!photon) return false;

  cutCounter++;
  
  // Determine which eta bin to apply the cuts                                                                                                                                                              
  float absEta = std::abs( photon->eta() );

  int etaBin = findCutIndex(absEta);

  // getting photon variable           
  Eratio     = photon->eratio();
  Reta       = photon->rcore();
  EmET       = photon->pt();
  HadET      = photon->etHad();
  f1         = photon->f1();

  if(m_acceptAll) {
    ATH_MSG_DEBUG ( "Accept all property is set: TrigPhoton: ET_em=" << EmET << " cut in etaBin " 
                    << etaBin << " is ET_em >= " << m_eTthr[0] );
    return true;
  }
  
  // eta range                                                                                                                                                                                              
  if ( etaBin==-1 ) {
    ATH_MSG_DEBUG( "Photon FAILS eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return false;
  } else {
    ATH_MSG_DEBUG( "eta bin used for cuts " << etaBin );
  }
  cutCounter++; // passed eta cut       
  mon_Eta = photon->eta();
  mon_Phi = photon->phi();

  // ET_em                                                                                                                                                                                                  
  if ( EmET < m_eTthr[etaBin]) {
    ATH_MSG_DEBUG( "TrigPhoton FAILS ET_em=" << EmET
		     << " not in etaBin " << etaBin << " is ET_em < " << m_eTthr[etaBin] );
    return false;
  }
  mon_Et =  EmET;
  cutCounter++; // passed ET threshold cut

  // Reta (was previously called Rcore) 
  if ( Reta < m_carcorethr[etaBin] ){
    ATH_MSG_DEBUG( "FastPhoton FAILS Reta=" << Reta << " cut in etaBin " 
                      << etaBin << " is Reta >= "  << m_carcorethr[etaBin]  );
    return  false;
  }
  mon_Rcore = Reta;
  cutCounter++;


  //  // Eratio           
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
  if ( inCrack || f1<m_F1thr[etaBin] ) {
    ATH_MSG_DEBUG(  "FastPhoton: InCrack= " << inCrack << " F1=" << f1
                     << " Eratio cut not being applied" );
  } else {
    if ( Eratio < m_caeratiothr[etaBin] ) {
    ATH_MSG_DEBUG( "FastPhoton FAILS Eratio=" << Eratio << " cut in etaBin " 
                      << etaBin << " is Eratio >= "  << m_caeratiothr[etaBin]  );
      return false;
    }
  }
  cutCounter++;
  if(inCrack)  Eratio  = -1; //Set default value in crack for monitoring.                                                                                                                                   
  mon_Eratio = Eratio;
  mon_F1 = f1;
  // ET_had                                                                                                                                                                                                 
  // find which ET_had to apply : this depends on the ET_em and the eta bin                                                                                                                                 
  float hadET_cut=-1;

  if ( EmET >  m_eT2thr[etaBin] ) {
    hadET_cut = m_hadeT2thr[etaBin] ;
    ATH_MSG_DEBUG( "ET_em>" << m_eT2thr[etaBin] );
  } else {
    hadET_cut = m_hadeTthr[etaBin];
    ATH_MSG_DEBUG( "ET_em<" << m_eT2thr[etaBin] );
  }
  
  HadEmRatio = (EmET!=0) ? HadET/EmET : -1.0;

  if ( HadEmRatio > hadET_cut ){
    ATH_MSG_DEBUG( "FastPhoton FAILS ET_had=" << HadEmRatio << " cut in etaBin " 
                      << etaBin << " is ERatio >= "  << hadET_cut  );
    return false;

  }
  mon_HadEt = HadET;
  mon_HadEmRatio = HadEmRatio;
  cutCounter++;
  ATH_MSG_DEBUG( "FastPhoton PASS");

  return true;
  
}

//==================================================================

int TrigEgammaFastPhotonHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}



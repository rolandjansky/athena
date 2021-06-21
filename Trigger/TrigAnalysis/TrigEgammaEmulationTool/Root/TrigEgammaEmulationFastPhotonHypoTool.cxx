

#include "TrigEgammaEmulationTool/TrigEgammaEmulationFastPhotonHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationFastPhotonHypoTool::TrigEgammaEmulationFastPhotonHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//!==========================================================================

bool TrigEgammaEmulationFastPhotonHypoTool::emulate(const Trig::TrigData &input,
                                                    bool &pass ) const
{
  pass = false;

  if(!input.roi){
    ATH_MSG_DEBUG( "RoI is null ptr");
    return false;
  }

  if(!input.trigPhoton){
    ATH_MSG_DEBUG( "Cluster is null ptr");
    return false;
  }
  
  pass = decide(input);
  
  return true;
}

//!==========================================================================


bool TrigEgammaEmulationFastPhotonHypoTool::decide(  const Trig::TrigData &input ) const
{

  float EmET         = -99.0;
  float HadEmRatio   = -99.0;
  float Reta         = -99.0;
  float Eratio       = -99.0;
  float f1           = -99.0;
  float HadET        = -99.0;
  int cutCounter     = 0;

  const xAOD::TrigPhoton* photon = input.trigPhoton;

  
  // Determine which eta bin to apply the cuts                                                                                                                                                              
  float absEta = std::abs( photon->eta() );

  
  int etaBin = findCutIndex(absEta);
  
  // getting photon variable                                                                                                           
  Eratio     = photon->eratio();
  Reta       = photon->rcore();
  EmET       = photon->pt();
  HadET      = photon->etHad();
  f1         = photon->f1();
  

  HadEmRatio = (EmET!=0) ? HadET/EmET : -1.0;


  if(m_acceptAll) {
    ATH_MSG_DEBUG ( "Accept all property is set: TrigPhoton: ET_em=" << EmET << " cut in etaBin " 
                    << etaBin << " is ET_em >= " << m_eTthr[0] );
    return true;
  }

  // eta range                                                                                                                                                                                              
  if ( etaBin==-1 ) {
    ATH_MSG_VERBOSE( "Photon eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return true;
  } else {
    ATH_MSG_VERBOSE( "eta bin used for cuts " << etaBin );
  }
  cutCounter++; // passed eta cut                                                                                                                                                                           
  // Reta (was previously called Rcore)                                                                                                                                                                     
  if ( Reta > m_carcorethr[etaBin] ){
    ATH_MSG_VERBOSE( "TrigPhoton Reta=" << Reta
		     << " cut in etaBin " << etaBin << " is Reta >= "  << m_carcorethr[etaBin]  );
    return  false;
  }
  cutCounter++;
  //  // Eratio           
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
  if ( inCrack || f1<m_F1thr[0] ) {
    ATH_MSG_VERBOSE(  "TrigPhoton: InCrack= " << inCrack << " F1=" << f1
		      << " Eratio cut not being applied" );
  } else {
    if ( Eratio > m_caeratiothr[etaBin] ) return false;
  }
  cutCounter++;
  if(inCrack)  Eratio  = -1; //Set default value in crack for monitoring.                                                                                                                                   

  // ET_em                                                                                                                                                                                                  
  if ( EmET < m_eTthr[etaBin]) {
    ATH_MSG_VERBOSE( "TrigPhoton: ET_em=" << EmET
		     << " not in etaBin " << etaBin << " is ET_em < " << m_eTthr[etaBin] );
    return false;
  }
  cutCounter++;

  // ET_had                                                                                                                                                                                                 
  // find which ET_had to apply : this depends on the ET_em and the eta bin                                                                                                                                 
  float hadET_cut=-1;

  if ( EmET >  m_eT2thr[etaBin] ) {
    hadET_cut = m_hadeT2thr[etaBin] ;
    ATH_MSG_VERBOSE( "ET_em>"     << m_eT2thr[etaBin] );
  } else {
    hadET_cut = m_hadeTthr[etaBin];
    ATH_MSG_VERBOSE( "ET_em<"    << m_eT2thr[etaBin] );
  }

  if ( HadEmRatio < hadET_cut ){
    ATH_MSG_VERBOSE( "TrigPhoton: ET_had=" <<  HadEmRatio
		     << "  not in etaBin " << etaBin );
    return false;

  }
  cutCounter++;
  
  return true;
  
}


//==================================================================

int TrigEgammaEmulationFastPhotonHypoTool::findCutIndex( float eta ) const {
  const float absEta = std::abs(eta);
  auto binIterator = std::adjacent_find( m_etabin.begin(), m_etabin.end(), [=](float left, float right){ return left < absEta and absEta < right; }  );
  if ( binIterator == m_etabin.end() ) {
    return -1;
  }
  return  binIterator - m_etabin.begin();
}




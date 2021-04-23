


#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationFastElectronHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationFastElectronHypoTool::TrigEgammaEmulationFastElectronHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//=================================================================

bool TrigEgammaEmulationFastElectronHypoTool::emulate(const Trig::TrigData &input,
                                                      bool &pass) const
{
  pass=false;

  if( !input.roi )  return false;

  if( input.trigElectrons.empty() )  return false;

  for ( const auto &el : input.trigElectrons )
  {
    if( decide( input, el ) ){
      pass=true;
      return true;
    }
  }

  return false;
}

//=================================================================

bool TrigEgammaEmulationFastElectronHypoTool::decide( const Trig::TrigData &/*input*/ ,
                                                      const xAOD::TrigElectron* electron ) const 
{
  bool pass=false;


  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
    return pass;
   } else {
     pass = false;
     ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
   }

  int cutCounter = 0;  
 

  const xAOD::TrackParticle* trkIter = electron-> trackParticle();
  if ( trkIter == 0 ){  // disconsider candidates without track
     pass = false;
     return pass;
  }
  cutCounter++;

  // Retrieve all quantities
  float dPhiCalo    = electron->trkClusDphi();
  float dEtaCalo    = electron->trkClusDeta();
  float ptCalo      = electron->pt();	  
  float eToverPt    = electron->etOverPt();	  
  float NTRHits     = electron->nTRTHits();
  float NStrawHits  = electron->nTRTHiThresholdHits();
  float TRTHitRatio = NStrawHits == 0 ? 1e10 : NTRHits/NStrawHits;

  if ( ptCalo < m_trackPt ){ 
    ATH_MSG_DEBUG( "Fails pt cut" << ptCalo << " < " << m_trackPt );
    pass = false;
    return  pass;
  }
  cutCounter++;

  if ( dEtaCalo > m_caloTrackDEta ) {
    ATH_MSG_DEBUG( "Fails dEta cut " << dEtaCalo << " < " << m_caloTrackDEta );
    
    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( dPhiCalo > m_caloTrackDPhi ) {
    ATH_MSG_DEBUG( "Fails dPhi cut " << dPhiCalo << " < " << m_caloTrackDPhi );
    
    pass = false;
    return  pass;
  }

  cutCounter++;
  if( eToverPt <  m_caloTrackdEoverPLow ) {
    ATH_MSG_DEBUG( "Fails eoverp low cut " << eToverPt << " < " <<  m_caloTrackdEoverPLow );

    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( eToverPt > m_caloTrackdEoverPHigh ) {
    ATH_MSG_DEBUG( "Fails eoverp high cut " << eToverPt << " < " << m_caloTrackdEoverPHigh );

    pass = false;
    return  pass;
  }
  cutCounter++;
  if ( TRTHitRatio < m_trtRatio ){
    ATH_MSG_DEBUG( "Fails TRT cut " << TRTHitRatio << " < " << m_trtRatio );

    pass = false;
    return  pass;
  }
  cutCounter++;


  if(m_doLRT){
   ATH_MSG_DEBUG( "doLRT: " <<m_doLRT);
   ATH_MSG_DEBUG( "Track d0: " <<m_trkd0);
   float trk_d0 = std::fabs(trkIter->d0());
   if(trk_d0 < m_trkd0){
     ATH_MSG_DEBUG( "Fails d0 cut " <<trk_d0<< " < " <<m_trkd0 );
     pass = false;
     return pass;
   }
  }


  ATH_MSG_DEBUG( "Passed selection" );
  pass = true;
  return  pass;

}


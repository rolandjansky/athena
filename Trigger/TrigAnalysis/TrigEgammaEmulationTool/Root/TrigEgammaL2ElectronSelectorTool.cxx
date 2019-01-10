/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaEmulationTool/TrigEgammaL2ElectronSelectorTool.h"
#include <boost/dynamic_bitset.hpp>
using namespace std;
using namespace Trig;

TrigEgammaL2ElectronSelectorTool::TrigEgammaL2ElectronSelectorTool(const std::string & myname)
: TrigEgammaSelectorBaseTool(myname)
{
  declareProperty( "EtCut",                   m_etThr = 0                       );
  declareProperty( "TrackPt",                 m_trackPtthr = 5.0*CLHEP::GeV     );
  declareProperty( "CaloTrackdETA",           m_calotrackdeta                   ); //loose cut
  declareProperty( "CaloTrackdPHI",           m_calotrackdphi                   ); //loose cut
  declareProperty( "CaloTrackdEoverPLow",     m_calotrackdeoverp_low            );
  declareProperty( "CaloTrackdEoverPHigh",    m_calotrackdeoverp_high           );
  declareProperty( "TRTRatio",                m_trtratio                        );
  m_emTauRois=nullptr;

}

//!================================================================================

TrigEgammaL2ElectronSelectorTool::~TrigEgammaL2ElectronSelectorTool()
{ }

//!================================================================================

StatusCode TrigEgammaL2ElectronSelectorTool::initialize()
{
  ATH_MSG_DEBUG( "Initialization:" );
  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "TrackPt                  = " << m_trackPtthr               );
  ATH_MSG_DEBUG( "CaloTrackdETA            = " << m_calotrackdeta            );
  ATH_MSG_DEBUG( "CaloTrackdPHI            = " << m_calotrackdphi            );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow      = " << m_calotrackdeoverp_low     );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh     = " << m_calotrackdeoverp_high    );
 
  return StatusCode::SUCCESS;
}

//!================================================================================
StatusCode TrigEgammaL2ElectronSelectorTool::finalize()
{
  ATH_MSG_INFO( "in finalize()" );
  return StatusCode::SUCCESS;
}
//!================================================================================
bool TrigEgammaL2ElectronSelectorTool::emulation(const xAOD::IParticleContainer *container, bool& pass, const Trig::Info &info)
{
  pass=false;
  if(!container){
    ATH_MSG_DEBUG("Container is nullptr");
    return false;
  }

  m_emTauRois=nullptr;

  const xAOD::TrigElectronContainer *trigElecColl = static_cast<const xAOD::TrigElectronContainer *>(container);
  if ( trigElecColl->size() == 0 ) {
    ATH_MSG_DEBUG( "No electrons to analyse, leaving!" );
    return false;
  }

  if ((m_storeGate->retrieve(m_emTauRois,"LVL1EmTauRoIs")).isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve LVL1EmTauRoIs ");
    return false;
  }

  setTrackPt( info.thrHLT );
  
  ATH_MSG_DEBUG( "Got collection with " << trigElecColl->size() << " TrigElectrons" );

  // Now loop over electrons, see if at least one passes all cuts
  xAOD::TrigElectronContainer::const_iterator elecIter, elecEnd = trigElecColl->end();
  boost::dynamic_bitset<> bitAccept(trigElecColl->size());
  unsigned bit=0;

  for (elecIter = trigElecColl->begin(); elecIter != elecEnd; ++elecIter) {   
    
    bit++;
    const xAOD::TrackParticle* trkIter = (*elecIter)-> trackParticle();
    if (trkIter==nullptr)  continue; // disconsider candidates without track
  
    // Retrieve all quantities
    float dPhiCalo    = (*elecIter)->trkClusDphi();
    float dEtaCalo    = (*elecIter)->trkClusDeta();
    float pTcalo      = (*elecIter)->pt();       
    float eTOverPt    = (*elecIter)->etOverPt();         
    //    int   trackIndx   = (*elecIter)->trackIndx();
    float NTRHits     = (float)((*elecIter)->nTRTHits());
    float NStrawHits  = (float)((*elecIter)->nTRTHiThresholdHits());
    float TRTHitRatio = NStrawHits==0 ? 1e10 : NTRHits/NStrawHits;
    
    ATH_MSG_DEBUG("ptCalo      = "<<pTcalo      << " > "<< m_trackPtthr << " (m_trackPtthr)");
    ATH_MSG_DEBUG("dEtaCalo    = "<<dEtaCalo    << " > "<< m_calotrackdeta << " (m_calotrackdeta)");
    ATH_MSG_DEBUG("dPhiCalo    = "<<dPhiCalo    << " > "<< m_calotrackdphi << " (m_calotrackdphi)");
    ATH_MSG_DEBUG("eTOverPt    = "<<eTOverPt    << " > "<< m_calotrackdeoverp_low << " (m_calotrackdeoverp_low)");
    ATH_MSG_DEBUG("TRTHitRatio = "<<TRTHitRatio << " > "<< m_calotrackdeoverp_high << " (m_calotrackdeoverp_high)");


    // apply cuts
    if (pTcalo > m_trackPtthr) {
      if (dEtaCalo < m_calotrackdeta) {
        if (dPhiCalo < m_calotrackdphi) {
          if(eTOverPt >  m_calotrackdeoverp_low ) {
            if ( eTOverPt < m_calotrackdeoverp_high ) {
              if (TRTHitRatio > m_trtratio){
                // TrigElectron passed all cuts: set flags
                bitAccept.set( bit-1, true ); 
                ATH_MSG_DEBUG( "Event accepted !" );             
              }//TRTHitRatio
            }//etOverPt
          }//dphi
        }//deta
      }//pt
    }//apply cuts

  } // end of loop over electrons
   
  if(bitAccept.count() > 0)  pass=true;
  return true;
}
//!================================================================================
void TrigEgammaL2ElectronSelectorTool::setTrackPt( float et ){
  float GeV=1e3;
  // hard cuts
  if(et < 15.){
    m_trackPtthr=1*GeV;
  }else if(et >=15. && et < 20.){
    m_trackPtthr=2*GeV;
  }else if(et >=20. && et < 50.){
    m_trackPtthr=3*GeV;
  }else{// over 50
    m_trackPtthr=5*GeV;
    m_calotrackdeta=999.; 
    m_calotrackdphi=999.;
  }
}
//!================================================================================
unsigned int TrigEgammaL2ElectronSelectorTool::getTrackAlgoID( const std::string &trigger ){
  // [0=All, 3=TRT, 5=SiTrack or IdScan, 6=L2StarB, 7=L2StarC, 8=FTK, 9=Fast]
  if(boost::contains(trigger,"IdScan") || boost::contains(trigger,"SiTrack"))
    return 5;
  else if(boost::contains(trigger,"TRT"))
    return 3;
  else if(boost::contains(trigger,"FTK"))
    return 8;
  else if(boost::contains(trigger,"L2StarC"))
    return 7;
  else if(boost::contains(trigger,"L2StarB"))
    return 6;
  else{
    return 9;
  }
}
//!================================================================================

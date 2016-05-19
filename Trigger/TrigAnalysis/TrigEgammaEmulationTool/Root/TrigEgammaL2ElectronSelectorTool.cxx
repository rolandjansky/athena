/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "TrigEgammaEmulationTool/TrigEgammaL2ElectronSelectorTool.h"
#include <boost/dynamic_bitset.hpp>
using namespace std;
using namespace Trig;

TrigEgammaL2ElectronSelectorTool::TrigEgammaL2ElectronSelectorTool(const std::string & myname)
: TrigEgammaSelectorBaseTool(myname)
{

  declareProperty( "Signature",               m_signature                       );
  declareProperty( "Pidname",                 m_pidname                         );
  declareProperty( "EtCut",                   m_etThr = 0                       );
  declareProperty( "TrackAlgoId",             m_trackalgoID = 0                 );
  declareProperty( "EtaBins",                 m_etabin                          );
  declareProperty( "TrackPt",                 m_trackPtthr = 5.0*CLHEP::GeV     );
  declareProperty( "CaloTrackdETA",           m_calotrackdeta                   ); //loose cut
  declareProperty( "CaloTrackdPHI",           m_calotrackdphi                   ); //loose cut
  declareProperty( "CaloTrackdEoverPLow",     m_calotrackdeoverp_low            );
  declareProperty( "CaloTrackdEoverPHigh",    m_calotrackdeoverp_high           );
  declareProperty( "TRTRatio",                m_trtratio                        );
  declareProperty( "EtaBinsTRT",              m_etabinTRT                       );
  declareProperty( "TrackPtTRT",              m_trackPtthrTRT = 5.0*CLHEP::GeV  );
  declareProperty( "CaloTrackdETATRT",        m_calotrackdetaTRT                ); //loose cut
  declareProperty( "CaloTrackdPHITRT",        m_calotrackdphiTRT                ); //loose cut
  declareProperty( "TRTRatioTRT",             m_trtratioTRT                     );
  declareProperty( "CaloTrackdEoverPLowTRT",  m_calotrackdeoverp_lowTRT         );
  declareProperty( "CaloTrackdEoverPHighTRT", m_calotrackdeoverp_highTRT        );

}

//!================================================================================

TrigEgammaL2ElectronSelectorTool::~TrigEgammaL2ElectronSelectorTool()
{ }

//!================================================================================

StatusCode TrigEgammaL2ElectronSelectorTool::initialize()
{
  ATH_MSG_DEBUG( "Initialization:" );
  m_str_etthr = boost::lexical_cast<std::string>(m_etThr*1e-3);
  boost::algorithm::to_lower(m_signature);

  ATH_MSG_DEBUG( "EtaBins                  = " << m_etabin                   );
  ATH_MSG_DEBUG( "TrackPt                  = " << m_trackPtthr               );
  ATH_MSG_DEBUG( "CaloTrackdETA            = " << m_calotrackdeta            );
  ATH_MSG_DEBUG( "CaloTrackdPHI            = " << m_calotrackdphi            );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLow      = " << m_calotrackdeoverp_low     );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHigh     = " << m_calotrackdeoverp_high    );
  ATH_MSG_DEBUG( "TrackAlgoId              = " << m_trackalgoID              );
  ATH_MSG_DEBUG( "TRTRatio                 = " << m_trtratio                 );
  ATH_MSG_DEBUG( "EtaBinsTRT               = " << m_etabinTRT                );
  ATH_MSG_DEBUG( "TrackPtTRT               = " << m_trackPtthrTRT            );
  ATH_MSG_DEBUG( "CaloTrackdETATRT         = " << m_calotrackdetaTRT         );
  ATH_MSG_DEBUG( "CaloTrackdPHITRT         = " << m_calotrackdphiTRT         );
  ATH_MSG_DEBUG( "CaloTrackdEoverPLowTRT   = " << m_calotrackdeoverp_lowTRT  );
  ATH_MSG_DEBUG( "CaloTrackdEoverPHighTRT  = " << m_calotrackdeoverp_highTRT );
  ATH_MSG_DEBUG( "TRTRatioTRT              = " << m_trtratioTRT              );
  ATH_MSG_DEBUG( "Initialization completed successfully:"                    );
 
  return StatusCode::SUCCESS;
}

//!================================================================================
StatusCode TrigEgammaL2ElectronSelectorTool::finalize()
{
  ATH_MSG_INFO( "in finalize()" );
  return StatusCode::SUCCESS;
}
//!================================================================================
bool TrigEgammaL2ElectronSelectorTool::is_correct_trigInfo(const TrigInfo &info){
  if(info.strThrHLT != m_str_etthr)  return false;
  if(info.pidname   != m_pidname  )  return false;
  if(info.type      != m_signature)  return false;
  return true;
}
//!================================================================================
bool TrigEgammaL2ElectronSelectorTool::emulation(const xAOD::IParticleContainer *container, bool& pass, const TrigInfo &info)
{
  pass=false;
  if(!container){
    ATH_MSG_DEBUG("Container is nullptr");
    return false;
  }

  m_emTauRois=nullptr;
  if(!is_correct_trigInfo(info)){
    ATH_MSG_DEBUG("Not correct sub selector for this trigger type");
    return false;
  }

  const xAOD::TrigElectronContainer *trigElecColl = static_cast<const xAOD::TrigElectronContainer *>(container);
  if ( trigElecColl->size() == 0 ) {
    ATH_MSG_DEBUG( "No electrons to analyse, leaving!" );
    return false;
  }

  if ((m_storeGate->retrieve(m_emTauRois,"LVL1EmTauRoIs")).isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve LVL1EmTauRoIs ");
    return false;
  }
 
  ATH_MSG_DEBUG( "Got collection with " << trigElecColl->size() << " TrigElectrons" );

  // initialize counter after all error conditions checked
  //bool pTcaloCut=false;
  //bool dEtaCaloCut=false;
  //bool dPhiCaloCut=false;
  //bool eTOverPtCut_lo=false;
  //bool eTOverPtCut_hi=false;
  //bool TRTRatioCut=false;

  // Now loop over electrons, see if at least one passes all cuts
  xAOD::TrigElectronContainer::const_iterator elecIter, elecEnd = trigElecColl->end();
  boost::dynamic_bitset<> bitAccept(trigElecColl->size());
  unsigned bit=0;

  for (elecIter = trigElecColl->begin(); elecIter != elecEnd; ++elecIter) {   
    bit++;
    const xAOD::TrackParticle* trkIter = (*elecIter)-> trackParticle();
    if (trkIter==NULL)  continue; // disconsider candidates without track

    int algoId = 0;
    if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) algoId=9;
    if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ) algoId=5;
    if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] ) algoId=6;
    if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] ) algoId=7;
    ATH_MSG_DEBUG( "Trackalgo: "<< algoId );
    
    // do not try track/cluster match if produced by wrong algo (0=all algos)
    // AlgID: 0=All, 3=TRT, 5=SiTrack or IdScan, 6, strategyB, 7, strategyC, 9 new Fast Tracking
    if (m_trackalgoID == 0 || (unsigned int)algoId == m_trackalgoID ||
        (m_trackalgoID == 5 &&  (unsigned int)algoId <= 2 )) {
   
      // Retrieve all quantities
      float absEta      = fabs((*elecIter)->caloEta());
      float dPhiCalo    = (*elecIter)->trkClusDphi();
      float dEtaCalo    = (*elecIter)->trkClusDeta();
      float pTcalo      = (*elecIter)->pt();       
      float eTOverPt    = (*elecIter)->etOverPt();         
      //    int   trackIndx   = (*elecIter)->trackIndx();
      float NTRHits     = (float)((*elecIter)->nTRTHits());
      float NStrawHits  = (float)((*elecIter)->nTRTHiThresholdHits());
      float TRTHitRatio = NStrawHits==0 ? 1e10 : NTRHits/NStrawHits;

      // figure out what eta bin this cluster is in
      //Separate binning for IDScan/SiTrack and TRTSegFinder tracks   
      std::vector<float> temp_etabin;
      std::vector<float> temp_calotrackdeta;
      std::vector<float> temp_calotrackdphi;
      std::vector<float> temp_calotrackdeoverp_low;
      std::vector<float> temp_calotrackdeoverp_high;
      float temp_trackPtthr = m_trackPtthr;
      std::vector<float> temp_trtratio;

      temp_etabin                = m_etabin;
      temp_calotrackdeta         = m_calotrackdeta;
      temp_calotrackdphi         = m_calotrackdphi;
      temp_calotrackdeoverp_low  = m_calotrackdeoverp_low;
      temp_calotrackdeoverp_high = m_calotrackdeoverp_high;
      temp_trackPtthr            = m_trackPtthr;
      temp_trtratio              = m_trtratio;

      //ignore if cuts/binning not configured
      int etaBin = 0;
      if(temp_etabin.size()<2 || temp_calotrackdeta.size()<1 || temp_calotrackdphi.size()<1
         || temp_calotrackdeoverp_low.size()<1 || temp_calotrackdeoverp_high.size()<1 || temp_trtratio.size()<1) {
        ATH_MSG_DEBUG( "Track type "<< algoId<<" does not have corresponding cut configuration" );
        continue;//eta bins and cuts not defined for this track type, ignore track
      }
   
      //ignore misconfigured cuts
      if(temp_calotrackdeta.size()!= (temp_etabin.size()-1)|| temp_calotrackdphi.size()!= (temp_etabin.size()-1)
         || temp_calotrackdeoverp_low.size()!= (temp_etabin.size()-1)|| temp_calotrackdeoverp_high.size()!= (temp_etabin.size()-1)
         || temp_trtratio.size()!= (temp_etabin.size()-1)){
        ATH_MSG_DEBUG( "Track type has inconsistent cut configuration" );
        continue;
      }
     
      //Searching the correct bin
      for (std::size_t iBin = 0; iBin < (temp_etabin.size()-1); iBin++ )
        if ( absEta > temp_etabin[iBin] && absEta < temp_etabin[iBin+1] ) etaBin = iBin;
    
      ATH_MSG_DEBUG( "absEta    = " << absEta << " ==> etaBin = " << etaBin );
      ATH_MSG_DEBUG( "pT (Calo) = " << pTcalo );
      ATH_MSG_DEBUG( "dEtaCalo  = " << dEtaCalo << ", cut = " << temp_calotrackdeta[etaBin] );
      ATH_MSG_DEBUG( "dPhiCalo  = " << dPhiCalo << ", cut = " << temp_calotrackdphi[etaBin] );
      ATH_MSG_DEBUG( "eTOverPt  = " << eTOverPt << ", cuts = [" <<  temp_calotrackdeoverp_low[etaBin] 
                                    << ", " << temp_calotrackdeoverp_high[etaBin] << "]" );
       
      // apply cuts
      if (pTcalo > temp_trackPtthr) {
        //pTcaloCut=true;
        if (dEtaCalo < temp_calotrackdeta[etaBin]) {
          //dEtaCaloCut=true;
          if (dPhiCalo < temp_calotrackdphi[etaBin]) {
            //dPhiCaloCut=true;
            if(eTOverPt >  temp_calotrackdeoverp_low[etaBin] ) {
              //eTOverPtCut_lo = true; 
              if ( eTOverPt < temp_calotrackdeoverp_high[etaBin] ) {
                //eTOverPtCut_hi = true;   
                if (TRTHitRatio > temp_trtratio[etaBin]){
                  //TRTRatioCut = true;
                  // TrigElectron passed all cuts: set flags
                  bitAccept.set( bit-1, true ); 
                  ATH_MSG_DEBUG( "Event accepted !" );             
                }//TRTHitRatio
              }//etOverPt
            }//dphi
          }//deta
        }//pt
      }//apply cuts

    }// enf of track Alg
  } // end of loop over electrons
   
  if(bitAccept.count() > 0)  pass=true;
  return true;
}

//!================================================================================

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////
// Markus Elsing
/////////////////////////////////

#include "InDetTrackScoringTools/InDetAmbiScoringTool.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TMath.h"
#include <vector>
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"


//---------------------------------------------------------------------------------------------------------------------

InDet::InDetAmbiScoringTool::InDetAmbiScoringTool(const std::string& t,
              const std::string& n,
              const IInterface*  p ) :
  AthAlgTool(t,n,p),
  m_trkSummaryTool("Trk::TrackSummaryTool"),
  m_selectortool("InDet::InDetTrtDriftCircleCutTool"),
  m_summaryTypeScore(Trk::numberOfTrackSummaryTypes),
  m_iBeamCondSvc("BeamCondSvc",n),
  m_magFieldSvc("AtlasFieldSvc",n), 
  m_incidentSvc("IncidentSvc", n),
  m_fieldOn(true),
  m_holesearch(false),
  m_useITkAmbigFcn(false),
  m_etaDependentCutsSvc("",n)
{
  declareInterface<Trk::ITrackScoringTool>(this);
  
  // declare properties
  declareProperty("minNDF",            m_minNDF             = 0);
  declareProperty("minPt",             m_minPt              = 500.);
  declareProperty("maxEta",            m_maxEta             = 2.7);
  declareProperty("maxRPhiImp",        m_maxRPhiImp         = 10.);
  declareProperty("maxZImp",           m_maxZImp            = 250.);

  // changed cuts on the selction of tracks
  declareProperty("minSiClusters",     m_minSiClusters         = 7);
  declareProperty("maxDoubleHoles",    m_maxDoubleHoles        = 2);
  declareProperty("maxSiHoles",        m_maxSiHoles            = 5, "max number of Silicon (Pixel+SCT) holes");
  declareProperty("maxPixelHoles",     m_maxPixelHoles         = 5, "max number of Pixel holes");
  declareProperty("maxSCTHoles",       m_maxSctHoles           = 5, "max number of SCT holes");
  declareProperty("minTRTonTrk",       m_minTRTonTrk           = 9);
  declareProperty("minTRTPrecisionFraction", m_minTRTprecision = 0.5);
  declareProperty("minPixel",          m_minPixel              = 0);

  // which detectors to use
  declareProperty("usePixel",          m_usePixel           = true);
  declareProperty("useSCT",            m_useSCT             = true);

  // switches and tools
  declareProperty("useAmbigFcn",       m_useAmbigFcn        = true);
  declareProperty("useTRT_AmbigFcn",   m_useTRT_AmbigFcn    = false);
  declareProperty("useLogProbBins",    m_useLogProbBins     = false);
  declareProperty("useSigmaChi2",      m_useSigmaChi2       = false);

  // tools
  declareProperty("Extrapolator",      m_extrapolator);
  declareProperty("SummaryTool" ,      m_trkSummaryTool);
  declareProperty("DriftCircleCutTool",m_selectortool );
  declareProperty("BeamPositionSvc",   m_iBeamCondSvc );
  declareProperty("MagFieldSvc",       m_magFieldSvc);
  
  declareProperty("maxRPhiImpEM",      m_maxRPhiImpEM  = 50.  );
  declareProperty("doEmCaloSeed",      m_useEmClusSeed = true );
  declareProperty("minPtEM",           m_minPtEm      = 5000. ); // in MeV
  declareProperty("phiWidthEM",        m_phiWidthEm   = 0.075 );
  declareProperty("etaWidthEM",        m_etaWidthEm   = 0.05  );
  declareProperty("InputEmClusterContainerName",m_inputEmClusterContainerName); 
  
  declareProperty("useITkAmbigFcn"             , m_useITkAmbigFcn);
  declareProperty("InDetEtaDependentCutsSvc"   , m_etaDependentCutsSvc);
  
  //set values for scores
  m_summaryTypeScore[Trk::numberOfPixelHits]            =  20;
  m_summaryTypeScore[Trk::numberOfPixelSharedHits]      = -10;  // NOT USED --- a shared hit is only half the weight
  m_summaryTypeScore[Trk::numberOfPixelHoles]           = -10;  // a hole is bad
  m_summaryTypeScore[Trk::numberOfBLayerHits]           =  10;  // addition for being b-layer
  m_summaryTypeScore[Trk::numberOfBLayerSharedHits]     =  -5;  // NOT USED --- a shared hit is only half the weight
  m_summaryTypeScore[Trk::numberOfGangedPixels]         =  -5;  // decrease for being ganged
  m_summaryTypeScore[Trk::numberOfGangedFlaggedFakes]   = -10;  // decrease for being ganged fake
  m_summaryTypeScore[Trk::numberOfSCTHits]              =  10;  // half of a pixel, since only 1dim
  m_summaryTypeScore[Trk::numberOfSCTSharedHits]        =  -5;  // NOT USED -- a shared hit is only half the weight
  m_summaryTypeScore[Trk::numberOfSCTHoles]             =  -5;  // a hole is bad !
  m_summaryTypeScore[Trk::numberOfSCTDoubleHoles]       = -15;  // a double hole is even worse !
  m_summaryTypeScore[Trk::numberOfTRTHits]              =   1;  // 10 straws ~ 1 SCT
  m_summaryTypeScore[Trk::numberOfTRTHighThresholdHits] =   0;  // addition for being TR
  m_summaryTypeScore[Trk::numberOfOutliersOnTrack]      =  -1;  // -ME- TRT oulier should not kill 5 TRT on track (was -5)
  
  // scoring for Muons not needed
  m_summaryTypeScore[Trk::numberOfMdtHits]    = 0;   
  m_summaryTypeScore[Trk::numberOfTgcPhiHits] = 0; 
  m_summaryTypeScore[Trk::numberOfTgcEtaHits] = 0;
  m_summaryTypeScore[Trk::numberOfCscPhiHits] = 0;     
  m_summaryTypeScore[Trk::numberOfCscEtaHits] = 0;
  m_summaryTypeScore[Trk::numberOfRpcPhiHits] = 0;
  m_summaryTypeScore[Trk::numberOfRpcEtaHits] = 0;
  
}

//---------------------------------------------------------------------------------------------------------------------

InDet::InDetAmbiScoringTool::~InDetAmbiScoringTool()
{
}

//---------------------------------------------------------------------------------------------------------------------

StatusCode InDet::InDetAmbiScoringTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  sc = m_trkSummaryTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trkSummaryTool << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::DEBUG) << "Retrieved tool " << m_trkSummaryTool << endreq;
  
  sc = m_extrapolator.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::DEBUG) << "Retrieved tool " << m_extrapolator << endreq;
  
  // Get segment selector tool
  //
  if(m_selectortool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_selectortool <<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::DEBUG) << "Retrieved tool " << m_selectortool << endreq;
  }

  sc = m_iBeamCondSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::DEBUG) << "Could not find BeamCondSvc." << endreq;
    return StatusCode::FAILURE;
  }

  sc =  m_magFieldSvc.retrieve();
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Failed to retrieve " << m_magFieldSvc << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    msg(MSG::DEBUG) << "Retrieved " << m_magFieldSvc << endreq;
  }

  if (m_useAmbigFcn && m_useTRT_AmbigFcn) {
    msg(MSG::FATAL) << "Both on, normal ambi funciton and the one for back tracking, configuration problem, not recoverable" << endreq;
    return StatusCode::FAILURE;
  }
  
  
  if (m_incidentSvc.retrieve().isFailure()){
    ATH_MSG_WARNING("Can not retrieve " << m_incidentSvc << ". Exiting.");
    return StatusCode::FAILURE;
  }
  
  if (not m_etaDependentCutsSvc.name().empty())
    ATH_CHECK(m_etaDependentCutsSvc.retrieve());
  
  // register to the incident service: EndEvent needed for memory cleanup
  m_incidentSvc->addListener( this, "BeginEvent");

  

  if (m_useAmbigFcn || m_useTRT_AmbigFcn) setupScoreModifiers();
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------

StatusCode InDet::InDetAmbiScoringTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore InDet::InDetAmbiScoringTool::score( const Trk::Track& track, const bool suppressHoleSearch )
{
  m_holesearch = !suppressHoleSearch;

  const Trk::TrackSummary* summary;
  if ( suppressHoleSearch) {
    ATH_MSG_DEBUG ("Get summary for new Track, suppress HoleSearch");
    summary = m_trkSummaryTool->createSummaryNoHoleSearch(track);
  } else {
    ATH_MSG_DEBUG ("Get summary for new Track with HoleSearch");
    summary = m_trkSummaryTool->createSummary(track);
  }
  // move to VERBOSE
  ATH_MSG_VERBOSE ("Track has TrackSummary "<<*summary);
  Trk::TrackScore score = Trk::TrackScore( simpleScore(track, *summary) );
  ATH_MSG_DEBUG ("Track has Score: "<<score);

  delete summary;

  return score;
}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore InDet::InDetAmbiScoringTool::simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSummary )
{
  if(!m_mapFilled)
    newEvent();
  
  int numPixel          = trackSummary.get(Trk::numberOfPixelHits);
  int numSCT            = trackSummary.get(Trk::numberOfSCTHits);
  int numTRT            = trackSummary.get(Trk::numberOfTRTHits);
  int numTRTTube        = trackSummary.get(Trk::numberOfTRTTubeHits);
  int numPixelHoles     = trackSummary.get(Trk::numberOfPixelHoles);
  int numSCTHoles       = trackSummary.get(Trk::numberOfSCTHoles);
  int numSCTDoubleHoles = trackSummary.get(Trk::numberOfSCTDoubleHoles);
  int numPixelDead      = trackSummary.get(Trk::numberOfPixelDeadSensors);  
  int numSCTDead        = trackSummary.get(Trk::numberOfSCTDeadSensors);  

  if (numPixelDead<0) numPixelDead = 0;
  if (numSCTDead<0)   numSCTDead   = 0;
  
  // is this a track from the pattern or a fitted track ?
  bool ispatterntrack = (track.info().trackFitter()==Trk::TrackInfo::Unknown);
  //
  // --- reject bad tracks
  //
  //
  // --- the following cuts we only apply to fitted tracks
  //
  if (ispatterntrack) {
    ATH_MSG_DEBUG ("==> this is a pattern track, no hit cuts !");
  } else {
    ATH_MSG_DEBUG ("==> this is a refitted track, so we can use the chi2 and other stuff ! ");
    
    double trackEta = track.trackParameters()->front()->eta();

    // NdF cut :
    if (track.fitQuality()) {
      ATH_MSG_DEBUG ("numberDoF = "<<track.fitQuality()->numberDoF());
      if (track.fitQuality()->numberDoF() < m_minNDF) {
        ATH_MSG_DEBUG ("track numberDoF < "<<m_minNDF<<", reject it");
        return Trk::TrackScore(0);
      }
    }
    // Number of double Holes
    if (numSCTDoubleHoles>=0) {
      int maxDoubleHoles = m_maxDoubleHoles;
      if (not m_etaDependentCutsSvc.name().empty()) maxDoubleHoles = m_etaDependentCutsSvc->getMaxDoubleHolesAtEta(trackEta);
      if (numSCTDoubleHoles > maxDoubleHoles ) {
        ATH_MSG_DEBUG ("Track has "<< numSCTDoubleHoles <<" double holes, reject it!");
        return Trk::TrackScore(0);
      }
    }
    // Number of Si (Pixel+SCT) Holes
    if (numSCTHoles>=0 && numPixelHoles>=0) {
      int maxSiHoles = m_maxSiHoles;
      if (not m_etaDependentCutsSvc.name().empty()) maxSiHoles = m_etaDependentCutsSvc->getMaxSiHolesAtEta(trackEta);
      if (numPixelHoles+numSCTHoles > maxSiHoles ) {
        ATH_MSG_DEBUG ("Track has "<< numPixelHoles <<" Pixel and " << numSCTHoles << " SCT holes, reject it!");
        return Trk::TrackScore(0);
      }
    }
    // Number of Pixel Holes
    if ( numPixelHoles>=0 ) {
      int maxPixelHoles = m_maxPixelHoles;
      if (not m_etaDependentCutsSvc.name().empty()) maxPixelHoles = m_etaDependentCutsSvc->getMaxPixelHolesAtEta(trackEta);
      if (numPixelHoles > maxPixelHoles ) {
        ATH_MSG_DEBUG ("Track has "<< numPixelHoles <<" Pixel  holes, reject it!");
        return Trk::TrackScore(0);
      }
    }
    // Number of SCT Holes
    if ( numSCTHoles>=0 ) {
      int maxSctHoles = m_maxSctHoles;
      if (not m_etaDependentCutsSvc.name().empty()) maxSctHoles = m_etaDependentCutsSvc->getMaxSctHolesAtEta(trackEta);
      if ( numSCTHoles > maxSctHoles ) {
        ATH_MSG_DEBUG ("Track has "<< numSCTHoles << " SCT holes, reject it!");
        return Trk::TrackScore(0);
      }
    }
    // TRT cut
    if (!m_useITkAmbigFcn && numTRT > 0 && numTRT < m_minTRTonTrk) {
      ATH_MSG_DEBUG ("Track has " << numTRT << " TRT hits,  reject it");
      return Trk::TrackScore(0);
    }
    // TRT precision hits cut  
    if (!m_useITkAmbigFcn && numTRT >= 15 && ((double)(numTRT-numTRTTube))/numTRT < m_minTRTprecision) {  
      ATH_MSG_DEBUG ("Track has " << ((double)numTRTTube)/numTRT << " TRT tube hit fraction,  reject it");
      return Trk::TrackScore(0);  
    }  
    // Number of Si Clusters
    if ( numSCT>=0 && numPixel>=0) {
      int minSiClusters = m_minSiClusters;
      if (not m_etaDependentCutsSvc.name().empty()) minSiClusters = m_etaDependentCutsSvc->getMinSiHitsAtEta(trackEta);
      if (numPixel+numSCT+numPixelDead+numSCTDead < minSiClusters) {
        ATH_MSG_DEBUG ("Track has " << numPixel+numSCT << " Si clusters and " << numPixelDead+numSCTDead << " dead sensors, reject it");
        return Trk::TrackScore(0);
      }
    }
    // Number of pixel clusters
    if (numPixel>=0) {
      int minPixel = m_minPixel;
      if (not m_etaDependentCutsSvc.name().empty()) minPixel = m_etaDependentCutsSvc->getMinPixelHitsAtEta(trackEta);
      if(numPixel < minPixel) {
        ATH_MSG_DEBUG ("Track has " << numPixel << " pixel hits, reject it");
        return Trk::TrackScore(0);
      }
    }
  }
  //
  // --- kinematic selection (done as well on input ?)
  //
  // --- beam spot position 
  Amg::Vector3D beamSpotPosition(0,0,0);
  if (m_iBeamCondSvc) beamSpotPosition = m_iBeamCondSvc->beamVtx().position();
  // --- create surface
  Trk::PerigeeSurface perigeeSurface(beamSpotPosition);

  const Trk::TrackParameters* input = track.trackParameters()->front();

  double trackEta = input->eta();

  // cuts on parameters
  if (m_magFieldSvc->solenoidOn()) {
    double minPt = m_minPt;
    if (not m_etaDependentCutsSvc.name().empty()) minPt = m_etaDependentCutsSvc->getMinPtAtEta(trackEta);
    if (fabs(input->pT()) < minPt) {
      ATH_MSG_DEBUG ("Track pt < "<<minPt<<", reject it");
      return Trk::TrackScore(0);
    } 
  }
  double maxEta = m_maxEta;
  if (not m_etaDependentCutsSvc.name().empty()) maxEta = m_etaDependentCutsSvc->getMaxEta();
  if (fabs(trackEta) > maxEta) {
    ATH_MSG_DEBUG ("Track eta > "<<maxEta<<", reject it");
    return Trk::TrackScore(0);
  }

  // uses perigee on track or extrapolates, no material in any case, we cut on impacts
  // add back extrapolation without errors
  const Trk::TrackParameters* parm = m_extrapolator->extrapolateDirectly(*input, perigeeSurface);

  const Trk::Perigee*extrapolatedPerigee = dynamic_cast<const Trk::Perigee*> (parm ); 
  if (!extrapolatedPerigee) {
      msg(MSG::WARNING) << "Extrapolation of perigee failed, this should never happen" << endreq;
      delete parm;
      return Trk::TrackScore(0);
  }
  
  ATH_MSG_VERBOSE ("extrapolated perigee: "<<*extrapolatedPerigee);
  if (fabs(extrapolatedPerigee->parameters()[Trk::z0]) > m_maxZImp) {
    ATH_MSG_DEBUG ("Track Z impact > "<<m_maxZImp<<", reject it");
    delete extrapolatedPerigee;
    return Trk::TrackScore(0);
  }

  double maxD0 = m_maxRPhiImp;
  if(m_useEmClusSeed && isEmCaloCompatible( track ) ) maxD0 = m_maxRPhiImpEM;
  if (fabs(extrapolatedPerigee->parameters()[Trk::d0]) > maxD0) {
    ATH_MSG_DEBUG ("Track Rphi impact > "<<m_maxRPhiImp<<", reject it");
    delete extrapolatedPerigee;
    return Trk::TrackScore(0);
  }



  delete extrapolatedPerigee;

  //
  // --- now start scoring
  //

  if ( m_useAmbigFcn || m_useTRT_AmbigFcn) {

    //
    // --- use scoring function
    //

    return ambigScore(track,trackSummary);

  } else {
  
    //
    // use classical score
    //
    // score of 100 per track
    Trk::TrackScore score (100);
    // --- summary score analysis
    for (int i=0; i<Trk::numberOfTrackSummaryTypes; ++i) {
      int value = trackSummary.get(static_cast<Trk::SummaryType>(i));
      //value is -1 if undefined.
      if (value>0) { 
        score+=m_summaryTypeScore[i]*value; 
        ATH_MSG_DEBUG ("\tType ["<<i<<"], value \t= "<<value<<"], score \t="<<score);
      }
    }
    // --- prob(chi2,NDF), protect for chi2 <= 0
    if (track.fitQuality()!=0 && track.fitQuality()->chiSquared()>0 && track.fitQuality()->numberDoF()>0 ) {
      double p = 1.0-Genfun::CumulativeChiSquare(track.fitQuality()->numberDoF())(track.fitQuality()->chiSquared());
      if ( p > 0 )
        score += log10( p );
      else
        score -= 50;
    }    
    
    return score;
  }

}

//---------------------------------------------------------------------------------------------------------------------

Trk::TrackScore InDet::InDetAmbiScoringTool::ambigScore( const Trk::Track& track, const Trk::TrackSummary& trackSummary ) 
{
  //
  // --- start with bonus for high pt tracks
  //
  // double prob = 1.;
  double pt = fabs(track.trackParameters()->front()->pT());
  double prob = log10( pt ) - 1.; // 100 MeV is min and gets score 1
  ATH_MSG_DEBUG ("Modifier for pt = " << pt / 1000. << " GeV is: "<< prob);

  //
  // --- prob and cuts on holes
  //
  if (m_usePixel) {
    // --- Pixel holes
    int iPixHoles = trackSummary.get(Trk::numberOfPixelHoles);
    if ( iPixHoles > -1 && m_maxPixHoles > 0) {
      if (iPixHoles > m_maxPixHoles) {
        prob /= (iPixHoles - m_maxPixHoles + 1); // holes are bad ! 
        iPixHoles = m_maxPixHoles;
      }
      prob *= m_factorPixHoles[iPixHoles];
      ATH_MSG_DEBUG ("Modifier for " << iPixHoles << " Pixel holes: "<<m_factorPixHoles[iPixHoles]
         << "  New score now: " << prob);
    }  
  }

  if (m_useSCT) {
    // --- SCT holes
    int iSCT_Holes = trackSummary.get(Trk::numberOfSCTHoles);
    if (iSCT_Holes > -1 && m_maxSCT_Holes > 0) {
      if (iSCT_Holes > m_maxSCT_Holes) {
        prob /= (iSCT_Holes - m_maxSCT_Holes + 1); // holes are bad ! 
        iSCT_Holes = m_maxSCT_Holes;
      }
      prob *= m_factorSCT_Holes[iSCT_Holes];
      ATH_MSG_DEBUG ("Modifier for " << iSCT_Holes << " SCT holes: "<<m_factorSCT_Holes[iSCT_Holes]
         << "  New score now: " << prob);
    }
    // --- SCT double holes
    int iDblHoles = trackSummary.get(Trk::numberOfSCTDoubleHoles);
    if (iDblHoles > -1 && m_maxDblHoles > 0) {
      if (iDblHoles > m_maxDblHoles) {
        prob /= (iDblHoles - m_maxDblHoles + 1); // holes are bad !
        iDblHoles = m_maxDblHoles;
      }
      prob *= m_factorDblHoles[iDblHoles];
      ATH_MSG_DEBUG ("Modifier for " << iDblHoles << " double holes: "<<m_factorDblHoles[iDblHoles]
         << "  New score now: " << prob);
    }
  }
  //
  // --- prob for other counters
  //
  if (m_usePixel) {

    // ME: this if statement needs to be removed...
    // --- count layers only if holes are not searched for
    // if (trackSummary.get(Trk::numberOfPixelHoles) == -1) {

    // --- Pixel layers
    int iPixLay = trackSummary.get(Trk::numberOfContribPixelLayers);
    if (iPixLay > -1 && m_maxPixLay > 0) {
      if (iPixLay > m_maxPixLay) {
        prob *= (iPixLay - m_maxPixLay + 1); // layers are good !
        iPixLay = m_maxPixLay;
      }
      prob *= m_factorPixLay[iPixLay];
      ATH_MSG_DEBUG ("Modifier for " << iPixLay << " Pixel layers: "<<m_factorPixLay[iPixLay]
         << "  New score now: " << prob);
    }

    // --- Pixel hits
    int pixelHits = trackSummary.get(Trk::numberOfPixelHits);
    if (pixelHits > -1 && m_maxPixelHits > 0) {
      if (pixelHits > m_maxPixelHits) {
        prob *= (pixelHits - m_maxPixelHits + 1); // hits are good ! 
        pixelHits = m_maxPixelHits;
      }
      prob *= m_factorPixelHits[pixelHits];
      ATH_MSG_DEBUG ("Modifier for " << pixelHits << " Pixel hits: "<<m_factorPixelHits[pixelHits]
         << "  New score now: " << prob);
    }
    // --- Pixel blayer hits
    int bLayerHits = trackSummary.get(Trk::numberOfBLayerHits);
    if (bLayerHits > -1 && m_maxB_LayerHits > 0) {
      if (bLayerHits > m_maxB_LayerHits) {
        prob *= (bLayerHits - m_maxB_LayerHits + 1); // hits are good !
        bLayerHits = m_maxB_LayerHits;
      }
      prob *= m_factorB_LayerHits[bLayerHits];
      ATH_MSG_DEBUG ("Modifier for " << bLayerHits << " b-layer hits: "<<m_factorB_LayerHits[bLayerHits]
         << "  New score now: " << prob);
    }
    // --- Pixel Ganged fakes
    int pixelGangedFakes = trackSummary.get(Trk::numberOfGangedFlaggedFakes);
    if (pixelGangedFakes > -1 && m_maxGangedFakes > 0) {
      if (pixelGangedFakes > m_maxGangedFakes) {
        prob /= (pixelGangedFakes - m_maxGangedFakes + 1); // ganged are bad !
        pixelGangedFakes = m_maxGangedFakes;
      }
      prob *= m_factorGangedFakes[pixelGangedFakes];
      ATH_MSG_DEBUG ("Modifier for " << pixelGangedFakes << " ganged fakes hits: "<<m_factorGangedFakes[pixelGangedFakes]
         << "  New score now: " << prob);
    }
  }

  int iHits = 0;
  iHits += m_usePixel ? trackSummary.get(Trk::numberOfPixelHits) : 3;  // if Pixel off, do not count as inefficient
  iHits += m_useSCT   ? trackSummary.get(Trk::numberOfSCTHits)   : 8;  // if SCT off, do not count as inefficient
  if (iHits > -1 && m_maxHits > 0) {
    if (iHits > m_maxHits) {
      prob *= (iHits - m_maxHits + 1); // hits are good !
      iHits = m_maxHits; 
    }
    prob *= m_factorHits[iHits]; 
    ATH_MSG_DEBUG ("Modifier for " << iHits << " Sihits: "<<m_factorHits[iHits]
       << "  New score now: " << prob);
  }

  //
  // --- special treatment for TRT hits
  //
  if (!m_useITkAmbigFcn) {
    int iTRT_Hits     = trackSummary.get(Trk::numberOfTRTHits);
    int iTRT_Outliers = trackSummary.get(Trk::numberOfTRTOutliers);
    //
    if ( iTRT_Hits > 0 && m_maxTrtRatio > 0) {
      // get expected number of TRT hits
      double nTrtExpected = 30.;
      nTrtExpected = m_selectortool->minNumberDCs(const_cast<const Trk::TrackParameters*>(track.trackParameters()->front()));
      ATH_MSG_DEBUG ("Expected number of TRT hits: " << nTrtExpected << " for eta: "
         << fabs(track.trackParameters()->front()->eta()));
      double ratio = (nTrtExpected != 0) ? iTRT_Hits / nTrtExpected : 0;
      if (ratio > m_boundsTrtRatio[m_maxTrtRatio]) ratio = m_boundsTrtRatio[m_maxTrtRatio];
      for (int i=0; i<m_maxTrtRatio; ++i) {
        if ( m_boundsTrtRatio[i] < ratio && ratio <= m_boundsTrtRatio[i+1]) {
          prob*= m_factorTrtRatio[i];
          ATH_MSG_DEBUG ("Modifier for " << iTRT_Hits << " TRT hits (ratio " << ratio
                   << ") is : "<< m_factorTrtRatio[i] << "  New score now: " << prob);
          break;
        }
      } 
    }
    //
    if ( iTRT_Hits > 0 && iTRT_Outliers >= 0 && m_maxTrtFittedRatio > 0) {
      double fitted = double(iTRT_Hits) / double(iTRT_Hits + iTRT_Outliers);
      if (fitted > m_boundsTrtFittedRatio[m_maxTrtFittedRatio]) fitted = m_boundsTrtFittedRatio[m_maxTrtFittedRatio];
      for (int i=0; i<m_maxTrtFittedRatio; ++i) {
        if (fitted <= m_boundsTrtFittedRatio[i+1]) {
          prob*= m_factorTrtFittedRatio[i];
          ATH_MSG_DEBUG ("Modifier for TRT fitted ratio of " << fitted
                   << " is : "<< m_factorTrtFittedRatio[i] << "  New score now: " << prob);
          break;
        }
      } 
    }
  }

  // is this a track from the pattern or a fitted track ?
  bool ispatterntrack = (track.info().trackFitter()==Trk::TrackInfo::Unknown);

  // 
  // --- non binned Chi2
  //
  if (!ispatterntrack && !m_useLogProbBins) {
    if (track.fitQuality()!=0 && track.fitQuality()->chiSquared()>0 && track.fitQuality()->numberDoF()>0 ) {
      int    indf  = track.fitQuality()->numberDoF();
      double chi2  = track.fitQuality()->chiSquared();
      double fac   = 1. / log10 (10. + 10. * chi2 / indf); // very soft chi2 
      prob        *= fac;
      ATH_MSG_DEBUG ("Modifier for chi2 = " << chi2 << " and NDF = " << indf
         << " is : "<< fac << "  New score now: " << prob);

    }
  }
  //
  // --- fit quality prob
  //
  if ( !ispatterntrack && (m_useLogProbBins || m_useSigmaChi2) && track.fitQuality() ) {

    int    ndf  = track.fitQuality()->numberDoF();
    double chi2 = track.fitQuality()->chiSquared();
    if (ndf > 0) {

      //
      // --- first the chi2 prob
      //
      if (m_useLogProbBins) {
        double p = TMath::Prob(chi2,ndf);
        if (p > 0.) {
          p = log(p);
          if ( m_boundsLogProb[0] < p  && p <= m_boundsLogProb[m_maxLogProb] ) {
            for (int ii=0; ii<m_maxLogProb; ++ii) {
              if ( m_boundsLogProb[ii]<p && p<=m_boundsLogProb[ii+1] ) {
          prob *= m_factorLogProb[ii];
          ATH_MSG_DEBUG ("Modifier for " << p << " prob.-log.: "<< m_factorLogProb[ii]
                   << "  New score now: " << prob);
          break;
              }
            }
          } else if ( p < m_boundsLogProb[0] ) { 
            prob *= m_factorLogProb[0];
            ATH_MSG_DEBUG ("Modifier for " << p << " prob.-log.: "<< m_factorLogProb[0]
               << "  New score now: " << prob);
          } else {
            prob *= m_factorLogProb[m_maxLogProb-1];
            ATH_MSG_DEBUG ("Modifier for " << p << " prob.-log.: "<< m_factorLogProb[m_maxLogProb-1]
               << "  New score now: " << prob);
          }
        }
      }

      //
      // --- special variable for bad chi2 distribution
      //
      if (m_useSigmaChi2) {
        int sigmaChi2times100 = trackSummary.get(Trk::standardDeviationOfChi2OS);
        if (sigmaChi2times100 > 0) {
          double  testvar = double(sigmaChi2times100)/100. - sqrt(2.*chi2/ndf);
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "sigma chi2 = " << testvar << endreq;
          if ( testvar< m_boundsSigmaChi2[0] ) {
            prob *= m_factorSigmaChi2[0];
            ATH_MSG_DEBUG ("Modifier for " << testvar << " sigma chi2: "<< m_factorSigmaChi2[0]
               << "  New score now: " << prob);
          } else if (m_boundsSigmaChi2[m_maxSigmaChi2] <= testvar) {
            prob *= m_factorSigmaChi2[m_maxSigmaChi2-1];
            ATH_MSG_DEBUG ("Modifier for " << testvar << " sigma chi2: "<< m_factorSigmaChi2[m_maxSigmaChi2-1]
               << "  New score now: " << prob);
          } else {
            for (int i = 0 ; i<m_maxSigmaChi2 ; ++i ) {
              if ( m_boundsSigmaChi2[i]<=testvar && testvar<m_boundsSigmaChi2[i+1] ) {
          prob *= m_factorSigmaChi2[i];
          ATH_MSG_DEBUG ("Modifier for " << testvar << " sigma chi2: "<< m_factorSigmaChi2[i]
                   << "  New score now: " << prob);
          break;
              }
            }
          }
        }
      }
    }
  }

  return Trk::TrackScore(prob); 
}

//---------------------------------------------------------------------------------------------------------------------

void InDet::InDetAmbiScoringTool::setupScoreModifiers() 
{
  //
  // --- number of Pixel holes
  //
  // --- NewTracking and BackTracking
  const int maxPixHoles = 2; // there is an explicit cut
  const double goodPixHoles[maxPixHoles+1] = {1.0 , 0.04 , 0.004 };
  const double fakePixHoles[maxPixHoles+1] = {1.0 , 0.30 , 0.200 };
  // put it into the private members
  m_maxPixHoles = maxPixHoles;
  for (int i=0; i<=m_maxPixHoles; ++i) m_factorPixHoles.push_back(goodPixHoles[i]/fakePixHoles[i]);

  //
  // --- number of SCT holes
  //
  if (!m_useTRT_AmbigFcn) {
    // --- NewTracking
    const int maxSCT_Holes = 5; // moved from 3 -> 5 , there is an explicit cut anyway
    const double goodSCT_Holes[maxSCT_Holes+1] = { 1.0 , 0.06 , 0.010 , 0.0007, 0.0005, 0.0003  };
    const double fakeSCT_Holes[maxSCT_Holes+1] = { 1.0 , 0.15 , 0.100 , 0.0100, 0.0100, 0.0100 };
    // put it into the private members
    m_maxSCT_Holes = maxSCT_Holes;
    for (int i=0; i<=m_maxSCT_Holes; ++i) m_factorSCT_Holes.push_back(goodSCT_Holes[i]/fakeSCT_Holes[i]);
  } else {
    // --- BackTracking
    const int maxSCT_Holes = 6;
    const double goodSCT_Holes[maxSCT_Holes+1] = {0.910, 0.074, 0.014, 0.001, 0.001, 0.00001, 0.00001};
    const double fakeSCT_Holes[maxSCT_Holes+1] = {0.910, 0.192, 0.229, 0.061, 0.065, 0.016  , 0.025};
    // put it into the private members
    m_maxSCT_Holes = maxSCT_Holes;
    for (int i=0; i<=m_maxSCT_Holes; ++i) m_factorSCT_Holes.push_back(goodSCT_Holes[i]/fakeSCT_Holes[i]);
  }

  //
  // --- number of SCT double holes
  //
  // --- NewTracking and BackTracking
  const int maxDblHoles = 3; // there is a cut on this anyway !
  const double goodDblHoles[maxDblHoles+1] = { 1.  , 0.03 , 0.007 , 0.0003  };
  const double fakeDblHoles[maxDblHoles+1] = { 1.  , 0.09 , 0.09  , 0.008  };
  // put it into the private members
  m_maxDblHoles = maxDblHoles;
  for (int i=0; i<=m_maxDblHoles; ++i) m_factorDblHoles.push_back(goodDblHoles[i]/fakeDblHoles[i]);

  //
  // --- number of Blayer hits
  //
  if (!m_useTRT_AmbigFcn) {
    // --- NewTracking
    if (m_useITkAmbigFcn) {
      const int maxB_LayerHitsITk = 8;
      m_maxB_LayerHits = maxB_LayerHitsITk;
      const double blayModi[maxB_LayerHitsITk + 1] = {0.25, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5};
      for (int i = 0; i <= m_maxB_LayerHits; ++i) 
        m_factorB_LayerHits.push_back(blayModi[i]);
    } else {
      const int maxB_LayerHits = 3;
      const double goodB_LayerHits[maxB_LayerHits+1] = {0.203, 0.732, 0.081, 0.010};
      const double fakeB_LayerHits[maxB_LayerHits+1] = {0.808, 0.174, 0.018, 0.002};
      // put it into the private members
      m_maxB_LayerHits = maxB_LayerHits;
      for (int i=0; i<=m_maxB_LayerHits; ++i) 
        m_factorB_LayerHits.push_back(goodB_LayerHits[i]/fakeB_LayerHits[i]);
    }
  } else {
    // --- BackTracking
    const int maxB_LayerHits = 3;
    const double goodB_LayerHits[maxB_LayerHits+1] = {0.605, 0.349, 0.044, 0.010};
    const double fakeB_LayerHits[maxB_LayerHits+1] = {0.865, 0.124, 0.011, 0.002};
    // put it into the private members
    m_maxB_LayerHits = maxB_LayerHits;
    for (int i=0; i<=m_maxB_LayerHits; ++i) m_factorB_LayerHits.push_back(goodB_LayerHits[i]/fakeB_LayerHits[i]);
  }

  //
  // --- number of Pixel hits without Blayer
  //
  if (!m_useTRT_AmbigFcn) {
    // --- NewTracking
    if (m_useITkAmbigFcn) {
      const int maxPixelHitsITk = 26;
      m_maxPixelHits = maxPixelHitsITk;
      double maxScore = 30.0;
      double minScore = 5.00;
      int maxBarrel = 10;
      int minBarrel = 3;
      int minEnd = 5;
      
      double step[2] = {
        (maxScore - minScore) / (maxBarrel - minBarrel), (maxScore - minScore) / (maxPixelHitsITk - minEnd)
      };
      
      for (int i = 0; i <= maxPixelHitsITk; i++) {
        if (i < minEnd) m_factorPixelHits.push_back(0.01 + (i * 0.33));
        else m_factorPixelHits.push_back(minScore + ((i - minEnd) * step[1]));
      }    
    } else {
      const int maxPixelHits = 8; // we see up to 8 with IBL (was 6)
      const double goodPixelHits[maxPixelHits+1] = {0.095, 0.031, 0.118, 0.615, 0.137, 0.011, 0.01   , 0.011, 0.012};
      const double fakePixelHits[maxPixelHits+1] = {0.658, 0.100, 0.091, 0.124, 0.026, 0.002, 0.001  , 0.001, 0.001};
      m_maxPixelHits = maxPixelHits;
      for (int i=0; i<=m_maxPixelHits; ++i) 
        m_factorPixelHits.push_back(goodPixelHits[i]/fakePixelHits[i]);
    }
  } else {
    // --- BackTracking
    const int maxPixelHits = 8; // we see up to 8 with IBL (was 6)
    const double goodPixelHits[maxPixelHits+1] = {0.401, 0.079, 0.140, 0.291, 0.011, 0.078, 0.01   , 0.011, 0.012};
    const double fakePixelHits[maxPixelHits+1] = {0.673, 0.138, 0.113, 0.057, 0.002, 0.011, 0.001  , 0.001, 0.001}; 
    m_maxPixelHits = maxPixelHits;
    for (int i=0; i<=m_maxPixelHits; ++i) m_factorPixelHits.push_back(goodPixelHits[i]/fakePixelHits[i]);
  }

  //
  // --- number of Pixel layers
  //
  if (!m_useTRT_AmbigFcn) {
    // --- NewTracking
    if (m_useITkAmbigFcn) {
      const int maxPixelLayITk = 16;
      m_maxPixLay = maxPixelLayITk;
      double maxScore = 30.0;
      double minScore = 5.00;
      int maxBarrel = 10;
      int minBarrel = 3;
      int minEnd = 5;
      
      double step[2] = {
        (maxScore - minScore) / (maxBarrel - minBarrel), (maxScore - minScore) / (maxPixelLayITk - minEnd)
      };
      
      for (int i = 0; i <= maxPixelLayITk; i++) {
        if (i < minEnd) m_factorPixLay.push_back(0.01 + (i * 0.33));
        else m_factorPixLay.push_back(minScore + ((i - minEnd) * step[1]));
      }
    } else {
      const int maxPixLay = 7; // 3 barrel, 3 endcap, IBL, in practice one should see maybe 5 (was 3)
      const double goodPixLay[maxPixLay+1] = {0.095, 0.033, 0.131, 0.740,   0.840, 0.940, 1.040,1.140};
      const double fakePixLay[maxPixLay+1] = {0.658, 0.106, 0.092, 0.144,   0.144, 0.144, 0.144,0.144};
      // put it into the private members
      m_maxPixLay = maxPixLay;
      for (int i=0; i<=m_maxPixLay; ++i) m_factorPixLay.push_back(goodPixLay[i]/fakePixLay[i]);
    }
  } else {
    // --- BackTracking
    const int maxPixLay = 7; // 3 barrel, 3 endcap, IBL, in practice one should see maybe 5 (was 5)
    const double goodPixLay[maxPixLay+1] = {0.401, 0.088, 0.152, 0.355,   0.405, 0.455, 0.505, 0.555};
    const double fakePixLay[maxPixLay+1] = {0.673, 0.146, 0.115, 0.065,   0.065, 0.065, 0.065, 0.065};
    // put it into the private members
    m_maxPixLay = maxPixLay;
    for (int i=0; i<=m_maxPixLay; ++i) m_factorPixLay.push_back(goodPixLay[i]/fakePixLay[i]);
  }
  
  //
  // --- number of Pixel Ganged Fakes
  //
  // --- NewTracking and BackTracking
  const int maxGangedFakes = 2; // there is an explicit cut
  const double goodGangedFakes[maxGangedFakes+1] = {0.62 , 0.23 , 0.15 };
  const double fakeGangedFakes[maxGangedFakes+1] = {0.12 , 0.41 , 0.47 };
  // put it into the private members
  m_maxGangedFakes = maxGangedFakes;
  for (int i=0; i<=m_maxGangedFakes; ++i) m_factorGangedFakes.push_back(goodGangedFakes[i]/fakeGangedFakes[i]);

  //
  // --- total number of SCT+Pixel hits
  //
  // --- NewTracking and BackTracking
  if (m_useITkAmbigFcn) {
    const int maxHitsITk = 26;
    m_maxHits = maxHitsITk;    
    double maxScore = 40.0;
    double minScore = 5.00;
    int maxBarrel = 25;
    int minBarrel = 9;
    int minEnd = 12;
    
    double step[2] = {
      (maxScore - minScore) / (maxBarrel - minBarrel), (maxScore - minScore) / (maxHitsITk - minEnd)
    };
    
    for (int i = 0; i <= maxHitsITk; i++) {
      if (i < minEnd) m_factorHits.push_back(0.01 + (i * 1.0 / minEnd));
      else m_factorHits.push_back(minScore + ((i - minEnd) * step[1]));
    }
  } else {
    const int maxHits = 19; // there is a min cut on this anyway !
    const double goodSiHits[maxHits+1] = { 0.001 , 0.002 , 0.003 , 0.004 , 0.01 , 0.01 , 0.01 ,
             0.015 , 0.02 , 0.06 , 0.1  , 0.3  , 0.2   , 0.50 , 0.055,
             0.03  , 0.015  , 0.010  , 0.002  , 0.0005 };
    const double fakeSiHits[maxHits+1] = { 1.0   , 1.0   , 1.0   , 1.0   , 0.5 , 0.25  , 0.15 ,
             0.20  , 0.1  , 0.2  , 0.08 , 0.07 , 0.035 , 0.08 , 0.008,
             0.004 , 0.0015 , 0.0008 , 0.0001 , 0.00001 };
    // put it into the private members
    m_maxHits = maxHits;
    for (int i=0; i<=m_maxHits; ++i) m_factorHits.push_back(goodSiHits[i]/fakeSiHits[i]);
  }
  
  //
  // --- ratio of TRT hits over expected
  //
  // --- NewTracking and BackTracking
  const int maxTrtRatio = 7 ;
  const double TrtRatioBounds[maxTrtRatio+1] = {  0,   0.2,  0.4,  0.6,  0.8,  1.0,  1.2, 2.4};
  // this needs tuning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  const double goodTrtRatio[maxTrtRatio]     = {      0.05, 0.11, 0.12, 0.15, 0.20, 0.16, 0.17};
  const double fakeTrtRatio[maxTrtRatio]     = {      0.6 , 0.08, 0.06, 0.05, 0.04, 0.03, 0.03};
  // put it into the private members
  m_maxTrtRatio = maxTrtRatio;
  for (int i=0; i<m_maxTrtRatio; ++i) m_factorTrtRatio.push_back(goodTrtRatio[i]/fakeTrtRatio[i]);
  for (int i=0; i<=m_maxTrtRatio; ++i) m_boundsTrtRatio.push_back(TrtRatioBounds[i]);

  //
  // --- ratio of TRT fitted to (fitted+outliers)
  //
  // --- NewTracking and BackTracking
  const int maxTrtFittedRatio = 4;
  const double TrtFittedRatioBounds[maxTrtFittedRatio+1] = {  0,   0.3, 0.6, 0.9, 1.0};
  // this needs tuning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  const double goodTrtFittedRatio[maxTrtFittedRatio]     = {       0.1, 0.2, 0.3, 0.5};
  const double fakeTrtFittedRatio[maxTrtFittedRatio]     = {       0.6, 0.1, 0.1, 0.1};
  // put it into the private members
  m_maxTrtFittedRatio = maxTrtFittedRatio;
  for (int i=0; i<m_maxTrtFittedRatio; ++i) m_factorTrtFittedRatio.push_back(goodTrtFittedRatio[i]/fakeTrtFittedRatio[i]);
  for (int i=0; i<=m_maxTrtFittedRatio; ++i) m_boundsTrtFittedRatio.push_back(TrtFittedRatioBounds[i]);

  //
  // --- chi2 prob
  //
  if (!m_useLogProbBins) {
    m_maxLogProb = -1 ;
  } else {
    if (!m_useTRT_AmbigFcn) {
      // --- NewTracking
      const int maxLogProb = 10;
      const double LogProbBounds[maxLogProb+1] = {-10., -9., -8., -7., -6., -5., -4., -3., -2., -1., 0.};
      const double goodLogProb[maxLogProb] = {0.01, 0.01, 0.015, 0.02, 0.025, 0.045, 0.08, 0.11 , 0.21, 0.45};
      const double fakeLogProb[maxLogProb] = {0.04, 0.04, 0.05 , 0.06, 0.06 , 0.07 , 0.09, 0.115, 0.15, 0.18};
      // put it into the private members
      m_maxLogProb = maxLogProb;
      for (int i=0; i<m_maxLogProb; ++i) m_factorLogProb.push_back(goodLogProb[i]/fakeLogProb[i]);
      for (int i=0; i<=m_maxLogProb; ++i) m_boundsLogProb.push_back(LogProbBounds[i]);
    } else {
      // --- BackTracking
      const int maxLogProb = 12;
      const double LogProbBounds[maxLogProb+1] = {-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0};
      const double goodLogProb[maxLogProb] = {0.002, 0.002, 0.004, 0.007, 0.008, 0.012, 0.020, 0.033, 0.051, 0.104, 0.219, 0.531};
      const double fakeLogProb[maxLogProb] = {0.020, 0.025, 0.032, 0.030, 0.042, 0.051, 0.063, 0.071, 0.080, 0.112, 0.156, 0.216};
      // put it into the private members
      m_maxLogProb = maxLogProb;
      for (int i=0; i<m_maxLogProb; ++i) m_factorLogProb.push_back(goodLogProb[i]/fakeLogProb[i]);
      for (int i=0; i<=m_maxLogProb; ++i) m_boundsLogProb.push_back(LogProbBounds[i]);
    }
  }

  //
  // --- sigma chi2
  //
  if (!m_useSigmaChi2) {
    m_maxSigmaChi2 = -1 ;
  } else {
    // --- NewTracking and BackTracking
    const int maxSigmaChi2 = 13 ;
    const double SigmaChi2Bounds[maxSigmaChi2+1] = { -5.,-4.,-3.,-2.,-1.,0.,1.,2.,3.,4.,5.,6.,7.,8.};
    const double goodSigmaChi2[maxSigmaChi2] = {0.00004, 0.0004, 0.002, 0.15, 0.8, 0.015, 0.01 , 0.009, 0.008, 0.0007 , 0.0006 , 0.0005, 0.00004};
    const double fakeSigmaChi2[maxSigmaChi2] = {0.0008 , 0.005 , 0.02 , 0.2 , 0.3, 0.1  , 0.1  , 0.1  , 0.1  , 0.01   , 0.01   , 0.01  , 0.001};
    // put it into the private members
    m_maxSigmaChi2 = maxSigmaChi2;
    for (int i=0; i<m_maxSigmaChi2; ++i) m_factorSigmaChi2.push_back(goodSigmaChi2[i]/fakeSigmaChi2[i]);
    for (int i=0; i<=m_maxSigmaChi2; ++i) m_boundsSigmaChi2.push_back(SigmaChi2Bounds[i]);
  }

  //
  // --- debug output
  //
  if (msgLvl(MSG::VERBOSE)) { 
    
    for (int i=0; i<=m_maxPixHoles; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " Pixel holes: " << m_factorPixHoles[i] <<endreq;
    
    for (int i=0; i<=m_maxSCT_Holes; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " SCT holes: " << m_factorSCT_Holes[i] <<endreq;
    
    for (int i=0; i<=m_maxDblHoles; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " double SCT holes: " << m_factorDblHoles[i] <<endreq;
    
    for (int i=0; i<=m_maxPixLay; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " Pixel layers: " << m_factorPixLay[i] <<endreq;
    
    for (int i=0; i<=m_maxB_LayerHits; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " b-layer hits: " << m_factorB_LayerHits[i] <<endreq;
    
    for (int i=0; i<=m_maxPixelHits; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " Pixel hits: " << m_factorPixelHits[i] <<endreq;
    
    for (int i=0; i<=m_maxGangedFakes; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " ganged fakes: " << m_factorGangedFakes[i] <<endreq;
    
    for (int i=0; i<=m_maxHits; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << i << " Si hits: " << m_factorHits[i] <<endreq;
    
    for (int i=0; i<m_maxTrtRatio; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << m_boundsTrtRatio[i] << " < TRT ratio  < "
      << m_boundsTrtRatio[i+1] <<"  : " <<m_factorTrtRatio[i] <<endreq;
    
    for (int i=0; i<m_maxTrtFittedRatio; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << m_boundsTrtFittedRatio[i] << " < TRT fitted ratio  < "
      << m_boundsTrtFittedRatio[i+1] <<"  : " <<m_factorTrtFittedRatio[i] <<endreq;
    
    // only if used
    for (int i=0; i<m_maxLogProb; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << m_boundsLogProb[i] << " < log(P)  < "
      << m_boundsLogProb[i+1] <<"  : " <<m_factorLogProb[i] <<endreq;
    
    // only if used
    for (int i=0; i<m_maxSigmaChi2; ++i)
      msg(MSG::VERBOSE) << "Modifier for " << m_boundsSigmaChi2[i] << " < sigma(chi2) - sqrt(2chi2)  < " << m_boundsSigmaChi2[i+1]
      <<"  : " <<m_factorSigmaChi2[i] <<endreq;
  }

}


void InDet::InDetAmbiScoringTool::handle(const Incident& inc) 
{ 
  // the cluster ambiguity map
  if ( inc.type() == IncidentType::BeginEvent ){
    m_mapFilled =false;
  }  
}


void InDet::InDetAmbiScoringTool::newEvent()
{
  // Reload ROI's
  if(m_useEmClusSeed) {
    m_emF.clear();
    m_emE.clear();
    m_emR.clear();
    m_emZ.clear();
    
    const CaloClusterROI_Collection* calo = 0;
    StatusCode sc = evtStore()->retrieve(calo,m_inputEmClusterContainerName);

    if(sc == StatusCode::SUCCESS && calo) {
      for( auto& ccROI : *calo) {
        if( ccROI->energy() * sin(ccROI->globalPosition().theta()) < m_minPtEm){ 
          continue;
        }  
        m_emF.push_back( ccROI->globalPosition().phi() );
        m_emE.push_back( ccROI->globalPosition().eta() );
        m_emR.push_back( ccROI->globalPosition().perp() );
        m_emZ.push_back( ccROI->globalPosition().z() );
      }
    }
  }
  m_mapFilled = true;
}


//==========================================================================================
bool InDet::InDetAmbiScoringTool::isEmCaloCompatible(const Trk::Track& track) const
{

  
  const Trk::TrackParameters * Tp = track.trackParameters()->front();
  
  
  //Switch to the track parameters of the first measurment instead of the perigee parameters 
  ATH_MSG_VERBOSE ("--> Looping over TSOS's");
  for (auto tsos : *track.trackStateOnSurfaces() ) {
    // get measurment from TSOS
    auto meas = tsos->measurementOnTrack();
    auto tp = tsos->trackParameters();
    // if we do not have a measurement, we should just mark it
    if (!meas || !tp) {
      continue;
    } else {
      Tp = tp;
      break;
    }
  }
  
  
  
  const double pi = M_PI, pi2 = 2.*M_PI;
  if(m_emF.empty()) return false;

  auto f = m_emF.begin(), fe = m_emF.end();
  auto e = m_emE.begin();
  auto r = m_emR.begin();
  auto z = m_emZ.begin();

  double F = Tp->momentum().phi();
  double E = Tp->momentum().eta();
  double R = Tp->position().perp();
  double Z = Tp->position().z();

  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidthEm) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Z;
      double newR   = *r - R;
      double newEta =  atanh( newZ / sqrt( newR*newR + newZ*newZ ) );
      double de = fabs(E-newEta);
       
      if(de < m_etaWidthEm) return true;
    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}



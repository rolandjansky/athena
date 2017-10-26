/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauConversionTagger.cxx
// package:     Reconstruction/tauRec
// authors:     Dimitris Varouchas
// date:        2013-11-08
//
//
//-----------------------------------------------------------------------------
//TODO:

//#include <GaudiKernel/IToolSvc.h>
//#include <GaudiKernel/ListItem.h>

//#include "FourMomUtils/P4Helpers.h"
//#include "FourMom/P4EEtaPhiM.h"
//#include "CLHEP/Vector/LorentzVector.h"
//#include "Particle/TrackParticle.h"

//#include "TrkParameters/TrackParameters.h"

#include "tauRecTools/TauEventData.h"
//#include "tauEvent/TauCommonDetails.h"
//#include "tauEvent/TauJetParameters.h"

#include "TauConversionTagger.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauConversionTagger::TauConversionTagger(const std::string &name) :
  TauRecToolBase(name),
  m_TrkIsConv(false),
  m_TRTHighTOutliersRatio(0),
  m_trackToVertexTool("Reco::TrackToVertex")
{
    declareProperty("ConversionTaggerVersion", m_ConvTaggerVer = 1);
    declareProperty("TrackContainerName", m_trackContainerName = "InDetTrackParticles");
    declareProperty("TrackToVertexTool", m_trackToVertexTool);
    declareProperty ("TRTRatio", m_doTRTRatio = true);
    declareProperty ("FullInfo", m_storeFullSummary = false);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauConversionTagger::~TauConversionTagger() {
}


//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauConversionTagger::initialize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauConversionTagger::finalize() {
    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauConversionTagger::execute(xAOD::TauJet& pTau) {

  StatusCode sc;

  sc = m_trackToVertexTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve TrackToVertexTool");
    return StatusCode::FAILURE;
  } 

  //2012 data reporocessing bug
  //events with no vertices had taus w/ associated tracks
  if(pTau.vertexLink().isValid()==0) return StatusCode::SUCCESS;

  for(unsigned int j=0; j<pTau.nTracks(); j++ ) {

    const xAOD::TrackParticle *TauJetTrack = pTau.track(j)->track();
    const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*TauJetTrack, (*pTau.vertexLink())->position());

    // Declare TrackSummary info
    // Note: all must be of type uint8_t for summaryValue filling to work in xAOD
    // TODO: check if these default values are sane
    uint8_t nBLHits             = 0;
    uint8_t expectInnermostPixelLayerHit     = 0;
    uint8_t nTRTHighTHits       = 0;
    uint8_t nTRTHighTOutliers   = 0;
    float   nTRTHighT_outl      = 0.;
    uint8_t nTRTXenon           = 0;
    uint8_t nTRTHits            = 0;
    uint8_t nTRTOutliers        = 0;

    // Fill TrackSummary info
    TauJetTrack->summaryValue(nBLHits,xAOD::numberOfInnermostPixelLayerHits);
    TauJetTrack->summaryValue(expectInnermostPixelLayerHit,xAOD::expectInnermostPixelLayerHit);
    TauJetTrack->summaryValue(nTRTHighTHits,xAOD::numberOfTRTHighThresholdHits);
    TauJetTrack->summaryValue(nTRTHighTOutliers,xAOD::numberOfTRTHighThresholdOutliers);
    nTRTHighT_outl = nTRTHighTHits + nTRTHighTOutliers;
    TauJetTrack->summaryValue(nTRTXenon,xAOD::numberOfTRTXenonHits);
    TauJetTrack->summaryValue(nTRTHits,xAOD::numberOfTRTHits);
    TauJetTrack->summaryValue(nTRTOutliers,xAOD::numberOfTRTOutliers);

    // TODO: check if default value is sane
    m_TRTHighTOutliersRatio = 0.;
    if (m_doTRTRatio || m_storeFullSummary) {
      if (nTRTXenon > 0)
        m_TRTHighTOutliersRatio = nTRTHighT_outl / nTRTXenon;
      else if (nTRTHits + nTRTOutliers > 0)
        m_TRTHighTOutliersRatio = nTRTHighT_outl / (nTRTHits+nTRTOutliers);
    }

    double pt = TauJetTrack->pt();
    double d0 = perigee->parameters()[Trk::d0];


    m_TrkIsConv = false;

    double Rconv = sqrt(fabs(d0)*pt/(0.15*2.));
    double auxprod= d0 * perigee->parameters()[Trk::qOverP];
    double rconvii= auxprod > 0 ? Rconv : -Rconv;
    delete perigee; //cleanup necessary to prevent mem leak

    if ( m_ConvTaggerVer==0 ) {

      m_a_cut[0][0]=0.0003;  m_b_cut[0][0]=0.1725;
      m_a_cut[0][1]=0.0003;  m_b_cut[0][1]=0.2025;

      if ( nBLHits==0 && expectInnermostPixelLayerHit ){
	if( m_TRTHighTOutliersRatio > -m_a_cut[0][0]*Rconv + m_b_cut[0][0] && (-rconvii) > 40 && pt < 20000 ) m_TrkIsConv=true;
      }
      else {
	if( m_TRTHighTOutliersRatio > -m_a_cut[0][1]*Rconv + m_b_cut[0][1] && (-rconvii) > 40 && pt < 20000 ) m_TrkIsConv=true;
    	}
    }

    else if ( m_ConvTaggerVer==1 ) {

      m_a_cut[1][0]=0.0003;  m_b_cut[1][0]=0.1725;
      m_a_cut[1][1]=0.0003;  m_b_cut[1][1]=0.2025;


      if(nBLHits==0 ){
      	if( m_TRTHighTOutliersRatio > -m_a_cut[1][0]*Rconv + m_b_cut[1][0] && (-rconvii) > 40 && pt < 20000 ) m_TrkIsConv=true;
      }
      else {
      	if( m_TRTHighTOutliersRatio > -m_a_cut[1][1]*Rconv + m_b_cut[1][1] && (-rconvii) > 40 && pt < 20000 ) m_TrkIsConv=true;
      }
    }

    else {

      ATH_MSG_WARNING("No tau conversion tagger compatible with version "<<m_ConvTaggerVer);
      return false;
    }

    ATH_MSG_VERBOSE("Is tau track a conversion? : " << m_TrkIsConv);
    // if (m_TrkIsConv && !pTau.trackFlag(TauJetTrack, xAOD::TauJetParameters::isConversion))
    //   pTau.setTrackFlag(TauJetTrack, xAOD::TauJetParameters::isConversion, true);
    xAOD::TauTrack* tauTrack = pTau.trackNonConst(j);
    if(m_TrkIsConv && !tauTrack->flag(xAOD::TauJetParameters::isConversionOld))
      tauTrack->setFlag( xAOD::TauJetParameters::isConversionOld, true);
  }

  return StatusCode::SUCCESS;
}

#endif

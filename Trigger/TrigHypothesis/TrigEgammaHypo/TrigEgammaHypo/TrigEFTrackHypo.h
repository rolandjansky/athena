/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFTrackHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   P. Conde Muino
 *           I. Grabowska-Bold, update Sept 2006
 * CREATED:  April 29, 2004
 * MODIFIED: V. Perez Reale  added doxygen comments 25/6/07 
 *
 *********************************************************************/
#ifndef TRIGEFTRACKHYPO_H
#define TRIGEFTRACKHYPO_H

//#include <string
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "Particle/TrackParticleContainer.h"

class StoreGateSvc;
class TriggerElement;


/**
 * \class TrigEFTrackHypo
 * \brief TrigEFTrackHypo is a Trigger Hypothesis  Algorithm that retrieves the EF TrackParticle
 * container created by the EF ID algorithms
 * Applies a pT cut on the track and the event is accepted if this is satisfied
 *
 */

class TrigEFTrackHypo : public HLT::HypoAlgo {

 public:

  TrigEFTrackHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFTrackHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  HLT::ErrorCode hltFinalize();
  

 private:

  // define the properties:
  //----------------------------
  int m_numBLayerHits;
  int m_numPixelHits;
  int m_numSCTHits;
  int m_numTRTHits;
  float m_TRTRatio;
  float m_chi2;
  float m_d0;
  float m_pt;
  int m_numTracks;

  // Cuts to be applied:
  bool m_acceptAll; //!<  true all events are taken
  bool m_applyTrackCut;

  // functions to retrieve monitored quantities

  const Rec::TrackParticleContainer* TrkParticleCont; //!<  pointer to TrackParticle container
  int m_numTrkPart;


/**
   * \brief implements histograms for monitoring
   */

  static inline double monPt(const Rec::TrackParticle * t)
    {
      const Trk::Perigee* aMeasPer = t->measuredPerigee();
      double ptSquare=pow(aMeasPer->momentum()[Trk::px],2.) + pow(aMeasPer->momentum()[Trk::py],2.);
      return (ptSquare>0.)?sqrt(ptSquare)/1000.:-sqrt(fabs(ptSquare))/1000.;
    }

  static inline double monPhi(const Rec::TrackParticle * t)
    {
      const Trk::Perigee* aMeasPer = t->measuredPerigee();
      return (aMeasPer->parameters()[Trk::phi]);
    }

  static inline double monEta(const Rec::TrackParticle * t)
    {
      const Trk::Perigee* aMeasPer = t->measuredPerigee();
      return (-std::log(tan(aMeasPer->parameters()[Trk::theta]/2.)));
    }

  static inline double mond0(const Rec::TrackParticle * t)
    {
      const Trk::Perigee* aMeasPer = t->measuredPerigee();
      return (aMeasPer->parameters()[Trk::d0]);
    }

  static inline double monz0(const Rec::TrackParticle * t)
    {
      const Trk::Perigee* aMeasPer = t->measuredPerigee();
      return (aMeasPer->parameters()[Trk::z0]);
    }

};
#endif


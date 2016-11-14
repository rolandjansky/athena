/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFHADCALIBHYPO_H
#define TRIGEFHADCALIBHYPO_H
/********************************************************************
 *
 * NAME:     EFHadCalibHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   Mario Sousa (mdacunha@cern.ch)
 * CREATED:  Jan 18, 2012 
 * Modified: Dec 03, 2012
 *
 * co-author : P. Conde
 *
 * DESCRIPTION:
 *
 * Implementation of hadCalib trigger with an extrapolation to the 
 *calorimeter at EF.
 *
 *********************************************************************/

#include "TrigInterfaces/HypoAlgo.h"
//#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "Particle/TrackParticleContainer.h"

class StoreGateSvc;
class  VxContainer;

namespace HLT {
  class TriggerElement;
}

class EFHadCalibHypo : public HLT::HypoAlgo
{

 public:

  /** constructor of EF tauRec hypo algo */
  EFHadCalibHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~EFHadCalibHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  /** HLT method to check a track quality criteria
      input is a Rec::TrackParticleContainer::const_iterator
      output gives track quality pass= True or False */
  HLT::ErrorCode hltTrackQualityCheck(Rec::TrackParticleContainer::const_iterator trackIter,Trk::Vertex beamposition);

 private:

  /** Cuts to be applied */

  /** Cut for minimum track Pt */
  double    m_PtMinCut;

  /** Max number of pixel hits of the leading track **/
  int m_minPixHits;

  /** Min number of SCT hits of the leading track **/
  int m_minSCTHits;

  /** Max absolute value for the d0 the leading track **/
  double m_maxD0;

  /** Max absolute value for the z0 the leading track **/
  double m_maxZ0;

  /** Max absolute value for the z0 the leading track **/
  double m_maxEta;

  /** Cut on Delta R */
  double    m_trackIsoDR;
  
  /** How many tracks are allowed in the isolation cone **/
  int       m_nMaxTracksOnDeltaR;

  /** Max pT allowed for a track in the isolation region **/
  double    m_maxPtInIso;


   // Monitoring variables

   // high pt tracks (HT)
  double m_HTpT;
  double m_HTeta;
  double m_HTphi;
  double m_HTz0;
  double m_HTd0;
  int    m_HTnPixHit;
  int    m_HTnSCTHit;

   // Number of tracks in isolation region
  int m_nTracksInIso;

   // Cut counter
  int m_cutCounter;

  // Additional cut counter (tracks)
  int m_cutCounter2;

  //sum pT in isolation region:
  //int m_sumPtInIso;
  /** First decision to accept or reject the high pt track **/
  bool m_DQtrackPass;
 
  /* track to calo extrapolation tool */
  ToolHandle<Reco::ITrackToVertex>   m_toVertex;

};
#endif


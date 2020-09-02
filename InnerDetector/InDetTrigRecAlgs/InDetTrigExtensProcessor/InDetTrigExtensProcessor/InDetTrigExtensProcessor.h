/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
//
// filename: InDetTrigExtensProcessor.h
//
// author: Patricia Conde Muino 07/09/2005
//         Patricia.Conde.Muino@cern.ch
//
// Description:  Trigger version of the InDetExtensionProcessor
//
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGEXTENSIONPROCESSOR_H
#define INDETTRIGEXTENSIONPROCESSOR_H

// Base class
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

//needed for RunOutlierRemoval
#include "TrkFitterUtils/FitterTypes.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"

// Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include <vector>

namespace InDet {

 /** @class InDetTrigExtensProcessor

     Algorithms that processes a TrackExtensionMap, refits the
     extended tracks and decides if the old track gets replaces
     
     @author Markus.Elsing@cern.ch
     */
 
class InDetTrigExtensProcessor : public HLT::FexAlgo  {

public:

  /** Constructor with parameters: */
  InDetTrigExtensProcessor(const std::string &name,ISvcLocator *pSvcLocator);

  /** public methods: */
  virtual HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
  virtual HLT::ErrorCode hltFinalize();

  //Some counters for monitoring
  int m_numNewTracks; //number of tracks afterwards should equal...
  int m_numTracks;    //number of tracks going in
  int m_numExtenTracks; // number of tracks inmproved by extension and kept
  int m_numOrigTracks; // number of better original tracks

private:
   /** private methods: */
  Trk::Track* trackPlusExtension(const Trk::Track*,
			const std::vector<const Trk::MeasurementBase*>&) const;

  // --- fitter steering
  bool                          m_refitPrds;
  Trk::RunOutlierRemoval        m_runOutlier;          //!< switch whether to run outlier logics or not
  int                           m_matEffects;          //!< type of material interaction in extrapolation
  bool                          m_suppressHoleSearch;  //!< no hole search
  Trk::ParticleHypothesis       m_ParticleHypothesis;  //!< nomen est omen 
  bool                          m_keepFailedExtensionOnTrack; //!< add rejected extension as outlier trajectory  

  // -- algorithm members
  const TrackCollection*        m_tracks;
  const TrackExtensionMap*      m_trackExtensionMap;
  ToolHandle< Trk::ITrackFitter > m_ITrackFitter;
  //

  /**\todo make this const, once createSummary method is const*/
  ToolHandle<Trk::IExtendedTrackSummaryTool> m_trkSummaryTool;
  ToolHandle< Trk::ITrackScoringTool > m_scoringTool;
  TrackCollection*              m_newtracks;

  bool                          m_cosmics;          //!< whether we are running cosmics

};

} 

#endif //INDETEXTENSIONPROCESSOR_H


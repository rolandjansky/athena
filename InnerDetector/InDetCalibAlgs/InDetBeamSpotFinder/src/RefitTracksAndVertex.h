/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @file RefitTracksAndVertex.cxx
//

//
// $Id: RefitTracksAndVertex.h,v 1.8 2009-05-18 15:48:05 moles Exp $
 
#ifndef RefitTracksAndVertex_H
#define RefitTracksAndVertex_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

class AtlasDetectorID ;

namespace Trk {
  class ITrackFitter ;
  class IVertexFitter ;
  class Track ;
  class TrackStateOnSurface ;
  class PseudoMeasurementOnTrack ;
}

class RefitTracksAndVertex: public AthAlgorithm {
 public:
  /** The RefitTracksAndVertex is an implementation to add the so-called TRT momentum constraint on a track.
      It was developed in the context of alignment, and its main idea is that if the TRT is free from "weak modes",
      its curvature measurement can be added as a PseudoMeasurement (PM) to a Si-only track. This extra information,
      taken into account while refitting the Si-only part of the track or read in by the alignment algorithms
      directly, can serve to eliminate the alignment weak modes from the Si tracker of ATLAS.
      More details can be found at: https://twiki.cern.ch/twiki/bin/view/Atlas/TRTMomConstraint */
  RefitTracksAndVertex(const std::string& name, ISvcLocator* pSvcLocator) ;
  ~RefitTracksAndVertex() ;
  StatusCode initialize() ; //!< initialize method of this algorithm.   
  StatusCode execute()    ; //!< execute method of this algorithm that is called for each event  
  StatusCode finalize()   ; //!< finalize method of this algorithm. Prints out a summary of all events
 
  MsgStream& dump( MsgStream& outst ) const ;

 private:
  // helper functions
  /** Verifies if the given track passes the track selection criteria specified via the jobOptions */
  bool accept( const Trk::Track& track ) ;
  /** adds a PseudoMeasurement to a MeasurementSet */
  const Trk::MeasurementSet addPM( Trk::MeasurementSet& ms, const Trk::PseudoMeasurementOnTrack* pm ) ;
  /** creates a PseudoMeasurement with (z0, theta) from extended track perigee parameters */
  const Trk::PseudoMeasurementOnTrack* createPMfromSi ( const Trk::Perigee* mp ) ;
   /** Strips of all TRT hits from the track and replaces them with a TRT momentum constraint 
  from a the TRT segment belonging to the (extended) track (returns NULL in case of failure) */
  Trk::Track* fitSCTOnlyTrack( const Trk::Track* track ) ;

  // infrastructure  
  const AtlasDetectorID*  m_idHelper  ; //!< Detector ID helper
  std::string m_vertexListInput  ; //!< Name of the TrackCollection (input)
  
  
  
  std::string m_trackListOutput ; //!< Name of the TrackCollection (Output)
  std::string m_outputVertexContainerName ; //!< Name of vertex container
  ToolHandle<Trk::ITrackFitter> m_trackFitter ; //!< The TrackFitter to refit the tracks (segment, momentum constraint)
  ToolHandle<Trk::IVertexFitter> m_vertexFitter ; //!< The TrackFitter to refit the tracks (segment, momentum constraint)

  // algorithm-internal members
  bool   m_applyTrkSel        ; //!< apply a selection on tracks or not
  bool   m_refitTracks        ; //!< refitTracks
  bool   m_addPM              ; //!< apply a pseudo measurement based on the original track (theta,z0)
  double m_selPtMin           ; //!< minimal pT cut value for the TrackSelection
  double m_selEtaMin          ; //!< minimal eta cut value for the TrackSelection
  double m_selEtaMax          ; //!< maximal eta cut value for the TrackSelection
  int    m_selNHitPIXMin      ; //!< minimal number of PIX hits cut value for the TrackSelection
  int    m_selNHitSCTMin      ; //!< minimal number of SCT hits cut value for the TrackSelection
  size_t m_nTracksProcessed   ; //!< Counter for number of tracks processed
  size_t m_nTracksPresel      ; //!< Counter for number of tracks passing the preselection
  size_t m_nTracksAccepted    ; //!< Counter for number of tracks passing the preselection and with PM
  size_t m_nRejectPIX         ; //!< Counter for number of tracks failing the min number of PIX hits req.
  size_t m_nRejectSCT         ; //!< Counter for number of tracks failing the min number of SCT hits req.
  size_t m_nRejectTRT         ; //!< Counter for number of tracks failing the min number of TRT hits req.
  size_t m_nRejectPM          ; //!< Counter for number of tracks failing the addition of a pseudo-measurement (PM)
} ;

MsgStream& operator<<( MsgStream& outst, const RefitTracksAndVertex& alg ) ;

#endif

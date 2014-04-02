/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// @file AddTRTMomConstr.cxx
//
// @brief Algorithm for 
// This code is derived from TRT_AlignAlgs/AddTRTMomConstr.h/.cxx
//
// @author Oleg Brandt
// many thanks to the TRTAlignAlgs designers, Pawel Bruckman, Markus Elsing and Thijs Cornelissen!
//
// $Id: AddTRTMomConstr.h,v 1.8 2009-05-18 15:48:05 moles Exp $
 
#ifndef INDETALIGNGENALGS_ADDTRTMOMCONSTR_H
#define INDETALIGNGENALGS_ADDTRTMOMCONSTR_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

class TRT_ID ;
class AtlasDetectorID ;

namespace Trk {
  class ITrackFitter ;
  class Track ;
  class ITrackSummaryTool ;
  class TrackStateOnSurface ;
  class PseudoMeasurementOnTrack ;
}

class AddTRTMomConstr: public AthAlgorithm {
 public:
  /** The AddTRTMomConstr is an implementation to add the so-called TRT momentum constraint on a track.
      It was developed in the context of alignment, and its main idea is that if the TRT is free from "weak modes",
      its curvature measurement can be added as a PseudoMeasurement (PM) to a Si-only track. This extra information,
      taken into account while refitting the Si-only part of the track or read in by the alignment algorithms
      directly, can serve to eliminate the alignment weak modes from the Si tracker of ATLAS.
      More details can be found at: https://twiki.cern.ch/twiki/bin/view/Atlas/TRTMomConstraint */
  AddTRTMomConstr(const std::string& name, ISvcLocator* pSvcLocator) ;
  ~AddTRTMomConstr() ;
  StatusCode initialize() ; //!< initialize method of this algorithm.   
  StatusCode execute()    ; //!< execute method of this algorithm that is called for each event  
  StatusCode finalize()   ; //!< finalize method of this algorithm. Prints out a summary of all events
 
  MsgStream& dump( MsgStream& outst ) const ;

 private:
  // helper functions
  /** Returns the outermost SCT TSOS on the given Track */
  const Trk::TrackStateOnSurface* findouterscthit( const Trk::Track* track ) ;
  /** Returns the innermost TRT TSOS on the given Track */
  const Trk::TrackStateOnSurface* findinnertrthit( const Trk::Track* track ) ;
  /** Verifies if the given track passes the track selection criteria specified via the jobOptions */
  bool accept( const Trk::Track& track ) ;
  /** adds a PseudoMeasurement to a MeasurementSet */
  const Trk::MeasurementSet addPM( Trk::MeasurementSet& ms, const Trk::PseudoMeasurementOnTrack* pm ) ;
  /** creates a PseudoMeasurement with (z0, theta) from extended track perigee parameters */
  const Trk::PseudoMeasurementOnTrack* createPMfromSi ( const Trk::Perigee* mp ) ;
  /** creates a PseudoMeasurement with (z0, theta, q/p) from TRT track perigee parameters */
  const Trk::PseudoMeasurementOnTrack* createPMfromTRT( const Trk::Perigee* mpSi
                                                      , const Trk::Perigee* mpTRT ) ;
  /** Strips of all TRT hits from the track and replaces them with a TRT momentum constraint 
  from a the TRT segment belonging to the (extended) track (returns NULL in case of failure) */
  Trk::Track* addTRTMomentumConstraint( const Trk::Track* track ) ;

  // infrastructure  
  const AtlasDetectorID*  m_idHelper  ; //!< Detector ID helper
  const TRT_ID*     m_trtid     ; //!< Identifier helper class for the TRT
  std::string m_trackListInput  ; //!< Name of the TrackCollection (input)
  std::string m_trackListOutput ; //!< Name of the TrackCollection (Output)
  ToolHandle<Trk::ITrackSummaryTool> m_trackSummaryTool ; //!<The summary tool to make the track selection
  ToolHandle<Trk::ITrackFitter> m_trackFitter ; //!< The TrackFitter to refit the tracks (segment, momentum constraint)

  // algorithm-internal members
  bool   m_useThetaCorrection ; //!< correct theta after TRT+PM(z0,theta) refit, small diff. typically < o(3x10e-4)
  double m_thetaCorr          ; //!< theta can be different after TRT+PM fit by < o(10e-4). Correct q/p optionally
  bool   m_applyTrkSel        ; //!< apply a selection on tracks or not
  double m_selPtMin           ; //!< minimal pT cut value for the TrackSelection
  double m_selEtaMin          ; //!< minimal eta cut value for the TrackSelection
  double m_selEtaMax          ; //!< maximal eta cut value for the TrackSelection
  int    m_selNHitPIXMin      ; //!< minimal number of PIX hits cut value for the TrackSelection
  int    m_selNHitSCTMin      ; //!< minimal number of SCT hits cut value for the TrackSelection
  int    m_selNHitTRTMin      ; //!< minimal number of TRT hits cut value for the TrackSelection
  int    m_selNHitTRTMinCrack ; //!< minimal number of TRT hits cut value in trans. region for the TrackSelection
  double m_selEtaCrackMin     ; //!< minimum |eta| to define the transition region
  double m_selEtaCrackMax     ; //!< maximum |eta| to define the transition region
  double m_selChiSqPerDOFMin  ; //!< maximal Chisquare per degree of freedom cut value for the TrackSelection
  size_t m_nTracksProcessed   ; //!< Counter for number of tracks processed
  size_t m_nTracksPresel      ; //!< Counter for number of tracks passing the preselection
  size_t m_nTracksAccepted    ; //!< Counter for number of tracks passing the preselection and with PM
  size_t m_nRejectPIX         ; //!< Counter for number of tracks failing the min number of PIX hits req.
  size_t m_nRejectSCT         ; //!< Counter for number of tracks failing the min number of SCT hits req.
  size_t m_nRejectTRT         ; //!< Counter for number of tracks failing the min number of TRT hits req.
  size_t m_nRejectPM          ; //!< Counter for number of tracks failing the addition of a pseudo-measurement (PM)
} ;

MsgStream& operator<<( MsgStream& outst, const AddTRTMomConstr& alg ) ;

#endif

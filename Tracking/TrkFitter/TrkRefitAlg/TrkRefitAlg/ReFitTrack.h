/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ReFit.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef TRKREFITALG_REFITTRACK_H
#define TRKREFITALG_REFITTRACK_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkTrack/Track.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class IBeamCondSvc;
class VxContainer;

namespace Trk{

class ITrackFitter;
class IExtrapolator;
class ITrackSummaryTool;
class ITrackSelectorTool;
class IPRD_AssociationTool;

 /** @brief Algorithm using an instance of a ITrackFitter
     to refit the tracks of a given track collection.
     
     Constraint fits are available:
     - Option 1 : fit the track with the vertex (first vertex is taken)
     - Option 2 : fit the track with the beamspot

     @author Maria.Jose.Costa@cern.ch, Andreas.Salzburger@cern.ch
     */
 
class ReFitTrack : public AthAlgorithm  {

public:

  typedef Trk::Track Track;

  //! standard Algorithm constructor
  ReFitTrack(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  ReFitTrack();
  ReFitTrack(const ReFitTrack&);
  ReFitTrack &operator=(const ReFitTrack&);
  

  // --- job options
  SG::ReadHandleKey<TrackCollection> m_TrackName;
  SG::WriteHandleKey<TrackCollection> m_NewTrackName;

  // --- fitter steering
  Trk::RunOutlierRemoval          m_runOutlier;             //!< switch whether to run outlier logics or not
  int                             m_matEffects;             //!< type of material interaction in extrapolation
  Trk::ParticleHypothesis         m_ParticleHypothesis;     //!< nomen est omen 
  bool                            m_fitRIO_OnTrack;         //!< switch to fit directly from MeasurementBase / RIO_OnTrack

  
  // -- algorithm members
  const TrackCollection              *m_tracks;              //!< job option for main track collection
  ToolHandle<Trk::ITrackFitter>       m_ITrackFitter;        //!< the refit tool
  ToolHandle<Trk::ITrackFitter>       m_ITrackFitterTRT;     //!< the TRT refit tool
  ToolHandle<Trk::ITrackSummaryTool>  m_trkSummaryTool;      //!< the track summary tool
  ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool;         //!< association tool for PRDs
  ToolHandle<Trk::ITrackSelectorTool> m_trkSelectorTool;     //!< the track selector tool
                                                            
  unsigned int                     m_constrainFitMode;       //!< 0 - not constraint, 1 - vertex, 2 - beamspot
  SG::ReadHandleKey<VxContainer>  m_vxContainerName;   
  ServiceHandle<IBeamCondSvc>      m_iBeamCondSvc;           //!< the beam conditions service
  ToolHandle<Trk::IExtrapolator>   m_extrapolator;           //!< the extrapoaltor for the consistent measurement frame
  
  bool m_usetrackhypo;                                       //!< Fit using particle hypothesis from input track    

  
};

} 

#endif //TRKREFITALG_REFITTRACK_H


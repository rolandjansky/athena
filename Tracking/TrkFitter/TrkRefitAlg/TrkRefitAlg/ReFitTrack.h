/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
class VxContainer;

namespace Trk{

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
  
  const Trk::RecVertex* setupConstrainedFit();
  

  // --- job options
  SG::ReadHandleKey<TrackCollection>  m_trackName;
  SG::WriteHandleKey<TrackCollection> m_newTrackName;

  // --- fitter steering
  Trk::RunOutlierRemoval              m_runOutlier;             //!< switch whether to run outlier logics or not
  int                                 m_matEffects;             //!< type of material interaction in extrapolation
  Trk::ParticleHypothesis             m_ParticleHypothesis;     //!< nomen est omen 

  
  // -- algorithm members
  const TrackCollection                     *m_tracks;          //!< job option for main track collection
  ToolHandle<Trk::ITrackFitter>              m_ITrackFitter;    //!< the refit tool
  ToolHandle<Trk::ITrackFitter>              m_ITrackFitterTRT; //!< the TRT refit tool
  ToolHandle<Trk::IExtendedTrackSummaryTool> m_trkSummaryTool;  //!< the track summary tool
  ToolHandle<Trk::IPRDtoTrackMapTool>        m_assoTool
     {this, "AssociationTool", "" };                            //!< Tool to create and populate PRD to track 
                                                                //!  association maps e.g. InDet::InDetPRDtoTrackMapToolGangedPixels

  ToolHandle<Trk::ITrackSelectorTool> m_trkSelectorTool;        //!< the track selector tool
                                                            
  unsigned int                        m_constrainFitMode;       //!< 0 - not constrained, 1 - vertex, 2 - beamspot
  SG::ReadHandleKey<VxContainer>      m_vxContainerName;   
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  // For P->T conversion ID tracks
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;           //!< the extrapolator for the consistent measurement frame
  
  bool m_usetrackhypo;                                       //!< Fit using particle hypothesis from input track    

  
};

} 

#endif //TRKREFITALG_REFITTRACK_H


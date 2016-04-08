/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_ALIGNTRACKPREPROCESSOR_H
#define TRKALIGNGENTOOLS_ALIGNTRACKPREPROCESSOR_H


#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IAlignTrackPreProcessor.h"
#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include <vector>

/**
   @file AlignTrackPreProcessor.h
   @class AlignTrackPreProcessor
   
   @brief Tool used to create AlignTracks from an input collection of tracks.
   At a minimum, the track is refit and the full covariance matrix and derivative
   matrix are stored on the AlignTrack.
   The AlignTracks are filled into a collection of Tracks.

   @author Robert Harrington <roberth@bu.edu>, Daniel Kollar <daniel.kollar@cern.ch>
   @date 10/1/09
*/


namespace Trk {

  class Track;
  class IGlobalTrackFitter;
  //class ITrackSelectorTool;
  class AlignTrackPreProcessor : virtual public IAlignTrackPreProcessor, public AthAlgTool
  {

  public:
    AlignTrackPreProcessor(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~AlignTrackPreProcessor();
    
    StatusCode initialize();
    StatusCode finalize();
    
    /** creates AlignTrack containing all TSOS on track */
    DataVector<Track> * processTrackCollection(const DataVector<Track>* trks);
   

  private:
    ToolHandle<IGlobalTrackFitter> m_trackFitterTool;
    ToolHandle<IGlobalTrackFitter> m_SLTrackFitterTool;

    //ToolHandle<ITrackSelectorTool> m_trackSelectorTool;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectorTool;
    ToolHandle<IInDetAlignHitQualSelTool> m_hitQualityTool;
    
    /** select silicon hits by quality. keep all the rest **/
    Track * performSiliconHitSelection(Track *, const ToolHandle<Trk::IGlobalTrackFitter> &);

    bool m_refitTracks; //!> flag to refit tracks
    bool m_storeFitMatricesAfterRefit; //!> flag to store derivative and covariance matrices after refit

    bool m_runOutlierRemoval;  //!> run outlier removal in track refit
    int  m_particleHypothesis; //!> particle hypothesis in track refit

    bool m_useSingleFitter;    //!< only use 1 fitter for refitting track

    bool m_selectTracks;       //!< do the track selection
    bool m_selectHits;         //!< perform the hit InnerDetector selection
    bool m_fixMomentum;        //!< Fix the momentum of the track so it is not refitted 

  }; // end class

} // end namespace



#endif // TRKALIGNGENTOOLS_ALIGNTRACKPREPROCESSOR_H



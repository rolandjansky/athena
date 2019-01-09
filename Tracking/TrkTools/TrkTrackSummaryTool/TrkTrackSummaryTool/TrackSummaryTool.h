/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRKTRACKSUMMARYTOOL_H
#define TRKTRACKSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "DataModel/DataVector.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/DetailedHitInfo.h"
#include "TrkParameters/TrackParameters.h" 

class AtlasDetectorID;
class Identifier;

#include <vector>

class ITRT_ToT_dEdx;

namespace Trk {
  class ITRT_ElectronPidTool;
  class ITrackHoleSearchTool;
  class IPixelToTPIDTool;

  /** @enum flag the methods for the probability calculation */

  enum TRT_ElectronPidProbability {
    Combined          = 0,
      HighThreshold     = 1,
      TimeOverThreshold = 2,
      Bremsstrahlung    = 3
    };


    class Track;
    class ITrackSummaryHelperTool;
    class RIO_OnTrack;
    class TrackStateOnSurface;
    class MeasurementBase;

    class TrackSummaryTool : virtual public ITrackSummaryTool, public AthAlgTool
    {

    public:
      TrackSummaryTool(const std::string&,const std::string&,const IInterface*);
      virtual ~TrackSummaryTool ();

      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

  /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.
      @param onlyUpdateTrack If false (default) then the summary is cloned and added to the track, and a separate summary returned. If true, only update track and return 0*/
      virtual const TrackSummary* createSummary( const Track& track, bool onlyUpdateTrack=false ) const;

  /** create a summary object of passed track without doing the tedious hole search.
      Same comments as for createSummary apply here, of course, too. */	   
      const Trk::TrackSummary* createSummaryNoHoleSearch( const Track& track ) ;

  /** use this method to update a track. this means a tracksummary is created for
      this track but not returned. the summary can then be obtained from the track.
      Because it is taken from the track the ownership stays with the track */
      void updateTrack(Track& track) const;
      
      /** method to update the shared hit content only, this is optimised for track collection merging. */
      void updateSharedHitCount(Track& track) const;
      
      /** method to update additional information (PID,shared hits, dEdX), this is optimised for track collection merging. */
      void updateAdditionalInfo(Track& track) const;

    private:
      
      /** Return the correct tool, matching the passed Identifier*/
      ITrackSummaryHelperTool*  getTool(const Identifier& id) const;
      
  /** controls whether holes on track are produced
      Turning this on will (slightly) increase processing time.*/
      bool m_doHolesMuon;
  /** For the InDet it is switched on automatically as soon as the HoleSearchTool is given */
      bool m_doHolesInDet;
      
  /** controls whether the detailed summary is added for the indet */
      bool m_addInDetDetailedSummary;

  /** controls whether the detailed summary is added for the muons */
      bool m_addMuonDetailedSummary;

  /** controls whether shared hits in Pix+SCT are produced
      Turning this on will increase processing time.*/
      bool m_doSharedHits;
	
  /**atlas id helper*/
      const AtlasDetectorID* m_detID;

  /**tool to decipher ID RoTs*/
      ToolHandle< ITrackSummaryHelperTool > m_idTool;

  /**tool to calculate electron probabilities*/
  // Troels.Petersen@cern.ch:
      ToolHandle< ITRT_ElectronPidTool > m_eProbabilityTool;
      /** tool to calculate the TRT_ToT_dEdx.
       */
      ToolHandle<ITRT_ToT_dEdx>          m_trt_dEdxTool;


  /**tool to calculate dE/dx using pixel clusters*/
      ToolHandle< IPixelToTPIDTool > m_dedxtool;

  /**tool to decipher muon RoTs*/
      ToolHandle< ITrackSummaryHelperTool > m_muonTool;

  /** tool to search holes in the InDet */
      ToolHandle< ITrackHoleSearchTool > m_idHoleSearch;

      /** Only compute TRT dE/dx if there are at least this number of TRT hits or outliers.
       */
      int m_minTRThitsForTRTdEdx;

  /** switch to deactivate Pixel info init */
      bool m_pixelExists;

      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_DivideByL;
      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_useHThits;
      /** Parameters for the TRT dE/dx compution see @ref ITRT_ToT_dEdx for details.*/
      bool m_TRTdEdx_corrected;

  /**loops over TrackStatesOnSurface and uses this to produce the summary information
      Fills 'information', 'eProbability', and 'hitPattern'*/
      void processTrackStates(const Track& track,
                              const DataVector<const TrackStateOnSurface>* tsos,
                              std::vector<int>& information,
                              std::bitset<numberOfDetectorTypes>& hitPattern,
                              Trk::DetailedHitInfo& detailedInfo) const;
        
      void processMeasurement(const Track& track,
                              const Trk::MeasurementBase* meas,
                              const Trk::TrackStateOnSurface* tsos,
                              std::vector<int>& information,
                              std::bitset<numberOfDetectorTypes>& hitPattern,
                              Trk::DetailedHitInfo& detailedInfo) const;

  /** Extrapolation is performed from one hit to the next, it is checked if surfaces in between
      the extrapolation are left out. The trackParameters of the destination hit (instead of the
        trackParameters of the extrapolation step) are then used as starting point for the next
        extrapolation step. */
      void searchHolesStepWise( const Trk::Track& track, std::vector<int>& information) const;

    };


  }
#endif 

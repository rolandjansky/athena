// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Header file for class SiCombinatorialTrackFinderData_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiCombinatorialTrackFinderData_xk_H
#define SiCombinatorialTrackFinderData_xk_H

#include "SiSPSeededTrackFinderData/SiTrajectory_xk.h"
#include "SiSPSeededTrackFinderData/SiTools_xk.h"
#include "TrkTrack/TrackInfo.h"

#include <list>

namespace Trk {
  class Track;
}

namespace InDet {

 /**
    @class SiCombinatorialTrackFinderData_xk
  
    InDet::SiCombinatorialTrackFinderData_xk holds event dependent data
    used by SiCombinatorialTrackFinder_xk.
    @author Susumu.Oda@cern.ch
  */

  class SiCombinatorialTrackFinderData_xk {

  public:
    /**
     * Constructor
     */
    SiCombinatorialTrackFinderData_xk();

    /**
     * Default destructor
     */
    ~SiCombinatorialTrackFinderData_xk() = default;

    /**
     * Set tools, service and magnetic field properties
     */
    void setTools(const Trk::IPatternParametersPropagator* propTool,
                  const Trk::IPatternParametersUpdator* updatorTool,
                  const Trk::IRIO_OnTrackCreator* rioTool,
                  const IInDetConditionsTool* pixCondTool,
                  const IInDetConditionsTool* sctCondTool,
                  const Trk::MagneticFieldProperties* fieldProp,
                  const Trk::IBoundaryCheckTool* boundaryCheckTool);

      
    /**
     * Set magnetif field cache
     */
    void setFieldCondObj(const  AtlasFieldCacheCondObj* fieldCondObj);
      
    /**
     * Set cached pointer to Pixel cluster collection in StoreGate
     */
    void setPixContainer(const InDet::PixelClusterContainer* pixcont);
    /**
     * Set cached pointer to SCT cluster collection in StoreGate
     */
    void setSctContainer(const InDet::SCT_ClusterContainer* sctcont);

    /**
     * Get cached pointer to Pixel cluster collection in StoreGate
     */
    const InDet::PixelClusterContainer* pixContainer() const;
    /**
     * Get cached pointer to SCT cluster collection in StoreGate
     */
    const InDet::SCT_ClusterContainer* sctContainer() const;

    /**
     * Get PRD to track map
     */
    const Trk::PRDtoTrackMap* PRDtoTrackMap() const;

    /**
     * Check if this object is initialized by the setTools method
     */
    bool isInitialized() const;

    //enum used in Track Maker tools for summary statistics table
    typedef enum summaryStatArraySizes{       
      kNStatAllTypes = 15,        
      kNStatEtaTypes = 2,
      kNSeedTypes = 4,
      kNRapidityRanges = 8,
      kNCombStats = 6 
    } summaryStatArraySizes;     

    /**
     * enum to indicate fit result status (for disappearing track trigger that wants not only for successful tracks)
     */
    enum ResultCode {
	Success          = 99,
	Unrecoverable    =  0,
	PixSeedDiffKFBwd =  3,
	PixSeedDiffKFFwd =  4,
	PixSeedNCluster  =  5,
	MixSeedDiffKFBwd = -3,
	MixSeedDiffKFFwd = -4,
	MixSeedNCluster  = -5,
	Quality  = 6,
	Pt       = 7,
	NCluster = 8,
	HoleCut  = 9,
    };
    enum ResultCodeThreshold {
       RecoverableForDisTrk = 4,
    };

    /**
     * Setter for ResultCode (for disappearing track trigger)
     */
    void setResultCode(const ResultCode);

    /**
     * Setter for flagToReturnFailedTrack (for disappearing track trigger)
     */
    void setFlagToReturnFailedTrack(const bool);

    /**
     * @name Getter methods using references
     */
    //@{
    SiTrajectory_xk& trajectory();
    Trk::TrackInfo& trackinfo();
    InDet::SiTools_xk& tools();
    std::list<Trk::Track*>& tracks();
    int& nprint();
    int& inputseeds();
    int& goodseeds();
    int& findtracks();
    int& inittracks();
    int& roadbug();
    std::array<bool,kNCombStats>& statistic();
    bool& heavyIon();
    int& cosmicTrack();
    int& nclusmin();
    int& nclusminb();
    int& nwclusmin();
    int& nholesmax();
    int& dholesmax();
    bool& simpleTrack();
    // flag to tell whether to return tracks even in case fit is un-successful (for disappearing track trigger)
    bool  flagToReturnFailedTrack();
    // code to tell the fit result (code includes non-succesful cases for disappearing track trigger)
    SiCombinatorialTrackFinderData_xk::ResultCode resultCode();
    double& pTmin();
    double& pTminBrem();
    double& xi2max();
    double& xi2maxNoAdd();
    double& xi2maxlink();

    /// Methods used to associate the hole search outcome to tracks without having to modify the EDM.

    /// This will try to find the hole outcome associated to the input track. If found, it will 
    /// return true and set the pass-by-ref argument to the result. 
    /// Otherwise, will return false and not touch the second argument 
    bool findPatternHoleSearchOutcome (Trk::Track* theTrack, InDet::PatternHoleSearchOutcome & outcome) const;
    ///  This is used to store the pattern hole search outcome for a given track. 
    void addPatternHoleSearchOutcome (Trk::Track* theTrack, const InDet::PatternHoleSearchOutcome & outcome); 
    //@}

  protected:
    /**
     * Dummy method defined in child classes. Is it necessary?
     */
    virtual void dummy() = 0;
    /**
     * Set PRD to track map
     */
    void setPRDtoTrackMap(const Trk::PRDtoTrackMap* prd_to_track_map);

  private:

    /// Initialization flag
    bool m_initialized{false};
    /// Track trajectory
    SiTrajectory_xk m_trajectory;
    /// Track info
    Trk::TrackInfo m_trackinfo;
    /// Hold tools, service, map, etc.
    InDet::SiTools_xk m_tools;
    /// List of found tracks
    std::list<Trk::Track*> m_tracks;
    /// Kind output information(?)
    int m_nprint{0};
    /// Number input seeds
    int m_inputseeds{0};
    /// Number accepted seeds
    int m_goodseeds{0};
    /// Number found tracks
    int m_findtracks{0};
    /// Number initial tracks
    int m_inittracks{0};
    /// Number wrong DE roads
    int m_roadbug{0};
    /// Switch array
    std::array<bool,kNCombStats> m_statistic{};
    // Heavy ion flag
    bool m_heavyIon{false};
    /// Is it cosmic track (0 or 1)
    int m_cosmicTrack{0};
    /// Min number clusters
    int m_nclusmin{0};
    /// Min number clusters
    int m_nclusminb{0};
    /// Min number weighted clusters
    int m_nwclusmin{0};
    /// Max number holes
    int m_nholesmax{0};
    /// Max holes gap
    int m_dholesmax{0};
    /// Simple track flag
    bool m_simpleTrack{false};
    /// Flag whether to return non-successful tracks (for disappearing track trigger)
    bool m_flagToReturnFailedTrack{false};
    /// Result code (to indicate fit result for disappearing track trigger)
    ResultCode m_resultCode{ResultCode::Success};
    /// min pT
    double m_pTmin{0.};
    /// min pT for brem noise model
    double m_pTminBrem{0.};
    /// max Xi2 for updators
    double m_xi2max{0.};
    /// max Xi2 for clusters
    double m_xi2maxNoAdd{0.};
    /// max Xi2 for clusters
    double m_xi2maxlink{0.};
    /// A helper map to associate hole search outcomes to tracks
    std::map<Trk::Track*, InDet::PatternHoleSearchOutcome> m_holeSearchOutcomes; 

    /// cached pointer to Pixel cluster collection in StoreGate
    const InDet::PixelClusterContainer* m_pixcontainer{nullptr};
    /// cached pointer to SCT cluster collection in StoreGate
    const InDet::SCT_ClusterContainer* m_sctcontainer{nullptr};
  };

} // end of name space

#endif // SiCombinatorialTrackFinderData_xk_H

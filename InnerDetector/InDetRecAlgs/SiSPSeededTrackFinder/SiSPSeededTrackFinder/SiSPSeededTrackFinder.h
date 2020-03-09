// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SiSPSeededTrackFinder_H
#define SiSPSeededTrackFinder_H

/// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

/// Athena includes
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiTrackMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h"
#include "StoreGate/DataHandle.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "xAODEventInfo/EventInfo.h"

/// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

/// STL includes
#include <atomic>
#include <string>

//class SpacePointContainer;
namespace InDet {

  /**
   * @class SiSPSeededTrackFinder
   * Class-algorithm for track finding in Pixels and SCT
   * initiated by space points seeds
   */
  class SiSPSeededTrackFinder : public AthReentrantAlgorithm 
  {
    
    ///////////////////////////////////////////////////////////////////
    // Public methods
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard Algotithm methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiSPSeededTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~SiSPSeededTrackFinder() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

    /// Make this algorithm clonable.
    virtual bool isClonable() const override { return true; };
    //@}

  protected:

    ///////////////////////////////////////////////////////////////////
    // Protected data 
    ///////////////////////////////////////////////////////////////////

    /// enums for Counter_t
    enum ECounter {kNSeeds, kNTracks, kNCounter};

    /// @class Counter_t
    /// For counters
    class Counter_t : public std::array<std::atomic_int, kNCounter> 
    {
    public:
      Counter_t& operator += (const Counter_t& counter) {
        for (unsigned int idx=0; idx <kNCounter; ++idx) {
          (*this)[idx] += counter[idx];
        }
        return *this;
      }
    };

    /// @name Flags to configure SiSPSeededTrackFinder
    //@{
    BooleanProperty m_useZvertexTool{this, "useZvertexTool", true}; 
    BooleanProperty m_useMBTS{this, "useMBTSTimeDiff", false};
    BooleanProperty m_useNewStrategy{this, "useNewStrategy", false};
    BooleanProperty m_useZBoundaryFinding{this, "useZBoundFinding", false};
    BooleanProperty m_ITKGeometry{this, "ITKGeometry", false, "Flag to specify if it is ITK geometry"};
    IntegerProperty m_maxNumberSeeds{this, "maxNumberSeeds", 3000000, "Max. number used seeds"};
    IntegerProperty m_maxPIXsp{this, "maxNumberPIXsp", 150000, "Max. number pixels space points"};
    IntegerProperty m_maxSCTsp{this, "maxNumberSCTsp", 500000, "Max. number sct    space points"};
    IntegerProperty m_nfreeCut{this, "FreeClustersCut", 1, "Min number free clusters"};
    IntegerProperty m_histsize{this, "HistSize", 1400};
    IntegerProperty m_nvertex{this, "maxVertices", 4};
    DoubleProperty m_zcut{this, "Zcut", 350.};
    StringProperty m_fieldmode{this, "MagneticFieldMode", "MapSolenoid"};
    //@}

    /// @name Data members for new strategy reconstruction
    //@{
    double                         m_pTcut{500};
    double                         m_imcut{2.};
    double                         m_zstep{0.};
    //@}

    /// @name Data handles for StoreGate access in AthenaMT
    //@{
    SG::ReadHandleKey<xAOD::EventInfo> m_evtKey{this, "EventInfoKey", "EventInfo"};
    SG::ReadHandleKey<SpacePointContainer> m_SpacePointsSCTKey{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_SpacePointsPixelKey{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<Trk::PRDtoTrackMap> m_prdToTrackMap{this,"PRDtoTrackMap",""};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

    SG::WriteHandleKey<TrackCollection> m_outputTracksKey{this, "TracksLocation", "SiSPSeededTracks"};
    //@}

    /// @name Tool handles
    //@{
    ToolHandle<ISiSpacePointsSeedMaker> m_seedsmaker{this, "SeedsTool", "InDet::SiSpacePointsSeedMaker_ATLxk/InDetSpSeedsMaker", "Space poins seed maker"};
    ToolHandle<ISiZvertexMaker> m_zvertexmaker{this, "ZvertexTool", "InDet::SiZvertexMaker_xk/InDetSiZvertexMaker", "Space poins z-vertex maker"};
    ToolHandle<ISiTrackMaker> m_trackmaker{this, "TrackTool", "InDet::SiTrackMaker_xk/InDetSiTrackMaker", "Track maker"};
    PublicToolHandle<Trk::IPatternParametersPropagator> m_proptool{this, "PropagatorTool", "Trk::RungeKuttaPropagator/InDetPropagator"};
    ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};
    //@}

    /// @name Magenetic fiedl propertis
    //@{
    Trk::MagneticFieldProperties m_fieldprop;
    //@}

    /// @name Counters
    //@{
    mutable Counter_t m_counterTotal ATLAS_THREAD_SAFE {};

    mutable std::atomic_int m_neventsTotal{0}; //!< Number events
    mutable std::atomic_int m_neventsTotalV{0}; //!< Number events
    mutable std::atomic_int m_problemsTotal{0}; //!< Numbe revents with number seeds > maxNumber
    mutable std::atomic_int m_problemsTotalV{0}; //!< Numbe revents with number seeds > maxNumber
    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected methods
    ///////////////////////////////////////////////////////////////////

    /// @name Print internal tool parameters and status
    //@{
    MsgStream& dump(MSG::Level lvl, const SiSPSeededTrackFinder::Counter_t*) const;
    MsgStream& dumptools(MsgStream& out) const;
    MsgStream& dumpevent(MsgStream& out, const SiSPSeededTrackFinder::Counter_t& counter) const;
    //@}

    bool isGoodEvent(const EventContext& ctx) const; //!< EventContext is used to specify which event
    double trackQuality(const Trk::Track*) const;
    void filterSharedTracks(std::multimap<double, Trk::Track*>&) const;
    void fillZHistogram(const Trk::Track* Tr,
                        Trk::PerigeeSurface& per,
                        std::vector<int>& nhistogram,
                        std::vector<double>& zhistogram,
                        std::vector<double>& phistogram) const;
    void findZvertex(std::list<Trk::Vertex>& ZV,
                     double* ZB,
                     std::vector<int>& nhistogram,
                     std::vector<double>& zhistogram,
                     std::vector<double>& phistogram) const;
    StatusCode oldStrategy(const EventContext& ctx) const; //!< EventContext is used to specify which event
    StatusCode newStrategy(const EventContext& ctx) const; //!< EventContext is used to specify which event
    void magneticFieldInit();

  };
}
#endif // SiSPSeededTrackFinder_H

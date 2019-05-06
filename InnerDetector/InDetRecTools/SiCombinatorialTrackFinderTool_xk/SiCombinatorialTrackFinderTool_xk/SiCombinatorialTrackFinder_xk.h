// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiCombinatorialTrackFinder_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for  Trk::Track production in SCT and Pixels
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 10/04/2007 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiCombinatorialTrackFinder_xk_H
#define SiCombinatorialTrackFinder_xk_H

#include "InDetRecToolInterfaces/ISiCombinatorialTrackFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTrajectory_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTools_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLinks_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <list>
#include <map>
#include <mutex>
#include <vector>

class MsgStream;

namespace InDet {

  //class SiClusterContainer;

 /**
  @class SiCombinatorialTrackFinder_xk 
  
  InDet::SiCombinatorialTrackFinder_xk is algorithm which produce track-finding
  in the road of InDetDD::SiDetectorElement* sorted in propagation order.
  @author Igor.Gavrilenko@cern.ch     
  */

  class SiCombinatorialTrackFinder_xk : 

    virtual public ISiCombinatorialTrackFinder, public AthAlgTool
    {
      friend class SiTrajectory_xk;

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      SiCombinatorialTrackFinder_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~SiCombinatorialTrackFinder_xk() = default;
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for local track finding
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 const TrackQualityCuts&) const;

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&) const;

      virtual const std::list<Trk::Track*>& getTracksWithBrem
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&,
	 bool) const;
   
      virtual void newEvent() const;
      virtual void newEvent(Trk::TrackInfo, const TrackQualityCuts&) const;

      virtual void endEvent() const;
     

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle{this, "MagFieldSvc",
          "AtlasFieldSvc"};
      ToolHandle<IInDetConditionsTool> m_pixelCondSummaryTool{this, "PixelSummaryTool",
          "PixelConditionsSummaryTool"};
      ToolHandle<IInDetConditionsTool> m_sctCondSummaryTool{this, "SctSummaryTool",
          "InDetSCT_ConditionsSummaryTool/SCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
      PublicToolHandle<Trk::IPatternParametersPropagator> m_proptool{this, "PropagatorTool",
          "Trk::RungeKuttaPropagator/InDetPropagator"};
      PublicToolHandle<Trk::IPatternParametersUpdator> m_updatortool{this, "UpdatorTool",
          "Trk::KalmanUpdator_xk/InDetPatternUpdator"};
      PublicToolHandle<Trk::IRIO_OnTrackCreator> m_riocreator{this, "RIOonTrackTool",
          "Trk::RIO_OnTrackCreator/RIO_OnTrackCreator"};
      PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssosiationTool",
          "InDet::InDetPRD_AssociationToolGangedPixels"};

      SG::ReadHandleKey<InDet::SiClusterContainer> m_pixcontainerkey{this, "PixelClusterContainer",
          "PixelClusters"};
      SG::ReadHandleKey<InDet::SiClusterContainer> m_sctcontainerkey{this, "SCT_ClusterContainer",
          "SCT_Clusters"};
      SG::ReadCondHandleKey<InDet::SiDetElementBoundaryLinks_xk> m_boundarySCTKey{this, "SCT_DetElementBoundaryLinks_xk",
          "SCT_DetElementBoundaryLinks_xk", "Key of InDet::SiDetElementBoundaryLinks_xk for SCT"};
      // For P->T converter of SCT_Clusters
      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey",
          "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

      BooleanProperty m_usePIX{this, "usePixel", true};
      BooleanProperty m_useSCT{this, "useSCT", true};
      StringProperty m_fieldmode{this, "MagneticFieldMode", "MapSolenoid", "Mode of magnetic field"};
      StringProperty m_pixm{this, "PixManagerLocation", "Pixel", "PIX manager location"};
      StringProperty m_sctm{this, "SCTManagerLocation", "SCT", "SCT manager location"}; // NOT USED
      DoubleProperty m_qualityCut{this, "TrackQualityCut", 9.3, "Simple track quality cut"};

      // Updated in only initialize
      int m_outputlevel{0};
      Trk::MagneticFieldProperties m_fieldprop; // Magnetic field properties
      std::string m_callbackString;

      // Updated in only mapDetectorElementsProduction
      InDet::SiDetElementBoundaryLinks_xk m_boundaryPIX;

      mutable std::mutex m_mutex;
      mutable std::vector<EventContext::ContextEvt_t> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
      struct EventData {
        SiTrajectory_xk trajectory; // Track trajectory
        Trk::TrackInfo trackinfo;
        InDet::SiTools_xk tools;
        std::list<Trk::Track*> tracks; // List found tracks
        int nprint{0};  // Kind output information
        int inputseeds{0}; // Number input seeds
        int goodseeds{0}; // Number accepted seeds
        int findtracks{0}; // Number found tracks
        int inittracks{0}; // Number initial tracks
        int roadbug{0}; // Number wrong DE roads
        bool heavyIon{false};
        int cosmicTrack{0};  // Is it cosmic track (0 or 1)
        int nclusmin{0}; // Min number clusters
        int nclusminb{0}; // Min number clusters
        int nwclusmin{0}; // Min number weighted clusters
        int nholesmax{0}; // Max number holes
        int dholesmax{0}; // Max holes gap
        bool simpleTrack{false};
        double pTmin{0.}; // min pT
        double pTminBrem{0.}; // min pT for brem noise model
        double xi2max{0.}; // max Xi2 for updators
        double xi2maxNoAdd{0.}; // max Xi2 for clusters
        double xi2maxlink{0.}; // max Xi2 for clusters
      };
      mutable std::vector<EventData> m_eventData ATLAS_THREAD_SAFE; // Guarded by m_mutex

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

      bool findTrack
        (EventData& data,
         const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&) const;

      void getTrackQualityCuts(EventData& data, const TrackQualityCuts&) const;

      Trk::Track* convertToTrack(EventData& data) const;
      Trk::Track* convertToNextTrack(EventData& data) const;
 
      void magneticFieldInit();

      StatusCode mapDetectorElementsProduction(IOVSVC_CALLBACK_ARGS);

      bool spacePointsToClusters
	(const std::list<const Trk::SpacePoint*>&,
	 std::list<const InDet::SiCluster*> &) const; 

      void detectorElementLinks
	(std::list<const InDetDD::SiDetectorElement*>        &,
	 std::list<const InDet::SiDetElementBoundaryLink_xk*>&) const;

      void newEvent(EventData& data) const;

      EventData& getEventData() const;

      MsgStream& dumpconditions(MsgStream& out) const;
      MsgStream& dumpevent(EventData& data, MsgStream& out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const SiCombinatorialTrackFinder_xk&);
  std::ostream& operator << (std::ostream&,const SiCombinatorialTrackFinder_xk&); 

} // end of name space

#endif // SiCombinatorialTrackFinder_xk_H


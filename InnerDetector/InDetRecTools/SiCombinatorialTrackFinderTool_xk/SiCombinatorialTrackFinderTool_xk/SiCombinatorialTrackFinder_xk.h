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
#include <vector>

class MsgStream          ;

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
	 const TrackQualityCuts&);

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);

      virtual const std::list<Trk::Track*>& getTracksWithBrem
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
	 bool);
   
      virtual void newEvent();
      virtual void newEvent(Trk::TrackInfo,const TrackQualityCuts&);

      virtual void endEvent();
     

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
      StringProperty m_sctm{this, "SCTManagerLocation", "SCT", "SCT manager location"};
      DoubleProperty m_qualityCut{this, "TrackQualityCut", 9.3, "Simple track quality cut"};

      SiTrajectory_xk                m_trajectory    ;  // Track trajector1
      Trk::TrackInfo                 m_trackinfo     ;
      InDet::SiTools_xk              m_tools         ;
      Trk::MagneticFieldProperties   m_fieldprop     ; // Magnetic field properties
      std::list<Trk::Track*>         m_tracks        ; // List found tracks
      InDet::SiDetElementBoundaryLinks_xk m_boundaryPIX;

      int                            m_outputlevel   ;
      int                            m_nprint        ;  // Kind output information
      int                            m_inputseeds{0};  // Number input seeds
      int                            m_goodseeds     ;  // Number accepted seeds
      int                            m_findtracks{0};  // Number found tracks
      int                            m_inittracks    ;  // Number initial tracks
      int                            m_roadbug       ;  // Number wrong DE roads
      std::string                    m_callbackString;

      bool                           m_pix           ;
      bool                           m_sct           ;
      bool                           m_heavyion{false};

      int                            m_cosmicTrack   ;  // Is it cosmic track (0 or 1)
      int                            m_nclusmin      ; // Min number clusters
      int                            m_nclusminb     ; // Min number clusters
      int                            m_nwclusmin     ; // Min number weighted clusters
      int                            m_nholesmax     ; // Max number holes
      int                            m_dholesmax     ; // Max holes gap
      bool                           m_simpleTrack{false};
      double                         m_pTmin         ; // min pT
      double                         m_pTminBrem     ; // min pT for brem noise model
      double                         m_xi2max        ; // max Xi2 for updators
      double                         m_xi2maxNoAdd   ; // max Xi2 for clusters
      double                         m_xi2maxlink    ; // max Xi2 for clusters

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

      bool findTrack
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);

      void getTrackQualityCuts(const TrackQualityCuts&);

      Trk::Track* convertToTrack();
      Trk::Track* convertToNextTrack();
 
      void magneticFieldInit();

      StatusCode mapDetectorElementsProduction(IOVSVC_CALLBACK_ARGS) ;

      bool spacePointsToClusters
	(const std::list<const Trk::SpacePoint*>&,
	 std::list<const InDet::SiCluster*>     &) const; 

      void detectorElementLinks
	(std::list<const InDetDD::SiDetectorElement*>        &,
	 std::list<const InDet::SiDetElementBoundaryLink_xk*>&) const;

      MsgStream& dumpconditions(MsgStream& out) const;
      MsgStream& dumpevent     (MsgStream& out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const SiCombinatorialTrackFinder_xk&);
  std::ostream& operator << (std::ostream&,const SiCombinatorialTrackFinder_xk&); 

} // end of name space

#endif // SiCombinatorialTrackFinder_xk_H


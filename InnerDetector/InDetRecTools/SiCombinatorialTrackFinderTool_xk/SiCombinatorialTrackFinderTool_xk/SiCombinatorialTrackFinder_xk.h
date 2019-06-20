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

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiSPSeededTrackFinderData/SiCombinatorialTrackFinderData_xk.h"
#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLink_xk.h"
#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLinks_xk.h"
#include "SiSPSeededTrackFinderData/SiTrajectory_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <list>
#include <map>
#include <vector>

class MsgStream;

namespace InDet {

 /**
  @class SiCombinatorialTrackFinder_xk 
  
  InDet::SiCombinatorialTrackFinder_xk is algorithm which produce track-finding
  in the road of InDetDD::SiDetectorElement* sorted in propagation order.
  @author Igor.Gavrilenko@cern.ch     
  */

  class SiCombinatorialTrackFinder_xk : 

    public extends<AthAlgTool, ISiCombinatorialTrackFinder>
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
      virtual StatusCode initialize() override;
      virtual StatusCode finalize  () override;

      ///////////////////////////////////////////////////////////////////
      // Main methods for local track finding
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Track*>& getTracks
        (SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 const TrackQualityCuts&) const override;

      virtual const std::list<Trk::Track*>& getTracks
        (SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&) const override;

      virtual const std::list<Trk::Track*>& getTracksWithBrem
        (SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&,
	 bool) const override;
   
      virtual void newEvent(SiCombinatorialTrackFinderData_xk& data) const override;
      virtual void newEvent(SiCombinatorialTrackFinderData_xk& data,
                            Trk::TrackInfo, const TrackQualityCuts&) const override;

      virtual void endEvent(SiCombinatorialTrackFinderData_xk& data) const override;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream& dump(SiCombinatorialTrackFinderData_xk& data, MsgStream& out) const override;

    private:
      
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

      SG::ReadHandleKey<InDet::PixelClusterContainer> m_pixcontainerkey{this, "PixelClusterContainer",
          "PixelClusters"};
      SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_sctcontainerkey{this, "SCT_ClusterContainer",
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

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

      bool findTrack
        (SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&) const;

      void getTrackQualityCuts(SiCombinatorialTrackFinderData_xk& data, const TrackQualityCuts&) const;

      Trk::Track* convertToTrack(SiCombinatorialTrackFinderData_xk& data) const;
      Trk::Track* convertToNextTrack(SiCombinatorialTrackFinderData_xk& data) const;
 
      void magneticFieldInit();

      StatusCode mapDetectorElementsProduction(IOVSVC_CALLBACK_ARGS);

      bool spacePointsToClusters
	(const std::list<const Trk::SpacePoint*>&,
	 std::list<const InDet::SiCluster*> &) const; 

      void detectorElementLinks
	(std::list<const InDetDD::SiDetectorElement*>        &,
	 std::list<const InDet::SiDetElementBoundaryLink_xk*>&) const;

      MsgStream& dumpconditions(MsgStream& out) const;
      MsgStream& dumpevent(SiCombinatorialTrackFinderData_xk& data, MsgStream& out) const;

      void initializeCombinatorialData(SiCombinatorialTrackFinderData_xk& data) const;

    };

} // end of name space

#endif // SiCombinatorialTrackFinder_xk_H


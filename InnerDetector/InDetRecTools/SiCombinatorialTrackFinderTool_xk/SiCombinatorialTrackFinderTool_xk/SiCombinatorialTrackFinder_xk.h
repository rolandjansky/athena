/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <list>
#include <vector>
#include <map>
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISiCombinatorialTrackFinder.h"

#include "SiCombinatorialTrackFinderTool_xk/SiTrajectory_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTools_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"

class MsgStream          ;
class IInDetConditionsSvc;

namespace InDet{

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
      virtual ~SiCombinatorialTrackFinder_xk();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for local track finding
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 const TrackQualityCuts&);

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);

      virtual const std::list<Trk::Track*>& getTracksWithBrem
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
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

      ServiceHandle<IInDetConditionsSvc>    m_pixelCondSummarySvc;
      ServiceHandle<IInDetConditionsSvc>    m_sctCondSummarySvc  ;
      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
      ToolHandle<Trk::IPatternParametersPropagator> m_proptool   ;
      ToolHandle<Trk::IPatternParametersUpdator>    m_updatortool;
      ToolHandle<Trk::IRIO_OnTrackCreator>          m_riocreator ;
      ToolHandle<Trk::IPRD_AssociationTool>         m_assoTool   ;

      bool                           m_usePIX        ;
      bool                           m_useSCT        ;
      int                            m_outputlevel   ;
      int                            m_nprint        ;  // Kind output information
      int                            m_inputseeds    ;  // Number input seeds
      int                            m_goodseeds     ;  // Number accepted seeds
      int                            m_findtracks    ;  // Number found tracks
      int                            m_inittracks    ;  // Number initial tracks
      int                            m_roadbug       ;  // Number wrong DE roads
      int                            m_cosmicTrack   ;  // Is it cosmic track (0 or 1)
      SiTrajectory_xk                m_trajectory    ;  // Track trajector
      std::string                    m_fieldmode     ;  // Mode of magnetic field
//      std::string                    m_pixelname     ;  // Name container with pixels 
//      std::string                    m_sctname       ;  // Name container with scts
      std::string                    m_pixm          ;  // PIX manager   location
      std::string                    m_sctm          ;  // SCT manager   location
      std::string                    m_callbackString;
      Trk::TrackInfo                 m_trackinfo     ;

      InDet::SiTools_xk              m_tools         ;
      bool                           m_pix           ;
      bool                           m_sct           ;
      bool                           m_simpleTrack   ;
      bool                           m_heavyion      ;

      SG::ReadHandle<InDet::SiClusterContainer>  m_pixcontainer  ;
      SG::ReadHandle<InDet::SiClusterContainer>  m_sctcontainer  ;
      MagField::IMagFieldSvc*        m_fieldService  ;
      Trk::MagneticFieldProperties   m_fieldprop     ; // Magnetic field properties
      double                         m_xi2max        ; // max Xi2 for updators
      double                         m_xi2maxNoAdd   ; // max Xi2 for clusters
      double                         m_xi2maxlink    ; // max Xi2 for clusters
      double                         m_pTmin         ; // min pT
      double                         m_pTminBrem     ; // min pT for brem noise model
      double                         m_qualityCut    ; // Simple track quality cut
      int                            m_nholesmax     ; // Max number holes
      int                            m_dholesmax     ; // Max holes gap
      int                            m_nclusmin      ; // Min number clusters
      int                            m_nclusminb     ; // Min number clusters
      int                            m_nwclusmin     ; // Min number weighted clusters
      std::list<Trk::Track*>         m_tracks        ; // List found tracks
      std::vector<InDet::SiDetElementBoundaryLink_xk> m_boundaryPIX;
      std::vector<InDet::SiDetElementBoundaryLink_xk> m_boundarySCT;

      bool                           m_passThroughExtension;

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

      bool findTrack
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);

      void getTrackQualityCuts(const TrackQualityCuts&);

      Trk::Track* convertToTrack();
      Trk::Track* convertToNextTrack();
 
      void magneticFieldInit();

      StatusCode mapDetectorElementsProduction(IOVSVC_CALLBACK_ARGS);

      bool spacePointsToClusters
	(const std::list<const Trk::SpacePoint*>&,
	 std::list<const InDet::SiCluster*>     &); 

      void detectorElementLinks
	(std::vector<const InDetDD::SiDetectorElement*>        &,
	 std::vector<const InDet::SiDetElementBoundaryLink_xk*>&);

      MsgStream&    dumpconditions(MsgStream&    out) const;
      MsgStream&    dumpevent     (MsgStream&    out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const SiCombinatorialTrackFinder_xk&);
  std::ostream& operator << (std::ostream&,const SiCombinatorialTrackFinder_xk&); 

} // end of name space

#endif // SiCombinatorialTrackFinder_xk_H


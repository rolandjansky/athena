/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackSegmentsMaker_ATLxk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using InDet::TRT_DriftCircles
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 11/10/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackSegmentsMaker_ATLxk_H
#define TRT_TrackSegmentsMaker_ATLxk_H

#include <list>
#include <map>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_TrackSegmentsTool_xk/TRT_DriftCircleLinkN_xk.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkEventUtils/EventDataBase.h"

#include "StoreGate/ReadHandleKey.h"

class MsgStream;

namespace Trk {

  class IPropagator;
}

namespace InDet{

  class ITRT_TrackExtensionTool;

  class TRT_TrackSegmentsMaker_ATLxk : 
    virtual public ITRT_TrackSegmentsMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrackSegmentsMaker_ATLxk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_TrackSegmentsMaker_ATLxk();
      virtual StatusCode               initialize() override;
      virtual StatusCode               finalize  () override;

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newEvent () const override;
      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newRegion(const std::vector<IdentifierHash>&) const override;
      void endEvent(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;
      
      ///////////////////////////////////////////////////////////////////
      // Methods of seeds production without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;

      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual Trk::TrackSegment* next(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;
  
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      virtual MsgStream&    dump          (MsgStream   & out) const override;
      virtual std::ostream& dump          (std::ostream& out) const override;

    protected:

      class EventData;
      class EventData : public Trk::EventDataBase<EventData,InDet::ITRT_TrackSegmentsMaker::IEventData>
      {
         friend class TRT_TrackSegmentsMaker_ATLxk;
      public:
         EventData(const TRT_DriftCircleContainer *trtcontainer, int cirsize) : m_trtcontainer(trtcontainer) {
            m_segiterator      = m_segments.begin();
            m_sizebin_iterator = m_sizebin.rend();
            m_circles  = new TRT_DriftCircleLinkN_xk[cirsize];
         }

         ~EventData() { delete [] m_circles; }

      protected:
         const InDet::TRT_DriftCircleContainer *m_trtcontainer = nullptr;
         std::unique_ptr<InDet::ITRT_TrackExtensionTool::IEventData>  m_extEventData;
         int                                    m_clusters  = 0   ;
         int                                    m_nlocal    = 0   ;
         int                                    m_nsegments = 0   ;

         std::multimap<unsigned int,unsigned int>  m_bincluster   ;
         std::multimap<unsigned int,unsigned int>  m_sizebin      ;

         std::list<Trk::TrackSegment*>            m_segments   ;
         std::list<Trk::TrackSegment*>::iterator  m_segiterator;

         std::multimap<unsigned int,unsigned int>::reverse_iterator m_sizebin_iterator;

         union {unsigned char H[227500*4]; unsigned int H4[227500];} m_U;

         std::multimap<const InDet::TRT_DriftCircle*,Trk::TrackSegment*> m_clusterSegment;
         std::multimap<int                          ,Trk::TrackSegment*> m_qualitySegment;

         TRT_DriftCircleLinkN_xk*               m_circles  = nullptr;

      };
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      std::string                            m_fieldmode       ; // Mode of magnetic field

      std::string                            m_ntrtmanager     ; // Name of TRT det. manager 
      ServiceHandle<IGeoModelSvc>            m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc"};
      ToolHandle<Trk::IPropagator>           m_propTool        ; // Propagator            tool
      ToolHandle<ITRT_TrackExtensionTool>    m_extensionTool
         {this, "TrackExtensionTool", "InDet::TRT_TrackExtensionTool_xk"} ; // TRT track extension   tool

      Trk::MagneticFieldProperties           m_fieldprop       ; // Magnetic field properties
      const InDetDD::TRT_DetectorManager   * m_trtmgr          ;
      const TRT_ID                         * m_trtid           ; 
      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_trtname{this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve TRT_DriftCircles"}; // TRTs   container ; // Name  TRT container
      SG::ReadHandleKey<Trk::PRDtoTrackMap>          m_prdToTrackMap
         {this,"PRDtoTrackMap",""};

      bool                                   m_build           ;
      bool                                   m_gupdate         ;
      bool                                   m_removeNoise     ;
      int                                    m_outputlevel     ;
      int                                    m_nprint          ;
      int                                    m_clustersCut     ;
      unsigned int                           m_nlayers[4]      ;
      unsigned int                           m_nstraws[4]      ;
      unsigned int                           m_flayers[4][30]  ;
      unsigned int                           m_begin  [4][200] ;
      unsigned int                           m_end    [4][200] ;
      unsigned int*                          m_ndzdr           ;
      int*                                   m_islope          ;
      int                                    m_Ts              ;
      int                                    m_Ns128           ;
      float                                  m_pTmin           ;
      float                                  m_sharedfrac      ;
      float                                  m_dzdr[26]        ;
      float                                  m_Psi             ;
      float                                  m_Psi128          ;
      float                                  m_A               ;
      float                                  m_Ai              ;
      float*                                 m_slope           ;
      int                                    m_nPhi            ; // number azimuthal channel
      int                                    m_nMom            ; // number momentum  channel
      int                                    m_histsize        ; // histogram size
      int                                    m_cirsize         ; // Size of m_circles


      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      StatusCode mapStrawsUpdate    (IOVSVC_CALLBACK_ARGS);
      void magneticFieldInit();
      void mapStrawsProduction();
      void eraseHistogramm(TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;
      void fillHistogramm   (float,int, TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;
      void analyseHistogramm(unsigned char&,unsigned int&,float,int,TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;
      unsigned int localMaximum(unsigned int, TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;

      void findLocaly(unsigned int,
                      const Trk::PRDtoTrackMap *prd_to_track_map,
                      TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;

      void segmentsPreparation(TRT_TrackSegmentsMaker_ATLxk::EventData &event_data) const;

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out,InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const;
    };

} // end of name space

#endif // TRT_TrackSegmentsMaker_ATLxk_H


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackExtensionTool_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Concret implementation for base class ITRT_TrackExtensionTool
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 22/08/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackExtensionTool_xk_H
#define TRT_TrackExtensionTool_xk_H

#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"
#include "TRT_TrackExtensionTool_xk/TRT_Trajectory_xk.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "StoreGate/ReadHandleKey.h"

class MsgStream;

namespace InDet{

  class ITrtDriftCircleCutTool;
  class ITRT_DetElementsRoadMaker;

  /**
  @class TRT_TrackExtensionTool_xk 
  
  InDet::TRT_TrackExtensionTool_xk is algorithm which produce track
  extension to TRT
  @author Igor.Gavrilenko@cern.ch     
  */

  class TRT_TrackExtensionTool_xk : 

    virtual public ITRT_TrackExtensionTool, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrackExtensionTool_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_TrackExtensionTool_xk();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Main methods for track extension to TRT
      ///////////////////////////////////////////////////////////////////
      
      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::Track&);
      virtual std::vector<const Trk::MeasurementBase*>& extendTrack
	(const Trk::TrackParameters&);
      virtual Trk::TrackSegment* findSegment
	(const Trk::TrackParameters&);
      virtual Trk::Track* newTrack
	(const Trk::Track&);
      virtual void newEvent();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump(MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
      MagField::IMagFieldSvc*                m_fieldService      ;
      ToolHandle<ITRT_DetElementsRoadMaker>         m_roadtool   ; // TRT road maker tool
      ToolHandle<Trk::IPatternParametersPropagator> m_proptool   ; //
      ToolHandle<Trk::IPatternParametersUpdator>    m_updatortool; //
      ToolHandle<ITrtDriftCircleCutTool>            m_selectortool;// Segment selector tool
      ToolHandle<Trk::IRIO_OnTrackCreator>          m_riontrackD ; //
      ToolHandle<Trk::IRIO_OnTrackCreator>          m_riontrackN ; //

      int                              m_segmentFindMode; // Method of segment find
      int                              m_outputlevel    ; // Print level
      int                              m_nprint         ; // Kind of print
      int                              m_minNumberDCs   ; // Min. number of DriftCircles
      int                              m_minNumberSCT   ; // Min. number SCT clusters for extension
      int                              m_minNumberPIX   ; // Min. number SCT clusters for extension
      double                           m_roadwidth      ; // Max width of the road
      double                           m_maxslope       ; //
      double                           m_zVertexWidth   ; // Z-vertex half width 
      double                           m_impact         ; // max impact parameter
      bool                             m_usedriftrad    ; // Use drift time ?
      bool                             m_parameterization; // Use table of min number DCs
      double                           m_scale_error    ; // Scalefactor for hit uncertainty
      std::string                      m_fieldmode      ; // Mode of magnetic field
      std::string                      m_trtmanager     ; // Name of TRT det. manager 
      //std::string                      m_trtname        ; // Name container with TRT clusters

      Trk::MagneticFieldProperties     m_fieldprop      ; // Magnetic field properties
      SG::ReadHandleKey<TRT_DriftCircleContainer> m_trtname {this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve TRT_DriftCircleContainer"};
      TRT_Trajectory_xk                         m_trajectory   ;
      std::vector<const Trk::MeasurementBase*>  m_measurement  ;

      ///////////////////////////////////////////////////////////////////
      // Methods 
      ///////////////////////////////////////////////////////////////////

      void       magneticFieldInit();
      StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);
      bool isGoodExtension(const Trk::TrackParameters&);
      bool numberPIXandSCTclustersCut(const Trk::Track&);

      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;
    };

  MsgStream&    operator << (MsgStream&   ,const TRT_TrackExtensionTool_xk&);
  std::ostream& operator << (std::ostream&,const TRT_TrackExtensionTool_xk&); 

} // end of name space

#endif // TRT_TrackExtensionTool_xk_H


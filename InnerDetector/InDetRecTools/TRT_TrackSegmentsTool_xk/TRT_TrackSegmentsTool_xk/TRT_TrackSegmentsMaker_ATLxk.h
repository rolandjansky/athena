/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TRT_TrackSegmentsTool_xk/TRT_DriftCircleLinkN_xk.h"


#include "StoreGate/ReadHandleKey.h"

class MsgStream;

namespace Trk {

  class IPropagator;
  class IPRD_AssociationTool;
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
      virtual StatusCode               initialize();
      virtual StatusCode               finalize  ();

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      void newEvent();
      void newRegion(const std::vector<IdentifierHash>&);
      void endEvent();
      
      ///////////////////////////////////////////////////////////////////
      // Methods of seeds production without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find();

      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual Trk::TrackSegment* next();
  
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump          (MsgStream   & out) const;
      std::ostream& dump          (std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      
      std::string                            m_fieldmode       ; // Mode of magnetic field

      std::string                            m_ntrtmanager     ; // Name of TRT det. manager 
      std::string                            m_callbackString  ;
      ToolHandle<Trk::IPropagator>           m_propTool        ; // Propagator            tool
      ToolHandle<ITRT_TrackExtensionTool>    m_extensionTool   ; // TRT track extension   tool
      ToolHandle<Trk::IPRD_AssociationTool>  m_assoTool        ; // Track-PRD association tool

      Trk::MagneticFieldProperties           m_fieldprop       ; // Magnetic field properties
      const InDetDD::TRT_DetectorManager   * m_trtmgr          ;
      const TRT_ID                         * m_trtid           ; 
      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_trtname{this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve TRT_DriftCircles"}; // TRTs   container ; // Name  TRT container
      
      bool                                   m_build           ;
      bool                                   m_gupdate         ;
      bool                                   m_useassoTool     ;
      bool                                   m_removeNoise     ;
      int                                    m_outputlevel     ;
      int                                    m_nprint          ;
      int                                    m_clustersCut     ;
      int                                    m_clusters        ;
      int                                    m_nlocal          ;
      int                                    m_nsegments       ;
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
      TRT_DriftCircleLinkN_xk*               m_circles         ;

      union {unsigned char H[227500*4]; unsigned int H4[227500];} m_U;
      std::multimap<unsigned int,unsigned int> 
	m_bincluster      ;
      std::multimap<unsigned int,unsigned int>                  
	m_sizebin         ;
      std::multimap<unsigned int,unsigned int>::reverse_iterator  
	m_sizebin_iterator;
      std::list<Trk::TrackSegment*>            m_segments   ;
      std::list<Trk::TrackSegment*>::iterator  m_segiterator;

      std::multimap<const InDet::TRT_DriftCircle*,Trk::TrackSegment*> m_clusterSegment;
      std::multimap<int                          ,Trk::TrackSegment*> m_qualitySegment;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      StatusCode mapStrawsUpdate    (IOVSVC_CALLBACK_ARGS);
      void magneticFieldInit();
      void mapStrawsProduction();
      void eraseHistogramm();
      void fillHistogramm   (float,int);
      void analyseHistogramm(unsigned char&,unsigned int&,float,int);
      unsigned int localMaximum(unsigned int);
      void findLocaly(unsigned int); 
      void segmentsPreparation();
      
      MsgStream&    dumpConditions(MsgStream   & out) const;
      MsgStream&    dumpEvent     (MsgStream   & out) const;
    };

  MsgStream&    operator << (MsgStream&   ,const TRT_TrackSegmentsMaker_ATLxk&);
  std::ostream& operator << (std::ostream&,const TRT_TrackSegmentsMaker_ATLxk&); 

} // end of name space

#endif // TRT_TrackSegmentsMaker_ATLxk_H


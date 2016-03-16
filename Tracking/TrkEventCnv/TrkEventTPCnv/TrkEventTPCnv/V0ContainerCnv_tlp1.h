/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  V0CONTAINER_CNV_TLP1_TRK_H
#define  V0CONTAINER_CNV_TLP1_TRK_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "V0Container_tlp1.h"

//-----------------------------------------
// TrkV0Vertex
//-----------------------------------------
#include "TrkEventTPCnv/TrkV0Vertex/V0ContainerCnv_p1.h"   
#include "TrkEventTPCnv/TrkV0Vertex/V0CandidateCnv_p1.h" 
#include "TrkEventTPCnv/TrkV0Vertex/V0HypothesisCnv_p1.h"

//-----------------------------------------
// VxVertex
//-----------------------------------------
#include "TrkEventTPCnv/VxVertex/ExtendedVxCandidateCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VxCandidateCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/PerigeeCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigeeCnv_p1.h"

//-----------------------------------------------------------------------------
// Surfaces and related
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"  

//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p1.h"

class V0ContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< V0ContainerCnv_p1, Trk::V0Container_tlp1 >
{

 public:

  V0ContainerCnv_tlp1();
  virtual ~V0ContainerCnv_tlp1() {}

//default methods  
  virtual void	setPStorage( Trk::V0Container_tlp1 *storage );
 
  virtual unsigned short       getConverterID() { return 2; }

 protected: 
  
//TrkV0Vertex    
  V0CandidateCnv_p1	   m_v0CandidateCnv;
  V0HypothesisCnv_p1   m_v0HypothesisCnv;
  
//VxVertex  
  ExtendedVxCandidateCnv_p1  m_extendedVxCandidateCnv;
  VxCandidateCnv_p1          m_vxCandidateCnv;
  VxTrackAtVertexCnv_p1      m_vxTrackAtVertexCnv;    
  RecVertexCnv_p1            m_recVertexCnv;
  VertexCnv_p1               m_VertexCnv;

//TrkTrack 
   TrackCnv_p1                m_trackCnv;
   
//TrkParameters   
   TrackParametersCnv_p1      m_trackParametersCnv;
   PerigeeCnv_p1              m_perigeeCnv;
   MeasuredPerigeeCnv_p1      m_measuredPerigeeCnv;
   
//Surfaces and related   
   PerigeeSurfaceCnv_p1       m_perigeeSurfacesCnv;
   
//TrkEventPrimitives
   FitQualityCnv_p1	      m_fitQualityCnv;
   ErrorMatrixCnv_p1	      m_errorMatrixCnv;   

}; //end of class definitions


template <>
class T_TPCnv<V0Container, Trk::V0Container_tlp1 >
  : public V0ContainerCnv_tlp1
{
public:
  // ??? Maybe this should be moved up to AthenaPoolTopLevelTPConverter...
  virtual void persToTrans (const Trk::V0Container_tlp1* pers,
                            V0Container* trans,
                            MsgStream& msg);
};


#endif //V0CONTAINER_CNV_TLP1_TRK_H

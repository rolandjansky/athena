/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCONTAINER_TLP1_TRK_H
#define VXCONTAINER_TLP1_TRK_H

//-----------------------------------------
// VxVertex
//-----------------------------------------
#include "TrkEventTPCnv/VxVertex/VxContainer_p1.h"
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/ExtendedVxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p2.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p1.h"
#include "TrkEventTPCnv/TrkParameters/MeasuredPerigee_p1.h"

//-----------------------------------------------------------------------------
// Surfaces and related
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkSurfaces/Surface_p1.h"


//-----------------------------------------------------------------------------
// TrkEventPrimitives
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/Track_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

//#include "GaudiKernel/MsgStream.h"
#include <ios>

namespace Trk
{
 class VxContainer_tlp1 
 {
 
  public:
  
   VxContainer_tlp1(){}  
   
   TPCnvTokenList_p1		             m_tokenList;

//VxVertex
   std::vector<VxContainer_p1>               m_vxContainers;
   std::vector<Trk::VxCandidate_p1>          m_vxCandidates;
   std::vector<Trk::ExtendedVxCandidate_p1>  m_extendedVxCandidates;
   std::vector<Trk::VxTrackAtVertex_p1>      m_vxTrackAtVertices;
   // std::vector<Trk::VxTrackAtVertex_p2>      m_vxTrackAtVertices_p2;
   std::vector<Trk::RecVertex_p1>            m_recVertices;
   std::vector<Trk::Vertex_p1>               m_vertices;

//TrkTrack 
   std::vector<Trk::Track_p1>                m_tracks;
   
//TrkParameters   
   std::vector<Trk::TrackParameters_p1>      m_trackParameters;
   std::vector<Trk::Perigee_p1>              m_perigees;
   std::vector<Trk::MeasuredPerigee_p1>      m_measPerigees;
   
//Surfaces and co 
  std::vector< Trk::Surface_p1 >	     m_surfaces; 
   
//TrkEventPrimitives
   std::vector<Trk::FitQuality_p1>           m_fitQualities;
   std::vector<Trk::HepSymMatrix_p1>         m_hepSymMatrices;
   std::vector<Trk::LocalPosition_p1>        m_localPositions;
 
 };//end of class definitions
} //end of namespace definitions



#endif //VXCONTAINER_TLP1_TRK_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0CONTAINER_TLP2_TRK_H
#define V0CONTAINER_TLP2_TRK_H

//-----------------------------------------
// TrkV0Vertex
//-----------------------------------------
#include "TrkEventTPCnv/TrkV0Vertex/V0Container_p1.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Hypothesis_p1.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0Candidate_p1.h"

//-----------------------------------------
// VxVertex
//-----------------------------------------
#include "TrkEventTPCnv/VxVertex/ExtendedVxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertex_p2.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p2.h"
// #include "TrkEventTPCnv/VxVertex/Vertex_p2.h"

//-----------------------------------------------------------------------------
// Trk Parameters
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include "TrkEventTPCnv/TrkParameters/Perigee_p2.h"
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

//-----------------------------------------------------------------------------
// TrkTrack
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkTrack/Track_p12.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"


namespace Trk
{

 class V0Container_tlp2 
 {
  
  public:
  
   V0Container_tlp2(){}

//TrkV0Vertex 
   std::vector<V0Container_p1> 	             m_v0Containers;
   std::vector<Trk::V0Candidate_p1>	     m_v0Candidates;
   std::vector<Trk::V0Hypothesis_p1>         m_v0Hypothesises;
   
//VxVertex and related
   std::vector<Trk::ExtendedVxCandidate_p1>  m_extendedVxCandidates;   
   std::vector<Trk::VxCandidate_p1>          m_vxCandidates;  
   std::vector<Trk::VxTrackAtVertex_p2>      m_vxTracksAtVertex;
   std::vector<Trk::RecVertex_p2>            m_recVertices;
   // std::vector<Trk::Vertex_p2>               m_vertices; 

//TrkTrack 
   std::vector<Trk::Track_p12>                m_tracks;
   
//TrkParameters   
   std::vector<Trk::TrackParameters_p1>      m_trackParameters;
   std::vector<Trk::Perigee_p2>              m_perigees;
   std::vector<Trk::MeasuredPerigee_p1>      m_measPerigees;
   
//Surfaces and co 
   std::vector< Trk::Surface_p1 >	     m_surfaces; 
   
//TrkEventPrimitives
   // std::vector<Trk::FitQuality_p1>           m_fitQualities;
   std::vector<Trk::HepSymMatrix_p1>         m_hepSymMatrices;    
    
 };//end of class definitions
}//end of namespace definitions

#endif

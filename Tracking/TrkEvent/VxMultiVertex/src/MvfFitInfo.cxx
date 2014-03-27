/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxMultiVertex/MvfFitInfo.h"
#include "VxVertex/RecVertex.h"

namespace Trk
{

//default constructor, setting all pointers to zero 
 MvfFitInfo::MvfFitInfo():m_constraintVertex(0), m_seedVertex(0), m_linearizationVertex(0)
 {}
 
 MvfFitInfo::MvfFitInfo(Trk::RecVertex* constraintVertex,
                        Trk::Vertex* seedVertex,
                        Trk::Vertex* linearizationVertex):
			m_constraintVertex(constraintVertex),
                        m_seedVertex(seedVertex),
                        m_linearizationVertex(linearizationVertex)
 {}
     
 MvfFitInfo::~MvfFitInfo()
 { 
  if (m_constraintVertex!=0) delete m_constraintVertex; m_constraintVertex=0;
  if (m_seedVertex!=0) delete m_seedVertex; m_seedVertex=0;
  if (m_linearizationVertex!=0) delete m_linearizationVertex; m_linearizationVertex=0;
 }//end of destructor
 
  MvfFitInfo::MvfFitInfo(const MvfFitInfo& rhs):
   m_constraintVertex(rhs.m_constraintVertex ? new Trk::RecVertex(*rhs.m_constraintVertex) : 0),
   m_seedVertex(rhs.m_seedVertex ? new Trk::Vertex(*rhs.m_seedVertex) : 0),
   m_linearizationVertex(rhs.m_linearizationVertex ? new Trk::Vertex(*rhs.m_linearizationVertex) : 0)
   {}
  
  MvfFitInfo &  MvfFitInfo::operator= (const MvfFitInfo & rhs)
  {
   if (this!=&rhs)
   {
    m_constraintVertex = rhs.m_constraintVertex ? new RecVertex(*rhs.m_constraintVertex) : 0;
    m_seedVertex = rhs.m_seedVertex? new Vertex(*rhs.m_seedVertex) : 0;
    m_linearizationVertex = rhs.m_linearizationVertex ? new Vertex(*rhs.m_linearizationVertex) : 0;
   }
   return *this;
  } 
 
 
 void MvfFitInfo::setSeedVertex(Trk::Vertex* seedVertex)
 {
   if (m_seedVertex!=0) delete m_seedVertex;
   m_seedVertex=seedVertex;
 }
 
 
 void MvfFitInfo::setLinearizationVertex(Trk::Vertex* linearizationVertex)
 {
    if (m_linearizationVertex!=0) delete m_linearizationVertex;
    m_linearizationVertex=linearizationVertex;
 }
 
 
}//end of namespace definitions

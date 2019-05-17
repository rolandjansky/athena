/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VxMultiVertex/MvfFitInfo.h"

namespace Trk
{

//default constructor, setting all pointers to zero 
 MvfFitInfo::MvfFitInfo():m_constraintVertex(0), m_seedVertex(0), m_linearizationVertex(0)
 {}
 
 MvfFitInfo::MvfFitInfo(xAOD::Vertex* constraintVertex,
                        Amg::Vector3D* seedVertex,
                        Amg::Vector3D* linearizationVertex):
			m_constraintVertex(constraintVertex),
                        m_seedVertex(seedVertex),
                        m_linearizationVertex(linearizationVertex)
 {
   // TODO: makePrivateStore() on constraintVertex here if not done so already?
 }
     
 MvfFitInfo::~MvfFitInfo()
 { 
   delete m_constraintVertex;
   delete m_seedVertex;
   delete m_linearizationVertex;
 }//end of destructor
 
  MvfFitInfo::MvfFitInfo(const MvfFitInfo& rhs):
   m_constraintVertex(rhs.m_constraintVertex ? new xAOD::Vertex(*rhs.m_constraintVertex) : 0),
   m_seedVertex(rhs.m_seedVertex ? new Amg::Vector3D(*rhs.m_seedVertex) : 0),
   m_linearizationVertex(rhs.m_linearizationVertex ? new Amg::Vector3D(*rhs.m_linearizationVertex) : 0)
   {}
  
  MvfFitInfo &  MvfFitInfo::operator= (const MvfFitInfo & rhs)
  {
   if (this!=&rhs)
   {
    delete m_constraintVertex;
    m_constraintVertex = rhs.m_constraintVertex ? new xAOD::Vertex(*rhs.m_constraintVertex) : 0;
    delete m_seedVertex;
    m_seedVertex = rhs.m_seedVertex? new Amg::Vector3D(*rhs.m_seedVertex) : 0;
    delete m_linearizationVertex;
    m_linearizationVertex = rhs.m_linearizationVertex ? new Amg::Vector3D(*rhs.m_linearizationVertex) : 0;
   }
   return *this;
  }
 
 
 void MvfFitInfo::setSeedVertex(Amg::Vector3D* seedVertex)
 {
   if (m_seedVertex!=0) delete m_seedVertex;
   m_seedVertex=seedVertex;
 }
 
 
 void MvfFitInfo::setLinearizationVertex(Amg::Vector3D* linearizationVertex)
 {
    if (m_linearizationVertex!=0) delete m_linearizationVertex;
    m_linearizationVertex=linearizationVertex;
 }
 
 
}//end of namespace definitions

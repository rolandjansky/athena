/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxSecVertexInfo.h - Description
           -------------------

    begin: March 2007
    authors: CSC vertexing note group members :-)
    work started by: Giacinto Piacquadio (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes: new!

    April 2014:  Replace Trk::VxCandidate by xAOD::Vertex
********************************************/

#include "VxSecVertex/VxSecVertexInfo.h"
#include <ostream>

namespace Trk {
  
  VxSecVertexInfo::VxSecVertexInfo(): m_SVOwnership(false) {}

  VxSecVertexInfo::VxSecVertexInfo(const std::vector<xAOD::Vertex*> & vertices): m_SVOwnership(false) {
    m_vertices=vertices;//ownership is taken over by the VxSecVertexInfo class!
  }
  
  VxSecVertexInfo::~VxSecVertexInfo() {
    std::vector<xAOD::Vertex*>::const_iterator verticesBegin=m_vertices.begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd=m_vertices.end();

    if(m_SVOwnership){  // Delete vertices only if ownership is set explicitly
      for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin;
	   verticesIter!=verticesEnd;++verticesIter) {
        xAOD::Vertex* myCandidate=*verticesIter;
        if (myCandidate!=0) {
	  delete myCandidate;
        } else {
	  std::cout << " Warning in VxSecVertexInfo: tried to delete a zero pointer --> Inconsistency found. " << std::endl;
        }
      }
    }
  }

  VxSecVertexInfo::VxSecVertexInfo(const VxSecVertexInfo & rhs) {

    std::vector<xAOD::Vertex*>::const_iterator verticesBegin=rhs.m_vertices.begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd=rhs.m_vertices.end();
    
    for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin;
	 verticesIter!=verticesEnd;++verticesIter) {
      xAOD::Vertex* myCandidate=*verticesIter;
      if (myCandidate!=0) {
	m_vertices.push_back(new xAOD::Vertex(*myCandidate));
      } else {
	std::cout << " Warning in VxSecVertexInfo: tried in copy constructor to copy over a zero pointer Vertex " << std::endl;
      }
    }
    m_SVOwnership=false;
  }

  VxSecVertexInfo& VxSecVertexInfo::operator= (const VxSecVertexInfo & rhs) {

    if (this!=&rhs) {

      m_vertices.clear();
      
      std::vector<xAOD::Vertex*>::const_iterator verticesBegin=rhs.m_vertices.begin();
      std::vector<xAOD::Vertex*>::const_iterator verticesEnd=rhs.m_vertices.end();
      
      for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin;
	   verticesIter!=verticesEnd;++verticesIter) {
	xAOD::Vertex* myCandidate=*verticesIter;
	if (myCandidate!=0) {
	  m_vertices.push_back(new xAOD::Vertex(*myCandidate));
	} else {
	  std::cout << " Warning in VxSecVertexInfo: tried in copy constructor to copy over a zero pointer Vertex " << std::endl;
	}
      }   
      m_SVOwnership=false;
    }
    return *this;
  }
  

  const std::vector<xAOD::Vertex*> & VxSecVertexInfo::vertices() const {
    return m_vertices;
  }

  void VxSecVertexInfo::setVertices(const std::vector<xAOD::Vertex*> & vertices) {
    m_vertices=vertices;
  }
    
}//end namespace Trk

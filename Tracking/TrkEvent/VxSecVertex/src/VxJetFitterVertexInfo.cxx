/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxSecVertexInfo.h - Description
           -------------------

    begin: January 2008
    author: Giacinto Piacquadio (University of Freiburg)
    e-mail: giacinto.piacquadio@physik.uni-freiburg.de
    changes: new!

    April 2014:  Trk::VxCandidate is replaced by xAOD::Vertex

********************************************/

#include "VxJetVertex/VxJetCandidate.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxJetVertex/SelectedTracksInJet.h"
#include "VxVertex/VxCandidate.h"

namespace Trk {

  VxJetFitterVertexInfo::VxJetFitterVertexInfo() :
          VxSecVertexInfo(),
          m_twoTrackVerticesInJet(0),
          m_selectedTracksInJet(0) {}

  VxJetFitterVertexInfo::VxJetFitterVertexInfo(const std::vector<Trk::VxJetCandidate*> & vertices,
                                               const Trk::TwoTrackVerticesInJet * twoTrackVertices,
                                               const Trk::SelectedTracksInJet* selectedTracksInJet) :
          VxSecVertexInfo(),
          m_twoTrackVerticesInJet(twoTrackVertices),
          m_selectedTracksInJet(selectedTracksInJet),
          m_verticesJF(vertices)
  {}
  

  VxJetFitterVertexInfo::VxJetFitterVertexInfo(const VxJetFitterVertexInfo & rhs):
    VxSecVertexInfo(rhs),
    m_twoTrackVerticesInJet(rhs.m_twoTrackVerticesInJet?new Trk::TwoTrackVerticesInJet(*rhs.m_twoTrackVerticesInJet):0),    
    m_selectedTracksInJet(rhs.m_selectedTracksInJet?new Trk::SelectedTracksInJet(*rhs.m_selectedTracksInJet):0)
  {
    std::vector<Trk::VxJetCandidate*>::const_iterator verticesBegin=rhs.m_verticesJF.begin();
    std::vector<Trk::VxJetCandidate*>::const_iterator verticesEnd=rhs.m_verticesJF.end();
    
    for (std::vector<Trk::VxJetCandidate*>::const_iterator verticesIter=verticesBegin;
	 verticesIter!=verticesEnd;++verticesIter) {
      Trk::VxJetCandidate* myCandidate=*verticesIter;
      if (myCandidate!=0) {
	m_verticesJF.push_back(new Trk::VxJetCandidate(*myCandidate));
      } else {
	std::cout << " Warning in VxSecVertexInfo: tried in copy constructor to copy over a zero pointer Vertex " << std::endl;
      }
    }
    m_SVOwnership=true;    
  }
  

  VxJetFitterVertexInfo & VxJetFitterVertexInfo::operator= (const VxJetFitterVertexInfo & rhs) {
    if (this!=&rhs) {

      m_verticesJF.clear();
      delete m_twoTrackVerticesInJet;
      delete m_selectedTracksInJet;
      this->operator=(rhs);
      std::vector<Trk::VxJetCandidate*>::const_iterator verticesBegin=rhs.m_verticesJF.begin();
      std::vector<Trk::VxJetCandidate*>::const_iterator verticesEnd=rhs.m_verticesJF.end();
      
      for (std::vector<Trk::VxJetCandidate*>::const_iterator verticesIter=verticesBegin;
	   verticesIter!=verticesEnd;++verticesIter) {
	Trk::VxJetCandidate* myCandidate=*verticesIter;
	if (myCandidate!=0) {
	  m_verticesJF.push_back(new Trk::VxJetCandidate(*myCandidate));
	} else {
	  std::cout << " Warning in VxSecVertexInfo: tried in copy constructor to copy over a zero pointer Vertex " << std::endl;
	}
      }   
      m_twoTrackVerticesInJet=rhs.m_twoTrackVerticesInJet?new Trk::TwoTrackVerticesInJet(*rhs.m_twoTrackVerticesInJet):0;
      m_selectedTracksInJet=rhs.m_selectedTracksInJet?new Trk::SelectedTracksInJet(*rhs.m_selectedTracksInJet):0;
      m_SVOwnership=true;    
    }
    return *this;
  }
  
  VxJetFitterVertexInfo::~VxJetFitterVertexInfo() {
    if(m_SVOwnership){  // Delete vertices only if ownership is set explicitly
      delete m_twoTrackVerticesInJet;
      delete m_selectedTracksInJet;
      std::vector<Trk::VxJetCandidate*>::const_iterator verticesBegin=m_verticesJF.begin();
      std::vector<Trk::VxJetCandidate*>::const_iterator verticesEnd=m_verticesJF.end();
      for (std::vector<Trk::VxJetCandidate*>::const_iterator verticesIter=verticesBegin;
	   verticesIter!=verticesEnd;++verticesIter) {
        Trk::VxJetCandidate* myCandidate=*verticesIter;
        if (myCandidate!=0) {
	  delete myCandidate;
        } else {
	  std::cout << " Warning in VxSecVertexInfo: tried to delete a zero pointer --> Inconsistency found. " << std::endl;
        }
      }
    }
  }

}//end namespace

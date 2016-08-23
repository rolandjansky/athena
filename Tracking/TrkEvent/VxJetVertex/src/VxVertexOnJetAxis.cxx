/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxVertexOnJetAxis.cxx  -  Description
                             -------------------
    begin   : Februar 2007
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

    (C) Atlas Collaboration 2007

   More information contained in the header file 

 ***************************************************************************/

#include "VxJetVertex/VxVertexOnJetAxis.h"


namespace Trk {


  VxVertexOnJetAxis::VxVertexOnJetAxis() :
    m_numVertex(-1),
    m_fitQuality(Trk::FitQuality(0.,0.)),
    m_compatibilityToPrimaryVtx(-1) {}
  
  VxVertexOnJetAxis::VxVertexOnJetAxis(const std::vector<VxTrackAtVertex*> & tracksAtVertex): 
    m_tracksAtVertex(tracksAtVertex),
    m_numVertex(-1),
    m_fitQuality(Trk::FitQuality(0.,0.)),
    m_compatibilityToPrimaryVtx(-1) {}
  
  VxVertexOnJetAxis::VxVertexOnJetAxis(const std::vector<VxTrackAtVertex*> & tracksAtVertex,
				       int numVertex): 
    m_tracksAtVertex(tracksAtVertex),
    m_numVertex(numVertex),
    m_fitQuality(Trk::FitQuality(0.,0.)),
    m_compatibilityToPrimaryVtx(-1) {}
  
  VxVertexOnJetAxis::VxVertexOnJetAxis(const VxVertexOnJetAxis& rhs):
    m_tracksAtVertex(rhs.m_tracksAtVertex),
    m_numVertex(rhs.m_numVertex),
    m_fitQuality(rhs.m_fitQuality),
    m_compatibilityToPrimaryVtx(rhs.m_compatibilityToPrimaryVtx)
  { }
  
  VxVertexOnJetAxis &VxVertexOnJetAxis::operator= (const VxVertexOnJetAxis& rhs)
  {
    if (this!=&rhs)
      {
	// you need to call the base class assignment operator
	m_tracksAtVertex=rhs.m_tracksAtVertex;
	m_numVertex=rhs.m_numVertex;
	m_fitQuality=rhs.m_fitQuality;
	m_compatibilityToPrimaryVtx=rhs.m_compatibilityToPrimaryVtx;
      }
    return *this;
  }
  
  MsgStream& VxVertexOnJetAxis::dump(MsgStream& sl) const {
    sl << "Trk::VxVertexOnJetAxis: no info implemented sofar" << std::endl;
    return sl;
  }

  std::ostream& VxVertexOnJetAxis::dump(std::ostream& sl) const {
    sl << "Trk::VxVertexOnJetAxis: no info implemented sofar" << std::endl;
    return sl;
  }

  VxVertexOnJetAxis::~VxVertexOnJetAxis() {}

  const Trk::FitQuality& VxVertexOnJetAxis::fitQuality() const {
    return m_fitQuality;
  }

  void VxVertexOnJetAxis::setFitQuality(const Trk::FitQuality& fitQuality) {
    m_fitQuality=fitQuality;
  }

  void VxVertexOnJetAxis::setNumVertex(int numVertex) {
    m_numVertex=numVertex;
  }
  
  int VxVertexOnJetAxis::getNumVertex(void) const {
    return m_numVertex;
  }
  
  const std::vector<VxTrackAtVertex*> & VxVertexOnJetAxis::getTracksAtVertex(void) const {
    return m_tracksAtVertex;
  }
  
  void VxVertexOnJetAxis::setTracksAtVertex(const std::vector<VxTrackAtVertex*> & tracksAtVertex) {
    m_tracksAtVertex=tracksAtVertex;
  }

  /**
   * get compatibility to the primary vertex
   */
  float VxVertexOnJetAxis::getCompatibilityToPrimaryVtx(void) const {
    return m_compatibilityToPrimaryVtx;
  }


  /**
   * set compatibility to the primary vertex
   */
  void VxVertexOnJetAxis::setCompatibilityToPrimaryVtx(float newcomp) {
    m_compatibilityToPrimaryVtx=newcomp;
  }



  
  
} // end of namespace

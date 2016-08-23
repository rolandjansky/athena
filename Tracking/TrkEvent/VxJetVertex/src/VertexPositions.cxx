/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VertexPositions.cxx  -  Description
                             -------------------
    begin   : Autumn 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : Giacinto.Piacquadio@physik.uni-freiburg.de
    comments: 
    changes : 
              
 ***************************************************************************/

#include "VxJetVertex/VertexPositions.h"

namespace Trk {

  unsigned int VertexPositions::s_numberOfInstantiations=0;

  VertexPositions::VertexPositions() : m_position(Amg::VectorX()),
				       m_useWeightTimesPosition(false) {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
    m_position.setZero();
  }

  VertexPositions::VertexPositions(const Amg::VectorX &p) : m_position(p),
						            m_useWeightTimesPosition(false) {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
  }

  VertexPositions::VertexPositions(const VertexPositions& rhs) : m_position(rhs.m_position),
								 m_useWeightTimesPosition(rhs.m_useWeightTimesPosition)
  {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
  }
  
  VertexPositions & VertexPositions::operator= (const VertexPositions& rhs)
  {
    if (this!=&rhs)
    {
      m_position = rhs.m_position;
      m_useWeightTimesPosition=rhs.m_useWeightTimesPosition;
    }
    return *this;
  }
  
  MsgStream& VertexPositions::dump(MsgStream& sl) const {
    if (m_useWeightTimesPosition) {
      sl << "Trk::VertexPositions weight times position: (" ;
    } else {
      sl << "Trk::VertexPositions position: (" ;
    }
    sl   << "xv " << m_position[jet_xv] << ", "
	 << "yv " << m_position[jet_yv] << ", "
	 << "zv " << m_position[jet_zv] << ", "
	 << "phi " << m_position[jet_phi] << ", "
	 << "theta " << m_position[jet_theta] << endreq;
    for (int i=5;i<m_position.rows();i++) {
      sl << "dist" << i << " " << m_position[i] << " ." << endreq;
    }
    return sl;
  }

  std::ostream& VertexPositions::dump(std::ostream& sl) const {
    if (m_useWeightTimesPosition) {
      sl << "Trk::VertexPositions weight times position: (" ;
    } else {
      sl << "Trk::VertexPositions position: (" ;
    }
    sl << "xv " << m_position[jet_xv] << ", "
       << "yv " << m_position[jet_yv] << ", "
       << "zv " << m_position[jet_zv] << ", "
       << "phi " << m_position[jet_phi] << ", "
       << "theta " << m_position[jet_theta] << std::endl;
    for (int i=5;i<m_position.rows();i++) {
      sl << "dist" << i << " " << m_position[i] << " ." << std::endl;
    }
    return sl;
  }
  
  MsgStream& operator << ( MsgStream& sl, const VertexPositions& sf) 
  { return sf.dump(sl); }

  std::ostream& operator << ( std::ostream& sl, const VertexPositions& sf) 
  { return sf.dump(sl); }
  
  VertexPositions::~VertexPositions() {
#ifndef NDEBUG
    s_numberOfInstantiations--; // delete VertexPositions, so decrement total count
#endif
  }
  
  const Amg::VectorX& VertexPositions::position() const {
    if (!m_useWeightTimesPosition) {
      return m_position; 
    }
    std::cout << "FATAL: VertexPositions is not able to return a valid position " << 
      " as a const object: need to go from Update to Use mode. Unrecovered Bug!" << std::endl;
    throw;
    //    return m_position;

  }

  void VertexPositions::setPosition(const Amg::VectorX & newposition) {
    m_position=newposition;
  }

  unsigned int VertexPositions::numberOfInstantiations()
  {
    return s_numberOfInstantiations;
  }
  
} // end of namespace

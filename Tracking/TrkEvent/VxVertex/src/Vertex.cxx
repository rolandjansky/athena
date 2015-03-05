/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           Vertex.cxx  -  Description
                             -------------------
    begin   : Autumn 2003
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    comments: original version by M. Elsing
    changes : 
              
 ***************************************************************************/

#include "VxVertex/Vertex.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {

  unsigned int Vertex::s_numberOfInstantiations=0;

  Vertex::Vertex() : m_position(0.,0.,0.){
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
  }

  Vertex::Vertex(const Amg::Vector3D &p) : m_position(p) {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
  }

  Vertex::Vertex(const Vertex& rhs) : m_position(rhs.m_position)
  {
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Vertex, so increment total count
#endif
  }
  
  Vertex &Vertex::operator= (const Vertex& rhs)
  {
    if (this!=&rhs)
    {
      m_position = rhs.m_position;
    }
    return *this;
  }
  
  MsgStream& Vertex::dump(MsgStream& sl) const {
    sl << "Trk::Vertex position: (" 
       << m_position[0] << ", "
       << m_position[1] << ", "
       << m_position[2] << ") mm."
       << endreq;
    return sl;
  }

  std::ostream& Vertex::dump(std::ostream& sl) const {
    sl << "Trk::Vertex position: (" 
       << m_position[0] << ", "
       << m_position[1] << ", "
       << m_position[2] << ") mm."
       << std::endl;
    return sl;
  }
  
  MsgStream& operator << ( MsgStream& sl, const Vertex& sf) 
  { return sf.dump(sl); }

  std::ostream& operator << ( std::ostream& sl, const Vertex& sf) 
  { return sf.dump(sl); }
  
  Vertex::~Vertex() {
#ifndef NDEBUG
    s_numberOfInstantiations--; // delete Vertex, so decrement total count
#endif
  }

  const Amg::Vector3D& Vertex::position() const { return m_position; }

  unsigned int Vertex::numberOfInstantiations()
  {
    return s_numberOfInstantiations;
  }
  
} // end of namespace

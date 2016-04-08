/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxCandidate.cxx  -  Description
                             -------------------
    begin   : Autumn 2003
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch

 ***************************************************************************/

#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {
  unsigned int VxCandidate::s_numberOfInstantiations=0;

  VxCandidate::VxCandidate() : m_vertexType(Trk::NotSpecified), m_recVertex(), m_vxTrackAtVertex()
  {
#ifndef NDEBUG
    s_numberOfInstantiations++;
#endif
  }

  VxCandidate::VxCandidate(
    const Trk::RecVertex& recVertex,
    const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex)
  : m_vertexType(Trk::NotSpecified), m_recVertex(recVertex), m_vxTrackAtVertex(vxTrackAtVertex)
  {
#ifndef NDEBUG
    s_numberOfInstantiations++;
#endif
  }

  VxCandidate::VxCandidate(
    Trk::RecVertex&& recVertex,
    std::vector<Trk::VxTrackAtVertex*>&& vxTrackAtVertex)
    : m_vertexType(Trk::NotSpecified), m_recVertex(std::move(recVertex)), m_vxTrackAtVertex(std::move(vxTrackAtVertex))
  {
#ifndef NDEBUG
    s_numberOfInstantiations++;
#endif
  }

  VxCandidate::VxCandidate(const VxCandidate& rhs) :
      m_vertexType(rhs.m_vertexType),
      m_recVertex(rhs.m_recVertex),
      m_vxTrackAtVertex(std::vector<Trk::VxTrackAtVertex*>())
    {
      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = rhs.m_vxTrackAtVertex.begin(); itr != rhs.m_vxTrackAtVertex.end(); ++itr)
      {
        m_vxTrackAtVertex.push_back((*itr)->clone());
      }
#ifndef NDEBUG
      s_numberOfInstantiations++;
#endif
    }//end of copy-constructor
  
  VxCandidate &VxCandidate::operator= (const VxCandidate& rhs)
  {
    if (this!=&rhs)
    {
      m_recVertex = rhs.m_recVertex;
      m_vertexType = rhs.m_vertexType;
      // delete objects where pointers pointed to
      // (otherwise -> memory leak)
      for (std::vector<Trk::VxTrackAtVertex*>::iterator i = m_vxTrackAtVertex.begin();
              i != m_vxTrackAtVertex.end() ; ++i)
      {
          delete (*i);
          (*i) = 0;
      }
      // and clear the vector
      m_vxTrackAtVertex.clear();
      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = rhs.m_vxTrackAtVertex.begin();
              itr != rhs.m_vxTrackAtVertex.end(); ++itr)
      {
          m_vxTrackAtVertex.push_back((*itr)->clone());
      }
    }
    return *this;
  }

  VxCandidate &VxCandidate::operator= (VxCandidate&& rhs)
  {
    if (this!=&rhs)
    {
      m_recVertex = std::move(rhs.m_recVertex);
      m_vertexType = rhs.m_vertexType;
      for (Trk::VxTrackAtVertex* tav : m_vxTrackAtVertex)
        delete tav;
      m_vxTrackAtVertex = std::move(rhs.m_vxTrackAtVertex);
    }
    return *this;
  }

  VxCandidate::~VxCandidate() { 
    for (std::vector<Trk::VxTrackAtVertex*>::iterator i = m_vxTrackAtVertex.begin();
	 i != m_vxTrackAtVertex.end() ; ++i) {
      delete (*i);
      (*i) = 0;
    }
    // and clear the vector
    m_vxTrackAtVertex.clear();
#ifndef NDEBUG
    s_numberOfInstantiations--;
#endif
  }

  MsgStream& VxCandidate::dump(MsgStream& sl) const {
    sl << "Printing Trk::VxCandidate of type: " << m_vertexType << endreq;
    sl << m_recVertex << endreq;
    sl << "Tracks used in the vertex fit: " << m_vxTrackAtVertex.size() << endreq;
    for (unsigned i = 0 ; i < m_vxTrackAtVertex.size() ; i++)
    {
      sl << "Track " << i+1 << " " << *(m_vxTrackAtVertex[i]);
    }
    return sl;
  }

  std::ostream& VxCandidate::dump(std::ostream& sl) const {
    sl << "Printing Trk::VxCandidate of type: " << m_vertexType << std::endl;
    sl << m_recVertex << std::endl;
    sl << "Tracks used in the vertex fit: " << m_vxTrackAtVertex.size() << std::endl;
    for (unsigned i = 0 ; i < m_vxTrackAtVertex.size() ; i++)
    {
      sl << "Track " << i+1 << " " << *(m_vxTrackAtVertex[i]);
    }
    return sl;
  }
  
  MsgStream& operator << ( MsgStream& sl, const VxCandidate& sf) 
  { return sf.dump(sl); }

  std::ostream& operator << ( std::ostream& sl, const VxCandidate& sf) 
  { return sf.dump(sl); }

  unsigned int VxCandidate::numberOfInstantiations()
  {
    return s_numberOfInstantiations;
  }

} // end of namespace

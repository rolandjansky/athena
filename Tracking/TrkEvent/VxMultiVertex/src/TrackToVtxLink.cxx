/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxTrackAtVertex.cxx  -  Description
                             -------------------
    begin   : May 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
    comments: 
    changes : 18/7/07 deleted all references to Track / TrackParticleBase origin
              since new EDM automatically deals with it... (thanks to Kirill/Andi's new LinkToTrack/
              LinkToTrackParticle)

              David Shope <david.richard.shope@cern.ch> (2016-04-19)
              Vertex objects stored in this class are now xAOD::Vertex (from Trk::VxCandidate)

 ***************************************************************************/

#include "VxMultiVertex/TrackToVtxLink.h"

namespace Trk {

  // TODO: think about if handed vertices with no private store yet

  TrackToVtxLink::TrackToVtxLink() : m_vertices(nullptr) {}
                             
  //constructor with pointers to vertices
  TrackToVtxLink::TrackToVtxLink(std::vector<xAOD::Vertex*>* ptrToVertices) {
    m_vertices=ptrToVertices;
  }

  TrackToVtxLink::~TrackToVtxLink() 
  {   
    if (m_vertices!=nullptr) {
      delete m_vertices; 
      m_vertices=nullptr;
    }
  }
  

  TrackToVtxLink::TrackToVtxLink(const TrackToVtxLink& rhs) :
    m_vertices(rhs.m_vertices ? new std::vector<xAOD::Vertex*>(*rhs.m_vertices) : nullptr) { }
  

  //assignement operator changed
  TrackToVtxLink & TrackToVtxLink::operator= (const TrackToVtxLink& rhs)
  {
    if (this!=&rhs)
      {
        delete m_vertices;
	m_vertices = m_vertices ? new std::vector<xAOD::Vertex*>(*rhs.m_vertices) : nullptr;
      }
    return *this;
  }

} // end of namespace

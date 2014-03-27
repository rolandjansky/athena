/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

 ***************************************************************************/

#include "VxVertex/VxCandidate.h"
#include "VxMultiVertex/TrackToVtxLink.h"

namespace Trk {
  
  TrackToVtxLink::TrackToVtxLink() : m_vertexes(0) {}
                             
  //constructor with pointers to vertexes
  TrackToVtxLink::TrackToVtxLink(std::vector<VxCandidate*>* ptrToVertexes) {
    m_vertexes=ptrToVertexes;
  }

  TrackToVtxLink::~TrackToVtxLink() 
  {   
    if (m_vertexes!=0) {
      delete m_vertexes; 
      m_vertexes=0;
    }
  }
  

  TrackToVtxLink::TrackToVtxLink(const TrackToVtxLink& rhs) :
    m_vertexes(rhs.m_vertexes ? new std::vector<VxCandidate*>(*rhs.m_vertexes) : 0) { }
  

  //assignement operator changed
  TrackToVtxLink & TrackToVtxLink::operator= (const TrackToVtxLink& rhs)
  {
    if (this!=&rhs)
      {
	m_vertexes = m_vertexes ? new std::vector<VxCandidate*>(*rhs.m_vertexes) : 0;
      }
    return *this;
  }

} // end of namespace

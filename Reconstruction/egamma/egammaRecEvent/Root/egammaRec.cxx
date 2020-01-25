/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRecEvent/egammaRec.h"
egammaRec::egammaRec()
{
  //This we do not know the exact size 
  //reserve something reasonable 
  m_caloClusters.reserve(2);
  m_trackParticles.reserve(4);
  m_vertices.reserve(4);
  // init to -999
  m_deltaEta = {-999,-999,-999,-999};
  m_deltaPhi = {-999,-999,-999,-999};
  m_deltaPhiRescaled= {-999,-999,-999,-999};
  m_deltaPhiLast = -999.;
  m_deltaEtaVtx = -999;
  m_deltaPhiVtx = -999;
}

const xAOD::CaloCluster*
egammaRec::caloCluster(size_t index) const
{
  if (index >= m_caloClusters.size() || !m_caloClusters[index].isValid()){
    return nullptr;
  }
  return (*(m_caloClusters[index]));
}

const ElementLink<xAOD::CaloClusterContainer>
egammaRec::caloClusterElementLink(size_t index) const
{
  if (index >= m_caloClusters.size() || !m_caloClusters[index].isValid()){
    return ElementLink<xAOD::CaloClusterContainer>();
  }
  return m_caloClusters[index];
}

const xAOD::TrackParticle*
egammaRec::trackParticle(size_t index) const
{
  if (index >= m_trackParticles.size() || !m_trackParticles[index].isValid()){
    return nullptr;
  }
  return (*(m_trackParticles[index]));
}

const ElementLink<xAOD::TrackParticleContainer>
egammaRec::trackParticleElementLink(size_t index) const
{
  if (index >= m_trackParticles.size() || !m_trackParticles[index].isValid()){
    return ElementLink<xAOD::TrackParticleContainer>();
  }
  return m_trackParticles[index];
}

const xAOD::Vertex*
egammaRec::vertex(size_t index) const
{
  if (index >= m_vertices.size() || !m_vertices[index].isValid()){
    return nullptr;
  }
  return (*(m_vertices[index]));
}

const ElementLink<xAOD::VertexContainer>
egammaRec::vertexElementLink(size_t index) const{
  if (index >= m_vertices.size() || !m_vertices[index].isValid()){
    return ElementLink<xAOD::VertexContainer>();
  }
  return m_vertices.at(index);
}

void
egammaRec::pushBackVertex(const ElementLink<xAOD::VertexContainer>& vertexElementLink)
{
  m_vertices.push_back(vertexElementLink);
}

void
egammaRec::pushFrontVertex(const ElementLink<xAOD::VertexContainer>& vertexElementLink)
{
  m_vertices.insert(m_vertices.begin(), vertexElementLink);
}


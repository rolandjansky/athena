/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaRecEvent/egammaRec.h"

egammaRec::egammaRec(){
  //We do not how many reserve
  m_caloClusters.reserve(4);   
  m_trackParticles.reserve(4); 
  m_vertices.reserve(4);
  //This has to have size 4
  m_deltaEta.resize(4);
  m_deltaPhi.resize(4);
  m_deltaPhiRescaled.resize(4);
  //init to -999
  m_deltaPhiLast=-999.;
  m_deltaEtaVtx = -999;
  m_deltaPhiVtx = -999;
}

size_t egammaRec::getNumberOfClusters() const{
  return m_caloClusters.size();
}
     
const xAOD::CaloCluster* egammaRec::caloCluster( size_t index) const{

  if( index >= m_caloClusters.size() || !m_caloClusters.at(index).isValid() ) return 0;
  return (*(m_caloClusters.at(index)));
}
  
const ElementLink< xAOD::CaloClusterContainer > egammaRec::caloClusterElementLink( size_t index) const{

  if( index >= m_caloClusters.size() || ! m_caloClusters.at(index).isValid() ) return ElementLink< xAOD::CaloClusterContainer >();
  return m_caloClusters.at(index);
} 
     
void egammaRec::setCaloClusters( const std::vector< ElementLink< xAOD::CaloClusterContainer > >& links){

  m_caloClusters=links;
}


size_t egammaRec::getNumberOfTrackParticles() const{

  return m_trackParticles.size();
}
  
const xAOD::TrackParticle* egammaRec::trackParticle( size_t index ) const{

  if( index >= m_trackParticles.size() || !m_trackParticles.at(index).isValid() ) return 0;
  return (*(m_trackParticles.at(index)));
}
  
const ElementLink< xAOD::TrackParticleContainer> egammaRec::trackParticleElementLink( size_t index  ) const{

  if( index >= m_trackParticles.size() || !m_trackParticles.at(index).isValid() ) return ElementLink< xAOD::TrackParticleContainer>();
  return m_trackParticles.at(index);
}
  
void egammaRec::setTrackParticles( const std::vector< ElementLink< xAOD::TrackParticleContainer > >& links ){

  m_trackParticles = links;
}
  

size_t egammaRec::getNumberOfVertices() const{

  return m_vertices.size();
}
  
const xAOD::Vertex* egammaRec::vertex( size_t index ) const{

  if( index >= m_vertices.size() || !m_vertices.at(index).isValid() ) return 0;
  return (*(m_vertices.at(index)));

}
  
const ElementLink< xAOD::VertexContainer > egammaRec::vertexElementLink( size_t index  ) const{

  if( index >= m_vertices.size() || !m_vertices.at(index).isValid() )
    return ElementLink< xAOD::VertexContainer >();
  return m_vertices.at(index);

}
  
void egammaRec::setVertices( const std::vector< ElementLink< xAOD::VertexContainer > >& links ){

  m_vertices=links;
}


double   egammaRec::deltaEta (int index) const {

  return m_deltaEta.at(index);
}

double  egammaRec::deltaPhi (int index) const{

  return m_deltaPhi.at(index);
}

double  egammaRec::deltaPhiRescaled (int index) const{

  return m_deltaPhiRescaled.at(index);
}

void egammaRec::setDeltaEta (int sampl, double value){

  m_deltaEta.at(sampl)=value;
}

void egammaRec::setDeltaPhi (int sampl, double value){

  m_deltaPhi.at(sampl)=value;
}

void egammaRec::setDeltaPhiRescaled (int sampl,double value){

  m_deltaPhiRescaled.at(sampl)=value;
}


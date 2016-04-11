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

    April 2014:  Trk::VxCandidate is replaced by xAOD::Vertex
                 xAOD::IParticle is added to interface

********************************************/

#include "VxSecVertex/VxSecVKalVertexInfo.h"

namespace Trk {

  VxSecVKalVertexInfo::VxSecVKalVertexInfo() :
    VxSecVertexInfo(), 
    m_mass(0.),
    m_energyFraction(0.),
    m_energyTrkInJet(0.),
    m_dstToMatLayer(1000.),
    m_n2trackvertices(0),
    m_nBigImpTracks(0),
    m_pseudoVertex(0){}

  VxSecVKalVertexInfo::VxSecVKalVertexInfo(const std::vector<xAOD::Vertex*> & vertices,
					   double mass,double energyFraction,int n2trackvertices,double energyTrkInJet,
					   const std::vector<const Trk::TrackParticleBase*> & badTracks):
    VxSecVertexInfo(vertices),
    m_mass(mass),
    m_energyFraction(energyFraction),
    m_energyTrkInJet(energyTrkInJet),
    m_dstToMatLayer(1000.),
    m_n2trackvertices(n2trackvertices),
    m_nBigImpTracks(0),
    m_pseudoVertex(0)
  {
    std::vector<const Trk::TrackParticleBase*>::const_iterator badTracksBegin=badTracks.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator badTracksEnd=badTracks.end();

    for (std::vector<const Trk::TrackParticleBase*>::const_iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      ElementLink<Trk::TrackParticleBaseCollection> link;
      link.setElement(*badTracksIter);
      m_badTracksTP.push_back(link);
    }
  }


  VxSecVKalVertexInfo::VxSecVKalVertexInfo(const std::vector<xAOD::Vertex*> & vertices,
					   double mass,double energyFraction,int n2trackvertices,double energyTrkInJet,
					   const std::vector<const xAOD::IParticle*> & badTracks):
    VxSecVertexInfo(vertices),
    m_mass(mass),
    m_energyFraction(energyFraction),
    m_energyTrkInJet(energyTrkInJet),
    m_dstToMatLayer(1000.),
    m_n2trackvertices(n2trackvertices),
    m_nBigImpTracks(0),
    m_pseudoVertex(0)
  {
    std::vector<const xAOD::IParticle*>::const_iterator badTracksBegin=badTracks.begin();
    std::vector<const xAOD::IParticle*>::const_iterator badTracksEnd=badTracks.end();

    for (std::vector<const xAOD::IParticle*>::const_iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      ElementLink<xAOD::IParticleContainer> link;
      link.setElement(*badTracksIter);
      m_badTracksIP.push_back(link);
    }
    
  }

  VxSecVKalVertexInfo::VxSecVKalVertexInfo(const xAOD::Vertex* vertex,
					   double mass,double energyFraction,int nBigImpTrk,
					   const std::vector<const xAOD::IParticle*> & badTracks):
    VxSecVertexInfo(),
    m_mass(mass),
    m_energyFraction(energyFraction),
    m_energyTrkInJet(0.),
    m_dstToMatLayer(1000.),
    m_n2trackvertices(0),
    m_nBigImpTracks(nBigImpTrk),
    m_pseudoVertex(vertex)
  {
    std::vector<const xAOD::IParticle*>::const_iterator badTracksBegin=badTracks.begin();
    std::vector<const xAOD::IParticle*>::const_iterator badTracksEnd=badTracks.end();

    for (std::vector<const xAOD::IParticle*>::const_iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      ElementLink<xAOD::IParticleContainer> link;
      link.setElement(*badTracksIter);
      m_badTracksIP.push_back(link);
    }
    
  }

  VxSecVKalVertexInfo::VxSecVKalVertexInfo(const VxSecVKalVertexInfo & rhs):
    VxSecVertexInfo(rhs),
    m_mass(rhs.m_mass),
    m_energyFraction(rhs.m_energyFraction),
    m_energyTrkInJet(rhs.m_energyTrkInJet),
    m_dstToMatLayer(rhs.m_dstToMatLayer),
    m_n2trackvertices(rhs.m_n2trackvertices),
    m_nBigImpTracks(rhs.m_nBigImpTracks),
    m_badTracksTP(rhs.m_badTracksTP),
    m_badTracksIP(rhs.m_badTracksIP),
    m_pseudoVertex(rhs.m_pseudoVertex)
  {}

  VxSecVKalVertexInfo & VxSecVKalVertexInfo::operator= (const VxSecVKalVertexInfo & rhs) {
    
    if (this!=&rhs) {
      this->operator=(rhs);
      m_mass=rhs.m_mass;
      m_energyFraction=rhs.m_energyFraction;
      m_energyTrkInJet=rhs.m_energyTrkInJet;
      m_dstToMatLayer=rhs.m_dstToMatLayer,
      m_n2trackvertices=rhs.m_n2trackvertices;
      m_nBigImpTracks=rhs.m_nBigImpTracks;
      m_badTracksTP=rhs.m_badTracksTP;
      m_badTracksIP=rhs.m_badTracksIP;
      m_pseudoVertex=rhs.m_pseudoVertex;
    }
    return *this;
  }
  
  VxSecVKalVertexInfo::~VxSecVKalVertexInfo() { if(m_pseudoVertex)delete m_pseudoVertex;}

  const std::vector<const Trk::TrackParticleBase*> VxSecVKalVertexInfo::badTracksTP() const {

    std::vector<const Trk::TrackParticleBase*> vectorOfTP;
    
    std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::const_iterator badTracksBegin=m_badTracksTP.begin();
    std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::const_iterator badTracksEnd=m_badTracksTP.end();

    for (std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::const_iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      if ((*badTracksIter).isValid()) {
	if (**badTracksIter!=0) {
	  vectorOfTP.push_back(**badTracksIter);
	}
      }
    }
    
    return vectorOfTP;
  }

  const std::vector<const xAOD::IParticle*> VxSecVKalVertexInfo::badTracksIP() const {

    std::vector<const xAOD::IParticle*> vectorOfIP;
    
    std::vector<ElementLink<xAOD::IParticleContainer> >::const_iterator badTracksBegin=m_badTracksIP.begin();
    std::vector<ElementLink<xAOD::IParticleContainer> >::const_iterator badTracksEnd=m_badTracksIP.end();

    for (std::vector<ElementLink<xAOD::IParticleContainer> >::const_iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      if ((*badTracksIter).isValid()) {
	if (**badTracksIter!=0) {
	  vectorOfIP.push_back(**badTracksIter);
	}
      }
    }
    
    return vectorOfIP;
  }

  void VxSecVKalVertexInfo::setTrackParticleOrigin(const Trk::TrackParticleBaseCollection* trkpartTES) {


    std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::iterator badTracksBegin=m_badTracksTP.begin();
    std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::iterator badTracksEnd=m_badTracksTP.end();

    for (std::vector<ElementLink<Trk::TrackParticleBaseCollection> >::iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      (*badTracksIter).setStorableObject(*trkpartTES);
    }
  }

  void VxSecVKalVertexInfo::setIParticleOrigin(const xAOD::IParticleContainer* ipartTES) {


    std::vector<ElementLink<xAOD::IParticleContainer> >::iterator badTracksBegin=m_badTracksIP.begin();
    std::vector<ElementLink<xAOD::IParticleContainer> >::iterator badTracksEnd=m_badTracksIP.end();

    for (std::vector<ElementLink<xAOD::IParticleContainer> >::iterator badTracksIter=badTracksBegin;
	 badTracksIter!=badTracksEnd;++badTracksIter) {
      (*badTracksIter).setStorableObject(*ipartTES);
    }

  }
    

}//end namespace

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MVFVxTrackAtVertex.cxx  -  Description
                             -------------------
    begin   : July 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
    comments: 
    changes :

 ***************************************************************************/

#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"



namespace Trk {
  
  MVFVxTrackAtVertex::MVFVxTrackAtVertex() :
    VxTrackAtVertex(),
    m_linkToVertices(0) {}

    
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(double chi2PerTrk, TrackParameters * perigeeAtVertex) :
    VxTrackAtVertex(chi2PerTrk,perigeeAtVertex),
    m_linkToVertices(0) {}

  
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(double chi2PerTrk, TrackParameters * perigeeAtVertex, 
				   double ndfPerTrk, Trk::LinearizedTrack * linState):
    VxTrackAtVertex(chi2PerTrk,perigeeAtVertex,
		    ndfPerTrk,linState),
    m_linkToVertices(0) {}
  
  //new constructors, including initial perigee state (refitting support)
  //Kirill Prokofiev 27-03-06  
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(double chi2PerTrk, 
					 TrackParameters* perigeeAtVertex, 
					 TrackParameters* initialPerigee):
    VxTrackAtVertex(chi2PerTrk,perigeeAtVertex,initialPerigee),
    m_linkToVertices(0) {}
 
  
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(double chi2PerTrk, 
                                   TrackParameters* perigeeAtVertex,  
				   TrackParameters* initialPerigee,
		                   double ndfPerTrk,
				   Trk::LinearizedTrack * linState):
    VxTrackAtVertex(chi2PerTrk,perigeeAtVertex,
		    initialPerigee,ndfPerTrk,linState),
    m_linkToVertices(0) {}


  MVFVxTrackAtVertex::MVFVxTrackAtVertex(Trk::ITrackLink* trackOrParticleLink):
   VxTrackAtVertex(trackOrParticleLink),
   m_linkToVertices(0) {}
  
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(Trk::ITrackLink* trackOrParticleLink,
					 TrackToVtxLink* actuallink):
    VxTrackAtVertex(trackOrParticleLink),
    m_linkToVertices(actuallink) {}

  MVFVxTrackAtVertex::MVFVxTrackAtVertex(TrackToVtxLink* actuallink,
					 const Track* track,
					 const TrackCollection* trackTES):
    VxTrackAtVertex(), 
    m_linkToVertices(actuallink) {

    ElementLink<TrackCollection> link;
    link.setElement(const_cast<Trk::Track*>(track));
    LinkToTrack * linkTT = new LinkToTrack(link);
    linkTT->setStorableObject(*trackTES);
    this->setOrigTrack(linkTT);

  }

  MVFVxTrackAtVertex::MVFVxTrackAtVertex(TrackToVtxLink* actuallink,
					 const Trk::TrackParticleBase* trackparticle,
					 const Trk::TrackParticleBaseCollection* trkpartTES):
    VxTrackAtVertex(),
    m_linkToVertices(actuallink) {

    ElementLink<TrackParticleBaseCollection> link;
    link.setElement(const_cast<Trk::TrackParticleBase*>(trackparticle));
    LinkToTrackParticleBase * linkTT = new LinkToTrackParticleBase(link);
    linkTT->setStorableObject(*trkpartTES);
    this->setOrigTrack(linkTT);
  
  }



//destructor changed
  MVFVxTrackAtVertex::~MVFVxTrackAtVertex() 
  { 
    if (m_linkToVertices!=0) { m_linkToVertices=0; }//YOU DON'T OWN THE OBJECT: PLEASE PAY ATTENTION
  }

//copy constructor changed
  MVFVxTrackAtVertex::MVFVxTrackAtVertex(const MVFVxTrackAtVertex& rhs) :
    VxTrackAtVertex(rhs),
    m_linkToVertices(rhs.m_linkToVertices)
  {}


//assignement operator changed
  MVFVxTrackAtVertex & MVFVxTrackAtVertex::operator= (const MVFVxTrackAtVertex& rhs)
  {
    if (this!=&rhs)
    {
      this->operator=(rhs);
      m_linkToVertices = rhs.m_linkToVertices;//YOU DON'T OWN THE OBJECT!please pay attention here!!!
    }
    return *this;
  }

  void Trk::MVFVxTrackAtVertex::setLinkToVertices(TrackToVtxLink* link) {
    m_linkToVertices=link;
  }
  
    MsgStream& MVFVxTrackAtVertex::dump(MsgStream& sl) const {
    sl << "Printing Trk::MVFVxTrackAtVertex:" << endreq;
    // for now just print base class
    VxTrackAtVertex::dump(sl);
    return sl;
  }

  std::ostream& MVFVxTrackAtVertex::dump(std::ostream& sl) const {
    sl << "Printing Trk::MVFVxTrackAtVertex:" << std::endl;
    // for now just print base class
    VxTrackAtVertex::dump(sl);
    return sl;
  }


} // end of namespace

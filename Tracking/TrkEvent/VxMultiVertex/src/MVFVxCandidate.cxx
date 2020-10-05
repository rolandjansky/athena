/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                      MVFVxCandidate.cxx  -  Description
                             -------------------
    begin   : June 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
    changes : Kirill.Prokofiev@cern.ch
              EDM cleanup, switching to the FitQuality class use

 ***************************************************************************/

#include "VxMultiVertex/MVFVxCandidate.h"

namespace Trk {
  
  MVFVxCandidate::MVFVxCandidate() : 
    VxCandidate(),	
    m_fitInfo(Trk::MvfFitInfo(nullptr,nullptr,nullptr)),
    m_initialized(false)
    {}
  
  MVFVxCandidate::MVFVxCandidate(const Trk::RecVertex& recVertex,
				 const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex):
    VxCandidate(recVertex,vxTrackAtVertex),
    m_fitInfo(Trk::MvfFitInfo(nullptr,nullptr,nullptr)),
    m_initialized(true){}
  
  MVFVxCandidate::MVFVxCandidate(xAOD::Vertex* constraintVertex,
				 Amg::Vector3D* seedVertex,
				 Amg::Vector3D* linearizationVertex,
				 const Trk::RecVertex& recVertex,
				 std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex):
    VxCandidate(recVertex,vxTrackAtVertex),
    m_fitInfo(Trk::MvfFitInfo(constraintVertex,seedVertex,linearizationVertex)),
    m_initialized(true){}

  MVFVxCandidate::MVFVxCandidate(xAOD::Vertex* constraintVertex,
				 Amg::Vector3D* seedVertex,
				 Amg::Vector3D* linearizationVertex):
    VxCandidate(),
    m_fitInfo(Trk::MvfFitInfo(constraintVertex,seedVertex,linearizationVertex)),
    m_initialized(false){}



  MVFVxCandidate::MVFVxCandidate(const MVFVxCandidate& rhs) 
    = default;

  MVFVxCandidate& MVFVxCandidate::operator= (const MVFVxCandidate& rhs)
  {
    if (this!=&rhs)
      {
	this->operator=(rhs);
        m_fitInfo = rhs.m_fitInfo; 
	m_initialized = rhs.m_initialized;
      }
    return *this;
  }
  
  MVFVxCandidate::~MVFVxCandidate()
  = default;
  
  MsgStream& MVFVxCandidate::dump(MsgStream& sl) const {
    sl << "Printing Trk::MVFVxCandidate:" << endmsg;
    // for now just print base class
    VxCandidate::dump(sl);
    return sl;
  }

  std::ostream& MVFVxCandidate::dump(std::ostream& sl) const {
    sl << "Printing Trk::MVFVxCandidate:" << std::endl;
    // for now just print base class
    VxCandidate::dump(sl);
    return sl;
  }
  
}//end of namespace definitions
	

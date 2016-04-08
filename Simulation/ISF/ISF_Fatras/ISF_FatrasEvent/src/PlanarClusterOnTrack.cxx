/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarClusterOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <new>
#include "ISF_FatrasEvent/PlanarClusterOnTrack.h"
#include "ISF_FatrasEvent/PlanarCluster.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <typeinfo>

// Constructor with parameters 

namespace iFatras {

  // Constructor with parameters
  PlanarClusterOnTrack::PlanarClusterOnTrack(const PlanarCluster * RIO,
					     const Trk::LocalParameters& locpos,
					     const Amg::MatrixX& locerr,
					     const IdentifierHash& idDE) :
    Trk::RIO_OnTrack(locpos, locerr, RIO->identify()), //call base class constructor
    m_idDE(idDE),
    m_detEl( RIO->detectorElement() ),
    m_globalPosition(0)
  {
    m_rio.setElement(RIO);
  }
  
  PlanarClusterOnTrack::PlanarClusterOnTrack(const PlanarCluster * RIO,
					     const Trk::LocalParameters& locpos,
					     const Amg::MatrixX& locerr,
					     const IdentifierHash& idDE,
					     const Amg::Vector3D& globalPosition) :
    Trk::RIO_OnTrack(locpos, locerr, RIO->identify()), //call base class constructor
    m_idDE(idDE),
    m_detEl( RIO->detectorElement() ),
    m_globalPosition( new Amg::Vector3D(globalPosition) )
  {
    m_rio.setElement(RIO);
  }
  
  // Destructor:
  PlanarClusterOnTrack::~PlanarClusterOnTrack() {
    delete m_globalPosition; }

  // Default constructor:
  PlanarClusterOnTrack::PlanarClusterOnTrack() :
    Trk::RIO_OnTrack(),
    m_rio(),
    m_idDE(),    
    m_detEl(0),
    m_globalPosition()
  {}

  // copy constructor:
  PlanarClusterOnTrack::PlanarClusterOnTrack( const PlanarClusterOnTrack & rot) :
    RIO_OnTrack(rot),
    m_rio(rot.m_rio),
    m_idDE(rot.m_idDE),
    m_detEl(rot.m_detEl),
    m_globalPosition(rot.m_globalPosition ? new  Amg::Vector3D(*rot.m_globalPosition) : 0)
  {}

  // assignment operator:
  PlanarClusterOnTrack& PlanarClusterOnTrack::operator=( const PlanarClusterOnTrack& rot) {
    if ( &rot != this) {
      delete m_globalPosition;
      Trk::RIO_OnTrack::operator=(rot);
      m_rio            = rot.m_rio;
      m_idDE           = rot.m_idDE;
      m_detEl          = rot.m_detEl;
      m_globalPosition = rot.m_globalPosition ? new Amg::Vector3D(*rot.m_globalPosition) : 0;
    }
    return *this;
  }

  void PlanarClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* /*prd*/)
  {
    m_detEl = dynamic_cast< const iFatras::PlanarDetElement* >(detEl);
  }
   
  MsgStream& PlanarClusterOnTrack::dump( MsgStream& sl ) const
  {
    
    sl << "PlanarClusterOnTrack {" << endreq;
    Trk::RIO_OnTrack::dump(sl); 
    
    sl << "Global position (x,y,z) = (";
    if ( this->globalPositionPtr() )
      {
        sl  <<this->globalPosition().x()<<", "
	    <<this->globalPosition().y()<<", "
	    <<this->globalPosition().z()<<")"<<endreq;
      } else {
      sl<<"NULL!), "<<endreq;
    }
    sl<<"}"<<endreq;
    return sl;
  }
  
  std::ostream& PlanarClusterOnTrack::dump( std::ostream& sl ) const
  {
    sl << "PlanarClusterOnTrack {"<<std::endl;
    
    Trk::RIO_OnTrack::dump(sl); 
    
    sl << "Global position (x,y,z) = (";
    if ( this->globalPositionPtr() )
      {
	sl  <<this->globalPosition().x()<<", "
	    <<this->globalPosition().y()<<", "
	    <<this->globalPosition().z()<<")"<<std::endl;
      } else {
      sl<<"NULL!), "<<std::endl;
    }
    sl<<"}"<<std::endl;
    return sl;
  }
  
  
}

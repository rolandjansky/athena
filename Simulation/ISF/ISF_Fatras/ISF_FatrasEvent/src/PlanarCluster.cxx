/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarCluster.cxx
//   Implementation file for class PlanarCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Class header
#include "ISF_FatrasEvent/PlanarCluster.h"

#include "GaudiKernel/MsgStream.h"

namespace iFatras {

  // Constructor:
  PlanarCluster::PlanarCluster( const Identifier& RDOId,
				const Amg::Vector2D& locpos, 
				const std::vector<Identifier>& rdoList,
				const InDet::SiWidth& width,
				const iFatras::PlanarDetElement* detEl,
				const Amg::MatrixX* locErrMat ) :
    PrepRawData(RDOId, locpos, rdoList, locErrMat), //call base class constructor
    m_width(width),
    m_globalPosition(),
    m_detEl(detEl) {}
  
  PlanarCluster::PlanarCluster( const Identifier& RDOId,
				const Amg::Vector2D& locpos, 
				std::vector<Identifier>&& rdoList,
				const InDet::SiWidth& width,
				const iFatras::PlanarDetElement* detEl,
				std::unique_ptr<const Amg::MatrixX> locErrMat ) :
    PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat)), //call base class constructor
    m_width(width),
    m_globalPosition(),
    m_detEl(detEl) {}
  
  // Destructor:
  PlanarCluster::~PlanarCluster()
  {
    // do not delete m_detEl since owned by DetectorStore
  }

  // Default constructor:
  PlanarCluster::PlanarCluster():
    PrepRawData(),
    m_globalPosition(),
    m_detEl(0)
  {}

  //copy constructor:
  PlanarCluster::PlanarCluster( const PlanarCluster & RIO):
    PrepRawData( RIO ),
    m_width( RIO.m_width ),
    m_globalPosition(),
    m_detEl( RIO.m_detEl )
  {
    // copy only if it exists
    if (RIO.m_globalPosition) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalPosition));
  }

  //assignment operator
  PlanarCluster& PlanarCluster::operator=(const PlanarCluster& RIO){
    if (&RIO !=this) {
      static_cast<Trk::PrepRawData&>(*this) = RIO;
      m_width = RIO.m_width;
      if (RIO.m_globalPosition) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*RIO.m_globalPosition));
      else if (m_globalPosition) m_globalPosition.release().reset();
      m_detEl =  RIO.m_detEl ;
      }
    return *this;
  }

  //move operator
  PlanarCluster& PlanarCluster::operator=(PlanarCluster&& RIO){
    if (&RIO !=this) {
      static_cast<Trk::PrepRawData&>(*this) = std::move(RIO);
      m_globalPosition = std::move(RIO.m_globalPosition);
      m_width = RIO.m_width;
      m_detEl =  RIO.m_detEl ;
      }
    return *this;
  }

  MsgStream& PlanarCluster::dump( MsgStream&    stream) const {
    
    stream << "PlanarCluster object"<<std::endl;
    // have to do a lot of annoying checking to make sure that PRD is valid.
    if ( this->globalPositionPtr() )
      {
	stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
	       <<this->globalPosition().y()<<", "
	       <<this->globalPosition().z()<<")"<<std::endl;
      }
    stream << "SiWidth: " << m_width << std::endl;
    
    stream << "Base class (PrepRawData):" << std::endl;
    this->PrepRawData::dump(stream);
    
    return stream;
  }

  std::ostream& PlanarCluster::dump(std::ostream& stream) const {

    stream << "PlanarCluster object"<<std::endl;
    // have to do a lot of annoying checking to make sure that PRD is valid.
    this->globalPosition();
    if ( this->globalPositionPtr() )
      {
	stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
	       <<this->globalPosition().y()<<", "
	       <<this->globalPosition().z()<<")"<<std::endl;
      }
    stream << "SiWidth: " << m_width << std::endl;
    
    stream << "Base class (PrepRawData):" << std::endl;
    this->PrepRawData::dump(stream);
    
    return stream;
  }

  MsgStream&    operator << (MsgStream& stream,    const PlanarCluster& prd)
  {
    return prd.dump(stream);
  }
  
  std::ostream& operator << (std::ostream& stream, const PlanarCluster& prd)
  {
    return prd.dump(stream);
  }
  
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonCluster.cxx
//   Implementation file for class MuonCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 03/07/2007 Niels van Eldik
///////////////////////////////////////////////////////////////////

#include <new>
#include "MuonPrepRawData/MuonCluster.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

  // Constructor 
  MuonCluster::MuonCluster( const Identifier& RDOId,
                const IdentifierHash & /**collectionHash*/, //FIXME! Should be removed.
                const Amg::Vector2D& locpos,
                const std::vector<Identifier>& rdoList,
                const Amg::MatrixX* locErrMat
                ) :
      PrepRawData(RDOId, locpos, rdoList, locErrMat), //call base class constructor
      m_globalPosition(0)
  { 
  }

  // Destructor:
  MuonCluster::~MuonCluster()
  {
    delete m_globalPosition;
  }

  // Default constructor:
  MuonCluster::MuonCluster():
      PrepRawData(), 
      m_globalPosition(0)
  { }

  //copy constructor:
  MuonCluster::MuonCluster(const MuonCluster& RIO):
      PrepRawData(RIO)
  { 
    // copy only if it exists
    m_globalPosition = (RIO.m_globalPosition) ? new Amg::Vector3D(*RIO.m_globalPosition) : 0;
  }

  MuonCluster::MuonCluster(MuonCluster&& RIO):
    PrepRawData(std::move(RIO)),
    m_globalPosition (RIO.m_globalPosition)
  { 
    RIO.m_globalPosition = nullptr;
  }

  //assignment operator
  MuonCluster& MuonCluster::operator=(const MuonCluster& RIO)
  {
    if (&RIO !=this)
    {
      Trk::PrepRawData::operator=(RIO);
      delete m_globalPosition;
      m_globalPosition = (RIO.m_globalPosition) ? new Amg::Vector3D(*RIO.m_globalPosition) : 0;
    }
    return *this;
  }
  
  MuonCluster& MuonCluster::operator=(MuonCluster&& RIO)
  {
    if (&RIO !=this)
    {
      Trk::PrepRawData::operator=(std::move(RIO));
      delete m_globalPosition;
      m_globalPosition = RIO.m_globalPosition;
      RIO.m_globalPosition = nullptr;
    }
    return *this;
  }
  
  MsgStream& MuonCluster::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"MuonCluster {"<<std::endl;
    Trk::PrepRawData::dump(stream);
    stream << "Global Coordinates (x,y,z) = (";
    stream<<this->globalPosition().x()<<", "
          <<this->globalPosition().y()<<", "
          <<this->globalPosition().z()<<")"<<std::endl;
    stream<<"} End MuonCluster"<<endmsg;
    return stream;
  }

  std::ostream& MuonCluster::dump( std::ostream&    stream) const
  {
    stream << "MuonCluster {"<<std::endl;
    Trk::PrepRawData::dump(stream);   
    stream<<"} End MuonCluster"<<std::endl;
    return stream;
  }
  //end of classdef

}//end of ns

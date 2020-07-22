/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCluster.cxx
//   Implementation file for class SiCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/SiCluster.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet {

// Constructor for EF:
SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const std::vector<Identifier>& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     const Amg::MatrixX* locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, locErrMat)
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{

  m_globalPosition =
    m_detEl->surface(identify()).localToGlobalPos(localPosition());
}

SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     std::vector<Identifier>&& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     std::unique_ptr<const Amg::MatrixX> locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat))
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{
  m_globalPosition =
    m_detEl->surface(identify()).localToGlobalPos(localPosition());
}

// Destructor:
SiCluster::~SiCluster()
{
  // do not delete m_detEl since owned by DetectorStore
}


// copy constructor:
SiCluster::SiCluster(const SiCluster& RIO)
   

= default;

// move constructor:
SiCluster::SiCluster(SiCluster&& RIO)
  : PrepRawData(std::move(RIO))
  , m_globalPosition(std::move(RIO.m_globalPosition))
  , m_width(RIO.m_width)
  , m_detEl(RIO.m_detEl)
  , m_gangedPixel(RIO.m_gangedPixel)

{}

// assignment operator
SiCluster&
SiCluster::operator=(const SiCluster& RIO)
{
  if (&RIO != this) {
    Trk::PrepRawData::operator=(RIO);
    m_globalPosition = RIO.m_globalPosition;
    m_width = RIO.m_width;
    m_detEl = RIO.m_detEl;
    m_gangedPixel = RIO.m_gangedPixel;
  }
  return *this;
}

// move operator
SiCluster&
SiCluster::operator=(SiCluster&& RIO)
{
  if (&RIO != this) {
    Trk::PrepRawData::operator=(std::move(RIO));
    m_globalPosition = std::move(RIO.m_globalPosition);
    m_width = RIO.m_width;
    m_detEl = RIO.m_detEl;
    m_gangedPixel = RIO.m_gangedPixel;
  }
  return *this;
}

MsgStream&
SiCluster::dump(MsgStream& stream) const
{
  stream << "SiCluster object" << std::endl;

  // have to do a lot of annoying checking to make sure that PRD is valid.
  {
    stream << "at global coordinates (x,y,z) = (" << this->globalPosition().x()
           << ", " << this->globalPosition().y() << ", "
           << this->globalPosition().z() << ")" << std::endl;
  }

  if (gangedPixel()) {
    stream << "and is a ganged pixel. " << std::endl;
  } else {
    stream << "and is not a ganged pixel. " << std::endl;
  }

  stream << "SiWidth: " << m_width << std::endl;

  stream << "Base class (PrepRawData):" << std::endl;
  this->PrepRawData::dump(stream);

  return stream;
}

std::ostream&
SiCluster::dump(std::ostream& stream) const
{
  stream << "SiCluster object" << std::endl;
  {
    stream << "at global coordinates (x,y,z) = (" << this->globalPosition().x()
           << ", " << this->globalPosition().y() << ", "
           << this->globalPosition().z() << ")" << std::endl;
  }

  if (gangedPixel()) {
    stream << "and is a ganged pixel. " << std::endl;
  } else {
    stream << "and is not a ganged pixel. " << std::endl;
  }

  stream << "SiWidth: " << m_width << std::endl;

  stream << "Base Class (PrepRawData): " << std::endl;
  this->PrepRawData::dump(stream);

  return stream;
}

MsgStream&
operator<<(MsgStream& stream, const SiCluster& prd)
{
  return prd.dump(stream);
}

std::ostream&
operator<<(std::ostream& stream, const SiCluster& prd)
{
  return prd.dump(stream);
}

} // end of ns

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
#include <ostream>
#include <sstream>

namespace InDet {

// Constructor for EF:
SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const std::vector<Identifier>& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     const Amg::MatrixX& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, locErrMat)
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

// Constructor for EF:
SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const std::vector<Identifier>& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, {})
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const Amg::Vector3D& globpos,
                     const std::vector<Identifier>& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     const Amg::MatrixX& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, locErrMat)
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{}

SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const Amg::Vector3D& globpos,
                     const std::vector<Identifier>& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList,{})
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{}

SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     std::vector<Identifier>&& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     Amg::MatrixX&& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat))
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

SiCluster::SiCluster(const Identifier& RDOId,
                     const Amg::Vector2D& locpos,
                     const Amg::Vector3D& globpos,
                     std::vector<Identifier>&& rdoList,
                     const InDet::SiWidth& width,
                     const InDetDD::SiDetectorElement* detEl,
                     Amg::MatrixX&& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat))
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
  , m_gangedPixel(false)
{}

// Destructor:
SiCluster::~SiCluster()
{
  // do not delete m_detEl since owned by DetectorStore
}

MsgStream&
SiCluster::dump(MsgStream& stream) const
{
  std::ostringstream out;
  dump(out);
  stream<<out.str();
  return stream;
}

std::ostream&
SiCluster::dump(std::ostream& stream) const
{
  const std::string lf("\n");
  stream << "SiCluster object" << lf;
  {
    stream << "at global coordinates (x,y,z) = (" << this->globalPosition().x()
           << ", " << this->globalPosition().y() << ", "
           << this->globalPosition().z() << ")" << lf;
  }

  if (gangedPixel()) {
    stream << "and is a ganged pixel. " << lf;
  } else {
    stream << "and is not a ganged pixel. " << lf;
  }

  stream << "SiWidth: " << m_width << lf;

  stream << "Base Class (PrepRawData): " << lf;
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

} // end of nsk

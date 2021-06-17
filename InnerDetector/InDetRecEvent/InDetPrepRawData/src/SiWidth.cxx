/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiWidth.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/SiWidth.h"
#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <sstream>


// Implicit constructor:
InDet::SiWidth::SiWidth() :
  m_colrow(0.0, 0.0),
  m_phirzWidth(0.0, 0.0)
{}

// Copy constructor:
InDet::SiWidth::SiWidth(const InDet::SiWidth& width) :
  m_colrow(width.m_colrow),
  m_phirzWidth(width.m_phirzWidth)
{}

// Constructor with parameters:
InDet::SiWidth::SiWidth(const Amg::Vector2D& colrow, const  Amg::Vector2D& phiRZ) :
  m_colrow( colrow ),
  m_phirzWidth(phiRZ)
{}


// online constructor:
InDet::SiWidth::SiWidth(const Amg::Vector2D& colrow) :
  m_colrow(colrow),
  m_phirzWidth(0,0)
{}


// Destructor:
InDet::SiWidth::~SiWidth()
{
}

// Assignment operator:
InDet::SiWidth& InDet::SiWidth::operator=(const InDet::SiWidth &width)
{
  if (this!=&width) {
    m_colrow = width.m_colrow;
    m_phirzWidth = width.m_phirzWidth;
  } 
  return *this;
}

// addition of positions:


// scaling:

// Dump methods
MsgStream& InDet::SiWidth::dump( MsgStream&    stream) const
{
  std::ostringstream out;
  dump(out);
  stream<<out.str();
  return stream;
}
std::ostream& InDet::SiWidth::dump( std::ostream&    stream) const
{
  stream << "SiWidth object"<<std::endl;
  stream <<"Column / Row " << m_colrow.x() << " " << m_colrow.y() << std::endl;
  stream <<"phi RZ width " << m_phirzWidth.x() << " " << m_phirzWidth.y() << std::endl;
  return stream;
}

MsgStream&    InDet::operator << (MsgStream& stream,    const InDet::SiWidth& prd)
{
  return prd.dump(stream);
}

std::ostream& InDet::operator << (std::ostream& stream, const InDet::SiWidth& prd)
{
  return prd.dump(stream);
}


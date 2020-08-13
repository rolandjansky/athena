/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcPrepData.cxx
//   Implementation file for class TgcPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 03/29/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#include <new>
#include "MuonPrepRawData/TgcPrepData.h"
#include "GaudiKernel/MsgStream.h" 

namespace Muon
{

  TgcPrepData::TgcPrepData( const Identifier& RDOId,
			    const IdentifierHash &idDE,
			    const Amg::Vector2D& locpos,
			    const std::vector<Identifier>& rdoList,
			    const Amg::MatrixX* locErrMat,
			    const MuonGM::TgcReadoutElement* detEl,
          const uint16_t bcBitMap) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_bcBitMap(bcBitMap)
{ }

// Destructor:
TgcPrepData::~TgcPrepData()
{
}

// Default constructor:
TgcPrepData::TgcPrepData():
  MuonCluster(),
  m_detEl(0),
  m_bcBitMap(0)
{ }

//copy constructor:
TgcPrepData::TgcPrepData(const TgcPrepData& RIO):
  MuonCluster(RIO),
  m_detEl( RIO.m_detEl ),
  m_bcBitMap(RIO.m_bcBitMap)
{ }

//assignment operator
TgcPrepData& TgcPrepData::operator=(const TgcPrepData& RIO)
{
    if (&RIO !=this)
    {
      MuonCluster::operator=(RIO);
      m_detEl =  RIO.m_detEl ;
      m_bcBitMap = RIO.m_bcBitMap;
    }
    return *this;
}

uint16_t TgcPrepData::getBcBitMap() const
{
  return m_bcBitMap;
}

void TgcPrepData::setBcBitMap(const uint16_t bcBitMap)
{
  m_bcBitMap = bcBitMap;
}

MsgStream& TgcPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"TgcPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);

    stream<<"}  End TgcPrepData"<<endmsg;

    return stream;
  }

  std::ostream& TgcPrepData::dump( std::ostream&    stream) const
  {
    stream << "TgcPrepData {"<<std::endl;

    MuonCluster::dump(stream);

    stream<<"} End TgcPrepData"<<std::endl;
    return stream;
  }
//end of classdef
}//end of ns


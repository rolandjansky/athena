/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcCoinData.cxx
//   Implementation file for class TgcCoinData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <new>
#include "MuonTrigCoinData/RpcCoinData.h"
#include "GaudiKernel/MsgStream.h" 

namespace Muon
{
  // Constructor 
  RpcCoinData::RpcCoinData(  const Identifier& stripId,
			     const IdentifierHash &idDE,
			     const Amg::Vector2D& locpos,
			     const std::vector<Identifier>& stripList,
			     const Amg::MatrixX& locErrMat,
			     const MuonGM::RpcReadoutElement* detEl,
			     const float time,
			     const unsigned short ambiguityFlag,
			     const unsigned short ijk,
			     const unsigned short threshold,
			     const unsigned short overlap,
			     const unsigned short parent_cmId,
			     const unsigned short parent_padId,
			     const unsigned short parent_sectorId,
                             bool lowPtCm):
    RpcPrepData(stripId, idDE, locpos, stripList, locErrMat, detEl, time, ambiguityFlag), 
    m_ijk(ijk),
    m_threshold(threshold),
    m_overlap(overlap),
    m_parentCmId(parent_cmId),
    m_parentPadId(parent_padId), 
    m_parentSectorId(parent_sectorId),
    m_lowPtCm(lowPtCm)
{ }

// Destructor:
RpcCoinData::~RpcCoinData()
{
}

// Default constructor:
RpcCoinData::RpcCoinData():
    RpcPrepData( ), 
    m_ijk(0),
    m_threshold(99),
    m_overlap(99),
    m_parentCmId(0),
    m_parentPadId(0), 
    m_parentSectorId(0),
    m_lowPtCm(false)
{ }

//copy constructor:
RpcCoinData::RpcCoinData(const RpcCoinData& RIO):
    RpcPrepData(RIO), 
    m_ijk( RIO.m_ijk ),
    m_threshold( RIO.m_threshold ),
    m_overlap( RIO.m_overlap ),
    m_parentCmId( RIO.m_parentCmId ),
    m_parentPadId( RIO.m_parentPadId ), 
    m_parentSectorId( RIO.m_parentSectorId ),
    m_lowPtCm( RIO.m_lowPtCm )
{ }

//move constructor:
RpcCoinData::RpcCoinData(RpcCoinData&& RIO):
    RpcPrepData(std::move(RIO)), 
    m_ijk( RIO.m_ijk ),
    m_threshold( RIO.m_threshold ),
    m_overlap( RIO.m_overlap ),
    m_parentCmId( RIO.m_parentCmId ),
    m_parentPadId( RIO.m_parentPadId ), 
    m_parentSectorId( RIO.m_parentSectorId ),
    m_lowPtCm( RIO.m_lowPtCm )
{ }

//assignment operator
RpcCoinData& RpcCoinData::operator=(const RpcCoinData& RIO)
{
  if(&RIO !=this)
    {
      RpcPrepData::operator=(RIO);
      m_ijk            = RIO.m_ijk;
      m_threshold      = RIO.m_threshold;
      m_overlap        = RIO.m_overlap;
      m_parentCmId     = RIO.m_parentCmId;
      m_parentPadId    = RIO.m_parentPadId; 
      m_parentSectorId = RIO.m_parentSectorId;
      m_lowPtCm        = RIO.m_lowPtCm;
    }
  return *this;
}

//move operator
RpcCoinData& RpcCoinData::operator=(RpcCoinData&& RIO)
{
  if(&RIO !=this)
    {
      m_ijk            = RIO.m_ijk;
      m_threshold      = RIO.m_threshold;
      m_overlap        = RIO.m_overlap;
      m_parentCmId     = RIO.m_parentCmId;
      m_parentPadId    = RIO.m_parentPadId; 
      m_parentSectorId = RIO.m_parentSectorId;
      m_lowPtCm        = RIO.m_lowPtCm;
      RpcPrepData::operator=(std::move(RIO));
    }
  return *this;
}

// << operator

MsgStream& RpcCoinData::dump( MsgStream&    stream) const
{
  stream << MSG::INFO<<"RpcCoinData {"<<std::endl;
    
  RpcPrepData::dump(stream);
  
  stream<<"ijk                  = "<<this->ijk()<<", ";
  stream<<"threshold            = "<<this->threshold()<<", ";
  stream<<"overlap              = "<<this->overlap()<<", ";
  stream<<"parentCmId           = "<<this->parentCmId()<<", ";
  stream<<"parentPadId          = "<<this->parentPadId()<<", ";
  stream<<"parentSectorId       = "<<this->parentSectorId()<<", ";
  stream<<"lowPtCm              = "<<this->isLowPtCoin()<<", ";
  stream<<"lowPtInputToHighPtCm = "<<this->isLowPtInputToHighPtCm()<<", ";
  stream<<"}"<<endmsg;
  
  return stream;
}

std::ostream& RpcCoinData::dump( std::ostream&    stream) const
{
  stream <<"RpcCoinData {"<<std::endl;
    
  RpcPrepData::dump(stream);
  
  stream<<"ijk                  = "<<this->ijk()<<", ";
  stream<<"threshold            = "<<this->threshold()<<", ";
  stream<<"overlap              = "<<this->overlap()<<", ";
  stream<<"parentCmId           = "<<this->parentCmId()<<", ";
  stream<<"parentPadId          = "<<this->parentPadId()<<", ";
  stream<<"parentSectorId       = "<<this->parentSectorId()<<", ";
  stream<<"lowPtCm              = "<<this->isLowPtCoin()<<", ";
  stream<<"lowPtInputToHighPtCm = "<<this->isLowPtInputToHighPtCm()<<", ";
  stream<<"}"<<std::endl;

  return stream;
}
bool RpcCoinData::isLowPtCoin() const
{
    if ( m_lowPtCm && m_ijk == 6 ) return true;
    return false;
}
bool RpcCoinData::isHighPtCoin() const
{
    if ( (!m_lowPtCm) && m_ijk == 6 ) return true;
    return false;
}
bool RpcCoinData::isLowPtInputToHighPtCm() const
{
    if ( m_ijk == 0 ) return true;
    return false;
}


}//end of namespace


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/sTgcPrepData.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

  sTgcPrepData::sTgcPrepData( const Identifier& RDOId,
			      const IdentifierHash &idDE,
			      const Amg::Vector2D& locpos,
			      const std::vector<Identifier>& rdoList,
			      const Amg::MatrixX* locErrMat,
			      const MuonGM::sTgcReadoutElement* detEl,
                              const int charge,
                              const int time, 
			      const uint16_t bcBitMap ) :
    MuonCluster(RDOId, idDE, locpos, rdoList, locErrMat), //call base class constructor
    m_detEl(detEl),
    m_charge(charge),
    m_time(time),
    m_bcBitMap(bcBitMap)
  { }


  // Destructor:
  sTgcPrepData::~sTgcPrepData()
  {

  }

  // Default constructor:
  sTgcPrepData::sTgcPrepData():
    MuonCluster(),
    m_detEl(0),
    m_charge(0),
    m_time(0),
    m_bcBitMap(0)
  { }

  //copy constructor:
  sTgcPrepData::sTgcPrepData(const sTgcPrepData& RIO):
    MuonCluster(RIO),
    m_detEl( RIO.m_detEl ),
    m_charge( RIO.m_charge ),
    m_time(RIO.m_time),
    m_bcBitMap( RIO.m_bcBitMap )
  { }

  //move constructor:
  sTgcPrepData::sTgcPrepData(sTgcPrepData&& RIO):
    MuonCluster(std::move(RIO)),
    m_detEl( RIO.m_detEl ),
    m_charge( RIO.m_charge ),
    m_time(RIO.m_time),
    m_bcBitMap( RIO.m_bcBitMap )
  { }

  //assignment operator
  sTgcPrepData&
  sTgcPrepData::operator=(const sTgcPrepData& RIO)
  {
    if (&RIO !=this)
      {
	MuonCluster::operator=(RIO);
	m_detEl =  RIO.m_detEl ;
        m_charge = RIO.m_charge;
        m_time = RIO.m_time;
	m_bcBitMap = RIO.m_bcBitMap;
      }
    return *this;

  }

  //move operator
  sTgcPrepData&
  sTgcPrepData::operator=(sTgcPrepData&& RIO)
  {
    if (&RIO !=this)
      {
	MuonCluster::operator=(std::move(RIO));
	m_detEl =  RIO.m_detEl ;
        m_charge = RIO.m_charge;
        m_time = RIO.m_time;
	m_bcBitMap = RIO.m_bcBitMap;
      }
    return *this;

  }

  MsgStream&
  sTgcPrepData::dump( MsgStream&    stream) const
  {
    stream << MSG::INFO<<"sTgcPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);
 
    stream<<"}"<<endmsg;

    return stream;
  }

  std::ostream&
  sTgcPrepData::dump( std::ostream&    stream) const
  {
    stream << "sTgcPrepData {"<<std::endl;
    
    MuonCluster::dump(stream);

    stream<<"}"<<std::endl;

    return stream;
  }
  //end of classdef
}//end of ns


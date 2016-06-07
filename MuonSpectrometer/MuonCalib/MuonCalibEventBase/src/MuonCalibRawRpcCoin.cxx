/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"

#include <iostream>

namespace MuonCalib {

  MuonCalibRawRpcCoin::MuonCalibRawRpcCoin() : MuonCalibRawRpcHit(),
					       m_ijk(0),m_threshold(0),m_overlap(0),m_parentCmId(0),
					       m_parentPadId(0),m_parentSectorId(0),m_lowPtCm(false)
  {} //!< default constructor

  MuonCalibRawRpcCoin::MuonCalibRawRpcCoin( const MuonCalibRawRpcHit &rpcHit, 
					    unsigned short ijk,
					    unsigned short threshold,
					    unsigned short overlap,
					    unsigned short parentCmId,
					    unsigned short parentPadId,
					    unsigned short parentSectorId,
					    bool lowPtCm ) :
    MuonCalibRawRpcHit(rpcHit),
    m_ijk(ijk),m_threshold(threshold),m_overlap(overlap),m_parentCmId(parentCmId),
    m_parentPadId(parentPadId),m_parentSectorId(parentSectorId),m_lowPtCm(lowPtCm)
  {}

  MuonCalibRawRpcCoin::~MuonCalibRawRpcCoin() {} //!< destructor

  std::ostream& MuonCalibRawRpcCoin::dump( std::ostream &stream ) const {
    stream << "MuonCalibRawRpcCoin with" <<  std::endl;
    stream << "   identifier " << identify() << std::endl;
    stream << "   global position " << globalPosition() << std::endl;
    stream << "   occupancy " << occupancy() << std::endl;
    stream << "   time " << t() << std::endl;
    stream << "   width " << width() << std::endl;
    stream << "   length " << length() << std::endl;
    return stream;
  }

}  //namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MuonCalibRawRpcCoin &hit) {
  return hit.dump(stream);
}

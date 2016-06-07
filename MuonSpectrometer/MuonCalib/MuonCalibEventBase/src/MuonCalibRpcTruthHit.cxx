/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"

namespace MuonCalib{
  
  MuonCalibRpcTruthHit::MuonCalibRpcTruthHit()
    : m_ID(0), m_barCode( 0 ), m_time( 0. )
  {}

  MuonCalibRpcTruthHit::MuonCalibRpcTruthHit( MuonFixedId id, int  barCode, double time )
    : m_ID(id), m_barCode( barCode ), m_time( time )
  {}
     
  MuonCalibRpcTruthHit::MuonCalibRpcTruthHit( const MuonCalibRpcTruthHit &truth )
    : m_ID(truth.identify()),  m_barCode( truth.barCode() ), m_time( truth.time() ) 
  {}
  
  MuonCalibRpcTruthHit& MuonCalibRpcTruthHit::operator=( const MuonCalibRpcTruthHit &rhs ) {
    if(this!=&rhs) {
      m_ID = rhs.identify();
      m_barCode = rhs.barCode() ;
      m_time = rhs.time() ;
    }
    return (*this) ;
  }

  std::ostream& MuonCalibRpcTruthHit::dump( std::ostream &stream ) const{
    stream << "MuonCalibRpcTruthHit: " << std::endl;
    stream << "                 ID = " << identify() << std::endl;
    stream << "            barCode = " << barCode() << std::endl;
    stream << "        Time = " << time() << std::endl;
    return stream;
  }

}// end namespace MuonCalib

std::ostream& operator << (std::ostream &stream, const MuonCalib::MuonCalibRpcTruthHit &truth) {
  return truth.dump( stream );
}

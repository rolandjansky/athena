/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"

namespace MuonCalib{
  
  MuonCalibMdtTruthHit::MuonCalibMdtTruthHit()
    : m_ID(0), m_barCode( 0 ), m_driftRadius( 0. ), m_positionAlongTube( 0. )
  {  }

  MuonCalibMdtTruthHit::MuonCalibMdtTruthHit( MuonFixedId id, int  barCode, double driftRadius, double positionAlongTube )
    : m_ID(id), m_barCode( barCode ), m_driftRadius( driftRadius ), m_positionAlongTube( positionAlongTube )
  {  }
     
  MuonCalibMdtTruthHit::MuonCalibMdtTruthHit( const MuonCalibMdtTruthHit& truth )
    : m_ID(truth.identify()),  m_barCode( truth.barCode() ), m_driftRadius( truth.driftRadius() ), m_positionAlongTube( truth.positionAlongTube() )
  {  }
  
  
  MuonCalibMdtTruthHit& MuonCalibMdtTruthHit::operator=( const MuonCalibMdtTruthHit& rhs ){
     if(this!=&rhs) {
       m_ID = rhs.identify();
       m_barCode = rhs.barCode() ;
       m_driftRadius = rhs.driftRadius() ;
       m_positionAlongTube = rhs.positionAlongTube() ; 
     }
     return (*this) ;
  }

  std::ostream& MuonCalibMdtTruthHit::dump( std::ostream& stream ) const{
    stream << "MuonCalibMdtTruthHit: " << std::endl;
    stream << "                 ID = " << identify() << std::endl;
    stream << "            barCode = " << barCode() << std::endl;
    stream << "        driftRadius = " << driftRadius() << std::endl;
    stream << "  positionAlongTube = " << positionAlongTube() << std::endl;
    return stream;
  }

}// end namespace MuonCalib


std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibMdtTruthHit& truth){
  return truth.dump( stream ) ;
}

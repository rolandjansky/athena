/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibHole_E.h"

namespace MuonCalib{
  MuonCalibHole_E::MuonCalibHole_E() : m_id(0), m_pos(0.,0.,0.) {
  }

  MuonCalibHole_E::MuonCalibHole_E( const MuonFixedId& id, const Amg::Vector3D& pos ) : m_id(id), m_pos(pos) {
  }

  
  MuonCalibHole_E::MuonCalibHole_E( const MuonCalibHole_E& hole ) {
    m_id = hole.identify();
    m_pos = hole.position();
  } 
  
  MuonCalibHole_E&  MuonCalibHole_E::operator=( const MuonCalibHole_E& rhs ) {
    if(this!=&rhs){
      m_id = rhs.identify() ;
      m_pos = rhs.position() ;
    }
    return (*this);
  } 

}//namespace MuonCalib

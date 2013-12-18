/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"

namespace MuonCalib{
  MuonCalibCaloHit::MuonCalibCaloHit() : m_id(0), m_pos(0.,0.,0.), m_time(0.) , m_charge(0.) {
  }

  MuonCalibCaloHit::MuonCalibCaloHit( const int& id, const Amg::Vector3D& pos, double time, double charge) : 
    m_id(id), m_pos(pos), m_time(time), m_charge(charge) {
  }

}//namespace MuonCalib

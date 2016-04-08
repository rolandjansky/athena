/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

namespace MuonCalib{
  MuonCalibHit_E::MuonCalibHit_E()
    : m_id(0), m_pos(0.,0.,0.), m_driftRadius(0.) , m_error(0.), m_resi(-9999.), m_pull(-9999.), m_measType(-9999) {
  }

  MuonCalibHit_E::MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos )
    : m_id(id), m_pos(pos), m_driftRadius(0.), m_error(0.), m_resi(-9999.), m_pull(-9999.), m_measType(-9999) {
  }
  

  MuonCalibHit_E::MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos, double driftRadius, double error)
    : m_id(id), m_pos(pos), m_driftRadius(driftRadius), m_error(error), m_resi(-9999.), m_pull(-9999.), m_measType(-9999) {
  }

  MuonCalibHit_E::MuonCalibHit_E( const MuonFixedId& id, const Amg::Vector3D& pos, double driftRadius, double error,
				  double res, double pull, int type) 
    : m_id(id), m_pos(pos), m_driftRadius(driftRadius), m_error(error), m_resi(res), m_pull(pull), m_measType(type) {
  }
  

}//namespace MuonCalib

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Pattern Recognition Event Data Model
 * -----------------------------------------
 *
 * Author       : Niels van Eldik
 * Creation Date: 27 July 2005
 * Last Update  : 27 July 2005
 ***************************************************************************/


#include "MuonPattern/MuonPattern.h"

namespace Muon {

  MuonPattern::MuonPattern( const Amg::Vector3D& pos, const Amg::Vector3D& dir ) : 
    m_globalPosition(pos), m_globalDirection(dir) 
  {

  }

  MuonPattern::~MuonPattern() 
  {
  
  }

}

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


#include "MuonPattern/MuonPrdPattern.h"

namespace Muon {

  MuonPrdPattern::MuonPrdPattern( const Amg::Vector3D& pos, const Amg::Vector3D& dir ) : 
    MuonPattern(pos,dir) 
  {

  } 

  MuonPrdPattern::MuonPrdPattern( const Amg::Vector3D& pos, const Amg::Vector3D& dir, const PrdVector& prds ) : 
    MuonPattern(pos,dir) , m_prds(prds)
  {

  }

  MuonPrdPattern::~MuonPrdPattern()
  {

  }
}

 

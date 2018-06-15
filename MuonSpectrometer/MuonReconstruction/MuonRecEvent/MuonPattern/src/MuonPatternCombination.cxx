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


#include "MuonPattern/MuonPatternCombination.h"

namespace Muon {



  MuonPatternCombination::MuonPatternCombination( const Trk::TrackParameters* parameter, 
						  const std::vector< MuonPatternChamberIntersect >& chamberData ) :
    m_parameter( parameter ),
    m_chamberData( chamberData ),
    m_roadType(0)
  {

  }
  
  MuonPatternCombination::MuonPatternCombination( const MuonPatternCombination& pat ) :
    m_parameter( pat.m_parameter ? pat.m_parameter->clone() : 0 ),
    m_chamberData( pat.m_chamberData ),
    m_roadType(0)
  {
    
  }

  MuonPatternCombination&  MuonPatternCombination::operator=( const MuonPatternCombination& pat )
  {
    if (this!=&pat ){
      // delete old MuonPatterns
      delete m_parameter;
      m_parameter = pat.m_parameter ? pat.m_parameter->clone() : 0;
      m_chamberData = pat.m_chamberData;
    }
    return *this;
  }

  MuonPatternCombination::~MuonPatternCombination(){
    delete m_parameter;
  }



}

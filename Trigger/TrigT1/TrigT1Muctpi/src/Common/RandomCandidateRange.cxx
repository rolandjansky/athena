/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RandomCandidateRange.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "RandomCandidateRange.h"
#include "SectorConstants.h"

/*******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
 *
 * Implementation of class RandomCandidateRange
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

namespace LVL1MUCTPI {

  // constructor of class RandomCandidateRange
  RandomCandidateRange::RandomCandidateRange()
    : m_allowSystems( 7 ), m_allowHemispheres( 3 ), m_minBarrelSectorID( MIN_BARREL_SECTOR ), m_maxBarrelSectorID( MAX_BARREL_SECTOR ),
      m_minEndcapSectorID( MIN_ENDCAP_SECTOR ), m_maxEndcapSectorID( MAX_ENDCAP_SECTOR ), m_minForwardSectorID( MIN_FORWARD_SECTOR ),
      m_maxForwardSectorID( MAX_FORWARD_SECTOR ), m_allowForce2Cand( 1 ), m_minBCID( 0 ), m_maxBCID( 0 ),
      m_minBarrelRoI( MIN_BARREL_ROI ), m_maxBarrelRoI( MAX_BARREL_ROI ), m_minEndcapRoI( MIN_ENDCAP_ROI ), m_maxEndcapRoI( MAX_ENDCAP_ROI ),
      m_minForwardRoI( MIN_FORWARD_ROI ), m_maxForwardRoI( MAX_FORWARD_ROI ),
      m_minPt( 1 ), m_maxPt( MAX_NUMBER_OF_THRESHOLDS ), m_allowForceGT1InPad1Flag( 1 ), m_allowForceGT1InPad2Flag( 1 ),
      m_allowForceGT2InSecFlag( 1 ) {

  }

  //destructor of class RandomCandidateRange
  RandomCandidateRange::~RandomCandidateRange(){

  }

} // namespace LVL1MUCTPI

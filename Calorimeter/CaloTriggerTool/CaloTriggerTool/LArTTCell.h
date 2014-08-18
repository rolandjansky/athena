/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_LARTTCELL_H
#define CALOTRIGGERTOOL_LARTTCELL_H

#include <vector>

// F. Ledroit, Sept 29, 2006 
// class created following LArOnOffId example by Hong Ma

// TRIGPN TRIGSAMPL TRIGREG TRIGETA TRIGPHI LAYER DETID PN SAMPL REGION ETA PHI
/**
   @brief Structure definition of the LArTTCellMap
 */
struct LArTTCell_t
{
          short tpn;
 unsigned short tsample ;
 unsigned short tregion ;
 unsigned short teta ;
 unsigned short tphi ;
 unsigned short layer ;

 unsigned short det;
          short pn;
 unsigned short sample;
 unsigned short region;
 unsigned short eta;
 unsigned short phi;

} ;

typedef std::vector<LArTTCell_t> LArTTCell; 

#endif 

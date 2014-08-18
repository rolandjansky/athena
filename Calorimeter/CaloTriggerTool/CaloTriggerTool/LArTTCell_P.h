/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_LARTTCELL_P_H
#define CALOTRIGGERTOOL_LARTTCELL_P_H

#include <vector>
#include "CaloTriggerTool/LArTTCell.h"

// F. Ledroit, Sept 29, 2006 
// class created following LArOnOffId_P example by Hong Ma

/**
   @brief This is a database object holding the offline TT-cell map

   It is the persitifiable version of the LArTTCellMap class

   @author Hong Ma
   @author maintained by F. Ledroit
 */
class LArTTCell_P 
{
public:

  /** Structure of the map */
 struct LArTTCell_P_t
 {
          short tpn;
 unsigned short tsample ;
 unsigned short tregion ;
 unsigned short teta ;
 unsigned short tphi ;
 unsigned short layer ;

   // det=0,1,2 for EM/HEC/FCAL
   //     9,8,7 for EM/HEC/FCAL disconnected.

 unsigned short det;
          short pn;
 unsigned short sample;
 unsigned short region;
 unsigned short eta;
 unsigned short phi;

  } ;
 typedef LArTTCell_P_t __t; 

 LArTTCell_P (): m_version(0)  { } 

 virtual ~LArTTCell_P() { } 


 std::vector<__t>  m_v; 

 int m_version ; 
}; 

#endif 

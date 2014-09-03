/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcSectorLogicContainer_p1.h

#ifndef MUONEVENTATHENAPOOL_RPCSECTORLOGICCONTAINER_P2_H
#define MUONEVENTATHENAPOOL_RPCSECTORLOGICCONTAINER_P2_H

/** 
	Persistent version of RpcPadContainer
@author edward.moyse@cern.ch
*/ 

#include <vector>

/** Class to represent 
*/
class RpcSectorLogicContainer_p1  {
public:  
    RpcSectorLogicContainer_p1() {}; 
    
    uint32_t m_sectorList[2]; //!< From RpcSectorLogicContainer. Dataword to contain the list of already filled sectors.
      
   /**This contains the bulk of the information from the various SL objects.
   
   The first number is the number of SectorLogics stored (to optimise reading)
   
   From SectorLogic directly (which is a DataVector<RpcSLTriggerHit>):
  - m_sectorId,
  - m_fel1Id,
  - m_bcid,
  - m_errorCode,
  - m_crc 
  - size of m_counters vector, then the m_counters variables
  - size of m_triggerRates (stored separately in m_triggerRates)
  
   Then we move onto the RpcSLTriggerHit information:
   - number of TriggerHit objects 
   and then for each:
   - m_rowinBcid
   - m_padid
   - m_ptid
   - m_roi
   - m_outerPlane
   - m_overlapPhi
   - m_overlapEta
   - m_triggerBcid
   - m_isInput
   */
  std::vector<uint16_t> m_slVariables;
  /** This is just one per SL*/
  std::vector<bool> m_hasMoreThan2TriggerCand;
  /** Number of m_triggerRates per SL is defined in m_slVariables*/
  std::vector<double> m_triggerRates;
};
#endif



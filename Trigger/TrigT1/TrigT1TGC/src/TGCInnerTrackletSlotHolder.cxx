/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCInnerTrackletSlotHolder.h"

namespace LVL1TGCTrigger {
  /** Constructor */
  TGCInnerTrackletSlotHolder::TGCInnerTrackletSlotHolder( TGCArguments* tgcargs):
    m_tgcArgs(tgcargs)
  {
    for(int sideId=0; sideId<NUMBER_OF_SIDES; sideId++) { // Side A and Side C
      for(int slotId=FIRST_SLOT; slotId<NUMBER_OF_SLOTS+FIRST_SLOT; slotId++) { // Slot01 - Slot24
	m_innerTrackletSlot[sideId][slotId-FIRST_SLOT].setSideId(sideId);
	// There is offset of 1. Slot01 is m_innerTrackletSlot[sideId][0]. 
	m_innerTrackletSlot[sideId][slotId-FIRST_SLOT].setSlotId(slotId); 
	m_innerTrackletSlot[sideId][slotId-FIRST_SLOT].setTgcArguments( tgcArgs() );
      }
    }
  }

  void TGCInnerTrackletSlotHolder::clearTriggerBits() {
    for(int sideId=0; sideId<NUMBER_OF_SIDES; sideId++) { // Side A and Side C
      for(int slotId=FIRST_SLOT; slotId<NUMBER_OF_SLOTS+FIRST_SLOT; slotId++) { // Slot01 - Slot24
	m_innerTrackletSlot[sideId][slotId-FIRST_SLOT].clearTriggerBits();
      }
    }
  }

  bool TGCInnerTrackletSlotHolder::setTriggerBit(const int sideId, const int slotId, 
						 const unsigned int region, const unsigned int readout, 
						 const unsigned int iBit, const bool triggerBit) {
    if(sideId<0 || sideId>=NUMBER_OF_SIDES) return false;
    if(slotId<FIRST_SLOT || slotId>=NUMBER_OF_SLOTS+FIRST_SLOT) return false;
    return m_innerTrackletSlot[sideId][slotId-FIRST_SLOT].setTriggerBit(region, readout, iBit, triggerBit);
  }

  const TGCInnerTrackletSlot* TGCInnerTrackletSlotHolder::getInnerTrackletSlot(const int sideId, const int slotId) const {
    if(sideId<0 || sideId>=NUMBER_OF_SIDES) return 0;
    if(slotId<FIRST_SLOT || slotId>=NUMBER_OF_SLOTS+FIRST_SLOT) return 0;
    return &m_innerTrackletSlot[sideId][slotId-FIRST_SLOT];
  }

  void TGCInnerTrackletSlotHolder::getInnerTrackletSlots(const int sideId, const int octId, const int moduleId,  
							 const TGCInnerTrackletSlot* innerTrackletSlots[]) const {
    // Initialize innerTrackletSlot array
    for(unsigned int iSlot=0; iSlot<NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) {
      innerTrackletSlots[iSlot] = 0;
    }
    if(sideId<0 || sideId>=NUMBER_OF_SIDES) return; // Side A or Side C
    if(octId<0 || octId>=8) return; // octId is 0-8. 
    // OnlineID moduleNumber
    //       <---- phi ----
    // EC:   7 6 4 3 1 0   11 10  9
    // FWD:   8   5   2    14 13 12
    //       [M1, M2, M3]  [EI/FI]
    if(!(moduleId==0 || moduleId==1 || 
	 moduleId==3 || moduleId==4 || 
	 moduleId==6 || moduleId==7)) return; // moduleId is 0,1,3,4,6,7 for Endcap. 
    int secId = (moduleId/3)*2+moduleId%3; // secId=0-5(EC)
    int phi = (secId+46+octId*6)%48+1; // phi=1-48(EC), 1 is just above +x-axis and 48 is just below +x-axis
    // slot23, slot24, slot01, slot02 go to A01 phi0/phi1 (phi=47/phi=48) 
    // slot24, slot01, slot02, slot03 go to A01 phi2/phi3 (phi= 1/phi= 2) 
    // slot01, slot02, slot03, slot04 go to A02 phi0/phi1 (phi= 3/phi= 4) 
    // ... ...
    for(unsigned int iSlot=0; iSlot<NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR; iSlot++) { 
      int slotId = (static_cast<int>(iSlot) + (phi-1)/2 + 23)%24 + 1;
      // phi (phi-1)/2 (static_cast<int>(iSlot) + (phi-1)/2 + 23)%24 + 1
      //               iSlot=0        iSlot=1        iSlot=2        iSlot=3
      //  47        23 (0+23+23)%24+1 (1+23+23)%24+1 (2+23+23)%24+1 (3+23+23)%24+1
      //  48        23 (0+23+23)%24+1 (1+23+23)%24+1 (2+23+23)%24+1 (3+23+23)%24+1
      //   1         0 (0+ 0+23)%24+1 (1+ 0+23)%24+1 (2+ 0+23)%24+1 (3+ 0+23)%24+1
      //   2         0 (0+ 0+23)%24+1 (1+ 0+23)%24+1 (2+ 0+23)%24+1 (3+ 0+23)%24+1
      //   3         1 (0+ 1+23)%24+1 (1+ 1+23)%24+1 (2+ 1+23)%24+1 (3+ 1+23)%24+1
      //   4         1 (0+ 1+23)%24+1 (1+ 1+23)%24+1 (2+ 1+23)%24+1 (3+ 1+23)%24+1
      innerTrackletSlots[iSlot] = getInnerTrackletSlot(sideId, slotId);
    }
  }

  int TGCInnerTrackletSlotHolder::getInnerTrackletBits(const TGCInnerTrackletSlot* innerTrackletSlots[]) const {

    int inner_eifi = 0;

    const int n_slots    = NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR;
    const int n_regions  = TGCInnerTrackletSlot::NUMBER_OF_REGIONS;
    const int n_readouts = TGCInnerTrackletSlot::NUMBER_OF_READOUTS;
    const int n_bits  = TGCInnerTrackletSlot::NUMBER_OF_TRIGGER_BITS;

    for (int ii = 0; ii < n_slots; ii++) {
      if (ii > 2) continue; // 3 slots per sector at online

      for (int jj = 0; jj < n_regions; jj++) {
        bool pass_readouts = true;

        for (int kk = 0; kk < n_readouts; kk++) {
          bool pass_bits = false;

          for (int ll = 0; ll < n_bits; ll++) {
            pass_bits |= innerTrackletSlots[ii]->getTriggerBit(jj, kk, ll);
          }

          pass_readouts &= pass_bits;
        }

        if (!pass_readouts) continue;

        int tmp_bit = 1 << (ii*2);
        if (jj == TGCInnerTrackletSlot::FI) tmp_bit = tmp_bit << 1;

        inner_eifi |= tmp_bit;
      }
    }

    return inner_eifi;
  }
} //end of namespace bracket

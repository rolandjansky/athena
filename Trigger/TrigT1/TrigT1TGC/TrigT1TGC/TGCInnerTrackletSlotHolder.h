/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCInnerTrackletSlotHolder_hh
#define TGCInnerTrackletSlotHolder_hh

#include "TrigT1TGC/TGCInnerTrackletSlot.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {
  //  class TGCInnerTrackletSlot;

  class TGCInnerTrackletSlotHolder {
  public: 
    /** Constructor */
    TGCInnerTrackletSlotHolder( TGCArguments* );
    /** Destructor */
    ~TGCInnerTrackletSlotHolder() {};

    void clearTriggerBits();
    bool setTriggerBit(const int sideId, const int slotId, 
		       const unsigned int region, const unsigned int readout, 
		       const unsigned int iBit, const bool triggerBit);
    const TGCInnerTrackletSlot* getInnerTrackletSlot(const int sideId, const int slotId) const;
    void getInnerTrackletSlots(const int sideId, const int octId, const int moduleId, 
			       const TGCInnerTrackletSlot* innerTrackletSlots[]) const;
    
    /** returns SL readout trigger bits FI2|EI2|FI1|EI1|FI0|EI0 **/
    int getInnerTrackletBits(const TGCInnerTrackletSlot* innerTrackletSlots[]) const;
    
    enum {
      NUMBER_OF_SIDES = 2, // Side A and Side C
      NUMBER_OF_SLOTS = 24, // Slot01 - Slot24
      FIRST_SLOT = 1,
      NUMBER_OF_SLOTS_PER_TRIGGER_SECTOR = 4 
      // slot23, slot24, slot01, slot02 go to A01 phi0/phi1 Endcap SL
    };

    const TGCArguments* tgcArgs() const { return m_tgcArgs; }

  private:
    TGCInnerTrackletSlot m_innerTrackletSlot[NUMBER_OF_SIDES][NUMBER_OF_SLOTS];
    TGCArguments* m_tgcArgs;
  };

} //end of namespace bracket

#endif // TGCInnerTrackletSlotHolder_hh

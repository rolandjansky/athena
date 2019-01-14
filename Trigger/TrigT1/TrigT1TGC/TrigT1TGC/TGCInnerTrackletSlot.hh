/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCInnerTrackletSlot_hh
#define TGCInnerTrackletSlot_hh

namespace LVL1TGCTrigger {
  class TGCInnerTrackletSlot {
  public: 
    /** Constructor */
    TGCInnerTrackletSlot();
    /** Destructor */
    ~TGCInnerTrackletSlot() = default;
    
    void setSideId(const int sideId);
    int getSideId() const;

    void setSlotId(const int slotId);
    int getSlotId() const;

    bool setTriggerBit(const unsigned int region, const unsigned int readout, const unsigned int iBit, const bool triggerBit);
    bool getTriggerBit(const unsigned int region, const unsigned int readout, const unsigned int iBit) const;
    bool setTriggerBits(const bool val);
    void clearTriggerBits();

    void setTriggerWord(unsigned int word);
    unsigned short getTriggerWord() const;

    enum {
      FI = 0, 
      EI = 1,
      NUMBER_OF_REGIONS = 2,
      WIRE = 0,
      STRIP = 1,
      NUMBER_OF_READOUTS = 2,
      NUMBER_OF_TRIGGER_BITS = 4 
    };

  private:
    int m_sideId; // Side A or Side C
    // slot01 - slot24
    // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/EIFI_PSB_SSW_ConnectionTable_v20080808.pdf
    int m_slotId; 
    // Trigger bits from EI/FI, TRIG0-TRIG3
    // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/celladdress2_asic_rev2.pdf
    bool m_triggerBit[NUMBER_OF_REGIONS][NUMBER_OF_READOUTS][NUMBER_OF_TRIGGER_BITS]; 
  };

} //end of namespace bracket

#endif // TGCInnerTrackletSlot_hh

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSTRIPOFFLINEDATA_H
#define MMSTRIPOFFLINEDATA_H

// Identifier includes
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MmIdHelper.h"

// local includes
#include "TrigT1NSWSimTools/MMStripData.h" 

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short class modeling the strip hit fragment for the NSW L1 offline simulation
   *
   * This holds the hit offline identifier with the corresponding helper plus the following:
   * strip charge. Additional data can be added providing the relative access methods are
   * also added in the abstract class.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *
   *
   */

  class MMStripOfflineData: public MMStripData {

  private:
    const Identifier    m_id;            //!< offline identifier of the strip hit
    const MmIdHelper* m_helper;          //!< helper for the identifier decoding

    int                 m_trig_bcid;     //!< trig BCID
    float m_mmstrip_charge;             //!< strip charge

  public:
    MMStripOfflineData(Identifier id, const MmIdHelper* helper);
    ~MMStripOfflineData();

    void setTrigBCID(int bcid);          //!< set the trig BCID        

    //! methods for retrieving the bare data
    uint32_t id()     const;  //!< get the 32 bit word identifing the fragment
    int BCID()        const;  //!< get the BCID
    float charge()     const;  //!< get the charge
    int trig_BCID()    const;  //!< get the BCID

    //! helper  methods decoding the hit fragment position within the NSW detector
    int sideId()      const;  //!< get the side (0==sideC, 1==sideA)
    int moduleId()    const;  //!< get the physics module (ranging from 0 at lowest R to 1 at highest R)
    int sectorId()    const;  //!< get the sector         (ranging from 0 to 15)
    int multipletId() const;  //!< get the multiplet      (ranging from 0 to 1, 1 is at lowest |z|)
    int gasGapId()    const;  //!< get the gas gap        (ranging from 0 to 3, 1 is at lowest |z|)
    int channelId()   const;  //!< get the channel

  };  // end of MMStripOfflineData class

} // namespace NSWL1

#endif

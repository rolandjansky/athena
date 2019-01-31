/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PADOFFLINEDATA_H
#define PADOFFLINEDATA_H

// Identifier includes
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

// local includes
#include "TrigT1NSWSimTools/PadData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"


// forward declarations
class TVector3;


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short class modeling the pad hit fragment for the NSW L1 offline simulation
   *
   * This holds the BDCI plus the hit offline identifier with the corresponding helper.
   * Additional data can be added provding the relative access methods are also added in
   * the abstract class.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *          Alaettin Serhan Mete <amete@cern.ch>
   *
   */

  class PadOfflineData: public PadData {

  private:
    const Identifier    m_id;      //!< offline identifier of the PAD hit
    const sTgcIdHelper* m_helper;  //!< helper for the identifier decoding

    float               m_time;    //!< signal arrival time after electronics delay is applied
    uint16_t            m_bc_tag;  //!< BC Tag
    
  public:
    PadOfflineData(Identifier id, float time, uint16_t bc_tag, const sTgcIdHelper* helper);
    //PadOfflineData(Identifier id, float time, uint16_t bc_tag, bool isdead, bool ispileup, const sTgcIdHelper* helper);
    ~PadOfflineData();

    //! methods for retrieving the bare data
    Identifier Id() const;
    uint32_t id()              const;  //!< get the 32 bit word identifing the fragment
    uint16_t BC_Tag()          const;  //!< get the BCID
    float    time()            const;  //!< get the arrival time after the delay is applied
    //bool     isDead()          const;  //!< get the isDead bool 
    //bool     isPileup()        const;  //!< get the isPileup bool

    //! helper  methods decoding the hit fragment position within the NSW detector
    int sideId()               const;  //!< get the side            (0==sideC, 1==sideA)
    int moduleId()             const;  //!< get the physics module  (ranging from 1 at lowest R to 3 at highest R)
    int sectorId()             const;  //!< get the sector          (ranging from 1 to 8)
    int sectorType()           const;  //!< get the sector type     (ranging from 0 to 1, Small and Large) 
    int multipletId()          const;  //!< get the multiplet       (ranging from 1 to 2, 1 is at lowest |z|)
    int gasGapId()             const;  //!< get the gas gap         (ranging from 1 to 4, 1 is at lowest |z|)
    int channelId()            const;  //!< get the channel
    int padEtaId()             const;  //!< get the pad eta channel  
    int padPhiId()             const;  //!< get the pad phi channel
    float m_cornerXyz[4][3];
  };  // end of PadOfflineData class

} // namespace NSWL1

#endif

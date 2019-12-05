/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPOFFLINEDATA_H
#define STRIPOFFLINEDATA_H

// Identifier includes
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigit.h"
// local includes
#include "TrigT1NSWSimTools/StripData.h"
#include <string.h>

// namespace for the NSW LVL1 related classes

namespace NSWL1 {


  /**
   *
   *   @short class modeling the strip hit fragment for the NSW L1 offline simulation
   *
   * This holds the hit offline identifier with the corresponding helper plus the following:
   * Trigger BCID, strip charge, band Id, phi Id. Additional data can be added providing the
   * relative access methods are also added in the abstract class.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */

  class StripOfflineData: public StripData {

  private:
    const Identifier    m_id;            //!< offline identifier of the strip hit
    const sTgcIdHelper* m_helper;        //!< helper for the identifier decoding
    int                 m_trig_bcid;
    int m_padTrigIndex;
    float               m_strip_charge;
    float               m_strip_time;
    int                 m_strip_charge_10bit;
    int                 m_strip_charge_6bit;
    int                 m_band_id;
    int                 m_phi_id;
    float               m_x,m_y,m_z=0;
    float               m_lx,m_ly,m_lz=0;
    bool                m_read_strip;
    
    int m_sideId;
    int m_sectorType;
    int m_sectorId;
    int m_moduleId;
    int m_wedgeId;
    int m_layerId;

  public:
    StripOfflineData(Identifier id, const sTgcIdHelper* helper, const sTgcDigit* digit);
    ~StripOfflineData();

    void setTrigBCID(int bcid);
    void setTrigIndex(int );
    void setStripCharge(float charge);
    void setStripCharge_6bit(int charge);
    void setStripCharge_10bit(int charge);
    void setBandId(int band_id);
    void setPhiId(int phi_id);
    void set_readStrip(bool readStrip);

    void setSideId(int);
    void setSectorType(int);
    void setSectorId(int);
    void setModuleId(int);
    void setWedgeId(int);
    void setLayerId(int);


        
    //! methods for retrieving the bare data
    uint32_t id()             const;  //!< get the 32 bit word identifing the fragment
    float time()              const;
    Identifier Identity()     const;  //!< return indentifier
    int BCID()                const;
    int trigIndex()           const;
    float charge()            const;
    int trig_BCID()           const;
    float strip_charge()      const;
    int strip_charge_6bit()   const;
    int strip_charge_10bit()  const;
    bool readStrip()          const;  //!< Check if strip should be read
    int bandId()              const;
    int phiId()               const;

    //! helper  methods decoding the hit fragment position within the NSW detector
    int sideId()      const;  //!< get the side (0==sideC, 1==sideA)
    int isSmall()     const;  //!< get the side (0==sideC, 1==sideA)
    int moduleId()    const;  //!< get the physics module (ranging from 0 at lowest R to 3 at highest R)
    int sectorId()    const;  //!< get the sector (ranging from 0 to 15)
    int sectorType() const;
    int wedge() const;  //!< get the multiplet (ranging from 0 to 1, 1 is at lowest |z|)
    int layer()    const;  //!< get the gas gap (ranging from 0 to 3, 1 is at lowest |z|)
    int channelId()   const;  //!< get the channel
    float globX()       const;
    float globY()       const;
    float globZ()       const;
    float etaCenter() const;     // in ATLAS global / eta of strip center
    void set_globX(float pos);
    void set_globY(float pos);
    void set_globZ(float pos);
    char type()    const;
    std::string stationName() const;
    float locX()       const;
    float locY()       const;
    float locZ()       const;
    void set_locX(float pos);
    void set_locY(float pos);
    void set_locZ(float pos);
  };

} // namespace NSWL1

#endif

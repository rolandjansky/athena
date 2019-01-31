/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    int                 m_trig_bcid;     //!< trig BCID 
    int m_padTrigIndex;
    float               m_strip_charge;  //!< strip charge
    float               m_strip_time;  //!< strip time
    int               m_strip_charge_10bit;  //!< strip 10 bit charge
    int               m_strip_charge_6bit;  //!< strip 6 bit charge
    int                 m_band_id;       //!< band id
    int                 m_phi_id;        //!< phi id
    float               m_x,m_y,m_z=0;
    float               m_lx,m_ly,m_lz=0;
    bool                m_read_strip;
    

  public:
    StripOfflineData(Identifier id, const sTgcIdHelper* helper, const sTgcDigit* digit);
    ~StripOfflineData();

    void setTrigBCID(int bcid);          //!< set the trig BCID
    void setTrigIndex(int );
    void setStripCharge(float charge);   //!< set the strip charge
    void setStripCharge_6bit(int charge);   //!< set the 6bit strip charge
    void setStripCharge_10bit(int charge);   //!< set the 10 bit strip charge
    void setBandId(int band_id);         //!< set the band id
    void setPhiId(int phi_id);           //!< set the phi id
    void set_readStrip(bool readStrip);
        
    //! methods for retrieving the bare data
    uint32_t id()        const;  //!< get the 32 bit word identifing the fragment
    float time()     const;
    Identifier Identity()     const;  //!< return indentifier
    int BCID()        const;  //!< get the BCID
    int trigIndex() const;
    float charge()     const;  //!< get the charge
    int trig_BCID()    const;  //!< get the BCID
    float strip_charge()   const;  //!< get the charge
    int strip_charge_6bit()  const;  //!< get the charge
    int strip_charge_10bit() const;  //!< get the charge
    bool readStrip() const;  //!< Check if strip should be read
    int bandId()      const;  //!< get the band id
    int phiId()       const;  //!< get the phi id

    //! helper  methods decoding the hit fragment position within the NSW detector
    int sideId()      const;  //!< get the side (0==sideC, 1==sideA)
    int isSmall()      const;  //!< get the side (0==sideC, 1==sideA)
    int moduleId()    const;  //!< get the physics module (ranging from 0 at lowest R to 3 at highest R)
    int sectorId()    const;  //!< get the sector (ranging from 0 to 15)
    int wedge() const;  //!< get the multiplet (ranging from 0 to 1, 1 is at lowest |z|)
    int layer()    const;  //!< get the gas gap (ranging from 0 to 3, 1 is at lowest |z|)
    int channelId()   const;  //!< get the channel
    float globX()       const;  //!< get the X position
    float globY()       const;  //!< get the Y position
    float globZ()       const;  //!< get the Z position
    void set_globX(float pos);  //!< get the X position
    void set_globY(float pos);  //!< get the Y position
    void set_globZ(float pos);  //!< get the Z position
    char type()    const;
    std::string stationName() const;
    float locX()       const;  //!< get the X position
    float locY()       const;  //!< get the Y position
    float locZ()       const;  //!< get the Z position
    void set_locX(float pos);  //!< get the X position
    void set_locY(float pos);  //!< get the Y position
    void set_locZ(float pos);  //!< get the Z position

    
  };  // end of StripOfflineData class

} // namespace NSWL1

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//Local includes
#include "TrigT1NSWSimTools/StripOfflineData.h"

#include <string>


namespace NSWL1 {

  StripOfflineData::StripOfflineData(Identifier id, const sTgcIdHelper* helper, const sTgcDigit* digit) : 
    StripData(),
    m_id(id), m_helper(helper),m_trig_bcid(0),m_band_id(-1),m_phi_id(-1)
    {
      m_strip_charge_6bit=digit-> charge_6bit(); 
      m_strip_charge_10bit= digit-> charge_10bit();
      m_strip_charge= digit-> charge();
      m_strip_time=digit->time();
    }

  StripOfflineData::~StripOfflineData() {
        // StripOfflineData doesn't own the helper, so nothing to do here
    }

    void StripOfflineData::setTrigBCID(int bcid) {
        m_trig_bcid = bcid;
    }
    void StripOfflineData::setTrigIndex(int idx){
        m_padTrigIndex=idx;
    }
    void StripOfflineData::setStripCharge_6bit(int charge) {
        m_strip_charge_6bit = charge;
    }

    void StripOfflineData::setStripCharge_10bit(int charge) {
        m_strip_charge_10bit = charge;
    }

    void StripOfflineData::setStripCharge(float charge) {
        m_strip_charge = charge;

    }


    void StripOfflineData::setBandId(int band_id){
        m_band_id = band_id;
    }

    void StripOfflineData::setPhiId(int phi_id) {
        m_phi_id = phi_id;
    }


    //! methods for retrieving the bare data
    uint32_t StripOfflineData::id()     const { return m_id.get_identifier32().get_compact(); }
    float StripOfflineData::time()     const { return m_strip_time; }
    Identifier StripOfflineData::Identity()     const { return m_id; }
    int StripOfflineData::trig_BCID()        const { return m_trig_bcid; }
    int StripOfflineData:: trigIndex() const { return m_padTrigIndex;}
    
    float StripOfflineData::strip_charge()    const {return  m_strip_charge;} 
    int StripOfflineData::strip_charge_6bit()    const {return  m_strip_charge_6bit;} 
    int StripOfflineData::strip_charge_10bit()    const {return  m_strip_charge_10bit;} 
    
    int StripOfflineData::bandId()      const { return m_band_id; }
    int StripOfflineData::phiId()       const { return m_phi_id; }

    //! helper  methods decoding the hit fragment position within the NSW detector
    int StripOfflineData::sideId()      const { 
        if (m_helper)  return (m_helper->stationEta(m_id)<0)? 0 : 1;
        return -1;
    }

    int StripOfflineData::moduleId()    const {
        if (m_helper)  return m_helper->stationEta(m_id);
        return -1;
    }

    int StripOfflineData::sectorId()    const {
      if (m_helper) {
        return  m_helper->stationPhi(m_id);
      }
      return -1;
    }



    std::string StripOfflineData::stationName()    const {
        if (m_helper) {
             return m_helper->stationNameString(m_helper->stationName(m_id));
        }
        return "";
    }

    char StripOfflineData::type()    const {
        if (m_helper) {
            std::string stName = m_helper->stationNameString(m_helper->stationName(m_id));
            bool isSmall = stName[2]=='S';
            return (isSmall)? 'S' : 'L';
        }
        return -1;
    }


  int StripOfflineData::isSmall() const {
    if (m_helper) {
        std::string stName = m_helper->stationNameString(m_helper->stationName(m_id));
        bool isSmall = stName[2]=='S';
        return isSmall;
    }
    return 0;
  }


  int StripOfflineData::wedge() const {
    if (m_helper) return m_helper->multilayer(m_id) ;
    return -1;
  }

  int StripOfflineData::layer()    const {
    if (m_helper) return m_helper->gasGap(m_id) ; 
    return -1;
  }

    int StripOfflineData::channelId()   const {
        if (m_helper) return m_helper->channel(m_id) ;
        return -1;
    }

  float StripOfflineData::globX()   const {
    return  m_x; 
      }
  float StripOfflineData::globY()   const {
    return m_y; 
      }
  float StripOfflineData::globZ()   const {
    return  m_z; 
      }

  void StripOfflineData::set_globX(float pos)    {
      m_x=pos; 
      }
  void StripOfflineData::set_globY(float pos)    {
     m_y=pos; 
      }
  void StripOfflineData::set_globZ(float pos)    {
      m_z=pos; 
      }

  float StripOfflineData::locX()   const {
    return  m_lx; 
      }
  float StripOfflineData::locY()   const {
    return m_ly; 
      }
  float StripOfflineData::locZ()   const {
    return  m_lz; 
      }


  bool StripOfflineData::readStrip()   const {
    return  m_read_strip;
  }

  void StripOfflineData::set_readStrip(bool readStrip){
      m_read_strip=readStrip;
  }

 
  void StripOfflineData::set_locX(float pos)    {
      m_lx=pos; 
  }
  void StripOfflineData::set_locY(float pos)    {
     m_ly=pos; 
  }
  void StripOfflineData::set_locZ(float pos)    {
      m_lz=pos; 
  }



}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_RESOURCE_ID_H_
#define _MUON_NSW_RESOURCE_ID_H_

#include "eformat/SourceIdentifier.h"

#include "MuonNSWCommonDecode/NSWDecodeBitmaps.h"
#include "MuonNSWCommonDecode/NSWDecodeHelper.h"

namespace Muon
{
  namespace nsw
  {
    enum NSWResourceTypes
    {
      NSW_RESOURCE_PAD = 0,
      NSW_RESOURCE_STRIP = 1,
      NSW_RESOURCE_TRIG_PROC = 2,
      NSW_RESOURCE_PAD_TRIG = 3,
      NSW_RESOURCE_L1DDC = 4,
      NSW_RESOURCE_ADDC = 5,
      NSW_RESOURCE_ROUTER = 6,
      NSW_RESOURCE_RIM_L1DDC = 7
    };

    enum NSWDataTypes
    {
      NSW_DATA_L1A = 0,
      NSW_DATA_MONITOR = 1,
      NSW_DATA_TO_SCA = 2,
      NSW_DATA_FROM_SCA = 3,
      NSW_DATA_TTC = 4,
      NSW_DATA_L1A_INFO = 5,
      NSW_DATA_AUX = 6
    };

    class NSWResourceId
    {
     private:
      uint8_t m_Elink;
      uint8_t m_Radius;
      uint8_t m_Layer;
      uint8_t m_Sector;
      uint8_t m_ResourceType;
      uint8_t m_DataType;
      uint8_t m_Version;
      uint8_t m_DetId;

      // For obsolete pre-versioned data

      uint8_t m_Eta;
      uint8_t m_Tech;

      bool m_pre_version;

     public:
      explicit NSWResourceId (uint32_t logical_id);
      virtual ~NSWResourceId () {};

      uint8_t elink          () {return m_Elink;};
      uint8_t radius         () {return m_Radius;};
      uint8_t layer          () {return m_Layer;};
      uint8_t sector         () {return m_Sector;};
      uint8_t resourceType   () {return m_ResourceType;};
      uint8_t dataType       () {return m_DataType;};
      uint8_t version        () {return m_Version;};
      uint8_t detId          () {return m_DetId;};

      // For obsolete pre-versioned data

      uint8_t group          () {return m_Elink;};
      uint8_t eta            () {return m_Eta;};
      uint8_t technology     () {return m_Tech;}

      bool pre_version       () {return m_pre_version;};

      // Offline decoder

      bool is_large_station  ();

      int8_t  station_eta    ();
      uint8_t station_phi    ();
      uint8_t multi_layer    ();
      uint8_t gas_gap        ();
    };
  }
}

inline bool Muon::nsw::NSWResourceId::is_large_station ()
{
  // counting from 0, even sectors are large
  return ((m_Sector % 2) == 0);
}

inline int8_t Muon::nsw::NSWResourceId::station_eta ()
{
  int8_t mod_eta;
  // Odd identifiers are on side A
  int8_t side_sign = (m_DetId % 2) == 0 ? -1 : 1;

  // MM identifiers are less than STGC identifiers
  if (m_DetId < eformat::MUON_STGC_ENDCAP_A_SIDE)
    mod_eta = m_Radius < 10 ? 1 : 2;
  else
    mod_eta = m_Radius + 1;

  return (side_sign * mod_eta);
}

inline uint8_t Muon::nsw::NSWResourceId::station_phi ()
{
  // Becomes 1 to 8
  return (m_Sector / 2 + 1);
}

inline uint8_t Muon::nsw::NSWResourceId::multi_layer ()
{
  // 1 = IP; 2 = HO
  return (m_Layer / 4 + 1);
}

inline uint8_t Muon::nsw::NSWResourceId::gas_gap ()
{
  // 1 to 4
  return (m_Layer + 1 - 4 * (m_Layer / 4));
}

inline Muon::nsw::NSWResourceId::NSWResourceId (uint32_t logical_id)
{
  m_DetId        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskDetId, Muon::nsw::bitPosDetId);

  if (m_DetId == 0) // pre-versioned logical ID data format
  {
    m_pre_version     = true;
    m_Version         = 0;
    m_Elink           = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskElink, Muon::nsw::bitPosElink);
    m_Radius          = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskRadius, Muon::nsw::bitPosRadius);
    m_Layer           = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskLayer, Muon::nsw::bitPosLayer);
    m_Sector          = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskSector, Muon::nsw::bitPosSector);
    m_Eta             = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsEta, Muon::nsw::bitPosObsEta);
    m_ResourceType    = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsResType, Muon::nsw::bitPosObsResType);
    m_Tech            = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsTech, Muon::nsw::bitPosObsTech);
    m_DataType        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskObsDataType, Muon::nsw::bitPosObsDataType);
    m_DetId           = eformat::MUON_STGC_ENDCAP_A_SIDE + m_Eta - 2 * m_Tech;
  }
  else
  {
    m_pre_version  = false;
    m_Elink        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskElink, Muon::nsw::bitPosElink);
    m_Radius       = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskRadius, Muon::nsw::bitPosRadius);
    m_Layer        = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskLayer, Muon::nsw::bitPosLayer);
    m_Sector       = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskSector, Muon::nsw::bitPosSector);
    m_ResourceType = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskResType, Muon::nsw::bitPosResType);
    m_DataType     = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskDataType, Muon::nsw::bitPosDataType);
    m_Version      = Muon::nsw::helper::get_bits (logical_id, Muon::nsw::bitMaskVersion, Muon::nsw::bitPosVersion);
  }
}

#endif // _MUON_NSW_RESOURCE_ID_H_



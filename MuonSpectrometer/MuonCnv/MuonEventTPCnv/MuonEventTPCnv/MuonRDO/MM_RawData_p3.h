/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MM_RAWDATA_P3_H
#define MM_RAWDATA_P3_H

namespace Muon
{
  class MM_RawData_p3
  {
  public:
  MM_RawData_p3() : m_id(0), m_channel(0), m_time(0), m_charge(0), m_relBcid(0) {}
    
    unsigned int        m_id; //!< FIXME! Remove this eventually
    
    /// writing out channel id, time and charge
    int m_channel;
    
    int m_time;
    
    int m_charge;

    uint16_t m_relBcid;
    
  };
}

#endif 

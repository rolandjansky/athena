/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MM_RAWDATA_P1_H
#define MM_RAWDATA_P1_H

namespace Muon
{
  class MM_RawData_p1
  {
  public:
  MM_RawData_p1() : m_id(0), m_channel(0), m_time(0), m_charge(0) {}
    
    unsigned int        m_id; //!< FIXME! Remove this eventually
    
    /// readout channel Id
    int m_channel;
    /// tdc counts
    int m_time;
    /// adc counts
    int m_charge;
    
    
  };
}

#endif 

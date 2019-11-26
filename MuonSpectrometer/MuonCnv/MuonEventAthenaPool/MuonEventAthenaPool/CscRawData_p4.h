/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NOTE: This is an exact copy of CscRawData_p3, since from p3 to p4 only the meaning of the m_hashId changed:
// Until p3, the m_hashId stores the identifier hash which encodes the geometrical position of the CSC
// From p4 onwards, the m_hashId stores the position of the CSC identifier in the vector of identifiers

#ifndef MUON_CSCRAWDATA_P4_H
#define MUON_CSCRAWDATA_P4_H

#include <iostream>
#include <vector>
#include <inttypes.h>

class CscRawData_p4  
{
public:
    CscRawData_p4 () 
        : 
        m_amps(), 
        m_address(0), 
        m_id(0), 
        m_time(0), 
        m_rpuID(0), 
        m_width(0), 
        m_isTimeComputed(false), 
        m_hashId(0)  
    {}
        
    friend class  CscRawDataCnv_p4;

private:
    
    /// in common with _p1
    
    /** ADC samples */
    std::vector<uint16_t> m_amps;
    /** online identifier of the first strip in the ROD cluster */
    uint32_t m_address; 
    /** online identifier of the collection in which this strip is */
    uint16_t m_id;
    
    // m_dpuiID removed from _p1
    
    //New to _p2
    
    /** online peaking time of the first strip: this is 0 if failed to compute time in the ROD */
    uint16_t m_time;
    /** SPU -Sparsifier Processing Unit - identifier in which this strip is */
    uint16_t m_rpuID;
    /** number of consecutive strips forming the on-line cluster */
    uint16_t m_width;
    /** failed to compute the time in the ROD or not - if failure m_time = 0 */
    bool m_isTimeComputed;
    /** offline hash identifier of the first strip in the online cluster */
    uint32_t m_hashId;
};

#endif


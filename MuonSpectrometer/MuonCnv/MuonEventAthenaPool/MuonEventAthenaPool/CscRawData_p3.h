/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATA_P3_H
#define MUON_CSCRAWDATA_P3_H


#include <iostream>
#include <vector>
#include <inttypes.h>

/* Persistent representation of the CscRawData class
     @author Edward.Moyse@cern.ch
 */
class CscRawData_p3  
{
public:
    CscRawData_p3 () 
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
        
    friend class  CscRawDataCnv_p3;

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


/** Overload of << operator for std::ostream for debug output
*/ 
//std::ostream& operator << ( std::ostream& sl, const  CscRawData_p3& data);
//std::ostream& operator << ( std::ostream& sl, const  CscRawData& data);


#endif







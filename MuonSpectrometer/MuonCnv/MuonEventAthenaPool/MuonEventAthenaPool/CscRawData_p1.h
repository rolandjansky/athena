/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATA_P1_H
#define MUON_CSCRAWDATA_P1_H

/*
 Persistent representation of the CscRawData class
 Author: Marcin Nowak
 CERN, December 2005
 */

#include <iostream>
#include <vector>
#include <inttypes.h>

class CscRawData_p1  
{
public:
    CscRawData_p1 () : m_address(0), m_id(0), m_dpuID(0) {}
    
    uint16_t dpuID() const          { return m_dpuID; }
    uint16_t identify() const       { return m_id;} 
    uint32_t getAddress() const 	{ return m_address; }
    const std::vector<uint16_t>  getSamples() const { return m_amps; }
    
    friend class  CscRawDataCnv_p1;
private:
    
    /// persistent data members
    std::vector<uint16_t> m_amps;
    uint32_t m_address; 
    uint16_t m_id;
    uint16_t m_dpuID;
    
};


/** Overload of << operator for std::ostream for debug output
*/ 
std::ostream& operator << ( std::ostream& sl, const  CscRawData_p1& data);
//std::ostream& operator << ( std::ostream& sl, const  CscRawData& data);


#endif







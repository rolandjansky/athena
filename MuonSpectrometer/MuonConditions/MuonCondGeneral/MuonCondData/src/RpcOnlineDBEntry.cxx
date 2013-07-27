/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondData/RpcOnlineDBEntry.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

namespace MuonCalib{

  // initialize from  whole string read from file

  RpcOnlineDBEntry::RpcOnlineDBEntry(uint64_t theID, uint64_t mask1, uint64_t mask2, uint64_t mask3):m_mask1(mask1), m_mask2(mask2), m_mask3(mask3), m_id(theID) {
  }

  uint64_t  RpcOnlineDBEntry::getID() const {
    
    return m_id;
    
  }
  
  void RpcOnlineDBEntry::getColumns(std::string &mask1, std::string &mask2,std::string &mask3) const{
    
    std::ostringstream mask1_str,mask2_str,mask3_str;

    mask1_str<<std::hex<<m_mask1<<std::dec;
    mask2_str<<std::hex<<m_mask2<<std::dec;
    mask3_str<<std::hex<<m_mask3<<std::dec;


    mask1=mask1_str.str();
    mask2=mask2_str.str();
    mask3=mask3_str.str();

    //    std::cout<<"giving columns are "<<

  }

}// End namespace MuonCalib

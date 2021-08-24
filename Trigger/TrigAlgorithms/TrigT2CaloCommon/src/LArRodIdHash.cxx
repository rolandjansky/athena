/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloCommon/LArRodIdHash.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include <iostream>

#include "eformat/SourceIdentifier.h"

#include "LArIdentifier/LArReadoutModuleService.h"

using eformat::helper::SourceIdentifier; 

// This class converts a LArReadoutModuleID into an integer, 
// 


void LArRodIdHash::initialize( int offset, const std::vector<HWIdentifier>& roms )  {

// 

  m_offset = offset; 

  LArReadoutModuleService larROMService;
   eformat::SubDetector detid ;
   std::vector<ID>  rmod;
   std::vector<HWIdentifier>::const_iterator tit =  roms.begin(); 
   std::vector<HWIdentifier>::const_iterator tit_end =  roms.end(); 
 
   for(; tit!=tit_end; ++tit)
     { 
      
       HWIdentifier mId = (*tit); 
       detid = (eformat::SubDetector) larROMService.subDet(mId); 
       uint8_t m = larROMService.rodFragId(mId); 
 
       SourceIdentifier sid = SourceIdentifier(detid,m); 
       uint32_t rod_id =  sid.code(); 
       rmod.push_back(rod_id);
       
     }

  std::vector<ID>::const_iterator 
    it = rmod.begin(); 
  std::vector<ID>::const_iterator 
    it_end = rmod.end() ; 

  int n = 0; 
  for (; it!=it_end;++it) {
    ID id = *it; 
    // std::cout << " LArReadoutModuleID = "<<id.id()<<std::endl;
    unsigned int i = id; 
    m_lookup[i] = n ; 
    m_int2id.push_back(id); 
    ++n; 
  } 

  // cout << " Number of LArReadoutModuleID valid ID  "<< n <<endl; 
  m_size = n; 

}

LArRodIdHash::ID LArRodIdHash::identifier(int index) const {

  return m_int2id[index] ; 

}

int LArRodIdHash::operator() (const ID& id) const {

  unsigned int i = id  ; 
  std::map<unsigned int, int> ::const_iterator it= m_lookup.find(i); 
  if(it!=m_lookup.end()) return (*it).second; 
  std::cout <<" ERROR in LArRodIdHash :  invalid Rod number"
	    <<std::endl;
  return -1; 

}

int LArRodIdHash::max() const {

return m_size; 

}

int LArRodIdHash::offset() const {

return m_offset; 

}


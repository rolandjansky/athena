/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloCommon/LArRodIdHash.h" 
#include "LArCabling/LArCablingLegacyService.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include <iostream>

#include "eformat/SourceIdentifier.h"

#include "LArIdentifier/LArReadoutModuleService.h"

using eformat::helper::SourceIdentifier; 

// This class converts a LArReadoutModuleID into an integer, 
// 

// default contructor 
LArRodIdHash::LArRodIdHash( ) 
  : m_size(0), m_offset(0)
{

}


void LArRodIdHash::initialize( int offset )  {

// 

  m_offset = offset; 

  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc);

  //StatusCode sc = svcLoc->service( "ToolSvc",toolSvc );
  if(sc.isFailure())
    {std::cerr << "LArRawChannelContainer::add ERROR: Can not retrieve ToolSvc" << std::endl;
     return;
    }

  LArCablingLegacyService *larCablingSvc;
  sc = toolSvc->retrieveTool("LArCablingLegacyService",larCablingSvc);
  if(sc.isFailure())
    {std::cerr << "LArRawChannelContainer::add ERROR: Can not retrieve LArCablingLegacyService" << std::endl;
     return;
    }

  //LArCablingService* larCablingSvc = LArCablingService::getInstance(); 
  LArReadoutModuleService larROMService;
   eformat::SubDetector detid ;
   std::vector<ID>  rmod;
   const std::vector<HWIdentifier>& roms = larCablingSvc->getLArRoModIDvec();
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

/*
  std::vector<ID>  rmod;
	for(unsigned int i=0x410000;i<=0x4100df;i++)
		rmod.push_back(i);
	for(unsigned int i=0x420000;i<=0x4200df;i++)
		rmod.push_back(i);
	for(unsigned int i=0x430000;i<=0x430089;i++)
		rmod.push_back(i);
	for(unsigned int i=0x440000;i<=0x440089;i++)
		rmod.push_back(i);
	for(unsigned int i=0x450000;i<=0x45000b;i++)
		rmod.push_back(i);
	//for(unsigned int i=0x46000c;i<=0x460017;i++)
	for(unsigned int i=0x460000;i<=0x46000b;i++)
		rmod.push_back(i);
	for(unsigned int i=0x470000;i<=0x470006;i++)
		rmod.push_back(i);
	//for(unsigned int i=0x480007;i<=0x48000d;i++)
	for(unsigned int i=0x480000;i<=0x480007;i++)
		rmod.push_back(i);
*/
  
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
/*
  std::vector<LArReadoutModuleID> vec = larCablingSvc->getLArRoModIDvec();
  for(int i=0;i<vec.size();i++){
	std::cout << i << " " << std::hex <<vec[i].id() << " 0x"
        << larCablingSvc->robFragId(vec[i].id()) << " 0x"
        << larCablingSvc->rosId(vec[i].id()) << " 0x"
        << larCablingSvc->rol(vec[i].id()) << " 0x"
        << larCablingSvc->rodCrate(vec[i].id()) << std::dec << std::endl;
  }
*/

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


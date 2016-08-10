/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArReadoutModuleService.h"
#include "AthenaKernel/getMessageSvc.h"


 // initialization of static member variables to shift bit fields 
 // of HardwareID in proper positions 
const unsigned int LArReadoutModuleService:: m_atlas_tb = 1 ;    //  bit  0
const unsigned int LArReadoutModuleService:: m_barrel_ec = 2;    //  bit  1
const unsigned int LArReadoutModuleService:: m_pos_neg = 4 ;     //  bit  2

const unsigned int LArReadoutModuleService:: m_emhf  =  8 ;      //  bits 3-4
const unsigned int LArReadoutModuleService:: s_rosId = 32 ;          //  bits 5-9
const unsigned int LArReadoutModuleService:: s_rodFragId = 1024 ;    //  bits 10-17
const unsigned int LArReadoutModuleService:: s_rodCrate =  262144 ;  //  bits 18-19
const unsigned int LArReadoutModuleService:: s_rodSlot = 1048576 ;   //  bits 20-23

const int LArReadoutModuleService:: s_LAR_EM_BARREL_A_SIDE  = 0x41 ;
const int LArReadoutModuleService:: s_LAR_EM_BARREL_C_SIDE  = 0x42 ;
const int LArReadoutModuleService:: s_LAR_EM_ENDCAP_A_SIDE  = 0x43 ; 
const int LArReadoutModuleService:: s_LAR_EM_ENDCAP_C_SIDE  = 0x44 ;
const int LArReadoutModuleService:: s_LAR_HAD_ENDCAP_A_SIDE = 0x45 ;
const int LArReadoutModuleService:: s_LAR_HAD_ENDCAP_C_SIDE = 0x46 ;
const int LArReadoutModuleService:: s_LAR_FCAL_A_SIDE       = 0x47 ;
const int LArReadoutModuleService:: s_LAR_FCAL_C_SIDE       = 0x48 ;


// Concrete methods for the LArReadoutModuleService

//-----------------------------------------------------
/** Creates LArReadoutModuleID from explicit ReadoutModule descriptors */
HWIdentifier LArReadoutModuleService::createReadoutModuleID(int l_atlas_tb, int l_barrel_ec, int l_pos_neg, int l_em_hec_fcal, int l_rosId, int l_rodFragId, int l_rodCrate, int l_rodSlot) {

     	
    std::string errorReport = checkReadoutModuleValues(l_atlas_tb, l_barrel_ec, l_pos_neg, l_em_hec_fcal, l_rosId, l_rodFragId, l_rodCrate, l_rodSlot) ;

    if( errorReport.length() != 0 ) { // error detected 
      MsgStream msg(Athena::getMessageSvc(), " LArReadoutModuleService");
      msg << MSG::ERROR << " createReadoutModuleID called with parameters: "  
	  << l_atlas_tb << ", " << l_barrel_ec << ", " << l_pos_neg << ", " << l_em_hec_fcal 
	  << ", " << l_rosId << ", " << l_rodFragId << ", " << l_rodCrate << ", " << l_rodSlot << endmsg;
      msg << MSG::ERROR << errorReport << endmsg;
      return HWIdentifier(); //Return (invalid) default instance
    }
    
    l_rodCrate = l_rodCrate-1; 

    unsigned int id_int = l_atlas_tb * m_atlas_tb + l_barrel_ec * m_barrel_ec + l_pos_neg * m_pos_neg + l_em_hec_fcal * m_emhf +
                          l_rosId * s_rosId + l_rodFragId * s_rodFragId + l_rodCrate * s_rodCrate + l_rodSlot * s_rodSlot ;
    HWIdentifier l_id(id_int) ;
    return l_id ; 
}



//-------------------------------------------------------------
std::string LArReadoutModuleService::checkReadoutModuleValues(int l_atlas_tb, int l_barrel_ec, int l_pos_neg, int l_em_hec_fcal, int l_rosId, int l_rodFragId, int l_rodCrate, int l_rodSlot)
{ 
    std::string l_std_str = "";

    if( l_atlas_tb > 1 || l_atlas_tb < 0 ) { // check of atlas_tb 
      l_std_str += std::string("atlas_tb out of range ,") ; 
    }
    if( l_barrel_ec > 1 || l_barrel_ec < 0 ) {  // check of barrel_ec
      l_std_str += std::string("barrel_ec out of range ,") ; 
    }
    if( l_pos_neg > 1 || l_pos_neg < 0 ) {  // check of pos_neg
      l_std_str += std::string("pos_neg out of range ,") ; 
    }
    if( l_em_hec_fcal > 2 || l_em_hec_fcal < 0 ) {  // check of em_hec_fcal
      l_std_str += std::string("em_hec_fcal out of range ,") ; 
    }
    // check of rodID, rodFragId, rodCrate and rodSlot
    if( l_barrel_ec == 0 && l_em_hec_fcal == 0 ) {
	// EMB
	if( l_rosId > 27 || l_rosId < 0) {
	  l_std_str += std::string("rosId out of range ,") ; 
        }
	if( l_rodFragId > 223 || l_rodFragId < 0) {
	  l_std_str += std::string("rodFragId out of range ,") ; 
        }
	if( l_rodCrate > 4 || l_rodCrate < 1) {
	  l_std_str += std::string("rodCrate out of range ,") ; 
        }
	if( l_rodSlot > 14 || l_rodSlot < 1) {
	  l_std_str += std::string("rodSlot out of range ,") ; 
        }
    } 
    if( l_barrel_ec == 1 && l_em_hec_fcal == 0 ) {
	// EMEC
	if( l_rosId > 17 || l_rosId < 0) {
	  l_std_str += std::string("rosId out of range ,") ; 
        }
	if( l_rodFragId > 137 || l_rodFragId < 0) {
	  l_std_str += std::string("rodFragId out of range ,") ; 
        }
	if( l_rodCrate > 3 || l_rodCrate < 1) {
	  l_std_str += std::string("rodCrate out of range ,") ; 
        }
	if( l_rodSlot > 13 || l_rodSlot < 1) {
	  l_std_str += std::string("rodSlot out of range ,") ; 
        }
    }
    if( l_barrel_ec == 1 && l_em_hec_fcal == 1 ) {
	// HEC
	if( l_rosId > 2 || l_rosId < 0) {
	  l_std_str += std::string("rosId out of range ,") ; 
        }
	if( l_rodFragId > 23 || l_rodFragId < 0) {
	  l_std_str += std::string("rodFragId out of range ,") ; 
        }
//	if( l_rodCrate > 1 || l_rodCrate < 1) {
	if( l_rodCrate > 2 || l_rodCrate < 1) {
	  l_std_str += std::string("rodCrate out of range ,") ; 
        }
	if( l_rodSlot > 6 || l_rodSlot < 1) {
	  l_std_str += std::string("rodSlot out of range ,") ; 
        }
    }
    if( l_barrel_ec == 1 && l_em_hec_fcal == 2 ) {
	// FCAL
	if( l_rosId > 1 || l_rosId < 0) {
	  l_std_str += std::string("rosId out of range ,") ; 
        }
	if( l_rodFragId > 13 || l_rodFragId < 0) {
	  l_std_str += std::string("rodFragId out of range ,") ; 
        }
	if( l_rodCrate > 2 || l_rodCrate < 1) {
//	if( l_rodCrate > 1 || l_rodCrate < 1) {
	  l_std_str += std::string("rodCrate out of range ,") ; 
        }
	if( l_rodSlot > 4 || l_rodSlot < 1) {
	  l_std_str += std::string("rodSlot out of range ,") ; 
        }
    }


  return l_std_str;

}

//-------------------------------------------------------
int LArReadoutModuleService::atlas_tb(const HWIdentifier & l_id) {
    return   ( ((int) l_id.get_identifier32().get_compact()) & 1 )  ;
}
//-------------------------------------------------------
int LArReadoutModuleService::barrel_ec(const HWIdentifier & l_id) {
   return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 2 ) ) >> 1 ;
}
//------------------------------------------------------
int LArReadoutModuleService::pos_neg(const HWIdentifier & l_id) {
   return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 4 ) ) >> 2  ;
}
//-------------------------------------------------------
int LArReadoutModuleService::em_hec_fcal(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x18 ) ) >> 3 ;
}
//----------------------------------------------------------
int LArReadoutModuleService::subDet(const HWIdentifier & l_id) {
    int value=0 ;
    int l_barrel_ec = ( ( ( (int) l_id.get_identifier32().get_compact() ) & 2 ) ) >> 1 ;
    int l_pos_neg   = ( ( ( (int) l_id.get_identifier32().get_compact() ) & 4 ) ) >> 2  ;
    int l_emhf      = ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x18 ) ) >> 3 ;
    if(l_barrel_ec == 0) {
      if(l_pos_neg == 1) {
        value = s_LAR_EM_BARREL_A_SIDE ;
      } else {
        value = s_LAR_EM_BARREL_C_SIDE ;
      }
    }
    else if(l_emhf == 0) {
      if(l_pos_neg == 1) {
        value = s_LAR_EM_ENDCAP_A_SIDE ;
      } else {
        value = s_LAR_EM_ENDCAP_C_SIDE ;
      }
    }
    else if(l_emhf == 1) {
      if(l_pos_neg == 1) {
        value = s_LAR_HAD_ENDCAP_A_SIDE ;
      } else {
        value = s_LAR_HAD_ENDCAP_C_SIDE ;
      }
    }
    else if(l_emhf == 2) {
      if(l_pos_neg == 1) {
        value = s_LAR_FCAL_A_SIDE ;
      } else {
        value = s_LAR_FCAL_C_SIDE ;
      }
    }
	
    return value ;
}
//------------------------------------------------------------
int LArReadoutModuleService::robFragId(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x3fc00 ) ) >> 10 ;
}
//------------------------------------------------------------
int LArReadoutModuleService::rol(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x3fc00 ) ) >> 10 ;
}
//----------------------------------------------------------
int LArReadoutModuleService::rosId(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x3e0 ) ) >> 5 ;
}
//------------------------------------------------------------
int LArReadoutModuleService::rodFragId(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0x3fc00 ) ) >> 10 ;
}
//------------------------------------------------------------
int LArReadoutModuleService::rodCrate(const HWIdentifier & l_id) {
// Shift by 1, internally (0-3), external (1-4)
    return ( ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0xc0000 ) ) >> 18 ) + 1 ;
}
//------------------------------------------------------------
int LArReadoutModuleService::rodSlot(const HWIdentifier & l_id) {
    return ( ( ( (int) l_id.get_identifier32().get_compact() ) & 0xf00000 ) ) >> 20 ;
}

//-------------------------------------------------------------
std::string LArReadoutModuleService::cnvToString(const HWIdentifier & id) { 
    char * l_str = new char[200] ; 
    sprintf( l_str , "atlas_tb : %d, barrel_ec : %d, pos_neg: %d, em_hec_fcal: %d, rosId: %d, rodFragId: %d, rodCrate: %d, rodSlot: %d ", 
             atlas_tb(id) , barrel_ec(id) , pos_neg(id) , em_hec_fcal(id) , rosId(id) , rodFragId(id), rodCrate(id), rodSlot(id) ) ;
    std::string str(l_str);
    delete[] l_str ;
    return str ;

}
//-------------------------------------------------------------
std::string LArReadoutModuleService::cnvToStringTDAQ(const HWIdentifier & id) { 
    char * l_str = new char[200] ; 
    sprintf( l_str , "atlas_tb : %d, subDet : %x, rosId: %d, rodFragId: %d, rodCrate: %d, rodSlot: %d ", 
             atlas_tb(id) , subDet(id) , rosId(id) , rodFragId(id), rodCrate(id), rodSlot(id) ) ;
    std::string str(l_str);
    delete[] l_str ;
    return str ;

}


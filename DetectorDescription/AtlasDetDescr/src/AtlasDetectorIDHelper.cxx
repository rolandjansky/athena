/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AtlasDetectorIDHelper.cxx,v 1.6 2006-01-11 09:24:23 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AtlasDetectorIDHelper.h"
#include "IdDict/IdDictDefs.h"  
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

AtlasDetectorIDHelper::AtlasDetectorIDHelper(void)
	:
	m_isSLHC(false),
	m_pixel_region_index(UNDEFINED),
	m_sct_region_index(UNDEFINED),
	m_trt_region_index(UNDEFINED),
	m_lar_em_region_index(UNDEFINED),
	m_lar_hec_region_index(UNDEFINED),
	m_lar_fcal_region_index(UNDEFINED),
        m_lvl1_region_index(UNDEFINED),
        m_dm_region_index(UNDEFINED),
	m_tile_region_index(UNDEFINED),
	m_mdt_region_index(UNDEFINED),
	m_csc_region_index(UNDEFINED),
	m_rpc_region_index(UNDEFINED),
	m_tgc_region_index(UNDEFINED),
	m_mm_region_index(UNDEFINED),
	m_stgc_region_index(UNDEFINED),
	m_muon_station_index(UNDEFINED),
        m_alfa_region_index(UNDEFINED),
        m_bcm_region_index(UNDEFINED),
        m_lucid_region_index(UNDEFINED),
        m_zdc_region_index(UNDEFINED),
	m_initialized(false),
	m_station_field(0),
        m_msgSvc(0)
{}

AtlasDetectorIDHelper::~AtlasDetectorIDHelper(void)
{
}

// AtlasDetectorIDHelper::AtlasDetectorIDHelper (const AtlasDetectorIDHelper &)
// {
// }

//  AtlasDetectorIDHelper & AtlasDetectorIDHelper::operator= (const AtlasDetectorIDHelper & other)
//  {
//      return other;
//  }


//  AtlasDetectorIDHelper * 
//  AtlasDetectorIDHelper::instance()
//  {

//      static AtlasDetectorIDHelper* instance = 0;

//      if (instance == 0) {
//  	instance = new AtlasDetectorIDHelper;
//      }
//      return instance;
//  }


int         
AtlasDetectorIDHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{

    if(m_initialized) return(0);
    m_initialized = true;

    AtlasDetectorID atlas_id;
    ExpandedIdentifier id;

    const IdDictDictionary* 	dict = dict_mgr.find_dictionary ("InnerDetector"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access InnerDetector dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access InnerDetector dictionary "
                      << std::endl;
        }
    }
    else {
	// Check if this is SLHC layout
	m_isSLHC = (dict->m_version=="SLHC");

	// Save index to a PIXEL region for unpacking
	id = atlas_id.pixel_exp(); 
	if (dict->find_region(id, m_pixel_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find pixel region index: id, reg "  
                    << (std::string)id << " " << m_pixel_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find pixel region index: id, reg "  
                          << (std::string)id << " " << m_pixel_region_index
                          << std::endl;
            }
	}

	// for SLHC layout one cannot get the sct region as below, nor
	// is there any trt regions
	if (!m_isSLHC) {
	    
	    // Save index to a SCT region for unpacking
	    id = atlas_id.sct_exp();
	    if (dict->find_region(id, m_sct_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find sct region index: id, reg "  
                        << (std::string)id << " " << m_sct_region_index
                        << endmsg;
                }
                else {
                    std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find sct region index: id, reg "  
                              << (std::string)id << " " << m_sct_region_index
                              << std::endl;
                }
	    }

	    // Save index to a TRT region for unpacking
	    id = atlas_id.trt_exp(); 
	    if (dict->find_region(id, m_trt_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find trt region index: id, reg "  
                        << (std::string)id << " " << m_trt_region_index
                        << endmsg;
                }
                else {
                    std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find trt region index: id, reg "  
                              << (std::string)id << " " << m_trt_region_index
                              << std::endl;
                }
            }
	}
    }
    
    dict = dict_mgr.find_dictionary ("LArCalorimeter"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access LArCalorimeter dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access LArCalorimeter dictionary "
                      << std::endl;
        }
    }
    else {
	
	// Save index to a LAR_EM region for unpacking
	id = atlas_id.lar_em_exp(); 
	if (dict->find_region(id, m_lar_em_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find lar_em region index: id, reg "  
                    << (std::string)id << " " << m_lar_em_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find lar_em region index: id, reg "  
                          << (std::string)id << " " << m_lar_em_region_index
                          << std::endl;
            }
	}

	// Save index to a LAR_HEC region for unpacking
	id = atlas_id.lar_hec_exp(); 
	if (dict->find_region(id, m_lar_hec_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find lar_hec region index: id, reg "  
                    << (std::string)id << " " << m_lar_hec_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find lar_hec region index: id, reg "  
                          << (std::string)id << " " << m_lar_hec_region_index
                          << std::endl;
            }
	}

	// Save index to a LAR_FCAL region for unpacking
	id = atlas_id.lar_fcal_exp(); 
	if (dict->find_region(id, m_lar_fcal_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find lar_fcal region index: id, reg "  
                    << (std::string)id << " " << m_lar_fcal_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find lar_fcal region index: id, reg "  
                          << (std::string)id << " " << m_lar_fcal_region_index
                          << std::endl;
            }
	}
    }
    
    // Get Calorimetry dictionary for both LVL1 and Dead material
    dict = dict_mgr.find_dictionary ("Calorimeter"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access Calorimeter dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access Calorimeter dictionary "
                      << std::endl;
        }
    }
    else {
	
	// Save index to a LVL1 region for unpacking
	IdDictRegion* reg = dict->find_region("Lvl1_0");
	if (reg) {
	    m_lvl1_region_index = reg->m_index;
	}
	else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find lvl1 region "  
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find lvl1 region "  
                          << std::endl;
            }
	}
    
	// Save index to a Dead Material region for unpacking
	reg = dict->find_region("DM_4_1_0_0");
	if (reg) {
	    m_dm_region_index = reg->m_index;
	}
	else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find dead material region "  
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find dead material region "  
                          << std::endl;
            }
	}
    }
    
    dict = dict_mgr.find_dictionary ("TileCalorimeter"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access TileCalorimeter dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access TileCalorimeter dictionary "
                      << std::endl;
        }
    }
    else {
	
	// Save index to a TILE region for unpacking
	id = atlas_id.tile_exp(); 
	if (dict->find_region(id, m_tile_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find tile region index: id, reg "  
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find tile region index: id, reg "  
                          << (std::string)id << " " << m_tile_region_index
                          << std::endl;
            }
	}
    }
    
    dict = dict_mgr.find_dictionary ("MuonSpectrometer"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access MuonSpectrometer dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access MuonSpectrometer dictionary "
                      << std::endl;
        }
    }
    else {
	
	m_station_field = dict->find_field ("stationName");
	if(!m_station_field) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - cannot access stationName field "
                    << endmsg;
            }
            else {
                std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access stationName field "
                          << std::endl;
            }
	}
	else {
	    m_muon_station_index = m_station_field->m_index;
	}
    

	// Save index to a MDT region for unpacking
	IdDictGroup* group = dict->find_group ("mdt");
	if (group) {
	    const std::vector<IdDictRegion*>&    regions = group->regions();
	    if (regions.size() > 0) {
		m_mdt_region_index = regions[0]->m_index;
	    }
	}
    

	if (UNDEFINED == m_mdt_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find mdt region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find mdt region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}

	// Save index to a CSC region for unpacking
	group = dict->find_group ("csc");
	if (group) {
	    if (group->regions().size() > 0) {
		m_csc_region_index = group->regions()[0]->m_index;
	    }
	}
	if (UNDEFINED == m_csc_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::DEBUG << "initialize_from_dictionary - unable to find csc region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find csc region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}

	// Save index to a RPC region for unpacking
	group = dict->find_group ("rpc");
	if (group) {
	    if (group->regions().size() > 0) {
		m_rpc_region_index = group->regions()[0]->m_index;
	    }
	}
	if (UNDEFINED == m_rpc_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find rpc region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find rpc region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}

	// Save index to a TGC region for unpacking
	group = dict->find_group ("tgc");
	if (group) {
	    if (group->regions().size() > 0) {
		m_tgc_region_index = group->regions()[0]->m_index;
	    }
	}
	if (UNDEFINED == m_tgc_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find tgc region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find tgc region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}

	// Save index to a MM region for unpacking
	group = dict->find_group ("mm");
	if (group) {
	    if (group->regions().size() > 0) {
		m_mm_region_index = group->regions()[0]->m_index;
	    }
	}
	if (UNDEFINED == m_mm_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
	    if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::DEBUG << "initialize_from_dictionary - unable to find mm region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find mm region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}

	// Save index to a sTGC region for unpacking
	group = dict->find_group ("stgc");
	if (group) {
	    if (group->regions().size() > 0) {
		m_stgc_region_index = group->regions()[0]->m_index;
	    }
	}
	if (UNDEFINED == m_stgc_region_index) {
	    int size = 0;
	    if (group) {
	        size = group->regions().size();
	    }
	    if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::DEBUG << "initialize_from_dictionary - unable to find stgc region index: group, region size "  
                    << group << " " << size
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find stgc region index: group, region size "  
                          << group << " " << size
                          << std::endl;
            }
	}
    }


    dict = dict_mgr.find_dictionary ("ForwardDetectors"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access ForwardDetectors dictionary "
                << endmsg;
        }
        else {
            std::cout << " AtlasDetectorIDHelper::initialize_from_dictionary - Warning: cannot access ForwardDetectors dictionary "
                      << std::endl;
        }
    }
    else {
	
	// Save index to a ALFA region for unpacking
	id = atlas_id.alfa_exp(); 
	if (dict->find_region(id, m_alfa_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find alfa region index: id, reg "  
                    << (std::string)id << " " << m_alfa_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find alfa region index: id, reg "  
                          << (std::string)id << " " << m_alfa_region_index
                          << std::endl;
            }
	}

	// Save index to a BCM region for unpacking
	id = atlas_id.bcm_exp(); 
	if (dict->find_region(id, m_bcm_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find bcm region index: id, reg "  
                    << (std::string)id << " " << m_bcm_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find bcm region index: id, reg "  
                          << (std::string)id << " " << m_bcm_region_index
                          << std::endl;
            }
	}

	// Save index to a LUCID region for unpacking
	id = atlas_id.lucid_exp(); 
	if (dict->find_region(id, m_lucid_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find lucid region index: id, reg "  
                    << (std::string)id << " " << m_lucid_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find lucid region index: id, reg "  
                          << (std::string)id << " " << m_lucid_region_index
                          << std::endl;
            }
	}

	// Save index to a ZDC region for unpacking
	id = atlas_id.zdc_exp(); 
	if (dict->find_region(id, m_zdc_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "AtlasDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find zdc region index: id, reg "  
                    << (std::string)id << " " << m_zdc_region_index
                    << endmsg;
            }
            else {
                std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary - Warning: unable to find zdc region index: id, reg "  
                          << (std::string)id << " " << m_zdc_region_index
                          << std::endl;
            }
	}
    }
    
//      std::cout << "AtlasDetectorIDHelper::initialize_from_dictionary " << std::endl;
//      std::cout << " pixel_region_index     " << m_pixel_region_index      << std::endl;
//      std::cout << " sct_region_index       " << m_sct_region_index        << std::endl;
//      std::cout << " trt_region_index       " << m_trt_region_index        << std::endl;
//      std::cout << " lar_em_region_index    " << m_lar_em_region_index     << std::endl;
//      std::cout << " lar_hec_region_index   " << m_lar_hec_region_index    << std::endl;
//      std::cout << " lar_fcal_region_index  " << m_lar_fcal_region_index   << std::endl;
//      std::cout << " lvl1_region_index      " << m_lvl1_region_index       << std::endl;
//      std::cout << " tile_region_index      " << m_tile_region_index       << std::endl;
//      std::cout << " mdt_region_index       " << m_mdt_region_index        << std::endl;
//      std::cout << " csc_region_index       " << m_csc_region_index        << std::endl;
//      std::cout << " rpc_region_index       " << m_rpc_region_index        << std::endl;
//      std::cout << " tgc_region_index	  " << m_tgc_region_index        << std::endl;
//      std::cout << " muon_station_index	  " << m_muon_station_index      << std::endl;

    return (0);
}

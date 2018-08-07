/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "ZdcIdentifier/ZdcID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"  
#include <set>
#include <iostream>

//______________________________________________________
ZdcID::ZdcID(void) {
  m_dict = 0;
} 
//______________________________________________________
ZdcID::~ZdcID(void){

}

int
ZdcID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    MsgStream log(m_msgSvc, "ZdcID");
    log << MSG::INFO << "Initialize from dictionary" << endmsg;
  
    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
        return (0);
    }
    else {
        if (m_msgSvc) {
            log << MSG::DEBUG << "(Re)initialize" << endmsg;
        }
        else {
            std::cout  << " DEBUG (Re)initialize" << std::endl;
        }
    }

    // init base object
    if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

    // Register version of ForwardDetectors dictionary 
    if (register_dict_tag(dict_mgr, "ForwardDetectors")) return(1);

    m_dict = dict_mgr.find_dictionary ("ForwardDetectors"); 
    if(!m_dict) {
        log << MSG::ERROR << " ZdcID::initialize_from_dict - cannot access ForwardDetectors dictionary " << endmsg;
        return 1;
    }

    // Initialize the field indices
    if(initLevelsFromDict()) return (1);


    //
    // Build multirange for the valid set of identifiers
    //


    // Find value for the field ForwardDetectors 
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int fwdField   = -1;
    if (atlasDict->get_label_value("subdet", "ForwardDetectors", fwdField)) {
        log << MSG::ERROR << "Could not get value for label 'ForwardDetectors' of field 'subdet' in dictionary " 
            << atlasDict->m_name
            << endmsg;
        return (1);
    }

    // Find value for the field ZDC
    int zdcField   = -1;
    if (m_dict->get_label_value("part", "ZDC", zdcField)) {
        log << MSG::ERROR << "Could not get value for label 'ZDC' of field 'part' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    if (m_msgSvc) {
        log << MSG::DEBUG << " ZdcID::initialize_from_dict " 
            << "Found field values: ZDC "  
            << zdcField
            << std::endl;
    }
    else {
        std::cout << " DEBUG ZdcID::initialize_from_dict " 
                  << "Found field values: ZDC "  
                  << zdcField
                  << std::endl;
    }
    
    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(fwdField);
    region_id.add(zdcField);
    Range prefix;
    m_full_module_range  = m_dict->build_multirange(region_id, prefix, "module");
    m_full_channel_range = m_dict->build_multirange(region_id, prefix);

    if (m_msgSvc) {
        log << MSG::INFO << " ZdcID::initialize_from_dict "  << endmsg;
        log << MSG::DEBUG
            << "Module  range -> " << (std::string)m_full_module_range
            << endmsg;
        log << MSG::DEBUG
            << "Channel range -> " << (std::string)m_full_channel_range
            << endmsg;
    }
    else {
        std::cout << " INFO ZdcID::initialize_from_dict "  << std::endl;
        std::cout << " DEBUG module  range -> " << (std::string)m_full_module_range
                  << std::endl;
        std::cout << " DEBUG Channel range -> " << (std::string)m_full_channel_range
                  << std::endl;
    }

    // Setup the hash tables
    if(init_hashes()) return (1);
    
    return 0;
}



int     
ZdcID::initLevelsFromDict(void)
{
    MsgStream log(m_msgSvc, "ZdcID");
    if(!m_dict) {
        log << MSG::ERROR << " ZdcID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
        return (1);
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/level.

    m_zdc_region_index          = 999;
    m_FORWARD_INDEX             = 999;
    m_ZDC_INDEX                 = 999;
    m_SIDE_INDEX                = 999;
    m_MODULE_INDEX              = 999;
    m_TYPE_INDEX                = 999;
    m_CHANNEL_INDEX             = 999;

    // Save index to a ZDC region for unpacking
    ExpandedIdentifier id; 
    id << forward_field_value() << zdc_field_value();
    if (m_dict->find_region(id, m_zdc_region_index)) {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find zdc region index: id, reg "  
            << (std::string)id << " " << m_zdc_region_index
            << endmsg;
        return (1);
    }

    // Find a ZDC region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_FORWARD_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find 'subdet' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("part");
    if (field) {
        m_ZDC_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find 'part' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("side");
    if (field) {
        m_SIDE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find 'side' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("module");
    if (field) {
        m_MODULE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find 'module' field "   << endmsg;
        return (1);
    }
    field = m_dict->find_field("type");
    if (field) {
        m_TYPE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR<< "ZdcID::initLevelsFromDict - unable to find 'type' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("channel");
    if (field) {
        m_CHANNEL_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "ZdcID::initLevelsFromDict - unable to find 'channel' field " << endmsg;        
        return (1);
    }
    
    // Set the field implementations: for bec, lay/disk, eta/phi mod

    const IdDictRegion& region = *m_dict->m_regions[m_zdc_region_index];

    m_forward_impl  = region.m_implementation[m_FORWARD_INDEX]; 
    m_zdc_impl      = region.m_implementation[m_ZDC_INDEX]; 
    m_side_impl     = region.m_implementation[m_SIDE_INDEX]; 
    m_module_impl   = region.m_implementation[m_MODULE_INDEX]; 
    m_type_impl     = region.m_implementation[m_TYPE_INDEX]; 
    m_channel_impl  = region.m_implementation[m_CHANNEL_INDEX]; 

    if (m_msgSvc) {
        log << MSG::DEBUG << "decode index and bit fields for each level: "  << endmsg;
        log << MSG::DEBUG << "forward  "  << m_forward_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << "zdc      "  << m_zdc_impl.show_to_string()     << endmsg; 
        log << MSG::DEBUG << "side     "  << m_side_impl.show_to_string()    << endmsg; 
        log << MSG::DEBUG << "module   "  << m_module_impl.show_to_string()  << endmsg; 
        log << MSG::DEBUG << "type     "  << m_type_impl.show_to_string()    << endmsg; 
        log << MSG::DEBUG << "channel  "  << m_channel_impl.show_to_string() << endmsg; 
    }
    else {
        std::cout << " DEBUG decode index and bit fields for each level: "  << std::endl;
        std::cout << " DEBUG forward  "  << m_forward_impl.show_to_string() << std::endl;
        std::cout << " DEBUG zdc      "  << m_zdc_impl.show_to_string()     << std::endl; 
        std::cout << " DEBUG side     "  << m_side_impl.show_to_string()    << std::endl; 
        std::cout << " DEBUG module   "  << m_module_impl.show_to_string()  << std::endl; 
        std::cout << " DEBUG type     "  << m_type_impl.show_to_string()    << std::endl; 
        std::cout << " DEBUG channel  "  << m_channel_impl.show_to_string() << std::endl; 
    }
    
    return (0);

}


int
ZdcID::init_hashes(void)
{
    //
    // create a vector(s) to retrieve the hashes for compact ids.
    //
    MsgStream log(m_msgSvc, "ZDC_ID");


    // module hash
    m_module_hash_max = m_full_module_range.cardinality();
    m_module_vec.resize(m_module_hash_max);
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
	const Range& range = m_full_module_range[i];
	Range::const_identifier_factory first = range.factory_begin();
	Range::const_identifier_factory last  = range.factory_end();
	for (; first != last; ++first) {
	    const ExpandedIdentifier& exp_id = (*first);
	    Identifier id = module_id(exp_id[m_SIDE_INDEX],
                                      exp_id[m_MODULE_INDEX]);
	    if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " ZDC_ID::init_hashes "
                    << " Error: duplicated id for module id. nid " << nids
                    << " compact id " << MSG::hex << id.get_compact() << MSG::dec
                    << " id " << (std::string)exp_id << endmsg;
		return (1);
	    }
	    nids++;
	}
    }
    if(ids.size() != m_module_hash_max) {
        log << MSG::ERROR << " ZDC_ID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_module_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_module_vec.size(); ++first) {
	m_module_vec[nids] = (*first);
	nids++;
    }

    // channel hash
    m_channel_hash_max = m_full_channel_range.cardinality();
    m_channel_vec.resize(m_channel_hash_max);
    nids = 0;
    ids.clear();
    for (unsigned int i = 0; i < m_full_channel_range.size(); ++i) {
	const Range& range = m_full_channel_range[i];
	Range::const_identifier_factory first = range.factory_begin();
	Range::const_identifier_factory last  = range.factory_end();
	for (; first != last; ++first) {
	    const ExpandedIdentifier& exp_id = (*first);
	    Identifier id = channel_id(exp_id[m_SIDE_INDEX],
                                       exp_id[m_MODULE_INDEX], 
                                       exp_id[m_TYPE_INDEX],
                                       exp_id[m_CHANNEL_INDEX]);
            
	    if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " ZDC_ID::init_hashes "
                    << " Error: duplicated id for channel id. nid " << nids
                    << " compact id " << MSG::hex << id.get_compact() << MSG::dec
                    << " id " << (std::string)exp_id << endmsg;
		return (1);
	    }
	    nids++;
	}
    }
    if(ids.size() != m_channel_hash_max) {
        log << MSG::ERROR << " ZDC_ID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_channel_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    first = ids.begin();
    last  = ids.end();
    for (; first != last && nids < m_channel_vec.size(); ++first) {
	m_channel_vec[nids] = (*first);
	nids++;
    }
    return (0);
}

void   
ZdcID::test_packing    (void) const
{
    MsgStream log(m_msgSvc, "ZDC_ID");

    if (m_dict) {
	
	int nids = 0;
	int nerr = 0;
        ExpandedIdentifier id;
	const_id_iterator first = m_module_vec.begin();
	const_id_iterator last  = m_module_vec.end();
	for (; first != last; ++first, ++nids) {
	    Identifier id = (*first);
	    Identifier new_id = module_id(side(id),
                                          module(id));
	    if (id != new_id) {
                log << MSG::ERROR << "ZDC_ID::test_packing: new and old compacts not equal. New/old/expanded ids " 
                    << show_to_string(id) << " " << show_to_string(new_id) << endmsg;
		nerr++;
		continue;
	    }
            IdentifierHash idHash = module_hash(id);
	    Identifier new_id_1   = module_id(idHash);
	    if (id != new_id_1) {
                log << MSG::ERROR << "ZDC_ID::test_packing: new and old compacts from hash not equal. New/old/expanded ids. hash " << idHash << " " 
                    << show_to_string(id) << " " << show_to_string(new_id_1) << endmsg;
		nerr++;
		continue;
	    }

            // std::cout << "module ids: " << idHash << " " << std::hex << id.get_compact() << std::dec << " " << new_id_1.get_compact() << " " << show_to_string(id) << " " << show_to_string(new_id) << " " << show_to_string(new_id_1) << std::endl;

	}

	if (m_msgSvc) {	
	    log << MSG::DEBUG << "ZDC_ID::test_packing: tested module ids. nids, errors " 
		<< nids << " " << nerr << endmsg;
	}
	else {
	    std::cout << " DEBUG ZDC_ID::test_packing: tested module ids. nids, errors " 
		      << nids << " " << nerr << std::endl;
	}

        first = m_channel_vec.begin();
        last  = m_channel_vec.end();
	for (; first != last; ++first, ++nids) {
	    Identifier id = (*first);
	    Identifier new_id = channel_id(side(id),
                                           module(id),
                                           type(id),
                                           channel(id));
	    if (id != new_id) {
                log << MSG::ERROR << "ZDC_ID::test_packing: new and old compacts not equal. New/old/expanded ids " 
                    << show_to_string(id) << " " << show_to_string(new_id) << endmsg;
		nerr++;
		continue;
	    }
            IdentifierHash idHash = channel_hash(id);
	    Identifier new_id_1   = channel_id(idHash);
	    if (id != new_id_1) {
                log << MSG::ERROR << "ZDC_ID::test_packing: new and old compacts from hash not equal. New/old/expanded ids hash " << idHash << " " 
                    << show_to_string(id) << " " << show_to_string(new_id_1) << endmsg;
		nerr++;
		continue;
	    }

            // std::cout << "channel ids: " << idHash << " " << show_to_string(id) << " " << show_to_string(new_id) << " " << show_to_string(new_id_1) << std::endl;


	}

	if (m_msgSvc) {	
	    log << MSG::DEBUG << "ZDC_ID::test_packing: tested channel ids. nids, errors " 
		<< nids << " " << nerr << endmsg;
	}
	else {
	    std::cout << " DEBUG ZDC_ID::test_packing: tested channel ids. nids, errors " 
		      << nids << " " << nerr << std::endl;
	}
	
	if (m_msgSvc) {
	    log << MSG::DEBUG << "ZDC_ID::test_packing: Successful tested " 
		<< nids << " ids. " 
		<< endmsg;
	}
	else {
	    std::cout << " DEBUG ZDC_ID::test_packing: Successful tested " 
		      << nids << " ids. " 
		      << std::endl;
	}
    }
    else {
        log << MSG::ERROR << "ZDC_ID::test_packing: Unable to test module/channel is packing - no dictionary has been defined. " 
            << endmsg;
    }
}

ZdcID::const_id_iterator   ZdcID::modules_begin (void) const
{
    return (m_module_vec.begin());
}

ZdcID::const_id_iterator   ZdcID::modules_end   (void) const
{
    return (m_module_vec.end());
}

ZdcID::const_id_iterator   ZdcID::channels_begin (void) const
{
    return (m_channel_vec.begin());
}

ZdcID::const_id_iterator   ZdcID::channels_end   (void) const
{
    return (m_channel_vec.end());
}




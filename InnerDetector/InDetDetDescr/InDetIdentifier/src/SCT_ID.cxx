/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Inner Detector identifier package
 -------------------------------------------
***************************************************************************/

//<doc><file>   $Id: SCT_ID.cxx,v 1.43.2.1 2009-03-26 21:54:31 jtseng Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"  
#include <set>
#include <algorithm>
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


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


SCT_ID::SCT_ID(void)
        :
        m_sct_region_index(0),
        m_INDET_INDEX(0),
        m_SCT_INDEX(1),
        m_BARREL_EC_INDEX(2),
        m_LAYER_DISK_INDEX(3),
        m_PHI_MODULE_INDEX(4),
        m_ETA_MODULE_INDEX(5),
        m_SIDE_INDEX(6),
        m_STRIP_INDEX(7),
        m_ROW_INDEX(999),
        m_dict(0),
        m_wafer_hash_max(0),
        m_strip_hash_max(0),
        m_barrel_field(0),
        m_hasRows(false)
{
}

void
SCT_ID::wafer_id_checks ( int barrel_ec,  
                          int layer_disk, 
                          int phi_module, 
                          int eta_module, 
                          int side ) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << indet_field_value() << sct_field_value()
       << barrel_ec << layer_disk << phi_module << eta_module << side;

    if (!m_full_wafer_range.match(id)) {  // module range check is sufficient
        MsgStream log(m_msgSvc, "SCT_ID");
        log << MSG::ERROR << " SCT_ID::wafer_id result is NOT ok. ID, range "
            << (std::string)id <<  " " << (std::string)m_full_wafer_range << endmsg;
    }
}

void
SCT_ID::strip_id_checks ( int barrel_ec,  
                          int layer_disk, 
                          int phi_module, 
                          int eta_module, 
                          int side,        
                          int strip) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << indet_field_value() << sct_field_value()
       << barrel_ec << layer_disk << phi_module << eta_module << side << strip;

    if (!m_full_strip_range.match(id)) {  
        MsgStream log(m_msgSvc, "SCT_ID");
        log << MSG::ERROR << " SCT_ID::strip_id result is NOT ok. ID, range "
            << (std::string)id << " " << (std::string)m_full_strip_range << std::endl;
    }
}


void
SCT_ID::strip_id_checks ( int barrel_ec,
    			  int layer_disk,
			  int phi_module,
			  int eta_module,
			  int side,
			  int row,
			  int strip) const
{

	// Check that id is within allowed range
      
     	// Fill expanded id
      	ExpandedIdentifier id;
	id << indet_field_value() << sct_field_value()
	  			  << barrel_ec << layer_disk << phi_module << eta_module << side << row << strip;
      
      
	if (!m_full_strip_range.match(id)) {
		MsgStream log(m_msgSvc, "SCT_ID");
		log << MSG::ERROR << " SCT_ID::strip_id result is NOT ok. ID, range "
		  << (std::string)id << " " << (std::string)m_full_strip_range << std::endl;
	}
}




int 
SCT_ID::layer_disk_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& layer_disk_field = range[m_LAYER_DISK_INDEX];
            if (layer_disk_field.has_maximum()) {
                return (layer_disk_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int 
SCT_ID::eta_module_min(const Identifier& id) const
{
    // get min from dictionary - note that eta modules skip 0 for
    // sct, so we must search for absolute min
    ExpandedIdentifier expId;
    IdContext layer_context(expId, 0, m_LAYER_DISK_INDEX);
    get_expanded_id(id, expId, &layer_context);
    int result = -999;
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& eta_field = range[m_ETA_MODULE_INDEX];
            if (eta_field.has_minimum()) {
                int etamin = eta_field.get_minimum();
                if (-999 == result) {
                    result = etamin;
                }
                else {
                    if (etamin < result) result = etamin;
                }
            }
        }
    }
    return (result);
}

int 
SCT_ID::eta_module_max(const Identifier& id) const
{
    // get max from dictionary - note that eta modules skip 0 for
    // sct, so we must search for absolute max
    ExpandedIdentifier expId;
    IdContext layer_context(expId, 0, m_LAYER_DISK_INDEX);
    get_expanded_id(id, expId, &layer_context);
    int result = -999;
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& eta_field = range[m_ETA_MODULE_INDEX];
            if (eta_field.has_maximum()) {
                int etamax = eta_field.get_maximum();
                if (result < etamax) result = etamax;
            }
        }
    }
    return (result);
}

int     
SCT_ID::strip_max       (const Identifier& id) const
{
    // get max from dictionary - note that eta modules skip 0 for
    // sct, so we must search for absolute max
    ExpandedIdentifier expId;
    IdContext layer_context(expId, 0, m_LAYER_DISK_INDEX);
    get_expanded_id(id, expId, &layer_context);
    int result = -999;
    for (unsigned int i = 0; i < m_full_strip_range.size(); ++i) {
        const Range& range = m_full_strip_range[i];
        if (range.match(expId)) {
            const Range::field& strip_field = range[m_STRIP_INDEX];
            if (strip_field.has_maximum()) {
                int strip = strip_field.get_maximum();
                if (result < strip) result = strip;
            }
        }
    }
    return (result);
}


bool    
SCT_ID::is_eta_module_min       (const Identifier& id) const 
{
    // get min from dictionary
    return (eta_module(id) == eta_module_min(id));
}

bool    
SCT_ID::is_eta_module_max       (const Identifier& id)  const
{
    // get max from dictionary
    return (eta_module(id) == eta_module_max(id));
}

int 
SCT_ID::phi_module_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext wafer_context1 = wafer_context();
    get_expanded_id(id, expId, &wafer_context1);
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        if (range.match(expId)) {
            const Range::field& phi_field = range[m_PHI_MODULE_INDEX];
            if (phi_field.has_maximum()) {
                return (phi_field.get_maximum());
            }
        }
    }
    return -1;
}

bool    
SCT_ID::is_phi_module_max(const Identifier& id) const
{
    // get max from dictionary
    return (phi_module(id) == phi_module_max(id));
}

int
SCT_ID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    MsgStream log(m_msgSvc, "SCT_ID");
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

    // Register version of InnerDetector dictionary 
    if (register_dict_tag(dict_mgr, "InnerDetector")) return(1);

    m_dict = dict_mgr.find_dictionary ("InnerDetector"); 
    if(!m_dict) {
        log << MSG::ERROR << " SCT_ID::initialize_from_dict - cannot access InnerDetector dictionary " << endmsg;
        return 1;
    }

    // Initialize the field indices
    if(initLevelsFromDict()) return (1);

    //
    // Set barrel field for testing is_barrel
    //
    int barrel_value;
    if (m_dict->get_label_value("barrel_endcap", "barrel", barrel_value)) {
        log << MSG::ERROR << "Could not get value for label 'barrel' of field 'barrel_endcap' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    m_barrel_field.clear();
    m_barrel_field.add_value(barrel_value);
    if (m_msgSvc) {
        log << MSG::DEBUG << " SCT_ID::initialize_from_dict " 
            << "Set barrel field values: "  
            << (std::string)m_barrel_field
            << endmsg;
    }
    else {
        std::cout << " DEBUG SCT_ID::initialize_from_dict " 
                  << "Set barrel field values: "  
                  << (std::string)m_barrel_field
                  << std::endl;
    }
    

    //
    // Build multirange for the valid set of identifiers
    //


    // Find value for the field InnerDetector
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int inDetField   = -1;
    if (atlasDict->get_label_value("subdet", "InnerDetector", inDetField)) {
        log << MSG::ERROR << "Could not get value for label 'InnerDetector' of field 'subdet' in dictionary " 
            << atlasDict->m_name
            << endmsg;
        return (1);
    }

    // Find value for the field SCT
    int sctField   = -1;
    if (m_dict->get_label_value("part", "SCT", sctField)) {
        log << MSG::ERROR << "Could not get value for label 'SCT' of field 'part' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    if (m_msgSvc) {
        log << MSG::DEBUG << " SCT_ID::initialize_from_dict " 
            << "Found field values: SCT "  
            << sctField
            << std::endl;
    }
    else {
        std::cout << " DEBUG SCT_ID::initialize_from_dict " 
                  << "Found field values: SCT "  
                  << sctField
                  << std::endl;
    }
    
    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(inDetField);
    region_id.add(sctField);
    Range prefix;
    m_full_wafer_range = m_dict->build_multirange(region_id, prefix, "side");
    m_full_strip_range = m_dict->build_multirange(region_id, prefix);

    // Setup the hash tables
    if(init_hashes()) return (1);

    // Setup hash tables for finding neighbors
    if(init_neighbors()) return (1);
    
    if (m_msgSvc) {
        log << MSG::INFO << " SCT_ID::initialize_from_dict "  << endmsg;
        log << MSG::DEBUG  
            << "Wafer range -> " << (std::string)m_full_wafer_range
            <<   endmsg;
        log << MSG::DEBUG
            << "Strip range -> " << (std::string)m_full_strip_range
            << endmsg;
    }
    else {
        std::cout << " INFO SCT_ID::initialize_from_dict "  << std::endl;
        std::cout << " DEBUG  Wafer range -> " << (std::string)m_full_wafer_range
                  <<   std::endl;
        std::cout << " DEBUG Strip range -> " << (std::string)m_full_strip_range
                  << std::endl;
    }
    
    return 0;
}

int
SCT_ID::init_hashes(void)
{

    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for wafers but NOT for strips
    // (too many)
    //
    MsgStream log(m_msgSvc, "SCT_ID");
    // wafer hash
    m_wafer_hash_max = m_full_wafer_range.cardinality();
    m_wafer_vec.resize(m_wafer_hash_max);
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            Identifier id = wafer_id(exp_id[m_BARREL_EC_INDEX],
                                     exp_id[m_LAYER_DISK_INDEX], 
                                     exp_id[m_PHI_MODULE_INDEX],
                                     exp_id[m_ETA_MODULE_INDEX],
                                     exp_id[m_SIDE_INDEX]);
            if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " SCT_ID::init_hashes "
                    << " Error: duplicated id for wafer id. nid " << nids
                    << " compact id " << id.getString()
                    << " id " << (std::string)exp_id << endmsg;
                return (1);
            }
            nids++;
        }
    }
    if(ids.size() != m_wafer_hash_max) {
        log << MSG::ERROR << " SCT_ID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_wafer_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_wafer_vec.size(); ++first) {
        m_wafer_vec[nids] = (*first);
        nids++;
    }

    // strip hash - we do not keep a vec for the strips - too large
    m_strip_hash_max = m_full_strip_range.cardinality();

    return (0);
}

int             
SCT_ID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
{
    unsigned short index = id;
    if (index < m_prev_phi_wafer_vec.size()) {
        if (m_prev_phi_wafer_vec[index] == NOT_VALID_HASH) return (1);
        prev =  m_prev_phi_wafer_vec[index];
        return (0);
    }
    return (1);
}

int             
SCT_ID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
{
    unsigned short index = id;
    if (index < m_next_phi_wafer_vec.size()) {
        if (m_next_phi_wafer_vec[index] == NOT_VALID_HASH) return (1);
        next =  m_next_phi_wafer_vec[index];
        return (0);
    }
    return (1);
}

int             
SCT_ID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
{
    unsigned short index = id;
    if (index < m_prev_eta_wafer_vec.size()) {
        if (m_prev_eta_wafer_vec[index] == NOT_VALID_HASH) return (1);
        prev =  m_prev_eta_wafer_vec[index];
        return (0);
    }
    return (1);
}

int
SCT_ID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
{
    unsigned short index = id;
    if (index < m_next_eta_wafer_vec.size()) {
        if (m_next_eta_wafer_vec[index] == NOT_VALID_HASH) return (1);
        next =  m_next_eta_wafer_vec[index];
        return (0);
    }
    return (1);
}

int
SCT_ID::get_other_side  (const IdentifierHash& hashId, IdentifierHash& other) const
{
    if (m_dict) {
        // get max from dictionary
        Identifier id;
        IdContext wafer_context1 = wafer_context();
        if(!get_id(hashId, id, &wafer_context1)) {
            other = side(id) ? hashId - 1 : hashId + 1;
            return (0);
        }
    }
    return (1);
}

int
SCT_ID::init_neighbors(void)
{
    //
    // create a vector(s) to retrieve the hashes for compact ids for
    // wafer neighbors.
    //

    MsgStream log(m_msgSvc, "SCT_ID");
    if (m_msgSvc) {
        log << MSG::DEBUG << "SCT_ID::init_neighbors " << endmsg;
    }
    else {
        std::cout << " DEBUG SCT_ID::init_neighbors " << std::endl;
    }
  

    m_prev_phi_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    m_next_phi_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    m_prev_eta_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    m_next_eta_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);

    for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
        const Range& range = m_full_wafer_range[i];
        const Range::field& phi_field = range[m_PHI_MODULE_INDEX];
        const Range::field& eta_field = range[m_ETA_MODULE_INDEX];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            ExpandedIdentifier::element_type previous_phi;
            ExpandedIdentifier::element_type next_phi;
            ExpandedIdentifier::element_type previous_eta;
            ExpandedIdentifier::element_type next_eta;
            bool pphi = phi_field.get_previous(exp_id[m_PHI_MODULE_INDEX], previous_phi);
            bool nphi = phi_field.get_next    (exp_id[m_PHI_MODULE_INDEX], next_phi);
            bool peta = eta_field.get_previous(exp_id[m_ETA_MODULE_INDEX], previous_eta);
            bool neta = eta_field.get_next    (exp_id[m_ETA_MODULE_INDEX], next_eta);

            IdContext      wcontext = wafer_context();
            
            // First get primary hash id
            IdentifierHash hash_id;
            Identifier id = wafer_id(exp_id[m_BARREL_EC_INDEX],
                                     exp_id[m_LAYER_DISK_INDEX], 
                                     exp_id[m_PHI_MODULE_INDEX],
                                     exp_id[m_ETA_MODULE_INDEX],
                                     exp_id[m_SIDE_INDEX]);
            if (get_hash(id, hash_id, &wcontext)) {
                log << MSG::ERROR << " SCT_ID::init_neighbors - unable to get hash, exp/compact "
                    << show_to_string(id, &wcontext)
                    << " " << (std::string)m_full_wafer_range << endmsg;
                return (1);
            }

            // index for the subsequent arrays
            unsigned short index = hash_id;
            assert (hash_id < m_prev_phi_wafer_vec.size());
            assert (hash_id < m_next_phi_wafer_vec.size());
            assert (hash_id < m_prev_eta_wafer_vec.size());
            assert (hash_id < m_next_eta_wafer_vec.size());
            
            if (pphi) {
                // Get previous phi hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_PHI_MODULE_INDEX] = previous_phi;
                Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
                                         expId[m_LAYER_DISK_INDEX], 
                                         expId[m_PHI_MODULE_INDEX],
                                         expId[m_ETA_MODULE_INDEX],
                                         expId[m_SIDE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " SCT_ID::init_neighbors - unable to get previous phi hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_prev_phi_wafer_vec[index] = hash_id;
            }
            
            if (nphi) {
                // Get next phi hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_PHI_MODULE_INDEX] = next_phi;
                Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
                                         expId[m_LAYER_DISK_INDEX], 
                                         expId[m_PHI_MODULE_INDEX],
                                         expId[m_ETA_MODULE_INDEX],
                                         expId[m_SIDE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " SCT_ID::init_neighbors - unable to get next phi hash, exp/compact " << id.getString() << 
                        " " << MSG::hex << id.getString() << MSG::dec << endmsg;
                    return (1);
                }
                m_next_phi_wafer_vec[index] = hash_id;
            }
            
            if (peta) {
                // Get previous eta hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_ETA_MODULE_INDEX] = previous_eta;
                Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
                                         expId[m_LAYER_DISK_INDEX], 
                                         expId[m_PHI_MODULE_INDEX],
                                         expId[m_ETA_MODULE_INDEX],
                                         expId[m_SIDE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " SCT_ID::init_neighbors - unable to get previous eta hash, exp/compact " << id.getString() 
                        << " " << std::endl;
                    return (1);
                }
                m_prev_eta_wafer_vec[index] = hash_id;
            }
            
            if (neta) {
                // Get next eta hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_ETA_MODULE_INDEX] = next_eta;
                Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
                                         expId[m_LAYER_DISK_INDEX], 
                                         expId[m_PHI_MODULE_INDEX],
                                         expId[m_ETA_MODULE_INDEX],
                                         expId[m_SIDE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " SCT_ID::init_neighbors - unable to get next eta hash, exp/compact " << id.getString() 
                        << " " << endmsg;
                    return (1);
                }
                m_next_eta_wafer_vec[index] = hash_id;
            }
            

//          std::cout << " SCT_ID::init_neighbors "
//                    << " phi, previous, next " << id[m_PHI_MODULE_INDEX]
//                    << " " << pphi
//                    << " " << previous_phi
//                    << " " << nphi
//                    << " " << next_phi
//                    << " eta, previous, next " << id[m_ETA_MODULE_INDEX]
//                    << " " << peta
//                    << " " << previous_eta
//                    << " " << neta
//                    << " " << next_eta
//                    << " id " << (std::string)(*first) 
//                    << std::endl;
        }
    }
    return (0);
}



int     
SCT_ID::initLevelsFromDict()
{


    MsgStream log(m_msgSvc, "SCT_ID");
    if(!m_dict) {
        log << MSG::ERROR << " SCT_ID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
        return (1);
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    m_INDET_INDEX               = 999;
    m_SCT_INDEX                 = 999;
    m_BARREL_EC_INDEX           = 999;
    m_LAYER_DISK_INDEX          = 999;
    m_PHI_MODULE_INDEX          = 999;
    m_ETA_MODULE_INDEX          = 999;
    m_SIDE_INDEX                = 999;
    m_ROW_INDEX			= 999;
    m_STRIP_INDEX               = 999;
    m_hasRows			= false;
    

    // Save index to a SCT region for unpacking
    ExpandedIdentifier id; 
    id << indet_field_value() << sct_field_value();
    if (m_dict->find_region(id, m_sct_region_index)) {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find sct region index: id, reg "  
            << (std::string)id << " " << m_sct_region_index
            << endmsg;
        return (1);
    }

    // Find a SCT region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_INDET_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'subdet' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("part");
    if (field) {
        m_SCT_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'part' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("barrel_endcap");
    if (field) {
        m_BARREL_EC_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'barrel_endcap' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("layer");
    if (field) {
        m_LAYER_DISK_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'layer' field "   << endmsg;
        return (1);
    }
    field = m_dict->find_field("phi_module");
    if (field) {
        m_PHI_MODULE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR<< "SCT_ID::initLevelsFromDict - unable to find 'phi_module' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("eta_module");
    if (field) {
        m_ETA_MODULE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'eta_module' field " << endmsg;       
        return (1);
    }
    field = m_dict->find_field("side");
    if (field) {
        m_SIDE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'side' field "        << endmsg;
        return (1);
    }


    field = m_dict->find_field("row");
    if (field) {
    	m_ROW_INDEX 	= field->m_index;
	m_hasRows	= true		;
    }
	    
    else {

    	//log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'row' field " << endmsg;
	//return (1);
    }


    field = m_dict->find_field("strip");
    if (field) {
        m_STRIP_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "SCT_ID::initLevelsFromDict - unable to find 'strip' field " << endmsg;    
        return (1);
    }
    
    // Set the field implementations: for bec, lay/disk, eta/phi mod

    const IdDictRegion& region = *m_dict->m_regions[m_sct_region_index];

    m_indet_impl      = region.m_implementation[m_INDET_INDEX]; 
    m_sct_impl        = region.m_implementation[m_SCT_INDEX]; 
    m_bec_impl        = region.m_implementation[m_BARREL_EC_INDEX]; 
    m_lay_disk_impl   = region.m_implementation[m_LAYER_DISK_INDEX]; 
    m_phi_mod_impl    = region.m_implementation[m_PHI_MODULE_INDEX]; 
    m_eta_mod_impl    = region.m_implementation[m_ETA_MODULE_INDEX]; 
    m_side_impl       = region.m_implementation[m_SIDE_INDEX];
    if (m_hasRows){
    	m_row_impl    = region.m_implementation[m_ROW_INDEX];
    }    	
    m_strip_impl      = region.m_implementation[m_STRIP_INDEX]; 

    if (m_msgSvc) {
        log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << "indet    "  << m_indet_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << "sct      "  << m_sct_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "bec      "  << m_bec_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "lay_disk "  << m_lay_disk_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "phi_mod  "  << m_phi_mod_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "eta_mod  "  << m_eta_mod_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "side     "  << m_side_impl.show_to_string() << endmsg; 
	if (m_hasRows){
		log << MSG::DEBUG << "row     "  << m_row_impl.show_to_string() << endmsg;
	}
        log << MSG::DEBUG << "strip    "  << m_strip_impl.show_to_string() << endmsg; 
    }
    else {
        std::cout << " DEBUG decode index and bit fields for each level: " << std::endl;
        std::cout << " DEBUG indet    "  << m_indet_impl.show_to_string() << std::endl;
        std::cout << " DEBUG sct      "  << m_sct_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG bec      "  << m_bec_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG lay_disk "  << m_lay_disk_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG phi_mod  "  << m_phi_mod_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG eta_mod  "  << m_eta_mod_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG side     "  << m_side_impl.show_to_string() << std::endl;
	if ( m_hasRows ){
		log <<" DEBUG row     "  << m_row_impl.show_to_string() << std::endl;
	}
        std::cout << " DEBUG strip    "  << m_strip_impl.show_to_string() << std::endl; 
    }
    
//      std::cout << "SCT_ID::initLevelsFromDict - found levels "       << std::endl;
//      std::cout << "subdet        "   << m_INDET_INDEX        << std::endl;
//      std::cout << "part          "   << m_SCT_INDEX          << std::endl;
//      std::cout << "barrel_endcap "   << m_BARREL_EC_INDEX    << std::endl;
//      std::cout << "layer or disk "   << m_LAYER_DISK_INDEX   << std::endl;
//      std::cout << "phi_module    "   << m_PHI_MODULE_INDEX   << std::endl;
//      std::cout << "eta_module    "   << m_ETA_MODULE_INDEX   << std::endl;
//      std::cout << "side          "   << m_SIDE_INDEX         << std::endl;
//      std::cout << "strip         "   << m_STRIP_INDEX        << std::endl;

    std::cout << "indet "  << m_indet_impl.decode_index() << " " 
              <<   (std::string)m_indet_impl.ored_field() << " " 
              << std::hex    << m_indet_impl.mask() << " " 
              << m_indet_impl.zeroing_mask() << " " 
              << std::dec    << m_indet_impl.shift() << " "
              << m_indet_impl.bits() << " "
              << m_indet_impl.bits_offset()
              << std::endl;
    std::cout << "sct"     << m_sct_impl.decode_index() << " " 
              <<   (std::string)m_sct_impl.ored_field() << " " 
              << std::hex    << m_sct_impl.mask() << " " 
              << m_sct_impl.zeroing_mask() << " " 
              << std::dec    << m_sct_impl.shift() << " "
              << m_sct_impl.bits() << " "
              << m_sct_impl.bits_offset()
              << std::endl;
    std::cout << "bec"     << m_bec_impl.decode_index() << " " 
              <<   (std::string)m_bec_impl.ored_field() << " " 
              << std::hex    << m_bec_impl.mask() << " " 
              << m_bec_impl.zeroing_mask() << " " 
              << std::dec    << m_bec_impl.shift() << " "
              << m_bec_impl.bits() << " "
              << m_bec_impl.bits_offset()
              << std::endl;
    std::cout << "lay_disk"<< m_lay_disk_impl.decode_index() << " " 
              <<   (std::string)m_lay_disk_impl.ored_field() << " " 
              << std::hex    << m_lay_disk_impl.mask() << " " 
              << m_lay_disk_impl.zeroing_mask() << " " 
              << std::dec    << m_lay_disk_impl.shift() << " "
              << m_lay_disk_impl.bits() << " "
              << m_lay_disk_impl.bits_offset()
              << std::endl;
    std::cout << "phi_mod" << m_phi_mod_impl.decode_index() << " " 
              <<   (std::string)m_phi_mod_impl.ored_field() << " " 
              << std::hex    << m_phi_mod_impl.mask() << " " 
              << m_phi_mod_impl.zeroing_mask() << " " 
              << std::dec    << m_phi_mod_impl.shift() << " "
              << m_phi_mod_impl.bits() << " "
              << m_phi_mod_impl.bits_offset()
              << std::endl;
    std::cout << "eta_mod" << m_eta_mod_impl.decode_index() << " " 
              <<   (std::string)m_eta_mod_impl.ored_field() << " " 
              << std::hex    << m_eta_mod_impl.mask() << " " 
              << m_eta_mod_impl.zeroing_mask() << " " 
              << std::dec    << m_eta_mod_impl.shift() << " "
              << m_eta_mod_impl.bits() << " "
              << m_eta_mod_impl.bits_offset()
              << std::endl;
    std::cout << "side"    << m_side_impl.decode_index() << " " 
              <<   (std::string)m_side_impl.ored_field() << " " 
              << std::hex    << m_side_impl.mask() << " " 
              << m_side_impl.zeroing_mask() << " " 
              << std::dec    << m_side_impl.shift() << " "
              << m_side_impl.bits() << " "
              << m_side_impl.bits_offset()
              << std::endl;

    if ( m_hasRows ){    
    	std::cout << "row"    << m_row_impl.decode_index() << " "
	  	  <<   (std::string)m_row_impl.ored_field() << " "
		  << std::hex    << m_row_impl.mask() << " "
		  << m_row_impl.zeroing_mask() << " "
		  << std::dec    << m_row_impl.shift() << " "
		  << m_row_impl.bits() << " "
		  << m_row_impl.bits_offset()
		  << std::endl;
    }


    std::cout << "strip"   << m_strip_impl.decode_index() << " " 
              <<   (std::string)m_strip_impl.ored_field() << " " 
              << std::hex    << m_strip_impl.mask() << " " 
              << m_strip_impl.zeroing_mask() << " " 
              << std::dec    << m_strip_impl.shift() << " "
              << m_strip_impl.bits() << " "
              << m_strip_impl.bits_offset()
              << std::endl;

    return (0);
}

SCT_ID::size_type       
SCT_ID::wafer_hash_max (void) const
{
    return m_wafer_hash_max;
}

SCT_ID::size_type       
SCT_ID::strip_hash_max (void) const
{
    return m_strip_hash_max;
}

SCT_ID::const_id_iterator       SCT_ID::wafer_begin             (void) const
{
    return (m_wafer_vec.begin());
}

SCT_ID::const_id_iterator       SCT_ID::wafer_end               (void) const
{
    return (m_wafer_vec.end());
}

SCT_ID::const_expanded_id_iterator      SCT_ID::strip_begin     (void) const
{
    return (m_full_strip_range.factory_begin());
}

SCT_ID::const_expanded_id_iterator      SCT_ID::strip_end       (void) const
{
    return (m_full_strip_range.factory_end());
}

// From hash get Identifier
int     
SCT_ID::get_id          (const IdentifierHash& hash_id,
                         Identifier& id,
                         const IdContext* context) const
{

    int result = 1;
    id.clear();

    size_t begin = (context) ? context->begin_index(): 0;
    // cannot get hash if end is 0:
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) { 
        // No hashes yet for ids with prefixes
        if (m_SIDE_INDEX == end) {
            if (hash_id < (unsigned int)(m_wafer_vec.end() - m_wafer_vec.begin())) {
                id = m_wafer_vec[hash_id];
                result = 0;
            }
        }
        else if (m_STRIP_INDEX == end) {
            // Do not know how to calculate strip id from hash yet!!
            std::cout << "Do not know how to calculate strip id from hash yet!!" << std::endl;
        }
    }
    return (result);
}

void
SCT_ID::get_expanded_id (const Identifier& id,
                         ExpandedIdentifier& exp_id,
                         const IdContext* context) const
{
    exp_id.clear();
    exp_id << indet_field_value()
           << sct_field_value()
           << barrel_ec(id)
           << layer_disk(id)
           << phi_module(id)
           << eta_module(id)
           << side(id);
    if(!context || context->end_index() == m_STRIP_INDEX) {
    	if ( m_hasRows) {
		exp_id << row(id) << strip(id);
	}
	else {
        	exp_id << strip(id);
	}
    }
}

int     
SCT_ID::get_hash        (const Identifier& id, 
                         IdentifierHash& hash_id,
                         const IdContext* context) const
{

    // Get the hash code from either a vec (for wafers) or calculate
    // it (strips). For the former, we convert to compact and call
    // get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index(): 0;
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_SIDE_INDEX  == end) {
            hash_id = wafer_hash(id);
            if (hash_id.is_valid()) result = 0;
        }
        else if (context && context->end_index() == m_STRIP_INDEX) {
            // Must calculate for strip hash
            ExpandedIdentifier new_id;
            get_expanded_id(id, new_id);
            hash_id =  m_full_strip_range.cardinalityUpTo(new_id);
            result = 0;
        }
    }
    return (result);
}


void    
SCT_ID::test_wafer_packing      (void) const
{
    MsgStream log(m_msgSvc, "SCT_ID");

    if (m_dict) {
        
        int nids = 0;
        int nerr = 0;
        IdContext context = wafer_context();
        const_id_iterator first = m_wafer_vec.begin();
        const_id_iterator last  = m_wafer_vec.end();
        for (; first != last; ++first, ++nids) {
            Identifier id = (*first);
            ExpandedIdentifier exp_id;
            get_expanded_id(id, exp_id, &context);
            Identifier new_id = wafer_id(exp_id[m_BARREL_EC_INDEX],
                                         exp_id[m_LAYER_DISK_INDEX], 
                                         exp_id[m_PHI_MODULE_INDEX],
                                         exp_id[m_ETA_MODULE_INDEX],
                                         exp_id[m_SIDE_INDEX]);
            if (id != new_id) {
                log << MSG::ERROR << "SCT_ID::test_wafer_packing: new and old compacts not equal. New/old/expanded ids " 
                    << MSG::hex << show_to_string(id) << " " << show_to_string(new_id) << " " << MSG::dec 
                    << (std::string)exp_id << endmsg;
                nerr++;
                continue;
            }
            // check wafer id
            if (!exp_id[m_SIDE_INDEX]) {
                
                Identifier new_id1 = module_id(exp_id[m_BARREL_EC_INDEX],
                                               exp_id[m_LAYER_DISK_INDEX], 
                                               exp_id[m_PHI_MODULE_INDEX],
                                               exp_id[m_ETA_MODULE_INDEX]);
                if (id != new_id1) {
                    log << MSG::ERROR << "SCT_ID::test_wafer_packing: new and old module idsnot equal. New/old/expanded ids " 
                        << MSG::hex << show_to_string(id) << " " << show_to_string(new_id1) << " " << MSG::dec 
                        << (std::string)exp_id << endmsg;
                    nerr++;
                    continue;
                }
            }
        }

        if (m_msgSvc) { 
            log << MSG::DEBUG << "SCT_ID::test_wafer_packing: tested wafer and module ids. nids, errors " 
                << nids << " " << nerr << endmsg;
        }
        else {
            std::cout << " DEBUG SCT_ID::test_wafer_packing: tested wafer and module ids. nids, errors " 
                      << nids << " " << nerr << std::endl;
        }
        
        nids = 0;
        context = strip_context();
        const_expanded_id_iterator      first_sct = strip_begin();  
        const_expanded_id_iterator      last_sct  = strip_end();
        for (; first_sct != last_sct; ++first_sct, ++nids) {
            if (nids%10000 != 1) continue;
            const ExpandedIdentifier& exp_id = *first_sct;
            ExpandedIdentifier new_exp_id;

            Identifier id = wafer_id(exp_id[m_BARREL_EC_INDEX],
                                     exp_id[m_LAYER_DISK_INDEX], 
                                     exp_id[m_PHI_MODULE_INDEX],
                                     exp_id[m_ETA_MODULE_INDEX],
                                     exp_id[m_SIDE_INDEX]);
            get_expanded_id(id, new_exp_id, &context);
            if (exp_id[0] != new_exp_id[0] ||
                exp_id[1] != new_exp_id[1] ||
                exp_id[2] != new_exp_id[2] ||
                exp_id[3] != new_exp_id[3] ||
                exp_id[4] != new_exp_id[4] ||
                exp_id[5] != new_exp_id[5] ||
                exp_id[6] != new_exp_id[6]) {
                log << MSG::ERROR << "SCT_ID::test_wafer_packing: new and old ids not equal. New/old/compact ids "
                    << (std::string)new_exp_id << " " << (std::string)exp_id
                    << " " << show_to_string(id) << endmsg;
                continue;
            }

            Identifier stripid	;
	    Identifier stripid1	;
	    if ( m_hasRows ) {	  
	   
	    	stripid = strip_id (	   exp_id[m_BARREL_EC_INDEX],
                                           exp_id[m_LAYER_DISK_INDEX], 
                                           exp_id[m_PHI_MODULE_INDEX],
                                           exp_id[m_ETA_MODULE_INDEX],
                                           exp_id[m_SIDE_INDEX],
					   exp_id[m_ROW_INDEX],					   
                                           exp_id[m_STRIP_INDEX]);

   		stripid1 = strip_id (	   barrel_ec(stripid),
		    			   layer_disk(stripid),
					   phi_module(stripid),
					   eta_module(stripid),
					   side(stripid),
					   row(stripid),
					   strip(stripid));

	    }
	    
	    else {
	    
            	stripid = strip_id (       exp_id[m_BARREL_EC_INDEX],
		    			   exp_id[m_LAYER_DISK_INDEX],
					   exp_id[m_PHI_MODULE_INDEX],
					   exp_id[m_ETA_MODULE_INDEX],
					   exp_id[m_SIDE_INDEX],
					   exp_id[m_STRIP_INDEX]);


	   	stripid1 = strip_id (	    barrel_ec(stripid),
                                            layer_disk(stripid),
                                            phi_module(stripid),
                                            eta_module(stripid),
                                            side(stripid),
                                            strip(stripid));

	    }


            if (stripid != stripid1) {
                log << MSG::ERROR << "SCT_ID::test_wafer_packing: new and old pixel ids not equal. New/old ids "
                    << " " << show_to_string(stripid1) << " " 
                    << show_to_string(stripid) << endmsg;
            }
        }



        if (m_msgSvc) {
            log << MSG::DEBUG << "SCT_ID::test_wafer_packing: Successful tested " 
                << nids << " ids. " 
                << endmsg;
        }
        else {
            std::cout << " DEBUG SCT_ID::test_wafer_packing: Successful tested " 
                      << nids << " ids. " 
                      << std::endl;
        }
    }
    else {
        log << MSG::ERROR << "SCT_ID::test_wafer_packing: Unable to test wafer is packing - no dictionary has been defined. " 
            << endmsg;
    }
}


/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <set>
#include <sstream>
#include <iostream>
#include <math.h>

/* See comments in Base class */

LArOnline_SuperCellID::LArOnline_SuperCellID(void) :
    LArOnlineID_Base()
{
  m_this_is_slar=true;
}


LArOnline_SuperCellID::~LArOnline_SuperCellID(void) 
{
}

/* =================================================================== */
int  LArOnline_SuperCellID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/* =================================================================== */
{
    MsgStream log(m_msgSvc, "LArOnline_SuperCellID" );
    std::string strg = "initialize_from_dictionary";
    if(m_msgSvc) {
        log << MSG::INFO << strg << endmsg;
    }
    else {
        std::cout << strg << std::endl;
    }
  
    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        if(m_msgSvc) log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
        return (0);
    }
    else {
        if(m_msgSvc) log << MSG::DEBUG << "(Re)initialize" << endmsg;
    }

    // init base object
    if(AtlasDetectorID::initialize_from_dictionary(dict_mgr)) return (1);
    m_dict = dict_mgr.find_dictionary ("LArCalorimeter"); 
    if(!m_dict) {

        strg = " initialize_from_dictionary - cannot access LArCalorimeter dictionary ";
        if(m_msgSvc) {
            log << MSG::ERROR << strg << endmsg;
        }
        else {
            std::cout << "LArOnline_SuperCellID::" << strg << std::endl;
        }
        return 1;
    }

    // Register version of the dictionary used
    if (register_dict_tag(dict_mgr, "LArCalorimeter")) return(1);

    // initialize dictionary version
    AtlasDetectorID::setDictVersion(dict_mgr, "LArCalorimeter");

    /* Initialize the field indices */
    std::string group_name("LArOnline_SuperCell");
    if(LArOnlineID_Base::initLevelsFromDict(group_name)) return (1);

    if(m_msgSvc) {
       log << MSG::INFO << "Finished initLevelsFromDict" << endmsg;
    } else {
       std::cout << "Finished initLevelsFromDict" << std::endl;
    }


    /* Find value for the field LAr Calorimeter */
    const IdDictDictionary* atlasDict = dict_mgr.find_dictionary ("ATLAS"); 
    int larField   = -1;
    if (atlasDict->get_label_value("subdet", "LArCalorimeter", larField)) {
        std::stringstream strm;
        strm << atlasDict->m_name;
        strg= " Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary "+strm.str();
        if(m_msgSvc) {
            log << MSG::ERROR << strg << endmsg;
        }
        else {
            std::cout << "LArOnline_SuperCellID:" << strg << std::endl;
        }
        return (1);
    }

    /* Find value for the field LArOnline */
    int larOnlineField   = -4;
    if (m_dict->get_label_value("part", "LArOnline", larOnlineField)) {
        std::stringstream strm;
        strm <<  m_dict->m_name;      
        strg = "Could not get value for label 'LArOnline' of field 'part' in dictionary "+strm.str(); 
        if(m_msgSvc) {
            log << MSG::ERROR << strg << endmsg;
        }
        else {
            std::cout << strg << std::endl;
        }
        return (1);
    }

    /* Find value for the field calibLArOnline */
    int larOnlineCalibField   = -5;
    if (m_dict->get_label_value("part", "LArOnlineCalib", larOnlineCalibField)) {
        std::stringstream strm;
        strm <<  m_dict->m_name;      
        strg = "Could not get value for label 'LArOnlineCalib' of field 'part' in dictionary "+strm.str();
        if(m_msgSvc) {
            log << MSG::ERROR << strg << endmsg;
        }
        else {
            std::cout << strg << std::endl;
        }
        return (1);
    }

    /* Set up id for Region and range prefix */
    ExpandedIdentifier region_id; 
    region_id.add(larField);
    region_id.add(larOnlineField);
    Range prefix;

    /*Full range for all channels*/
    m_full_laronline_range = m_dict->build_multirange( region_id, group_name,  prefix); 
    m_full_feb_range       = m_dict->build_multirange( region_id, group_name, prefix, "slar_slot"); 
    m_full_feedthrough_range = m_dict->build_multirange( region_id , group_name, prefix, "slar_feedthrough");

    std::string strg0= " initialize_from_dictionary :";
    std::string strg1= " feedthrough range -> " + (std::string)m_full_feedthrough_range;
    std::string strg2= " feedthrough slot range -> " + (std::string)m_full_feb_range;  
    std::string strg3= " channel range -> " + (std::string)m_full_laronline_range;
    if(m_msgSvc) {
        log << MSG::DEBUG << strg0 << endmsg;
        log << MSG::DEBUG << strg1 << endmsg;
        log << MSG::DEBUG << strg2 << endmsg;
        log << MSG::DEBUG << strg3 << endmsg;
    }
    else {
        std::cout << strg0 << std::endl;
        std::cout << strg1 << std::endl;
        std::cout << strg2 << std::endl;
        std::cout << strg3 << std::endl;
    }
  
    /* Setup the hash tables */
    std::stringstream strm;
    strm << dictionaryVersion();
    strg="[initialize_from_dictionary] version= " + strm.str();      
    if(m_msgSvc) {
        log << MSG::DEBUG << strg << endmsg;
    }
    else {
        std::cout << "LArOnline_SuperCellID: " << strg << std::endl;      
    }
    if( dictionaryVersion() == "fullAtlas" ) {
        if(LArOnlineID_Base::init_hashes()) return (1);
    }
  
    // Setup for hash calculation for channels (febs is further below)

    // Febs have a uniform number of channels
    // The lookup table only needs to contain the
    // hash offset for each feb
  
    // The implementation requires:
  
    //   1) a lookup table for each feb containing hash offset
    //   2) a decoder to access the "index" corresponding to the
    //      bec/side/ft/slot fields. These fields use x bits, so the
    //      vector has a length of 2**x.

    /* Create decoder for fields bec to slot */
    IdDictFieldImplementation::size_type bits = 
        m_bec_impl.bits() +
        m_side_impl.bits() +
        m_feedthrough_impl.bits() +
        m_slot_impl.bits();
    IdDictFieldImplementation::size_type bits_offset = m_bec_impl.bits_offset();
    m_bec_slot_impl.set_bits(bits, bits_offset);
    int size = (1 << bits);

    // Set up vector as lookup table for hash calculation. 
    m_chan_hash_calcs.resize(size);

    std::stringstream strm1;
    std::stringstream strm2;
    for (unsigned int i = 0; i < m_febHashMax; ++i) {

        HWIdentifier febId = feb_Id(i) ;

        HashCalc hc;
      
        HWIdentifier min = channel_Id ( febId, 0);

        IdentifierHash min_hash = channel_Hash_binary_search(min);
        hc.m_hash   = min_hash;
        m_chan_hash_calcs[m_bec_slot_impl.unpack(min)] = hc;

        if (m_bec_slot_impl.unpack(min) >= size) {
            strm << size;
            strm1 << show_to_string(min);
            strm2 << m_bec_slot_impl.unpack(min);
            strg = "Min > "+strm.str();
            strg1= " "+strm1.str();
            strg2= " "+strm2.str();
            if(m_msgSvc) {
                log << MSG::DEBUG << strg << endmsg;
                log << MSG::DEBUG << strg1 << endmsg;
                log << MSG::DEBUG << strg2 << endmsg;
            }
            else {
                std::cout << strg << std::endl;
                std::cout << strg1 << std::endl;
                std::cout << strg2 << std::endl;
            }     
        }
    }

    // Check channel hash calculation
    for (unsigned int i = 0; i < m_channelHashMax; ++i) {
        HWIdentifier id = channel_Id(i);
        if (channel_Hash(id) != i) {
            strm << show_to_string(id);
            strm1 << channel_Hash(id);
            strm2 << i;
            strg = " *****  Error channel ranges, id, hash, i = "+strm.str();
            strg1= " , "+strm1.str();
            strg2= " , "+strm2.str();
            if(m_msgSvc) {
                log << MSG::ERROR << strg << endmsg;
                log << MSG::ERROR << strg1 << endmsg;
                log << MSG::ERROR << strg2 << endmsg;
            }
            else {
                std::cout << strg << std::endl;
                std::cout << strg1 << std::endl;
                std::cout << strg2 << std::endl;
            }
        }
    }


  
    // Setup for hash calculation for febs

    // We calculate the feb hash by saving the hash of each
    // feedthrough in a HashCalc object and then adding on the slot
    // number for a particular feb
  
    // The implementation requires:
  
    //   1) a lookup table for each ft containing hash offset
    //   2) a decoder to access the "index" corresponding to the
    //      bec/side/ft fields. These fields use x bits, so the
    //      vector has a length of 2**x.

    /* Create decoder for fields bec to ft */
    bits = m_bec_impl.bits() +
        m_side_impl.bits() +
        m_feedthrough_impl.bits();
    bits_offset = m_bec_impl.bits_offset();
    m_bec_ft_impl.set_bits(bits, bits_offset);
    size = (1 << bits);

    // Set up vector as lookup table for hash calculation. 
    m_feb_hash_calcs.resize(size);

    // Get context for conversion to expanded ids
    IdContext ftContext = feedthroughContext();
    ExpandedIdentifier ftExpId;

    for (unsigned int i = 0; i < m_feedthroughHashMax; ++i) {

        HWIdentifier min = feedthrough_Id(i) ;

        HashCalcFeb hc;

        // Set the hash id for each feedthrough, and then check if one
        // needs to also save the slot values
        IdentifierHash min_hash = LArOnlineID_Base::feb_Hash_binary_search(min);
        hc.m_hash   = min_hash;

        // For each feedthrough we save the possible slot values for
        // the hash calculation.
        if (get_expanded_id(min, ftExpId, &ftContext)) {
            if(m_msgSvc) {
               log << MSG::WARNING << " *****  Warning cannot get ft expanded id for " << show_to_string(min) << endmsg;
            } else {
               std::cout << " *****  Warning cannot get ft expanded id for " << show_to_string(min) << std::endl;
            }
        }

        // Now and save values if we either have an
        // enumerated set of slots, or more than one FT
        for (unsigned int i = 0; i < m_full_feb_range.size(); ++i) {
            if (m_full_feb_range[i].match(ftExpId)) {
                const Range::field& slotField = m_full_feb_range[i][m_slot_index];
                if (slotField.get_mode() == Range::field::both_bounded ||
                    slotField.get_mode() == Range::field::enumerated) {
                    // save values
                    unsigned int nvalues = slotField.get_indices();
                    hc.m_slot_values.reserve(hc.m_slot_values.size() + nvalues);
                    for (unsigned int j = 0; j < nvalues; ++j) {
                        hc.m_slot_values.push_back(slotField.get_value_at(j));
                    }
                }
            }
        }

        // Set hash calculator
        m_feb_hash_calcs[m_bec_ft_impl.unpack(min)] = hc;


        if (m_bec_ft_impl.unpack(min) >= size) {
            std::stringstream strm, strm1, strm2, strm3;
            strm << size;
            strm1 << show_to_string(min);
            strm2 << m_bec_ft_impl.unpack(min);
            strm3 << min_hash;
            strg = "Min > " + strm.str() + " " + strm1.str() + " " + strm2.str() + " " + strm3.str();
            if(m_msgSvc) {
                log << MSG::DEBUG << strg << endmsg;
            }
            else {
                std::cout << strg << std::endl;
            }     
        }
    }

    // Check feb hash calculation
    for (unsigned int i = 0; i < m_febHashMax; ++i) {
        HWIdentifier id = feb_Id(i);
        if (feb_Hash(id) != i) {
            std::stringstream strm, strm1, strm2;
            strm << show_to_string(id);
            strm1 << feb_Hash(id);
            strm2 << i;
            strg = " *****  Error feb ranges, id, hash, i = " + 
                strm.str() + " , " + strm1.str() + " , "+strm2.str();
            if(m_msgSvc) {
                log << MSG::ERROR << strg << endmsg;
            }
            else {
                std::cout << strg << std::endl;
            }
        }
    }

    return 0;
}


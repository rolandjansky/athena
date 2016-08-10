/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArOnlineID.h"
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

LArOnlineID::LArOnlineID(void) :
  LArOnlineID_Base(),
  m_calibModuleHashMax(0),
  m_calibChannelHashMax(0)
{
  m_this_is_slar=false;
}


LArOnlineID::~LArOnlineID(void) 
{
}

/* =================================================================== */
int  LArOnlineID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/* =================================================================== */
{
    MsgStream log(m_msgSvc, "LArOnlineID" );
    std::string strg = "initialize_from_dictionary";
    if(m_msgSvc) {
        log << MSG::INFO << strg << endmsg;
    }
    else {
        std::cout << strg << std::endl;
    }
  
    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        if(m_msgSvc)log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
        return (0);
    }
    else {
        log << MSG::DEBUG << "(Re)initialize" << endmsg;
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
            std::cout << "LArOnlineID::" << strg << std::endl;
        }
        return 1;
    }

    // Register version of the dictionary used
    if (register_dict_tag(dict_mgr, "LArCalorimeter")) return(1);

    // initialize dictionary version
    AtlasDetectorID::setDictVersion(dict_mgr, "LArCalorimeter");

    /* Initialize the field indices */
//    if(initLevelsFromDict()) return (1);
    std::string group_name("LArOnline");
    if(LArOnlineID_Base::initLevelsFromDict(group_name)) return (1);


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
            std::cout << "LArOnlineID:" << strg << std::endl;
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
    /* Calib Multirange */
    ExpandedIdentifier region_id2; 
    region_id2.add(larField);
    region_id2.add(larOnlineCalibField);
    Range prefix2;

    /*Full range for all channels*/
    m_full_laronline_range = m_dict->build_multirange( region_id , group_name, prefix); 
    m_full_feb_range       = m_dict->build_multirange( region_id , group_name, prefix, "slot"); 
    m_full_feedthrough_range = m_dict->build_multirange( region_id , group_name, prefix, "feedthrough");

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
    /* calib */ 
    /* Full range for calib channels */
    m_full_calib_laronline_range = m_dict->build_multirange( region_id2 , group_name, prefix2); 
    m_full_calib_module_range    = m_dict->build_multirange( region_id2 , group_name, prefix2, "slot"); 

    strg0= "=> initialize_from_dictionary : ";
    strg2= "=> Calib module range -> " + (std::string)m_full_calib_module_range;  
    strg3= "=> Calib channel range -> " + (std::string)m_full_calib_laronline_range;
    if(m_msgSvc) {
        log << MSG::DEBUG << strg0 << endmsg;
        log << MSG::DEBUG << strg2 << endmsg;
        log << MSG::DEBUG << strg3 << endmsg;
    }
    else {
        std::cout << strg0 << std::endl;
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
        std::cout << "LArOnlineID: " << strg << std::endl;      
    }
    if( dictionaryVersion() == "fullAtlas" ) {
        if(LArOnlineID_Base::init_hashes()) return (1);
        if(init_calib_hashes()) return (1);
    }
    if( dictionaryVersion() == "H8TestBeam" ) {
        if(init_H8Hashes()) return (1);
        if(init_calib_hashes()) return (1);
    }
    if( dictionaryVersion() == "H6TestBeam" ) {
        log << MSG::DEBUG << "[initialze_from_dictionary] ...call init_H6hashes.." << endmsg;
        if(init_H6Hashes()) return (1);
        if(init_calib_hashes()) return (1);
    }
    log << MSG::DEBUG << "initialize_from_dictionary -> calibModuleHash= " << m_calibModuleHashMax << endmsg; 
    log << MSG::DEBUG << "initialize_from_dictionary -> calibChannelHash= " << m_calibChannelHashMax << endmsg; 

  
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
            //std::cout << "min > " << size << " " 
            //    << i << " "
            //      << show_to_string(min) << " " 
            //      << m_pnz_reg_impl.unpack(min) << " " 
            //      << std::endl;
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

        // For each feedthrough we must check to see if the slot
        // values are enumerated or not. If they are enumerate we must
        // save the values in order to calculate the fed hash
        if (get_expanded_id(min, ftExpId, &ftContext)) {
            std::cout << " *****  Error cannot get ft expanded id for " << show_to_string(min) << std::endl;
        }
        unsigned int nrangesFound = 0;
        for (unsigned int i = 0; i < m_full_feb_range.size(); ++i) {
            if (m_full_feb_range[i].match(ftExpId)) {
                nrangesFound += 1;
                const Range::field& slotField = m_full_feb_range[i][m_slot_index];
                if (slotField.get_mode() == Range::field::enumerated) {
                    // save values
                    hc.m_slot_values = slotField.get_values();
                }
            }
        }

        // Similarly, if there is more than one range per feedthrough,
        // this means that slot values are not a continuous range. In
        // this case, as well, we save all possible slot values
        if (nrangesFound > 1) {
            for (unsigned int i = 0; i < m_full_feb_range.size(); ++i) {
                if (m_full_feb_range[i].match(ftExpId)) {
                    const Range::field& slotField = m_full_feb_range[i][m_slot_index];
                    if (slotField.get_mode() == Range::field::both_bounded) {
                        // save values
                        unsigned int nvalues = slotField.get_maximum() - slotField.get_minimum() + 1;
                        hc.m_slot_values.reserve(hc.m_slot_values.size() + nvalues);
                        for (unsigned int j = 0; j < nvalues; ++j) {
                            hc.m_slot_values.push_back(j + slotField.get_minimum());
                        }
                    }
                    else {
                        std::cout << " *****  Error feb range slot field is NOT both_bounded - id, slot mode: " 
                                  << show_to_string(min) << " " << slotField.get_mode() << std::endl;
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
            //std::cout << "min > " << size << " " 
            //    << i << " "
            //      << show_to_string(min) << " " 
            //      << m_pnz_reg_impl.unpack(min) << " " 
            //      << std::endl;
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


/*========================================*/
int LArOnlineID::init_H8Hashes(void) 
/*========================================*/
{
  MsgStream log(m_msgSvc, "LArOnlineID" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  /* Channel hash */
  unsigned int nids=0;
  std::set<HWIdentifier> ids;
  log << MSG::DEBUG << "[init_H8hashes] > ChannelId : m_full_laronline_range.size() = " << m_full_laronline_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_laronline_range.size(); ++i) 
    {
      const Range& range = m_full_laronline_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier id = this->channel_Id (exp_id[m_bec_index],
                                              exp_id[m_side_index],
                                              exp_id[m_feedthrough_index],
                                              exp_id[m_slot_index],
                                              exp_id[m_channel_in_slot_index]);
          if(!(ids.insert(id)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes : duplicated id for channel nb = "+strm1.str();
              strm3 << show_to_string(id);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID::Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }           
            }
          nids++;
        }
    }
  unsigned int nidtb=0;
  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H8(*first) )
        {
          m_channel_vec.push_back(*first);
          nidtb++;
        } 
    }
  m_channelHashMax = m_channel_vec.size();


  /* FEB hash */
  /*==========*/
  nids = 0;
  ids.clear();
  log << MSG::DEBUG << "[init_H8hashes] > FebId : m_full_feb_range.size() = " << m_full_feb_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_feb_range.size(); ++i) 
    {
      const Range& range = m_full_feb_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier febId = feb_Id( exp_id[m_bec_index],
                                       exp_id[m_side_index],
                                       exp_id[m_feedthrough_index],
                                       exp_id[m_slot_index] );
          if(!(ids.insert(febId)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes: duplicated id for FEB nb = "+strm1.str();
              strm3 << show_to_string(febId);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID:: Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  nidtb=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H8(*first) )
        {
          m_feb_vec.push_back(*first);
          nidtb++;
        }
    }
  m_febHashMax = m_feb_vec.size();

  /* Feedthrough hash */
  /*=================*/
  nids = 0;
  ids.clear();
  log << MSG::DEBUG << "[init_H8hashes] FeedthroughId: m_feedthrough_range.size() = " << m_full_feedthrough_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_feedthrough_range.size(); ++i) 
    {
      const Range& range = m_full_feedthrough_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier feedthroughId = feedthrough_Id( exp_id[m_bec_index],
                                                       exp_id[m_side_index],
                                                       exp_id[m_feedthrough_index] );
          int test_bec = barrel_ec( feedthroughId);
          int test_pn  = pos_neg( feedthroughId);
          int test_ft = feedthrough( feedthroughId);
          log << MSG::VERBOSE << "[init_H8hashes] in loop : [bec,pn,ft]= [" << test_bec 
              << "," << test_pn << "," << test_ft << "]"<< endmsg;
          if(!(ids.insert(feedthroughId)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes : duplicated id for feedthrough nb = "+strm1.str();
              strm3 << show_to_string(feedthroughId);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID::Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  nidtb=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H8(*first) )
        {
          m_feedthrough_vec.push_back(*first);
          nidtb++;
        }
    }
  m_feedthroughHashMax = m_feedthrough_vec.size();
  log << MSG::DEBUG << "[init_H8hashes] final m_feedthroughHashMax = " << m_feedthroughHashMax << endmsg;

  return (0);
}


/*========================================*/
int LArOnlineID::init_H6Hashes(void) 
/*========================================*/
{
  MsgStream log(m_msgSvc, "LArOnlineID" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  unsigned int nids=0;
  std::set<HWIdentifier> ids;
  log << MSG::DEBUG << "[init_H6hashes] : ChannelId : m_full_laronline_range.size() = " 
      << m_full_laronline_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_laronline_range.size(); ++i) 
    {
      const Range& range = m_full_laronline_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier id = this->channel_Id (exp_id[m_bec_index],
                                              exp_id[m_side_index],
                                              exp_id[m_feedthrough_index],
                                              exp_id[m_slot_index],
                                              exp_id[m_channel_in_slot_index]);
          if(!(ids.insert(id)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes: duplicated id for channel nb = "+strm1.str();
              strm3 << show_to_string(id);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID:: Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  unsigned int nidtb=0;
  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H6(*first) )
        {
          m_channel_vec.push_back(*first);
          nidtb++;
        } 
    }
  m_channelHashMax = m_channel_vec.size();
  /*
  std::cout << "==> init_H6Hashes() -->LArOnlineID::init_H6hashes Found " 
            << m_channel_vec.size() 
            << " H6 channels "  
            << std::endl;
  */

  /* FEB hash */
  /*==========*/
  nids = 0;
  ids.clear();
  log << MSG::DEBUG << "[init_H6hashes] > FebId : m_full_feb_range.size() = " << m_full_feb_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_feb_range.size(); ++i) 
    {
      const Range& range = m_full_feb_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier febId = feb_Id( exp_id[m_bec_index],
                                       exp_id[m_side_index],
                                       exp_id[m_feedthrough_index],
                                       exp_id[m_slot_index] );
          if(!(ids.insert(febId)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes : duplicated id for FEB nb = "+strm1.str();
              strm3 << show_to_string(febId);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID::Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  nidtb=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H6(*first) )
        {
          m_feb_vec.push_back(*first);
          nidtb++;
        }
    }
  m_febHashMax = m_feb_vec.size();


  /* Feedthrough hash */
  /*=================*/
  nids = 0;
  ids.clear();
  // AL-->
  log << MSG::DEBUG << "[init_H6hashes] FeedthroughId: m_feedthrough_range.size() = " << m_full_feedthrough_range.size() << endmsg;
  for (unsigned int i = 0; i < m_full_feedthrough_range.size(); ++i) 
    {
      const Range& range = m_full_feedthrough_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) 
        {
          const ExpandedIdentifier& exp_id = (*first);
          HWIdentifier feedthroughId = feedthrough_Id( exp_id[m_bec_index],
                                                       exp_id[m_side_index],
                                                       exp_id[m_feedthrough_index] );
          log << MSG::DEBUG << "[init_H6hashes] m_bec_index= " << m_bec_index 
              << "m_side_index= " << m_bec_index 
              << "m_feedthrough_index= " << m_bec_index 
              << "m_slot_index= " << m_bec_index << endmsg;
          int test_bec = barrel_ec( feedthroughId);
          int test_pn  = pos_neg( feedthroughId);
          int test_ft = feedthrough( feedthroughId);
          log << MSG::DEBUG << "[init_H6hashes] in loop : [bec,pn,ft]= [" << test_bec 
              << "," << test_pn << "," << test_ft << "]"<< endmsg;
          if(!(ids.insert(feedthroughId)).second)
            {
              strm1 << nids;
              strg1 = " init_hashes : duplicated id for feedthrough nb = "+strm1.str();
              strm3 << show_to_string(feedthroughId);
              strg3 = " expanded Id= "+strm3.str();
              if(m_msgSvc)
                {
                  log  << MSG::ERROR << strg1 << endmsg;
                  //log  << MSG::ERROR << strg2 << endmsg;
                  log  << MSG::ERROR << strg3 << endmsg;
                }
              else
                {
                  std::cout << "LArOnlineID::Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  nidtb=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      if( is_H6FT( *first ))
        {
          log << MSG::DEBUG << "[init_H6hashes] filling m_feedthrough_vec IS-H6 !!"<< endmsg;
          m_feedthrough_vec.push_back(*first);
          nidtb++;
        }
    }
  m_feedthroughHashMax = m_feedthrough_vec.size();
  log << MSG::DEBUG << "[init_H6hashes] final m_feedthroughHashMax = " << m_feedthroughHashMax << endmsg;

  return (0);
}



/*========================================*/
int LArOnlineID::init_calib_hashes(void) 
/*========================================*/
{
  unsigned int nids=0;
  std::set<HWIdentifier> ids;
  for (unsigned int i = 0; i < m_full_calib_laronline_range.size(); ++i) 
    {
      const Range& range = m_full_calib_laronline_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) {
        const ExpandedIdentifier& exp_id = (*first);
        HWIdentifier id = this->calib_channel_Id(exp_id[m_bec_index],
                                                 exp_id[m_side_index],
                                                 exp_id[m_feedthrough_index],
                                                 exp_id[m_slot_index],
                                                 exp_id[m_channel_in_slot_index]);
        if(!(ids.insert(id)).second)
          {
            std::cout << " LArOnlineID::init_calib_Hashes "
                      << " Error: duplicated id for channel id. nids= " << nids
                      << " compact Id  " ;
            (*first).show();
            std::cout << " " << show_to_string(id) << std::endl;
          }
        nids++;
      }
    }
  unsigned int nidtb=0;
  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      m_calib_channel_vec.push_back(*first);
      nidtb++; 
    }
  m_calibChannelHashMax = m_calib_channel_vec.size();

  /* FEB hash */
  /*==========*/
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_calib_module_range.size(); ++i) 
    {
      const Range& range = m_full_calib_module_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) {
        const ExpandedIdentifier& exp_id = (*first);
        HWIdentifier febId = calib_module_Id( exp_id[m_bec_index],
                                              exp_id[m_side_index],
                                              exp_id[m_feedthrough_index],
                                              exp_id[m_slot_index] );
        if(!(ids.insert(febId)).second){
          std::cout << " LArOnlineID::init_calibhashes "
                    << " Error: duplicated id for feb id. nids= " << nids
                    << " compact Id  " ;
          (*first).show();
          std::cout << " " << show_to_string(febId) << std::endl;
          std::cout << std::endl;
        }
        nids++;
      }
    }
  nidtb=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nidtb < nids; ++first) 
    {
      m_calib_module_vec.push_back(*first);
      nidtb++;
    }
  m_calibModuleHashMax = m_calib_module_vec.size();
  return (0);
}



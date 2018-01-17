/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArIdentifier/LArOnlineID_Base.h"
#include "LArIdentifier/LArOnlID_Exception.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <set>
#include <sstream>
#include <iostream>
#include <math.h>

/* MAY-17-04:*/
/* AL added init_hashes for calib lines */
/* */
/* FEB-25-04: */
/* AL removed H6_channel_vec, H8_channel_vec and associated methods/iterators*/
/*    replaced init_H8channelHashes by init_H8hashes */
/*    replaced init_H6channelHashes by init_H6hashes */
/*    removed init_H6febHashes and init_H8febHashes */
/*    Choice of the init_hashes method NOW based on the "version" ID taken from */
/*    the Dictionary */ 
/* FEB-16-04: */
/* AL added access to TestBeamH8 Channels via init_H8hashes */
/*    added access to TestBeamH6 Channels via init_H6hashes*/
/*    added access to TestBeamH8 febs via init_H8hashes */
/*    added access to TestBeamH6 febs via init_H6hashes*/
/* */
/* FEB-12-04: */
/* FLG improved check_ids methods */
/* */
/* Set of instances developped in the following: */
/* LAronline_ID::destructor & constructor */
/* LArOnlineID::feedthroughContext() */
/* LArOnlineID::channelContext()  */
/* LArOnlineID::initialize_from_dictionary(IdDictMgr&)  */
/* LArOnlineID::get_expanded_id(HWIdentifier, ExpandedIdentifier, IdContext)*/
/* LArOnlineID::initLevelsFromDict() */
/* LArOnlineID::init_hashes() */
/* LArOnlineID::init_H8hashes() */
/* LArOnlineID::init_H6hashes() */

LArOnlineID_Base::LArOnlineID_Base(void) :
  m_this_is_slar(false),
  m_laronlineRegion_index(999),
  m_lar_index(999), 
  m_laronline_index(999), 
  m_bec_index(999),
  m_side_index(999),
  m_feedthrough_index(999),
  m_slot_index(999),
  m_channel_in_slot_index(999),
  m_slar_index(999),
  m_dict(0),
  m_feedthroughHashMax(0),
  m_febHashMax(0),
  m_channelHashMax(0)
{
}


LArOnlineID_Base::~LArOnlineID_Base(void) 
{
}


//==================================================================
std::string LArOnlineID_Base::feedthrough_name( const HWIdentifier id )const{
//==================================================================
  std::string result;
  int ft = feedthrough(id);
  if( barrel_ec(id)== 0 ){
    // if BARREL
    if( pos_neg(id) == 1 ){
      // if EMBA
      if( ft==31 ){result = "I01R";}
      if( ft==0  ){result = "I01L";}
      if( ft==1  ){result = "I02R";}
      if( ft==2  ){result = "I02L";}
      if( ft==3  ){result = "I03R";}
      if( ft==4  ){result = "I03L";}
      if( ft==5  ){result = "I04R";}
      if( ft==6  ){result = "I04L";}
      if( ft==7  ){result = "I05R";}
      if( ft==8  ){result = "I05L";}
      if( ft==9  ){result = "I06R";}
      if( ft==10 ){result = "I06L";}
      if( ft==11 ){result = "I07R";}
      if( ft==12 ){result = "I07L";}
      if( ft==13 ){result = "I08R";}
      if( ft==14 ){result = "I08L";}
      if( ft==15 ){result = "I09R";}
      if( ft==16 ){result = "I09L";}
      if( ft==17 ){result = "I10R";}
      if( ft==18 ){result = "I10L";}
      if( ft==19 ){result = "I11R";}
      if( ft==20 ){result = "I11L";}
      if( ft==21 ){result = "I12R";}
      if( ft==22 ){result = "I12L";}
      if( ft==23 ){result = "I13R";}
      if( ft==24 ){result = "I13L";}
      if( ft==25 ){result = "I14R";}
      if( ft==26 ){result = "I14L";}
      if( ft==27 ){result = "I15R";}
      if( ft==28 ){result = "I15L";}
      if( ft==29 ){result = "I16R";}
      if( ft==30 ){result = "I16L";}
    }
    else{
      // EMBC
      if( ft==15 ){result = "H01R";}
      if( ft==16 ){result = "H01L";}
      if( ft==13 ){result = "H02R";}
      if( ft==14 ){result = "H02L";}
      if( ft==11 ){result = "H03R";}
      if( ft==12 ){result = "H03L";}
      if( ft==9 ){result  = "H04R";}
      if( ft==10){result  = "H04L";}
      if( ft==7 ){result  = "H05R";}
      if( ft==8 ){result  = "H05L";}
      if( ft==5 ){result  = "H06R";}
      if( ft==6 ){result  = "H06L";}
      if( ft==3 ){result  = "H07R";}
      if( ft==4 ){result  = "H07L";}
      if( ft==1 ){result  = "H08R";}
      if( ft==2 ){result  = "H08L";}
      if( ft==31){result  = "H09R";}
      if( ft==0 ){result  = "H09L";}
      if( ft==29 ){result = "H10R";}
      if( ft==30 ){result = "H10L";}
      if( ft==27 ){result = "H11R";}
      if( ft==28 ){result = "H11L";}
      if( ft==25 ){result = "H12R";}
      if( ft==26 ){result = "H12L";}
      if( ft==23 ){result = "H13R";}
      if( ft==24 ){result = "H13L";}
      if( ft==21 ){result = "H14R";}
      if( ft==22 ){result = "H14L";}
      if( ft==19 ){result = "H15R";}
      if( ft==20 ){result = "H15L";}
      if( ft==17 ){result = "H16R";}
      if( ft==18 ){result = "H16L";}
    }
  }
  if ( barrel_ec(id) == 1 ){
    // ENDCAP
    if( pos_neg(id) == 1 ){
      // EMEC-A
      if( ft==0 ){result = "A01R";}
      if( ft==1 ){result = "A01L";}
      if( ft==2 ){result = "A02R";}
      if( ft==3 ){result = "A02L";}
      if( ft==4 ){result = "A03R";}
      if( ft==5 ){result = "A03L";}
      if( ft==6 ){result = "A04L";}
      if( ft==7 ){result = "A05R";}
      if( ft==8 ){result = "A05L";}
      if( ft==9 ){result = "A06R";}
      if( ft==10){result = "A06L";}
      if( ft==11){result = "A07R";}
      if( ft==12){result = "A07L";}
      if( ft==13){result = "A08R";}
      if( ft==14){result = "A08L";}
      if( ft==15){result = "A09R";}
      if( ft==16){result = "A09L";}
      if( ft==17){result = "A10R";}
      if( ft==18){result = "A10L";}
      if( ft==19){result = "A11R";}
      if( ft==20){result = "A11L";}
      if( ft==21){result = "A12R";}
      if( ft==22){result = "A12L";}
      if( ft==23){result = "A13R";}
      if( ft==24){result = "A13L";}
      if( ft==25){result = "A14R";} // ft25-27 are sFCal; A14R, A14L, A15L  
      if( ft==26){result = "A14L";} // will need to be replaced by actual
      if( ft==27){result = "A15L";} // crate name once it exists           
    }
    else{
      // EMEC-C
      if( ft==11 ){result = "C01R";}
      if( ft==12 ){result = "C01L";}
      if( ft==9  ){result = "C02R";}
      if( ft==10 ){result = "C02L";}
      if( ft==7  ){result = "C03R";}
      if( ft==8  ){result = "C03L";}
      if( ft==4  ){result = "C05R";}
      if( ft==5  ){result = "C05L";}
      if( ft==2  ){result = "C06R";}
      if( ft==3  ){result = "C06L";}
      if( ft==0  ){result = "C07R";}
      if( ft==1  ){result = "C07L";}
      if( ft==23 ){result = "C08R";}
      if( ft==24 ){result = "C08L";}
      if( ft==21 ){result = "C09R";}
      if( ft==22 ){result = "C09L";}
      if( ft==19 ){result = "C10R";}
      if( ft==20 ){result = "C10L";}
      if( ft==17 ){result = "C11R";}
      if( ft==18 ){result = "C11L";}
      if( ft==15 ){result = "C12R";}
      if( ft==16 ){result = "C12L";}
      if( ft==13 ){result = "C13R";}
      if( ft==14 ){result = "C13L";}
      if( ft==6  ){result = "C04L";}
      if( ft==25 ){result = "C14R";} // ft25-27 are sFCal; C14R, C14L, C15L 
      if( ft==26 ){result = "C14L";} // will need to be replaced by actual  
      if( ft==27 ){result = "C15L";} // crate name once it exists           
    }
  }
  return result;
}

//==================================================================
std::string LArOnlineID_Base::channel_name( const HWIdentifier id )const{
//==================================================================
  std::ostringstream s1;
  //std::string s2;
  int Nchan = channel(id);
  int Nslot = slot(id);
  int Nft   = feedthrough(id);
  std::string det_barrel_endcap = "NODETE";
  std::string det_side = "/X-SIDE";

  if( barrel_ec(id) == 0 ) det_barrel_endcap = "BARREL";
  if( barrel_ec(id) == 1 ) det_barrel_endcap = "ENDCAP";
  if( pos_neg(id)   == 1 ) det_side = "/A-SIDE";
  if( pos_neg(id)   == 0 ) det_side = "/C-SIDE";

  s1 << det_barrel_endcap
     << det_side
     << "/FT-" << Nft 
     << "/SLOT-" << Nslot 
     << "/CHAN-" << Nchan; 
  return s1.str();
  //return s2;
}



/* =================================================================== */
IdContext LArOnlineID_Base::cryostatEndContext(void) const
/* =================================================================== */
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_side_index));
}



/* =================================================================== */
IdContext LArOnlineID_Base::feedthroughContext(void) const
/* =================================================================== */
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_feedthrough_index));
}




/* =================================================================== */
IdContext LArOnlineID_Base::channelContext(void) const
/* =================================================================== */
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_channel_in_slot_index));
}



/* =================================================================== */
IdContext LArOnlineID_Base::febContext(void) const
/* =================================================================== */
{
  ExpandedIdentifier id;
  return (IdContext(id, 0, m_slot_index));
}



/* =================================================================== */
int LArOnlineID_Base::channelInSlotMax(const HWIdentifier channelId) const
/* =================================================================== */
{

  ExpandedIdentifier expId;
  IdContext context = channelContext();
  if(!get_expanded_id(channelId, expId, &context)) {
    int result = -999;
    for (unsigned int i = 0; i < m_full_laronline_range.size(); ++i) {
      const Range& range = m_full_laronline_range[i];
      if (range.match(expId)) {
        const Range::field& channel_in_slot_field = range[m_channel_in_slot_index];
        if (channel_in_slot_field.has_maximum()) {
          int channel_in_slotmax = channel_in_slot_field.get_maximum();
          if (result < channel_in_slotmax) result = channel_in_slotmax + 1;
        }
      }
    }
    return (result);
  }
  return (-999);  // default
}





/* =================================================================== */
int  LArOnlineID_Base::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/* =================================================================== */
{
    MsgStream log(m_msgSvc, "LArOnlineID_Base" );
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
            std::cout << "LArOnlineID_Base::" << strg << std::endl;
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
    if ( m_this_is_slar ) group_name+="_SuperCell";
    if(initLevelsFromDict(group_name)) return (1);


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
            std::cout << "LArOnlineID_Base:" << strg << std::endl;
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

  
    /* Setup the hash tables */
    std::stringstream strm;
    strm << dictionaryVersion();
    strg="[initialize_from_dictionary] version= " + strm.str();      
    if(m_msgSvc) {
        log << MSG::DEBUG << strg << endmsg;
    }
    else {
        std::cout << "LArOnlineID_Base: " << strg << std::endl;      
    }
    if( dictionaryVersion() == "fullAtlas" ) {
        if(init_hashes()) return (1);
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
        IdentifierHash min_hash = feb_Hash_binary_search(min);
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



/* ================================================================*/
void LArOnlineID_Base::channel_Id_checks( int barrel_ec, int pos_neg, int feedthrough,
                                     int slot,       int channel ) const
/* ================================================================*/
{  
    // Check that id is within allowed range
    // Fill expanded id
    ExpandedIdentifier expId(lar_exp());
    expId << s_lar_online_field_value << barrel_ec << pos_neg << feedthrough << slot << channel ;
    if ( m_this_is_slar ) expId << 1;
    if( expId.last_error () != ExpandedIdentifier::none) {
        char * l_str = new char[200];
        std::string errorMessage ;
        sprintf(l_str,"Error in LArOnlineID_Base::channel_Id_checks(field values) , values ok but did not build , barrel_ec: %d, feedthrough: %d , slot: %d , channel_in_slot: %d ", barrel_ec , feedthrough, slot, channel);
        errorMessage += std::string(l_str);
        delete[] l_str ;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }

    if (!m_full_laronline_range.match(expId)) { 
        std::string errorMessage = "LArOnlineID_Base::channel_Id_checks() result is not OK: ID, range = "
            + std::string(expId) + " , " + (std::string)m_full_laronline_range;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }
}

/* ================================================================*/
void LArOnlineID_Base::channel_Id_checks   ( const HWIdentifier febId,
                                        int channel ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
    ExpandedIdentifier expId; 
    IdContext context = febContext();
    if (get_expanded_id(febId, expId, &context)) {
        std::string errorMessage = "LArOnlineID_Base::channel_Id_checks(febId) result is not OK: ID = "
            + show_to_string(febId) ;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }

    expId << channel;
    if ( m_this_is_slar ) expId << 1;
    if (!m_full_laronline_range.match(expId)) { 
        std::string errorMessage = "LArOnlineID_Base::channel_Id_checks(febId) result is not OK: ID, range = "
            + std::string(expId) + " , " + (std::string)m_full_laronline_range;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }
}

/* ================================================================*/
void LArOnlineID_Base::channel_Id_checks   ( const HWIdentifier feedthroughId,
                                         int slot,  int channel ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
    ExpandedIdentifier expId; 
    IdContext context = feedthroughContext();
    if (get_expanded_id(feedthroughId, expId, &context)) {
        std::string errorMessage = "LArOnlineID_Base::channel_Id_checks(feedthroughId) result is not OK: ID = "
            + show_to_string(feedthroughId) ;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }

    expId << slot << channel;
    if ( m_this_is_slar ) expId << 1;
    if (!m_full_laronline_range.match(expId)) { 
        std::string errorMessage = "LArOnlineID_Base::channel_Id_checks(feedthroughId) result is not OK: ID, range = "
            + std::string(expId) + " , " + (std::string)m_full_laronline_range;
        LArOnlID_Exception except(errorMessage , 99);
        throw except ;   
    }
}

/* ================================================================*/
void LArOnlineID_Base::feb_Id_checks( int barrel_ec, int pos_neg, int feedthrough, int slot ) const
/* ================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_exp());
  expId << s_lar_online_field_value << barrel_ec << pos_neg << feedthrough << slot ;

  if ( m_this_is_slar ) expId << 0 << 1;
  if(  expId.last_error () != ExpandedIdentifier::none ){
    char * l_str = new char[200];
    std::string errorMessage ;
    sprintf(l_str,"Error in LArOnlineID_Base::feb_Id_checks , values ok but did not build , barrel_ec: %d , pos_neg: %d , feedthrough: %d , slot: %d", 
            barrel_ec , pos_neg, feedthrough, slot);
    errorMessage += std::string(l_str);
    delete[] l_str ;
    LArOnlID_Exception except(errorMessage , 99);
    throw except ;
    }  
    if (!m_full_feedthrough_range.match(expId)) { std::string errorMessage = "LArOnlineID_Base::feb_Id_checks() result is not OK: ID, range = "
    + std::string(expId) + " , " + (std::string)m_full_feb_range;
    LArOnlID_Exception except(errorMessage , 99);
    throw except ;   
  }
}

/* ================================================================*/
void LArOnlineID_Base::feedthrough_Id_checks( int barrel_ec, int pos_neg, int feedthrough ) const
/* ==============================================================================*/
{
  // Check that id is within allowed range
  // Fill expanded id
  ExpandedIdentifier expId(lar_exp());
  expId << s_lar_online_field_value << barrel_ec << pos_neg << feedthrough << 0 << 0 << (int)m_this_is_slar;

//  if ( m_this_is_slar ) expId << 0 << 0 << 1;
  if(  expId.last_error () != ExpandedIdentifier::none ){
    char * l_str = new char[200];
    std::string errorMessage ;
    sprintf(l_str,"Error in LArOnlineID_Base::feedthrough_Id_checks , values ok but did not build , barrel_ec: %d , pos_neg: %d , feedthrough: %d ", 
    barrel_ec , pos_neg, feedthrough);
    errorMessage += std::string(l_str);
    delete[] l_str ;
    LArOnlID_Exception except(errorMessage , 99);
    throw except ;
    }  
    if (!m_full_feedthrough_range.match(expId)) { std::string errorMessage = "LArOnlineID_Base::feedthrough_Id_checks() result is not OK: ID, range = "
    + std::string(expId) + " , " + (std::string)m_full_feedthrough_range;
    LArOnlID_Exception except(errorMessage , 99);
    throw except ;   
  }
}



/* =================================================================================*/
int  LArOnlineID_Base::get_expanded_id  (const HWIdentifier& id, ExpandedIdentifier& exp_id, 
                                    const IdContext* context) const
/*==================================================================================*/
{
  // We assume that the context is >= feedthrough
  exp_id.clear();
  exp_id << lar_field_value()
         << s_lar_online_field_value 
         << barrel_ec(id)
         << pos_neg(id);
  if(context && context->end_index() >= m_feedthrough_index) {
    exp_id << feedthrough(id);
    if(context && context->end_index() >= m_slot_index) {
      exp_id << slot(id);
      if(context->end_index() >= m_channel_in_slot_index) {
        exp_id << channel(id);
        if(context->end_index() >= m_slar_index) {
           exp_id << is_slar(id);
        }
      }
    }
  }
  return (0);
}


/* =================================================================================*/
int  LArOnlineID_Base::get_expanded_calib_id  (const HWIdentifier& id, ExpandedIdentifier& exp_id, 
                                          const IdContext* context) const
/*==================================================================================*/
{
  // We assume that the context is >= feedthrough
  exp_id.clear();
  exp_id << lar_field_value()
         << s_lar_onlineCalib_field_value 
         << barrel_ec(id)
         << pos_neg(id)
         << feedthrough(id);
  if(context && context->end_index() >= m_slot_index) {
    exp_id << slot(id);
    if(context->end_index() >= m_channel_in_slot_index) {
      exp_id << channel(id);
      if(context->end_index() >= m_slar_index) {
        exp_id << is_slar(id);
      }
    }
  }
  return (0);
}



/*======================================*/
int LArOnlineID_Base::initLevelsFromDict(const std::string& group_name) 
/*======================================*/
{
  MsgStream log(m_msgSvc, "LArOnlineID_Base" );

  if(!m_dict) 
    {
    if(m_msgSvc) {
      log << MSG::ERROR << "initLevelsFromDict - dictionary NOT initialized "
          << endmsg;
    }
    else {
      std::cout << "LArEM_ID::initLevelsFromDict - dictionary NOT initialized "
                << std::endl ;
    }
      return (1);
    }

  /* Find out which identifier field corresponds to each level.*/

  m_lar_index             = 999 ;
  m_laronline_index       = 999 ;
  m_bec_index             = 999 ;
  m_side_index            = 999 ;
  m_feedthrough_index     = 999 ;
  m_slot_index            = 999 ;
  m_channel_in_slot_index = 999 ;
  m_slar_index            = 999 ;
  std::string strg;
 
  // Save index to a DictionaryRegion for unpacking
  IdDictRegion* reg = m_dict->find_region("laronline-barrel");
  if (reg) 
    {
      m_laronlineRegion_index = reg->m_index;
    }
  else 
    {
      strg = "initLevelsFromDict - unable to find laronline region ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LAronline_ID::" << strg << std::endl;
        }
      return (1);
    }
  /* Find LAr field */
  IdDictField* field = m_dict->find_field("subdet") ;
  if (field) 
    {
      m_lar_index = field->m_index ;
    }
  else 
    {
      strg= "initLevelsFromDict - unable to find 'subdet' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }
      return (1);
    }
  
  /* Find LArOnline field */
  field = m_dict->find_field("part") ;
  if (field) 
    {
      m_laronline_index = field->m_index ;
    }
  else 
    {
      strg= "initLevelsFromDict - unable to find 'part' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }
      return (1);      
    }
  
  /* Find barrel-endcap field */
  field = m_dict->find_field("barrel-ec") ;
  if (field) 
    {
      m_bec_index = field->m_index ;
    }
  else 
    {
      strg= "initLevelsFromDict - unable to find 'barrel-endcap' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }
      return (1);    
    }
  
  /* Find detector-side field */
  field = m_dict->find_field("pos_neg") ;
  if (field) 
    {
      m_side_index = field->m_index ;
    }
  else 
    {
      strg=  "initLevelsFromDict - unable to find 'barrel-endcap' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }
      return (1);  
    }
  
  /* Find feedthrough field */
  std::string feedthrough_name;
  if ( m_this_is_slar ){
  feedthrough_name+="slar_feedthrough";
  } else {
  feedthrough_name+="feedthrough";
  }
  field = m_dict->find_field(feedthrough_name) ;
  if (field) {
    m_feedthrough_index = field->m_index ;
  }
  else 
    {
      strg =  "initLevelsFromDict - unable to find 'feedthrough' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }
      return (1);  
    }
  
  /* Find slot field */
  std::string slot_name;
  if ( m_this_is_slar ){
  slot_name+="slar_slot";
  } else {
  slot_name+="slot";
  }
  field = m_dict->find_field(slot_name) ;
  if (field) 
    {
      m_slot_index = field->m_index ;
    }
  else 
    {
      strg =  "initLevelsFromDict - unable to find 'slot' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }       
      return (1);
    }
  
  /* Find channel_in_slot field */
  std::string channel_name;
  if ( m_this_is_slar ){
  channel_name+="sch_in_slot";
  } else {
  channel_name+="channel_in_slot";
  }
  field = m_dict->find_field(channel_name) ;
  if (field) 
    {
      m_channel_in_slot_index = field->m_index ;
    }
  else 
    {
      strg =  "initLevelsFromDict - unable to find 'channel_in_slot' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg << std::endl;
        }       
      return (1);
    }

    /* Find slar field */
  if ( m_this_is_slar ) {
  field = m_dict->find_field("is-slar") ;
  if (field)
    {
      m_slar_index = field->m_index ;
    }
  else
    {
      strg =  "initLevelsFromDict - unable to find 'slar' field ";
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg << endmsg;
        }
      else
        {
          std::cout << "LArOnline_SuperCellID::" << strg << std::endl;
        }
      return (1);
    }
  } // end of m_this_is_slar
  
  /* Set the field implementations */

  IdDictGroup* group = m_dict->find_group(group_name);
  if ( !group ) {
        log << MSG::ERROR << "initLevelsFromDict - cannot find "
                << group_name << endmsg;
  } else {
        m_laronlineRegion_index = group->regions()[0]->m_index;
  }

  const IdDictRegion& region = *m_dict->m_regions[m_laronlineRegion_index];

  m_lar_impl            = region.m_implementation[m_lar_index]; 
  m_laronline_impl      = region.m_implementation[m_laronline_index]; 
  m_bec_impl            = region.m_implementation[m_bec_index]; 
  m_side_impl           = region.m_implementation[m_side_index]; 
  m_feedthrough_impl    = region.m_implementation[m_feedthrough_index]; 
  m_slot_impl           = region.m_implementation[m_slot_index]; 
  m_channel_in_slot_impl= region.m_implementation[m_channel_in_slot_index]; 
  if ( m_this_is_slar ) 
   m_slar_impl           = region.m_implementation[m_slar_index];

  if(m_msgSvc) {
    log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
    log << MSG::DEBUG << "lar     " << m_lar_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "online  " << m_laronline_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "bec     " << m_bec_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "side    " << m_side_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "ft      " << m_feedthrough_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "slot    " << m_slot_impl.show_to_string() << endmsg;
    log << MSG::DEBUG << "channel " << m_channel_in_slot_impl.show_to_string() << endmsg;
    if ( m_this_is_slar )
      log << MSG::DEBUG << "is-slar " << m_slar_impl.show_to_string() << endmsg;
  }
  else {
    std::cout << "decode index and bit fields for each level: " << std::endl;
    std::cout << "lar     "  << m_lar_impl.show_to_string() << std::endl;
    std::cout << "online  "  << m_laronline_impl.show_to_string() << std::endl;
    std::cout << "bec     "  << m_bec_impl.show_to_string() << std::endl;
    std::cout << "side    "  << m_side_impl.show_to_string() << std::endl;
    std::cout << "ft      "  << m_feedthrough_impl.show_to_string() << std::endl;
    std::cout << "solt    "  << m_slot_impl.show_to_string() << std::endl;
    std::cout << "channel "  << m_channel_in_slot_impl.show_to_string() << std::endl;
    if ( m_this_is_slar )
       std::cout << "slar    "  << m_slar_impl.show_to_string() << std::endl;
  }
  

  return(0) ;
}



/*======================================*/
int LArOnlineID_Base::init_hashes(void) 
/*======================================*/
{
  MsgStream log(m_msgSvc, "LArOnlineID_Base" );
  std::stringstream strm1;
  std::stringstream strm2;
  std::stringstream strm3;
  std::string strg1;
  std::string strg2;
  std::string strg3;

  /* channel hash */
  m_channelHashMax = m_full_laronline_range.cardinality();
  m_channel_vec.resize(m_channelHashMax);
  unsigned int nids = 0;
  std::set<HWIdentifier> ids;
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
                  std::cout << "LArOnlineID_Base::Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  if(ids.size() != m_channelHashMax) 
    {
      strm1 << ids.size();
      strm2 << m_channelHashMax;
      strg1 = " init_hashes ";
      strg2 = " set size NOT EQUAL to hash max. size "+strm1.str();
      strg3 = " hash max "+strm2.str();
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg1 << endmsg;
          log << MSG::ERROR << strg2 << endmsg;
          log << MSG::ERROR << strg3 << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg1 << std::endl;
          std::cout << "Error" << strg2 << std::endl;
          std::cout << strg3 << std::endl;
        }
      return (1);
    }
  nids=0;
  std::set<HWIdentifier>::const_iterator first = ids.begin();
  std::set<HWIdentifier>::const_iterator last  = ids.end();
  for (;first != last && nids < m_channel_vec.size(); ++first) 
    {
      m_channel_vec[nids] = (*first) ;
      nids++;
    } 

  /* feedthrough hash */
  m_feedthroughHashMax = m_full_feedthrough_range.cardinality();
  m_feedthrough_vec.resize(m_feedthroughHashMax);
  nids = 0;
  ids.clear();
  for (unsigned int i = 0; i < m_full_feedthrough_range.size(); ++i) 
    {
      const Range& range = m_full_feedthrough_range[i];
      Range::const_identifier_factory first = range.factory_begin();
      Range::const_identifier_factory last  = range.factory_end();
      for (; first != last; ++first) {
        const ExpandedIdentifier& exp_id = (*first);
        HWIdentifier feedthroughId = feedthrough_Id( exp_id[m_bec_index],
                                                     exp_id[m_side_index],
                                                     exp_id[m_feedthrough_index] );
        if(!(ids.insert(feedthroughId)).second){
              strm1 << nids;
              strg1 = " init_hashes: duplicated id for feedthrough nb = "+strm1.str();
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
                  std::cout << "LArOnlineID_Base:: Error" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
        }
        nids++;
      }
    }
  if(ids.size() != m_feedthroughHashMax) 
    {
      strm1 << ids.size();
      strm2 << m_feedthroughHashMax;
      strg1 = " init_hashes ";
      strg2 = " set size NOT EQUAL to feedthrough hash max. size "+strm1.str();
      strg3 = " hash max= "+strm2.str();
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg1 << endmsg;
          log << MSG::ERROR << strg2 << endmsg;
          log << MSG::ERROR << strg3 << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg1 << std::endl;
          std::cout << " Error:" << strg2 << std::endl;
          std::cout << strg3 << std::endl;
        }
      return (1);
    }
  nids=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nids < m_feedthrough_vec.size(); ++first) 
    {
      m_feedthrough_vec[nids] = (*first) ;
      nids++;
    }

  /* FEB hash */
  /*==========*/
  m_febHashMax = m_full_feb_range.cardinality();
  m_feb_vec.resize(m_febHashMax);
  nids = 0;
  ids.clear();
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
              strg1 = " init_hashes:  duplicated id for FEB nb = "+strm1.str();
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
                  std::cout << "LArOnlineID_Base::Error:" << strg1 << std::endl;
                  //std::cout << strg2 << std::endl;
                  std::cout << strg3 << std::endl;
                }
            }
          nids++;
        }
    }
  if(ids.size() != m_febHashMax) 
    {
      strm1 << ids.size();
      strm2 << m_febHashMax;
      strg1 = " init_hashes ";
      strg2 = " set size NOT EQUAL to FEB hash max. size "+strm1.str();
      strg3 = " hash max "+strm2.str();
      if(m_msgSvc)
        {
          log << MSG::ERROR << strg1 << endmsg;
          log << MSG::ERROR << strg2 << endmsg;
          log << MSG::ERROR << strg3 << endmsg;
        }
      else
        {
          std::cout << "LArOnlineID_Base::" << strg1 << std::endl;
          std::cout << "Error:" << strg2 << std::endl;
          std::cout << strg3 << std::endl;
        }
      return (1);
    }
  nids=0;
  first = ids.begin();
  last  = ids.end();
  for (;first != last && nids < m_feb_vec.size(); ++first) 
    {
      m_feb_vec[nids] = (*first) ;
      nids++;
    }


  return (0);
}




bool LArOnlineID_Base::isValidId(const HWIdentifier id) const {

  //std::cout << "larid=" << larid << ", onl=" << laronlineid << ", side=" << side << ", ft=" << ft << std::endl; 

  const int larid=m_lar_impl.unpack(id);
  const int laronlineid=m_laronline_impl.unpack(id);
  //std::cout << "larid=" << larid << ", onl=" << laronlineid << std::endl;
  if (larid!=lar_field_value()) return false;

  if (!(laronlineid==s_lar_online_field_value || laronlineid==s_lar_onlineCalib_field_value)) return false; 

  const int channel=m_channel_in_slot_impl.unpack(id);
  if (channel<0 || channel>128) return false;

  const int bec=m_bec_impl.unpack(id);
  const int ft=m_feedthrough_impl.unpack(id);
  const int slot=m_slot_impl.unpack(id);
  //std::cout << "bec=" << bec << ", ft=" << ft << ", slot=" << slot << std::endl; 
  if (slot<1) return false;
  if (ft<0) return false;
  if (bec==0) { // Barrel case
    if (ft>31) return false;
    if (slot>14) return false;
  }
  else { //Endcap case
    if (ft>27) return false;
    if (ft==2 || ft==9 || ft==15 || ft==21){ //Special crate
      if (slot >15) return false;
    }
    else if (ft==6) { //FCAL
      if (slot>15) return false; 
    }
    else if (ft>24 && ft<28) { //sFCAL
      if (slot>10) return false; 
    }
    else if (ft==3 || ft==10 || ft==16 || ft==22) { //HEC
      if (slot==3 || slot==4 || slot>10) return false;
    }
    else //standard crate
      if (slot>13) return false;
  }//end endcap
  return true;
}

//=====================
// Inline Definitions
//=====================

							   


HWIdentifier LArOnlineID_Base::feedthrough_Id (int barrel_ec, int pos_neg, int feedthrough ) const 
{
  HWIdentifier result(0);

  /*Pack fields independently */
  m_lar_impl.pack        (lar_field_value(), result);
  m_laronline_impl.pack  (s_lar_online_field_value, result);
  m_bec_impl.pack        (barrel_ec        , result);
  m_side_impl.pack       (pos_neg          , result);
  m_feedthrough_impl.pack(feedthrough      , result);
  if ( m_this_is_slar )
	m_slar_impl.pack       (1                , result);

  /* Do checks */
  if(m_do_checks) 
    {
      feedthrough_Id_checks ( barrel_ec, pos_neg, feedthrough );
    }
  return result;
}

HWIdentifier LArOnlineID_Base::feedthrough_Id(IdentifierHash feedthroughHashId) const
/*=============================================================================== */
{
  return(m_feedthrough_vec[feedthroughHashId]);
}

IdentifierHash LArOnlineID_Base::feedthrough_Hash (HWIdentifier feedthroughId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_feedthrough_vec.begin(),m_feedthrough_vec.end(),feedthroughId);
    if ( it != m_feedthrough_vec.end() ){
	return (it - m_feedthrough_vec.begin());
    }
    return (0);
}

HWIdentifier LArOnlineID_Base::feedthrough_Id(const HWIdentifier Id) const
/*============================================================================== */
{
  HWIdentifier result(Id);
  m_slot_impl.reset(result);
  m_channel_in_slot_impl.reset(result);
  return(result);
}

std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feedthrough_begin(void) const
/*====================================================================*/
{
  return(m_feedthrough_vec.begin());
}

std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feedthrough_end(void) const
/*==================================================================*/
{
  return(m_feedthrough_vec.end());
}


/* FEB id */
/*========*/

HWIdentifier LArOnlineID_Base::feb_Id(int barrel_ec, int pos_neg, 
					int feedthrough, int slot ) const 
/*==================================================================== */
{
  HWIdentifier result(0);

  /*Pack fields independently */
  m_lar_impl.pack        (lar_field_value(), result);
  m_laronline_impl.pack  (s_lar_online_field_value, result);
  m_bec_impl.pack        (barrel_ec        , result);
  m_side_impl.pack       (pos_neg          , result);
  m_feedthrough_impl.pack(feedthrough      , result);
  m_slot_impl.pack       (slot             , result);
  if ( m_this_is_slar )
    m_slar_impl.pack       (1                , result);

  /* Do checks */
  if(m_do_checks) {
    feb_Id_checks ( barrel_ec, pos_neg, feedthrough, slot );
  }
  return result;
}

HWIdentifier LArOnlineID_Base::feb_Id(const HWIdentifier feedthroughId , int slot) const
/*==================================================================================== */
{
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_slot_impl.pack            (slot  , result);
  if ( m_this_is_slar )
     m_slar_impl.pack            (1     , result);

  return(result);
}

HWIdentifier LArOnlineID_Base::feb_Id(const HWIdentifier channelId ) const
/*======================================================================= */
{
  HWIdentifier result(channelId);
  m_channel_in_slot_impl.reset(result);
  return(result);
}

HWIdentifier LArOnlineID_Base::feb_Id(IdentifierHash febHashId) const
/*================================================================== */
{
  return(m_feb_vec[febHashId]);
}

IdentifierHash LArOnlineID_Base::feb_Hash (HWIdentifier febId) const
/*=============================================================================== */
{
    // Get the hash caculator for the febs
    const HashCalcFeb& hc = m_feb_hash_calcs[m_bec_ft_impl.unpack(febId)];
    // Two cases: 
    //   1) slot values are enumerated and we must look for a matching
    //      value to obtain the index
    //   2) slot values are a continuous range, then the slot index is
    //      sufficient for the hash calculation
    if (hc.m_slot_values.size()) {
        // find matching value
        int slotValue = slot(febId);
        for (int i = 0; (unsigned int)i < hc.m_slot_values.size(); ++i) {
            if (slotValue == hc.m_slot_values[i]) return (hc.m_hash + i);
        }
        std::cout << "LArOnlineID_Base::feb_Hash - ***** ERROR: could not match slot value for has calculation " << std::endl;
    }
    size_type slotIndex = m_slot_impl.unpackToIndex(febId);
    return (hc.m_hash + slotIndex);
}

IdentifierHash LArOnlineID_Base::feb_Hash_binary_search (HWIdentifier febId) const
/*=============================================================================== */
{
    std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_feb_vec.begin(),m_feb_vec.end(),febId);
    if ( it != m_feb_vec.end() ){
	return (it - m_feb_vec.begin());
    }
    return (0);
}

HWIdentifier LArOnlineID_Base::channel_Id( int barrel_ec, int pos_neg, int feedthrough, 
					     int slot,      int channel ) const 
/*============================================================================== */
{  
  HWIdentifier result(0);
  /* Pack fields independently */
  m_lar_impl.pack            (lar_field_value()    , result);
  m_laronline_impl.pack      (s_lar_online_field_value , result);
  m_bec_impl.pack            (barrel_ec            , result);
  m_side_impl.pack           (pos_neg              , result);
  m_feedthrough_impl.pack    (feedthrough          , result);
  m_slot_impl.pack           (slot                 , result);
  m_channel_in_slot_impl.pack(channel              , result);
  if ( m_this_is_slar )
     m_slar_impl.pack           (1                    , result);

  /* Do checks */
  if(m_do_checks) {
    channel_Id_checks( barrel_ec, pos_neg, feedthrough, slot, channel );
  }
  return result;
}


HWIdentifier LArOnlineID_Base::channel_Id(IdentifierHash channelHashId) const
/*===================================================================*/
{
    return(m_channel_vec[channelHashId]);
}


HWIdentifier LArOnlineID_Base::channel_Id(const HWIdentifier feedthroughId,int slot,int channel) const 
/*==================================================================================================== */
{  
  HWIdentifier result(feedthroughId);
  /* Pack fields independently */
  m_slot_impl.reset           (result);
  m_channel_in_slot_impl.reset(result);
  m_slot_impl.pack            (slot  , result);
  m_channel_in_slot_impl.pack (channel, result);
  if ( m_this_is_slar )
    m_slar_impl.pack            (1  , result);

  /* Do checks */
  if(m_do_checks) {
      channel_Id_checks( feedthroughId, slot, channel );
  }
  return result;
}

HWIdentifier LArOnlineID_Base::channel_Id(const HWIdentifier febId, int channel) const 
/*======================================================================================= */
{  
  HWIdentifier result(febId);
  /* Pack fields independently */
  m_channel_in_slot_impl.reset(result);
  m_channel_in_slot_impl.pack (channel, result);
  if ( m_this_is_slar )
	m_slar_impl.pack (1, result);

  /* Do checks */
  if(m_do_checks) {
    channel_Id_checks( febId, channel );
  }
  return result;
}

//----------------------------------------------------------------------------
IdentifierHash LArOnlineID_Base::channel_Hash (HWIdentifier channelId) const
{
    const HashCalc& hc = m_chan_hash_calcs[m_bec_slot_impl.unpack(channelId)];
    return (hc.m_hash + channel(channelId));
}

IdentifierHash LArOnlineID_Base::channel_Hash_binary_search (HWIdentifier channelId) const
/*=========================================================================*/
{
  std::vector<HWIdentifier>::const_iterator it = std::lower_bound(m_channel_vec.begin(),m_channel_vec.end(),channelId);
  if ( it != m_channel_vec.end() ){
    return (it - m_channel_vec.begin());
  }
  return(0) ;
}

/* BOOLEAN */

/* 
 *=============================================================
 *=============================================================
 *                   Recommended methods 
 *=============================================================
 *=============================================================
 */

bool LArOnlineID_Base::isEMBchannel(const HWIdentifier id) const
/*========================================================*/
{
    return (barrel_ec(id)==0);
}
bool LArOnlineID_Base::isFCALchannel(const HWIdentifier id) const
/*========================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 && (ft == 6 || (ft > 24 && ft < 28)) );
}
bool LArOnlineID_Base::isHECchannel(const HWIdentifier id) const
/*========================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 
	   && 
	   ( ft==3 || ft==10 || ft==16 || ft==22 )
	   &&
	   slot(id) > 2 );
}
bool LArOnlineID_Base::isEMECIW(const HWIdentifier id) const {
  /*======================================================*/
  // 
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  int sl = slot(id);
  return (bec==1 && sl<3 && (ft==3  || ft==10 || 
			     ft==16 || ft==22)); 
}
bool LArOnlineID_Base::isEMECOW(const HWIdentifier id) const {
  /*======================================================*/
  // 
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  return (bec==1 && 
	  (ft==0 || ft==1 ||
	   ft==2 || ft==4 ||
	   ft==5 || ft==7 ||
	   ft==8 || ft==9 ||
	   ft==11|| ft==12||
	   ft==13|| ft==14||
	   ft==15|| ft==17||
	   ft==18|| ft==19||
	   ft==20|| ft==21||
	   ft==23|| ft==24 )
	  );
}
bool LArOnlineID_Base::isEMECchannel(const HWIdentifier id) const
/*========================================================*/
{/* redefinition with isEMECIW and isEMECOW */
  return (isEMECOW(id) || isEMECIW(id));
}


bool LArOnlineID_Base::isEMBPS(const HWIdentifier id) const
/*=================================================================*/
{
  int bec= barrel_ec(id);
  int sl = slot(id);
  return ( 
	  bec == 0 && sl == 1 
	  );
}

bool LArOnlineID_Base::isEMECPS(const HWIdentifier id) const
/*=================================================================*/
{/* redefinition (bug fix) */
  return (isEMECOW(id) && slot(id)==1);
}

bool LArOnlineID_Base::isPS(const HWIdentifier id) const
/*=================================================================*/
{/* redefinition with isEMBPS and isEMECPS */
  return ( isEMBPS(id) || isEMECPS(id));
}


bool LArOnlineID_Base::isNotWarmCableConnected(const HWIdentifier id) const
/*========================================================*/
{
  int ft = feedthrough(id);
  int sl = slot(id);
  int ch = channel(id);
  return ( 
	  (
	   isEMBchannel(id) &&
	   ( (-1 < ft  && ft < 32) &&
	     ( sl == 14 ) &&
	     ( 63 < ch && ch < 128 )
	     )
	   ) 
	  ||
	  (
	   isEMECchannel(id) && sl == 1 &&
	   (ft == 2 || ft == 9 || ft == 15 || ft == 21 ) &&
	   (63 < ch && ch < 128) 
	   )
	  );
}


/* 
 *=============================================================
 *=============================================================
 *                   Obsolete methods 
 *=============================================================
 *=============================================================
 */

bool LArOnlineID_Base::isEmBarrelOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefined to isEMBchannel() */
  return (isEMBchannel(id));
}

bool LArOnlineID_Base::isEndcapOnline(const HWIdentifier id) const
/*========================================================*/
{
  return (barrel_ec(id)==1);
}

bool LArOnlineID_Base::isFcalOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefinition (was redundant with isFCALchannel) */
  return isFCALchannel(id);
}
bool LArOnlineID_Base::isEmEndcapOnline(const HWIdentifier id) const
/*========================================================*/
{/* redefinition */
 return isEMECchannel(id);
}


/* 
 *=============================================================
 *=============================================================
 *                  Specific UseCases
 *=============================================================
 *=============================================================
 */

bool LArOnlineID_Base::isEmEndcapStandardOnline(const HWIdentifier id) const
/*======================================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 && 
	   (ft==0 ||
	    ft==1 ||
	    ft==4 ||
	    ft==5 ||
	    ft==7 ||
	    ft==8 ||
	    ft==11 ||
	    ft==12 ||
	    ft==13 ||
	    ft==14 ||
	    ft==17 ||
	    ft==18 ||
	    ft==19 ||
	    ft==20 ||
	    ft==23 ||
	    ft==24)
	   );
}
bool LArOnlineID_Base::isEmEndcapSpecialOnline(const HWIdentifier id) const
/*======================================================================*/
{
  int ft = feedthrough(id);
  return ( barrel_ec(id)==1 && 
	   (ft==2 ||
	    ft==9 ||
	    ft==15 ||
	    ft==21 )
	   );
}

bool LArOnlineID_Base::isCalibration(const HWIdentifier id) const
/*========================================================*/
{
  int bec= barrel_ec(id);
  int ft = feedthrough(id);
  int sl = slot(id);
  return (  (bec==0 && sl==15) 
	    ||
	    ( bec==1 && sl==15 &&
	      (ft==0 ||
	       ft==1 ||
	       ft==4 ||
	       ft==5 ||
	       ft==7 ||
	       ft==8 ||
	       ft==11 ||
	       ft==12 ||
	       ft==13 ||
	       ft==14 ||
	       ft==17 ||
	       ft==18 ||
	       ft==19 ||
	       ft==20 ||
	       ft==23 ||
	       ft==24 )
	      )
	    ||
	    ( barrel_ec(id)==1 && 
	      (sl==3 || sl==4 || sl==12) &&
	      (ft==3 ||
	       ft==10 ||
	       ft==16 ||
	       ft==22 )
	      )
	    ||
	    ( barrel_ec(id)==1 && sl==16 )
	    ||
	    ( barrel_ec(id)==1 && ft==27 && sl==11 )
	    );
}


/*
 *================================================================
 *================================================================
 * The following methods should NOT be used for new applications:
 *================================================================
 *================================================================
 */

bool LArOnlineID_Base::isHecOnline(const HWIdentifier id) const
/*========================================================*/
{
  /* Must be used with CARE !
   * This method returns true for all slots/channel in HEC crates,  
   * thus including the EMEC slot/channels that are present 
   * in slot 1 and slot 2 of HEC crates ! 
   * To access specifically EMEC in HEC slot/channel, you then 
   * have to use isEMECinHECchannel()
   */
  int ft = feedthrough(id);
  return ( 
	  //(slot(id)!=1 && slot(id)!= 2) &&
	  barrel_ec(id)==1 && 
	  (ft==3 || 
	   ft==10 || 
	   ft==16 || 
	   ft==22 )
	  );
}

bool LArOnlineID_Base::isHecOnlineFebId(const HWIdentifier febId) const
/*========================================================*/
{
  /* Must be used with CARE !
   * This method returns true for all slots/channel in HEC crates,  
   * thus including the EMEC slot/channels that are present 
   * in slot 1 and slot 2 of HEC crates ! 
   * To access specifically EMEC in HEC slot/channel, you then 
   * have to use isEMECinHECchannel()
   */
  int ft = feedthrough(febId);
  return ( barrel_ec(febId)==1 && 
	   (ft==3 || 
	    ft==10 || 
	    ft==16 || 
	    ft==22 )
	   );
}

bool LArOnlineID_Base::isEMECinHECchannel(const HWIdentifier id) const
/*=================================================================*/
{/* re-definition with isEMECIW */
  return isEMECIW(id);
}









LArOnlineID_Base::size_type LArOnlineID_Base::feedthroughHashMax (void) const
     /*=======================================================================*/
{
  return m_feedthroughHashMax;
}
LArOnlineID_Base::size_type LArOnlineID_Base::febHashMax (void) const
/*=======================================================================*/
{
  return m_febHashMax;
}
LArOnlineID_Base::size_type LArOnlineID_Base::channelHashMax (void) const
/*====================================================================*/
{
  return m_channelHashMax;
}




std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feb_begin(void) const
/*====================================================================*/
{
  return(m_feb_vec.begin());
}
std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::feb_end(void) const
/*==================================================================*/
{
  return(m_feb_vec.end());
}

std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::channel_begin(void) const
/*======================================================================*/
{
  return(m_channel_vec.begin());
}
std::vector<HWIdentifier>::const_iterator LArOnlineID_Base::channel_end(void) const
/*======================================================================*/
{
  return(m_channel_vec.end());
}

int LArOnlineID_Base::barrel_ec(const HWIdentifier id)const
/*=========================================================*/
{
  return (m_bec_impl.unpack(id));
}

int LArOnlineID_Base::feedthrough(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_feedthrough_impl.unpack(id));
}

int LArOnlineID_Base::pos_neg(const HWIdentifier id)const
/*===========================================================*/
{
  return (m_side_impl.unpack(id));
}


int LArOnlineID_Base::slot(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_slot_impl.unpack(id));
}

int LArOnlineID_Base::channel(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_channel_in_slot_impl.unpack(id));
}

int LArOnlineID_Base::is_slar(const HWIdentifier id)const
/*=====================================================*/
{
  return (m_slar_impl.unpack(id));
}








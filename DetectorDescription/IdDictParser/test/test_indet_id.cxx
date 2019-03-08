/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// $Header: /build/atlas/cvs/atlas/offline/DetectorDescription/IdDictParser/test/test_indet_id.cxx,v 1.13 2008-12-09 09:55:22 dquarrie Exp $ 
  
//  #include "AGDD_Model/AGDD_Model.h"
//  #include "AGDD_Parser/AGDD_Factory.h"

#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/Identifier.h" 
 
#include <cstdlib>
#include <iostream> 
 
static void tab (size_t level) 
{ 
    for (size_t i = 0; i < level; ++i) std::cout << " "; 
} 
 
//  static ExpandedIdentifier
//  pixel_id ( int barrel_ec,  
//  	   int layer_disk, 
//  	   int phi_module, 
//  	   int eta_module,      
//  	   int phi_index,
//  	   int eta_index) 
//  {
//      // Build identifier
//      ExpandedIdentifier result;
//      result << 1 << barrel_ec << layer_disk << phi_module << eta_module;
//      result << phi_index << eta_index;

//      assert (result.last_error () == ExpandedIdentifier::none);

//      return result;
//}


static bool
pixel_values_ok ( int barrel_ec,  
		  int layer_disk, 
		  int phi_module, 
		  int eta_module,
		  int phi_index,
		  int eta_index ) 
{

    // Check values

    if ( barrel_ec != -2 && barrel_ec != 0 && barrel_ec != 2 ) return false;
    if ( 0 == barrel_ec ) {
        if ( layer_disk  < 0  || layer_disk  >= 3   ) return false;
        if ( phi_module  < 0  || phi_module  >= 59  ) return false;
        if ( eta_module  < -6 || eta_module  >= 7   ) return false;
	//  the limit has been increased by one unit - logically it is
	//  incorrect and it needs further investigation, the numbers
	//  should be 206, 328-
	//  Note: ID note has 0-191, 0-319
        if ( phi_index    < 0  || phi_index  >= 329 ) return false;
        if ( eta_index    < 0  || eta_index  >= 207 ) return false;
    }
    else {
        if ( layer_disk  < 0  || layer_disk  >= 7   ) return false;
        if ( phi_module  < 0  || phi_module  >= 144 ) return false;
        if ( eta_module  < 0  || eta_module  >= 2   ) return false;
	//  the limit has been increased by one unit - logically it is
	//  incorrect and it needs further investigation the number
	//  should be 165, 192-
	//  Note: ID note has 0-143, 0-327
        if ( phi_index    < 0  || phi_index  >= 328 ) return false;
        if ( eta_index    < 0  || eta_index  >= 166 ) return false;
    }
    return true;
}

static void initLevelsFromDict(const IdDictDictionary* dict)
{

    if(!dict) {
	std::cout << " initLevelsFromDict - dictionary NOT initialized "
		  << std::endl;
	return;
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    size_t INDET_INDEX		= 999;
    size_t PIXEL_INDEX		= 999;
    size_t BARREL_EC_INDEX	= 999;
    size_t LAYER_DISK_INDEX	= 999;
    size_t PHI_MODULE_INDEX	= 999;
    size_t ETA_MODULE_INDEX	= 999;
    size_t PHI_INDEX_INDEX  	= 999;
    size_t ETA_INDEX_INDEX	= 999;
    
    IdDictField* field = dict->find_field("subdet");
    if (field) {
	INDET_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'subdet' field " 	
		  << std::endl;
    }
    field = dict->find_field("part");
    if (field) {
	PIXEL_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'part' field " 	
		  << std::endl;
    }
    field = dict->find_field("barrel_endcap");
    if (field) {
	BARREL_EC_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'barrel_endcap' field " 	
		  << std::endl;
    }
    field = dict->find_field("layer");
    if (field) {
	LAYER_DISK_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'layer' field " 	
		  << std::endl;
    }
    field = dict->find_field("phi_module");
    if (field) {
	PHI_MODULE_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'phi_module' field " 	
		  << std::endl;
    }
    field = dict->find_field("eta_module");
    if (field) {
	ETA_MODULE_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'eta_module' field " 	
		  << std::endl;
    }
    field = dict->find_field("phi_index");
    if (field) {
	PHI_INDEX_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'phi_index' field " 	
		  << std::endl;
    }
    field = dict->find_field("eta_index");
    if (field) {
	ETA_INDEX_INDEX = field->m_index;
    }
    else {
	std::cout << "PixelID::initLevelsFromDict - unable to find 'eta_index' field " 	
		  << std::endl;
    }
    
    std::cout << "PixelID::initLevelsFromDict - found levels " 	<< std::endl;
    std::cout << "subdet        " 	<< INDET_INDEX	<< std::endl;
    std::cout << "part          " 	<< PIXEL_INDEX	<< std::endl;
    std::cout << "barrel_endcap " 	<< BARREL_EC_INDEX 	<< std::endl;
    std::cout << "layer or disk " 	<< LAYER_DISK_INDEX 	<< std::endl;
    std::cout << "phi_module    " 	<< PHI_MODULE_INDEX 	<< std::endl;
    std::cout << "eta_module    " 	<< ETA_MODULE_INDEX 	<< std::endl;
    std::cout << "phi_index     " 	<< PHI_INDEX_INDEX    << std::endl;
    std::cout << "pixel         " 	<< ETA_INDEX_INDEX   	<< std::endl;
}

static int
test_lar (const IdDictMgr& idd)
{
    const IdDictDictionary* dictionary = idd.find_dictionary ("LArCalorimeter"); 
    if(!dictionary) {
	std::cout << " test_lar - cannot access LArCalorimeter dictionary "
		  << std::endl;
	return (1);
    }

    IdDictDictionary::regions_const_it rit; 
    for (rit = dictionary->m_regions.begin (); rit != dictionary->m_regions.end (); ++rit) { 
	const IdDictRegion& region = *(*rit);
 
	std::cout << "region #" << region.m_index << std::endl; 

	std::cout << "name "
		  << region.m_name << " group " 
		  << region.m_group << " tag " 
		  << region.m_tag << " next eta ";
	if (region.m_next_abs_eta) {
	    std::cout << region.m_next_abs_eta->m_name << " ";
	    if (region.m_next_abs_eta->m_prev_abs_eta) {
		std::cout << region.m_next_abs_eta->m_prev_abs_eta->m_name;
	    }
	}
	std::cout << " prev samp ";
	for (unsigned int i = 0; i < region.m_prev_samp.size(); ++i) {
	    std::cout << region.m_prev_samp[i]->m_name << " ";
	}
	std::cout << "  next samp ";
	for (unsigned int i = 0; i < region.m_next_samp.size(); ++i) {
	    std::cout << region.m_next_samp[i]->m_name << " ";
	}
	std::cout << " eta0 " << region.m_eta0 << " deta "
		  << region.m_deta << " "
		  << std::endl; 
 

    }


    // Set up id for region and range prefix
    // Connected channels
    ExpandedIdentifier reg_id;
    reg_id.add(4);
    reg_id.add(1);
    Range prefix;
    MultiRange full_em_range = dictionary->build_multirange(reg_id, prefix);
    MultiRange full_region_range = dictionary->build_multirange(reg_id, prefix, "region");
    std::string str = "lar_em";
    MultiRange full_em_range1 = dictionary->build_multirange(reg_id, str, prefix);
    MultiRange full_region_range1 = dictionary->build_multirange(reg_id, str, prefix, "region");

    std::cout << " em range  " << (std::string)full_em_range << " em_region  "
		  << (std::string)full_region_range << " "
		  << std::endl; 
    std::cout << " em range1 " << (std::string)full_em_range1 << " em_region1 "
		  << (std::string)full_region_range1 << " "
		  << std::endl; 

    return (0);
}


static int
test_pixel (const IdDictMgr& idd, Identifier::size_type pixel_region)
{

    // Build multirange for the valid set of identifiers

    MultiRange full_wafer_range;

    // Find value for the field InnerDetector
    const IdDictDictionary* atlasDict = idd.find_dictionary ("ATLAS"); 
    int inDetField   = -1;
    if (atlasDict->get_label_value("subdet", "InnerDetector", inDetField)) {
	std::cout << "Could not get value for label 'InnerDetector' of field 'subdet' in dictionary " 
		  << atlasDict->m_name
		  << std::endl;
	return (1);
    }

    const IdDictDictionary* dict = idd.find_dictionary ("InnerDetector"); 
    if(!dict) {
	std::cout << " PixelID::initialize_from_dict - cannot access InnerDetector dictionary "
		  << std::endl;
	return (1);
    }

    // Find value for the field PIXEL
    int pixelField   = -1;
    if (dict->get_label_value("part", "Pixel", pixelField)) {
	std::cout << "Could not get value for label 'Pixel' of field 'part' in dictionary " 
		  << dict->m_name
		  << std::endl;
	return (1);
    }
    std::cout << " PixelID::initialize_from_dict " 
	      << "Found field values: InDet/Pixel "  
	      << inDetField << "/"
	      << pixelField
	      << std::endl;

    // Test access to levels
    initLevelsFromDict(dict);

    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(inDetField);
    region_id.add(pixelField);
    Range prefix;
    full_wafer_range = dict->build_multirange(region_id, prefix, "eta_module");

    
    std::cout << "range -> " << (std::string) full_wafer_range << std::endl; 
    
    
    // Try loop over fullRange and check each id that comes out
    
    int nids = 0;
    for (unsigned int i = 0; i < full_wafer_range.size(); ++i) {
	
	Range range = full_wafer_range[i];
	Range::identifier_factory first = range.factory_begin();
	Range::identifier_factory last  = range.factory_end();
	
	int test_print = 0;
	for (; first != last; ++first, ++nids) {

	    ExpandedIdentifier id = *first;
	    
	    int barrel_ec  = id[2];
	    int layer_disk = id[3];
	    int phi_module = id[4];
	    int eta_module = id[5];
	    int phi_index  = id[6];
	    int eta_index  = id[7];
	    bool are_values_ok = pixel_values_ok ( barrel_ec,  
						   layer_disk, 
						   phi_module, 
						   eta_module,      
						   phi_index,
						   eta_index);

  	    if (!are_values_ok) {
		std::cout << " ERROR: id " << (std::string)id << " ok " << are_values_ok << std::endl;
  	    }

	    int begin = 0;
	    int end   = id.fields() - 1;
	    Identifier compact_id((Identifier::value_type)0);
	    if (dict->pack32(id, begin, end, compact_id)) {
  		std::cout << " ERROR: id " << (std::string)id << " begin, end  " 
  			  << begin << " " << end << std::hex << " " << compact_id << std::dec << std::endl;
//  		std::cout << " nid " << nids
//  			  << " compact id " << std::hex << compact_id << std::dec
//  			  << " id ";
//  		(*first).show();
//  		std::cout << std::endl;
	    }
	    else {
		int field;
		dict->unpack(compact_id, 0, 2, pixel_region, field);
		if (barrel_ec != field) {
		    std::cout << " ERROR: id " << (std::string)id << " compact  " 
			      << std::hex << " " << compact_id << std::dec << " index " << pixel_region
			      << " barrel_ec " << barrel_ec << " found " << field
			      << std::endl;
		}
		else {
		    if (test_print < 10) {
                        Identifier32 id32 = compact_id.get_identifier32();
                        Identifier idNew(id32);
			std::cout << " field comparison ";
			std::cout << " id " << (std::string)id << " compact  " 
				  << std::hex << " " << compact_id << std::dec 
                                  << " id32 " << id32.getString() << " compact  " 
				  << std::hex << " " << id32.get_compact() << std::dec 
                                  << " is_valid " << id32.is_valid() 
                                  << " idNew " << idNew.getString() << " compact  " 
				  << std::hex << " " << idNew.get_compact() << std::dec 
				  << " barrel_ec " << barrel_ec << " found " << field
				  << " OK - test_print " << test_print
				  << std::endl;
		    }
		}
		dict->unpack(compact_id, 0, 3, pixel_region, field);
		if (layer_disk != field) {
		    std::cout << " ERROR: id " << (std::string)id << " compact  " 
			      << std::hex << " " << compact_id << std::dec << " index " << pixel_region
			      << " layer_disk " << layer_disk << " found " << field
			      << std::endl;
		}
		else {
		    if (test_print < 10) {
                        Identifier32 id32 = compact_id.get_identifier32();
                        Identifier idNew(id32);
			std::cout << " field comparison ";
			std::cout << " id " << (std::string)id << " compact  " 
				  << std::hex << " " << compact_id << std::dec 
                                  << " id32 " << id32.getString() << " compact  " 
				  << std::hex << " " << id32.get_compact() << std::dec 
                                  << " is_valid " << id32.is_valid() 
                                  << " idNew " << idNew.getString() << " compact  " 
				  << std::hex << " " << idNew.get_compact() << std::dec 
				  << " layer_disk " << layer_disk << " found " << field
				  << " OK - test_print " << test_print
				  << std::endl;
		    }
		}
		dict->unpack(compact_id, 0, 4, pixel_region, field);
		if (phi_module != field) {
		    std::cout << " ERROR: id " << (std::string)id << " compact  " 
			      << std::hex << " " << compact_id << std::dec << " index " << pixel_region
			      << " phi_module " << phi_module << " found " << field
			      << std::endl;
		}
		else {
		    if (test_print < 10) {
                        Identifier32 id32 = compact_id.get_identifier32();
                        Identifier idNew(id32);
			std::cout << " field comparison ";
			std::cout << " id " << (std::string)id << " compact  " 
				  << std::hex << " " << compact_id << std::dec 
                                  << " id32 " << id32.getString() << " compact  " 
				  << std::hex << " " << id32.get_compact() << std::dec 
                                  << " is_valid " << id32.is_valid() 
                                  << " idNew " << idNew.getString() << " compact  " 
				  << std::hex << " " << idNew.get_compact() << std::dec 
				  << " phi_module " << phi_module << " found " << field
				  << " OK - test_print " << test_print
				  << std::endl;
		    }
		}
		dict->unpack(compact_id, 0, 5, pixel_region, field);
		if (eta_module != field) {
		    std::cout << " ERROR: id " << (std::string)id << " compact  " 
			      << std::hex << " " << compact_id << std::dec << " index " << pixel_region
			      << " eta_module " << eta_module << " found " << field
			      << std::endl;
		}
		else {
		    if (test_print < 10) {
                        Identifier32 id32;
                        std::cout << " is_valid " << id32.is_valid() << std::endl;
                        id32 = compact_id.get_identifier32();
                        Identifier idNew(id32);
			std::cout << " field comparison ";
			std::cout << " id " << (std::string)id << " compact  " 
				  << std::hex << " " << compact_id << std::dec 
                                  << " id32 " << id32.getString() << " compact  " 
				  << std::hex << " " << id32.get_compact() << std::dec 
                                  << " is_valid " << id32.is_valid() 
                                  << " idNew " << idNew.getString() << " compact  " 
				  << std::hex << " " << idNew.get_compact() << std::dec 
				  << " eta_module " << eta_module << " found " << field
				  << " OK - test_print " << test_print
				  << std::endl;
			test_print++;
		    }
		}
	    }
	}

    }
    return(0);
}

// static void
// set_indet_indexes(IdDictMgr& idd, 
// 		  Identifier::size_type& pixel_region, 
// 		  Identifier::size_type& sct_region, 
// 		  Identifier::size_type& trt_region)
// {
    
//     // Set overlap for pixel, sct and trt

//     IdDictDictionary* dict = idd.find_dictionary ("InnerDetector"); 
//     if(!dict) {
// 	std::cout << " set_indet_indexes - cannot access InnerDetector dictionary "
// 		  << std::endl;
// 	return;
//     }

//     // Find value for the field indet
//     int inDetField   = -1;
//     if (dict->get_label_value("subdet", "InnerDetector", inDetField)) {
// 	std::cout << "set_indet_indexes: Could not get value for label 'InnerDetector' of field 'subdet' in dictionary " 
// 		  << dict->m_name
// 		  << std::endl;
// 	return;
//     }

//     // Find value for the field PIXEL
//     int pixelField   = -1;
//     if (dict->get_label_value("part", "Pixel", pixelField)) {
// 	std::cout << "set_indet_indexes: Could not get value for label 'Pixel' of field 'part' in dictionary " 
// 		  << dict->m_name
// 		  << std::endl;
// 	return;
//     }
//     // Find value for the field SCT
//     int sctField   = -1;
//     if (dict->get_label_value("part", "SCT", sctField)) {
// 	std::cout << "set_indet_indexes: Could not get value for label 'SCT' of field 'part' in dictionary " 
// 		  << dict->m_name
// 		  << std::endl;
// 	return;
//     }
//     // Find value for the field TRT
//     int trtField   = -1;
//     if (dict->get_label_value("part", "TRT", trtField)) {
// 	std::cout << "set_indet_indexes: Could not get value for label 'TRT' of field 'part' in dictionary " 
// 		  << dict->m_name
// 		  << std::endl;
// 	return;
//     }
//     std::cout << " set_indet_indexes:  " 
// 	      << "Found field values: InDet - Pixel, SCT, TRT "  
// 	      << inDetField << " "
// 	      << pixelField << " "
// 	      << sctField << " "
// 	      << trtField << " "
// 	      << std::endl;

//     // Force uniform bit for pixel
//     ExpandedIdentifier pixel_id;
//     pixel_id.add(inDetField);
//     pixel_id.add(pixelField);
//     if(dict->find_region(pixel_id, pixel_region)){
// 	std::cout << " set_indet_indexes:  " 
// 		  << "unable to find region Pixel: id, reg "  << (std::string)pixel_id
// 		  << pixel_region
// 		  << std::endl;
//     }
    
	
//     // Force uniform bit for sct
//     ExpandedIdentifier sct_id;
//     sct_id.add(inDetField);
//     sct_id.add(sctField);
//     if (dict->find_region(sct_id, sct_region)) {
// 	std::cout << " set_indet_indexes:  " 
// 		  << "unable to find region SCT: id, reg "  << (std::string)sct_id
// 		  << sct_region
// 		  << std::endl;
//     }
	
    
    
//     // Force uniform bit for trt
//     ExpandedIdentifier trt_id;
//     trt_id.add(inDetField);
//     trt_id.add(trtField);
//     if (dict->find_region(trt_id, trt_region)) {
// 	std::cout << " set_indet_indexes:  " 
// 		  << "unable to find region TRT: id, reg "  << (std::string)trt_id
// 		  << trt_region
// 		  << std::endl;
//     }
	
    
//     std::cout << " set_indet_indexes:  " 
// 	      << "Region indices for  Pixel, SCT, TRT "  
// 	      << pixel_region << " "
// 	      << sct_region << " "
// 	      << trt_region << " "
// 	      << std::endl;

// }

//  static void
//  set_lar_overlap(IdDictMgr& idd)
//  {
    
//      // Set overlap for pixel, sct and trt

//      IdDictDictionary* dict = idd.find_dictionary ("LArCalorimeter"); 
//      if(!dict) {
//  	std::cout << " set_lar_overlap - cannot access LArCalorimeter dictionary "
//  		  << std::endl;
//  	return;
//      }

//      // Find value for the field lar
//      int larField   = -1;
//      if (dict->get_label_value("subdet", "LArCalorimeter", larField)) {
//  	std::cout << "set_lar_overlap: Could not get value for label 'LArCalorimeter' of field 'subdet' in dictionary " 
//  		  << dict->m_name
//  		  << std::endl;
//  	return;
//      }

//      // Find value for the field LAREM
//      int laremField   = -1;
//      if (dict->get_label_value("part", "LArEM", laremField)) {
//  	std::cout << "set_lar_overlap: Could not get value for label 'LArEM' of field 'part' in dictionary " 
//  		  << dict->m_name
//  		  << std::endl;
//  	return;
//      }
//      // Find value for the field LARHEC
//      int larhecField   = -1;
//      if (dict->get_label_value("part", "LArHEC", larhecField)) {
//  	std::cout << "set_lar_overlap: Could not get value for label 'LArHEC' of field 'part' in dictionary " 
//  		  << dict->m_name
//  		  << std::endl;
//  	return;
//      }
//      // Find value for the field LARFCAL
//      int larfcalField   = -1;
//      if (dict->get_label_value("part", "LArFCAL", larfcalField)) {
//  	std::cout << "set_lar_overlap: Could not get value for label 'LArFCAL' of field 'part' in dictionary " 
//  		  << dict->m_name
//  		  << std::endl;
//  	return;
//      }
//      std::cout << " set_lar_overlap:  " 
//  	      << "Found field values: Lar - LArEM, LArHEC, LArFCAL "  
//  	      << larField << " "
//  	      << laremField << " "
//  	      << larhecField << " "
//  	      << larfcalField << " "
//  	      << std::endl;

//      // Force uniform bit for larem
//      ExpandedIdentifier larem_id;
//      larem_id.add(larField);
//      larem_id.add(laremField);
//      size_t larem_region;
//      if(dict->find_region(larem_id, larem_region)){
//  	std::cout << " set_indet_indexes:  " 
//  		  << "unable to find region LArEM: id, reg "  << (std::string)larem_id
//  		  << larem_region
//  		  << std::endl;
//      }
    
//      // Force uniform bit for larhec
//      ExpandedIdentifier larhec_id;
//      larhec_id.add(larField);
//      larhec_id.add(larhecField);
//      size_t larhec_region;
//      if(dict->find_region(larhec_id, larhec_region)){
//  	std::cout << " set_indet_indexes:  " 
//  		  << "unable to find region LArHEC: id, reg "  << (std::string)larhec_id
//  		  << larhec_region
//  		  << std::endl;
//      }
    
//      // Force uniform bit for larfcal
//      ExpandedIdentifier larfcal_id;
//      larfcal_id.add(larField);
//      larfcal_id.add(larfcalField);
//      size_t larfcal_region;
//      if(dict->find_region(larfcal_id, larfcal_region)){
//  	std::cout << " set_indet_indexes:  " 
//  		  << "unable to find region LArFCAL: id, reg "  << (std::string)larfcal_id
//  		  << larfcal_region
//  		  << std::endl;
//      }
    
//      std::cout << " set_lar_overlap:  " 
//  	      << "Region indices for  LArEM, LArHEC, LArFCAL "  
//  	      << larem_region << " "
//  	      << larhec_region << " "
//  	      << larfcal_region << " "
//  	      << std::endl;

//      // Set integral over the number of bits
//      dict->integrate_bits ();
//  }

static void
print_bits (const IdDictMgr& idd)
{
    const IdDictMgr::dictionary_map& dm = idd.get_dictionary_map (); 

    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
	const IdDictDictionary& dictionary = *((*it).second); 
 
	std::cout << "---- " << n << " ----------------------------" << std::endl; 
	std::cout << "Dictionary " << dictionary.m_name << std::endl; 


//  	if ("MuonSpectrometer" != dictionary.m_name) {
//  	    std::cout << "skipping dictionary " << std::endl;
//  	    continue;
//  	}
	


	IdDictDictionary::regions_const_it rit; 
	for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) { 
	    const IdDictRegion& region = *(*rit);
 
	    std::cout << "region #" << region.m_index
		      << " name "   << region.m_name 
		      << " group "  << region.m_group 
		      << " tag "    << region.m_tag	
		      << std::endl;  

	    std::vector <IdDictFieldImplementation>::const_iterator fit; 
           
	    size_t width = 0; 
 
	    for (fit = region.m_implementation.begin ();  
		 fit != region.m_implementation.end (); 
		 ++fit) {
		const IdDictFieldImplementation& impl = *fit; 
 
		size_t w = impl.range()->m_field->m_name.size (); 
 
		if (w > width) width = w; 
	    } 
 
	    int bits = 0; 
 
	    for (fit = region.m_implementation.begin ();  
		 fit != region.m_implementation.end (); 
		 ++fit) { 
		const IdDictFieldImplementation& impl = *fit; 
 
		size_t w = impl.range()->m_field->m_name.size (); 
 
		std::cout << "  implement field #" << impl.range()->m_field->m_index <<  
		    " " << impl.range()->m_field->m_name; 
 
		tab (width - w); 
 
//  		int index = impl.field();
//  		std::string label = impl.range()->m_field->labels[index]->m_name;
//  		std::cout << " name " << label << " ";

		std::cout << " -> " << (std::string) impl.field() <<  
		    "/" << (std::string) impl.ored_field() <<  
		    " (" << impl.bits() << " bits)" <<  
		    " (" << impl.bits_offset() << " offset)" <<  
		    std::endl; 
 
		bits += impl.bits(); 
	    } 
 
	    Range range = region.build_range (); 
 
	    std::cout << " -> " << (std::string) range <<  
		" (cardinality=" << range.cardinality () << ")" << 
		" (" << bits << " bits)" << std::endl; 
	} 
    } 
}

static void
print_ranges (const IdDictMgr& idd)
{
    const IdDictMgr::dictionary_map& dm = idd.get_dictionary_map (); 

    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
	const IdDictDictionary& dictionary = *((*it).second); 
 
	std::cout << "---- " << n << " ----------------------------" << std::endl; 
	std::cout << "Dictionary " << dictionary.m_name << std::endl; 

	MultiRange mr = dictionary.build_multirange(); 
	std::cout << "Multirange: " << (std::string)mr << std::endl;
    } 
}

int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  
//      AGDD_Factory& f = AGDD_Factory::Xerces_instance ();
  
//      std::cout << "AGDD_Parser created" << std::endl;
  
//      f.build_detector_description ("unittest.agdd");

//      AGDD* agdd = f.get_detector_description ();

    IdDictParser parser;  
  
// Use the following to specify a "non-standard" file for one or more
// the the dictionary files

//    parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_H8_2004.xml");
     std::cout << "Read IdDictInnerDetector_slhc.xml " << std::endl;
     parser.register_external_entity("InnerDetector", "IdDictInnerDetector_slhc.xml");
    IdDictMgr& idd = parser.parse (argv[1]);  
//    IdDictMgr& idd = parser.parse (argv[1], "initial_layout");  

//    std::cout << "regenerate for tag = initial_layout " << std::endl;
//     idd.generate_implementation ("initial_layout");  

    
//     {
// 	// Test multiple parsing

// 	// Printout full multiranges - first parse
// 	std::cout << "Ranges for first pass " << std::endl;
// 	print_ranges (idd);

// 	// Set LArCalo to test beam
// 	parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter.xml");
// //	parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_H8_2004.xml");
// 	IdDictMgr& idd1 = parser.parse (argv[1]);  

// 	// Printout full multiranges - second parse
// 	std::cout << "Ranges for second pass " << std::endl;
// 	print_ranges (idd1);
//     }
    

    // Set the overlap for some dictionaries
    Identifier::size_type pixel_region = 0;
//     Identifier::size_type sct_region;
//     Identifier::size_type trt_region;

//    set_indet_indexes(idd, pixel_region, sct_region, trt_region);
    //set_lar_overlap(idd);
 
    // Printout full multiranges
    print_ranges (idd);

    // print out the number of bit required
    print_bits (idd);
 
    { 
	const IdDictDictionary* atlasDict = idd.find_dictionary ("ATLAS"); 

	// Find value for the field InnerDetector
	const IdDictField* partField = atlasDict->find_field("subdet");
	if (partField) {
	    std::cout << "Found field " << partField->m_name 
		      << " from dictionary " << atlasDict->m_name
		      << std::endl;
	}
	else {
	    std::cout << "Could not find 'part' field"
		      << " in dictionary " << atlasDict->m_name
		      << std::endl;
	    return 1;
	}
    
	std::vector <IdDictLabel*>::const_iterator itLabels = partField->m_labels.begin();
	int inDetField = -1;
	std::cout << "Found field label size "  
		  << partField->m_labels.size()
		  << std::endl;

	for (; itLabels != partField->m_labels.end(); ++itLabels) {
	    std::cout << "Field label  "  
		      << (*itLabels)->m_name << " " 
		      << (*itLabels)->m_valued
		      << " " 	<< (*itLabels)->m_value
		      << std::endl;

	    if("InnerDetector" == (*itLabels)->m_name) {
		if ((*itLabels)->m_valued) {
		    inDetField = (*itLabels)->m_value;
		}
	    }
	}
	
	std::cout << "inDetField "  
		  << inDetField
		  << std::endl;

	const IdDictDictionary* dictionary = idd.find_dictionary ("InnerDetector"); 
 
	if (dictionary != 0) { 
	    ExpandedIdentifier id ("1/0/0/21/6/319/191"); 
 
	    if(!dictionary->verify()) {
		std::cout << "Unable to verify dictionary " << dictionary->m_name << std::endl; 
	    }
	    
//  	    IdDictDictionary::bits32 b = dictionary->pack32 (id, 0, 6); 
	    Identifier packedB((Identifier::value_type)0);
	    dictionary->pack32 (id, 0, 6, packedB); 
 
	    std::cout << "b=[" << packedB << "]" << std::endl; 
 
	    ExpandedIdentifier id2;
	    dictionary->unpack (packedB, ExpandedIdentifier (), 6, id2); 
 
	    std::cout << "unpack->[" << (std::string) id2 << "]" << std::endl; 

	    if(test_pixel (idd, pixel_region)) {
		std::cout << "test_pixel failed" << std::endl;
	    }
	    else {
		std::cout << "test_pixel succeeded" << std::endl;
	    }
	    if(test_lar (idd)) {
		std::cout << "test_lar failed" << std::endl;
	    }
	    else {
		std::cout << "test_lar succeeded" << std::endl;
	    }
	}
    }
     
    return 0;  
}  
  
 
 
 
 
 

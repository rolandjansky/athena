/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/AtlasTest/DetDescrTest/IdDictTest/test/test_indet_id.cxx,v 1.6 2005-04-29 16:20:49 schaffer Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "InDetIdentifier/PixelID.h" 
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetIdentifier/TRT_ID.h" 
 
#include <iostream> 
#include <set> 
 
static void tab (size_t level) 
{ 
    for (size_t i = 0; i < level; ++i) std::cout << " "; 
} 

static void
test_pixel (const MultiRange& fullRange)
{
    
    std::cout << " Testing pixel ids'   " << std::endl;

    std::cout << "range -> " << (std::string) fullRange << std::endl; 
    
    // Try loop over fullRange and check each id that comes out

    PixelID pixel_id;

    for (unsigned int i = 0; i < fullRange.size(); ++i) {
	
	Range range = fullRange[i];

	std::cout << " Range " << i << " " << (std::string)range << std::endl;

	Range::identifier_factory first = range.factory_begin();
	Range::identifier_factory last  = range.factory_end();
	
	for (; first != last; ++first) {

	    ExpandedIdentifier id = *first;
	    
	    int barrel_ec  = id[2];
	    int layer_disk = id[3];
	    int phi_module = id[4];
	    int eta_module = id[5];
	    int phi_index  = id[6];
	    int eta_index  = id[7];
	    Identifier new_id =  pixel_id.pixel_id ( barrel_ec,  
						   layer_disk, 
						   phi_module, 
						   eta_module,      
						   phi_index,
						   eta_index);

//    	    if (!are_values_ok) {
//  		std::cout << " id " << (string)id << " ok " << are_values_ok << std::endl;
//    	    }
	}
	std::cout << " Successful end of pixel test   " << std::endl;
    }
    
}

static void
test_sct (const MultiRange& fullRange)
{
    
    std::cout << " Testing sct ids'   " << std::endl;

    std::cout << "range -> " << (std::string) fullRange << std::endl; 
    
    // Try loop over fullRange and check each id that comes out

    SCT_ID sct_id;

    for (unsigned int i = 0; i < fullRange.size(); ++i) {
	
	Range range = fullRange[i];

	std::cout << " Range " << i << " " << (std::string)range << std::endl;

	Range::identifier_factory first = range.factory_begin();
	Range::identifier_factory last  = range.factory_end();
	
	for (; first != last; ++first) {

	    ExpandedIdentifier id = *first;
	    
	    int barrel_ec  = id[2];
	    int layer_disk = id[3];
	    int phi_module = id[4];
	    int eta_module = id[5];
	    int side  	   = id[6];
	    int strip      = id[7];
	    Identifier new_id =  sct_id.strip_id ( barrel_ec,  
						   layer_disk, 
						   phi_module, 
						   eta_module,      
						   side,
						   strip);

//    	    if (!are_values_ok) {
//  		std::cout << " id " << (string)id << " ok " << are_values_ok << std::endl;
//    	    }
	}
	std::cout << " Successful end of sct test   " << std::endl;
    }
    
}

//  static void
//  test_trt (const MultiRange& fullRange)
//  {
    
//      std::cout << " Testing trt ids'   " << std::endl;

//      std::cout << "range -> " << (std::string) fullRange << std::endl; 
    
//      // Try loop over fullRange and check each id that comes out

//      TRT_ID trt_id;

//      for (unsigned int i = 0; i < fullRange.size(); ++i) {
	
//  	Range range = fullRange[i];

//  	std::cout << " Range " << i << " " << (std::string)range << std::endl;

//  	Range::identifier_factory first = range.factory_begin();
//  	Range::identifier_factory last  = range.factory_end();
	
//  	for (; first != last; ++first) {

//  	    Identifier id = *first;
	    
//  	    int barrel_ec      = id[1];   
//  	    int layer_or_wheel = id[2];	 
//  	    int phi_module     = id[3];	 
//  	    int straw_layer    = id[4];	 
//  	    int straw          = id[5]; 
//  	    Identifier new_id =  trt_id.straw_id ( barrel_ec,  
//  						   layer_or_wheel, 
//  						   phi_module,
//  						   straw_layer,
//  						   straw);

//  //    	    if (!are_values_ok) {
//  //  		std::cout << " id " << (string)id << " ok " << are_values_ok << std::endl;
//  //    	    }
//  	}
//  	std::cout << " Successful end of trt test   " << std::endl;
//      }
    
//  }

static void
test_trt (const TRT_ID& trt_id)
{
    
    std::cout << " Testing trt ids   " << std::endl;

    // Save context for TRT module id
    IdContext	idContext = trt_id.module_context();    

    std::cout 
	<< "IDDICT of trt elements ids "
	<< std::endl;

    std::set<Identifier> ids;

    int nids = 0;
    
    {

	TRT_ID::const_id_iterator idIt       = trt_id.module_begin();
	TRT_ID::const_id_iterator idItEnd    = trt_id.module_end();
	
	// Check the sorted list of ids

	for (; idIt != idItEnd; ++idIt) {
  	    ids.insert((*idIt));
	    nids++;
	}
	
	idIt       = trt_id.module_begin();
	idItEnd    = trt_id.module_end();
	std::set<Identifier>::iterator    firstIds = ids.begin();
	std::set<Identifier>::iterator    lastIds  = ids.end();
	unsigned int ncount = 0;
	for(; firstIds != lastIds || idIt != idItEnd; ++firstIds, ++idIt, ncount++) {
	    if ((*firstIds) != (*idIt) ) {
		std::cout 
		    << " IDs not the same: N " << std::dec << ncount
		    << " id manually sorted " << trt_id.show_to_string((*firstIds))
		    << " id already sorted " << trt_id.show_to_string((*idIt))
		    << std::endl;
	    }
	}

	std::cout << " Found " << ncount << " module ids " << std::endl;
	std::cout << " Successful end of trt test   " << std::endl;
    }
}

static void
test_indet_ids (const IdDictDictionary* dictionary)
{

    // Build multirange for the valid set of identifiers

    MultiRange pixelFullRange;
    MultiRange sctFullRange;
    MultiRange trtFullRange;

    // Loop over all dictionary regions: 
    std::vector<IdDictRegion*>::const_iterator rit; 
    for (rit = dictionary->m_regions.begin (); rit != dictionary->m_regions.end (); ++rit) { 
	const IdDictRegion& region = *(*rit); 
 
	const IdDictFieldImplementation& impl = *(region.m_implementation.begin());

//  	std::cout << "  implement field #" << impl.range()->m_field->m_index << " " 
//  		  << impl.range()->m_field->m_name << " "
//  		  << (std::string) impl.field()  << std::endl;
	
	// Select only Pixels
  	if (impl.field().match(1)) {
	    std::cout << "region #" << region.m_index << " found pixels " << std::endl;

	    // Build range for wafers by hand - loop over range fields
	    // and skip the phi_ and eta_index

	    Range range;

	    std::vector <IdDictFieldImplementation>::const_iterator fit; 
	    for (fit = region.m_implementation.begin ();  
		 fit != region.m_implementation.end (); 
		 ++fit) { 
		const IdDictFieldImplementation& impl = *fit; 
 
		if ("phi_index" == impl.range()->m_field->m_name ||
		    "eta_index" == impl.range()->m_field->m_name) {
		    break;
		}
 
		range.add(impl.field());
	    } 

	    //Range range = region.build_range (); 
 
	    std::cout << "range -> " << (std::string) range << std::endl; 
	    
	    pixelFullRange.add(range);
	}
  	else if (impl.field().match(2)) {  // test SCT
	    std::cout << "region #" << region.m_index << " found sct " << std::endl;

	    // Build range for wafers by hand - loop over range fields
	    // and skip the phi_ and eta_index

	    Range range;

	    std::vector <IdDictFieldImplementation>::const_iterator fit; 
	    for (fit = region.m_implementation.begin ();  
		 fit != region.m_implementation.end (); 
		 ++fit) { 
		const IdDictFieldImplementation& impl = *fit; 
 
		if ("strip" == impl.range()->m_field->m_name) {
		    break;
		}
 
		range.add(impl.field());
	    } 

	    //Range range = region.build_range (); 
 
	    std::cout << "range -> " << (std::string) range << std::endl; 
	    
	    sctFullRange.add(range);
	}
  	else if (impl.field().match(3)) {  // test TRT
	    std::cout << "region #" << region.m_index << " found trt " << std::endl;

	    // Build range for wafers by hand - loop over range fields
	    // and skip the phi_ and eta_index

	    Range range;

	    std::vector <IdDictFieldImplementation>::const_iterator fit; 
	    for (fit = region.m_implementation.begin ();  
		 fit != region.m_implementation.end (); 
		 ++fit) { 
		const IdDictFieldImplementation& impl = *fit; 
 
		if ("straw" == impl.range()->m_field->m_name) {
		    break;
		}
 
		range.add(impl.field());
	    } 

	    //Range range = region.build_range (); 
 
	    std::cout << "range -> " << (std::string) range << std::endl; 
	    
	    trtFullRange.add(range);
	}
    }
    
    // Run tests
    test_pixel (pixelFullRange);
    test_sct (sctFullRange);
//      test_trt (trtFullRange);

}




int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  
    IdDictParser parser;  
  
    std::cout << "Reading file " << argv[1] << std::endl; 

    const IdDictMgr& idd = parser.parse (argv[1]);  
    std::cout << "Reading file " << argv[1] << std::endl; 
    TRT_ID trt_id;
    std::cout << "Reading file " << argv[1] << std::endl; 
    trt_id.initialize_from_dictionary(idd);
 
    const IdDictMgr::dictionary_map& dm = idd.get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
	const IdDictDictionary& dictionary = *((*it).second); 
 
	std::cout << "---- " << n << " ----------------------------" << std::endl; 
	std::cout << "Dictionary " << dictionary.m_name << std::endl; 
 
	std::vector<IdDictRegion*>::const_iterator rit; 
	for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) { 
	    const IdDictRegion& region = *(*rit); 
 
	    std::cout << "region #" << region.m_index << std::endl; 
 
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
		    std::endl; 
 
		bits += impl.bits(); 
	    } 
 
	    Range range = region.build_range (); 
 
	    std::cout << " -> " << (std::string) range <<  
		" (cardinality=" << range.cardinality () << ")" << 
		" (" << bits << " bits)" << std::endl; 
	} 
    } 
 
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
	    exit (1);
	}
    
	std::vector <IdDictLabel*>::const_iterator itLabels = partField->m_labels.begin();
	int inDetField = -1;
	std::cout << "Found field label size "  
		  << partField->m_labels.size()
		  << std::endl;

	for (; itLabels != partField->m_labels.end(); ++itLabels) {
	    std::cout << "Field label  "  
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
 
	    Identifier b(0);

	    dictionary->pack32 (id, 0, 6, b); 
 
	    std::cout << "b=[" << b << "]" << std::endl; 
 
	    ExpandedIdentifier id2;
	    dictionary->unpack (b, ExpandedIdentifier (), 6, id2); 
 
	    std::cout << "unpack->[" << (std::string) id2 << "]" << std::endl; 

	    test_trt (trt_id);
	    test_indet_ids (dictionary);
	}
    }
     
    return 0;  
}  
  
 
 
 
 
 

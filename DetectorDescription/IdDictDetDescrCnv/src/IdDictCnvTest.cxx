/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict converter package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IdDictCnvTest.cxx,v 1.6 2004-10-12 16:45:36 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#include "IdDictCnvTest.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
     
// Athena-related includes
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"


/////////////////////////////////////////////////////////////////////

void 	
IdDictCnvTest::tab 		(size_t level) const
{ 
  for (size_t i = 0; i < level; ++i) msg() << " "; 
} 

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictCnvTest::IdDictCnvTest(const std::string& name, ISvcLocator* pSvcLocator) 
    :
  AthAlgorithm(name, pSvcLocator)
{}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictCnvTest::~IdDictCnvTest()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

StatusCode 
IdDictCnvTest::initialize()
{
    ATH_MSG_INFO ( "Start initialization" );

    const IdDictManager* idDictMgr = nullptr;
    ATH_CHECK( detStore()->retrieve(idDictMgr, "IdDict") );
    ATH_MSG_DEBUG ( " Found the IdDictManager. " );
    ATH_MSG_DEBUG ( " Tag is " << idDictMgr->manager()->tag() );

    const IdDictMgr::dictionary_map& dm = idDictMgr->manager()->get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 

    if (dm.begin () == dm.end ()) {
 	// No dicts found
        ATH_MSG_ERROR ( "No dictionaries found!" );
 	return StatusCode::FAILURE;
    }
    
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
 	const IdDictDictionary& dictionary = *((*it).second); 
 
 
 	ATH_MSG_INFO( "---- " << n << " ----------------------------" );
 	std::string version = ("" != dictionary.m_version) ? dictionary.m_version : "default";
 	ATH_MSG_INFO ( "Dictionary " << dictionary.m_name 
                       << " version " << version );

 	if(dictionary.verify()) {
          ATH_MSG_INFO( "Dictionary verification is OK" );
 	}
 	else {
          ATH_MSG_FATAL( "Dictionary verification has failed: "
                         << dictionary.m_name << " multirange: "
                         << (std::string)dictionary.build_multirange() );
 	    return StatusCode::FAILURE;
 	}
	
 
// 	std::vector<IdDictRegion*>::const_iterator rit; 
// 	for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) { 
// 	    const IdDictRegion& region = *(*rit); 
 
// 	    log << MSG::DEBUG 
// 		<< "region #" << region.m_index 
// 		<< endmsg;
 
// 	    std::vector <IdDictFieldImplementation>::const_iterator fit; 
           
// 	    size_t width = 0; 
 
// 	    for (fit = region.m_implementation.begin ();  
// 		 fit != region.m_implementation.end (); 
// 		 ++fit) {
// 		const IdDictFieldImplementation& impl = *fit; 
 
// 		size_t w = impl.range()->m_field->m_name.size (); 
 
// 		if (w > width) width = w; 
// 	    } 
 
// 	    int bits = 0; 
 
// 	    for (fit = region.m_implementation.begin ();  
// 		 fit != region.m_implementation.end (); 
// 		 ++fit) { 
// 		const IdDictFieldImplementation& impl = *fit; 
 
// 		size_t w = impl.range()->m_field->m_name.size (); 
 
// 		log << MSG::DEBUG 
// 		    << "  implement field #" << impl.range()->m_field->m_index <<  
// 		    " " << impl.range()->m_field->m_name; 
 
// 		tab (width - w); 
 
// //  		int index = impl.field();
// //  		std::string label = impl.range()->m_field->labels[index]->m_name;
// //  		std::cout << " name " << label << " ";

// 		log << " -> " << (std::string) impl.field() 
// 		    <<  "/" << (std::string) impl.ored_field() 
// 		    <<  " (" << impl.bits() << " bits)" 
// 		    << endmsg;
 
// 		bits += impl.bits(); 
// 	    } 
 
// 	    Range range = region.build_range (); 
 
// 	    log << MSG::DEBUG 
// 		<< " -> " << (std::string) range <<  
// 		" (cardinality=" << range.cardinality () << ")" << 
// 		" (" << bits << " bits)" 
// 		<< endmsg;
// 	} 
//     } 
    }
     

    ATH_MSG_DEBUG ( " Get AtlasDetectorID. " );
    const AtlasDetectorID* atlasID;
    // Access AtlasID from detector store
    ATH_CHECK(  detStore()->retrieve(atlasID, "AtlasID") );
    ATH_MSG_DEBUG ( " Found the AtlasDetectorID. " );

    ATH_MSG_DEBUG ( " Begin tests for AtlasDetectorID. " );
    Identifier id = atlasID->indet();
    ATH_MSG_DEBUG ( " is indet " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_indet(id) );
    ATH_MSG_DEBUG ( " is sct " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_sct(id) );
    ATH_MSG_DEBUG ( " is lar " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_lar(id) );
    ATH_MSG_DEBUG ( " is muon " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_muon(id) );
    id = atlasID->trt();
    ATH_MSG_DEBUG ( " is indet " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_indet(id) );
    ATH_MSG_DEBUG ( " is sct " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_sct(id) );
    ATH_MSG_DEBUG ( " is trt " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_trt(id) );
    ATH_MSG_DEBUG ( " is lar " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_lar(id) );
    ATH_MSG_DEBUG ( " is muon " << atlasID->show_to_string(id) 
                    << " " << atlasID->is_muon(id) );

    ATH_MSG_INFO( "Initialization completed successfully" );

    return StatusCode::SUCCESS;

}


/////////////////////////////////////////////////////////////////////
StatusCode 
IdDictCnvTest::execute()
{
    return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

StatusCode 
IdDictCnvTest::finalize()
{
  ATH_MSG_DEBUG ( "finalize completed successfully" );
  return StatusCode::SUCCESS;
}



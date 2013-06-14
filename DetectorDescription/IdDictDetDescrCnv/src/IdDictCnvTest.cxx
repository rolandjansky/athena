/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict converter package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: IdDictCnvTest.cxx,v 1.6 2004-10-12 16:45:36 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDictCnvTest.h"

// Id-related includes
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
     
// Athena-related includes
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"


//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

/////////////////////////////////////////////////////////////////////

void 	
IdDictCnvTest::tab 		(size_t level) const
{ 
    MsgStream  log(msgSvc(),name());
    for (size_t i = 0; i < level; ++i) log << " "; 
} 

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

IdDictCnvTest::IdDictCnvTest(const std::string& name, ISvcLocator* pSvcLocator) 
    :
    Algorithm(name, pSvcLocator)
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
    MsgStream  log(msgSvc(),name());

    log << MSG::INFO 
	<< "Start initialization" 
	<< endreq;

    // Access InDet_DetDescrManager from detector store
    StoreGateSvc* detStore = 0;
    const IdDictManager* idDictMgr;
    StatusCode sc = service( "DetectorStore", detStore );
    if ( sc.isSuccess( ) ) {
	// Get the dictionary manager from the detector store
	sc = detStore->retrieve(idDictMgr, "IdDict");
	if (sc.isFailure()) {
	    log << MSG::FATAL << "Could not get IdDictManager !" << endreq;
	    return StatusCode::FAILURE;
	} 
	else {
	    log << MSG::DEBUG << " Found the IdDictManager. " << endreq;
	    log << MSG::DEBUG << " Tag is " << idDictMgr->manager()->tag() << endreq;
	}
    } 
    else {
	log << MSG::ERROR << "Could not locate DetectorStore" << endreq;
	return StatusCode::FAILURE;
    }


    const IdDictMgr::dictionary_map& dm = idDictMgr->manager()->get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 

    if (dm.begin () == dm.end ()) {
 	// No dicts found
 	log << MSG::ERROR << "No dictionaries found!" << endreq;
 	return StatusCode::FAILURE;
    }
    
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
 	const IdDictDictionary& dictionary = *((*it).second); 
 
 
 	log << MSG::INFO 
 	    << "---- " << n << " ----------------------------" 
 	    << endreq;
 	std::string version = ("" != dictionary.m_version) ? dictionary.m_version : "default";
 	log << MSG::INFO 
 	    << "Dictionary " << dictionary.m_name 
 	    << " version " << version
 	    << endreq;

 	if(dictionary.verify()) {
 	    log << MSG::INFO
		<< "Dictionary verification is OK"
 		<< endreq;
 	}
 	else {
 	    log << MSG::FATAL
 		<< "Dictionary verification has failed: "
 		<< dictionary.m_name << " multirange: "
 		<< (std::string)dictionary.build_multirange()
 		<< endreq;
 	    return StatusCode::FAILURE;
 	}
	
 
// 	std::vector<IdDictRegion*>::const_iterator rit; 
// 	for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) { 
// 	    const IdDictRegion& region = *(*rit); 
 
// 	    log << MSG::DEBUG 
// 		<< "region #" << region.m_index 
// 		<< endreq;
 
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
// 		    << endreq;
 
// 		bits += impl.bits(); 
// 	    } 
 
// 	    Range range = region.build_range (); 
 
// 	    log << MSG::DEBUG 
// 		<< " -> " << (std::string) range <<  
// 		" (cardinality=" << range.cardinality () << ")" << 
// 		" (" << bits << " bits)" 
// 		<< endreq;
// 	} 
//     } 
    }
     

    log << MSG::DEBUG << " Get AtlasDetectorID. " << endreq;
    const AtlasDetectorID* atlasID;
    // Access AtlasID from detector store
    sc = detStore->retrieve(atlasID, "AtlasID");
    if (sc.isFailure()) {
	log << MSG::FATAL << "Could not get AtlasDetectorID !" << endreq;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the AtlasDetectorID. " << endreq;
    }


    log << MSG::DEBUG << " Begin tests for AtlasDetectorID. " << endreq;
    Identifier id = atlasID->indet();
    log << MSG::DEBUG << " is indet " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_indet(id)
	<< endreq;
    log << MSG::DEBUG << " is sct " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_sct(id)
	<< endreq;
    log << MSG::DEBUG << " is lar " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_lar(id)
	<< endreq;
    log << MSG::DEBUG << " is muon " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_muon(id)
	<< endreq;
    id = atlasID->trt();
    log << MSG::DEBUG << " is indet " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_indet(id)
	<< endreq;
    log << MSG::DEBUG << " is sct " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_sct(id)
	<< endreq;
    log << MSG::DEBUG << " is trt " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_trt(id)
	<< endreq;
    log << MSG::DEBUG << " is lar " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_lar(id)
	<< endreq;
    log << MSG::DEBUG << " is muon " << atlasID->show_to_string(id) 
	<< " " << atlasID->is_muon(id)
	<< endreq;

    log << MSG::INFO
	<< "Initialization completed successfully"
	<< endreq;

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

    MsgStream  log(msgSvc(),name());
    log << MSG::DEBUG 
	<< "finalize completed successfully" 
	<< endreq;

    return StatusCode::SUCCESS;

}



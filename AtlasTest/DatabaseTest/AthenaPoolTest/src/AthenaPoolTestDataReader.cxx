/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestAttrWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, reads AthenaPoolData
 * objects from the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestDataReader.cxx,v 1.25 2009-03-30 17:28:18 schaffer Exp $
 *
 */

// INCLUDE HEADER FILES:

#include "AthenaPoolTestDataReader.h"
#include "AthenaPoolTestData/IAthenaPoolTestCollection.h"
#include "AthenaPoolTestData/AthenaPoolTestA.h"
#include "AthenaPoolTestData/AthenaPoolTestB.h"
#include "AthenaPoolTestData/AthenaPoolTestC.h"
#include "AthenaPoolTestData/AthenaPoolTestD.h"
#include "AthenaPoolTestData/AthenaPoolTestMatrix.h"
#include "AthenaPoolTestData/AthenaPoolTestMatrix.h"
#include "AthenaPoolTestData/AthenaPoolTestMap.h"
#include "AthenaPoolTestData/dummy_A.h"
#include "AthenaPoolTestData/dummy_E.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include <algorithm>
#include <sstream>

AthenaPoolTestDataReader::AthenaPoolTestDataReader(const std::string& name,
					 ISvcLocator* pSvcLocator):
    Algorithm(name, pSvcLocator)
{}

AthenaPoolTestDataReader::~AthenaPoolTestDataReader()
{}

StatusCode AthenaPoolTestDataReader::initialize()
{
    MsgStream log( messageService(), name());

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure())
	{
	    log << MSG::ERROR
		<< "Unable to get pointer to StoreGate Service"
		<< endreq;
	    return sc;
	}
    return StatusCode::SUCCESS; 
      
}

StatusCode AthenaPoolTestDataReader::execute()
{ 

    StatusCode sc;

    MsgStream log( messageService(), name() );   
    log << MSG::DEBUG << "Executing AthenaPoolTestDataReader" << endreq;
   

    // Retrieve collections

    // A's only
 
    std::string test1;
    std::string test2;
    bool error = false;
    
    const IAthenaPoolTestCollection* col;
    sc=m_storeGate->retrieve(col, "AthenaPoolTestCollection_1");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolTestCollection_1 object " 
	    << endreq;
	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved collection 1" <<  endreq;
    }
    
	
    log << MSG::DEBUG << "Collection of A's" <<  endreq;
    IAthenaPoolTestCollection::const_iterator it   = col->begin();
    IAthenaPoolTestCollection::const_iterator last = col->end();
    int i = 0; 
    for (; it != last; ++it, ++i) {
	log << MSG::DEBUG << "Read:      " << (*it)->printWhoYouAre() <<  endreq;
	AthenaPoolTestA data(i+1, i+2);
	log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	test1 = (*it)->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
	    log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
	    error = true;
	}
	
    }
				    

    // B's only
 
    sc=m_storeGate->retrieve(col, "AthenaPoolTestCollection_2");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolTestCollection_2 object " 
	    << endreq;
	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved collection 2" <<  endreq;
    }
    
    log << MSG::DEBUG << "Collection of B's" <<  endreq;
    it   = col->begin();
    last = col->end();
    i = 0; 
    for (; it != last; ++it, ++i) {
	log << MSG::DEBUG << "Read:      " << (*it)->printWhoYouAre() <<  endreq;
	AthenaPoolTestB data(i+11, i+12);
	log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	test1 = (*it)->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
	    log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
	    error = true;
	}
    }
				    

    // A's and B's 
 
    sc=m_storeGate->retrieve(col, "AthenaPoolTestCollection_3");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolTestCollection_3 object " 
	    << endreq;
	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved collection 3" <<  endreq;
    }

    log << MSG::DEBUG << "Collection of A's and B's" <<  endreq;
    it   = col->begin();
    last = col->end();
    i = 0; 
    for (; it != last; ++it, ++i) {
	log << MSG::DEBUG << "Read:      " << (*it)->printWhoYouAre() <<  endreq;
	if (i < 3) {
	    AthenaPoolTestA data(i+21, i+22);
	    log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	    test1 = (*it)->printWhoYouAre();
	    test2 = data.printWhoYouAre();
	    if (test1 != test2) {
		log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
		error = true;
	    }
	}
	else {
	    AthenaPoolTestB data(i+21, i+22);
	    log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	    test1 = (*it)->printWhoYouAre();
	    test2 = data.printWhoYouAre();
	    if (test1 != test2) {
		log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
		error = true;
	    }
	}
    }


    // C's only
 
    sc=m_storeGate->retrieve(col, "AthenaPoolTestCollection_4");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolTestCollection_4 object " 
	    << endreq;
	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved collection 4" <<  endreq;
    }

    log << MSG::DEBUG << "Collection of C's" <<  endreq;
    it   = col->begin();
    last = col->end();
    i = 0; 
    for (; it != last; ++it, ++i) {
	log << MSG::DEBUG << "Read:      " << (*it)->printWhoYouAre() <<  endreq;
	AthenaPoolTestC data(0, 0, i+3, i+4);
	log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	test1 = (*it)->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
	    log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
	    error = true;
	}
	
    }
				    

    // D's only
 
    sc=m_storeGate->retrieve(col, "AthenaPoolTestCollection_5");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolTestCollection_5 object " 
	    << endreq;
	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved collection 5" <<  endreq;
    }

    log << MSG::DEBUG << "Collection of D's" <<  endreq;
    it   = col->begin();
    last = col->end();
    i = 0; 
    for (; it != last; ++it, ++i) {
	log << MSG::DEBUG << "Read:      " << (*it)->printWhoYouAre() <<  endreq;
	AthenaPoolTestD data(0, 0, i+13, i+14, i+15, i+16);
	log << MSG::DEBUG << "Should be: " << data.printWhoYouAre() <<  endreq << endreq;
	test1 = (*it)->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
	    log << MSG::ERROR << "Incorrect data:  in/out " << test1 << " " << test2 << endreq;
	    error = true;
	}
	
    }
				    

    // Test for CLHEP matrix

    // Now create a write 10 Hit objects
    //bool readMatrix = false;
    bool readMatrix = true;
    if (readMatrix) {
	const AthenaPoolTestMatrix* matrix;
	for ( int i = 0; i < 3; ++i ) {
	    std::stringstream stream;
	    stream << "AthenaPoolTestMatrix_" << i;
	    std::string key(stream.str());
	    sc = m_storeGate->retrieve(matrix, key);
	    if ( sc.isFailure() ) {
		log << MSG::ERROR
		    << "Could not retrieve " << key << " data object"
		    << endreq;
		return StatusCode::FAILURE;
	    }

	    
	    log << MSG::DEBUG << "Retrieved matrix " << i + 1 << " " << key << endreq;

	    const HepGeom::Point3D<double>& point  = matrix->point();
	    const HepGeom::Point3D<double>& point1 = matrix->point1();
	    log << MSG::DEBUG << "   Points (by value,by ptr) x,y,z : " 
		<< point.x() << " " 
		<< point.y() << " " 
		<< point.z() << " " 
		<< ", x,y,z " 
		<< point1.x() << " " 
		<< point1.y() << " " 
		<< point1.z() << " " 
		<< endreq;
	    const CLHEP::HepMatrix& smallMatrix = matrix->smallMatrix();
	    log << MSG::DEBUG << "   Small matrix : ( " << smallMatrix.num_row() << ", " << smallMatrix.num_col() << " ) " << endreq;
	    log << MSG::VERBOSE << "   values: ";
	    for (int i = 0; i < smallMatrix.num_row(); ++i) {
		for (int j = 0; j < smallMatrix.num_col(); ++ j) {
		    log << MSG::VERBOSE << smallMatrix[i][j] << ", ";
		}
	    }
	    log << MSG::VERBOSE << endreq;
	    
	    const CLHEP::HepMatrix& bigMatrix = matrix->bigMatrix();
	    log << MSG::DEBUG << "   Big matrix : ( " << bigMatrix.num_row() << ", " << bigMatrix.num_col() << " ) " << endreq;
	    log << MSG::VERBOSE << "   values: ";
	    for (int i = 0; i < bigMatrix.num_row(); ++i) {
		for (int j = 0; j < bigMatrix.num_col(); ++ j) {
		    log << MSG::VERBOSE << bigMatrix[i][j] << ", ";
		}
	    }
	    log << MSG::VERBOSE << endreq;
	    log << MSG::DEBUG << "   Other matrices : " << endreq;
	    for ( int ii = 0; ii < matrix->numberOfMatrices(); ++ii ) {
		const CLHEP::HepMatrix& matrx = matrix->matrix( ii );
		log << MSG::DEBUG << "                  ( " << matrx.num_row() << ", " << matrx.num_col() << " ) " << endreq;
		log << MSG::VERBOSE << "   values: ";
		for (int i = 0; i < matrx.num_row(); ++i) {
		    for (int j = 0; j < matrx.num_col(); ++ j) {
			log << MSG::VERBOSE << matrx[i][j] << ", ";
		    }
		}
		log << MSG::VERBOSE << endreq;
	    }

	    // print out matrix maps
	    AthenaPoolTestMatrix::matrixMaps_t mats = matrix->matrixMaps();

	    log << MSG::DEBUG << "   Matrix maps : " << mats.size() << endreq;
	    std::map<unsigned int, CLHEP::HepMatrix>& mp0 = mats[0];
	    std::map<unsigned int, CLHEP::HepMatrix>& mp1 = mats[1];
	    std::map<unsigned int, CLHEP::HepMatrix>::const_iterator itmp =  mp0.begin();
	    log << MSG::DEBUG << "   Key, row, column " << (*itmp).first << " " 
		<< (*itmp).second.num_row() << ", " << (*itmp).second.num_col() << endreq;
	    ++itmp;
	    log << MSG::DEBUG << "   Key, row, column " << (*itmp).first << " " 
		<< (*itmp).second.num_row() << ", " << (*itmp).second.num_col() << endreq;
	    itmp =  mp1.begin();
	    log << MSG::DEBUG << "   Key, row, column " << (*itmp).first << " " 
		<< (*itmp).second.num_row() << ", " << (*itmp).second.num_col() << endreq;
	    ++itmp;
	    log << MSG::DEBUG << "   Key, row, column " << (*itmp).first << " " 
		<< (*itmp).second.num_row() << ", " << (*itmp).second.num_col() << endreq;

	    // Print out the transform:
	    log << MSG::DEBUG << "   Matrix transform: " << endreq;
	    const HepGeom::Transform3D& trans  = matrix->trans();
	    log << MSG::DEBUG << "   Matrix transform: " << endreq;
	    log << MSG::DEBUG << "  xx " << trans(0,0) << endreq;
	    log << MSG::DEBUG << "  xy " << trans(0,1)  << endreq;
	    log << MSG::DEBUG << "  xz " << trans(0,2)  << endreq;
	    log << MSG::DEBUG << "  yx " << trans(1,0)  << endreq;
	    log << MSG::DEBUG << "  yy " << trans(1,1)  << endreq;
	    log << MSG::DEBUG << "  yz " << trans(1,2)  << endreq;
	    log << MSG::DEBUG << "  zx " << trans(2,0)  << endreq;
	    log << MSG::DEBUG << "  zy " << trans(2,1)  << endreq;
	    log << MSG::DEBUG << "  zz " << trans(2,2)  << endreq;
	    log << MSG::DEBUG << "  dx " << trans(3,0)  << endreq;
	    log << MSG::DEBUG << "  dy " << trans(3,1)  << endreq;
	    log << MSG::DEBUG << "  dz " << trans(3,2)  << endreq;

	}
    }
    


    // Read in nested collections

    bool readMap = true;
    const AthenaPoolTestMap* tmapPtr;
    sc=m_storeGate->retrieve(tmapPtr, "AthenaPoolMap");
    if (sc.isFailure()) {
	log << MSG::ERROR
	    << "Unable to retrieve AthenaPoolMap object " 
	    << endreq;
	readMap = false;
//	return sc;
    }
    else {
	log << MSG::DEBUG << "Retrieved AthenaPoolMap" <<  endreq;
    }

    if (readMap) {
	
	const AthenaPoolTestMap& tmap    = *tmapPtr;

	typedef AthenaPoolTestMapData::Deposit deposit_t;
	typedef std::vector<deposit_t>         deposit_vec;

	deposit_vec deposits;
    
	//deposit_t deposit;

	// Printout data
	AthenaPoolTestMap::const_iterator itm  = tmap.begin();
	AthenaPoolTestMap::const_iterator endm = tmap.end();
	for (; itm != endm; ++itm) {

	    log << MSG::DEBUG << "AthenaPoolTestMap: key " << (*itm).first 
		<< "  word " << (*itm).second.word()
		<< "  Deposits: (barCode, evtIndx, wgt) ";

	    deposits.clear();
	    (*itm).second.deposits(deposits);
	    for (unsigned int i = 0; i < deposits.size(); ++i) {
		AthenaPoolTestMapDataLink& link = deposits[i].first;
		log << MSG::DEBUG << link.barcode() << " " 
		    << link.eventIndex() << " " 
		    << deposits[i].second << " ";
	    }
	    log << MSG::DEBUG << endreq;
	}
    }
    

    // Read back dummy classes
    
    const dummy_A* dummyA;
    sc = m_storeGate->retrieve(dummyA, "Dummy_A");
    if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not retrieve Dummy_A"
 	    << endreq;
// 	return StatusCode::FAILURE;
    }
    else {
	log << MSG::DEBUG << "Read back dummy_A class " << endreq;
    }
    const std::vector<dummy_B>& b = dummyA->dummy();
    log << MSG::DEBUG << "Dummy A: size B " << b.size() << endreq;
    for (unsigned int i = 0; i < b.size(); ++i) {
	const std::vector<dummy_C>& c = b[i].dummy();
	log << MSG::DEBUG << "Dummy B: size C " << c.size() << endreq;
	for (unsigned int j = 0; j < c.size(); ++j) {
	    log << MSG::DEBUG << "i,c: " << j << " " << c[j].value() << endreq;
	}
    }
    
    // dummy_E, D

    const dummy_E* dummyE;
    sc = m_storeGate->retrieve(dummyE, "Dummy_E");
    if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not retrieve Dummy_E"
 	    << endreq;
 	return StatusCode::FAILURE;
    }
    else {
	log << MSG::DEBUG << "Read back dummy_E class " << endreq;
    }
    const std::vector<const dummy_D*>& d = dummyE->dummy();
    log << MSG::DEBUG << "Dummy E: size D " << d.size() << endreq;
    for (unsigned int i = 0; i < d.size(); ++i) {
	log << MSG::DEBUG << "Dummy D: value " << d[i]->value() << endreq;
    }


    // Read PileupEventInfo

    // Normal EventInfo
    const EventInfo * evt = 0;
    sc = m_storeGate->retrieve( evt, "McEventInfo" );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get event info" << endreq;      
 	return StatusCode::FAILURE;
    }
    else {
 	log << MSG::DEBUG << "Normal EventInfo"
 	    << endreq;
 	log << MSG::DEBUG << "Event ID: ["
            << evt->event_ID()->event_number() << ":"
 	    << evt->event_ID()->time_stamp() << "] "
 	    << endreq;
 	log << MSG::DEBUG << *(evt->event_ID())
 	    << endreq;
 	log << MSG::DEBUG << "Event type: user type "
 	    << evt->event_type()->user_type();
        for (unsigned int i = 0; i < evt->event_type()->n_mc_event_weights (); ++i) {
            log << MSG::DEBUG << " weight " << i << ": " 
                << evt->event_type()->mc_event_weight(i);
        }
 	log << MSG::DEBUG << endreq;

        // Check interactions per crossing
        log << MSG::DEBUG << "Actual interatctions per crossing: " 
            << evt->actualInteractionsPerCrossing() << " and ave: " 
            << evt->averageInteractionsPerCrossing() << endreq;

        // We only loop up to Lumi and not nDets since Lumi is set separately
        for (unsigned int i = 0; i < EventInfo::Lumi; ++i) {
            log << MSG::DEBUG << "Subdet: " << i << " flags        " 
                << MSG::hex << evt->eventFlags(EventInfo::EventFlagSubDet(i)) << MSG::dec << endreq;
            EventInfo::EventFlagErrorState error = evt->errorState(EventInfo::EventFlagSubDet(i));
            if (error == EventInfo::NotSet)
                log << MSG::DEBUG << "Subdet: " << i << " error state: NotSet " << endreq;
            else if (error == EventInfo::Warning)
                log << MSG::DEBUG << "Subdet: " << i << " error state: Warning " << endreq;
            else if (error == EventInfo::Error)
                log << MSG::DEBUG << "Subdet: " << i << " error state: Error " << endreq;
        }
    }

//     Cannot have symlink to pileupeventinfo - would make two in SG
//     and app mgr/event loop cannot handle more than one.
//     // PileupInfo as EventInfo
//     evt = 0;
//     sc = m_storeGate->retrieve( evt, "OverlayEvent" );
//     if ( sc.isFailure() ) {
//  	log << MSG::INFO << "  Could not get pileup event info as event info" << endreq;      
// // 	return StatusCode::FAILURE;
//     }
//     else {
//  	log << MSG::DEBUG << "PileUpEventInfo as EventInfo"
//  	    << endreq;
//  	log << MSG::DEBUG << "Event ID: ["
//  	    << evt->event_ID()->run_number()   << ","
//  	    << evt->event_ID()->event_number() << ":"
//  	    << evt->event_ID()->time_stamp() << "] "
//  	    << endreq;
//  	log << MSG::DEBUG << "Event type: user type "
//  	    << evt->event_type()->user_type() 
//  	    << endreq;
//     }

    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = 0;
    sc = m_storeGate->retrieve( pevt, "OverlayEvent" );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get pileup event info" << endreq;      
 	return StatusCode::FAILURE;
    }
    else {
 	log << MSG::DEBUG << "PileUpEventInfo"
 	    << endreq;
 	log << MSG::DEBUG << "Event ID: ["
 	    << pevt->event_ID()->run_number()   << ","
            << pevt->event_ID()->event_number() << ":"
 	    << pevt->event_ID()->time_stamp() << "] "
 	    << endreq;
 	log << MSG::DEBUG << *(pevt->event_ID())
 	    << endreq;
 	log << MSG::DEBUG << "Event type: user type "
 	    << pevt->event_type()->user_type()
 	    << endreq;

        log << MSG::DEBUG << "Trigger info: status " << pevt->trigger_info()->statusElement()
            << " extLvl1ID " << pevt->trigger_info()->extendedLevel1ID()
            << " lvl1Type  " << pevt->trigger_info()->level1TriggerType()
            << " lvl1Info  ";
        for (unsigned int i = 0; i < pevt->trigger_info()->level1TriggerInfo().size(); ++i) {
            log << MSG::DEBUG << pevt->trigger_info()->level1TriggerInfo()[i] << " ";
        }
        log << MSG::DEBUG << "lvl2Info ";
        for (unsigned int i = 0; i < pevt->trigger_info()->level2TriggerInfo().size(); ++i) {
            log << MSG::DEBUG << pevt->trigger_info()->level2TriggerInfo()[i] << " ";
        }
        log << MSG::DEBUG << "EventFilterInfo ";
        for (unsigned int i = 0; i < pevt->trigger_info()->eventFilterInfo().size(); ++i) {
            log << MSG::DEBUG << pevt->trigger_info()->eventFilterInfo()[i] << " ";
        }
        log << MSG::DEBUG << "EventFilterInfo ";
        for (unsigned int i = 0; i < pevt->trigger_info()->streamTags().size(); ++i) {
            log << MSG::DEBUG << " i " << i << " name " << pevt->trigger_info()->streamTags()[i].name();
            log << MSG::DEBUG << " type " << pevt->trigger_info()->streamTags()[i].type();
            log << MSG::DEBUG << " ObeyLumi " << pevt->trigger_info()->streamTags()[i].obeysLumiblock();
        }
        log << MSG::DEBUG << endreq;



	// Get normal event info as a sub-event info
	log << MSG::DEBUG << "SubEventInfos"
	    << endreq;
	PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
	PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
	if (it == end) {
	    log << MSG::DEBUG << "None found" << endreq;
	}
	for (; it != end; ++it) {
	    const EventInfo* sevt = (*it).pSubEvt;
	    log << MSG::DEBUG << "Time, index " 
		<< (*it).time() << " " << (*it).index()
		<< endreq;
	    if (sevt) {
		log << MSG::DEBUG << "Event ID: ["
		    << sevt->event_ID()->run_number()   << ","
                    << sevt->event_ID()->event_number() << ":"
		    << sevt->event_ID()->time_stamp() << "] "
		    << endreq;
		log << MSG::DEBUG << "Event type: user type "
		    << sevt->event_type()->user_type()
		    << endreq;
                log << MSG::DEBUG << "Pileup time, index type "
                    << it->time() << " " << it->index()
                    << endreq;
	    }
	    else {
		log << MSG::DEBUG << "Subevent is null ptr "
		    << endreq;
	    }
	}
    }


    if (error) return (StatusCode::FAILURE);
				    
    return StatusCode::SUCCESS;
   
}

StatusCode AthenaPoolTestDataReader::finalize() 
{
    return StatusCode::SUCCESS;
}



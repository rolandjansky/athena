/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestDataWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, writes AthenaPoolData
 * objects to the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestDataWriter.cxx,v 1.27 2009-03-30 17:28:18 schaffer Exp $
 *
 */

// INCLUDE HEADER FILES:

#include "AthenaPoolTestDataWriter.h"

#include "AthenaPoolTestData/IAthenaPoolTestCollection.h"
#include "AthenaPoolTestData/AthenaPoolTestA.h"
#include "AthenaPoolTestData/AthenaPoolTestB.h"
#include "AthenaPoolTestData/AthenaPoolTestC.h"
#include "AthenaPoolTestData/AthenaPoolTestD.h"
#include "AthenaPoolTestData/AthenaPoolTestMatrix.h"
#include "AthenaPoolTestData/AthenaPoolTestMap.h"
#include "AthenaPoolTestData/dummy_A.h"
#include "AthenaPoolTestData/dummy_E.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"

#include <sstream>

AthenaPoolTestDataWriter::AthenaPoolTestDataWriter(const std::string& name,
						   ISvcLocator* pSvcLocator):
    Algorithm(name, pSvcLocator),
    m_partialCreate(false),
    m_readOtherHalf(false),
    m_readFirstHalf(false)
{
    declareProperty("PartialCreate", m_partialCreate);
    declareProperty("ReadOtherHalf", m_readOtherHalf);
    declareProperty("ReadFirstHalf", m_readFirstHalf);
}

AthenaPoolTestDataWriter::~AthenaPoolTestDataWriter()
{}

StatusCode AthenaPoolTestDataWriter::initialize()
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

StatusCode AthenaPoolTestDataWriter::execute()
{ 

    StatusCode sc;

    MsgStream log( messageService(), name() );   
    log << MSG::DEBUG << "Executing AthenaPoolTestDataWriter" << endreq;
   

    // Tests:
    //

    //   collections 1-3: test of storing abstract base collections
    //                    and data objects
    //   collections 4,5: test of transient base class attributes
    //   collection    6: test of storing DataVector<const obj>*

    IAthenaPoolTestCollection* col1 = new IAthenaPoolTestCollection();
    IAthenaPoolTestCollection* col2 = new IAthenaPoolTestCollection();
    IAthenaPoolTestCollection* col3 = new IAthenaPoolTestCollection();
    IAthenaPoolTestCollection* col4 = 0;
    IAthenaPoolTestCollection* col5 = 0;
    if (!m_partialCreate) {
	col4 = new IAthenaPoolTestCollection();
	col5 = new IAthenaPoolTestCollection();
    }
    

    IAthenaPoolTestData* data;

    // Create AthenaPoolTestA's only
    for (int i = 0; i < 5; ++i) {
	data = new AthenaPoolTestA(i+1, i+2);
	col1->push_back(data);
    }
    
	
    // Create AthenaPoolTestB's only
    for (int i = 10; i < 15; ++i) {
	data = new AthenaPoolTestB(i+1, i+2);
	col2->push_back(data);
    }
    
    // Create AthenaPoolTestA's and B's
    for (int i = 20; i < 26; ++i) {
	if (i < 23) {
	    data = new AthenaPoolTestA(i+1, i+2);
	}
	else {
	    data = new AthenaPoolTestB(i+1, i+2);
	}
	col3->push_back(data);
    }
    
    log << MSG::DEBUG << "Collection of A's" <<  endreq;

    IAthenaPoolTestCollection::const_iterator it   = col1->begin();
    IAthenaPoolTestCollection::const_iterator last = col1->end();
    for (; it != last; ++it) {
	log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
    }
				    
    log << MSG::DEBUG << "Collection of B's" <<  endreq;
    it   = col2->begin();
    last = col2->end();
    for (; it != last; ++it) {	
	log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
    }
				    
    log << MSG::DEBUG << "Collection of A's and B's" <<  endreq;
    it   = col3->begin();
    last = col3->end();
    for (; it != last; ++it) {
	log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
    }
				    
    if (!m_partialCreate) {
	// Create AthenaPoolTestC's
	for (int i = 0; i < 5; ++i) {
	    data = new AthenaPoolTestC(i+1, i+2, i+3, i+4);
	    col4->push_back(data);
	}
    
	
	// Create AthenaPoolTestD's only
	for (int i = 10; i < 15; ++i) {
	    data = new AthenaPoolTestD(i+1, i+2, i+3, i+4, i+5, i+6);
	    col5->push_back(data);
	}
    
	log << MSG::DEBUG << "Collection of C's" <<  endreq;
	it   = col4->begin();
	last = col4->end();
	for (; it != last; ++it) {
	    log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
	}
				    
	log << MSG::DEBUG << "Collection of D's" <<  endreq;
	it   = col5->begin();
	last = col5->end();
	for (; it != last; ++it) {
	    log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
	}
    }
    
 

    if (m_partialCreate && m_readOtherHalf) {
	const IAthenaPoolTestCollection* col = 0;
	sc = m_storeGate->retrieve(col, "AthenaPoolTestCollection_4");
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not retrieve AthenaPoolTestCollection_4 data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    log << MSG::DEBUG << "Retrieved col4" <<  endreq;
	}

	log << MSG::DEBUG << "Collection of C's" <<  endreq;
	it   = col->begin();
	last = col->end();
	for (; it != last; ++it) {
	    log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
	}
				    
	sc = m_storeGate->retrieve(col, "AthenaPoolTestCollection_5");
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not retrieve AthenaPoolTestCollection_5 data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    log << MSG::DEBUG << "Retrieved col5" <<  endreq;
	}
	
	log << MSG::DEBUG << "Collection of D's" <<  endreq;
	it   = col->begin();
	last = col->end();
	for (; it != last; ++it) {
	    log << MSG::DEBUG << (*it)->printWhoYouAre() <<  endreq;
	}
    }

    if (m_partialCreate && m_readFirstHalf) {
	// Try to first read the collections that we want to write
	// again => this should give and error!!
	const IAthenaPoolTestCollection* col = 0;
	sc = m_storeGate->retrieve(col, "AthenaPoolTestCollection_1");
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not retrieve AthenaPoolTestCollection_1 data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    log << MSG::DEBUG << "Retrieved col1" <<  endreq;
	}
	sc = m_storeGate->retrieve(col, "AthenaPoolTestCollection_2");
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not retrieve AthenaPoolTestCollection_2 data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    log << MSG::DEBUG << "Retrieved col2" <<  endreq;
	}
	sc = m_storeGate->retrieve(col, "AthenaPoolTestCollection_3");
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not retrieve AthenaPoolTestCollection_3 data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    log << MSG::DEBUG << "Retrieved col3" <<  endreq;
	}

    }


    // Register collections in the Gaudi Transient Store:
 
     sc = m_storeGate->record(col1, "AthenaPoolTestCollection_1");
     if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register AthenaPoolTestCollection_1 data object"
 	    << endreq;
 	return StatusCode::FAILURE;
     }
     else {
 	log << MSG::DEBUG << "Recorded col1" <<  endreq;
     }
     sc = m_storeGate->record(col2, "AthenaPoolTestCollection_2");
     if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register AthenaPoolTestCollection_2 data object"
 	    << endreq;
 	return StatusCode::FAILURE;
     }
     else {
 	log << MSG::DEBUG << "Recorded col2" <<  endreq;
     }
     sc = m_storeGate->record(col3, "AthenaPoolTestCollection_3");
     if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register AthenaPoolTestCollection_3 data object"
 	    << endreq;
 	return StatusCode::FAILURE;
     }
     else {
 	log << MSG::DEBUG << "Recorded col3" <<  endreq;
     }
     if (!m_partialCreate) {
 	sc = m_storeGate->record(col4, "AthenaPoolTestCollection_4");
 	if ( sc.isFailure() ) {
 	    log << MSG::ERROR
 		<< "Could not register AthenaPoolTestCollection_4 data object"
 		<< endreq;
 	    return StatusCode::FAILURE;
 	}
 	else {
 	    log << MSG::DEBUG << "Recorded col4" <<  endreq;
 	}

 	sc = m_storeGate->record(col5, "AthenaPoolTestCollection_5");
 	if ( sc.isFailure() ) {
 	    log << MSG::ERROR
 		<< "Could not register AthenaPoolTestCollection_5 data object"
 		<< endreq;
 	    return StatusCode::FAILURE;
 	}
 	else {
 	    log << MSG::DEBUG << "Recorded col5" <<  endreq;
 	}
	
     }


    // Test for CLHEP matrix

    // Now create a write 10 Hit objects
    int nd = 0;
    double k = 7.;
    for ( int i = 0; i < 3; ++i ) {

	AthenaPoolTestMatrix* matrix = new AthenaPoolTestMatrix;
	for ( int j = 0; j < 2; ++j ) {
	    CLHEP::HepMatrix* mat = new CLHEP::HepMatrix( 4 + j, 2 + 2*i );
	    CLHEP::HepMatrix& matrx = *mat;
	    for (int i = 0; i < matrx.num_row(); ++i) {
		for (int j = 0; j < matrx.num_col(); ++j) {
		    matrx[i][j] = k;
		    k *= 5.0;
		}
	    }
	    matrix->addMatrix( mat );
	}

	// set transfor m
	HepGeom::Transform3D trans1(HepGeom::Point3D<double>( i, 0, 0), 
			      HepGeom::Point3D<double>( 0, 1, 0),
			      HepGeom::Point3D<double>( 0, 0, 1),
			      HepGeom::Point3D<double>(-i, 0, 0),
			      HepGeom::Point3D<double>( 0, 1, 0),
			      HepGeom::Point3D<double>( 0, 0, 1));
	matrix->setTransform(trans1);
	
	std::stringstream stream;
	stream << "AthenaPoolTestMatrix_" << i;
	std::string key(stream.str());
	sc = m_storeGate->record(matrix, key);
	if ( sc.isFailure() ) {
	    log << MSG::ERROR
		<< "Could not register " << key << " data object"
		<< endreq;
	    return StatusCode::FAILURE;
	}
	log << MSG::DEBUG << "Registering matrix " << i + 1 << " " << key << endreq;

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
	log << MSG::VERBOSE  << endreq;
	nd += smallMatrix.num_row()*smallMatrix.num_col();
	
	const CLHEP::HepMatrix& bigMatrix = matrix->bigMatrix();
	log << MSG::DEBUG << "   Big matrix : ( " << bigMatrix.num_row() << ", " << bigMatrix.num_col() << " ) " << endreq;

	log << MSG::VERBOSE << "   values: ";
	for (int i = 0; i < bigMatrix.num_row(); ++i) {
	    for (int j = 0; j < bigMatrix.num_col(); ++ j) {
		log << MSG::VERBOSE << bigMatrix[i][j] << ", ";
	    }
	}
	log << MSG::VERBOSE << endreq;
	nd += bigMatrix.num_row()*bigMatrix.num_col();

	log << MSG::DEBUG << "   Other matrices : " << endreq;
	for ( int i = 0; i < matrix->numberOfMatrices(); ++i ) {
	    const CLHEP::HepMatrix& matrx = matrix->matrix( i );
	    log << MSG::DEBUG << "                  ( " << matrx.num_row() << ", " << matrx.num_col() << " ) " << endreq;
	    log << MSG::VERBOSE << "   values: ";
	    for (int i = 0; i < matrx.num_row(); ++i) {
		for (int j = 0; j < matrx.num_col(); ++ j) {
		    log << MSG::VERBOSE << matrx[i][j] << ", ";
		}
	    }
	    log << MSG::VERBOSE << endreq;

	    nd += matrx.num_row()*matrx.num_col();
	    
	}

	nd += 3;
	
	log << MSG::DEBUG << "   N doubles : " << nd << endreq;


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


    // Fill and write out nested collections

    AthenaPoolTestMap* tmapPtr = new AthenaPoolTestMap();
    AthenaPoolTestMap& tmap    = *tmapPtr;

    typedef AthenaPoolTestMapData::Deposit deposit_t;
    typedef std::vector<AthenaPoolTestMapData::Deposit>         deposit_vec;

    deposit_vec deposits;
    
    deposit_t deposit;

    for (unsigned int j = 0; j < 10; ++j) {
	for (unsigned int i = 0; i < 10; ++i) {
	
	    deposit.first  = AthenaPoolTestMapDataLink(i, i + 10);
	    deposit.second = i + 20;
	    deposits.push_back(deposit);
	}

	tmap[j*5] = AthenaPoolTestMapData(deposits, j);
	deposits.clear();
    }
    
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

    sc = m_storeGate->record(tmapPtr, "AthenaPoolMap");
    if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register AthenaPoolMap data object"
 	    << endreq;
 	return StatusCode::FAILURE;
    }


    // Fill and write out dummy classes
    dummy_A* dummyA = new dummy_A();
    const std::vector<dummy_B>& b = dummyA->dummy();
    log << MSG::DEBUG << "Dummy A: size B " << b.size() << endreq;
    for (unsigned int i = 0; i < b.size(); ++i) {
	const std::vector<dummy_C>& c = b[i].dummy();
	log << MSG::DEBUG << "Dummy B: size C " << c.size() << endreq;
	for (unsigned int j = 0; j < c.size(); ++j) {
	    log << MSG::DEBUG << "i,c: " << j << " " << c[j].value() << endreq;
	}
    }
    sc = m_storeGate->record(dummyA, "Dummy_A");
    if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register Dummy_A"
 	    << endreq;
 	return StatusCode::FAILURE;
    }
    else {
	log << MSG::DEBUG << "Wrote dummy_A class " << endreq;
    }


    // Fill and write out dummy E classes
    dummy_E* dummyE = new dummy_E();
    dummyE->setData();
    const std::vector<const dummy_D*>& d = dummyE->dummy();
    log << MSG::DEBUG << "Dummy E: size D " << d.size() << endreq;
    for (unsigned int i = 0; i < d.size(); ++i) {
	log << MSG::DEBUG << "Dummy D: value " << d[i]->value() << endreq;
    }
    sc = m_storeGate->record(dummyE, "Dummy_E");
    if ( sc.isFailure() ) {
 	log << MSG::ERROR
 	    << "Could not register Dummy_E"
 	    << endreq;
 	return StatusCode::FAILURE;
    }
    else {
	log << MSG::DEBUG << "Wrote dummy_E class " << endreq;
    }


    // Write a PileupEventInfo

    const EventInfo * evt = 0;
    sc = m_storeGate->retrieve( evt, "McEventInfo" );
    if ( sc.isFailure() ) {
 	log << MSG::ERROR << "  Could not get event info" << endreq;      
 	return StatusCode::FAILURE;
    }
    else {
        // Add in weight
	EventType* eventType = const_cast<EventType*>(evt->event_type());
	eventType->set_mc_event_weight(0.9876, 0, 3);
	eventType->set_mc_event_weight(1.9876, 1);
	eventType->set_mc_event_weight(2.9876, 2);
 	log << MSG::DEBUG << "Event ID: ["
 	    << evt->event_ID()->run_number()   << ","
            << evt->event_ID()->event_number() << ":"
 	    << evt->event_ID()->time_stamp() << "] "
 	    << endreq;
 	log << MSG::DEBUG << "Event type: user type "
 	    << evt->event_type()->user_type() << " weight 0: "
 	    << eventType->mc_event_weight() << " weight 1: "
 	    << eventType->mc_event_weight(1) << " weight 2: "
 	    << eventType->mc_event_weight(2)
 	    << endreq;
        // Set lumi interactions
        EventInfo * evt1 = const_cast<EventInfo*>(evt);
        evt1->setActualInteractionsPerCrossing(35.238);
        evt1->setAverageInteractionsPerCrossing(169.415);
        log << MSG::DEBUG << "Setting actual interatctions per crossing: " 
            << evt1->actualInteractionsPerCrossing() << " and ave: " 
            << evt1->averageInteractionsPerCrossing() << endreq;
        // Add in subdet flags
        unsigned int flags = 0;
        // We only loop up to Lumi and not nDets since Lumi is set separately
        for (unsigned int i = 0; i < EventInfo::Lumi; ++i) {
            flags = 3*i +  0xFFFFFF00;
            if (!evt1->setEventFlags(EventInfo::EventFlagSubDet(i), flags)) {
                log << MSG::ERROR << "Error setting flag: i, flags " << i << " " << MSG::hex << flags << MSG::dec << endreq;
            }
            else {
                log << MSG::DEBUG << "Setting flag: i, flags " << i << " " << MSG::hex << flags << " " 
                    << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                    << MSG::dec << endreq;
            }
            if (!evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 6)) {
                log << MSG::ERROR << "Error setting bit 6 for subDet " << i << endreq;
            }
            else {
                log << MSG::DEBUG << "Setting bit 6: i, isSet " << i << " " 
                    << evt1->isEventFlagBitSet(EventInfo::EventFlagSubDet(i), 6) 
                    << MSG::hex << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                    << MSG::dec << endreq;
            }
            if (!evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 10, false)) {
                log << MSG::ERROR << "Error unsetting bit 10 for subDet " << i << endreq;
            }
            else {
                log << MSG::DEBUG << "Unsetting bit 10: i, isSet " << i << " " 
                    << evt1->isEventFlagBitSet(EventInfo::EventFlagSubDet(i), 10)
                    << MSG::hex << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                    << MSG::dec << endreq;
            }
            if (evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 29)) {
                log << MSG::ERROR << "Error trying to set bit 29 for subDet " << i << endreq;
            }
            else {
                log << MSG::DEBUG << "Returned false for trying to set bit 29 for subDet " << i << " " 
                    << endreq;
            }

            EventInfo::EventFlagErrorState state = EventInfo::NotSet;
            if (i%3 == 1)      state =  EventInfo::Warning;
            else if (i%3 == 2) state =  EventInfo::Error;
            if (!evt1->setErrorState(EventInfo::EventFlagSubDet(i), state)) {
                log << MSG::ERROR << "Error setting error state: i, state " << i << " " << state << endreq;
            }
            else {
                log << MSG::DEBUG << "Setting error state: i, state " << i << " " << state << endreq;
            }
            EventInfo::EventFlagErrorState error = evt1->errorState(EventInfo::EventFlagSubDet(i));
            if (error == EventInfo::NotSet)
                log << MSG::DEBUG << "Subdet: " << i << " error state: NotSet " << error << endreq;
            else if (error == EventInfo::Warning)
                log << MSG::DEBUG << "Subdet: " << i << " error state: Warning " << error <<  endreq;
            else if (error == EventInfo::Error)
                log << MSG::DEBUG << "Subdet: " << i << " error state: Error " << error << endreq;
        }
    }

    EventID* pOvrID = new EventID(evt->event_ID()->run_number(),
                                  evt->event_ID()->event_number(),
                                  12,  // time_stamp,            
                                  13,  // time_stamp_ns_offset,  
                                  14,  // lumi_block,            
                                  15,  // bunch_crossing_id,     
                                  16,  // detector_mask0
                                  17,  // detector_mask1
                                  18,  // detector_mask2
                                  19); // detector_mask3
    EventType* pOvrEt = new EventType(); //FIXME
    pOvrEt->set_user_type("Overlaid"); //FIXME


    // Add in trigger info to the existing event info
    std::vector<unsigned int>           level1TriggerInfo;
    level1TriggerInfo.push_back(14);
    level1TriggerInfo.push_back(15);
    std::vector<unsigned int>           level2TriggerInfo;
    level2TriggerInfo.push_back(10);
    level2TriggerInfo.push_back(11);
    std::vector<unsigned int>           eventFilterInfo;
    eventFilterInfo.push_back(12);
    eventFilterInfo.push_back(13);
    std::vector<TriggerInfo::StreamTag> streamTags;
    streamTags.push_back(TriggerInfo::StreamTag("StreamA", "TypeA", true));
    streamTags.push_back(TriggerInfo::StreamTag("StreamB", "TypeB", false));

    TriggerInfo* triggerInfo = new TriggerInfo(7, 8, 9, 
                                               level1TriggerInfo,
                                               level2TriggerInfo,
                                               eventFilterInfo,
                                               streamTags);

    PileUpEventInfo* pOverEvent = new PileUpEventInfo(pOvrID, pOvrEt, triggerInfo);
    //  register as sub event of the overlaid
    const EventInfo * newEvt = new EventInfo(*evt);
    pOverEvent->addSubEvt(25, PileUpTimeEventIndex::MinimumBias, newEvt, m_storeGate);
    if( !(m_storeGate->record(pOverEvent, "OverlayEvent")).isSuccess() )  {
 	log << MSG::ERROR
 	    << "Error recording overlayed event object" << endreq;
 	return StatusCode::FAILURE;
    } 

//     *********** CANNOT DO SYMLINK IF THERE ARE TWO EVENTINFO OBJECTS WRITTEN *******
//     *********** APP MGR CANNOT READ EVENTINFO WHEN THERE ARE TWO *******
//     // Register symlink of PileupEventInfo to just EventInfo
//     sc = m_storeGate->symLink(pOverEvent, evt);
//     if (sc != StatusCode::SUCCESS) {
// 	log << MSG::ERROR 
// 	    << "Error making symLink from PileUpEventInfo to EventInfo "
// 	    << endreq;
// 	return sc;
//     } 
//     else {
// 	log << MSG::DEBUG << "Registered symlink from PileUpEventInfo to EventInfo " << endreq;
//     }


    // Printout
    PileUpEventInfo* pevt = pOverEvent;
    
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
    PileUpEventInfo::SubEvent::const_iterator it1  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end1 = pevt->endSubEvt();
    if (it1 == end1) {
	log << MSG::DEBUG << "None found" << endreq;
    }
    for (; it1 != end1; ++it1) {
	const EventInfo* sevt = (*it1).pSubEvt;
	log << MSG::DEBUG << "Time, index " 
	    << (*it1).time() << " " << (*it1).index()
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
                << it1->time() << " " << it1->index()
                << endreq;
	}
	else {
	    log << MSG::DEBUG << "Subevent is null ptr "
		<< endreq;
	}
    }

    
    
    return StatusCode::SUCCESS;
   
}

StatusCode AthenaPoolTestDataWriter::finalize() 
{
  return StatusCode::SUCCESS;
}



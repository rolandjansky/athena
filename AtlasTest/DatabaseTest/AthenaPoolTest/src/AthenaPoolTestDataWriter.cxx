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

#include <sstream>

AthenaPoolTestDataWriter::AthenaPoolTestDataWriter(const std::string& name,
                                                   ISvcLocator* pSvcLocator):
    AthAlgorithm(name, pSvcLocator),
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
    return StatusCode::SUCCESS; 
}

StatusCode AthenaPoolTestDataWriter::execute()
{ 
    ATH_MSG_DEBUG("Executing AthenaPoolTestDataWriter" );
   

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
    
    ATH_MSG_DEBUG("Collection of A's" );

    IAthenaPoolTestCollection::const_iterator it   = col1->begin();
    IAthenaPoolTestCollection::const_iterator last = col1->end();
    for (; it != last; ++it) {
        ATH_MSG_DEBUG((*it)->printWhoYouAre() );
    }
                                    
    ATH_MSG_DEBUG("Collection of B's" );
    it   = col2->begin();
    last = col2->end();
    for (; it != last; ++it) {  
        ATH_MSG_DEBUG((*it)->printWhoYouAre() );
    }
                                    
    ATH_MSG_DEBUG("Collection of A's and B's" );
    it   = col3->begin();
    last = col3->end();
    for (; it != last; ++it) {
        ATH_MSG_DEBUG((*it)->printWhoYouAre() );
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
    
        ATH_MSG_DEBUG("Collection of C's" );
        it   = col4->begin();
        last = col4->end();
        for (; it != last; ++it) {
            ATH_MSG_DEBUG((*it)->printWhoYouAre() );
        }
                                    
        ATH_MSG_DEBUG("Collection of D's" );
        it   = col5->begin();
        last = col5->end();
        for (; it != last; ++it) {
            ATH_MSG_DEBUG((*it)->printWhoYouAre() );
        }
    }
    
 

    if (m_partialCreate && m_readOtherHalf) {
	const IAthenaPoolTestCollection* col = 0;
	ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_4") );

        ATH_MSG_DEBUG("Collection of C's" );
        it   = col->begin();
        last = col->end();
        for (; it != last; ++it) {
            ATH_MSG_DEBUG((*it)->printWhoYouAre() );
        }
                                    
	ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_5") );
        
        ATH_MSG_DEBUG("Collection of D's" );
        it   = col->begin();
        last = col->end();
        for (; it != last; ++it) {
            ATH_MSG_DEBUG((*it)->printWhoYouAre() );
        }
    }

    if (m_partialCreate && m_readFirstHalf) {
	// Try to first read the collections that we want to write
	// again => this should give and error!!
	const IAthenaPoolTestCollection* col = 0;
	ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_1") );
	ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_2") );
	ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_3") );
    }


    // Register collections in the Gaudi Transient Store:
 
     ATH_CHECK( evtStore()->record(col1, "AthenaPoolTestCollection_1") );
     ATH_CHECK( evtStore()->record(col2, "AthenaPoolTestCollection_2") );
     ATH_CHECK( evtStore()->record(col3, "AthenaPoolTestCollection_3") );
     if (!m_partialCreate) {
        ATH_CHECK( evtStore()->record(col4, "AthenaPoolTestCollection_4") );
 	ATH_CHECK( evtStore()->record(col5, "AthenaPoolTestCollection_5") );
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

        // set transform
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
	ATH_CHECK( evtStore()->record(matrix, key) );
        ATH_MSG_DEBUG("Registering matrix " << i + 1 << " " << key );

        const HepGeom::Point3D<double>& point  = matrix->point();
        const HepGeom::Point3D<double>& point1 = matrix->point1();
        ATH_MSG_DEBUG("   Points (by value,by ptr) x,y,z : " 
                      << point.x() << " " 
                      << point.y() << " " 
                      << point.z() << " " 
                      << ", x,y,z " 
                      << point1.x() << " " 
                      << point1.y() << " " 
                      << point1.z() << " ");
        const CLHEP::HepMatrix& smallMatrix = matrix->smallMatrix();
        ATH_MSG_DEBUG("   Small matrix : ( " << smallMatrix.num_row() << ", " << smallMatrix.num_col() << " ) " );

        stream.clear();
        for (int i = 0; i < smallMatrix.num_row(); ++i) {
            for (int j = 0; j < smallMatrix.num_col(); ++ j) {
                stream << smallMatrix[i][j] << ", ";
            }
        }
        ATH_MSG_VERBOSE("   values: " << stream.str());
        nd += smallMatrix.num_row()*smallMatrix.num_col();
        
        const CLHEP::HepMatrix& bigMatrix = matrix->bigMatrix();
        ATH_MSG_DEBUG("   Big matrix : ( " << bigMatrix.num_row() << ", " << bigMatrix.num_col() << " ) " );

        stream.clear();
        for (int i = 0; i < bigMatrix.num_row(); ++i) {
            for (int j = 0; j < bigMatrix.num_col(); ++ j) {
                stream << bigMatrix[i][j] << ", ";
            }
        }
        ATH_MSG_VERBOSE("   values: " << stream.str());
        nd += bigMatrix.num_row()*bigMatrix.num_col();

        ATH_MSG_DEBUG("   Other matrices : " );
        for ( int i = 0; i < matrix->numberOfMatrices(); ++i ) {
            const CLHEP::HepMatrix& matrx = matrix->matrix( i );
            ATH_MSG_DEBUG("                  ( " << matrx.num_row() << ", " << matrx.num_col() << " ) " );
            stream.clear();
            for (int i = 0; i < matrx.num_row(); ++i) {
                for (int j = 0; j < matrx.num_col(); ++ j) {
                    stream << matrx[i][j] << ", ";
                }
            }
            ATH_MSG_VERBOSE("   values: " << stream.str());

            nd += matrx.num_row()*matrx.num_col();
            
        }

        nd += 3;
        
        ATH_MSG_DEBUG("   N doubles : " << nd );


        // print out matrix maps
        AthenaPoolTestMatrix::matrixMaps_t mats = matrix->matrixMaps();

        ATH_MSG_DEBUG("   Matrix maps : " << mats.size() );
        std::map<unsigned int, CLHEP::HepMatrix>& mp0 = mats[0];
        std::map<unsigned int, CLHEP::HepMatrix>& mp1 = mats[1];
        std::map<unsigned int, CLHEP::HepMatrix>::const_iterator itmp =  mp0.begin();
        ATH_MSG_DEBUG("   Key, row, column " << (*itmp).first << " " 
                      << (*itmp).second.num_row() << ", " << (*itmp).second.num_col() );
        ++itmp;
        ATH_MSG_DEBUG("   Key, row, column " << (*itmp).first << " " 
                      << (*itmp).second.num_row() << ", " << (*itmp).second.num_col() );
        itmp =  mp1.begin();
        ATH_MSG_DEBUG("   Key, row, column " << (*itmp).first << " " 
                      << (*itmp).second.num_row() << ", " << (*itmp).second.num_col() );
        ++itmp;
        ATH_MSG_DEBUG("   Key, row, column " << (*itmp).first << " " 
                      << (*itmp).second.num_row() << ", " << (*itmp).second.num_col() );

        // Print out the transform:
        ATH_MSG_DEBUG("   Matrix transform: " );
        const HepGeom::Transform3D& trans  = matrix->trans();
        ATH_MSG_DEBUG("   Matrix transform: " );
        ATH_MSG_DEBUG("  xx " << trans(0,0) );
        ATH_MSG_DEBUG("  xy " << trans(0,1)  );
        ATH_MSG_DEBUG("  xz " << trans(0,2)  );
        ATH_MSG_DEBUG("  yx " << trans(1,0)  );
        ATH_MSG_DEBUG("  yy " << trans(1,1)  );
        ATH_MSG_DEBUG("  yz " << trans(1,2)  );
        ATH_MSG_DEBUG("  zx " << trans(2,0)  );
        ATH_MSG_DEBUG("  zy " << trans(2,1)  );
        ATH_MSG_DEBUG("  zz " << trans(2,2)  );
        ATH_MSG_DEBUG("  dx " << trans(3,0)  );
        ATH_MSG_DEBUG("  dy " << trans(3,1)  );
        ATH_MSG_DEBUG("  dz " << trans(3,2)  );

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

        deposits.clear();
        (*itm).second.deposits(deposits);
        std::stringstream stream;
        for (unsigned int i = 0; i < deposits.size(); ++i) {
            AthenaPoolTestMapDataLink& link = deposits[i].first;
            stream << link.barcode() << " " 
                   << link.eventIndex() << " " 
                   << deposits[i].second << " ";
        }
        ATH_MSG_DEBUG("AthenaPoolTestMap: key " << (*itm).first 
                      << "  word " << (*itm).second.word()
                      << "  Deposits: (barCode, evtIndx, wgt) " << stream.str());
    }

    ATH_CHECK( evtStore()->record(tmapPtr, "AthenaPoolMap") );


    // Fill and write out dummy classes
    dummy_A* dummyA = new dummy_A();
    const std::vector<dummy_B>& b = dummyA->dummy();
    ATH_MSG_DEBUG("Dummy A: size B " << b.size() );
    for (unsigned int i = 0; i < b.size(); ++i) {
        const std::vector<dummy_C>& c = b[i].dummy();
        ATH_MSG_DEBUG("Dummy B: size C " << c.size() );
        for (unsigned int j = 0; j < c.size(); ++j) {
            ATH_MSG_DEBUG("i,c: " << j << " " << c[j].value() );
        }
    }
    ATH_CHECK( evtStore()->record(dummyA, "Dummy_A") );


    // Fill and write out dummy E classes
    dummy_E* dummyE = new dummy_E();
    dummyE->setData();
    const std::vector<const dummy_D*>& d = dummyE->dummy();
    ATH_MSG_DEBUG("Dummy E: size D " << d.size() );
    for (unsigned int i = 0; i < d.size(); ++i) {
        ATH_MSG_DEBUG("Dummy D: value " << d[i]->value() );
    }
    ATH_CHECK( evtStore()->record(dummyE, "Dummy_E") );


    // Write a PileupEventInfo

    const EventInfo * evt = 0;
    ATH_CHECK( evtStore()->retrieve( evt, "McEventInfo" ) );
    // Add in weight
    EventType* eventType = const_cast<EventType*>(evt->event_type());
    eventType->set_mc_event_weight(0.9876, 0, 3);
    eventType->set_mc_event_weight(1.9876, 1);
    eventType->set_mc_event_weight(2.9876, 2);
    ATH_MSG_DEBUG( "Event ID: ["
                   << evt->event_ID()->run_number()   << ","
                   << evt->event_ID()->event_number() << ":"
                   << evt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( "Event type: user type "
                   << evt->event_type()->user_type() << " weight 0: "
                   << eventType->mc_event_weight() << " weight 1: "
                   << eventType->mc_event_weight(1) << " weight 2: "
                   << eventType->mc_event_weight(2) );
    // Set lumi interactions
    EventInfo * evt1 = const_cast<EventInfo*>(evt);
    evt1->setActualInteractionsPerCrossing(35.238);
    evt1->setAverageInteractionsPerCrossing(169.415);
    ATH_MSG_DEBUG( "Setting actual interatctions per crossing: " 
                   << evt1->actualInteractionsPerCrossing() << " and ave: " 
                   << evt1->averageInteractionsPerCrossing()  );
    // Add in subdet flags
    unsigned int flags = 0;
    // We only loop up to Lumi and not nDets since Lumi is set separately
    for (unsigned int i = 0; i < EventInfo::Lumi; ++i) {
      flags = 3*i +  0xFFFFFF00;
      if (!evt1->setEventFlags(EventInfo::EventFlagSubDet(i), flags)) {
        ATH_MSG_ERROR( "Error setting flag: i, flags " << i << " " << MSG::hex << flags << MSG::dec  );
      }
      else {
        ATH_MSG_DEBUG( "Setting flag: i, flags " << i << " " << MSG::hex << flags << " " 
                       << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                       << MSG::dec  );
      }
      if (!evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 6)) {
        ATH_MSG_ERROR( "Error setting bit 6 for subDet " << i  );
      }
      else {
        ATH_MSG_DEBUG( "Setting bit 6: i, isSet " << i << " " 
                       << evt1->isEventFlagBitSet(EventInfo::EventFlagSubDet(i), 6) 
                       << MSG::hex << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                       << MSG::dec  );
      }
      if (!evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 10, false)) {
        ATH_MSG_ERROR( "Error unsetting bit 10 for subDet " << i  );
      }
      else {
        ATH_MSG_DEBUG( "Unsetting bit 10: i, isSet " << i << " " 
                       << evt1->isEventFlagBitSet(EventInfo::EventFlagSubDet(i), 10)
                       << MSG::hex << " flags " << evt1->eventFlags(EventInfo::EventFlagSubDet(i)) 
                       << MSG::dec  );
      }
      if (evt1->setEventFlagBit(EventInfo::EventFlagSubDet(i), 29)) {
        ATH_MSG_ERROR( "Error trying to set bit 29 for subDet " << i  );
      }
      else {
        ATH_MSG_DEBUG( "Returned false for trying to set bit 29 for subDet " << i << " " 
                       );
      }

      EventInfo::EventFlagErrorState state = EventInfo::NotSet;
      if (i%3 == 1)      state =  EventInfo::Warning;
      else if (i%3 == 2) state =  EventInfo::Error;
      if (!evt1->setErrorState(EventInfo::EventFlagSubDet(i), state)) {
        ATH_MSG_ERROR( "Error setting error state: i, state " << i << " " << state  );
      }
      else {
        ATH_MSG_DEBUG( "Setting error state: i, state " << i << " " << state  );
      }
      EventInfo::EventFlagErrorState error = evt1->errorState(EventInfo::EventFlagSubDet(i));
      if (error == EventInfo::NotSet)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: NotSet " << error  );
      else if (error == EventInfo::Warning)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: Warning " << error  );
      else if (error == EventInfo::Error)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: Error " << error  );
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
    auto newEvt = std::make_unique<EventInfo>(*evt);

    // get StoreGate service
    // StoreGateSvc* storeGate;
    // if (service("StoreGateSvc", storeGate).isFailure()) {
    //     ATH_MSG_ERROR("StoreGate service not found !");
    //     return StatusCode::FAILURE;
    // } 
    // pOverEvent->addSubEvt(25, PileUpTimeEventIndex::MinimumBias, newEvt, storeGate);
    pOverEvent->addSubEvt(25, PileUpTimeEventIndex::MinimumBias, std::move(newEvt), 0);
    ATH_CHECK( evtStore()->record(pOverEvent, "OverlayEvent") );

    // Printout
    PileUpEventInfo* pevt = pOverEvent;
    
    ATH_MSG_DEBUG("PileUpEventInfo");
    ATH_MSG_DEBUG("Event ID: ["
                  << pevt->event_ID()->run_number()   << ","
                  << pevt->event_ID()->event_number() << ":"
                  << pevt->event_ID()->time_stamp() << "] ");
    ATH_MSG_DEBUG(*(pevt->event_ID()));
    ATH_MSG_DEBUG("Event type: user type "
                  << pevt->event_type()->user_type());
                      
    std::stringstream stream;
    stream << "Trigger info: status " << pevt->trigger_info()->statusElement()
           << " extLvl1ID " << pevt->trigger_info()->extendedLevel1ID()
           << " lvl1Type  " << pevt->trigger_info()->level1TriggerType()
           << " lvl1Info  ";
    for (unsigned int i = 0; i < pevt->trigger_info()->level1TriggerInfo().size(); ++i) {
        stream << pevt->trigger_info()->level1TriggerInfo()[i] << " ";
    }
    stream << "lvl2Info ";
    for (unsigned int i = 0; i < pevt->trigger_info()->level2TriggerInfo().size(); ++i) {
        stream << pevt->trigger_info()->level2TriggerInfo()[i] << " ";
    }
    stream << "EventFilterInfo ";
    for (unsigned int i = 0; i < pevt->trigger_info()->eventFilterInfo().size(); ++i) {
        stream << pevt->trigger_info()->eventFilterInfo()[i] << " ";
    }
    stream << "EventFilterInfo ";
    for (unsigned int i = 0; i < pevt->trigger_info()->streamTags().size(); ++i) {
        stream << " i " << i << " name " << pevt->trigger_info()->streamTags()[i].name()
               << " type " << pevt->trigger_info()->streamTags()[i].type()
               << " ObeyLumi " << pevt->trigger_info()->streamTags()[i].obeysLumiblock();
    }
    ATH_MSG_DEBUG(stream.str());

    // Get normal event info as a sub-event info
    ATH_MSG_DEBUG("SubEventInfos");
    PileUpEventInfo::SubEvent::const_iterator it1  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end1 = pevt->endSubEvt();
    if (it1 == end1) {
        ATH_MSG_DEBUG("None found" );
    }
    for (; it1 != end1; ++it1) {
        const EventInfo* sevt = (*it1).pSubEvt;
        ATH_MSG_DEBUG("Time, index " 
                      << (*it1).time() << " " << (*it1).index()
                      );
        if (sevt) {
            ATH_MSG_DEBUG("Event ID: ["
                          << sevt->event_ID()->run_number()   << ","
                          << sevt->event_ID()->event_number() << ":"
                          << sevt->event_ID()->time_stamp() << "] ");
            ATH_MSG_DEBUG("Event type: user type "
                          << sevt->event_type()->user_type());
            ATH_MSG_DEBUG("Pileup time, index type "
                          << it1->time() << " " << it1->index());
        }
        else {
            ATH_MSG_DEBUG("Subevent is null ptr ");
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode AthenaPoolTestDataWriter::finalize() 
{
    return StatusCode::SUCCESS;
}



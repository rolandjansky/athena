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
#include "AthenaPoolTestData/AthenaPoolTestMap.h"
#include "AthenaPoolTestData/dummy_A.h"
#include "AthenaPoolTestData/dummy_E.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include <algorithm>
#include <sstream>

AthenaPoolTestDataReader::AthenaPoolTestDataReader(const std::string& name,
                                                   ISvcLocator* pSvcLocator):
    AthAlgorithm(name, pSvcLocator)
{}

AthenaPoolTestDataReader::~AthenaPoolTestDataReader()
{}

StatusCode AthenaPoolTestDataReader::initialize()
{
    return StatusCode::SUCCESS; 
}

StatusCode AthenaPoolTestDataReader::execute()
{ 

    ATH_MSG_DEBUG("Executing AthenaPoolTestDataReader" );
   

    // Retrieve collections

    // A's only
 
    std::string test1;
    std::string test2;
    bool error = false;
    
    const IAthenaPoolTestCollection* col;
    ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_1") );
    
        
    ATH_MSG_DEBUG("Collection of A's" );
    int i = 0; 
    for (const IAthenaPoolTestData* elt : *col) {
      ATH_MSG_DEBUG( "Read:      " << elt->printWhoYouAre()  );
      AthenaPoolTestA data(i+1, i+2);
      ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
      test1 = elt->printWhoYouAre();
      test2 = data.printWhoYouAre();
      if (test1 != test2) {
        ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
        error = true;
      }
      ++i;
    }
                                    

    // B's only
 
    ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_2") );
    
    ATH_MSG_DEBUG("Collection of B's" );
    i = 0; 
    for (const IAthenaPoolTestData* elt : *col) {
        ATH_MSG_DEBUG( "Read:      " << elt->printWhoYouAre()  );
	AthenaPoolTestB data(i+11, i+12);
	ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
	test1 = elt->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
            ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
	    error = true;
	}
        ++i;
    }
                                    

    // A's and B's 
 
    ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_3") );

    ATH_MSG_DEBUG("Collection of A's and B's" );
    i = 0; 
    for (const IAthenaPoolTestData* elt : *col) {
        ATH_MSG_DEBUG( "Read:      " << elt->printWhoYouAre()  );
	if (i < 3) {
	    AthenaPoolTestA data(i+21, i+22);
	    ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
	    test1 = elt->printWhoYouAre();
	    test2 = data.printWhoYouAre();
	    if (test1 != test2) {
                ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
		error = true;
	    }
	}
	else {
	    AthenaPoolTestB data(i+21, i+22);
	    ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
	    test1 = elt->printWhoYouAre();
	    test2 = data.printWhoYouAre();
	    if (test1 != test2) {
                ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
		error = true;
	    }
	}
        ++i;
    }


    // C's only
 
    ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_4") );

    ATH_MSG_DEBUG("Collection of C's" );
    i = 0; 
    for (const IAthenaPoolTestData* elt : *col) {
        ATH_MSG_DEBUG( "Read:      " << elt->printWhoYouAre()  );
	AthenaPoolTestC data(0, 0, i+3, i+4);
	ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
	test1 = elt->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
            ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
	    error = true;
	}
        ++i;
    }
                                    

    // D's only
 
    ATH_CHECK( evtStore()->retrieve(col, "AthenaPoolTestCollection_5") );

    ATH_MSG_DEBUG("Collection of D's" );
    i = 0; 
    for (const IAthenaPoolTestData* elt : *col) {
        ATH_MSG_DEBUG( "Read:      " << elt->printWhoYouAre()  );
	AthenaPoolTestD data(0, 0, i+13, i+14, i+15, i+16);
	ATH_MSG_DEBUG( "Should be: " << data.printWhoYouAre()  );
	test1 = elt->printWhoYouAre();
	test2 = data.printWhoYouAre();
	if (test1 != test2) {
            ATH_MSG_ERROR( "Incorrect data:  in/out " << test1 << " " << test2  );
	    error = true;
	}
	++i;
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
	    ATH_CHECK( evtStore()->retrieve(matrix, key) );
            ATH_MSG_DEBUG("Retrieved matrix " << i + 1 << " " << key );

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
            stream << "   values: ";
            for (int i = 0; i < smallMatrix.num_row(); ++i) {
                for (int j = 0; j < smallMatrix.num_col(); ++ j) {
                    stream << smallMatrix[i][j] << ", ";
                }
            }
            ATH_MSG_VERBOSE(stream.str());
            
            const CLHEP::HepMatrix& bigMatrix = matrix->bigMatrix();
            ATH_MSG_DEBUG("   Big matrix : ( " << bigMatrix.num_row() << ", " << bigMatrix.num_col() << " ) " );
            stream.clear();
            stream << "   values: ";
            for (int i = 0; i < bigMatrix.num_row(); ++i) {
                for (int j = 0; j < bigMatrix.num_col(); ++ j) {
                    stream << bigMatrix[i][j] << ", ";
                }
            }
            ATH_MSG_VERBOSE(stream.str());
            ATH_MSG_DEBUG("   Other matrices : " );
            for ( int ii = 0; ii < matrix->numberOfMatrices(); ++ii ) {
                const CLHEP::HepMatrix& matrx = matrix->matrix( ii );
                ATH_MSG_DEBUG("                  ( " << matrx.num_row() << ", " << matrx.num_col() << " ) " );
                stream.clear();
                stream << "   values: ";
                for (int i = 0; i < matrx.num_row(); ++i) {
                    for (int j = 0; j < matrx.num_col(); ++ j) {
                        stream << matrx[i][j] << ", ";
                    }
                }
                ATH_MSG_VERBOSE(stream.str());
            }

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
    }
    


    // Read in nested collections

    bool readMap = true;
    const AthenaPoolTestMap* tmapPtr;
    if (evtStore()->retrieve(tmapPtr, "AthenaPoolMap").isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve AthenaPoolMap object ");
        readMap = false;
    }
    else {
        ATH_MSG_DEBUG("Retrieved AthenaPoolMap" );
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

            std::stringstream stream;
            stream << "AthenaPoolTestMap: key " << (*itm).first 
                   << "  word " << (*itm).second.word()
                   << "  Deposits: (barCode, evtIndx, wgt) ";

            deposits.clear();
            (*itm).second.deposits(deposits);
            for (unsigned int i = 0; i < deposits.size(); ++i) {
                AthenaPoolTestMapDataLink& link = deposits[i].first;
                stream << link.barcode() << " " 
                       << link.eventIndex() << " " 
                       << deposits[i].second << " ";
            }
            ATH_MSG_DEBUG(stream.str());
        }
    }
    

    // Read back dummy classes
    
    const dummy_A* dummyA;
    if (evtStore()->retrieve(dummyA, "Dummy_A").isFailure() ) {
        ATH_MSG_ERROR("Could not retrieve Dummy_A");
    }
    else {
        ATH_MSG_DEBUG("Read back dummy_A class " );
    }
    const std::vector<dummy_B>& b = dummyA->dummy();
    ATH_MSG_DEBUG("Dummy A: size B " << b.size() );
    for (unsigned int i = 0; i < b.size(); ++i) {
        const std::vector<dummy_C>& c = b[i].dummy();
        ATH_MSG_DEBUG("Dummy B: size C " << c.size() );
        for (unsigned int j = 0; j < c.size(); ++j) {
            ATH_MSG_DEBUG("i,c: " << j << " " << c[j].value() );
        }
    }
    
    // dummy_E, D

    const dummy_E* dummyE;
    ATH_CHECK( evtStore()->retrieve(dummyE, "Dummy_E") );
    const std::vector<const dummy_D*>& d = dummyE->dummy();
    ATH_MSG_DEBUG("Dummy E: size D " << d.size() );
    for (unsigned int i = 0; i < d.size(); ++i) {
        ATH_MSG_DEBUG("Dummy D: value " << d[i]->value() );
    }


    // Read PileupEventInfo

    // Normal EventInfo
    const EventInfo * evt = 0;
    ATH_CHECK( evtStore()->retrieve( evt, "McEventInfo" ) );
    ATH_MSG_DEBUG( "Normal EventInfo" );
    ATH_MSG_DEBUG( "Event ID: ["
                   << evt->event_ID()->event_number() << ":"
                   << evt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( *(evt->event_ID()) );
    {
      std::stringstream stream;
      stream << "Event type: user type "
             << evt->event_type()->user_type();
      for (unsigned int i = 0; i < evt->event_type()->n_mc_event_weights (); ++i)
        stream<< " weight " << i << ": " << evt->event_type()->mc_event_weight(i);
      ATH_MSG_DEBUG(stream.str());
    }

    // Check interactions per crossing
    ATH_MSG_DEBUG( "Actual interatctions per crossing: " 
                   << evt->actualInteractionsPerCrossing() << " and ave: " 
                   << evt->averageInteractionsPerCrossing()  );

    // We only loop up to Lumi and not nDets since Lumi is set separately
    for (unsigned int i = 0; i < EventInfo::Lumi; ++i) {
      ATH_MSG_DEBUG( "Subdet: " << i << " flags        " 
                     << MSG::hex << evt->eventFlags(EventInfo::EventFlagSubDet(i)) << MSG::dec  );
      EventInfo::EventFlagErrorState error = evt->errorState(EventInfo::EventFlagSubDet(i));
      if (error == EventInfo::NotSet)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: NotSet "  );
      else if (error == EventInfo::Warning)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: Warning "  );
      else if (error == EventInfo::Error)
        ATH_MSG_DEBUG( "Subdet: " << i << " error state: Error "  );
    }
    

    // PileupEventInfo as itself
    const PileUpEventInfo* pevt = 0;
    ATH_CHECK( evtStore()->retrieve( pevt, "OverlayEvent" ) );
    ATH_MSG_DEBUG( "PileUpEventInfo" );
    ATH_MSG_DEBUG( "Event ID: ["
                   << pevt->event_ID()->run_number()   << ","
                   << pevt->event_ID()->event_number() << ":"
                   << pevt->event_ID()->time_stamp() << "] " );
    ATH_MSG_DEBUG( *(pevt->event_ID()) );
    ATH_MSG_DEBUG( "Event type: user type "
                   << pevt->event_type()->user_type() );

    {
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
        stream << " i " << i << " name " << pevt->trigger_info()->streamTags()[i].name();
        stream << " type " << pevt->trigger_info()->streamTags()[i].type();
        stream << " ObeyLumi " << pevt->trigger_info()->streamTags()[i].obeysLumiblock();
      }
      ATH_MSG_DEBUG(stream.str());
    }

    // Get normal event info as a sub-event info
    ATH_MSG_DEBUG( "SubEventInfos" );
    PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
    if (it == end) {
      ATH_MSG_DEBUG( "None found"  );
    }
    for (; it != end; ++it) {
      const EventInfo* sevt = (*it).pSubEvt;
      ATH_MSG_DEBUG( "Time, index " 
                     << (*it).time() << " " << (*it).index() );
      if (sevt) {
        ATH_MSG_DEBUG( "Event ID: ["
                       << sevt->event_ID()->run_number()   << ","
                       << sevt->event_ID()->event_number() << ":"
                       << sevt->event_ID()->time_stamp() << "] " );
        ATH_MSG_DEBUG( "Event type: user type "
                       << sevt->event_type()->user_type() );
        ATH_MSG_DEBUG( "Pileup time, index type "
                       << it->time() << " " << it->index() );
      }
      else {
        ATH_MSG_DEBUG( "Subevent is null ptr " );
      }
    }

    if (error) return (StatusCode::FAILURE);
                                    
    return StatusCode::SUCCESS;
}

StatusCode AthenaPoolTestDataReader::finalize() 
{
    return StatusCode::SUCCESS;
}



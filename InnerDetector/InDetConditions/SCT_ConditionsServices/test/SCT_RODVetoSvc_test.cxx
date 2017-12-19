/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoSvc_test.cxx
 * Unit tests to test the methods of 
 * SCT_RodVetoSvc
 *
 * @author fschenck@cern.ch
 *
 * */

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"
//#include "gmock/gmock.h"


// ATLAS C++
#include "CxxUtils/make_unique.h"

// Tested Algorithms
#include "../src/SCT_RODVetoSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"


#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"

//Gaudi includes
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

//local includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"
#include "SCT_Cabling/SCT_CablingSvc.h"
#include "SCT_Cabling/ISCT_FillCabling.h"
#include "src/SCT_FillCablingFromText.h"
//#include "mock_CablingSvc.h"


#include "InDetIdentifier/SCT_ID.h"


// global test environment takes care of setting up Gaudi
class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
        ISvcLocator* g_svcLoc =  Gaudi::svcLocator();

	Athena_test::initGaudi(g_svcLoc);

        // Needed for the setup of StorgateSvc
        const SCT_ID * pHelper = new SCT_ID();

        // Easy way to set up services
        const ServiceLocatorHelper helper(*g_svcLoc, "HELPER");

        // Set up a new StoreGateSvc instance named DetectorStore, as SCT_RODVetoSvc needs
        IService* i_svc = helper.service("StoreGateSvc/DetectorStore", true /*queit*/ , true /*createIf*/);
        StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*> (i_svc);
        if (detStore) {
            StatusCode sc = detStore->record(pHelper, "SCT_ID");
        } else {
            delete pHelper;
            pHelper = nullptr;
        }
    }
};



class SCT_RODVetoSvc_test: public ::testing::Test{

  protected:


    virtual void SetUp() override {
        ISvcLocator* g_svcLoc =  Gaudi::svcLocator();
    	m_svc = new SCT_RODVetoSvc("SCT_RODVetoSvc", g_svcLoc );
        
    }


    virtual void TearDown() override {
    	delete m_svc;
    }   



    SCT_RODVetoSvc* m_svc = nullptr;
    };




TEST_F(SCT_RODVetoSvc_test, Initialization) {
  ASSERT_TRUE( m_svc->initialize().isSuccess() );
}

TEST_F(SCT_RODVetoSvc_test, Finalization) {
  ASSERT_TRUE( m_svc->finalize().isSuccess() );
  }


TEST_F(SCT_RODVetoSvc_test, queryInterface) {

  //It wants a pointer to a pointer, I give it a pointer to a pointer
  void* b = nullptr;
  void** ppvInterface = &b;
  const InterfaceID rrid("ISCT_ConditionsSvc", 1, 0);
  ASSERT_TRUE( m_svc->queryInterface(rrid  ,ppvInterface ).isSuccess() );
  }


 
TEST_F(SCT_RODVetoSvc_test, canReportAbout) {
  ASSERT_TRUE(  m_svc->canReportAbout(InDetConditions::DEFAULT)  );
}


/*
 //This segfaults  the tests
 //
 //This test should probably be done as well,
 //but to do it you have to create the cabling
 //service and properly initialize it with
 //rods etc.
TEST_F(SCT_RODVetoSvc_test, isGood) {
  //m_svc->initialize();
  //m_svc->fillData();
  //IdentifierHash * hashId = new IdentifierHash(0x240100);
  //IdentifierHash anyRandomHash(1000); 
  ASSERT_FALSE(  m_svc->isGood(0x240100)  );
  //ASSERT_FALSE(  m_svc->isGood(0)  );
}
*/


TEST_F(SCT_RODVetoSvc_test, isGood) {
  m_svc->initialize(); 
  const Identifier elementId (0);


  ASSERT_TRUE(  m_svc->isGood( elementId , InDetConditions::DEFAULT)  );
}
     


TEST_F(SCT_RODVetoSvc_test, fillData ) {
  m_svc->initialize();
  ASSERT_FALSE(  m_svc->fillData().isSuccess()  );
}


TEST_F(SCT_RODVetoSvc_test, canFillDuringInitialization) {
  ASSERT_FALSE(  m_svc->canFillDuringInitialize()  );
}


TEST_F(SCT_RODVetoSvc_test, isFilled ) {
 m_svc->initialize();
 
  auto bad_elems = CxxUtils::make_unique< std::vector<unsigned int> > ();
  SG::WriteHandle< std::vector<unsigned int> > wh_badRODElements{"BadRODIdentifiers"};
  wh_badRODElements.initialize();
  wh_badRODElements.record( std::move(bad_elems) );
  wh_badRODElements->push_back(0x240100 ); 
  std::cout << wh_badRODElements.key() << std::endl;
  std::cout << wh_badRODElements->at(0) << std::endl;


  m_svc->fillData();

  ASSERT_TRUE(  m_svc->filled()   );
}




int main(int argc, char *argv[]){
      
     ::testing::InitGoogleTest( &argc, argv );
     //::testing::InitGoogleMock( &argc, argv );
     ::testing::AddGlobalTestEnvironment( new GaudiEnvironment );
     return RUN_ALL_TESTS();
}

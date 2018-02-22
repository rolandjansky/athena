/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTool_test.cxx
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
#include "../src/SCT_RODVetoTool.h"

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
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"
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

        // Set up a new StoreGateSvc instance named DetectorStore, as SCT_RODVetoTool needs
        IService* i_svc = helper.service("StoreGateSvc/DetectorStore", true /*quiet*/ , true /*createIf*/);
        StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*> (i_svc);
        if (detStore) {
            StatusCode sc = detStore->record(pHelper, "SCT_ID");
        } else {
            delete pHelper;
            pHelper = nullptr;
        }
    }
};



class SCT_RODVetoTool_test: public ::testing::Test{

  protected:


    virtual void SetUp() override {
        ISCT_ConditionsTool* parent{nullptr};
        m_tool = new SCT_RODVetoTool("SCT_RODVetoTool", "SCT_RODVetoTool", parent);
        
    }


    virtual void TearDown() override {
    	delete m_tool;
    }   



    SCT_RODVetoTool* m_tool = nullptr;
    };




TEST_F(SCT_RODVetoTool_test, Initialization) {
  ASSERT_TRUE( m_tool->initialize().isSuccess() );
}

TEST_F(SCT_RODVetoTool_test, Finalization) {
  ASSERT_TRUE( m_tool->finalize().isSuccess() );
  }


TEST_F(SCT_RODVetoTool_test, queryInterface) {

  //It wants a pointer to a pointer, I give it a pointer to a pointer
  void* b = nullptr;
  void** ppvInterface = &b;
  const InterfaceID rrid("ISCT_ConditionsSvc", 1, 0);
  ASSERT_TRUE( m_tool->queryInterface(rrid  ,ppvInterface ).isSuccess() );
  }


 
TEST_F(SCT_RODVetoTool_test, canReportAbout) {
  ASSERT_TRUE(  m_tool->canReportAbout(InDetConditions::DEFAULT)  );
}


/*
 //This segfaults  the tests
 //
 //This test should probably be done as well,
 //but to do it you have to create the cabling
 //service and properly initialize it with
 //rods etc.
TEST_F(SCT_RODVetoTool_test, isGood) {
  //m_tool->initialize();
  //m_tool->fillData();
  //IdentifierHash * hashId = new IdentifierHash(0x240100);
  //IdentifierHash anyRandomHash(1000); 
  ASSERT_FALSE(  m_tool->isGood(0x240100)  );
  //ASSERT_FALSE(  m_tool->isGood(0)  );
}
*/


TEST_F(SCT_RODVetoTool_test, isGood) {
  m_tool->initialize(); 
  const Identifier elementId (0);


  ASSERT_TRUE(  m_tool->isGood( elementId , InDetConditions::DEFAULT)  );
}
     


// TEST_F(SCT_RODVetoTool_test, fillData ) {
//   m_tool->initialize();
//   ASSERT_FALSE(  m_tool->fillData().isSuccess()  );
// }


// TEST_F(SCT_RODVetoTool_test, canFillDuringInitialization) {
//   ASSERT_FALSE(  m_tool->canFillDuringInitialize()  );
// }


// TEST_F(SCT_RODVetoTool_test, isFilled ) {
//  m_tool->initialize();
 
//   auto bad_elems = CxxUtils::make_unique< std::vector<unsigned int> > ();
//   SG::WriteHandle< std::vector<unsigned int> > wh_badRODElements{"BadRODIdentifiers"};
//   wh_badRODElements.initialize();
//   wh_badRODElements.record( std::move(bad_elems) );
//   wh_badRODElements->push_back(0x240100 );
//   std::cout << wh_badRODElements.key() << std::endl;
//   std::cout << wh_badRODElements->at(0) << std::endl;


//   m_tool->fillData();

//   ASSERT_TRUE(  m_tool->filled()   );
// }




int main(int argc, char *argv[]){
      
     ::testing::InitGoogleTest( &argc, argv );
     //::testing::InitGoogleMock( &argc, argv );
     ::testing::AddGlobalTestEnvironment( new GaudiEnvironment );
     return RUN_ALL_TESTS();
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Ben Wynne <bwynne@cern.ch>
 * mostly just copied from Elmar Ritsch
 * @date March, 2018
 * @brief Tests for merging collections from EventViews
 */

#undef NDEBUG

//Views
#include "AthViews/View.h"
#include "AthViews/ViewHelper.h"

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxStoreInternal.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthLinks/ElementLink.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;



// Google Test and Google Mock
#if __GNUC__ >= 9
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#if __GNUC__ >= 9
# pragma GCC diagnostic pop
#endif


// Names for storing collections
std::string const VIEWS_NAME = "viewVector";
std::string const DATA_NAME = "dataVector";

// Set up storable objects
CLASS_DEF( DataVector<int>, 345654756, 1 )
class DummyData: public SG::AuxElement  // Derive from AuxElement to allow for aux data
{
  public:
    DummyData() : m_data(0){}
    DummyData( int data ) : m_data( data ){}
    int value() const{ return m_data; }

  private:
    int m_data;
};
CLASS_DEF( DataVector<DummyData>, 345654757, 1 )

class DummyEL {
  public:
    DummyEL() : m_dummyEL() {} // This is the expected form if no EL is given to the View

    ElementLink<TrigRoiDescriptorCollection>& get() {
      return m_dummyEL;
    }

  private:
    ElementLink<TrigRoiDescriptorCollection> m_dummyEL;
};

DummyEL dummyEL;

namespace ViewTesting {

// Gaudi Test fixture that provides a clean Gaudi environment for
// each individual test case
class GaudiFixture {

protected:
  GaudiFixture() {
    SetUpGaudi();
  }

  ~GaudiFixture() {
    TearDownGaudi();
  }

  void SetUpGaudi() {
    m_appMgr = Gaudi::createApplicationMgr();
    ASSERT_TRUE( m_appMgr!=nullptr );

    m_svcLoc = m_appMgr;
    ASSERT_TRUE( m_svcLoc.isValid() );

    m_svcMgr = m_appMgr;
    ASSERT_TRUE( m_svcMgr.isValid() );

    m_propMgr = m_appMgr;
    ASSERT_TRUE( m_propMgr.isValid() );
    ASSERT_TRUE( m_propMgr->setProperty( "EvtSel",         "NONE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "FILE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsPath", "ViewCollectionMerge_test.txt" ).isSuccess() );

    m_toolSvc = m_svcLoc->service("ToolSvc");
    ASSERT_TRUE( m_toolSvc.isValid() );

    ASSERT_TRUE( m_appMgr->configure().isSuccess() );
    ASSERT_TRUE( m_appMgr->initialize().isSuccess() );

    m_sg = nullptr;
    ASSERT_TRUE( m_svcLoc->service ("StoreGateSvc", m_sg).isSuccess() );
  }

  void TearDownGaudi() {
    ASSERT_TRUE( m_svcMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
    m_svcLoc->release();
    m_svcMgr->release();
    Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr) );
  }

  StoreGateSvc* evtStore()
  {
    return m_sg;
  }

  // protected member variables for Core Gaudi components
  IAppMgrUI*               m_appMgr = nullptr;
  SmartIF<ISvcLocator>     m_svcLoc;
  SmartIF<ISvcManager>     m_svcMgr;
  SmartIF<IToolSvc>        m_toolSvc;
  SmartIF<IProperty>       m_propMgr;
  StoreGateSvc*            m_sg = nullptr;
};


// Test fixture specifically for this environment
class ViewCollectionMerge_test: public ::testing::Test, public GaudiFixture {

protected:
  virtual void SetUp() override {
  }

  virtual void TearDown() override {
    // Let the Gaudi ServiceManager finalize all services
    ASSERT_TRUE( m_svcMgr->finalize().isSuccess() );

  }
};  // ViewCollectionMerge_test fixture


// Just read and write with handles - basic
TEST_F( ViewCollectionMerge_test, testBasicReadWrite ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", -1 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 1u );
}

// Write data to a view, and retrieve
TEST_F( ViewCollectionMerge_test, testViewReadWrite ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", -1 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Make a data vector
  auto viewData = std::make_unique< DataVector< int > >();
  viewData->push_back( new int(1) );

  // Store the data vector in a view
  SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( 0 ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 1u );

  // Retrieve the data vector from a view  
  SG::ReadHandle< DataVector< int > > inputDataHandle( DATA_NAME );
  ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( 0 ) ).isSuccess() );
  ASSERT_TRUE( inputDataHandle.isValid() );
  ASSERT_EQ( inputDataHandle->size(), 1u );
  ASSERT_EQ( *inputDataHandle->at(0), 1 );
}

// Write data to many views, and retrieve
TEST_F( ViewCollectionMerge_test, testManyViewReadWrite ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", 1 ) );
  viewVector->push_back( new SG::View( "testView", 2 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Store a data vector in each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    auto viewData = std::make_unique< DataVector< int > >();
    viewData->push_back( new int(viewIndex) );

    SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
    ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.isValid() );
  }

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Retrieve the data vector from each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    SG::ReadHandle< DataVector< int > > inputDataHandle( DATA_NAME );
    ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( inputDataHandle.isValid() );
    ASSERT_EQ( inputDataHandle->size(), 1u );
    ASSERT_EQ( *inputDataHandle->at(0), static_cast<int> (viewIndex) );
  }
}

// Write data to many views, retrieve and merge
TEST_F( ViewCollectionMerge_test, testViewMerge ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", 1 ) );
  viewVector->push_back( new SG::View( "testView", 2 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Store a data vector in each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    auto viewData = std::make_unique< DataVector< int > >();
    viewData->push_back( new int(viewIndex) );

    SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
    ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.isValid() );
  }

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Make a merge collection
  auto mergedData = std::make_unique< DataVector< int > >();

  // Retrieve the data vector from each view and merge
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    SG::ReadHandle< DataVector< int > > inputDataHandle( DATA_NAME );
    
    ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( inputDataHandle.isValid() );
    ASSERT_EQ( inputDataHandle->size(), 1u );
    ASSERT_EQ( *inputDataHandle->at(0), static_cast<int> (viewIndex) );
    
    //Merge the data
    for ( int const* inputDatum : *inputDataHandle.cptr() )
    {
      int * outputDatum = new int();
      mergedData->push_back( outputDatum );
      *outputDatum = *inputDatum;
    }
  }

  // Verify merging
  ASSERT_EQ( mergedData->size(), inputViewsHandle->size() );
  for ( unsigned int dataIndex = 0; dataIndex < mergedData->size(); ++dataIndex ) {

    ASSERT_EQ( static_cast<int>(dataIndex), *mergedData->at( dataIndex ) );
  }
}

// Write data to many views, retrieve and merge - alternative method
TEST_F( ViewCollectionMerge_test, testOtherViewMerge ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", 1 ) );
  viewVector->push_back( new SG::View( "testView", 2 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Store a data vector in each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    auto viewData = std::make_unique< DataVector< int > >();
    viewData->push_back( new int(viewIndex) );

    SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
    ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.isValid() );
  }

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Make a merge collection
  DataVector< int > mergedData(SG::VIEW_ELEMENTS);

  // Retrieve the data vector from each view and merge
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    SG::ReadHandle< DataVector< int > > inputDataHandle( DATA_NAME );

    ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( inputDataHandle.isValid() );
    ASSERT_EQ( inputDataHandle->size(), 1u );
    ASSERT_EQ( *inputDataHandle->at(0), static_cast<int> (viewIndex) );

    //Merge the data (don't fiddle incautiously with the details here)
    const size_t sizeSoFar = mergedData.size();
    mergedData.resize( sizeSoFar + inputDataHandle->size() );
    DataVector< int > inputData = *( inputDataHandle.ptr() );
    std::swap_ranges( inputData.begin(), inputData.end(), mergedData.begin() + sizeSoFar );
  }

  // Verify merging
  ASSERT_EQ( mergedData.size(), inputViewsHandle->size() );
  for ( unsigned int dataIndex = 0; dataIndex < mergedData.size(); ++dataIndex ) {

    ASSERT_EQ( static_cast<int> (dataIndex), *mergedData.at( dataIndex ) );
  }
}

// Make an element link
TEST_F( ViewCollectionMerge_test, elementLinkTest ) {

  // Make a data vector
  auto viewData = std::make_unique< DataVector< int > >();
  viewData->push_back( new int(1) );

  // Store the data vector
  SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Make element link and test
  ElementLink< DataVector< int > > dataLink( DATA_NAME, 0 );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 1 );
  ASSERT_EQ( **dataLink, 1 );
}

// Make an element link and remap it
TEST_F( ViewCollectionMerge_test, elementLinkRemapTest ) {

  // Make a data vector
  auto viewData = std::make_unique< DataVector< int > >();
  viewData->push_back( new int(1) );

  // Store the data vector
  SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Make element link and test
  ElementLink< DataVector< int > > dataLink( DATA_NAME, 0 );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 1 );
  ASSERT_EQ( **dataLink, 1 );

  // Make another data vector
  auto viewData2 = std::make_unique< DataVector< int > >();
  viewData2->push_back( new int(2) );
  viewData2->push_back( new int(3) );
  viewData2->push_back( new int(4) );

  // Store the new data vector
  SG::WriteHandle< DataVector< int > > outputDataHandle2( DATA_NAME + "2" );
  ASSERT_TRUE( outputDataHandle2.record( std::move( viewData2 ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle2.isValid() );

  // Declare remapping
  evtStore()->remap( ClassID_traits< DataVector< int > >::ID(), DATA_NAME, DATA_NAME + "2", 0 );

  // Test the link again - should not have changed
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 1 );
  ASSERT_EQ( **dataLink, 1 );

  // Trigger remapping
  dataLink.toPersistent();

  // Element link should now point to the other container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 3u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 2 );
  ASSERT_EQ( **dataLink, 2 );
}

// Make an element link and remap it in view
TEST_F( ViewCollectionMerge_test, elementLinkViewRemapTest ) {

  SG::View * testView = new SG::View( "testView", -1 );

  // Make a data vector
  auto viewData = std::make_unique< DataVector< int > >();
  viewData->push_back( new int(1) );

  // Store the data vector
  SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.setProxyDict( testView ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Make element link and test
  ElementLink< DataVector< int > > dataLink( DATA_NAME, 0, testView );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 1 );
  ASSERT_EQ( **dataLink, 1 );

  // Make another data vector
  auto viewData2 = std::make_unique< DataVector< int > >();
  viewData2->push_back( new int(2) );
  viewData2->push_back( new int(3) );
  viewData2->push_back( new int(4) );

  // Store the new data vector
  SG::WriteHandle< DataVector< int > > outputDataHandle2( DATA_NAME + "2" );
  ASSERT_TRUE( outputDataHandle2.setProxyDict( testView ).isSuccess() );
  ASSERT_TRUE( outputDataHandle2.record( std::move( viewData2 ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle2.isValid() );

  // Declare remapping - pretty hacky, have to specify view object names explicitly
  evtStore()->remap( ClassID_traits< DataVector< int > >::ID(), "_testView_" + DATA_NAME, "_testView_" + DATA_NAME + "2", 0 );

  // Test the link again - should not have changed
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 1 );
  ASSERT_EQ( **dataLink, 1 );

  // Trigger remapping
  dataLink.toPersistent();

  // Element link should now point to the other container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 3u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 2 );
  ASSERT_EQ( **dataLink, 2 );
}


// Make an element link and remap it to a merged view collection
TEST_F( ViewCollectionMerge_test, elementLinkMergeRemapTest ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", 1 ) );
  viewVector->push_back( new SG::View( "testView", 2 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Store a data vector in each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    auto viewData = std::make_unique< DataVector< int > >();
    viewData->push_back( new int(viewIndex) );

    SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
    ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.isValid() );
  }

  // Element link
  ElementLink< DataVector< int > > dataLink( DATA_NAME, 0, outputViewsHandle->at( 0 ) );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( **dataLink, 0 );

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Make a merge collection
  auto mergedData = std::make_unique< DataVector< int > >();

  // Retrieve the data vector from each view and merge
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    SG::ReadHandle< DataVector< int > > inputDataHandle( DATA_NAME );

    ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( inputDataHandle.isValid() );
    ASSERT_EQ( inputDataHandle->size(), 1u );
    ASSERT_EQ( *inputDataHandle->at(0), static_cast<int> (viewIndex) );

    //Merge the data
    for ( int const* inputDatum : *inputDataHandle.cptr() )
    {
      int * outputDatum = new int();
      mergedData->push_back( outputDatum );
      *outputDatum = *inputDatum;
    }
  }

  // Verify merging
  ASSERT_EQ( mergedData->size(), inputViewsHandle->size() );
  for ( unsigned int dataIndex = 0; dataIndex < mergedData->size(); ++dataIndex ) {

    ASSERT_EQ( static_cast<int> (dataIndex), *mergedData->at( dataIndex ) );
  }

  // Store the merged collection
  SG::WriteHandle< DataVector< int > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.record( std::move( mergedData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Declare remapping - pretty hacky, have to specify view object names explicitly
  evtStore()->remap( ClassID_traits< DataVector< int > >::ID(), "_testView_1_" + DATA_NAME, DATA_NAME, 0 );
  evtStore()->remap( ClassID_traits< DataVector< int > >::ID(), "_testView_2_" + DATA_NAME, DATA_NAME, 1 );

  // Element link should still be pointing to view container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(0) ), 0 );
  ASSERT_EQ( **dataLink, 0 );

  // Trigger remapping
  dataLink.toPersistent();

  // Element link should now point to the merged container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 2u );
  ASSERT_EQ( *( dataLink.getDataPtr()->at(1) ), 1 );
  ASSERT_EQ( **dataLink, 0 );
}

// Make an element link and remap it to a merged view collection with bookkeeping
TEST_F( ViewCollectionMerge_test, elementLinkMergeRemapBookkeepTest ) {

  // Make a view vector
  auto viewVector = std::make_unique< ViewContainer >();
  viewVector->push_back( new SG::View( "testView", 1 ) );
  viewVector->push_back( new SG::View( "testView", 2 ) );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Store a data vector in each view
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    auto viewData = std::make_unique< DataVector< DummyData > >();
    viewData->push_back( new DummyData( viewIndex ) );

    SG::WriteHandle< DataVector< DummyData > > outputDataHandle( DATA_NAME );
    ASSERT_TRUE( outputDataHandle.setProxyDict( outputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.record( std::move( viewData ) ).isSuccess() );
    ASSERT_TRUE( outputDataHandle.isValid() );
  }

  // Element link
  ElementLink< DataVector< DummyData > > dataLink( DATA_NAME, 0, outputViewsHandle->at( 0 ) );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Make a merge collection
  auto mergedData = std::make_unique< DataVector< DummyData > >();
  //auto store = std::make_unique< SG::AuxStoreInternal >(); //for some reason this doesn't work
  SG::AuxStoreInternal* store = new SG::AuxStoreInternal;
  mergedData->setStore( store );
  DummyData::Accessor< ElementLink<TrigRoiDescriptorCollection> > viewBookkeeper( "viewIndex" );

  // Retrieve the data vector from each view and merge
  for ( unsigned int viewIndex = 0; viewIndex < outputViewsHandle->size(); ++viewIndex ) {

    SG::ReadHandle< DataVector< DummyData > > inputDataHandle( DATA_NAME );


    ASSERT_TRUE( inputDataHandle.setProxyDict( inputViewsHandle->at( viewIndex ) ).isSuccess() );
    ASSERT_TRUE( inputDataHandle.isValid() );
    ASSERT_EQ( inputDataHandle->size(), 1u );
    ASSERT_EQ( inputDataHandle->at(0)->value(), static_cast<int> (viewIndex) );
    //Merge the data
    for ( DummyData const* inputDatum : *inputDataHandle.cptr() )
    {
      DummyData * outputDatum = new DummyData();
      mergedData->push_back( outputDatum );
      *outputDatum = *inputDatum;
      viewBookkeeper( *outputDatum ) = dummyEL.get();
    }
  }

  // Verify merging
  ASSERT_EQ( mergedData->size(), inputViewsHandle->size() );
  for ( unsigned int dataIndex = 0; dataIndex < mergedData->size(); ++dataIndex ) {

    ASSERT_EQ( static_cast<int> (dataIndex), mergedData->at( dataIndex )->value() );
  }

  // Store the merged collection
  SG::WriteHandle< DataVector< DummyData > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.record( std::move( mergedData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Need an aux store as well
  SG::WriteHandle< SG::AuxStoreInternal > auxHandle( DATA_NAME + "Aux." );
  ASSERT_TRUE( auxHandle.record( std::make_unique< SG::AuxStoreInternal >( store ) ).isSuccess() );
  ASSERT_TRUE( auxHandle.isValid() );

  // Declare remapping - pretty hacky, have to specify view object names explicitly
  evtStore()->remap( ClassID_traits< DataVector< DummyData > >::ID(), "_testView_1_" + DATA_NAME, DATA_NAME, 0 );
  evtStore()->remap( ClassID_traits< DataVector< DummyData > >::ID(), "_testView_2_" + DATA_NAME, DATA_NAME, 1 );

  // Element link should still be pointing to view container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( dataLink.getDataPtr()->at(0)->value(), 0 );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Trigger remapping
  dataLink.toPersistent();

  // Element link should now point to the merged container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 2u );
  ASSERT_EQ( dataLink.getDataPtr()->at(1)->value(), 1 );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Check the bookkeeping
  ASSERT_EQ( viewBookkeeper( **dataLink ), dummyEL.get() );
  ASSERT_EQ( viewBookkeeper( *dataLink.getDataPtr()->at(1) ), dummyEL.get() );
}

// Attempt to parcel all of the above with view helpers
TEST_F( ViewCollectionMerge_test, mergeHelperTest ) {

  // Make a dummy event context
  EventContext dummyContext( 0, 0 );
  Atlas::setExtendedEventContext (dummyContext, Atlas::ExtendedEventContext( evtStore(), 0 ) );

  // Parcel the view data
  auto viewData = std::vector< DataVector< DummyData > >( 2 );
  viewData[0].push_back( new DummyData( 0 ) );
  viewData[1].push_back( new DummyData( 1 ) );

  // Make a view vector and store data in each view, with a helper
  auto viewVector = std::make_unique< ViewContainer >();
  SG::WriteHandleKey< DataVector< DummyData > > outputDataHandleKey( DATA_NAME );
  ASSERT_TRUE( outputDataHandleKey.initialize().isSuccess() );
  ASSERT_TRUE( ViewHelper::makeAndPopulate( "testView", viewVector.get(), outputDataHandleKey, dummyContext, viewData ).isSuccess() );

  // Store the view vector
  SG::WriteHandle< ViewContainer > outputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( outputViewsHandle.record( std::move( viewVector ) ).isSuccess() );
  ASSERT_TRUE( outputViewsHandle.isValid() );

  // Make an element link
  ElementLink< DataVector< DummyData > > dataLink( DATA_NAME, 0, outputViewsHandle->at( 0 ) );
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Retrieve the view vector
  SG::ReadHandle< ViewContainer > inputViewsHandle( VIEWS_NAME );
  ASSERT_TRUE( inputViewsHandle.isValid() );
  ASSERT_EQ( inputViewsHandle->size(), 2u );

  // Make a merge collection
  auto mergedData = std::make_unique< DataVector< DummyData > >();
  //auto store = std::make_unique< SG::AuxStoreInternal >(); //for some reason this doesn't work
  SG::AuxStoreInternal* store = new SG::AuxStoreInternal;
  mergedData->setStore( store );

  // Merge data into the collection with a helper
  SG::ReadHandleKey< DataVector< DummyData > > inputDataHandleKey( DATA_NAME );
  ASSERT_TRUE( inputDataHandleKey.initialize().isSuccess() );
  
  MsgStream log(Athena::getMessageSvc(), "ViewCollectionMerge_test");
  ViewHelper::ViewMerger merger( evtStore(),  log);
  ASSERT_TRUE( merger.mergeViewCollection( *inputViewsHandle, inputDataHandleKey, dummyContext, *mergedData ).isSuccess() );

  // Verify merging
  ASSERT_EQ( mergedData->size(), inputViewsHandle->size() );
  for ( unsigned int dataIndex = 0; dataIndex < mergedData->size(); ++dataIndex ) {

    ASSERT_EQ( static_cast<int> (dataIndex), mergedData->at( dataIndex )->value() );
  }

  // Store the merged collection
  SG::WriteHandle< DataVector< DummyData > > outputDataHandle( DATA_NAME );
  ASSERT_TRUE( outputDataHandle.record( std::move( mergedData ) ).isSuccess() );
  ASSERT_TRUE( outputDataHandle.isValid() );

  // Need an aux store as well
  SG::WriteHandle< SG::AuxStoreInternal > auxHandle( DATA_NAME + "Aux." );
  ASSERT_TRUE( auxHandle.record( std::make_unique< SG::AuxStoreInternal >( store ) ).isSuccess() );
  ASSERT_TRUE( auxHandle.isValid() );

  // Element link should still be pointing to view container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 1u );
  ASSERT_EQ( dataLink.getDataPtr()->at(0)->value(), 0 );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Trigger remapping
  dataLink.toPersistent();

  // Element link should now point to the merged container
  ASSERT_TRUE( dataLink.isValid() );
  ASSERT_EQ( dataLink.getDataPtr()->size(), 2u );
  ASSERT_EQ( dataLink.getDataPtr()->at(1)->value(), 1 );
  ASSERT_EQ( ( *dataLink )->value(), 0 );

  // Check the bookkeeping
  DummyData::Accessor< ElementLink<TrigRoiDescriptorCollection> > viewBookkeeper( "viewIndex" );
  ASSERT_EQ( viewBookkeeper( **dataLink ), dummyEL.get() );
  ASSERT_EQ( viewBookkeeper( *dataLink.getDataPtr()->at(1) ), dummyEL.get() );
}

} // namespace ViewTesting


int main( int argc, char *argv[] ) {
  ::testing::InitGoogleTest( &argc, argv );

  //return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use the following:
  // skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}


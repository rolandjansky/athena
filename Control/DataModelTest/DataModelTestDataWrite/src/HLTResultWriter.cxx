/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataWrite/src/HLTResultWriter.cxx
 * @author snyder@bnl.gov
 * @date Mar, 2016
 * @brief Test for serialiclang warnings: unused members.zing an xAOD object into bytestream.
 */


#include "HLTResultWriter.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CView.h"
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataWrite/HVec.h"
#include "DataModelTestDataWrite/HView.h"
#include "DataModelTestDataWrite/H.h"
#include "DataModelTestDataWrite/HAuxContainer.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Holder.icc"
#include "TrigSteeringEvent/HLTResult.h"
#include "CxxUtils/make_unique.h"


HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(DMTest::C, DMTest::CVec, DMTest::CVec, DMTest::CAuxContainer) 
  HLT_REGISTER_TYPE(DMTest::C, DMTest::CView, DMTest::CView, DMTest::CAuxContainer) 
  HLT_REGISTER_TYPE(DMTest::H, DMTest::HVec, DMTest::HVec, DMTest::HAuxContainer) 
  HLT_REGISTER_TYPE(DMTest::H, DMTest::HView, DMTest::HView, DMTest::HAuxContainer) 
HLT_END_TYPE_REGISTRATION(DataModelTest)


struct class_DataModelTest
{
  typedef TypeInfo_DataModelTest map;
  static const std::string package_name;
};
  
struct TypeInfo_EDM {
  typedef HLT::TypeInformation::newlist
  ::add<class_DataModelTest> ::go
  ::done map;
};


using HLTNavDetails::HolderImp;


namespace DMTest {


/**
 * @brief Constructor.
 * @param name The algorithm name.
 * @param svc The service locator.
 */
HLTResultWriter::HLTResultWriter (const std::string &name,
                                    ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm (name, pSvcLocator),
    m_nav ("TestNav", this)
{
  declareProperty ("ResultKey", m_resultKey = "HLTResult_HLT");
  declareProperty ("Nav", m_nav);
}
  

/**
 * @brief Algorithm initialization; called at the beginning of the job.
 */
StatusCode HLTResultWriter::initialize()
{
  ATH_CHECK( m_resultKey.initialize() );
  ATH_CHECK( m_nav.retrieve() );
  HLT::TypeMaps::registerFeatureContainer<DMTest::CVec,DMTest::CVec>();
  HLT::TypeMaps::registerFeatureContainer<DMTest::CView,DMTest::CView>();
  HLT::TypeMaps::registerType<DMTest::CAuxContainer>();
  HLT::TypeMaps::registerFeatureContainer<DMTest::HVec,DMTest::HVec>();
  HLT::TypeMaps::registerFeatureContainer<DMTest::HView,DMTest::HView>();
  HLT::TypeMaps::registerType<DMTest::HAuxContainer>();
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm event processing.
 */
StatusCode HLTResultWriter::execute_r (const EventContext& ctx) const
{
  SG::WriteHandle<HLT::HLTResult> result (m_resultKey, ctx);
  ATH_CHECK( result.record (CxxUtils::make_unique<HLT::HLTResult>()) );

  m_nav->prepare();
  if (!m_nav->serialize (result->getNavigationResult(),
                         result->getNavigationResultCuts(),
                         result->getNavigationResultIDName()))
    return StatusCode::FAILURE;
  m_nav->reset();

  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm finalization; called at the end of the job.
 */
StatusCode HLTResultWriter::finalize()
{
  return StatusCode::SUCCESS;
}


} // namespace DMTest


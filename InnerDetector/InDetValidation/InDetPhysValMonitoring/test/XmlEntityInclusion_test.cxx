/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoring/test/XmlEntityInclusion_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief One test of HistogramDefinitionSvc to show entity inclusion works 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "IDPVM_GaudiFixtureBase.h"
#include "../InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "../InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <utility> //std::pair

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This test uses global svcLoc and hDefSvc, and is not thread safe.

static const std::string entityInclusionOptions("XmlEntityInclusionOptions.txt");
/*
  The job options refer to an xml file 'entityMaster.xml' which contains 
  <!ENTITY daughter SYSTEM "entityDaughter.xml">
  in the header, and then 
  &daughter;
  in the body of the document, where the daughter file contents are inserted.
  The xml parser should resolve the two files to one continuous xml.
*/

struct GaudiKernelFixture:public IDPVM_GaudiFixtureBase{
  GaudiKernelFixture(const std::string & joFilename):IDPVM_GaudiFixtureBase(joFilename){
    //nop, everything in base.
  }
};

BOOST_AUTO_TEST_SUITE(XmlEntityInclusionTest)
  GaudiKernelFixture g(entityInclusionOptions);
  const auto & svcLoc=g.svcLoc;
  ServiceHandle<IHistogramDefinitionSvc> hDefSvc ("HistogramDefinitionSvc", "test");

  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(svcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
  }
  
  BOOST_AUTO_TEST_CASE( retrieveIncludedDefinition ){
    const std::string entityDefinition("include");
    BOOST_TEST(hDefSvc->title(entityDefinition) == "Included using ENTITY!");
  }
  
BOOST_AUTO_TEST_SUITE_END()


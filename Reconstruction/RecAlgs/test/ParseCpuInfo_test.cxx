/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_PARSECPUINFO

#include <boost/test/unit_test.hpp>

#include "PathResolver/PathResolver.h"
#include "src/ParseCpuInfo.h"
#include <stdexcept>
#include <optional>

BOOST_AUTO_TEST_SUITE(ParseCpuInfoTest)
  BOOST_AUTO_TEST_CASE( ParseCpuInfoTestWithLocalFile ){
    const std::string filename("RecAlgs/cpuinfo");
    std::string fullPathname = PathResolverFindDataFile(filename);
    std::optional<CpuInfo> myCpuInfo;
    BOOST_CHECK_NO_THROW(myCpuInfo = ParseCpuInfoFromFile(fullPathname));
    BOOST_CHECK_THROW(auto dummy = ParseCpuInfoFromFile("nonExistent"), std::runtime_error);
    BOOST_CHECK(myCpuInfo.has_value());
    BOOST_CHECK(myCpuInfo->cpuId == 1);
    //
    BOOST_CHECK(myCpuInfo->cpuId_a == 0x4765656C);
    BOOST_CHECK(myCpuInfo->cpuId_b == 0x63C0110);
    //
    BOOST_TEST_MESSAGE(myCpuInfo->msg);
    BOOST_TEST_MESSAGE(myCpuInfo->cpuId_a);
    BOOST_TEST_MESSAGE(myCpuInfo->cpuId_b);
    
  }
  


BOOST_AUTO_TEST_SUITE_END()



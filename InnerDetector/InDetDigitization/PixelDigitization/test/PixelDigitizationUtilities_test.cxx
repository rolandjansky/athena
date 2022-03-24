/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_PIXELDIGITIZATION_UTILITIES

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "PathResolver/PathResolver.h"


#include "src/BichselData.h"
#include "src/PixelDigitizationUtilities.h"
#include <tuple>
#include <ostream>

namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;


namespace PixelDigitization{
  using ThreeDoubles = std::tuple<double, double, double>;
  // 
  std::ostream & 
  operator << (std::ostream &out, const ThreeDoubles &t){
    out << "[ "<<std::get<0>(t)<<", "<<std::get<1>(t)<<", "<<std::get<2>(t)<<" ]";
    return out;
  }

  std::ostream & 
  boost_test_print_type(std::ostream& ostr, const ThreeDoubles& t) {
    ostr<<t;
    return ostr;
  }
}
  
namespace boost { 
  namespace test_tools {
    namespace tt_detail {
      template<> 
      struct print_log_value<std::tuple<double, double, double> > {
        void operator()( std::ostream& os, const std::tuple<double, double, double>& t){
          PixelDigitization::operator<<(os,t);
        }
      };
    }
  }
}


using namespace PixelDigitization;

BOOST_AUTO_TEST_SUITE(PixelDigitizationUtilitiesTest)
  BOOST_AUTO_TEST_CASE(formBichselDataFileNameTest){
    BOOST_CHECK(formBichselDataFileName(10,100) == std::string("PixelDigitization/Bichsel_10_100steps.dat"));
    BOOST_CHECK(formBichselDataFileName(20,1) == std::string("PixelDigitization/Bichsel_20.dat"));
  }
  
  BOOST_AUTO_TEST_CASE(getBichselDataFromFileTest){
    std::string filename("PixelDigitization/BichselTest.dat");
    std::string fullPathname = PathResolverFindDataFile(filename);
    BOOST_TEST_MESSAGE("Test data path: "<<fullPathname);
    BichselData b;
    BOOST_CHECK_THROW(getBichselDataFromFile("nonExistentFile"), std::runtime_error);
    BOOST_CHECK_NO_THROW(b = getBichselDataFromFile(fullPathname));
    //check size of the elements
    BOOST_TEST_MESSAGE("BetaGammaLog10 size: "<<b.logBetaGammaVector.size());
    BOOST_CHECK(b.logBetaGammaVector.size() == 3);
    BOOST_CHECK(b.logCollisionEnergyVectorOfVector.size() == 3);
    BOOST_CHECK(b.logIntegratedCrossSectionsVectorOfVector.size() == 3);
    BOOST_CHECK(b.logHighestCrossSectionsVector.size() == 3);
    //check the actual values in the simple array of log(betaGamma)
    const std::vector<double> referenceLogBetaGamma{-1.0038383867e+00, -8.7889964996e-01, -7.5747750641e-01 };
    BOOST_TEST(b.logBetaGammaVector == referenceLogBetaGamma);
    //check the extrema of the component arrays 
    auto logCollisionEnergy = b.logCollisionEnergyVectorOfVector[0];
    BOOST_TEST(logCollisionEnergy.front() == 1.6800000000);
    BOOST_TEST(logCollisionEnergy.back() == 6.0000000000);
    //
    auto logIntegratedCrossSection = b.logIntegratedCrossSectionsVectorOfVector[2];
    BOOST_TEST(logIntegratedCrossSection.front() == -2.5055028196e-01);
    BOOST_TEST(logIntegratedCrossSection.back() == 5.9043516780e+00);
  }
  
  BOOST_AUTO_TEST_CASE(parseThreeDoublesTest, * utf::expected_failures(1)){
    ThreeDoubles reference{1.6800000000, -2.5055028196e-01, 5.9043516780e+00};
    const std::string source("1.6800000000 -2.5055028196e-01 5.9043516780e+00");
    auto parsedDoubles = parseThreeDoubles(source);
    BOOST_TEST(parsedDoubles == reference);
    //if you insert commas, it doesn't parse the line, and throws
    const std::string wronglyFormatted("1.6800000000, -2.5055028196e-01, 5.9043516780e+00");
    BOOST_CHECK_THROW(parseThreeDoubles(wronglyFormatted), std::runtime_error);
    //if one number fails std::stod, this is caught and thrown as runtime error
    const std::string errorInNumber("e1.ee0000000 -2.5055028196e-01 5.9043516780e+00");
    BOOST_CHECK_THROW(parseThreeDoubles(errorInNumber), std::runtime_error);
    //need to veto this in the pattern matching; for now it fails the test
    const std::string error2InNumber("1.ee0000000 -2.5055028196e-01 5.9043516780e+00");
    BOOST_CHECK_THROW(parseThreeDoubles(error2InNumber), std::runtime_error);
  }
  
  BOOST_AUTO_TEST_CASE(fastSearchTest){
    std::vector<double> testVector{1.0, 2.0, 3.0, 4.0, 5.5, 6.0};
    auto result = fastSearch(testVector, 2.0);
    BOOST_TEST_MESSAGE("first idx: "<<result.first<<", second idx: "<<result.second);
    BOOST_TEST(result.first == 1);
    BOOST_TEST(result.second == 1);
    //search for item which is below the range
    result = fastSearch(testVector, 0.0);
    BOOST_TEST(result.first == -1);
    BOOST_TEST(result.second == -1);
    //search for item which is above the range; returns invalid result
    result = fastSearch(testVector, 10.0);
    BOOST_TEST(result.first == -1);
    BOOST_TEST(result.second == -1);
    result = fastSearch(testVector, 3.4);
    BOOST_TEST(result.first == 2);
    BOOST_TEST(result.second == 3);
    result = fastSearch(testVector, 6.0);
    BOOST_TEST(result.first == 5);
    BOOST_TEST(result.second == 5);
    result = fastSearch(testVector, 1.0);
    BOOST_TEST(result.first == 0);
    BOOST_TEST(result.second == 0);
  }
  

BOOST_AUTO_TEST_SUITE_END()

/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_PIXELDIGITIZATION_BICHSELDATA

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop


#include "src/BichselData.h"
#include <cmath> //isnan
#include <utility> //pair
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

BOOST_AUTO_TEST_SUITE(BichselDataTest)
  BOOST_AUTO_TEST_CASE(BichselDataEmptyTest){
    BichselData d;
    BOOST_TEST_MESSAGE("Checks on an empty BichselData");
    BOOST_CHECK(d.empty()==true);
    BOOST_CHECK(std::isnan(d.lastBetaGammaValue()));
    const auto invalidIndices = std::make_pair<int, int>(-1,-1);
    double someBetaGamma = 4.;
    BOOST_CHECK(d.getBetaGammaIndices(someBetaGamma) == invalidIndices);
    const auto anyIndices = std::make_pair<int, int>(2,3);
    BOOST_CHECK(d.interpolateCollisionEnergy(anyIndices,someBetaGamma) == -1. );
    BOOST_CHECK(d.interpolateCrossSection(anyIndices, someBetaGamma) == -1. );
  }
  BOOST_AUTO_TEST_CASE(FillBichselDataTest){
    BichselData d;
    /** Typical entries:
    -1.0038383867e+00 1.6800000000e+00 6.3966909486e-01
    -1.0038383867e+00 1.7400000000e+00 1.8129941377e+00
    **/
    d.addEntry(-1., 1.68, 0.6);
    d.addEntry(-1., 1.74, 1.8);
    d.addEntry(-0.8, 2.8, 6.);
    d.addEntry(-0.7, 2.9, 7.);
    BOOST_TEST_MESSAGE("Checks on a BichselData with data added");
    BOOST_CHECK(d.empty()==false);
    BOOST_CHECK(d.size() == 3);
    BOOST_CHECK(d.lastBetaGammaValue()== -0.7);
    const auto expectedIndices = std::make_pair<int, int>(0,1);
    double validBetaGamma = -0.9;
    BOOST_CHECK(d.getBetaGammaIndices(validBetaGamma) == expectedIndices);
    //
    const auto validIndices = std::make_pair<int, int>(0,1);
    BOOST_CHECK(d.interpolateCollisionEnergy(validIndices,validBetaGamma) == -1. );
    BOOST_CHECK_EQUAL(d.interpolateCrossSection(validIndices, validBetaGamma), 7943.2823472428136 );
  }
BOOST_AUTO_TEST_SUITE_END()

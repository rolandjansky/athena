/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRT_G4_UTILITIES
#include <boost/test/unit_test.hpp>
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"


namespace utf = boost::unit_test;

struct setupParameters{
  const TRTParameters * p;
  setupParameters()   { 
    BOOST_TEST_MESSAGE("starting test" );
    p=TRTParameters::GetPointer();
  }
 
  ~setupParameters(){
    BOOST_TEST_MESSAGE("ending test");
  }
};

struct setupOutputFile{
  TRTOutputFile * p;
  setupOutputFile()   { 
    BOOST_TEST_MESSAGE("starting test" );
    p=TRTOutputFile::GetPointer();
  }
 
  ~setupOutputFile(){
    BOOST_TEST_MESSAGE("ending test");
  }
};


BOOST_FIXTURE_TEST_SUITE(Test_TRTParameters , setupParameters)

BOOST_AUTO_TEST_CASE( GetExistingInteger ){
  BOOST_TEST(p->GetInteger("NumberOfSectorsAB") == 24);
}

BOOST_AUTO_TEST_CASE( GetExistingDouble , * utf::tolerance(0.01)){
  BOOST_TEST(p->GetDouble("InnerRadiusOfStraw") == 2.);
}

BOOST_AUTO_TEST_CASE( GetExistingIntegerArray){
  const int numberOfStrawLayersB =p->GetInteger("NumberOfStrawLayersB");
  BOOST_TEST(numberOfStrawLayersB == 24);
  int* numberOfStrawsInLayersB = new int[numberOfStrawLayersB];
  p->GetIntegerArray("NumberOfStrawsInLayersB", numberOfStrawLayersB,numberOfStrawsInLayersB);
  std::array<int, 24> answer{19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 23};
  BOOST_CHECK_EQUAL_COLLECTIONS(numberOfStrawsInLayersB, (numberOfStrawsInLayersB+24), answer.begin(), answer.end());
  delete[] numberOfStrawsInLayersB;
}

BOOST_AUTO_TEST_CASE(GetExistingDoubleArray, * utf::tolerance(0.01)){
  const int numberOfShellCorners = p->GetInteger("NumberOfShellCorners");
  BOOST_TEST(numberOfShellCorners == 4);
  double* xOfShellCornersA = new double[numberOfShellCorners];
  p->GetDoubleArray("XOfShellCornersA", numberOfShellCorners,xOfShellCornersA);
  std::array<double, 4> answer{564.600, 564.600, 686.690, 699.874};
  BOOST_CHECK_EQUAL_COLLECTIONS(xOfShellCornersA, (xOfShellCornersA+4), answer.begin(), answer.end());
  delete[] xOfShellCornersA;
}

BOOST_AUTO_TEST_CASE( GetPartOfExistingIntegerArray){
  int numberOfLayersWithShortStrawsA =     p->GetInteger("NumberOfLayersWithShortStrawsA");
  BOOST_TEST(numberOfLayersWithShortStrawsA == 9);
  int* numberOfShortStrawsInLayersA =     new int[numberOfLayersWithShortStrawsA];
  p->GetPartOfIntegerArray("NumberOfStrawsInLayersA",   numberOfLayersWithShortStrawsA, numberOfShortStrawsInLayersA);
  std::array<int, 9> answer{15, 16, 16, 16, 16, 17, 17, 17, 17};
  BOOST_CHECK_EQUAL_COLLECTIONS(numberOfShortStrawsInLayersA, (numberOfShortStrawsInLayersA+9), answer.begin(), answer.end());
  delete[] numberOfShortStrawsInLayersA;
}

BOOST_AUTO_TEST_CASE(GetPartOfExistingDoubleArray, * utf::tolerance(0.01) ){
  const int numberOfStrawPlanesB = p->GetInteger("NumberOfStrawPlanesB");
  BOOST_TEST(numberOfStrawPlanesB == 8);
  double * rotationAnglesOfStrawPlanesB = new double[numberOfStrawPlanesB];
  p->GetPartOfDoubleArray("RotationsOfStrawPlanes",numberOfStrawPlanesB, rotationAnglesOfStrawPlanesB);
  std::array<double, 8> answer{0.000, 0.375, 0.750, 0.125, 0.500, 0.875, 0.250, 0.625};
  BOOST_CHECK_EQUAL_COLLECTIONS(rotationAnglesOfStrawPlanesB, (rotationAnglesOfStrawPlanesB+8), answer.begin(), answer.end());
  delete[] rotationAnglesOfStrawPlanesB;
}

//GetElementOfIntegerArray method is never used in code
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(Test_TRTOutputFile , setupOutputFile)
BOOST_AUTO_TEST_CASE(OutputToFile  ){
  auto & f(p->GetReference());
  BOOST_CHECK_NO_THROW((f<<"My Testing"));
}
BOOST_AUTO_TEST_SUITE_END()



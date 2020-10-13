/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "TestTools/FLOATassert.h"
#include "CxxUtils/checker_macros.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

static const unsigned int OBJVERSIONV100(100);
static const unsigned int OBJVERSIONV200(200);
static const int NCHANNELS(2);


const std::vector<std::vector<float> > channel0data = {
  {1, 2, 3, 10, 20, 30}
};

const std::vector<float> channel1data = {100, 200, 300, 1000, 2000, 3000};


void testTileCalibDrawerFltV100 ATLAS_NOT_THREAD_SAFE () {

  std::cout << "testTileCalibDrawerFltV100\n";
  
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobFlt", "blob");
  coral::AttributeList fltList(*spec);
  coral::Blob& blob = fltList["TileCalibBlobFlt"].data<coral::Blob>();
  
  TileCalibDrawerFlt* drawerFlt = TileCalibDrawerFlt::getInstance(blob, channel0data, NCHANNELS, OBJVERSIONV100);
  drawerFlt->setData(1, 0, channel1data);

  assert(drawerFlt->getNChans() == NCHANNELS);
  assert(drawerFlt->getNGains() == channel0data.size());
  assert(drawerFlt->getObjSizeUint32() == (*channel0data.begin()).size());


  assert(Athena_test::isEqual(drawerFlt->getCalib(0, 0, 2.5), 25));
  assert(Athena_test::isEqual(drawerFlt->getCalib(1, 0, 250), 2500));

  assert(Athena_test::isEqual(drawerFlt->getData(1, 0, 1), 200));

  // Test default policy
  assert(Athena_test::isEqual(drawerFlt->getCalib(10, 0, 2.5), 25));
  assert(Athena_test::isEqual(drawerFlt->getCalib(97, 0, 250), 2500));

  assert(Athena_test::isEqual(drawerFlt->getData(10, 0, 1), 2));
  assert(Athena_test::isEqual(drawerFlt->getData(97, 0, 1), 200));

  delete drawerFlt;
}


void testTileCalibDrawerFltV200 ATLAS_NOT_THREAD_SAFE () {

  std::cout << "testTileCalibDrawerFltV200\n";
  
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobFlt", "blob");
  coral::AttributeList fltList(*spec);
  coral::Blob& blob = fltList["TileCalibBlobFlt"].data<coral::Blob>();
  
  TileCalibDrawerFlt* drawerFlt = TileCalibDrawerFlt::getInstance(blob, channel0data, NCHANNELS, OBJVERSIONV200);
  drawerFlt->setData(1, 0, channel1data);

  assert(drawerFlt->getNChans() == NCHANNELS);
  assert(drawerFlt->getNGains() == channel0data.size());
  assert(drawerFlt->getObjSizeUint32() == (*channel0data.begin()).size());


  assert(Athena_test::isEqual(drawerFlt->getY(0, 0, 2.5), 25));
  assert(Athena_test::isEqual(drawerFlt->getY(1, 0, 250), 2500));

  float y(-1.0);
  float dy(0.0);
  assert(drawerFlt->getYDY(0, 0, 2.5, y, dy));
  assert(Athena_test::isEqual(y, 25));

  y = -1.0;
  dy = 0.0;
  assert(drawerFlt->getYDY(1, 0, 250, y, dy));
  assert(Athena_test::isEqual(y, 2500));


  // Test default policy
  assert(Athena_test::isEqual(drawerFlt->getY(10, 0, 2.5), 25));
  assert(Athena_test::isEqual(drawerFlt->getY(97, 0, 250), 2500));

  y = -1.0;
  dy = 0.0;
  assert(drawerFlt->getYDY(10, 0, 2.5, y, dy));
  assert(Athena_test::isEqual(y, 25));

  y = -1.0;
  dy = 0.0;
  assert(drawerFlt->getYDY(97, 0, 250, y, dy));
  assert(Athena_test::isEqual(y, 2500));

  

  delete drawerFlt;
}



int main ATLAS_NOT_THREAD_SAFE () {


  testTileCalibDrawerFltV100();
  testTileCalibDrawerFltV200();

  return 0;
}

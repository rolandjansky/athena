/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"

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

const std::vector<float> phases = {-2.0, 0.0, 0.1, 0.2, 0.5, 1.0};
const int nPhases = phases.size();

static const unsigned int OBJVERSION(3);
static const int NCHANNELS(2);
static const int NGAINS(1);
static const unsigned int NSAMPLES(1);


void test1 ATLAS_NOT_THREAD_SAFE () {

  std::cout << "test1\n";
  
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobOfc", "blob");
  coral::AttributeList ofcList(*spec);
  coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();
  
  TileCalibDrawerOfc* drawerOfc = TileCalibDrawerOfc::getInstance(blob, OBJVERSION, NSAMPLES, -nPhases, NCHANNELS, NGAINS);
  drawerOfc->setPhases(0, 0, phases);

  for (unsigned int channel = 0; channel < NCHANNELS; ++channel) {
    for (unsigned int gain = 0; gain < NGAINS; ++gain) {
      for (float phase : phases) {
        for (unsigned int isam = 0; isam < NSAMPLES; ++isam) {
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldA, channel, gain, phase, isam, phase + 100 * channel + 10.0F);
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldB, channel, gain, phase, isam, phase + 100 * channel + 20.0F);
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldG, channel, gain, phase, isam, phase + 100 * channel + 30.0F);
          if (OBJVERSION == 3) {
            drawerOfc->setOfc(TileCalibDrawerOfc::FieldC, channel, gain, phase, isam, phase + 100 * channel + 40.0F);
            drawerOfc->setOfc(TileCalibDrawerOfc::FieldDG, channel, gain, phase, isam, phase + 100 * channel + 50.0F);
          }
        }
      }
    }
  }

  assert(drawerOfc->getNSamples() == NSAMPLES);
  assert(drawerOfc->getNPhases() == -nPhases);

  int channel(0);
  int gain(0);

  float a[NSAMPLES] = {0};
  float b[NSAMPLES] = {0};
  float c[NSAMPLES] = {0};
  float g[NSAMPLES] = {0};
  float dg[NSAMPLES] = {0};

  float phase(0.0);

  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.0));
  assert(Athena_test::isEqual(b[0], 20.0));
  assert(Athena_test::isEqual(g[0], 30.0));
  assert(Athena_test::isEqual(c[0], 40.0));
  assert(Athena_test::isEqual(dg[0], 50.0));
  assert(Athena_test::isEqual(phase, 0.0));

  phase = 0.1;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.1));
  assert(Athena_test::isEqual(phase, 0.1));

  phase = 0.2;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.2));
  assert(Athena_test::isEqual(phase, 0.2));

  phase = 0.3;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.2));
  assert(Athena_test::isEqual(phase, 0.2));

  phase = 0.6;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.5));
  assert(Athena_test::isEqual(phase, 0.5));

  phase = 0.8;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 11.0));
  assert(Athena_test::isEqual(phase, 1.0));

  phase = 2.0;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 11.0));
  assert(Athena_test::isEqual(phase, 1.0));

  phase = -1.0;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.0));
  assert(Athena_test::isEqual(phase, 0.0));

  phase = -1.1;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 8.0));
  assert(Athena_test::isEqual(phase, -2.0));


  phase = -20.0;
  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 8.0));
  assert(Athena_test::isEqual(phase, -2.0));


  phase = 0.0;
  channel = 1;

  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 110.0));
  assert(Athena_test::isEqual(b[0], 120.0));
  assert(Athena_test::isEqual(g[0], 130.0));
  assert(Athena_test::isEqual(c[0], 140.0));
  assert(Athena_test::isEqual(dg[0], 150.0));
  assert(Athena_test::isEqual(phase, 0.0));

  // Test default policy (should return for channel 0)
  channel = 20;

  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 10.0));
  assert(Athena_test::isEqual(b[0], 20.0));
  assert(Athena_test::isEqual(g[0], 30.0));
  assert(Athena_test::isEqual(c[0], 40.0));
  assert(Athena_test::isEqual(dg[0], 50.0));
  assert(Athena_test::isEqual(phase, 0.0));

  // Test default policy (shoud return for channel (97 % 48) == 1)
  channel = 97;

  drawerOfc->fillOfc(channel, gain, phase, a, b, c, g, dg);
  assert(Athena_test::isEqual(a[0], 110.0));
  assert(Athena_test::isEqual(b[0], 120.0));
  assert(Athena_test::isEqual(g[0], 130.0));
  assert(Athena_test::isEqual(c[0], 140.0));
  assert(Athena_test::isEqual(dg[0], 150.0));
  assert(Athena_test::isEqual(phase, 0.0));


  delete drawerOfc;
}


int prepareReferenceFile () {

  std::ofstream refFile;
  refFile.open ("../share/TileCalibDrawerOfc_test.ref");
  refFile << "test1\n";
  refFile.close();
  return 0;
}


int main ATLAS_NOT_THREAD_SAFE () {

  //  prepareReferenceFile();

  test1();

  return 0;
}

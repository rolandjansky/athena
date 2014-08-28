// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PyDumper/PyDumperDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Generate dictionaries for adaptors needed to access event data.
 */


#include "PyDumper/PySTLAdaptor.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"


struct PyDumperDict {
  PyDumperDict();
  PyDumper::PySTLAdaptor<DetailedTrackTruthCollection> a;
  PyDumper::PySTLAdaptor<DetailedTrackTruthCollection>::iterator a_i;
};

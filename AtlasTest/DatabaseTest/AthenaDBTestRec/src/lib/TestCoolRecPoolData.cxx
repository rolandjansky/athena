/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCoolRecPoolData.cxx
// Richard Hawkings, started 15/9/06

#include "AthenaDBTestRec/TestCoolRecPoolData.h"

TestCoolRecPoolData::TestCoolRecPoolData() {m_run=0; m_chan=0; }

TestCoolRecPoolData::TestCoolRecPoolData(const int run, const int chan, 
		 const std::string folder, const int size) : 
  m_run(run), m_chan(chan), m_folder(folder) {
  m_data.resize(size);
}

TestCoolRecPoolData::~TestCoolRecPoolData() {}

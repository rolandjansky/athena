/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Csc2dSegmentMaker.h
#ifndef CscSegmentMaker_H
#define CscSegmentMaker_H

// Algorithm to find CSC 2D segments from clusters.
//
// Segmetns are witten as MuonSegment objects to a
// MuonSegmentCombinationCollection. There is a separate combination for
// each chamber (with segments) and separate r and phi collections
// for each combination.

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "CscSegmentMakers/ICscSegmentFinder.h" // MuonSegmentCombinationCollection.h MuonSegmentCombination.h included

namespace Muon {
  class CscPrepData;
}

class CscSegmentMaker : public AthAlgorithm {
  
public:  // methods

  // Constructor.
  CscSegmentMaker(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor.
  ~CscSegmentMaker();

  // Initialization.
  StatusCode initialize();

  // Event processing.
  StatusCode execute();

  // Finalization.
  StatusCode finalize();

  
private:  // methods

  StatusCode build_segments();
  
private:  // data

  // Number of events dumped.
  int m_dumped;
  // Debug flags.
  bool m_dump;
  int m_dumpcount;

  // Properties.
  std::string m_sg_inkey;
  std::string m_sg_outkey;
  std::string m_sg_4d_outkey;

  ToolHandle<ICscSegmentFinder> m_2dseg_finder;
  ToolHandle<ICscSegmentFinder> m_4dseg_finder;

  // Output container.
  std::unique_ptr<MuonSegmentCombinationCollection> m_psegs;
  std::unique_ptr<MuonSegmentCombinationCollection> m_psegs4d;

};

#endif

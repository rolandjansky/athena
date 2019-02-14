/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Csc4dSegmentMaker_H
#define Csc4dSegmentMaker_H

// Algorithm to find CSC 4D segments from clusters.
//
// Segmetns are witten as MuonSegment objects to a
// MuonSegmentCombinationCollection. There is a separate combination for
// each chamber (with segments) and separate r and phi collections
// for each combination.

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "CscSegmentMakers/ICscSegmentFinder.h" //including MuonSegmentCombination(Collection).h CscPrepDataContainer.h"

class ICscSegmentUtilTool;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Track {
  class TrackRaod;
  class RIO_OnTrack;
}

namespace Muon {
  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;
  class MuonEDMPrinterTool;
  class MuonSegment;
}

class Csc4dSegmentMaker : virtual public ICscSegmentFinder, public AthAlgTool {

public:  // methods

  // Constructor.
  Csc4dSegmentMaker(const std::string&, const std::string&, const IInterface*);

  // Destructor.
  ~Csc4dSegmentMaker();

  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

  std::unique_ptr<MuonSegmentCombinationCollection> find( const MuonSegmentCombinationCollection& ) const ;
  std::unique_ptr<MuonSegmentCombinationCollection> find( const std::vector<const Muon::CscPrepDataCollection*>& pcols) const; //not used here

private:  // data

  // Properties.
  // Number of events dumped.
  mutable int m_dumpcount;
  mutable int m_dumped;
  // Debug flags.
  mutable bool m_dump;
  double m_max_chisquare;
  double m_max_slope_r;
  double m_max_slope_phi;
  double m_max_seg_per_chamber;

  // Pointer muon geometry manager.
  const MuonGM::MuonDetectorManager* m_pgm;
  const CscIdHelper* m_phelper;

  ToolHandle<ICscSegmentUtilTool> m_segmentTool;  
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h" // added..

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

class Csc4dSegmentMaker : virtual public ICscSegmentFinder, virtual public Muon::IMuonSegmentMaker, public AthAlgTool {

public:  // methods

  // Constructor.
  Csc4dSegmentMaker(const std::string&, const std::string&, const IInterface*);

  // Destructor.
  ~Csc4dSegmentMaker();

  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

  MuonSegmentCombinationCollection* find( const std::vector<const Muon::CscPrepDataCollection*>& pcols);
  MuonSegmentCombinationCollection* find( const MuonSegmentCombinationCollection& );

  // Only for Csc4dSegmentMaker
  std::vector<const Muon::MuonSegment*>*  find( const Trk::TrackRoad &,
                                                const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > > &,
                                                const std::vector< std::vector< const Muon::MuonClusterOnTrack* > > &  ,
                                                bool, double);
  std::vector<const Muon::MuonSegment*>*  find( const Amg::Vector3D&, const Amg::Vector3D&,
                                                const std::vector< const Muon::MdtDriftCircleOnTrack* > &,
                                                const std::vector< const Muon::MuonClusterOnTrack* > &  ,
                                                bool, double);

  std::vector<const Muon::MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& /*rios*/ );
  std::vector<const Muon::MuonSegment*>* find( const std::vector<const Trk::RIO_OnTrack*>& /*rios1*/,
                                         const std::vector<const Trk::RIO_OnTrack*>& /*rios2*/ ) ;
  std::vector<const Muon::MuonSegment*>* find( const std::vector<const Muon::MdtDriftCircleOnTrack*>& /*mdts*/,
                                         const std::vector<const Muon::MuonClusterOnTrack*>&  /*clusters*/);


private:  // data

  // Properties.
  // Number of events dumped.
  int m_dumpcount;
  int m_dumped;
  // Debug flags.
  bool m_dump;
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

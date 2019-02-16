/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Csc2dSegmentMaker.h
#ifndef Csc2dSegmentMaker_H
#define Csc2dSegmentMaker_H

// Algorithm to find CSC 2D segments from clusters.
//
// Segmetns are witten as MuonSegment objects to a
// MuonSegmentCombinationCollection. There is a separate combination for
// each chamber (with segments) and separate r and phi collections
// for each combination.

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" // separately...
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
//#include "MuonCondInterface/CscICoolStrSvc.h"
// MuonSegmentCombination(Collection).h included
// and "MuonPrepRawData/CscPrepDataContainer.h"

class ICscClusterFitter;
class ICscStripFitter;
class ICscSegmentUtilTool;
class ICSCConditionsSvc;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class IMuonClusterOnTrackCreator;
  class MdtDriftCircleOnTrack;
  class MuonClusterOnTrack;
  class MuonSegment;
  class MuonIdHelperTool;
  class IMuonClusterOnTrackCreator;
  class MuonEDMPrinterTool;
}
namespace Track{
  class RIO_OnTrack;
}

class Csc2dSegmentMaker : virtual public ICscSegmentFinder, public AthAlgTool {

public:  // methods

  // Constructor.
  Csc2dSegmentMaker(const std::string&, const std::string&, const IInterface*);

  // Destructor.
  ~Csc2dSegmentMaker();

  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

  std::unique_ptr<MuonSegmentCombinationCollection> find( const std::vector<const Muon::CscPrepDataCollection*>& pcols) const;
  std::unique_ptr<MuonSegmentCombinationCollection> find( const MuonSegmentCombinationCollection& ) const;

private:  // methods

  Muon::MuonSegmentCombination* findSegmentCombination( const Muon::CscPrepDataCollection& pcol ) const;
  

private:  // data

  // Pointer muon geometry manager.
  const MuonGM::MuonDetectorManager* m_pgm;
  
  // Output container.
  //  MuonSegmentCombinationCollection* m_psegs;
  const CscIdHelper* m_phelper;
 
  std::string m_cscdig_sg_inkey;

  ToolHandle<ICscSegmentUtilTool> m_segmentTool;  
  ToolHandle<Muon::IMuonClusterOnTrackCreator> m_cscClusterOnTrackCreator;  
  ToolHandle<Muon::MuonIdHelperTool> m_idHelper;  
  ToolHandle<Muon::MuonEDMPrinterTool> m_printer;
  //ServiceHandle<MuonCalib::CscICoolStrSvc> m_cscCoolStrSvc;
  //ServiceHandle<ICSCConditionsSvc> m_cscCondSvc; //CSC conditions

};

#endif

   


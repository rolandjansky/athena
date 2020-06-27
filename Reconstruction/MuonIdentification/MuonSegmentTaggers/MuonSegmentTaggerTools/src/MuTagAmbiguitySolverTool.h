/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuTagAmbiguitySolverTool_H
#define MuTagAmbiguitySolverTool_H

#include "MuonSegmentTaggerToolInterfaces/IMuTagAmbiguitySolverTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkSegment/SegmentCollection.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>

/**
   @class MuTagAmbiguitySolverTool
   
   @author Zdenko.van.Kesteren@cern.ch
  
*/

namespace Muon {
  class MuonSegment;
}

class MuTagAmbiguitySolverTool : virtual public IMuTagAmbiguitySolverTool, public AthAlgTool{
 public:
  MuTagAmbiguitySolverTool(const std::string& t ,const std::string& n ,const IInterface* p);
  virtual ~MuTagAmbiguitySolverTool()=default;
  
  virtual StatusCode initialize();
  
  std::vector<  MuonCombined::MuonSegmentInfo > solveAmbiguities( std::vector<  MuonCombined::MuonSegmentInfo > mtos ) const ;

  std::vector<  MuonCombined::MuonSegmentInfo > selectBestMuTaggedSegments(  std::vector<  MuonCombined::MuonSegmentInfo > mtss ) const ;
  
 private:
  ///////////////////////////////////
  int     ambiguousSegment( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const;
  double  Rseg( unsigned int nseg ) const ;

  ServiceHandle<Muon::IMuonEDMHelperSvc>  m_edmHelperSvc  {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" }; //!< Pointer on IMuonEDMHelperSvc
  ToolHandle< Muon::MuonEDMPrinterTool > p_muonPrinter ; //!< Pointer on MuonEDMPrinterTool
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle< Muon::IMuonSegmentMatchingTool> p_segmentMatchingTool ; //!< Pointer on MuonSegmentMatchingTool

  bool m_hitOverlapMatching; //!< check hit overlap of segments in ambi solving
  bool m_slOverlapMatching; //!< for segments in a SL overlap in the same station layer, check whether from same particle
  bool m_rejectOuterEndcap; //!< reject Endcap Outer one station layer tags (without EI or EM)  
  bool m_rejectMatchPhi; //!< reject one station tags with phi hits and a fabs(MatchPhi = minPullPhi) > 3 
};



#endif //MuTagAmbiguitySolverTool_H

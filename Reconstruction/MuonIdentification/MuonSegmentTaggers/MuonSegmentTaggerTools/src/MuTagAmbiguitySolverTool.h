/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuTagAmbiguitySolverTool_H
#define MuTagAmbiguitySolverTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagAmbiguitySolverTool.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"

class StoreGateSvc;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;

/**
   @class MuTagAmbiguitySolverTool
   
   @author Zdenko.van.Kesteren@cern.ch
  
*/

namespace Muon {
  class MuonSegment;
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMuonSegmentMatchingTool;
}

class MuTagAmbiguitySolverTool : virtual public IMuTagAmbiguitySolverTool, public AthAlgTool{
 public:
  MuTagAmbiguitySolverTool(const std::string& t ,const std::string& n ,const IInterface* p);
  virtual ~MuTagAmbiguitySolverTool          ();
  
  virtual StatusCode initialize        ();
  virtual StatusCode finalize          ();
  
  std::vector<  MuonCombined::MuonSegmentInfo > solveAmbiguities( std::vector<  MuonCombined::MuonSegmentInfo > mtos ) ;

  std::vector<  MuonCombined::MuonSegmentInfo > selectBestMuTaggedSegments(  std::vector<  MuonCombined::MuonSegmentInfo > mtss );
  
 private:
  ///////////////////////////////////
  int     ambiguousSegment( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const;
  double  Rseg( unsigned int nseg ) const ;

  ToolHandle< Muon::MuonEDMHelperTool >  p_muonHelper ; //!< Pointer on MuonEDMHelperTool
  ToolHandle< Muon::MuonEDMPrinterTool > p_muonPrinter ; //!< Pointer on MuonEDMPrinterTool
  ToolHandle< Muon::MuonIdHelperTool >   p_muonIdHelper ; //!< Pointer on MuonIdHelperTool
  ToolHandle< Muon::IMuonSegmentMatchingTool> p_segmentMatchingTool ; //!< Pointer on MuonSegmentMatchingTool
  StoreGateSvc* p_StoreGateSvc ; //!< Pointer On StoreGateSvc
   
  const MdtIdHelper*  m_mdtIdHelper;
  const CscIdHelper*  m_cscIdHelper;
  const RpcIdHelper*  m_rpcIdHelper;
  const TgcIdHelper*  m_tgcIdHelper;

  bool m_hitOverlapMatching; //!< check hit overlap of segments in ambi solving
  bool m_slOverlapMatching; //!< for segments in a SL overlap in the same station layer, check whether from same particle
  bool m_rejectOuterEndcap; //!< reject Endcap Outer one station layer tags (without EI or EM)  
  bool m_rejectMatchPhi; //!< reject one station tags with phi hits and a fabs(MatchPhi = minPullPhi) > 3 
};



#endif //MuTagAmbiguitySolverTool_H

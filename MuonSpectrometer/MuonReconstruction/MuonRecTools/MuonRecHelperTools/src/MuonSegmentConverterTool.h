/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTCONVERTERTOOL_H
#define MUON_MUONSEGMENTCONVERTERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthLinks/ElementLink.h"
#include "TrkSegment/SegmentCollection.h"

#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"

static const InterfaceID IID_MuonSegmentConverterTool("xAODMaker::IMuonSegmentConverterTool",1,0);

namespace Muon {
 
  class MuonSegmentConverterTool : public AthAlgTool, virtual public xAODMaker::IMuonSegmentConverterTool {
  public:
    /** @brief default AlgTool constructor */
    MuonSegmentConverterTool(const std::string&, const std::string&, const IInterface*);
    
    /** @brief destructor */
    ~MuonSegmentConverterTool();
    
    /** @brief initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** @brief finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonSegmentConverterTool; }

    /** @brief convert a ElementLink to a Trk::Segment (should be of type MuonSegment) to a xAOD::MuonSegment, the segment is add to the container if provided 
        so the caller should not delete it */
    xAOD::MuonSegment* convert( const ElementLink< ::Trk::SegmentCollection >& segLink, xAOD::MuonSegmentContainer* container = 0 ) const;

    /** @brief convert a Muon::MuonSegment to a xAOD::MuonSegment, the segment is add to the container if provided 
        so the caller should not delete it */
    xAOD::MuonSegment* convert( const MuonSegment& seg, xAOD::MuonSegmentContainer* container = 0 ) const;

  private:

    /** helper function to dress output segment with cluster hit timing information */
    void addClusterTiming( const MuonSegment& seg , xAOD::MuonSegment& xaodSeg) const;

    ToolHandle<IMuonSegmentHitSummaryTool> m_hitSummaryTool;
    ToolHandle<MuonIdHelperTool>  m_idHelper;
    ToolHandle<MuonEDMHelperTool> m_edmHelper;
    ToolHandle<IMuonHitTimingTool> m_hitTimingTool;

  };
  
}

#endif

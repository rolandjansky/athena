/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSEGMENTMATCHINGTOOL_H
#define MUON_MUONLAYERSEGMENTMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"

#include "MuonLayerEvent/MuonSystemExtension.h"

#include <vector>

namespace Muon {
  
  class MuonSegment;

  class MuonLayerSegmentMatchingTool : virtual public Muon::IMuonLayerSegmentMatchingTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerSegmentMatchingTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerSegmentMatchingTool()=default;
    StatusCode initialize();

    /**IMuonLayerSegmentMatchingTool interface: select */   
    void select( const MuonSystemExtension::Intersection& intersection, const std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, 
                 std::vector< std::shared_ptr<const Muon::MuonSegment> >& selectedSegments ) const;

  private:
    /** match segment to intersection */
    bool match( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment ) const;

    /** tool handles */
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<Trk::IExtrapolator>                    m_extrapolator; 
    ToolHandle<IMuTagMatchingTool>                    m_matchingTool; 
  };
}



#endif
 



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSEGMENTMATCHINGTOOL_H
#define MUON_MUONLAYERSEGMENTMATCHINGTOOL_H

#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonLayerEvent/MuonSystemExtension.h"

class IMuTagMatchingTool;

namespace Trk {
  class IExtrapolator;
}

namespace Muon {
  
  class MuonSegment;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;

  class MuonLayerSegmentMatchingTool : virtual public Muon::IMuonLayerSegmentMatchingTool,  public AthAlgTool {
  public:

    /** Default AlgTool functions */
    MuonLayerSegmentMatchingTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonLayerSegmentMatchingTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonLayerSegmentMatchingTool interface: select */   
    void select( const MuonSystemExtension::Intersection& intersection, const std::vector< std::shared_ptr<const Muon::MuonSegment> >& segments, 
                 std::vector< std::shared_ptr<const Muon::MuonSegment> >& selectedSegments ) const;

  private:
    /** match segment to intersection */
    bool match( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment ) const;

    /** tool handles */
    ToolHandle<MuonIdHelperTool>                      m_idHelper; 
    ToolHandle<MuonEDMPrinterTool>                    m_printer; 
    ToolHandle<Trk::IExtrapolator>                    m_extrapolator; 
    ToolHandle<IMuTagMatchingTool>                    m_matchingTool; 
  };
}



#endif
 



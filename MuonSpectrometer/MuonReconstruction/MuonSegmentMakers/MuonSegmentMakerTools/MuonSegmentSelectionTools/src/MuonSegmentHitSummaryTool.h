/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTHITSUMMARYTOOL_H
#define MUON_MUONSEGMENTHITSUMMARYTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "Identifier/Identifier.h"

class MsgStream;
class MdtIdHelper;

namespace Muon {
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
  class MuonSegment;
  
}

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  

  /**
     @brief tool to calculate muon segment hit summary
  */
  class MuonSegmentHitSummaryTool : virtual public IMuonSegmentHitSummaryTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentHitSummaryTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentHitSummaryTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();

    /** @brief calculate segment hit counts */
    virtual HitCounts getHitCounts( const MuonSegment& seg ) const;
    
  private:

    ToolHandle<Muon::MuonIdHelperTool>               m_idHelperTool;     //!< IdHelper tool
    ToolHandle<Muon::MuonEDMHelperTool>              m_helperTool;       //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer;          //!< EDM printer tool
    const MuonGM::MuonDetectorManager* m_detMgr;

    double m_positionAlongTubeCut; //!< cut on the distance from the tube wall use for hole count
    double m_lowerADCBound; //!< lower bound for good MDT hits

  };

}

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTPAIRMATCHINGTOOL_H
#define MUON_MUONSEGMENTPAIRMATCHINGTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentPairMatchingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


#include "TrkTrack/TrackCollection.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

class MdtIdHelper;

namespace Muon {
  class MuonSegment;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
}

namespace Muon {
  

  /**
     @brief tool to match segments

  */
  class MuonSegmentPairMatchingTool : virtual public IMuonSegmentPairMatchingTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentPairMatchingTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentPairMatchingTool ();
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief performance match and return result */
    SegmentMatchResult matchResult( const MuonSegment& seg1, const MuonSegment& seg2 ) const; 
    std::pair<Amg::Vector3D, Amg::Vector3D> getShortestTubePos(const Muon::MuonSegment& seg) const;

  private:
    
    ToolHandle<MuonIdHelperTool>               m_idHelper;         //!< IdHelper tool
    ServiceHandle<IMuonEDMHelperSvc>           m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };       //!< EDM Helper tool
    ToolHandle<MuonEDMPrinterTool>             m_printer;          //!< EDM printer tool
  };

}

#endif

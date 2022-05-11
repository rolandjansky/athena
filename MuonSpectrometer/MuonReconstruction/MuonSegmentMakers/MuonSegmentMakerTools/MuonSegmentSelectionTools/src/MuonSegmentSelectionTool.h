/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTSELECTIONTOOL_H
#define MUON_MUONSEGMENTSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"

namespace Muon {
class MuonSegment;
}

namespace Muon {


/**
   @brief tool to select MDT segments, all CSC segments will be kept
   Quality levels:
   Level 0 = remove segments with more mdt holes than mdt hits that have no phi hits
             remove segments with less than 3 mdt hits
   Level 1 = remove segments with more than 50% mdt holes and no phi hits
                             with phi hits and more mdt holes than mdt hits
   Level 2 = remove segments with mdt hits in only one multilayer
   Level 3 = remove segments with mdt holes or no phi hits in chambers where one would expect phi hits
*/
class MuonSegmentSelectionTool : virtual public IMuonSegmentSelectionTool, public AthAlgTool {
  public:
    MuonSegmentSelectionTool(const std::string&, const std::string&, const IInterface*);

    virtual ~MuonSegmentSelectionTool() = default;

    StatusCode initialize();

    /** @brief select segment */
    bool select(const MuonSegment& seg, bool ignoreHoles = false, int qualityLevel = 0, bool useEta = true,
                bool usePhi = true) const;

    /** @brief segment quality */
    int quality(const MuonSegment& seg, bool ignoreHoles = false, bool useEta = true, bool usePhi = true) const;

  private:
    /** @brief calculate segment quality for MDT segments */
    int mdtSegmentQuality(const MuonSegment& seg, bool ignoreHoles) const;


    /** @brief calculate segment quality for CSC segments */
    int cscSegmentQuality(const MuonSegment& seg, bool useEta, bool usePhi) const;

    /** @brief calculate segment quality for NSW segments */
    int nswSegmentQuality(const MuonSegment& seg, const Identifier& chid, bool ignoreHoles) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>      m_edmHelperSvc{
        this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface"};  //!< EDM Helper tool

    PublicToolHandle<MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };  //!< EDM printer tool
    
    PublicToolHandle<IMuonSegmentHitSummaryTool> m_hitSummaryTool{
        this,
        "MuonSegmentHitSummaryTool",
        "Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool",
    };  //!< hit summary tool

    Gaudi::Property<double> m_cutSegmentQuality{this, "SegmentQualityCut", 10.};    //!< cut on the segment quality
    Gaudi::Property<double> m_adcFractionCut{this, "GoodADCFractionCut",-1};       //!< cut on fraction of MDT hits above ADC cut
    Gaudi::Property<int>    m_minAdcPerSegmentCut{this,"MinADCPerSegmentCut",70 };  //!< minimum value for the MDT with the highest ADC value on the segment
};

}  // namespace Muon

#endif

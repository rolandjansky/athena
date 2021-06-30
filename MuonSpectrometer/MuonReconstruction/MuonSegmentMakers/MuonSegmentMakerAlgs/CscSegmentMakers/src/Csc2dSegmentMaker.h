/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Csc2dSegmentMaker_H
#define Csc2dSegmentMaker_H

// Algorithm to find CSC 2D segments from clusters.
//
// Segmetns are witten as MuonSegment objects to a
// MuonSegmentCombinationCollection. There is a separate combination for
// each chamber (with segments) and separate r and phi collections
// for each combination.

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

class Csc2dSegmentMaker : virtual public ICscSegmentFinder, public AthAlgTool {
public:
    Csc2dSegmentMaker(const std::string&, const std::string&, const IInterface*);

    ~Csc2dSegmentMaker() = default;

    StatusCode initialize();

    std::unique_ptr<MuonSegmentCombinationCollection> find(const std::vector<const Muon::CscPrepDataCollection*>& pcols,
                                                           const EventContext& ctx) const;
    std::unique_ptr<MuonSegmentCombinationCollection> find(const MuonSegmentCombinationCollection&, const EventContext& ctx) const;

private:
    Muon::MuonSegmentCombination* findSegmentCombination(const Muon::CscPrepDataCollection& pcol, const EventContext& ctx) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    Gaudi::Property<std::string> m_cscdig_sg_inkey{this, "scdig_sg_inkey", "CSC_Measurements"};

    ToolHandle<ICscSegmentUtilTool> m_segmentTool{this, "segmentTool", "CscSegmentUtilTool/CscSegmentUtilTool"};
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_cscClusterOnTrackCreator{this, "cscRotCreator",
                                                                            "Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"};
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "printerTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
};

#endif

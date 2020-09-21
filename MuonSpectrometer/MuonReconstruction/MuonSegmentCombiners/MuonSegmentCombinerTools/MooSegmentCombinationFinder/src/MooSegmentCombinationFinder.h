/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMOOSEGMENTFINDER_H
#define MUONMOOSEGMENTFINDER_H

#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonCurvedSegmentCombiner.h"
#include "MuonSegmentCombinerToolInterfaces/IMuonSegmentCombinationCleanerTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternSegmentMaker.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "TrkSegment/SegmentCollection.h"


class ICscSegmentFinder;
class MdtIdHelper;

namespace Muon {
class IMuonHoughPatternFinderTool;
class IMuonPatternSegmentMaker;
class IMuonCurvedSegmentCombiner;
class IMuonSegmentCombinationCleanerTool;
class IMuonSegmentSelectionTool;
class MuonEDMPrinterTool;

/** @class MooSegmentCombinationFinder

    @author  Edward Moyse <edward.moyse@cern.ch>
*/

class MooSegmentCombinationFinder : virtual public IMooSegmentCombinationFinder, public AthAlgTool {
  public:
    MooSegmentCombinationFinder(const std::string&, const std::string&, const IInterface*);

    /** default destructor */
    virtual ~MooSegmentCombinationFinder() = default;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();

    /** This method cannot currently be const, since it needs to call non-const methods of child tools.
    However this should be okay for MT as long as this tool is a private tool of the parent Algorithm.*/
    void findSegments(const std::vector<const MdtPrepDataCollection*>& mdtCols,
                      const std::vector<const CscPrepDataCollection*>& cscCols,
                      const std::vector<const TgcPrepDataCollection*>& tgcCols,
                      const std::vector<const RpcPrepDataCollection*>& rpcCols,
                      IMooSegmentCombinationFinder::Output&            output);

  private:
    /** helper functions to print summary output for the different stages */
    void printStage(std::string stageTag) const;
    void printSummary(std::string stageTag, const MuonSegmentCombinationCollection* col) const;
    void printSummary(std::string stageTag, const MuonPatternCombinationCollection* col) const;
    void printSummary(std::string stageTag, const Trk::SegmentCollection* col) const;

    /** helper functions to write out intermediate results */
    void postProcess(MuonSegmentCombinationCollection*         col,
                     MuonSegmentCombPatternCombAssociationMap& segmentPatternMap);

    /** extract a segment collection from a segment combination collection */
    void extractSegmentCollection(const MuonSegmentCombinationCollection* combiCol,
                                  Trk::SegmentCollection&                 segments) const;

    /** select segment on quality */
    bool                goodSegment(const MuonSegment& segment) const;
    std::pair<int, int> hitsInMultilayer(const Muon::MuonSegment& segment) const;
    bool                firstIsBest(const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2) const;

    Gaudi::Property<bool> m_doSummary{this, "DoSummary", false, "Print summary after each stage"};
    Gaudi::Property<bool> m_doCscSegments{this, "DoCscSegments", true, "Run CSC segment finding"};
    Gaudi::Property<bool> m_doMdtSegments{this, "DoMdtSegments", true, "Run MDT segment finding"};
    Gaudi::Property<bool> m_doSegmentCombinations{this, "DoSegmentCombinations", false,
                                                  "Run segment combination finding"};
    Gaudi::Property<bool> m_doSegmentCombinationCleaning{this, "DoSegmentCombinationCleaning", false,
                                                         "Run segment combination cleaning"};
    Gaudi::Property<bool> m_cloneSegments{this, "CloneSegments", false, ""};

    ToolHandle<MuonEDMPrinterTool>          m_edmPrinter{"Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ServiceHandle<IMuonEDMHelperSvc>        m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                    "Handle to the service providing the IMuonEDMHelperSvc interface"};
    ServiceHandle<Muon::IMuonIdHelperSvc>   m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ToolHandle<ICscSegmentFinder>           m_csc2dSegmentFinder{this, "Csc2dSegmentMaker",
                                                       "Csc2dSegmentMaker/Csc2dSegmentMaker"};
    ToolHandle<ICscSegmentFinder>           m_csc4dSegmentFinder{this, "Csc4dSegmentMaker",
                                                       "Csc4dSegmentMaker/Csc4dSegmentMaker"};
    ToolHandle<IMuonHoughPatternFinderTool> m_houghPatternFinder{this, "HoughPatternFinder",
                                                       "Muon::MuonHoughPatternFinderTool/MuonHoughPatternFinderTool"};
    ToolHandle<IMuonPatternSegmentMaker>    m_patternSegmentMaker{this, "MdtSegmentMaker",
                                                               "Muon::MuonPatternSegmentMaker/MuonPatternSegmentMaker"};
    ToolHandle<IMuonCurvedSegmentCombiner>  m_curvedSegmentCombiner{
        this, "SegmentCombiner", "Muon::MuonCurvedSegmentCombiner/MuonCurvedSegmentCombiner"};
    ToolHandle<IMuonSegmentCombinationCleanerTool> m_segmentCombinationCleaner{
        this, "SegmentCombinationCleaner", "Muon::MuonSegmentCombinationCleanerTool/MuonSegmentCombinationCleanerTool"};
    ToolHandle<IMuonSegmentSelectionTool> m_segmentSelector{this, "SegmentSelector",
                                                            "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"};


    /** counters */
    mutable std::atomic_uint m_nevents{0};
    mutable std::atomic_uint m_ncsc2SegmentCombinations{0};
    mutable std::atomic_uint m_ncsc4SegmentCombinations{0};
    mutable std::atomic_uint m_npatternCombinations{0};
    mutable std::atomic_uint m_nmdtSegmentCombinations{0};
    mutable std::atomic_uint m_ncombinedSegmentCombinations{0};
    mutable std::atomic_uint m_ncleanedSegmentCombinations{0};
    mutable std::atomic_uint m_nsegments{0};
    mutable std::atomic_uint m_nsegmentsStraight{0};
    mutable std::atomic_uint m_nsegmentsCurved{0};
    mutable std::atomic_uint m_nremovedBadSegments{0};
};

}  // namespace Muon

#endif

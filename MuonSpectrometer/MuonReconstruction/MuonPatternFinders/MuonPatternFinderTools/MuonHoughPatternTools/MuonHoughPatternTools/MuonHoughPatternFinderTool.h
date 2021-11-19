/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNFINDERTOOL_H
#define MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNFINDERTOOL_H

#include <TFile.h>
#include <TH1.h>

#include <iostream>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPattern/MuonPatternCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"
#include "MuonRecToolInterfaces/IMuonHoughPatternFinderTool.h"
#include "MuonRecToolInterfaces/IMuonHoughPatternTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkDriftCircleMath/DriftCircle.h"

class MuonHoughHitContainer;

namespace Muon {

    class MuonHoughPatternFinderTool : virtual public IMuonHoughPatternFinderTool, public AthAlgTool {
    public:
        /** constructor */
        MuonHoughPatternFinderTool(const std::string&, const std::string&, const IInterface*);

        /** destructor */
        virtual ~MuonHoughPatternFinderTool();

        /** initialize */
        virtual StatusCode initialize() override;
        /** finalize */
        virtual StatusCode finalize() override;

        /** find patterns for a give set of MuonPrepData collections + optionally CSC
         * segment combinations */
        std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<Muon::HoughDataPerSectorVec>> find(
            const std::vector<const MdtPrepDataCollection*>& mdtCols, const std::vector<const CscPrepDataCollection*>& cscCols,
            const std::vector<const TgcPrepDataCollection*>& tgcCols, const std::vector<const RpcPrepDataCollection*>& rpcCols,
            const MuonSegmentCombinationCollection* cscSegmentCombis, const EventContext& ctx) const override;

        std::pair<std::unique_ptr<MuonPatternCombinationCollection>, std::unique_ptr<HoughDataPerSectorVec>> find(
            const MdtPrepDataContainer* mdtCont, const CscPrepDataContainer* cscCols, const TgcPrepDataContainer* tgcCont,
            const RpcPrepDataContainer* rpcCont, const sTgcPrepDataContainer* stgcCont, const MMPrepDataContainer* mmCont,
            const EventContext& ctx) const override;

    private:
        template <class T> std::vector<const T*> stdVec(const MuonPrepDataContainer<T>* cont) const;

        /** retrieves all hits and converts them into internal EDM */
        std::unique_ptr<MuonHoughHitContainer> getAllHits(
            const std::vector<const MdtPrepDataCollection*>& mdtCols, const std::vector<const CscPrepDataCollection*>& cscCols,
            const std::vector<const TgcPrepDataCollection*>& tgcCols, const std::vector<const RpcPrepDataCollection*>& rpcCols,
            const MuonSegmentCombinationCollection* cscSegmentCombis, std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
            std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap,
            std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const;

        /** analyse hits */
        MuonPatternCombinationCollection* analyse(
            const MuonHoughHitContainer& hitcontainer,
            const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phietahitassociation,
            const EventContext& ctx) const;

    private:
        /** record patterncollection to storegate or deletes collection when
         * m_recordAllOutput is false */
        void record(std::unique_ptr<MuonPrdPatternCollection>& patCol, const SG::WriteHandleKey<MuonPrdPatternCollection>& key,
                    const EventContext& ctx) const;

        /** convert and add rpc preprawdata collection (1 chamber) */
        void addRpcCollection(
            const RpcPrepDataCollection* rpc_coll, MuonHoughHitContainer& hitcontainer,
            std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
            std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const;
        /** convert and add mdt preprawdata collection (1 chamber) */
        void addMdtCollection(const MdtPrepDataCollection* mdt_coll, MuonHoughHitContainer& hitcontainer,
                              std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap,
                              std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap) const;
        /** convert and add csc preprawdata collection (1 chamber) */
        void addCscCollection(
            const CscPrepDataCollection* csc_coll, MuonHoughHitContainer& hitcontainer,
            std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const;
        /** convert and add tgc preprawdata collection (1 chamber) */
        void addTgcCollection(
            const Muon::TgcPrepDataCollection*, MuonHoughHitContainer& hitcontainer,
            std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap,
            std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& phietahitassociation) const;

        /** finds best segment for given driftcircle vector (nl1/2 = number of dc's in
         * ml 1 and 2, angledif is difference between angle of segment and
         * chamberangle, sel is vector of selected hits (0 not selected, 1 selected)
         */
        void fastSegmentFinder(TrkDriftCircleMath::DCVec& dcs, int& nl1, int& nl2, double& angleDif, std::vector<int>& sel) const;

        /** calculateStationCode(const Identifier)*/
        int calculateStationCode(const Identifier) const;

        /** update station map for rpc chamber, with id of chamber, and size of hits
         * in rpc chamber */
        void updateRpcMdtStationMap(const Identifier rpcid, int hit_begin, int hit_end,
                                    std::map<int, std::vector<std::pair<int, int>>>& rpcmdtstationmap) const;

        /** update station map for tgc chamber, with id of chamber, and size of hits
         * in tgc chamber */
        void updateTgcMdtStationMap(const Identifier tgcid, int hit_begin, int hit_end,
                                    std::map<int, std::vector<std::pair<int, int>>>& tgcmdtstationmap) const;

        /** calculates an unique stationcode integer (own convention)*/
        int stationCode(Identifier id) const;

        /** calculates an unique stationcode integer (own convention)*/
        static int stationCode(int stationname, int phi, int eta);

        static void addToStationMap(std::map<int, std::vector<std::pair<int, int>>>& stationmap,
                                    std::map<int, std::vector<std::pair<int, int>>>::iterator& it, int& stationcode, const int& hit_begin,
                                    const int& hit_end);

        ToolHandle<IMuonHoughPatternTool> m_muonHoughPatternTool{this, "muonHoughPatternTool",
                                                                 "MuonHoughPatternTool"};  //!< Pointer to concrete tool
        ToolHandle<Muon::IMuonCombinePatternTool> m_muonCombinePatternTool{this, "muonCombinePatternTool",
                                                                           "MuonCombinePatternTool"};  //!< Pointer to concrete tool
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "printerTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
                                                       "ToolHandle for EDM printing of segments"};

        /** reweight hits (true) */
        Gaudi::Property<bool> m_hit_reweights{this, "HitReweights", true};
        /** use adc cut (true) */
        Gaudi::Property<bool> m_mdt_adc_cut{this, "MDT_ADC_cut", true};
        /** value of adc cut (50) */
        Gaudi::Property<int> m_mdt_adc_min{this, "MDT_ADC_value", 50};
        /** use tdc cut (false) */
        Gaudi::Property<bool> m_mdt_tdc_cut{this, "MDT_TDC_cut", true};

        /** use rpc preprawdata (true) */
        Gaudi::Property<bool> m_use_rpc{this, "RPC", true};
        /** use tgc preprawdata (true) */
        Gaudi::Property<bool> m_use_tgc{this, "TGC", true};
        /** use csc preprawdata (true) */
        Gaudi::Property<bool> m_use_csc{this, "CSC", true};
        /** use mdt preprawdata (true) */
        Gaudi::Property<bool> m_use_mdt{this, "MDT", true};
        /** use weight for csc segments */
        double m_weight_csc_on_segment;

        /** reduce cpu for showers (true) */
        Gaudi::Property<bool> m_showerskip{this, "ShowerSkipping", true};
        /** percentage of occupancy to skip MDT chamber (0.3) */
        Gaudi::Property<double> m_showerskipperc{this, "ShowerSkipPercentage", 0.3};

        /** flag to output a root file to study the weights of hits */
        Gaudi::Property<bool> m_use_histos{this, "UseHistos", false};

        /** flag to print out a summary of what comes in and what comes out */
        Gaudi::Property<bool> m_summary{this, "DoSummary", false};

        /** flag to write out intermediate patterns */
        Gaudi::Property<bool> m_recordAllOutput{this, "RecordAll", false};

        /** storegate location for csc association map */
        Gaudi::Property<std::string> m_cscAssoOutputLocation{this, "PatCscSegAssMapOutputLocation", "MuonPatCscSegAssMap"};  // Not used

        /** pointer to the CSC segment combination collection */
        // const MuonSegmentCombinationCollection* m_csc_segments;

        /** histogram file for studies on weighting (only in use, when m_use_histos is
         * true) */
        std::unique_ptr<TFile> m_file{};
        /** all hits histograms for studies on weighting (only in use, when
         * m_use_histos is true) */
        std::unique_ptr<TH1> m_weighthistogram{};
        /** mdt histogram */
        std::unique_ptr<TH1> m_weighthistogrammdt{};
        /** rpc histogram */
        std::unique_ptr<TH1> m_weighthistogramrpc{};
        /** tgc histogram */
        std::unique_ptr<TH1> m_weighthistogramtgc{};
        /** csc histogram */
        std::unique_ptr<TH1> m_weighthistogramcsc{};

        SG::WriteHandleKey<MuonPrdPatternCollection> m_CosmicPhiPatternsKey{this, "CosmicPhiKey", "CosmicPhiPatterns"};
        SG::WriteHandleKey<MuonPrdPatternCollection> m_CosmicEtaPatternsKey{this, "CosmicEtaPatterns", "CosmicEtaPatterns"};
        SG::WriteHandleKey<MuonPrdPatternCollection> m_COMBINED_PATTERNSKey{this, "PATTERNS", "COMBINED_PATTERNS"};
    };

}  // namespace Muon

#endif  // MUONHOUGHPATTERNALGS_MUONHOUGHPATTERNALG_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H
#define TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace Muon {

    class MuonSegmentTruthAssociationAlg : public AthReentrantAlgorithm {
    public:
        // Constructor with parameters:
        MuonSegmentTruthAssociationAlg(const std::string &name, ISvcLocator *pSvcLocator);

        // Basic algorithm methods:
        virtual StatusCode initialize() override;
        virtual StatusCode execute(const EventContext &ctx) const override;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<Muon::IMuonTrackTruthTool> m_muonTrackTruthTool{this, "MuonTrackTruthTool",
                                                                   "Muon::MuonTrackTruthTool/MuonTrackTruthTool"};

        SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> m_muonTruthSegmentContainerName{
            this, "MuonTruthSegmentName", "MuonTruthSegments", "muon truth segment container name"};
        SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> m_muonSegmentCollectionName{this, "MuonSegmentLocation", "MuonSegments",
                                                                                        "muon segment container name"};

        SG::ReadHandleKey<McEventCollection> m_mcEventColl{this, "McEventCollectionKey", "TruthEvent", "McEventCollection"};
        SG::ReadHandleKeyArray<MuonSimDataCollection> m_muonSimData{
            this, "MuonSimDataNames", {"MDT_SDO", "RPC_SDO", "TGC_SDO", "sTGC_SDO", "MM_SDO"}, "Muon SDO maps"};
        SG::ReadHandleKey<CscSimDataCollection> m_cscSimData{this, "CSC_SDO_Container", "CSC_SDO", "CSC SDO"};
        SG::ReadHandleKey<TrackRecordCollection> m_trackRecord{this, "TrackRecord", "MuonEntryLayerFilter", "Track Record Collection"};

        Gaudi::Property<int> m_barcodeOffset{this, "BarcodeOffset", 1000000, "barcode offset for matching truth particles"};
    };

}  // namespace Muon

#endif  // TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H

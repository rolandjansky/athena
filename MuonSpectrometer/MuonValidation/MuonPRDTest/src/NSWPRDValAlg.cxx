/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// NSWValAlg inlcudes
#include "NSWPRDValAlg.h"

#include <MuonPRDTest/MuonPRDTestDict.h>

#include "EDM_object.h"

// Other NSW includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// Misc includes
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
using namespace MuonPRDTest;
NSWPRDValAlg::NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthHistogramAlgorithm(name, pSvcLocator) {}

StatusCode NSWPRDValAlg::initialize() {
    ATH_MSG_DEBUG("initialize()");
    // MuonDetectorManager from the conditions store
    ATH_CHECK(m_DetectorManagerKey.initialize(m_isData));
    m_tree.addBranch(std::make_unique<EventInfoBranch>(m_tree, !m_isData));

    // MuonDetectorManager from the Detector Store (to be used only at initialize)
    ATH_CHECK(detStore()->retrieve(m_muonDetMgrDS));

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(evtStore().retrieve());
    if (m_doTruth) { m_tree.addBranch(std::make_unique<TruthVariables>(m_tree, m_Truth_ContainerName, msgLevel())); }

    if (m_doMuEntry) {
        m_testers.emplace_back(
            std::make_unique<MuEntryVariables>(evtStore().get(), m_muonDetMgrDS, m_tree.tree(), m_MuEntry_ContainerName, msgLevel()));
    }

    if (m_doSTGCHit) { m_tree.addBranch(std::make_unique<sTGCSimHitVariables>(m_tree, m_NSWsTGC_ContainerName.value(), msgLevel())); }
    if (m_doSTGCDigit) {
        m_tree.addBranch(std::make_unique<sTgcDigitVariables>(m_tree, m_NSWsTGC_DigitContainerName.value(), msgLevel()));
        // Take SDO conainer
        m_tree.addBranch(std::make_unique<sTgcSDOVariables>(m_tree, m_NSWsTGC_SDOContainerName.value(), msgLevel()));
    }
    if (m_doSTGCFastDigit) { m_tree.addBranch(std::make_unique<sTgcSDOVariables>(m_tree, "sTGCfast_SDO", msgLevel())); }
    if (m_doSTGCRDO) { m_tree.addBranch(std::make_unique<sTGCRDOVariables>(m_tree, m_NSWsTGC_RDOContainerName.value(), msgLevel())); }
    if (m_doSTGCPRD) { m_tree.addBranch(std::make_unique<sTGCPRDVariables>(m_tree, m_NSWsTGC_PRDContainerName.value(), msgLevel())); }

    if (m_doMMHit) { m_tree.addBranch(std::make_unique<MMSimHitVariables>(m_tree, m_NSWMM_ContainerName.value(), msgLevel())); }
    if (m_doMMDigit) {
        m_tree.addBranch(std::make_unique<MMDigitVariables>(m_tree, m_NSWMM_DigitContainerName.value(), msgLevel()));
        // Take SDO conainer
        m_tree.addBranch(std::make_unique<MMSDOVariables>(m_tree, m_NSWMM_SDOContainerName.value(), msgLevel()));
    }
    if (m_doMMFastDigit) { m_tree.addBranch(std::make_unique<MMSDOVariables>(m_tree, "MMfast_SDO", msgLevel())); }
    if (m_doMMRDO) { m_tree.addBranch(std::make_unique<MMRDOVariables>(m_tree, m_NSWMM_RDOContainerName.value(), msgLevel())); }
    if (m_doMMPRD) { m_tree.addBranch(std::make_unique<MMPRDVariables>(m_tree, m_NSWMM_PRDContainerName.value(), msgLevel())); }

    if (m_doCSCHit) { m_tree.addBranch(std::make_unique<CSCSimHitVariables>(m_tree, m_CSC_SimContainerName.value(), msgLevel())); }
    if (m_doCSCSDO) { m_tree.addBranch(std::make_unique<CscSDOVariables>(m_tree, m_CSC_SDOContainerName.value(), msgLevel())); }
    if (m_doCSCDigit) { m_tree.addBranch(std::make_unique<CscDigitVariables>(m_tree, m_CSC_DigitContainerName.value(), msgLevel())); }
    if (m_doCSCRDO) { 
        ATH_CHECK(m_csc_decoder.retrieve());
        m_tree.addBranch(std::make_unique<CSCRDOVariables>(m_tree, m_CSC_RDOContainerName.value(), msgLevel(), &m_idHelperSvc->cscIdHelper(), m_csc_decoder.get())); 
    }
    if (m_doCSCPRD) { m_tree.addBranch(std::make_unique<CSCPRDVariables>(m_tree, m_CSC_PRDContainerName.value(), msgLevel())); }

    if (m_doMDTHit) { m_tree.addBranch(std::make_unique<MDTSimHitVariables>(m_tree, m_MDT_SimContainerName.value(), msgLevel())); }
    if (m_doMDTSDO) { m_tree.addBranch(std::make_unique<MdtSDOVariables>(m_tree, m_MDT_SDOContainerName.value(), msgLevel())); }
    if (m_doMDTDigit) { m_tree.addBranch(std::make_unique<MdtDigitVariables>(m_tree, m_MDT_DigitContainerName.value(), msgLevel())); }

    if (m_doRPCHit) { m_tree.addBranch(std::make_unique<RPCSimHitVariables>(m_tree, m_RPC_SimContainerName, msgLevel())); }
    if (m_doRPCSDO) { m_tree.addBranch(std::make_unique<RpcSDOVariables>(m_tree, m_RPC_SDOContainerName, msgLevel())); }
    if (m_doRPCDigit) { m_tree.addBranch(std::make_unique<RpcDigitVariables>(m_tree, m_RPC_DigitContainerName, msgLevel())); }

    if (m_doTGCHit) { m_tree.addBranch(std::make_unique<TGCSimHitVariables>(m_tree, m_TGC_SimContainerName.value(), msgLevel())); }
    if (m_doTGCSDO) { m_tree.addBranch(std::make_unique<TgcSDOVariables>(m_tree, m_TGC_SDOContainerName.value(), msgLevel())); }
    if (m_doTGCDigit) { m_tree.addBranch(std::make_unique<TgcDigitVariables>(m_tree, m_TGC_DigitContainerName.value(), msgLevel())); }
    if (m_doTGCRDO) {
        const ITGCcablingServerSvc* TgcCabGet = nullptr;
        ATH_CHECK(service("TGCcablingServerSvc", TgcCabGet, true));
        ATH_CHECK(TgcCabGet->giveCabling(m_tgcCabling));
        m_tree.addBranch(std::make_unique<TGCRDOVariables>(m_tree, m_TGC_RDOContainerName.value(), msgLevel(), m_tgcCabling));
    }
    if (m_doTGCPRD) { m_tree.addBranch(std::make_unique<TGCPRDVariables>(m_tree, m_TGC_PRDContainerName.value(), msgLevel())); }

    for (std::unique_ptr<ValAlgVariables>& tester : m_testers) { ATH_CHECK(tester->initializeVariables()); }

    ATH_CHECK(m_tree.init(histSvc()));

    return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::finalize() {
    ATH_MSG_DEBUG("NSWPRDValAlg:: Finalize + Matching");
    if (m_doNSWMatching) { ATH_CHECK(NSWMatchingAlg()); }
    ATH_CHECK(m_tree.write());
    return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::execute() {
    ATH_MSG_DEBUG("execute()");
    const EventContext& ctx = Gaudi::Hive::currentContext();

    // MuonDetectorManager from the detector store for MC
    const MuonGM::MuonDetectorManager* muonDetMgr = m_muonDetMgrDS;
    // MuonDetectorManager from the conditions store for data
    if (m_isData) {
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
        muonDetMgr = DetectorManagerHandle.cptr();
        if (!muonDetMgr) {
            ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
            return StatusCode::FAILURE;
        }
    }
    for (std::unique_ptr<ValAlgVariables>& tester : m_testers) {
        ATH_CHECK(tester->fillVariables(muonDetMgr));
    }

    if (!m_tree.fill(ctx)) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

/*****************************************************************************************************************************************
The rest of this file is the NSW matching algorithm. This can be (de)activated with the input variable doNSWMatchingAlg.
The matching algorithm will check each conversion in the Hit->PRD digitization chain.
For each conversion, the algorithm will try to match the two objects.
The input variable setMaxStripDistance (default value = 3) give the maximum strip distance two objects can have, while still be considered a
match The input variable doNSWMatchingMuonOnly can be set to true to only use events which contain exclusively muons
******************************************************************************************************************************************/

// First set up which object should be matched, given the input used to fill the NSW Ntuple
StatusCode NSWPRDValAlg::NSWMatchingAlg() {
    ATH_MSG_DEBUG("NSWMatchingAlg: building Data objects");

    // Use the EDM object, as defined in EDM_object.h, to build the hits, digits, SDO, RDO and PRD for both sTGC and MM
    EDM_object Hits_sTGC, Digits_sTGC, SDO_sTGC, RDO_sTGC, PRD_sTGC;
    EDM_object Hits_MM, Digits_MM, SDO_MM, RDO_MM, PRD_MM;

    Hits_sTGC.setName("Hits", "sTGC");
    Digits_sTGC.setName("Digits", "sTGC");
    SDO_sTGC.setName("SDO", "sTGC");
    RDO_sTGC.setName("RDO", "sTGC");
    PRD_sTGC.setName("PRD", "sTGC");

    Hits_MM.setName("Hits", "MM");
    Digits_MM.setName("Digits", "MM");
    SDO_MM.setName("SDO", "MM");
    RDO_MM.setName("RDO", "MM");
    PRD_MM.setName("PRD", "MM");

    // Match the EDM objects with the variables in the NSW Validation Ntuple
    TString branch_name;
    TObjArray* brlst = m_tree->GetListOfBranches();
    for (TBranch* branch = (TBranch*)brlst->First(); branch; branch = (TBranch*)brlst->After(branch)) {
        branch_name = branch->GetName();
        ATH_MSG_VERBOSE("About to check branch: " << branch_name);

        ATH_CHECK(setDataAdress(Hits_sTGC, branch_name));
        ATH_CHECK(setDataAdress(Digits_sTGC, branch_name));
        ATH_CHECK(setDataAdress(SDO_sTGC, branch_name));
        ATH_CHECK(setDataAdress(RDO_sTGC, branch_name));
        ATH_CHECK(setDataAdress(PRD_sTGC, branch_name));

        ATH_CHECK(setDataAdress(Hits_MM, branch_name));
        ATH_CHECK(setDataAdress(Digits_MM, branch_name));
        ATH_CHECK(setDataAdress(SDO_MM, branch_name));
        ATH_CHECK(setDataAdress(RDO_MM, branch_name));
        ATH_CHECK(setDataAdress(PRD_MM, branch_name));
    }

    // Prepare the output file
    std::ofstream efficiencies;
    efficiencies.open("NSWMatchingAlg_efficiencies.txt");
    efficiencies << "NSW Matching algorithm, efficiencies of conversion from and to various EDM objects" << std::endl;
    efficiencies << "Settings:\n" << m_doNSWMatchingMuon << std::endl;
    efficiencies << " 'Maximum Strip Distance':" << m_maxStripDiff << std::endl;
    efficiencies.close();

    // sTGC matching
    if (m_doSTGCHit && m_doSTGCDigit) {
        ATH_CHECK(NSWMatchingAlg(Hits_sTGC, Digits_sTGC));
        ATH_CHECK(NSWMatchingAlg(Hits_sTGC, SDO_sTGC));
    }
    if (m_doSTGCDigit && m_doSTGCRDO) {
        ATH_CHECK(NSWMatchingAlg(Digits_sTGC, RDO_sTGC));
        ATH_CHECK(NSWMatchingAlg(SDO_sTGC, RDO_sTGC));
    }
    if (m_doSTGCRDO && m_doSTGCPRD) { ATH_CHECK(NSWMatchingAlg(RDO_sTGC, PRD_sTGC)); }

    // sTGC fast digitization
    if (m_doSTGCHit && m_doSTGCFastDigit) {
        ATH_CHECK(NSWMatchingAlg(Hits_sTGC, PRD_sTGC));
        ATH_CHECK(NSWMatchingAlg(Hits_sTGC, SDO_sTGC));
        ATH_CHECK(NSWMatchingAlg(SDO_sTGC, PRD_sTGC));
    }

    // MM matching
    if (m_doMMHit && m_doMMDigit) {
        ATH_CHECK(NSWMatchingAlg(Hits_MM, Digits_MM));
        ATH_CHECK(NSWMatchingAlg(Hits_MM, SDO_MM));
    }
    if (m_doMMDigit && m_doMMRDO) {
        ATH_CHECK(NSWMatchingAlg(Digits_MM, RDO_MM));
        ATH_CHECK(NSWMatchingAlg(SDO_MM, RDO_MM));
    }
    if (m_doMMRDO && m_doMMPRD) { ATH_CHECK(NSWMatchingAlg(RDO_MM, PRD_MM)); }

    //  MM fast digitization
    if (m_doMMHit && m_doMMFastDigit) {
        ATH_CHECK(NSWMatchingAlg(Hits_MM, PRD_MM));
        ATH_CHECK(NSWMatchingAlg(Hits_MM, SDO_MM));
        ATH_CHECK(NSWMatchingAlg(SDO_MM, PRD_MM));
    }

    return StatusCode::SUCCESS;
}

// This part of the matching algortihm does the actual comparison given two EDM obects
StatusCode NSWPRDValAlg::NSWMatchingAlg(EDM_object data0, EDM_object data1) {
    if (data0.getDetector() != data1.getDetector()) {
        ATH_MSG_ERROR("Matching " << data0.getDetector() << " data with " << data1.getDetector()
                                  << " data. This is not implemented in this algorithm");
    }

    ATH_MSG_DEBUG("NSWMatchingAlg: Start matching " << data0.getName() << " and " << data1.getName() << " for " << data0.getDetector());
    data0.setMatchedwith(data1.getName());
    data1.setMatchedwith(data0.getName());

    // Prepare Muon only check
    std::vector<int>* TruthParticle_Pdg;
    if (m_doNSWMatchingMuon) {
        TruthParticle_Pdg = nullptr;
        m_tree->SetBranchAddress("TruthParticle_Pdg", &TruthParticle_Pdg);
    }

    Long64_t nEntries = m_tree->GetEntriesFast();
    for (Long64_t i_entry = 0; i_entry < nEntries; ++i_entry) {
        // event numbering starts at 1
        ATH_MSG_DEBUG("Now checking event number " << i_entry + 1);
        m_tree->GetEntry(i_entry);

        if (data0.empty()) {
            ATH_MSG_WARNING("No " << data0.getDetector() << data0.getName() << " found in event " << i_entry + 1);
            continue;
        }
        if (data1.empty()) {
            ATH_MSG_WARNING("No " << data1.getDetector() << data1.getName() << " found in event " << i_entry + 1);
            continue;
        }

        ATH_MSG_DEBUG("Number of " << data0.getDetector() << data0.getName() << ": " << data0.size() << ", number of "
                                   << data1.getDetector() << data1.getName() << ": " << data1.size());

        // only muon events:
        if (m_doNSWMatchingMuon) {
            bool allMu = true;
            int mu_pdg = 13;
            for (int pdg : *TruthParticle_Pdg) { allMu &= (abs(pdg) == mu_pdg); }
            if (!allMu) {
                ATH_MSG_VERBOSE("Skipping event, because doNSWMatchingMuonOnly is true and there non muon particles");
                continue;
            }
        }

        // Reset variables to have a clean sheet
        data0.init_matching();
        data1.init_matching();

        // Actual Matching
        for (uint i = 0; i < data0.size(); ++i) {
            int nMatch = 0;
            for (uint j = 0; j < data1.size(); ++j) {
                if (data0.identifierMatch(data0, data1, i, j)) {
                    nMatch++;
                    data0.update_match(i, data1.m_channel->at(j));
                    data1.update_match(j, data0.m_channel->at(i));
                }
            }
            ATH_MSG_VERBOSE("Total Number of matches found: " << nMatch << " " << data1.getName() << " for a single " << data0.getName());
            static bool warningPrinted = false;
            if (nMatch == 0) {
                if (m_noMatchWarning) {
                    if (!warningPrinted) {
                        ATH_MSG_WARNING(
                            "No match found! Will now disable this kind of WARNING but please be aware that you are running with "
                            "suppressNoMatchWarning set to true!");
                        warningPrinted = true;
                    }
                } else {
                    ATH_MSG_DEBUG("No match found!");
                }
            }
        }
        if (msgLevel() <= MSG::DEBUG) {
            ATH_MSG_DEBUG("Full info for " << data0.getName() << data0.getDetector());
            data0.printInfo();
            ATH_MSG_DEBUG("Full info for " << data1.getName() << data1.getDetector());
            data1.printInfo();
        }

        data0.update_efficiency(m_maxStripDiff);
        data1.update_efficiency(m_maxStripDiff);

        // The data0 vs data1 matching will now be overwritten
        data0.clearVars();
        data1.clearVars();
    }

    // Write result to file
    std::ofstream efficiencies;
    efficiencies.open("NSWMatchingAlg_efficiencies.txt", std::ofstream::app);
    data0.printEfficiency(efficiencies);
    data1.printEfficiency(efficiencies);
    efficiencies.close();

    return StatusCode::SUCCESS;
}

// This function couples the branch of the NSW validation Ntuple with the EDM object.
// In case the brach does not exist, or is completely empty, the object will remain empty and the code will skip the matching entirly,
// giving a ATH_WARNING in the process
StatusCode NSWPRDValAlg::setDataAdress(EDM_object& oData, const TString& branch_name) {
    bool setBranch = false;
    if (!(branch_name.Contains(oData.getName()) && branch_name.Contains(oData.getDetector()))) { return StatusCode::SUCCESS; }
    // For sim hits select the offline identifiers, rather than the sim identifiers
    if (branch_name.Contains("_sim_")) { return StatusCode::SUCCESS; }
    if (branch_name.EndsWith("stationName")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_stationName);
        setBranch = true;
    }
    if (branch_name.EndsWith("stationEta")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_stationEta);
        setBranch = true;
    }
    if (branch_name.EndsWith("stationPhi")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_stationPhi);
        setBranch = true;
    }
    if (branch_name.EndsWith("multiplet")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_multiplet);
        setBranch = true;
    }
    if (branch_name.EndsWith("gas_gap")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_gas_gap);
        setBranch = true;
    }
    if (branch_name.EndsWith("channel") && !branch_name.Contains("rdos")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_channel);
        setBranch = true;
    }
    if (branch_name.EndsWith("channel_type")) {
        m_tree->SetBranchAddress(branch_name, &oData.m_channel_type);
        setBranch = true;
    }
    if (setBranch) { ATH_MSG_DEBUG("Set data adress of branch " << branch_name); }

    return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGMNtupleWriter.h"

#include <TTree.h>

#include <fstream>
#include <sstream>

#include "GaudiKernel/ITHistSvc.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

namespace MuonGM {

    MuonGMNtupleWriter::MuonGMNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator) :
        AthAlgorithm(name, pSvcLocator), m_tree(nullptr), m_nevents(0) {
        declareProperty("NtupleFileName", m_ntupleFileName = "MuonGMNtuple");
        declareProperty("NtupleDirectoryName", m_ntupleDirName = "MuonGM");
        declareProperty("NtupleTreeName", m_ntupleTreeName = "data");
        declareProperty("OutputTextFile", m_outputToTextFile = false);
    }

    StatusCode MuonGMNtupleWriter::initialize() {
        // MuonDetectorManager from the conditions store
        ATH_CHECK(m_DetectorManagerKey.initialize());

        std::string streamName = "/" + m_ntupleFileName + "/" + m_ntupleDirName;
        std::string treeName = m_ntupleTreeName;

        std::string treePath = streamName + "/" + treeName;

        m_tree = new TTree(treeName.c_str(), "MdtTubeSurface");

        ITHistSvc* rootsvc = nullptr;
        if (service("THistSvc", rootsvc, true).isFailure()) {
            ATH_MSG_ERROR("Unable to locate the MDTPRDValAlg Histogram service");
            delete m_tree;
            return StatusCode::FAILURE;
        }
        if (rootsvc->regTree(treePath, m_tree).isFailure()) {
            ATH_MSG_ERROR("MuonGMNtupleWriter:: Unable to register TTreeTuple : " << treePath);
            delete m_tree;
            return StatusCode::FAILURE;
        }

        m_mdtSurfaceBranch.initForWrite(*m_tree, "mdt_");

        return AthAlgorithm::initialize();
    }

    StatusCode MuonGMNtupleWriter::execute() {
        ++m_nevents;
        if (m_nevents == 1) fillNtuple();

        return StatusCode::SUCCESS;
    }

    void MuonGMNtupleWriter::fillNtuple() {
        // MuonDetectorManager from the conditions store
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
        const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr();
        if (!MuonDetMgr) {
            ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
            return;
        }

        std::unique_ptr<std::ofstream> fout{};
        if (m_outputToTextFile) {
            std::string gVersion = MuonDetMgr->geometryVersion();
            std::string fileName = "muon_current_" + gVersion;
            fout.reset(new std::ofstream(fileName));
        }

        unsigned int nmdt{0}, nrpc{0}, ntgc{0}, ncsc{0};
        for (unsigned int mdt_hash = 0; mdt_hash < MuonDetectorManager::MdtDetElMaxHash; ++mdt_hash) {
            IdentifierHash id_hash{mdt_hash};
            const MdtReadoutElement* detEl = MuonDetMgr->getMdtReadoutElement(id_hash);
            if (!detEl) continue;
            ++nmdt;
            if (fout) {
                (*fout) << " New MDT ReadoutElement " << detEl->identify().get_compact() << " "
                        << m_idHelperSvc->toString(detEl->identify()) << " nlayers " << detEl->getNLayers() << " ntubes "
                        << detEl->getNtubesperlayer() << std::endl
                        << Amg::toString(detEl->transform(), 6) << std::endl;
            }

            for (int tl = 0; tl < detEl->getNLayers(); ++tl) {
                for (int t = 0; t < detEl->getNtubesperlayer(); ++t) {
                    const Trk::Surface& surf = detEl->surface(tl + 1, t + 1);
                    m_mdtSurfaceBranch.fill(surf);
                    m_tree->Fill();
                    if (fout) {
                        (*fout) << " New tube: layer " << tl << " tube " << t << std::endl
                                << Amg::toString(surf.transform(), 6) << std::endl;
                    }
                }
            }
        }

        for (int i1 = 0; i1 < MuonDetectorManager::NRpcStatType; ++i1) {
            for (int i2 = 0; i2 < MuonDetectorManager::NRpcStatEta; ++i2) {
                for (int i3 = 0; i3 < MuonDetectorManager::NRpcStatPhi; ++i3) {
                    for (int i4 = 0; i4 < MuonDetectorManager::NDoubletR; ++i4) {
                        for (int i5 = 0; i5 < MuonDetectorManager::NDoubletZ; ++i5) {
                            int stationName = MuonDetMgr->rpcStationName(i1);
                            bool isValid = false;
                            Identifier id =
                                MuonDetMgr->rpcIdHelper()->channelID(stationName, i2, i3, i4, i5, 1, 1, 1, 1, true,
                                                                     &isValid);  // last 5 arguments are: int doubletPhi, int gasGap, int
                                                                                 // measuresPhi, int strip, bool check, bool* isValid
                            if (!isValid) continue;
                            const RpcReadoutElement* detEl = MuonDetMgr->getRpcReadoutElement(id);
                            if (!detEl) continue;
                            ++nrpc;

                            if (fout) {
                                (*fout) << " New RPC ReadoutElement " << detEl->identify().get_compact() << " "
                                        << MuonDetMgr->rpcIdHelper()->print_to_string(detEl->identify()) << "  NphiStripPanels "
                                        << detEl->NphiStripPanels() << std::endl
                                        << Amg::toString(detEl->transform(), 6) << std::endl;
                            }

                            for (int dbPhi = 1; dbPhi <= detEl->NphiStripPanels(); ++dbPhi) {
                                for (int gp = 1; gp <= 2; ++gp) {
                                    for (int mp = 0; mp < 2; ++mp) {
                                        const Trk::Surface& surf = detEl->surface(detEl->surfaceHash(dbPhi, gp, mp));
                                        if (fout) {
                                            (*fout) << " New layer " << dbPhi << " gp " << gp << " measPhi " << mp << std::endl
                                                    << Amg::toString(surf.transform(), 6) << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for (unsigned int hash = 0; hash < MuonDetectorManager::TgcRElMaxHash; ++hash) {
            IdentifierHash id_hash(hash);
            const TgcReadoutElement* detEl = MuonDetMgr->getTgcReadoutElement(id_hash);
            if (!detEl) continue;
            ++ntgc;

            if (fout) {
                (*fout) << " New TGC ReadoutElement " << detEl->identify().get_compact() << " "
                        << MuonDetMgr->tgcIdHelper()->print_to_string(detEl->identify()) << std::endl
                        << Amg::toString(detEl->transform(), 6) << std::endl;
            }

            for (int gp = 0; gp < 2; ++gp) {
                for (int mp = 0; mp < 2; ++mp) {
                    const Trk::Surface& surf = detEl->surface(2 * gp + mp);
                    if (fout) {
                        (*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
                                << Amg::toString(surf.transform(), 6) << std::endl;
                    }
                }
            }
        }

        for (unsigned int hash = 0; hash < MuonDetectorManager::CscRElMaxHash; ++hash) {
            IdentifierHash id_hash(hash);
            const CscReadoutElement* detEl = MuonDetMgr->getCscReadoutElement(id_hash);
            if (!detEl) continue;
            ++ncsc;

            if (fout) {
                (*fout) << " New CSC ReadoutElement " << detEl->identify().get_compact() << " "
                        << MuonDetMgr->cscIdHelper()->print_to_string(detEl->identify()) << std::endl
                        << Amg::toString(detEl->transform(), 6) << std::endl;
            }

            for (int gp = 0; gp < 4; ++gp) {
                for (int mp = 0; mp < 2; ++mp) {
                    const Trk::Surface& surf = detEl->surface(2 * gp + mp);
                    if (fout) {
                        (*fout) << " New layer: gp " << gp << " measPhi " << mp << std::endl
                                << Amg::toString(surf.transform(), 6) << std::endl;
                    }
                }
            }
        }

        if (fout) {
            (*fout) << " NMDT " << nmdt << " NRPC " << nrpc << " NTGC " << ntgc << " NCSC " << ncsc << std::endl;
            fout->close();
        }
    }

}  // namespace MuonGM

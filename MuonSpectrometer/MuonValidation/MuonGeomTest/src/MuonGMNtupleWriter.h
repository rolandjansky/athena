/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGM_MUONGMNTUPLEWRITER_H
#define MUONGM_MUONGMNTUPLEWRITER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkValidationUtils/SurfaceNtupleBranch.h"
class TTree;

namespace MuonGM {

    /** @class MuonGMNtupleWriter

        Basic MuonGM Ntuple dumper
    */

    class MuonGMNtupleWriter : public AthAlgorithm {
    public:
        MuonGMNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);

        virtual ~MuonGMNtupleWriter() = default;

        StatusCode initialize() override;
        StatusCode execute() override;

    protected:
        void fillNtuple();

        std::string m_ntupleTreeName; /** jobOption: Ntuple tree name*/
        std::string m_ntupleFileName; /** jobOption: Ntuple file name*/
        std::string m_ntupleDirName;  /** jobOption: Ntuple directory name*/
        TTree* m_tree;
        int m_nevents;

        Trk::SurfaceNtupleBranch m_mdtSurfaceBranch;

        // MuonDetectorManager from the conditions store
        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        bool m_outputToTextFile;
    };

}  // namespace MuonGM

#endif

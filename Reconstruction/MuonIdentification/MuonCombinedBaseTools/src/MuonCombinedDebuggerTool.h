/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDDEBUGGERTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDDEBUGGERTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuonMatchQuality.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "TrkToolInterfaces/ITruthToTrack.h"

class TTree;

namespace MuonCombined {

    class MuonCombinedDebuggerTool : public AthAlgTool  // the MuonCombinedDebuggerTool can only be used when running with one thread
    {
    public:
        MuonCombinedDebuggerTool(const std::string& type, const std::string& name, const IInterface* parent);
        ~MuonCombinedDebuggerTool() = default;

        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_MuonCombinedDebuggerTool("MuonCombined::MuonCombinedDebuggerTool", 1, 0);
            return IID_MuonCombinedDebuggerTool;
        }

        StatusCode initialize() override;

        void fillBranches(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates);

        void fillMsIdBranches(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates);

        void fillIdBranches(const InDetCandidateCollection& inDetCandidates);

        void fillMsBranches(const MuonCandidateCollection& muonCandidates);

    private:
        void bookBranches();

        ToolHandle<Rec::IMuonMatchQuality> m_matchQuality{this, "MuonMatchQuality", "Rec::MuonMatchQuality/MuonMatchQuality"};
        ToolHandle<Trk::ITruthToTrack> m_truthToTrack{this, "TruthToTrack", "Trk::TruthToTrack/TruthToTrack"};

        ServiceHandle<ITHistSvc> m_histSvc;

        TTree* m_recoTree;
        int m_eventNumber{};

        std::vector<int> m_mstrack_has_truth;
        std::vector<int> m_mstrack_has_truth_par;
        std::vector<int> m_mstrack_truth_id;
        std::vector<int> m_mstrack_truth_barcode;
        std::vector<double> m_mstrack_truth_sur_x;
        std::vector<double> m_mstrack_truth_sur_y;
        std::vector<double> m_mstrack_truth_sur_z;
        std::vector<double> m_mstrack_truth_d0;
        std::vector<double> m_mstrack_truth_z0;
        std::vector<double> m_mstrack_truth_phi0;
        std::vector<double> m_mstrack_truth_theta;
        std::vector<double> m_mstrack_truth_qOverP;

        std::vector<double> m_mstrack_sur_x;
        std::vector<double> m_mstrack_sur_y;
        std::vector<double> m_mstrack_sur_z;
        std::vector<double> m_mstrack_d0;
        std::vector<double> m_mstrack_z0;
        std::vector<double> m_mstrack_phi0;
        std::vector<double> m_mstrack_theta;
        std::vector<double> m_mstrack_qOverP;
        std::vector<double> m_mstrack_cov_d0;
        std::vector<double> m_mstrack_cov_z0;
        std::vector<double> m_mstrack_cov_phi0;
        std::vector<double> m_mstrack_cov_theta;
        std::vector<double> m_mstrack_cov_qOverP;

        std::vector<int> m_mstrack_has_sa;
        std::vector<double> m_mstrack_sa_sur_x;
        std::vector<double> m_mstrack_sa_sur_y;
        std::vector<double> m_mstrack_sa_sur_z;
        std::vector<double> m_mstrack_sa_d0;
        std::vector<double> m_mstrack_sa_z0;
        std::vector<double> m_mstrack_sa_phi0;
        std::vector<double> m_mstrack_sa_theta;
        std::vector<double> m_mstrack_sa_qOverP;
        std::vector<double> m_mstrack_sa_cov_d0;
        std::vector<double> m_mstrack_sa_cov_z0;
        std::vector<double> m_mstrack_sa_cov_phi0;
        std::vector<double> m_mstrack_sa_cov_theta;
        std::vector<double> m_mstrack_sa_cov_qOverP;

        std::vector<int> m_idtrack_has_truth;
        std::vector<int> m_idtrack_has_truth_par;
        std::vector<int> m_idtrack_truth_id;
        std::vector<int> m_idtrack_truth_barcode;
        std::vector<double> m_idtrack_truth_sur_x;
        std::vector<double> m_idtrack_truth_sur_y;
        std::vector<double> m_idtrack_truth_sur_z;
        std::vector<double> m_idtrack_truth_d0;
        std::vector<double> m_idtrack_truth_z0;
        std::vector<double> m_idtrack_truth_phi0;
        std::vector<double> m_idtrack_truth_theta;
        std::vector<double> m_idtrack_truth_qOverP;

        std::vector<double> m_idtrack_sur_x;
        std::vector<double> m_idtrack_sur_y;
        std::vector<double> m_idtrack_sur_z;
        std::vector<double> m_idtrack_d0;
        std::vector<double> m_idtrack_z0;
        std::vector<double> m_idtrack_phi0;
        std::vector<double> m_idtrack_theta;
        std::vector<double> m_idtrack_qOverP;
        std::vector<double> m_idtrack_cov_d0;
        std::vector<double> m_idtrack_cov_z0;
        std::vector<double> m_idtrack_cov_phi0;
        std::vector<double> m_idtrack_cov_theta;
        std::vector<double> m_idtrack_cov_qOverP;

        std::vector<std::vector<double> > m_ms_id_ochi2;
        std::vector<std::vector<int> > m_ms_id_ondf;
        std::vector<std::vector<double> > m_ms_id_oprob;
        std::vector<std::vector<double> > m_ms_id_ichi2;
        std::vector<std::vector<int> > m_ms_id_indf;
        std::vector<std::vector<double> > m_ms_id_iprob;
    };

}  // namespace MuonCombined

#endif

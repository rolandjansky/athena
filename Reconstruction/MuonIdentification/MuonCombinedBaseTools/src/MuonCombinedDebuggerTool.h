/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDDEBUGGERTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDDEBUGGERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "MuidInterfaces/IMuonMatchQuality.h"

#include <string>
#include <vector>

class TTree;

namespace MuonCombined {

  static const InterfaceID IID_MuonCombinedDebuggerTool("MuonCombined::MuonCombinedDebuggerTool", 1, 0);

  class ATLAS_NOT_THREAD_SAFE MuonCombinedDebuggerTool: public AthAlgTool // the MuonCombinedDebuggerTool can only be used when running with one thread
  {
    
  public:
    MuonCombinedDebuggerTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonCombinedDebuggerTool()=default;

    static const InterfaceID& interfaceID() {return IID_MuonCombinedDebuggerTool;}
  
    StatusCode initialize();

    void bookBranches();

    void fillBranches(const MuonCandidateCollection& muonCandidates, 
		      const InDetCandidateCollection& inDetCandidates) const;
    
    void fillMsIdBranches(const MuonCandidateCollection& muonCandidates, 
			  const InDetCandidateCollection& inDetCandidates) const;

    void fillIdBranches(const InDetCandidateCollection& inDetCandidates) const;

    void fillMsBranches(const MuonCandidateCollection& muonCandidates) const;

  private:

    ToolHandle<Rec::IMuonMatchQuality> m_matchQuality {this, "MuonMatchQuality", "Rec::MuonMatchQuality/MuonMatchQuality"};
    ToolHandle<Trk::ITruthToTrack> m_truthToTrack {this, "TruthToTrack", "Trk::TruthToTrack/TruthToTrack"};

    ServiceHandle<ITHistSvc> m_histSvc;

    TTree* m_recoTree;
    mutable int m_eventNumber;

    mutable std::vector<int>    m_mstrack_has_truth;
    mutable std::vector<int>    m_mstrack_has_truth_par;
    mutable std::vector<int>    m_mstrack_truth_id;
    mutable std::vector<int>    m_mstrack_truth_barcode;
    mutable std::vector<double> m_mstrack_truth_sur_x;
    mutable std::vector<double> m_mstrack_truth_sur_y;
    mutable std::vector<double> m_mstrack_truth_sur_z;
    mutable std::vector<double> m_mstrack_truth_d0;
    mutable std::vector<double> m_mstrack_truth_z0;
    mutable std::vector<double> m_mstrack_truth_phi0;
    mutable std::vector<double> m_mstrack_truth_theta;
    mutable std::vector<double> m_mstrack_truth_qOverP;

    mutable std::vector<double> m_mstrack_sur_x;
    mutable std::vector<double> m_mstrack_sur_y;
    mutable std::vector<double> m_mstrack_sur_z;
    mutable std::vector<double> m_mstrack_d0;
    mutable std::vector<double> m_mstrack_z0;
    mutable std::vector<double> m_mstrack_phi0;
    mutable std::vector<double> m_mstrack_theta;
    mutable std::vector<double> m_mstrack_qOverP;
    mutable std::vector<double> m_mstrack_cov_d0;
    mutable std::vector<double> m_mstrack_cov_z0;
    mutable std::vector<double> m_mstrack_cov_phi0;
    mutable std::vector<double> m_mstrack_cov_theta;
    mutable std::vector<double> m_mstrack_cov_qOverP;

    mutable std::vector<int>    m_mstrack_has_sa;
    mutable std::vector<double> m_mstrack_sa_sur_x;
    mutable std::vector<double> m_mstrack_sa_sur_y;
    mutable std::vector<double> m_mstrack_sa_sur_z;
    mutable std::vector<double> m_mstrack_sa_d0;
    mutable std::vector<double> m_mstrack_sa_z0;
    mutable std::vector<double> m_mstrack_sa_phi0;
    mutable std::vector<double> m_mstrack_sa_theta;
    mutable std::vector<double> m_mstrack_sa_qOverP;
    mutable std::vector<double> m_mstrack_sa_cov_d0;
    mutable std::vector<double> m_mstrack_sa_cov_z0;
    mutable std::vector<double> m_mstrack_sa_cov_phi0;
    mutable std::vector<double> m_mstrack_sa_cov_theta;
    mutable std::vector<double> m_mstrack_sa_cov_qOverP;

    mutable std::vector<int>    m_idtrack_has_truth;
    mutable std::vector<int>    m_idtrack_has_truth_par;
    mutable std::vector<int>    m_idtrack_truth_id;
    mutable std::vector<int>    m_idtrack_truth_barcode;
    mutable std::vector<double> m_idtrack_truth_sur_x;
    mutable std::vector<double> m_idtrack_truth_sur_y;
    mutable std::vector<double> m_idtrack_truth_sur_z;
    mutable std::vector<double> m_idtrack_truth_d0;
    mutable std::vector<double> m_idtrack_truth_z0;
    mutable std::vector<double> m_idtrack_truth_phi0;
    mutable std::vector<double> m_idtrack_truth_theta;
    mutable std::vector<double> m_idtrack_truth_qOverP;

    mutable std::vector<double> m_idtrack_sur_x;
    mutable std::vector<double> m_idtrack_sur_y;
    mutable std::vector<double> m_idtrack_sur_z;
    mutable std::vector<double> m_idtrack_d0;
    mutable std::vector<double> m_idtrack_z0;
    mutable std::vector<double> m_idtrack_phi0;
    mutable std::vector<double> m_idtrack_theta;
    mutable std::vector<double> m_idtrack_qOverP;
    mutable std::vector<double> m_idtrack_cov_d0;
    mutable std::vector<double> m_idtrack_cov_z0;
    mutable std::vector<double> m_idtrack_cov_phi0;
    mutable std::vector<double> m_idtrack_cov_theta;
    mutable std::vector<double> m_idtrack_cov_qOverP;

    mutable std::vector<std::vector<double> > m_ms_id_ochi2;
    mutable std::vector<std::vector<int> >    m_ms_id_ondf;
    mutable std::vector<std::vector<double> > m_ms_id_oprob;
    mutable std::vector<std::vector<double> > m_ms_id_ichi2;
    mutable std::vector<std::vector<int> >    m_ms_id_indf;
    mutable std::vector<std::vector<double> > m_ms_id_iprob;
    
  };

}	// end of namespace

#endif



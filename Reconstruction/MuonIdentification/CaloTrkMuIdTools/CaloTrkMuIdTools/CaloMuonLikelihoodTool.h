/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H
#define CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

class TH1F;

/** @class CaloMuonLikelihoodTool

    Compares track energy deposition ratios found by CaloTopoClusters to
    single muon and single pion distributions to build a likelihood ratio discriminant.

    @author Luis.Flores.Castillo@cern.ch
*/
class CaloMuonLikelihoodTool : public AthAlgTool, virtual public ICaloMuonLikelihoodTool {
public:
    CaloMuonLikelihoodTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~CaloMuonLikelihoodTool() = default;

    virtual StatusCode initialize();

    double getLHR(const xAOD::TrackParticle* trk, const xAOD::CaloClusterContainer* ClusCollection = nullptr,
                  const double dR_CUT = 0.3) const;
    double getLHR(const xAOD::CaloClusterContainer* ClusCollection, const double eta_trk, const double p_trk, const double eta_trkAtCalo,
                  const double phi_trkAtCalo, const double dR_CUT = 0.3) const;

private:
    StatusCode retrieveHistograms();

    std::unique_ptr<const TH1F> m_TH1F_sig[9][11]{};
    std::unique_ptr<const TH1F> m_TH1F_bkg[9][11]{};
    std::string m_TH1F_key[9][11]{};
    int m_numKeys[9]{};
    mutable std::atomic_int m_cnt_warn{0};

    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool{this, "ParticleCaloExtensionTool", ""};
    Gaudi::Property<std::string> m_calibRelease{this, "CalibRelease", "CaloTrkMuIdTools/cutBased_release21"};

    std::vector<std::string> m_fileNames;
};

#endif

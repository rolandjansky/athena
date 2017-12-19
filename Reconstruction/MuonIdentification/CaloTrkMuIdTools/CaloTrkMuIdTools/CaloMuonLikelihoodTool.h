/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H
#define CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H

#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"
#include "ICaloTrkMuIdTools/ITrackEnergyInCaloTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloClusterContainer.h"


#include <string>

class TFile;
class TH1F;

/** @class CaloMuonLikelihoodTool
        
    Compares track energy deposition ratios found by CaloTopoClusters to
    single muon and single pion distributions to build a likelihood ratio discriminant.

    @author Luis.Flores.Castillo@cern.ch
*/
class CaloMuonLikelihoodTool : public AthAlgTool, virtual public ICaloMuonLikelihoodTool {
public:
  CaloMuonLikelihoodTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CaloMuonLikelihoodTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  double getLHR(const Trk::TrackParameters* trkpar, const xAOD::CaloClusterContainer* ClusCollection=nullptr, const double dR_CUT=0.3) const;
  double getLHR(const xAOD::CaloClusterContainer* ClusCollection, const double eta_trk, const double p_trk, const double eta_trkAtCalo, const double phi_trkAtCalo, const double dR_CUT=0.3) const;

private:
  StatusCode        retrieveHistograms();

  const TH1F*       m_TH1F_sig[9][11];
  const TH1F*       m_TH1F_bkg[9][11];
  std::string       m_TH1F_key[9][11];
  int               m_numKeys[9];
  mutable int       m_cnt_warn;

  std::vector<std::string>  m_fileNames;

  ToolHandle<ITrackEnergyInCaloTool>  m_trkEnergyInCalo;
};

#endif

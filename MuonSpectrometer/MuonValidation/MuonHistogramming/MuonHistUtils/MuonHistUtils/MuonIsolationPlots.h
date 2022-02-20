/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONISOLATIONPLOTS_H
#define MUONHISTUTILS_MUONISOLATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"

namespace Muon{

#ifndef XAOD_ANALYSIS
class IsoCorrPlots:public PlotBase {
 public:
      IsoCorrPlots(PlotBase* pParent, const std::string& sDir, std::string sCorrType);

      void fill(const xAOD::Muon& muon,
		const xAOD::Iso::IsolationType &isoType_cone20,
		const xAOD::Iso::IsolationType &isoType_cone30,
		const xAOD::Iso::IsolationType &isoType_cone40,
		const xAOD::Iso::IsolationFlavour& flavour,
		const xAOD::Iso::IsolationCaloCorrection &isoCorrType,
		const xAOD::Iso::IsolationCorrectionParameter& isoCorrParam,
		float weight=1.0);
      void fill(float fIso20, float fIso30, float fIso40, float fPt, float fIsoCorr, float weight=1.0);
      std::string m_sCorrType;
      
      TH1* isocorr{nullptr};
      TH1* isocorr_relPt{nullptr};
      TH1* isocorr_relIsocone20{nullptr};
      TH1* isocorr_relIsocone30{nullptr};
      TH1* isocorr_relIsocone40{nullptr};
      
 private:
      void initializePlots();
      
};
#endif // not XAOD_ANALYSIS

class IsoPlots:public PlotBase {
    public:
      IsoPlots(PlotBase* pParent, const std::string& sDir, std::string sConeSize);
      void fill(const xAOD::Muon& muon, const xAOD::Iso::IsolationType &isoType, float weight=1.0);
      void fill(float fIso, float fPt, float weight=1.0);
      std::string m_sConeSize;

      TH1* cone;   
      TH1* conerel;

    private:
      void initializePlots();
    
};

 
class MuonIsolationPlots:public PlotBase {
   public:
     MuonIsolationPlots(PlotBase* pParent, const std::string& sDir);
     void fill(const xAOD::Muon& muon, float weight=1.0);
     
     IsoPlots m_oPtCone20;
     IsoPlots m_oPtCone30;
     IsoPlots m_oPtCone40;

     IsoPlots m_oTopoEtCone20;
     IsoPlots m_oTopoEtCone30;
     IsoPlots m_oTopoEtCone40;

     IsoPlots m_oNEFlowIso20;
     IsoPlots m_oNEFlowIso30;
     IsoPlots m_oNEFlowIso40;

     IsoPlots m_oPtVarCone20;
     IsoPlots m_oPtVarCone30;
     IsoPlots m_oPtVarCone40;

#ifndef XAOD_ANALYSIS
     IsoCorrPlots m_oTopoEtCone_coreCone;
     IsoCorrPlots m_oNEFlowIso_coreCone;
#endif // not XAOD_ANALYSIS
	  
};

}

#endif

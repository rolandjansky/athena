/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMMONEFFICIENCYTOOL_H
#define COMMONEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

// // Local include(s):
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/CorrectionCode.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>
#include <typeinfo>

#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TObject.h"
#include "TLorentzVector.h"
#include "TKey.h"
#include "TParameter.h"

namespace TauAnalysisTools {

  class CommonEfficiencyTool : public virtual ITauEfficiencyCorrectionsTool,
			       public TObject,
			       public asg::AsgTool
  {
    
    public:
    
    CommonEfficiencyTool(std::string sName);
    
    ~CommonEfficiencyTool();

    StatusCode configure();

    virtual StatusCode configure(PropertyMgr* pPropertyMgr);

    // CommonEfficiencyTool pure virtual public functionality
    //__________________________________________________________________________
    
    virtual TauAnalysisTools::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, double& dEfficiencyScaleFactor) = 0;
    virtual TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactor(xAOD::TauJet& xTau) = 0;
    virtual TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorStatUnc(const xAOD::TauJet& tau, double& efficiencyScaleFactorStatUnc) = 0;
    virtual TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorStatUnc(xAOD::TauJet& tau) = 0;
    virtual TauAnalysisTools::CorrectionCode getEfficiencyScaleFactorSysUnc(const xAOD::TauJet& tau, double& efficiencyScaleFactorSysUnc) = 0;
    virtual TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactorSysUnc(xAOD::TauJet& tau) = 0;

    // CommonEfficiencyTool public functions
    //____________________________________________________________________________
    /* TauAnalysisTools::CorrectionCode getEfficiencyScaleFactor(const xAOD::TauJet& tau, */
    /* 						   double& efficiencyScaleFactor, */
    /* 						   int iConfig1, */
    /* 						   int iConfig2, */
    /* 						   int iConfig3); */
    /* TauAnalysisTools::CorrectionCode applyEfficiencyScaleFactor(xAOD::TauJet& tau, */
    /* 						     int iConfig1, */
    /* 						     int iConfig2, */
    /* 						     int iConfig3); */
    
  protected:
   
    std::string ConvertProngToString(const int& iProngness);
    
    typedef std::map<std::string, TH1F*> SFMAP;
    SFMAP* m_mSF;
    int m_iSysDirection;
    
    template<class T>
      void ReadInputs(TFile* fFile, std::map<std::string, T>* mMap);
  };
} // namespace TauAnalysisTools

#endif // COMMONEFFICIENCYTOOL_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_BDTHELPER_H
#define TAURECTOOLS_BDTHELPER_H

#include "AsgTools/AsgMessaging.h"
#include "AsgMessaging/StatusCode.h"
#include "xAODTau/TauJet.h"
#include "MVAUtils/BDT.h"

class TString;

namespace tauRecTools {
  class BDTHelper : public asg::AsgMessaging {
    public:
      BDTHelper(); 
      ~BDTHelper(); 
   
      StatusCode initialize(const TString& weightFileName);

      float getGradBoostMVA(const std::map<TString, float>& availableVariables) const; 

      float getResponse(const std::map<TString, float*>& availableVariables) const; 

      float getGradBoostMVA(const xAOD::TauJet& tau) const; 

      MVAUtils::BDT* getBDT() const { return m_BDT.get(); }

    private:
      std::vector<TString> parseString(const TString& str, const TString& delim = ",") const;
      
      std::vector<float> getInputVariables(const std::map<TString, float>& availableVariables) const ;
      
      std::vector<float> getInputVariables(const std::map<TString, float*>& availableVariables) const;

      std::vector<float> getInputVariables(const xAOD::TauJet& tau) const ;

      std::unique_ptr<MVAUtils::BDT> m_BDT;
      std::vector<TString> m_inputVariableNames;
  };
}

#endif // not TAURECTOOLS_BDTHELPER_H

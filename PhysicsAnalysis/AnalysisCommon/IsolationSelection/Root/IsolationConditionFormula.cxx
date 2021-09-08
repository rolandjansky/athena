/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "IsolationSelection/IsolationConditionFormula.h"
// #include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <TF1.h>
#include <TH3.h>
#include <TString.h>
#include <cmath>
#include <algorithm>

namespace CP {
    IsolationConditionFormula::IsolationConditionFormula(std::string name, xAOD::Iso::IsolationType isoType, const std::string& cutFunction, bool invertCut) :
                IsolationCondition(name, isoType) {
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_invertCut = invertCut;
    }
    IsolationConditionFormula::IsolationConditionFormula(std::string name, std::string isoType, const std::string& cutFunction, bool invertCut) :
                IsolationCondition(name, isoType) {
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_invertCut = invertCut;
    }
    IsolationConditionFormula::IsolationConditionFormula(std::string name, std::string isoType, std::string isoBin, std::pair<int,int> isoBinRange, std::pair<float,float> etaRange, const std::string& cutFunction, bool invertCut) :
                IsolationCondition(name, isoType, isoBin) {
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_cutBinLow = isoBinRange.first;
	m_cutBinUp  = isoBinRange.second;
	m_cutEtaLow = etaRange.first;
	m_cutEtaUp  = etaRange.second;
        m_invertCut = invertCut;
    }
    bool IsolationConditionFormula::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt());
        if (c) (*c)[type()] = m_cutValue;

	//Needed for isolation-formulae only valid for specific eta-ranges
	float abseta(-99);
	getAbsEta(x, abseta);
	bool passEta = (m_cutEtaUp > 0. && m_cutEtaLow >= 0.) ? ( abseta >= m_cutEtaLow && abseta <= m_cutEtaUp ) : true;
	if(!passEta) return true;

	if(m_cutBinUp > -99){
	  if (!(*accessor()).isAvailable(x) || !(*accessor_bin()).isAvailable(x)) Warning("IsolationConditionFormula","Accessor is not available. Expected when using primary AODs, post-p3793 derivations (only for *FixedRad or FixedCutPflow*  for electrons), pre-p3517 derivations (only for FC*), or pre-p3830 derivations (for other electron WPs)");

	  bool inBinRange = (*accessor_bin())(x) >= m_cutBinLow && (*accessor_bin())(x) <= m_cutBinUp;
	  if(!inBinRange) return true;
	  
	  if (!m_invertCut) return (*accessor())(x) <= m_cutValue;
	  return (*accessor())(x) > m_cutValue;
	}

	if (!(*accessor()).isAvailable(x)) Warning("IsolationConditionFormula","Accessor is not available. Expected when using primary AODs, post-p3793 derivations (only for *FixedRad or FixedCutPflow*  for electrons), pre-p3517 derivations (only for FC*), or pre-p3830 derivations (for other electron WPs)");

	if (!m_invertCut) return (*accessor())(x) <= m_cutValue;
	return (*accessor())(x) > m_cutValue;
    }

    bool IsolationConditionFormula::accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt);
        if (c) (*c)[type()] = m_cutValue;
        if (!m_invertCut) return x.isolationValues[type()] <= m_cutValue;
        return x.isolationValues[type()] > m_cutValue;
    }

    void IsolationConditionFormula::getCutValue(const float pt) {
        m_cutValue = m_cutFunction->Eval(pt);
    }

  void IsolationConditionFormula::getAbsEta(const xAOD::IParticle& x, float &eta) {
    switch(x.type()){
    case xAOD::Type::Muon:
      eta = fabs( x.eta() );
      break;
    case xAOD::Type::Electron:
      if(dynamic_cast< const xAOD::Electron* >(&x)->caloCluster() != nullptr){
	eta = fabs( (dynamic_cast< const xAOD::Electron* >(&x))->caloCluster()->etaBE(2) );
      }
      else{
	eta = fabs( x.eta() );
      }
      break;
    case xAOD::Type::Photon:
      if(dynamic_cast< const xAOD::Photon* >(&x)->caloCluster() != nullptr){
	eta = fabs( dynamic_cast< const xAOD::Photon* >(&x)->caloCluster()->etaBE(2) );
      }
      else{
	eta = fabs( x.eta() );
      }
      break;
    default: break;
    }
  }

}

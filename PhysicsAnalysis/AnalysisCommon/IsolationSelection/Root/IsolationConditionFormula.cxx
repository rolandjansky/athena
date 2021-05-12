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
   bool IsolationConditionFormula::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt());
        if (c) (*c)[type()] = m_cutValue;
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
}

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "IsolationSelection/IsolationConditionFormula.h"
// #include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <TF1.h>
#include <TH3.h>
#include <TString.h>
#include <cmath>
#include <algorithm>

namespace CP {
    IsolationConditionFormula::IsolationConditionFormula(std::string name, xAOD::Iso::IsolationType isoType, const std::string& cutFunction) :
                IsolationCondition(name, isoType) {
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
    }

    void IsolationConditionFormula::setCut(xAOD::Iso::IsolationType isoType, const std::string& cutFunction) {
        IsolationCondition::setCut(isoType);
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
    }

    bool IsolationConditionFormula::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt());
        if (c) (*c)[m_isolationType] = m_cutValue;
        return (*m_acc)(x) <= m_cutValue;
    }

    bool IsolationConditionFormula::accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt);
        if (c) (*c)[m_isolationType] = m_cutValue;
        return x.isolationValues[m_isolationType] <= m_cutValue;
    }

    void IsolationConditionFormula::getCutValue(const float pt) {
        m_cutValue = m_cutFunction->Eval(pt);
    }
}

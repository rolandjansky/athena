/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "IsolationSelection/IsolationConditionCombined.h"
// #include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <TF1.h>
#include <TF2.h>
#include <TH3.h>
#include <TString.h>
#include <cmath>
#include <algorithm>

namespace CP {
    IsolationConditionCombined::IsolationConditionCombined(std::string name, std::vector<xAOD::Iso::IsolationType> isoType, const TF2& isoFunction, const std::string& cutFunction) :
                m_isolationTypes(isoType),
                IsolationCondition(name, isoType[0]) {
        for (auto& iso : isoType) m_accs.push_back(xAOD::getIsolationAccessor(iso));
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_isoFunction = std::shared_ptr < TF1 > (new TF2(isoFunction));
    }

    void IsolationConditionCombined::setCut(std::vector<xAOD::Iso::IsolationType> isoType, const TF2& isoFunction, const std::string& cutFunction) {
        m_isolationTypes = isoType;
        IsolationCondition::setCut(isoType[0]);
        for (auto& iso : isoType) m_accs.push_back(xAOD::getIsolationAccessor(iso));
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_isoFunction = std::shared_ptr < TF1 > (new TF2(isoFunction));
    }

    bool IsolationConditionCombined::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt());
        if (c) (*c)[m_isolationType] = m_cutValue;
        float isoValue = 0;
        std::vector<double> isoVars;
        for (auto& acc : m_accs) isoVars.push_back((*acc)(x));
        isoValue = m_isoFunction->EvalPar(&isoVars[0]);
        return isoValue <= m_cutValue;
    }

    bool IsolationConditionCombined::accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt);
        if (c) (*c)[m_isolationType] = m_cutValue;
        return x.isolationValues[m_isolationType] <= m_cutValue;
    }

    void IsolationConditionCombined::getCutValue(const float pt) {
        m_cutValue = m_cutFunction->Eval(pt);
    }
}

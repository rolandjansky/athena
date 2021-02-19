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
                IsolationCondition(name, isoType){
        m_cutFunction = std::shared_ptr < TF1 > (new TF1(cutFunction.c_str(), cutFunction.c_str()));
        m_isoFunction = std::shared_ptr < TF1 > (new TF2(isoFunction));
    }
    bool IsolationConditionCombined::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt());
        if (c) (*c)[type()] = m_cutValue;
        float isoValue = 0;
        std::vector<double> isoVars;
        for (unsigned int acc = 0; acc < num_types(); ++acc) if (!(*accessor(acc)).isAvailable(x)) Warning("IsolationConditionFormula","Accessor is not available. Expected when using primary AODs, post-p3793 derivations (only for *FixedRad or FixedCutPflow* for electrons), pre-p3517 derivations (only for FC*), or pre-p3830 derivations (for other electron WPs)");
        for (unsigned int acc = 0; acc < num_types(); ++acc) isoVars.push_back( (*accessor(acc) )(x));
        isoValue = m_isoFunction->EvalPar(&isoVars[0]);
        return isoValue <= m_cutValue;
    }

    bool IsolationConditionCombined::accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt);
        if (c) (*c)[type()] = m_cutValue;
        float isoValue = 0;
        std::vector<double> isoVars;
        for (unsigned int itype = 0; itype < num_types(); ++itype) isoVars.push_back( x.isolationValues[type(itype)] );
        isoValue = m_isoFunction->EvalPar(&isoVars[0]);
        return isoValue <= m_cutValue;
    }

    void IsolationConditionCombined::getCutValue(const float pt) {
        m_cutValue = m_cutFunction->Eval(pt);
    }
}

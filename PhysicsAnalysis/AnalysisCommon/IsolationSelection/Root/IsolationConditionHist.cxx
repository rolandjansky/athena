/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: IsolationConditionHist.cxx 678002 2015-06-24 15:39:36Z morrisj $
#include "IsolationSelection/IsolationConditionHist.h"
// #include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <TF1.h>
// #include <TH1.h>
#include <TH3.h>
#include <TString.h>
#include <cmath>
#include <algorithm>
// #include <sstream>

#include "IsolationSelection/Interp3D.h"

// #include <iostream>
// using std::cout;
// using std::endl;

namespace CP {
    IsolationConditionHist::IsolationConditionHist(std::string name, xAOD::Iso::IsolationType isoType, const std::string& isolationFunction, const std::shared_ptr<TH3F> efficiencyHisto3D) :
                IsolationCondition(name, isoType),
                m_efficiencyHisto3D(efficiencyHisto3D),
                m_interp(0) {
        m_isolationFunction = std::shared_ptr < TF1 > (new TF1(isolationFunction.c_str(), isolationFunction.c_str(), 0.0, 1000.0));

        /// check if pt is using GeV as unit
        std::string xtitle(m_efficiencyHisto3D->GetXaxis()->GetTitle());
        m_ptGeV = (xtitle.find("GeV") != std::string::npos);
    }

    void IsolationConditionHist::setCut(xAOD::Iso::IsolationType isoType, const std::string& isolationFunction, const std::shared_ptr<TH3F> efficiencyHisto3D) {
        if (efficiencyHisto3D) {
            m_efficiencyHisto3D = efficiencyHisto3D;
            /// check if pt is using GeV as unit
            std::string xtitle(m_efficiencyHisto3D->GetXaxis()->GetTitle());
            m_ptGeV = (xtitle.find("GeV") != std::string::npos);
        }

        IsolationCondition::setCut(isoType);
        m_isolationFunction = std::shared_ptr < TF1 > (new TF1(isolationFunction.c_str(), isolationFunction.c_str(), 0.0, 1000.0));
    }

    bool IsolationConditionHist::accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt(), x.eta());
        if (c) (*c)[m_isolationType] = m_cutValue;
        return (*m_acc)(x) <= m_cutValue * x.pt();
    }

    bool IsolationConditionHist::accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* c) {
        getCutValue(x.pt, x.eta);
        if (c) (*c)[m_isolationType] = m_cutValue;
        return x.isolationValues[m_isolationType] <= m_cutValue * x.pt;
    }

    void IsolationConditionHist::getCutValue(const float pt, const float eta) {
        float xpt = pt * 0.001; // convert to GeV
        if (!m_interp) m_cutValue = m_efficiencyHisto3D->GetBinContent(m_efficiencyHisto3D->GetXaxis()->FindBin(m_ptGeV ? xpt : pt), m_efficiencyHisto3D->GetYaxis()->FindBin(eta), std::min(int(m_isolationFunction->Eval(xpt)), 99));
        else m_cutValue = m_interp->Interpol3d(m_ptGeV ? xpt : pt, eta, std::min(m_isolationFunction->Eval(xpt), 99.), m_efficiencyHisto3D);
    }
}

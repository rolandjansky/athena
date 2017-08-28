// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONCONDITIONHIST_H
#define ISOLATIONSELECTION_ISOLATIONCONDITIONHIST_H

#include <memory>
#include <vector>
#include <map>

#include "IsolationSelection/IsolationCondition.h"

// Forward Declaration(s)
class TF1;
class TH3F;

class Interp3D;

namespace CP {
    class IsolationConditionHist: public IsolationCondition {
        public:
            IsolationConditionHist(std::string name, xAOD::Iso::IsolationType isoType, const std::string& isolationFunction, const std::shared_ptr<TH3F> efficiencyHisto3D);
            virtual ~IsolationConditionHist() {
            }

            IsolationConditionHist(const IsolationConditionHist& rhs) = delete;
            IsolationConditionHist& operator=(const IsolationConditionHist& rhs) = delete;
            void setCut(xAOD::Iso::IsolationType isoType, const std::string& isolationFunction, const std::shared_ptr<TH3F> efficiencyHisto3D = nullptr);

            bool accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0);
            bool accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0);
            void getCutValue(const float pt, const float eta);

            void setInterp(Interp3D* interp) {
                m_interp = interp;
            }


        private:
            std::shared_ptr<TH3F> m_efficiencyHisto3D;
            std::shared_ptr<TF1> m_isolationFunction;
            bool m_ptGeV;

            Interp3D* m_interp;
    };
}
#endif

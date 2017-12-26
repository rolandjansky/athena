// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONCONDITIONCOMBINED_H
#define ISOLATIONSELECTION_ISOLATIONCONDITIONCOMBINED_H

#include <memory>
#include "IsolationSelection/IsolationCondition.h"

// Forward Declaration(s)
class TF1;
class TF2;
class TH3F;

namespace CP {
    class IsolationConditionCombined: public IsolationCondition {
        public:
            IsolationConditionCombined(std::string name, std::vector<xAOD::Iso::IsolationType> isoType, const TF2& isoFunction, const std::string& cutFunction);
            virtual ~IsolationConditionCombined() {
            }

            IsolationConditionCombined(const IsolationConditionCombined& rhs) = delete;
            IsolationConditionCombined& operator=(const IsolationConditionCombined& rhs) = delete;
            void setCut(std::vector<xAOD::Iso::IsolationType> isoType, const TF2& isoFunction, const std::string& cutFunction);

            bool accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0);
            bool accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0);
            void getCutValue(const float pt);

        private:
            std::shared_ptr<TF1> m_cutFunction;
            std::shared_ptr<TF1> m_isoFunction;
            std::vector<xAOD::Iso::IsolationType> m_isolationTypes;
            std::vector<SG::AuxElement::Accessor<float>*> m_accs;
    };
}
#endif

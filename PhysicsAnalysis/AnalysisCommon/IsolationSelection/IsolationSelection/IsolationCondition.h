/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONCONDITION_H
#define ISOLATIONSELECTION_ISOLATIONCONDITION_H

#include <memory>
#include <vector>
#include <map>
#include <xAODPrimitives/IsolationType.h>
#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include "AthContainers/AuxElement.h"
#include <xAODBase/IParticle.h>

// Forward Declaration(s)
class TF1;
class TH3F;

struct strObj {
        float pt;
        float eta;
        std::vector<float> isolationValues;
        xAOD::Type::ObjectType type;
};

namespace CP {
    class IsolationCondition {
        public:
            IsolationCondition(const std::string& name, xAOD::Iso::IsolationType isoType);
            IsolationCondition(const std::string& name, const std::vector<xAOD::Iso::IsolationType>& isoTypes);
            IsolationCondition(const std::string& name, std::string &isoType);
            IsolationCondition(const std::string& name, const std::vector<std::string>& isoTypes);
           
            IsolationCondition(const IsolationCondition& rhs) = delete;
            IsolationCondition& operator=(const IsolationCondition& rhs) = delete;
            virtual ~IsolationCondition();
         
            std::string name() const;
            
            unsigned int num_types() const;
            xAOD::Iso::IsolationType  type(unsigned int n = 0) const;
            SG::AuxElement::Accessor<float>* accessor(unsigned int n = 0) const;
            
            virtual bool accept(const xAOD::IParticle& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0) = 0;
            virtual bool accept(const strObj& x, std::map<xAOD::Iso::IsolationType, float>* cutValues = 0) = 0;

        private:
            std::string m_name;
            std::vector<xAOD::Iso::IsolationType> m_isolationType;
            std::vector< std::unique_ptr<SG::AuxElement::Accessor<float>>> m_acc;           
        protected:
            float m_cutValue;
    };
}
#endif

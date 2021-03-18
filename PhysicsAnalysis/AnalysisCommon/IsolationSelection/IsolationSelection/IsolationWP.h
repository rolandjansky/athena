/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONWP_H
#define ISOLATIONSELECTION_ISOLATIONWP_H

#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <IsolationSelection/IsolationCondition.h>
#include "PATCore/AcceptData.h"
#include <map>
#include <memory>
namespace CP {
    class IsolationWP {
        public:
            IsolationWP(const std::string &name);
            ~IsolationWP();
            std::string name() const;
            void name(const std::string &name);


            asg::AcceptData accept(const xAOD::IParticle& p) const;
            asg::AcceptData accept(const strObj& p) const;
            void addCut(IsolationCondition* cut);
            void addCut(std::unique_ptr<IsolationCondition>& cut);
      
            const asg::AcceptInfo& getAccept() const;
            const std::vector<std::unique_ptr<IsolationCondition>>& conditions() const;
           
        private:
            std::string m_name;
            std::vector<std::unique_ptr<IsolationCondition>> m_cuts;
            std::map<xAOD::Iso::IsolationType, float>* m_cutValues;
            asg::AcceptInfo m_acceptInfo;
    };
}
#endif // ISOLATIONSELECTION_ISOLATIONWP_H

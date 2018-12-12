/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONWP_H
#define ISOLATIONSELECTION_ISOLATIONWP_H

#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <IsolationSelection/IsolationCondition.h>
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
#include <map>

namespace CP {
    class IsolationWP {
        public:
            IsolationWP(const std::string &name);
            ~IsolationWP();
            std::string name() const;
            void name(const std::string &name);


            const asg::AcceptData accept(const xAOD::IParticle& p) const;
            const asg::AcceptData accept(const strObj& p) const;
            void addCut(IsolationCondition* cut);
            const asg::AcceptInfo& getAcceptInfo() const;
            const std::vector<IsolationCondition*>& conditions() const;


        private:
            std::string m_name;
            std::vector<IsolationCondition*> m_cuts;
            std::map<xAOD::Iso::IsolationType, float>* m_cutValues;
            asg::AcceptInfo m_acceptInfo;
    };
}
#endif // ISOLATIONSELECTION_ISOLATIONWP_H

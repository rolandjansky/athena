/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ISOLATIONSELECTION_ISOLATIONWP_H
#define ISOLATIONSELECTION_ISOLATIONWP_H

#include <xAODPrimitives/tools/getIsolationAccessor.h>
#include <IsolationSelection/IsolationCondition.h>
#include "PATCore/TAccept.h"
#include <map>

namespace CP {
    class IsolationWP {
        public:
            IsolationWP(const std::string &name);
            ~IsolationWP();
            std::string name() const;
            void name(const std::string &name);


            const Root::TAccept& accept(const xAOD::IParticle& p) const;
            const Root::TAccept& accept(const strObj& p) const;
            void addCut(IsolationCondition* cut);
            const Root::TAccept& getAccept() const;
            const std::vector<IsolationCondition*>& conditions() const;


//            void saveCutValues(bool yes = true) {
//                if (yes && (!m_cutValues)) m_cutValues = new std::map<xAOD::Iso::IsolationType, float>();
//                else if ((!yes) && m_cutValues) {
//                    delete m_cutValues;
//                }
//            }
//            std::map<xAOD::Iso::IsolationType, float>* cutValues() {
//                return m_cutValues;
//            }

        private:
            std::string m_name;
            std::vector<IsolationCondition*> m_cuts;
            std::map<xAOD::Iso::IsolationType, float>* m_cutValues;
            mutable Root::TAccept m_accept;
    };
}
#endif // ISOLATIONSELECTION_ISOLATIONWP_H

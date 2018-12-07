/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationWP.h>

namespace CP {
    IsolationWP::~IsolationWP() {
        for (auto c : m_cuts)
            if (c) delete c;
    }

    std::string IsolationWP::name() const {
        return m_name;
    }
    void IsolationWP::name(const std::string &name) {
        m_name = name;
    }
    
    const asg::AcceptData IsolationWP::accept(const xAOD::IParticle& p) const {
      asg::AcceptData acceptData(&m_acceptInfo);
        for (auto c : m_cuts) {
            if (c->accept(p, m_cutValues)) {
              acceptData.setCutResult(c->name(), true);
            }
        }
        return acceptData;
    }

    const asg::AcceptData IsolationWP::accept(const strObj& p) const {
      asg::AcceptData acceptData(&m_acceptInfo);
      for (auto c : m_cuts) {
        if (c->accept(p, m_cutValues)) {
          acceptData.setCutResult(c->name(), true);
        }
      }
      return acceptData;
    }

    void IsolationWP::addCut(IsolationCondition* i) {
        m_cuts.push_back(i);
        m_acceptInfo.addCut(i->name(), i->name());
    }
    
    const asg::AcceptInfo& IsolationWP::getAcceptInfo() const {
        return m_acceptInfo;
    }
    
    const std::vector<IsolationCondition*>& IsolationWP::conditions() const{
        return m_cuts;
    }

    IsolationWP::IsolationWP(const std::string &name) :
                            m_name(name),
                            m_cuts(),
                            m_cutValues(nullptr),
                            m_acceptInfo(){
                }
}

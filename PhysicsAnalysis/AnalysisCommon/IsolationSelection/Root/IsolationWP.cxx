/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <IsolationSelection/IsolationWP.h>

namespace CP {
    IsolationWP::~IsolationWP() {
    }

    std::string IsolationWP::name() const {
        return m_name;
    }
    void IsolationWP::name(const std::string &name) {
        m_name = name;
    }
    asg::AcceptData IsolationWP::accept(const xAOD::IParticle& p) const {
        asg::AcceptData result (&m_acceptInfo);
        for (const auto& c : m_cuts) {
            if (c->accept(p, m_cutValues)) result.setCutResult(c->name(), true);
        }
        return result;
    }

    asg::AcceptData IsolationWP::accept(const strObj& p) const {
        asg::AcceptData result (&m_acceptInfo);
        for (const auto& c : m_cuts) {
            if (c->accept(p, m_cutValues)) {
                result.setCutResult(c->name(), true);
            }
        }
        return result;
    }

    
    void IsolationWP::addCut(IsolationCondition* i){
        m_cuts.push_back(std::unique_ptr<IsolationCondition>(i));
        m_acceptInfo.addCut(i->name(), i->name());
    }
      
    void IsolationWP::addCut(std::unique_ptr<IsolationCondition>& i) {
        // Push first an empty element back to the vector
        m_cuts.push_back(std::unique_ptr<IsolationCondition>());
        // Steal the owner ship from the old pointer :-P
        i.swap( m_cuts.back());
        m_acceptInfo.addCut(i->name(), i->name());
    }
    const asg::AcceptInfo& IsolationWP::getAccept() const {
        return m_acceptInfo;
    }
    const std::vector<std::unique_ptr<IsolationCondition>>& IsolationWP::conditions() const{
        return m_cuts;
    }

    IsolationWP::IsolationWP(const std::string &name) :
                            m_name(name),
                            m_cuts(),
                            m_cutValues(nullptr),
                            m_acceptInfo(){
                }
}

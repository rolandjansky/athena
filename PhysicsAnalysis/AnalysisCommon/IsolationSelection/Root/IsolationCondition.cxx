/*
 * IsolationCondition.cxx
 *
 *  Created on: 15.06.2017
 *      Author: johannes
 */

#include "IsolationSelection/IsolationCondition.h"

namespace CP{

    IsolationCondition::IsolationCondition(const std::string& name, const std::vector<xAOD::Iso::IsolationType> &isoTypes) :
                m_name(name),
                m_isolationType(isoTypes),
                m_cutValue(-999.) {
        for (auto iso_type : m_isolationType){
            m_acc.push_back(xAOD::getIsolationAccessor(iso_type));
        }        
    }
    IsolationCondition::IsolationCondition(const std::string &name, xAOD::Iso::IsolationType isoType) :
        IsolationCondition(name, std::vector<xAOD::Iso::IsolationType> {isoType}){
        }

    unsigned int IsolationCondition::num_types() const {
        return m_isolationType.size();
    }
            
    IsolationCondition::~IsolationCondition() {
    }
    std::string IsolationCondition::name() const{
        return m_name;
    }
    xAOD::Iso::IsolationType  IsolationCondition::type(unsigned int n) const{
        return  m_isolationType[n];
    }
    SG::AuxElement::Accessor<float>* IsolationCondition::accessor(unsigned int n) const{
        return m_acc[n];
    }
}




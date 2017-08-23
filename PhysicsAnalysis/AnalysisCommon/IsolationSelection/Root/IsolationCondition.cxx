/*
 * IsolationCondition.cxx
 *
 *  Created on: 15.06.2017
 *      Author: johannes
 */

#include "IsolationSelection/IsolationCondition.h"

namespace CP{

    IsolationCondition::IsolationCondition(std::string name, xAOD::Iso::IsolationType isoType) :
                m_name(name),
                m_isolationType(isoType),
                m_cutValue(-999.) {
        m_acc = xAOD::getIsolationAccessor(m_isolationType);
    }

    IsolationCondition::~IsolationCondition() {
    }

    IsolationCondition::IsolationCondition() :
                m_name(""),
                m_isolationType(xAOD::Iso::numIsolationTypes),
                m_acc(0),
                m_cutValue(-999) {
    }
     void IsolationCondition::setName(const std::string &name) {
        m_name = name;
    }
   void IsolationCondition::setCut(xAOD::Iso::IsolationType isoType) {
        m_isolationType = isoType;
        m_acc = xAOD::getIsolationAccessor(m_isolationType);
    }
    std::string IsolationCondition::name() const{
        return m_name;
    }
    xAOD::Iso::IsolationType  IsolationCondition::type() const{
        return  m_isolationType;
    }
    SG::AuxElement::Accessor<float>* IsolationCondition::accessor() const{
        return m_acc;
    }

}




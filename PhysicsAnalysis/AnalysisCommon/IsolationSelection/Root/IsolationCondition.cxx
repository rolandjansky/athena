/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "IsolationSelection/IsolationCondition.h"
#include "AthContainers/AuxElement.h"
namespace CP{

    IsolationCondition::IsolationCondition(const std::string& name, const std::vector<xAOD::Iso::IsolationType> &isoTypes) :
                m_name(name),
                m_isolationType(isoTypes),             
                m_cutValue(-999.) {
        for (auto iso_type : m_isolationType){
            m_acc.push_back(xAOD::getIsolationAccessor(iso_type));
        }        
    }
    IsolationCondition::IsolationCondition(const std::string& name, const std::vector<std::string> &isoTypes) :
                m_name(name),
                m_isolationType(std::vector<xAOD::Iso::IsolationType>()),              
                m_cutValue(-999.) {
        for (auto iso_type : isoTypes){
            m_isolationType.push_back(xAOD::Iso::IsolationType(0));
            m_acc.push_back(std::make_unique< SG::AuxElement::Accessor< float > >( iso_type ));
        }        
    }
    IsolationCondition::IsolationCondition(const std::string &name, xAOD::Iso::IsolationType isoType) :
        IsolationCondition(name, std::vector<xAOD::Iso::IsolationType> {isoType}){
        }
    IsolationCondition::IsolationCondition(const std::string &name, std::string &isoType) :
        IsolationCondition(name, std::vector<std::string> {isoType}){
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
        return m_acc[n].get();
    }
}




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
            m_acc.push_back(std::make_unique< SG::AuxElement::Accessor< float > >( toString(iso_type)));
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
    IsolationCondition::IsolationCondition(const std::string& name, const std::vector<std::string> &isoTypes, const std::vector<std::string> &isoBins) :
                m_name(name),
                m_isolationType(std::vector<xAOD::Iso::IsolationType>()),
                m_cutValue(-999.) {
        for (auto iso_type : isoTypes){
            m_isolationType.push_back(xAOD::Iso::IsolationType(0));
            m_acc.push_back(std::make_unique< SG::AuxElement::Accessor< float > >( iso_type ));
        }
        for (auto iso_bin : isoBins){
            m_acc_bin.push_back(std::make_unique< SG::AuxElement::Accessor< short > >( iso_bin ));
        }
    }
    IsolationCondition::IsolationCondition(const std::string &name, xAOD::Iso::IsolationType isoType) :
        IsolationCondition(name, std::vector<xAOD::Iso::IsolationType> {isoType}){
        }
    IsolationCondition::IsolationCondition(const std::string &name, std::string &isoType) :
        IsolationCondition(name, std::vector<std::string> {isoType}){
        }
    IsolationCondition::IsolationCondition(const std::string &name, std::string &isoType, std::string &isoBin) :
        IsolationCondition(name, std::vector<std::string> {isoType}, std::vector<std::string> {isoBin}){
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
    SG::AuxElement::Accessor<short>* IsolationCondition::accessor_bin(unsigned int n) const{
        return m_acc_bin[n].get();
  }

}




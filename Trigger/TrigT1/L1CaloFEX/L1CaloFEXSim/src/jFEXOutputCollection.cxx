/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/

//***************************************************************************  
//                           jFEXOutputCollection.cxx  - 
//                              -------------------
//     begin                : 09 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************
//
#include "L1CaloFEXSim/jFEXOutputCollection.h"

LVL1::jFEXOutputCollection::jFEXOutputCollection() {
  m_dooutput = false;
}

LVL1::jFEXOutputCollection::~jFEXOutputCollection() {

    for(uint i=0; i<m_allvalues_smallRJet.size(); i++) {
        m_allvalues_smallRJet.at(i).reset();
    }

    for(uint i=0; i<m_allvalues_largeRJet.size(); i++) {
        m_allvalues_largeRJet.at(i).reset();
    }

    for(uint i=0; i<m_allvalues_tau.size(); i++) {
        m_allvalues_tau.at(i).reset();
    }

    for(uint i=0; i<m_allvalues_pileup.size(); i++) {
        m_allvalues_pileup.at(i).reset();
    }

    for(uint i=0; i<m_allvalues_pileup_map.size(); i++) {
        m_allvalues_pileup_map.at(i).reset();
    }

}

void LVL1::jFEXOutputCollection::clear()
{

    for(uint i=0; i<m_allvalues_smallRJet.size(); i++) {
        (*m_allvalues_smallRJet.at(i)).clear();
    }

    for(uint i=0; i<m_allvalues_largeRJet.size(); i++) {
        (*m_allvalues_largeRJet.at(i)).clear();
    }

    for(uint i=0; i<m_allvalues_tau.size(); i++) {
        (*m_allvalues_tau.at(i)).clear();
    }

    for(uint i=0; i<m_allvalues_pileup.size(); i++) {
        (*m_allvalues_pileup.at(i)).clear();
    }

    for(uint i=0; i<m_allvalues_pileup_map.size(); i++) {
        (*m_allvalues_pileup_map.at(i)).clear();
    }

}

void LVL1::jFEXOutputCollection::addValue_smallRJet(std::string key, int value)
{
    m_values_tem_smallRJet.insert(std::make_pair(key, value));
}

void LVL1::jFEXOutputCollection::addValue_largeRJet(std::string key, int value)
{
    m_values_tem_largeRJet.insert(std::make_pair(key, value));
}

void LVL1::jFEXOutputCollection::addValue_tau(std::string key, int value)
{
    m_values_tem_tau.insert(std::make_pair(key, value));
}

void LVL1::jFEXOutputCollection::addValue_pileup(std::string key, int value)
{
    m_values_tem_pileup.insert(std::make_pair(key, value));
}

void LVL1::jFEXOutputCollection::addValue_pileup(std::string key, std::vector<int>  value)
{
    m_values_tem_pileup_maps.insert(std::make_pair(key, value));
}

void LVL1::jFEXOutputCollection::fill_smallRJet()
{
    std::unique_ptr<std::unordered_map<std::string, int>> values_local = std::make_unique<std::unordered_map<std::string, int>>(m_values_tem_smallRJet);
    m_allvalues_smallRJet.push_back(std::move(values_local));
    m_values_tem_smallRJet.clear();

}
void LVL1::jFEXOutputCollection::fill_largeRJet()
{
    std::unique_ptr<std::unordered_map<std::string, int>> values_local = std::make_unique<std::unordered_map<std::string, int>>(m_values_tem_largeRJet);
    m_allvalues_largeRJet.push_back(std::move(values_local));
    m_values_tem_largeRJet.clear();

}
void LVL1::jFEXOutputCollection::fill_tau()
{
    std::unique_ptr<std::unordered_map<std::string, int>> values_local = std::make_unique<std::unordered_map<std::string, int>>(m_values_tem_tau);
    m_allvalues_tau.push_back(std::move(values_local));
    m_values_tem_tau.clear();

}
void LVL1::jFEXOutputCollection::fill_pileup()
{
    std::unique_ptr<std::unordered_map<std::string, int>> values_local = std::make_unique<std::unordered_map<std::string, int>>(m_values_tem_pileup);
    std::unique_ptr<std::unordered_map<std::string, std::vector<int> >> values_local_map = std::make_unique<std::unordered_map<std::string, std::vector<int>>>(m_values_tem_pileup_maps);
    m_allvalues_pileup.push_back(std::move(values_local));
    m_allvalues_pileup_map.push_back(std::move(values_local_map));
    m_values_tem_pileup.clear();
    m_values_tem_pileup_maps.clear();

}


int LVL1::jFEXOutputCollection::SRJetSize() const
{
    return m_allvalues_smallRJet.size();
}

int LVL1::jFEXOutputCollection::LRJetSize() const
{
    return m_allvalues_largeRJet.size();
}
int LVL1::jFEXOutputCollection::TauSize() const
{
    return m_allvalues_tau.size();
}
int LVL1::jFEXOutputCollection::PileupSize() const
{
    return m_allvalues_pileup.size();
}
int LVL1::jFEXOutputCollection::PileupEtSize() const
{
    return m_allvalues_pileup_map.size();
}

int LVL1::jFEXOutputCollection::get_smallRJet(int location,std::string str_) const
{
    return (*m_allvalues_smallRJet.at(location))[str_];
}
int LVL1::jFEXOutputCollection::get_largeRJet(int location,std::string str_) const
{
    return (*m_allvalues_largeRJet.at(location))[str_];
}
int LVL1::jFEXOutputCollection::get_tau(int location,std::string str_) const
{
    return (*m_allvalues_tau.at(location))[str_];
}
int LVL1::jFEXOutputCollection::get_pileup(int location,std::string str_) const
{
    return (*m_allvalues_pileup.at(location))[str_];
}
std::vector<int> LVL1::jFEXOutputCollection::get_pileup_map(int location,std::string str_) const
{
    return (*m_allvalues_pileup_map.at(location))[str_];
}

void LVL1::jFEXOutputCollection::setdooutput(bool input) {
    m_dooutput = input;
}

bool LVL1::jFEXOutputCollection::getdooutput() const {
    return m_dooutput;
}

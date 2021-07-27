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

LVL1::jFEXOutputCollection::~jFEXOutputCollection()
{
  for(auto iValues : m_allvalues_smallRJet){
    delete iValues;	
  }

 for(auto iValues: m_allvalues_largeRJet){
   delete iValues;
  }
  
 for(auto iValues: m_allvalues_tau){
   delete iValues;
  }
  
}

void LVL1::jFEXOutputCollection::clear() 
{
  for(auto iValues : m_allvalues_smallRJet){
    iValues->clear();
  }
  for(auto iValues : m_allvalues_largeRJet){
    iValues->clear();
  }
  for(auto iValues : m_allvalues_tau){
    iValues->clear();
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

void LVL1::jFEXOutputCollection::fill_smallRJet()
{
  std::map<std::string, int>* values_local = new std::map<std::string, int>(m_values_tem_smallRJet);
  m_allvalues_smallRJet.push_back(values_local);
  m_values_tem_smallRJet.clear();

}
void LVL1::jFEXOutputCollection::fill_largeRJet()
{
  std::map<std::string, int>* values_local = new std::map<std::string, int>(m_values_tem_largeRJet);
  m_allvalues_largeRJet.push_back(values_local);
  m_values_tem_largeRJet.clear();

}
void LVL1::jFEXOutputCollection::fill_tau()
{
  std::map<std::string, int>* values_local = new std::map<std::string, int>(m_values_tem_tau);
  m_allvalues_tau.push_back(values_local);
  m_values_tem_tau.clear();

}


int LVL1::jFEXOutputCollection::SRsize()
{
  return m_allvalues_smallRJet.size();
}

int LVL1::jFEXOutputCollection::LRsize()
{
  return m_allvalues_largeRJet.size();
}
int LVL1::jFEXOutputCollection::tausize()
{
  return m_allvalues_tau.size();
}

std::map<std::string, int>* LVL1::jFEXOutputCollection::get_smallRJet(int location)
{
  return m_allvalues_smallRJet[location];
}
std::map<std::string, int>* LVL1::jFEXOutputCollection::get_largeRJet(int location)
{
  return m_allvalues_largeRJet[location];
}
std::map<std::string, int>* LVL1::jFEXOutputCollection::get_tau(int location)
{
  return m_allvalues_tau[location];
}

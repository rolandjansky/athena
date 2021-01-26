/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
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
  for (auto iValues : m_allvalues_smallRJet){
    delete iValues;	
  }

 // for(auto iValues: m_allvalues_largeRJet){
 //   delete iValues;
 // }
}

void LVL1::jFEXOutputCollection::clear() 
{
  for (auto iValues : m_allvalues_smallRJet){
    iValues->clear();
  }
//for(auto iValues : m_allvallues_largeRJet){
//  iValues->clear();
//}
}
void LVL1::jFEXOutputCollection::addValue_smallRJet(std::string key, float value)
{
 m_values_tem_smallRJet.insert(std::make_pair(key, value));
}

//void LVL1::jFEXOutputCollection::addValue_largeRJet(std::string key, float value)
//{
// m_values_tem_largeRJet.insert(std::make_pair(key, value));
//}

void LVL1::jFEXOutputCollection::fill_smallRJet()
{
  std::map<std::string, float>* values_local = new std::map<std::string, float>(m_values_tem_smallRJet);
  m_allvalues_smallRJet.push_back(values_local);
  m_values_tem_smallRJet.clear();

}
//void LVL1::jFEXOutputCollection::fill_largeRJet()
//
//
//
int LVL1::jFEXOutputCollection::size()
{
  return m_allvalues_smallRJet.size();
}

std::map<std::string, float>* LVL1::jFEXOutputCollection::get_smallRJet(int location)
{
  return m_allvalues_smallRJet[location];
}
//std::map<std::string, float>* LVL1::jFEXOutputCollection::get_largeRJet(int location)

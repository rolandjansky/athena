/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXOutputCollection.cxx  -  
//                              -------------------
//     begin                : 28 02 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************

#include "L1CaloFEXSim/eFEXOutputCollection.h"

LVL1::eFEXOutputCollection::~eFEXOutputCollection()
{
  for (auto iValues : m_allvalues_eg) {
    delete iValues;
  }
  for (auto iValues : m_allvalues_tau) {
    delete iValues;
  }
}

void LVL1::eFEXOutputCollection::clear()
{
  for (auto iValues : m_allvalues_eg) {
    iValues->clear();
  }
  for (auto iValues : m_allvalues_tau) {
    iValues->clear();
  }
}

void LVL1::eFEXOutputCollection::addValue_eg(std::string key, float value)
{
  m_values_tem_eg.insert(std::make_pair(key, value));
}

void LVL1::eFEXOutputCollection::addValue_tau(std::string key, float value)
{
  m_values_tem_tau.insert(std::make_pair(key, value));
}

void LVL1::eFEXOutputCollection::fill_eg()
{
  std::map<std::string, float>* values_local = new std::map<std::string, float>(m_values_tem_eg);
  m_allvalues_eg.push_back(values_local);
  m_values_tem_eg.clear();
}

void LVL1::eFEXOutputCollection::fill_tau()
{
  std::map<std::string, float>* values_local = new std::map<std::string, float>(m_values_tem_tau);
  m_allvalues_tau.push_back(values_local);
  m_values_tem_tau.clear();
}

int LVL1::eFEXOutputCollection::size()
{
  return m_allvalues_eg.size();
}

std::map<std::string, float>* LVL1::eFEXOutputCollection::get_eg(int location)
{
  return m_allvalues_eg[location];
}

std::map<std::string, float>* LVL1::eFEXOutputCollection::get_tau(int location)
{
  return m_allvalues_tau[location];
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXOutputCollection
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gFEXOutputCollection.h"

LVL1::gFEXOutputCollection::~gFEXOutputCollection()
{
}

void LVL1::gFEXOutputCollection::clear()
{
  for (auto iValues : m_allvalues_gFEXjet) {
    iValues.clear();
  }
  m_jettob.clear();
}


void LVL1::gFEXOutputCollection::addValueJet(std::string key, float value)
{
  m_values_gFEXJet.emplace(std::move(key), value);
}


void LVL1::gFEXOutputCollection::fillJet()
{
  m_allvalues_gFEXjet.push_back(std::move(m_values_gFEXJet));
  m_values_gFEXJet.clear();
}


int LVL1::gFEXOutputCollection::size()
{
  return m_allvalues_gFEXjet.size();
}

std::unordered_map<std::string, float> LVL1::gFEXOutputCollection::getJet(int location) const
{
  return m_allvalues_gFEXjet[location];
}


void LVL1::gFEXOutputCollection::addJetTob(uint32_t jettob)
{
  m_jettob.push_back(jettob);
}

std::vector<uint32_t> LVL1::gFEXOutputCollection::getJetTob() const
{
  return m_jettob;
}

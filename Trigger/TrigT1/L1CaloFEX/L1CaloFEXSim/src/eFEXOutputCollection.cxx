/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1CaloFEXSim/eFEXOutputCollection.h"

LVL1::eFEXOutputCollection::eFEXOutputCollection() {
  m_dooutput = false;
}

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
  m_eFexNumber.clear();
  m_emtob.clear();
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

int LVL1::eFEXOutputCollection::tau_size() const
{
  return m_allvalues_tau.size();
}

int LVL1::eFEXOutputCollection::size() const
{
  return m_allvalues_eg.size();
}

std::map<std::string, float>* LVL1::eFEXOutputCollection::get_eg(int location) const
{
  return m_allvalues_eg.at(location);
}

std::map<std::string, float>* LVL1::eFEXOutputCollection::get_tau(int location) const
{
  return m_allvalues_tau.at(location);
}

void LVL1::eFEXOutputCollection::addeFexNumber(int efexnumber)
{
  m_eFexNumber.push_back(efexnumber);
}

std::vector<int> LVL1::eFEXOutputCollection::geteFexNumber() const
{
  return m_eFexNumber;
}

void LVL1::eFEXOutputCollection::addEMtob(uint32_t emtob)
{
  m_emtob.push_back(emtob);
}

std::vector<uint32_t> LVL1::eFEXOutputCollection::getEMtob() const
{
  return m_emtob;
}


void LVL1::eFEXOutputCollection::setdooutput(bool input) {
  m_dooutput = input;
}

bool LVL1::eFEXOutputCollection::getdooutput() const {
  return m_dooutput;
}
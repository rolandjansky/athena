/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRampData.h"

void L1CaloRampStep::addEvent(double energy)
{
    ++m_nEntries;
    m_w += energy;
    m_w2 += (energy * energy);
}

double L1CaloRampStep::mean() const 
{
    if(!m_nEntries) return 0.;
    return m_w / double(m_nEntries);
}

double L1CaloRampStep::rms() const 
{
    if(!m_nEntries) return 1.;
    if(m_nEntries == 1) return 1.;
    double mean = this->mean();
    return std::sqrt((m_w2 / double(m_nEntries))) - mean;
}

void L1CaloRampData::addData(unsigned int step, const std::pair<double, double>& energies)
{
    this->addData(step, energies.first, energies.second);
}

void L1CaloRampData::addData(unsigned int step, double calorimeter, double level1)
{
    if(m_rampDataVector.size() <= step) {
        m_rampDataVector.resize(step + 1);
    }

    m_rampDataVector[step].first.addEvent(calorimeter);
    m_rampDataVector[step].second.addEvent(level1);
}

void L1CaloRampData::addData(unsigned int step, const L1CaloRampDataPoint& data)
{
    if(m_rampDataVector.size() <= step) {
        m_rampDataVector.resize(step + 1);
    }
    m_rampDataVector[step] = data;
}

void L1CaloRampData::addData(const std::pair<double, double>& energies) {
    this->addData(energies.first, energies.second);
}

void L1CaloRampData::addData(double calorimeter, double level1) {
    m_rampDataVector[m_iStep].first.addEvent(calorimeter);
    m_rampDataVector[m_iStep].second.addEvent(level1);
}

void L1CaloRampData::setStep(unsigned int step) {
    m_iStep = step;

    if(m_rampDataVector.size() <= step) {
        m_rampDataVector.resize(step + 1);
    }
}

void L1CaloRampData::nextStep() {
    ++m_iStep;
    if(m_rampDataVector.size() <= m_iStep) {
        m_rampDataVector.resize(m_iStep + 1);
    }
}

unsigned int L1CaloRampData::getNSteps() const {
    return m_rampDataVector.size();
}

const L1CaloRampData::L1CaloRampDataPoint* L1CaloRampData::getStep(unsigned int step) const {
    if(m_rampDataVector.size() <= step) return 0;
    return &(m_rampDataVector[step]);
}

const L1CaloRampStep* L1CaloRampData::getCalo(unsigned int step) const {
    if(m_rampDataVector.size() <= step) return 0;
    return &(m_rampDataVector[step].first);
}

const L1CaloRampStep* L1CaloRampData::getLevel1(unsigned int step) const {
    if(m_rampDataVector.size() <= step) return 0;
    return &(m_rampDataVector[step].second);
}

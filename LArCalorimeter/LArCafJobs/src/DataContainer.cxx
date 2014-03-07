/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/ClassCounts.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


DataContainer::DataContainer()
  : m_gain(CaloGain::UNKNOWNGAIN), m_eventIndex(-1),
    m_energy(Definitions::none), m_time(Definitions::none), m_quality(Definitions::none),
    m_pedestal(Definitions::none), m_pedestalRMS(Definitions::none),
    m_status(0xffffffff),  m_adcMax(-1)
{ 
  ClassCounts::incrementInstanceCount("DataContainer"); 
}


DataContainer::DataContainer(CaloGain::CaloGain gain, const std::vector<short>& samples,
                             float energy, float time, float quality,
                             int eventIndex,
                             LArVectorProxy autoCorrs, float noise,
                             float pedestal, float pedestalRMS,
                             int status, float adcMax)
  : m_gain(gain), m_samples(samples), m_eventIndex(eventIndex),
    m_energy(energy), m_time(time), m_quality(quality),
    m_pedestal(pedestal), m_pedestalRMS(pedestalRMS),
    m_status(status), m_adcMax(adcMax)
{
  ClassCounts::incrementInstanceCount("DataContainer"); 
  m_corrs.reserve( autoCorrs.size());
  m_corrs.push_back(noise*noise);
  for (unsigned int i = 0; i < autoCorrs.size() && i < nSamples() - 1; ++i) 
    m_corrs.push_back(autoCorrs[i]*noise*noise);
}
      
      
DataContainer::DataContainer(CaloGain::CaloGain gain, const std::vector<short>& samples, 
                             const std::vector<float>& corrs,
                             int eventIndex,
                             float energy, float time, float quality,
                             float pedestal, float pedestalRMS,
                             int status, float adcMax)
  : m_gain(gain), m_samples(samples), m_corrs(corrs), m_eventIndex(eventIndex),
    m_energy(energy), m_time(time), m_quality(quality),
    m_pedestal(pedestal), m_pedestalRMS(pedestalRMS), 
    m_status(status), m_adcMax(adcMax)
{
  ClassCounts::incrementInstanceCount("DataContainer"); 
}
      

DataContainer::DataContainer(const DataContainer& other, double adcMax, double time)
  : m_gain(other.m_gain), m_samples(other.m_samples), m_corrs(other.m_corrs),
    m_eventIndex(other.m_eventIndex),
    m_energy(other.m_energy), m_time(adcMax > 0 ? time : other.m_time), m_quality(other.m_quality),
    m_pedestal(other.m_pedestal), m_pedestalRMS(other.m_pedestalRMS), 
    m_status(other.m_status), m_adcMax(adcMax > 0 ? adcMax : other.m_adcMax)
{
  ClassCounts::incrementInstanceCount("DataContainer"); 
}
             

DataContainer::~DataContainer()
{
  ClassCounts::decrementInstanceCount("DataContainer"); 
}


bool DataContainer::isValid() const
{
  //if (gain() == CaloGain::UNKNOWNGAIN) return false;
  if (eventIndex() < 0) return false;
  if (Definitions::isNone(energy())) return false;
  if (Definitions::isNone(ofcTime())) return false;
  if (Definitions::isNone(quality())) return false;
  //if (Definitions::isNone(pedestal())) return false;
  //if (m_corrs.size() == 0) return false; temporary
  if (status() < 0) return false;

  return true;
}

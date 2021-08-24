/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/DataTweaker.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/ShapeFitter.h"
#include "LArCafJobs/SimpleShape.h"
#include "LArSamplesMon/ScaledErrorData.h"
#include "LArSamplesMon/OFC.h"

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
   
using namespace LArSamples;
     
        
bool DataTweaker::set(const TString& tweaks)
{
  TString toMod = tweaks;
  
  if (findOption(toMod, "removeNoiseCorrelations")) { m_removeNoiseCorrelations = true; cout << "--> removeNoiseCorrelations" << endl; }
  if (findOption(toMod, "refit"))                   { m_refit = true;                   cout << "--> refit" << endl; }
  if (findOption(toMod, "adjust"))                  { m_adjust = true;                  cout << "--> adjust" << endl; }
  if (findOption(toMod, "removeRoIs"))              { m_removeRoIs = true;              cout << "--> removeRoIs" << endl; }

  if (toMod == "") return true;
  cout << "Invalid tweaker option(s) " << toMod << endl;
  return false;
}


bool DataTweaker::findOption(TString& tweaks, const TString& option) const
{
  int i = tweaks.Index(option);
  if (i < 0) return false;
  tweaks = tweaks(0, i) + tweaks(i + option.Length(), tweaks.Length() - i - option.Length());
  return true;
}


Data* DataTweaker::tweak(const Data& data, int evtIndex) const
{
  std::vector<short> samples = data.container().samples();
  std::vector<float> corrs = data.container().corrs();
  float time = data.container().ofcTime();
  double k = 1, deltaT = 0;

  if (m_removeNoiseCorrelations)
    for (unsigned int i = 1; i < corrs.size(); i++) corrs[i] = 0;

  if (m_nSamplesTruncation > 0) {
    unsigned int maxIndex = data.maxPosition();
    
    int i1 = maxIndex - (m_nSamplesTruncation - 1)/2;
    if (i1 < 0) i1 = 0;
    
    int i2 = maxIndex + m_nSamplesTruncation/2 + 1;
    if (i2 > (int)data.nSamples()) i2 = (int)data.nSamples();
    samples.clear();
    for (int i = i1; i < i2; i++) samples.push_back(data.sample(i));
    corrs.resize(i2 - i1);
    time = time - data.time(i1);
  }
  
  if (!m_samples.empty()) {
    std::vector<short> oldSamples = samples;
    samples.clear();
    for (std::map<unsigned int, bool>::const_iterator i = m_samples.begin();
         i != m_samples.end(); ++i) 
    {
      samples.push_back(oldSamples[i->first]);
      if (i == m_samples.begin()) time = time - data.time(i->first);
    }
  }

  if (m_refit) {
    SimpleShape* reference = data.referenceShape();
    const ScaledErrorData* sed = data.scaledErrorData();
    ShapeFitter fitter(m_fitParams);
    double chi2;
    if (!reference) return nullptr;
    bool result = fitter.fit(data, *reference, k, deltaT, chi2, sed);
    delete sed;
    if (!result) return nullptr;
  }

  if (m_adjust) {
    if (!data.history() || data.adcMax() == 0) return nullptr;
    OFC* ofc = data.history()->ofc(data.index());
    if (!ofc) return nullptr;
    k = ofc->A(data)/data.adcMax();
    deltaT = ofc->time(data);
  }
  
  if (evtIndex < 0) evtIndex = data.container().eventIndex();
  
  DataContainer* newContainer = new DataContainer(data.container().gain(), samples, corrs,
                evtIndex,
                data.container().energy(), time + deltaT, data.container().quality(),
                data.container().pedestal(), data.container().pedestalRMS(),
                data.container().status(), k*data.container().adcMax());
  Data* newData = new Data(*newContainer, *data.eventData(), data.history(), data.index(), true);
  return newData;
}


EventData* DataTweaker::tweak(const EventData& eventData, int runIndex) const
{
  EventData* newEventData = new EventData(eventData, runIndex);
  if (m_removeRoIs) newEventData->removeRoIs();
  return newEventData;
}

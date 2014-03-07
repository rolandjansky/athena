/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArCafJobs/RunData.h"

#include "LArCafJobs/ClassCounts.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


RunData::RunData(int run) 
  : m_run(run) 
{ 
  ClassCounts::incrementInstanceCount("RunData");
}
      
RunData::RunData(const RunData& other)
       : m_run(other.m_run), m_triggerConfig(other.m_triggerConfig)
{
  ClassCounts::incrementInstanceCount("RunData");
}


RunData::~RunData()
{
  ClassCounts::decrementInstanceCount("RunData");
}


int RunData::triggerBitPosition(const TString& triggerBitName) const
{
  std::map<TString, unsigned int>::const_iterator entry = m_triggerConfig.find(triggerBitName);
  if (entry == m_triggerConfig.end()) return -1;
  return entry->second;
}


TString RunData::triggerBits() const
{
  TString bits;
  for (std::map<TString, unsigned int>::const_iterator bit = m_triggerConfig.begin();
       bit != m_triggerConfig.end(); bit++) {
    if (bits != "") bits += "\n";
    bits += bit->first;
  }
  return bits;
}


TString RunData::description(unsigned int verbosity) const
{
  TString printout = "";
  if (verbosity & 1)
    printout +=  Form("run %6d", run());
  return printout;
}

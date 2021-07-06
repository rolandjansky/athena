/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "LArCafJobs/EventData.h"

#include "LArCafJobs/RunData.h"
#include "LArCafJobs/ClassCounts.h"
#include "TMath.h"
#include "TEllipse.h"
#include "TBox.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


EventData::EventData() :
  m_runData(0)
{ 
  ClassCounts::incrementInstanceCount("EventData");
  m_eventData.push_back(0);  // event
  m_eventData.push_back(0);  // run
  m_eventData.push_back(0);  // LB
  m_eventData.push_back(0);  // BCID
}


EventData::EventData(const EventData& other, int newRunIndex)
   : m_eventData(other.m_eventData), 
     m_rois(other.m_rois),
     m_runData(other.m_runData) 
{ 
  ClassCounts::incrementInstanceCount("EventData");
  if (newRunIndex != -1) setRunIndex(newRunIndex); 
}
             

EventData::EventData(int event, int run, int lumiBlock, int bunchId) :
  m_runData(0)
{
  ClassCounts::incrementInstanceCount("EventData");
  m_eventData.push_back(event);
  m_eventData.push_back(run);
  m_eventData.push_back(lumiBlock);
  m_eventData.push_back(bunchId);
}


EventData::~EventData()
{
  ClassCounts::decrementInstanceCount("EventData");
}


int EventData::run() const
{
  if (!runData()) return -1;
  return runData()->run(); 
}


std::vector<unsigned int> EventData::triggerData() const
{
  std::vector<unsigned int> trigData;
  if (m_eventData.size() < 4) return trigData; // should never happen
  trigData.insert(trigData.end(), m_eventData.begin() + 4, m_eventData.end());
  return trigData;
}


void EventData::setTriggerData(const std::vector<unsigned int>& l1Words)
{ 
  m_eventData.insert(m_eventData.end(), l1Words.begin(), l1Words.end());
}


unsigned int EventData::triggerWord(unsigned int i) const 
{ 
  if (i >= nTriggerWords()) return 0;
  return m_eventData[i + 4]; 
}


bool EventData::isPassed(const TString& bitName) const
{
  if (!runData()) return false;
  int index = runData()->triggerBitPosition(bitName);
  if (index < 0 && bitName == "L1_MBTS_1_1") index = 228;
  if (index < 0) return false;
  return bit((unsigned int)index);
}


bool EventData::bit(unsigned int pos) const
{
  unsigned int word = pos/32;
  unsigned int bpos = pos%32;
  if (word >= nTriggerWords()) return false;
  return ((triggerWord(word) >> bpos) & 0x1);
}


TString EventData::triggers() const
{
  TString triggers = "";
  if (!runData()) return triggers;
  for (const std::pair<const TString, unsigned int>& p : runData()->triggerConfig())
  {
    if (isPassed(p.first)) {
      if (triggers != "") triggers += " ";
      triggers += p.first;
    }
  }
  return triggers;
}


TString EventData::roIs() const
{
  TString str = "";
  for (unsigned int i = 0; i < nRoIs(); i++)
    str += Form("(%4.2f, %4.2f) ", roI(i).eta, roI(i).phi);
  return str;
}


TString EventData::description(unsigned int verbosity) const
{
  TString printout = "";
  if (verbosity & 1)
    printout +=  Form("run %6d, event %8d, lb = %4d", run(), event(), lumiBlock());
  if (verbosity & 2)
    printout += Form(", triggers = %40s", triggers().Data());
  if (verbosity & 4) {
    printout += ", RoIs = ";
    for (unsigned int i = 0; i < nRoIs(); i++)
      printout += Form("%-20s(%5.2f, %5.2f) ", roI(i).name.Data(), roI(i).eta, roI(i).phi);
  }
  return printout;
}


void EventData::drawDRRoIs(double roISize) const
{
  TEllipse ell;
  ell.SetFillStyle(0);
  
  for (unsigned int i = 0; i < nRoIs(); i++)
    ell.DrawEllipse(roI(i).eta, roI(i).phi, roISize, roISize, 0, 360, 0);
}

void EventData::drawBoxRoIs(double etaSize, double phiSize) const
{
  TBox box;
  box.SetFillStyle(0);
  
  for (unsigned int i = 0; i < nRoIs(); i++) drawBoxRoI(roI(i).eta, roI(i).phi, etaSize, phiSize);
}


void EventData::drawBoxRoI(double eta, double phi, double etaSize, double phiSize)
{
  TBox box;
  box.SetFillStyle(0);
  
  box.DrawBox(eta - etaSize/2, phi - phiSize/2, eta + etaSize/2, phi + phiSize/2);
}


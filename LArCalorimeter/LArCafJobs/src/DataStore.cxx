/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/DataStore.h"

#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/HistoryContainer.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/RunData.h"
#include "LArCafJobs/PersistentAccessor.h"
#include "Identifier/IdentifierHash.h"
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace LArSamples;


DataStore::DataStore()
{
  for (unsigned int i = 0; i < nChannels(); i++) m_cellHistories.push_back(0);
}
  
  
DataStore::~DataStore()
{
  for (HistoryContainer* h : m_cellHistories)
    delete h;
}


HistoryContainer* DataStore::makeNewHistory(const IdentifierHash& hash, CellInfo* info)
{
  HistoryContainer*& histCont = hist_cont(hash);
  if (histCont) return histCont;
  histCont = new HistoryContainer(info);
  return histCont;
}


bool DataStore::addData(const IdentifierHash& hash, DataContainer* data)
{
  HistoryContainer*& histCont = hist_cont(hash);
  histCont->add(data);
  return true;
}


unsigned int DataStore::addEvent(EventData* eventData)
{
  m_events.push_back(eventData);
  return nEvents() - 1;
}


unsigned int DataStore::addRun(RunData* runData)
{
  m_runs.push_back(runData);
  return nRuns() - 1;
}


unsigned int DataStore::size() const
{
  unsigned int size = 0;
 
 for (unsigned int i = 0; i < nChannels(); i++) {
   const HistoryContainer* hist = historyContainer(i);
   if (!hist) continue;
   size += hist->nDataContainers();
 }
 
 return size;
}


unsigned int DataStore::nFilledChannels() const
{
  unsigned int n = 0;
 
 for (unsigned int i = 0; i < nChannels(); i++) {
   const HistoryContainer* hist = historyContainer(i);
   if (!hist) continue;
   n++;
 }
 
 return n;
}


double DataStore::footprint() const 
{  
  double fp = sizeof(*this); 
  for (unsigned int i = 0; i < nChannels(); i++) {
    const HistoryContainer* hist = historyContainer(i);
    if (!hist) continue;
    fp += hist->footprint();
  }
    
  return fp; 
}


bool DataStore::writeTrees(const char* fileName)
{
  PersistentAccessor* acc = new PersistentAccessor(fileName);

  for (unsigned int i = 0; i < nRuns(); i++) {
    RunData*& r = runData(i);
    acc->addRun(r);
    delete r; r = 0;
  }

  for (unsigned int i = 0; i < nEvents(); i++) {
    EventData*& ev = eventData(i);
    acc->addEvent(ev);
    delete ev; ev = 0;
  }
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    HistoryContainer*& hc = hist_cont(i);
    acc->add(hc);	
    delete hc; hc = 0;
  }

  bool result = acc->save();
  delete acc;
  return result;
}

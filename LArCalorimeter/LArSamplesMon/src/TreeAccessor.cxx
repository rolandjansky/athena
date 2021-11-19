/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/TreeAccessor.h"

#include "LArSamplesMon/Data.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/RunData.h"
#include "LArSamplesMon/FilterParams.h"
#include "LArSamplesMon/DataTweaker.h"
#include "LArSamplesMon/ClassCounts.h"
#include "TObjString.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TString.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using std::cout;
using std::endl;

using namespace LArSamples;


TreeAccessor* TreeAccessor::open(const TString& fileName) 
{
  TFile* file = TFile::Open(fileName);
  if (!file) return nullptr;
  if (!file->IsOpen()) { delete file; return nullptr; }
  TTree* cellTree = (TTree*)file->Get("cells");
  if (!cellTree) return nullptr;
  TTree* eventTree = (TTree*)file->Get("events");
  if (!eventTree) return nullptr;
  TTree* runTree = (TTree*)file->Get("runs");
  TreeAccessor* accessor = new TreeAccessor(*cellTree, *eventTree, runTree, file);
  return accessor;
}


const CellInfo* TreeAccessor::getCellInfo(unsigned int i) const 
{ 
  const HistoryContainer* cont = historyContainer(i);
  if (!cont || !cont->cellInfo()) return nullptr;
  return new CellInfo(*cont->cellInfo());
}


const History* TreeAccessor::getCellHistory(unsigned int i) const 
{ 
  //cout << "A" << endl;
  if (i >= cellTree().GetEntries()) return nullptr;
  //cout << "---> TTree::GetEntry " << i << endl;
  getCellEntry(i);
  //cout << "---> done TTree::GetEntry" << endl;

  //cout << "B " << currentContainer()->nDataContainers() << endl;
  std::vector<const EventData*> eventDatas;
  
  for (unsigned int k = 0; k < currentContainer()->nDataContainers(); k++) {
    const EventData* evtData = eventData(currentContainer()->dataContainer(k)->eventIndex());
    EventData* newEvtData = (evtData ? new EventData(*evtData) : nullptr);
    eventDatas.push_back(newEvtData);
  }
  //cout << "C" << endl;
  //cout << "---> TreeAcc : make new hist" << endl;
  return (currentContainer()->cellInfo() ? new History(*currentContainer(), eventDatas, i) : nullptr);
}


TreeAccessor* TreeAccessor::merge(const std::vector<const Accessor*>& accessors,
                                                          const TString& fileName)
{
  cout << "Merging to " << fileName << endl;
  TreeAccessor* newAcc = new TreeAccessor(fileName);
  unsigned int size = 0;
  CellInfo* info = nullptr;

  int evtIndex = 0, runIndex = 0;
  std::map<std::pair<int, int>, int> evtMap;
  std::map<int, int> runMap;

  cout << "Merging runs" << endl;
  for (const Accessor* accessor : accessors) {
    if (!accessor) {
      cout << "Cannot merge: one of the inputs is null!" << endl;
      return nullptr;
    }
    for (unsigned int i = 0; i < accessor->nRuns(); i++) {
      int run = accessor->runData(i)->run();
      if (runMap.find(run) != runMap.end()) continue;
      runMap[run] = runIndex;
      RunData* newRun = new RunData(*accessor->runData(i));
      newAcc->addRun(newRun);
      delete newRun;
      runIndex++;
    }
  }
  
  cout << "Merging events" << endl;
  unsigned int nEventsTotal = 0, iEvt = 0;
  for (const Accessor* accessor : accessors)
    nEventsTotal += accessor->nEvents();  
  for (const Accessor* accessor : accessors) {
    for (unsigned int i = 0; i < accessor->nEvents(); i++) {
      iEvt++;
      if (iEvt % 100000 == 0) cout << "Merging event " << iEvt << "/" << nEventsTotal << endl;
      std::pair<int, int> evtId(accessor->eventData(i)->run(), accessor->eventData(i)->event());
      if (evtMap.find(evtId) != evtMap.end()) continue;
      evtMap[evtId] = evtIndex;
      std::map<int, int>::const_iterator idx = runMap.find(accessor->eventData(i)->run());
      int newRunIndex = (idx == runMap.end() ? -999 : idx->second);
      //cout << "Storing eventData for run " << accessor->eventData(i)->run() << " at index " << newRunIndex << " instead of " << accessor->eventData(i)->runIndex() << endl;
      EventData* newEvent = new EventData(*accessor->eventData(i), newRunIndex);
      newAcc->addEvent(newEvent);
      delete newEvent;
      evtIndex++;
    }
  } 
  
  for (unsigned int i = 0; i < newAcc->nChannels(); i++) {
    if (i % 10000 == 0) {
      cout << "Merging channel " << i << "/" <<  newAcc->nChannels() << " (current size = " << size << ")" << endl;
      //ClassCounts::printCountsTable();
    }
    HistoryContainer* historyContainer = nullptr;
  for (const Accessor* accessor : accessors) {
      const History* history = accessor->cellHistory(i);
      if (!history || !history->isValid()) continue;
      if (!historyContainer) {
        info = new CellInfo(*history->cellInfo());
        historyContainer = new HistoryContainer(info);
      }
      for (unsigned int j = 0; j < history->nData(); j++) {
        DataContainer* newDC = new DataContainer(history->data(j)->container());
        std::map<std::pair<int, int>, int>::const_iterator newIndex 
          = evtMap.find(std::make_pair(history->data(j)->run(), history->data(j)->event()));
        if (newIndex == evtMap.end()) cout << "Event not found for cell " << i << ", data " << j << "." << endl;
        newDC->setEventIndex(newIndex != evtMap.end() ? newIndex->second : -1);
        historyContainer->add(newDC);
        if (!info->shape(history->data(j)->gain())) {
          const ShapeInfo* shape = history->cellInfo()->shape(history->data(j)->gain());
          if (!shape) 
            cout << "Shape not filled for hash = " << i << ", index = " << j << ", gain = " << Data::gainStr(history->data(j)->gain()) << endl;
          info->setShape(history->data(j)->gain(), (shape ? new ShapeInfo(*shape) : nullptr));
        }
      }
    }
    if (historyContainer) size += historyContainer->nDataContainers();
    newAcc->add(historyContainer);
    delete historyContainer;
  }

  cout << "Merging done, final size = " << size << endl;
  newAcc->save();
  return newAcc;
}


TreeAccessor* TreeAccessor::merge(const std::vector<const Accessor*>& accessors,const TString& fileName,const TString& LBFile)
{
  // O.Simard - 01.07.2011
  // Alternative version with LB cleaning.

  bool kBadLB=false;
  std::vector<unsigned int> LBList;
  std::ifstream infile(LBFile.Data());
  std::string line;
  // assume single-line format with coma-separated LBs (from python)
  std::getline(infile,line,'\n');
  TString filter(line.c_str());
  TObjArray* list = filter.Tokenize(", "); // coma\space delimiters
  if(list->GetEntries() == 0){
    printf("No LB filtering specified, or bad format. Exiting.\n");
    delete list;
    return nullptr;
  }
  
  for(int k = 0; k < list->GetEntries(); k++){
    TObjString* tobs = (TObjString*)(list->At(k));
    LBList.push_back((unsigned int)(tobs->String()).Atoi());
  }
  delete list;
  printf("LB List: %d\n",(int)LBList.size());
  

  // from here it is similar to other functions of this class
  TreeAccessor* newAcc = new TreeAccessor(fileName);
  unsigned int size = 0;
  CellInfo* info = nullptr;

  int evtIndex = 0, runIndex = 0;
  std::map<std::pair<int, int>, int> evtMap;
  std::map<int, int> runMap;

  cout << "Merging runs" << endl;
  for (const Accessor* accessor : accessors) {
    if (!accessor) {
      cout << "Cannot merge: one of the inputs is null!" << endl;
      return nullptr;
    }
    for (unsigned int i = 0; i < accessor->nRuns(); i++) {
      int run = accessor->runData(i)->run();
      if (runMap.find(run) != runMap.end()) continue;
      runMap[run] = runIndex;
      RunData* newRun = new RunData(*accessor->runData(i));
      newAcc->addRun(newRun);
      delete newRun;
      runIndex++;
    }
  }
  
  cout << "Merging events" << endl;
  unsigned int nEventsTotal = 0, iEvt = 0;
  for (const Accessor* accessor : accessors)
    nEventsTotal += accessor->nEvents();  
  for (const Accessor* accessor : accessors) {
    for (unsigned int i = 0; i < accessor->nEvents(); i++) {
      iEvt++;
      if (iEvt % 100000 == 0) cout << "Merging event " << iEvt << "/" << nEventsTotal << endl;

      // ----
      // skip LBs which are found in the list
      kBadLB=false;
      for(unsigned int ilb = 0 ; ilb < LBList.size() ; ilb++){
        if(LBList.at(ilb)==accessor->eventData(i)->lumiBlock()){
	  kBadLB=true;
	  //printf("  == Rejecting Event in LB %4d\n",accessor->eventData(i)->lumiBlock());
	  break;
	}
      }
      if(kBadLB) continue;
      // ----

      std::pair<int, int> evtId(accessor->eventData(i)->run(), accessor->eventData(i)->event());
      if (evtMap.find(evtId) != evtMap.end()) continue;
      evtMap[evtId] = evtIndex;
      std::map<int, int>::const_iterator idx = runMap.find(accessor->eventData(i)->run());
      int newRunIndex = (idx == runMap.end() ? -999 : idx->second);
      EventData* newEvent = new EventData(*accessor->eventData(i), newRunIndex);
      newAcc->addEvent(newEvent);
      delete newEvent;
      evtIndex++;
    }
  } 
  
  for (unsigned int i = 0; i < newAcc->nChannels(); i++) {
    if (i % 10000 == 0) {
      cout << "Merging channel " << i << "/" <<  newAcc->nChannels() << " (current size = " << size << ")" << endl;
      //ClassCounts::printCountsTable();
    }
    HistoryContainer* historyContainer = nullptr;
  for (const Accessor* accessor : accessors) {
      const History* history = accessor->cellHistory(i);
      if (!history || !history->isValid()) continue;
      if (!historyContainer) {
        info = new CellInfo(*history->cellInfo());
        historyContainer = new HistoryContainer(info);
      }
      for (unsigned int j = 0; j < history->nData(); j++) {
        DataContainer* newDC = new DataContainer(history->data(j)->container());
        std::map<std::pair<int, int>, int>::const_iterator newIndex 
          = evtMap.find(std::make_pair(history->data(j)->run(), history->data(j)->event()));
        //if (newIndex == evtMap.end()) cout << "Event not found for cell " << i << ", data " << j << "." << endl;
        newDC->setEventIndex(newIndex != evtMap.end() ? newIndex->second : -1);
        historyContainer->add(newDC);
        if (!info->shape(history->data(j)->gain())) {
         const ShapeInfo* shape = history->cellInfo()->shape(history->data(j)->gain());
         if (!shape) 
           cout << "Shape not filled for hash = " << i << ", index = " << j << ", gain = " << Data::gainStr(history->data(j)->gain()) << endl;
          info->setShape(history->data(j)->gain(), (shape ? new ShapeInfo(*shape) : nullptr));
        }
      }
    }
    if(historyContainer){
      size += historyContainer->nDataContainers();
    }
    newAcc->add(historyContainer);
    delete historyContainer;
    historyContainer=nullptr;
    //}
  }

  cout << "Merging done, final size = " << size << endl;
  newAcc->save();
  return newAcc;
}


TreeAccessor* TreeAccessor::filter(const Accessor& accessor,
                                   const FilterParams& filterParams,
                                   const TString& fileName,
                                   const DataTweaker& tweaker)
{
  FilterList filterList; filterList.add(filterParams, fileName);  
  std::vector<TreeAccessor*> result = filter(accessor, filterList, tweaker);
  return (!result.empty() ? result[0] : nullptr);
}

std::vector<TreeAccessor*> 
TreeAccessor::filter(const Accessor& accessor,
                     const FilterList& filterList, 
                     const DataTweaker& tweaker)
{
  if (filterList.size() == 0) {
    cout << "No filter categories specified, done! (?)" << endl;
    return std::vector<TreeAccessor*>();
  }
  
  for (unsigned int f = 0; f < filterList.size(); f++) {
    cout << "Skimming to " << filterList.fileName(f) << endl;
    if (!gSystem->AccessPathName(filterList.fileName(f))) {
      cout << "File already exists, exiting." << endl;
      return std::vector<TreeAccessor*>();
    }
  }
  
  std::vector<TreeAccessor*> newAccessors;
  for (unsigned int f = 0; f < filterList.size(); f++)
    newAccessors.push_back(new TreeAccessor(filterList.fileName(f)));
  std::map<std::pair<unsigned int, unsigned int>, unsigned int> eventIndices;
  std::vector< std::map<unsigned int, unsigned int> > eventsToKeep(filterList.size());
  std::vector< std::map<unsigned int, unsigned int> > runsToKeep(filterList.size());
  
  double nTot = 0, nPass = 0; 
  
  for (unsigned int i = 0; i < accessor.nEvents(); i++) {
    const EventData* eventData = accessor.eventData(i);
    eventIndices[std::pair<unsigned int, unsigned int>(eventData->run(), eventData->event())] = i;
  }  

  for (unsigned int i = 0; i < accessor.nChannels(); i++) {
    if (i % 25000 == 0) {
      cout << "Filtering " << i << "/" <<  accessor.nChannels() 
       << " (passing so far = " << nPass << ", total seen = " << nTot << ")" << endl;
      //ClassCounts::printCountsTable();
    }    
    bool first = true;
    
    const History* history = nullptr;
    for (unsigned int f = 0; f < filterList.size(); f++) {
      history = accessor.pass(i, filterList.filterParams(f));
      if (history) break;
    }
    for (unsigned int f = 0; f < filterList.size(); f++) {
      if (!history || !history->cellInfo() || !filterList.filterParams(f).passCell(*history->cellInfo())) {
        HistoryContainer* newHist = new HistoryContainer();
        newAccessors[f]->add(newHist);
        delete newHist;
        continue;
      }
      if (first) { nTot += history->nData(); first = false; }
      HistoryContainer* newHist = new HistoryContainer(new CellInfo(*history->cellInfo()));
      for (unsigned int k = 0; k < history->nData(); k++) {
        if (!filterList.filterParams(f).passEvent(*history->data(k))) continue;
        const EventData* eventData = history->data(k)->eventData();
        std::map<std::pair<unsigned int, unsigned int>, unsigned int>::const_iterator findIndex = 
          eventIndices.find(std::pair<unsigned int, unsigned int>(eventData->run(), eventData->event()));
        if (findIndex == eventIndices.end()) { 
          cout << "Inconsistent event numbering!!!" << endl; 
          delete newHist;
          return std::vector<TreeAccessor*>(); 
        }
        int oldEvtIndex = findIndex->second;
        bool isNewEvt = (eventsToKeep[f].find(oldEvtIndex) == eventsToKeep[f].end());
        unsigned int newEvtIndex = (isNewEvt ? eventsToKeep[f].size() : eventsToKeep[f][oldEvtIndex]);      
        if (isNewEvt) eventsToKeep[f][oldEvtIndex] = newEvtIndex;

        int oldRunIndex = history->data(k)->eventData()->runIndex();
        bool isNewRun = (runsToKeep[f].find(oldRunIndex) == runsToKeep[f].end());
        unsigned int newRunIndex = (isNewRun ? runsToKeep[f].size() : runsToKeep[f][oldRunIndex]);      
        if (isNewRun) runsToKeep[f][oldRunIndex] = newRunIndex;

        Data* newData = tweaker.tweak(*history->data(k), newEvtIndex);
        if (!newData) {
          cout << "Filtering failed on data " << k << " of cell " << i << ", aborting" << endl;
          delete newHist;
          for (unsigned int f = 0; f < filterList.size(); f++) delete newAccessors[f];
          return std::vector<TreeAccessor*>();
        }
        nPass++;
        newHist->add(newData->dissolve());
      }
      newAccessors[f]->add(newHist);
      delete newHist;
    }
  }

  for (unsigned int f = 0; f < filterList.size(); f++) {      
    cout << "Adding runs..." << endl;
    std::vector<unsigned int> runsToKeep_ordered(runsToKeep[f].size());
    for (const auto& runIndex : runsToKeep[f]) 
      runsToKeep_ordered[runIndex.second] = runIndex.first;

    for (unsigned int runIndex : runsToKeep_ordered) {
      RunData* newRun = new RunData(*accessor.runData(runIndex));
      newAccessors[f]->addRun(newRun);
      delete newRun;  
    }
    cout << "Adding events..." << endl;
    std::vector<unsigned int> eventsToKeep_ordered(eventsToKeep[f].size());
    for (const auto& eventIndex : eventsToKeep[f])
      eventsToKeep_ordered[eventIndex.second] = eventIndex.first;

    for (unsigned int eventIndex : eventsToKeep_ordered) {
      std::map<unsigned int, unsigned int>::const_iterator idx = runsToKeep[f].find(accessor.eventData(eventIndex)->runIndex());
      int newRunIndex = (idx == runsToKeep[f].end() ? 0 : idx->second);
      EventData* newEvent = tweaker.tweak(*accessor.eventData(eventIndex), newRunIndex);
      newAccessors[f]->addEvent(newEvent);
      delete newEvent;
    }
  }
  cout << "Filtering done! final size = " << nPass << endl;
  //ClassCounts::printCountsTable();
  for (unsigned int f = 0; f < filterList.size(); f++) {      
    cout << "Saving " << newAccessors[f]->fileName() << endl;    
    newAccessors[f]->save();
  }
  return newAccessors;
}


TreeAccessor* TreeAccessor::makeTemplate(const Accessor& accessor, const TString& fileName)
{
  TreeAccessor* newAccessor = new TreeAccessor(fileName);
  
  std::vector<short> samples;
  for (unsigned int k = 0; k < 5; k++) samples.push_back(0);
  std::vector<float> autoCorrs;
  for (unsigned int k = 0; k < 4; k++) autoCorrs.push_back(0);
  
  RunData* dummyRun = new RunData(0);
  newAccessor->addRun(dummyRun);
  delete dummyRun;
  
  EventData* dummyEvent = new EventData(0, 0, 0, 0);
  newAccessor->addEvent(dummyEvent);
  delete dummyEvent;

  for (unsigned int i = 0; i < accessor.nChannels(); i++) {
    if (i % 25000 == 0)
      cout << "Templating " << i << "/" <<  accessor.nChannels() << endl;
    const History* history = accessor.cellHistory(i);
    if (!history || !history->cellInfo()) {
      HistoryContainer* newHist = new HistoryContainer();
      newAccessor->add(newHist);
      delete newHist;
      continue;
    }
    HistoryContainer* newHist = new HistoryContainer(new CellInfo(*history->cellInfo()));
    DataContainer* dataContainer = new DataContainer(CaloGain::LARHIGHGAIN, samples, 0, 0, 0, 0, autoCorrs);
    newHist->add(dataContainer);
    newAccessor->add(newHist);
    delete newHist;
  }

  newAccessor->save();  
  return newAccessor;
}
      
bool TreeAccessor::writeToFile(const TString& fileName) const
{
  TFile* newFile = new TFile(fileName, "RECREATE");
  if (newFile && !newFile->IsOpen()) { delete newFile; newFile = nullptr; }
  if (!newFile) return false;
  
  cellTree().Write();
  eventTree().Write();

  delete newFile;
  
  return true;
}

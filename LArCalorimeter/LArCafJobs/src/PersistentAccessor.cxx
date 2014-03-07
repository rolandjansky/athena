/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/PersistentAccessor.h"

#include "LArCafJobs/ClassCounts.h"
#include "LArCafJobs/Definitions.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ShapeInfo.h"
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


PersistentAccessor::PersistentAccessor(TTree& cellTree, TTree& eventTree, TTree* runTree, TFile* file)
  : m_cellTree(&cellTree), m_eventTree(&eventTree), m_runTree(runTree), m_file(file), 
    m_historyCont(0), m_eventData(0), m_runData(0)
{
  ClassCounts::incrementInstanceCount("PersistentAccessor"); 
  m_cellTree->SetBranchAddress("history", &m_historyCont);
  m_eventTree->SetBranchAddress("event", &m_eventData);
  m_eventTree->LoadBaskets(); // loads the tree to memory
  if (m_eventTree->MemoryFull(0)) {
    cout << "WARNING: could not load all the baskets of the event tree into memory -- processing will be very slow." << endl;
    cout << "Please check if the file you are loading is too large or corrupted." << endl;
  }
  if (m_runTree) {
    m_runTree->SetBranchAddress("run", &m_runData);
    m_runTree->LoadBaskets();
  }
}


PersistentAccessor::PersistentAccessor(const TString& fileName)
  : m_cellTree(0), m_eventTree(0), m_runTree(0), m_file(0), 
    m_historyCont(0), m_eventData(0), m_runData(0)
{
  ClassCounts::incrementInstanceCount("PersistentAccessor"); 
  if (TString(fileName) != "") m_file = new TFile(fileName, "RECREATE");
  if (m_file && !m_file->IsOpen()) { delete m_file; m_file = 0; }
  m_cellTree = new TTree("cells", "");
  m_eventTree = new TTree("events", "");
  m_runTree = new TTree("runs", "");
  m_cellTree->Branch("history", &m_historyCont, 32000, 0);
  m_eventTree->Branch("event", &m_eventData, 32000, 0);
  m_runTree->Branch("run", &m_runData, 32000, 0);
  m_eventTree->SetAutoSave(0); // keep everything in memory
  m_eventTree->SetAutoFlush(0); // keep everything in memory
  m_runTree->SetAutoSave(0);   // keep everything in memory
  m_runTree->SetAutoFlush(0);   // keep everything in memory
}


PersistentAccessor* PersistentAccessor::open(const TString& fileName) 
{
  TFile* file = TFile::Open(fileName);
  if (!file) return 0;
  if (!file->IsOpen()) { delete file; return 0; }
  TTree* cellTree = (TTree*)file->Get("cells");
  if (!cellTree) return 0;
  TTree* eventTree = (TTree*)file->Get("events");
  if (!eventTree) return 0;
  TTree* runTree = (TTree*)file->Get("runs");
  PersistentAccessor* accessor = new PersistentAccessor(*cellTree, *eventTree, runTree, file);
  return accessor;
}


PersistentAccessor::~PersistentAccessor()
{
  ClassCounts::decrementInstanceCount("PersistentAccessor"); 
  for (std::map<unsigned int, const RunData*>::iterator run = m_runCache.begin();
      run != m_runCache.end(); ++run) 
    delete run->second;
  if (m_file) 
    delete m_file;
  else {
    delete m_cellTree;
    delete m_eventTree;
    delete m_runTree;
  }
  if (m_historyCont) delete m_historyCont;
  if (m_eventData) delete m_eventData;
  if (m_runData) delete m_runData;
}


const HistoryContainer* PersistentAccessor::historyContainer(unsigned int i) const
{
  m_cellTree->GetEntry(i);
  return m_historyCont;
}


unsigned int PersistentAccessor::historySize(unsigned int i) const
{
  const HistoryContainer* cont = historyContainer(i);
  return (cont ? cont->nDataContainers() : 0);
}


const EventData* PersistentAccessor::eventData(unsigned int i) const
{
  if (i >= nEvents()) return 0;
  m_eventTree->GetEntry(i);
  m_eventData->setRunData(runData(m_eventData->runIndex()));
  return m_eventData;
}


const RunData* PersistentAccessor::runData(unsigned int i) const
{
  if (i >= nRuns()) return 0;
  std::map<unsigned int, const RunData*>::const_iterator cache = m_runCache.find(i);
  if (cache != m_runCache.end()) return cache->second;

  m_runTree->GetEntry(i);
  RunData* newRunData = new RunData(*m_runData);
  m_runCache[m_eventData->runIndex()] = newRunData;
  return newRunData;
}

        
TString PersistentAccessor::fileName() const 
{ 
  return (m_file ? m_file->GetName() : ""); 
}


void PersistentAccessor::add(HistoryContainer* cont)
{
  m_historyCont = cont;
  m_cellTree->Fill();
  m_historyCont = 0;
}


void PersistentAccessor::addEvent(EventData* eventData)
{
  m_eventData = eventData;
  m_eventTree->Fill();
  m_eventData = 0;
}


void PersistentAccessor::addRun(RunData* runData)
{
  m_runData = runData;
  m_runTree->Fill();
  m_runData = 0;
}


bool PersistentAccessor::save() const
{
  if (!m_file) return 0;
  m_file->cd();
  cout << "Writing " << m_runTree->GetEntries() << " run(s)..." << endl;
  m_runTree->Write();
  cout << "Writing " << m_eventTree->GetEntries() << " event(s)..." << endl;
  m_eventTree->Write();
  cout << "Writing " << m_cellTree->GetEntries() << " cell(s)..." << endl;
  m_cellTree->Write();
  m_file->Flush();
  cout << "Writing done!" << endl;
  return true;
}


PersistentAccessor* PersistentAccessor::merge(const std::vector<const PersistentAccessor*>& accessors,
                                              const TString& fileName)
{
  PersistentAccessor* newAcc = new PersistentAccessor(fileName);
  unsigned int size = 0;
  CellInfo* info = 0;

  int evtIndex = 0, runIndex = 0;
  std::map<std::pair<int, int>, int> evtMap;
  std::map<int, int> runMap;
  std::map< std::pair<const PersistentAccessor*, int>, int > evtAccMap;
  
  cout << "Merging runs" << endl;
  for (std::vector<const PersistentAccessor*>::const_iterator accessor = accessors.begin();
       accessor != accessors.end(); accessor++) {
    if (!*accessor) {
      cout << "Cannot merge: one of the inputs is null!" << endl;
      delete newAcc;
      return 0;
    }
    for (unsigned int i = 0; i < (*accessor)->nRuns(); i++) {
      int run = (*accessor)->runData(i)->run();
      if (runMap.find(run) != runMap.end()) continue;
      runMap[run] = runIndex;
      RunData* newRun = new RunData(*(*accessor)->runData(i));
      newAcc->addRun(newRun);
      delete newRun;
      runIndex++;
    }
  }
  
  cout << "Merging events" << endl;
  unsigned int nEventsTotal = 0, iEvt = 0;
    for (std::vector<const PersistentAccessor*>::const_iterator accessor = accessors.begin();
       accessor != accessors.end(); accessor++) nEventsTotal += (*accessor)->nEvents();  
  for (std::vector<const PersistentAccessor*>::const_iterator accessor = accessors.begin();
       accessor != accessors.end(); accessor++) {
    for (unsigned int i = 0; i < (*accessor)->nEvents(); i++) {
      iEvt++;
      if (iEvt % 100000 == 0) cout << "Merging event " << iEvt << "/" << nEventsTotal << endl;
      std::pair<int, int> evtId((*accessor)->eventData(i)->run(), (*accessor)->eventData(i)->event());
      std::pair<const PersistentAccessor*, int> evtAccId(*accessor, i);
      if (evtMap.find(evtId) != evtMap.end()) {
        cout << "ERROR: Skipping duplicate entry for run " << (*accessor)->eventData(i)->run() << ", event " << (*accessor)->eventData(i)->event() << endl;
        continue;
      }
      evtAccMap[evtAccId] = evtIndex;
      evtMap[evtId] = evtIndex;
      std::map<int, int>::const_iterator idx = runMap.find((*accessor)->eventData(i)->run());
      int newRunIndex = (idx == runMap.end() ? -999 : idx->second);
      //cout << "Storing eventData for run " << (*accessor)->eventData(i)->run() << " at index " << newRunIndex << " instead of " << (*accessor)->eventData(i)->runIndex() << endl;
      EventData* newEvent = new EventData(*(*accessor)->eventData(i), newRunIndex);
      newAcc->addEvent(newEvent);
      delete newEvent;
      evtIndex++;
    }
  } 
  
  for (unsigned int i = 0; i < Definitions::nChannels; i++) {
    if (i % 10000 == 0) {
      cout << "Merging channel " << i << "/" << Definitions::nChannels << " (current size = " << size << ")" << endl;
      //ClassCounts::printCountsTable();
    }
    HistoryContainer* newHistory = 0;
    for (std::vector<const PersistentAccessor*>::const_iterator accessor = accessors.begin();
         accessor != accessors.end(); accessor++) {
      const HistoryContainer* history = (*accessor)->historyContainer(i);
      if (!history || !history->isValid()) continue;
      if (!newHistory) {
        info = new CellInfo(*history->cellInfo());
        newHistory = new HistoryContainer(info);
      }
      for (unsigned int j = 0; j < history->nDataContainers(); j++) {
        DataContainer* newDC = new DataContainer(*history->dataContainer(j));
        std::map<std::pair<const PersistentAccessor*, int>, int>::const_iterator newIndex 
          = evtAccMap.find(std::make_pair(*accessor, history->dataContainer(j)->eventIndex()));
        if (newIndex == evtAccMap.end()) cout << "Event not found for cell " << i << ", data " << j << "." << endl;
        newDC->setEventIndex(newIndex != evtAccMap.end() ? newIndex->second : -1);
        newHistory->add(newDC);
        if (!info->shape(history->dataContainer(j)->gain())) {
         const ShapeInfo* shape = history->cellInfo()->shape(history->dataContainer(j)->gain());
         if (!shape) 
           cout << "Shape not filled for hash = " << i << ", index = " << j << ", gain = " << history->dataContainer(j)->gain() << endl;
         info->setShape(history->dataContainer(j)->gain(), (shape ? new ShapeInfo(*shape) : 0));
        }
      }
    }
    if (newHistory) size += newHistory->nDataContainers();
    newAcc->add(newHistory);
    delete newHistory;
  }

  cout << "Merging done, final size = " << size << endl;
  newAcc->save();
  return newAcc;
}


PersistentAccessor* PersistentAccessor::merge(const std::vector<TString>& inputFiles, const TString& fileName)
{
  std::vector<const PersistentAccessor*> accessors;
  for (std::vector<TString>::const_iterator inputFile = inputFiles.begin(); inputFile != inputFiles.end(); inputFile++) {
    PersistentAccessor* accessor = open(*inputFile);
    if (!accessor) {
      cout << "ERROR : could not open file " << *inputFile << endl;
      return 0;
    }
    accessors.push_back(accessor);
  }
  PersistentAccessor* result = merge(accessors, fileName);
  for (std::vector<const PersistentAccessor*>::iterator accessor = accessors.begin(); accessor != accessors.end(); accessor++) 
    delete *accessor;
  return result;
}

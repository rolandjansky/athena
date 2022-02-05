/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/MultiTreeAccessor.h"

#include "LArSamplesMon/TreeAccessor.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/EventData.h"
#include "TSystem.h"
#include "TChain.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using std::string; 
using std::cout;
using std::endl;
using namespace std;

using namespace LArSamples;


MultiTreeAccessor* MultiTreeAccessor::open(const std::vector<TString>& files) 
{
  std::vector<const TreeAccessor*> accessors;
  for (const TString& fileName : files) {
    const TreeAccessor* accessor = TreeAccessor::open(fileName);
    if (!accessor) {
      cout << "Skipping invalid file " << fileName << endl;
      continue;
    }
    accessors.push_back(accessor);
  }
  
  if (accessors.empty()) return nullptr;
  return new MultiTreeAccessor(accessors);
}


MultiTreeAccessor* MultiTreeAccessor::openList(const TString& fileList) 
{
  std::ifstream f(fileList);
  if (!f) {
    cout << "file " << fileList << " not accessible" << endl;
    return nullptr;
  }

  std::string fileName;  
  unsigned int i = 0;

  std::vector<const TreeAccessor*> accessors;
  
  while (f >> fileName) {
    //gSystem->Exec("free");
    const TreeAccessor* accessor = TreeAccessor::open(fileName.c_str());
    if (!accessor) {
      cout << "Skipping invalid file " << fileName << endl;
      continue;
    }
    cout << std::setw(2) << ++i << " - " << fileName << endl;
    accessors.push_back(accessor);
  }
  
  if (accessors.empty()) return nullptr;
  return new MultiTreeAccessor(accessors);
}


MultiTreeAccessor* MultiTreeAccessor::openWild(const TString& wcName) 
{
  // Piggyback on TChain wildcarding feature...
  TChain chain("");
  chain.Add(wcName);
  
  std::vector<const TreeAccessor*> accessors;
  
  for (int i = 0; i < chain.GetListOfFiles()->GetEntries(); i++) {
    std::string fileName = chain.GetListOfFiles()->At(i)->GetTitle();
    const TreeAccessor* accessor = TreeAccessor::open(fileName.c_str());
    if (!accessor) {
      cout << "Skipping invalid file " << fileName << endl;
      continue;
    }
    cout << std::setw(2) << i+1 << " - " << fileName <<  " , nEvents = " << accessor->nEvents() << ", nRuns = " << accessor->nRuns() << endl;
    accessors.push_back(accessor);
  }
  
  if (accessors.empty()) return nullptr;
  return new MultiTreeAccessor(accessors);
}


MultiTreeAccessor::~MultiTreeAccessor()
{
  for (const TreeAccessor* accessor : m_accessors)
    delete accessor;
}


const EventData* MultiTreeAccessor::eventData(unsigned int i) const
{
  unsigned int nEventsSoFar = 0;
  for (const TreeAccessor* accessor : m_accessors) {
    unsigned int n = accessor->nEvents();
    if (i < nEventsSoFar + n) return accessor->eventData(i - nEventsSoFar);
    nEventsSoFar += n;
  }
  return nullptr;
}


const RunData* MultiTreeAccessor::runData(unsigned int i) const 
{ 
  unsigned int nRunsSoFar = 0;
  for (const TreeAccessor* accessor : m_accessors) {
    unsigned int n = accessor->nRuns();
    if (i < nRunsSoFar + n) return accessor->runData(i - nRunsSoFar);
    nRunsSoFar += n;
  }
  return nullptr;
}


unsigned int MultiTreeAccessor::nEvents() const
{
  unsigned int n = 0;
  for (const TreeAccessor* accessor : m_accessors)
     n += accessor->nEvents();
  return n;
}


unsigned int MultiTreeAccessor::nRuns() const
{
  unsigned int n = 0;
  for (const TreeAccessor* accessor : m_accessors)
     n += accessor->nRuns();
  return n;
}


unsigned int MultiTreeAccessor::historySize(unsigned int i) const
{
  resetCache();
  unsigned int size = 0;
  for (const TreeAccessor* accessor : m_accessors) {
    const HistoryContainer* cont = accessor->historyContainer(i);
    if (cont) size += cont->nDataContainers();
  }
  return size;
}


const History* MultiTreeAccessor::getCellHistory(unsigned int i) const 
{ 
  CellInfo* cellInfo = nullptr;
  std::vector<const Data*> allData;
  std::vector<const EventData*> allEventData;
  for (const TreeAccessor* accessor : m_accessors) {
    //cout << "---> Getting history for a treeAccessor..." << endl; 
    const History* thisHistory = accessor->getCellHistory(i);
    //cout << "---> done Getting history for a treeAccessor..." << endl; 
    if (!thisHistory) continue;
    if (!cellInfo) {
      cellInfo = new CellInfo(*thisHistory->cellInfo());
    //cout << "---> done new cell info" << endl; 
    }
    //cout << "---> Creating new event data N = " << thisHistory->eventData().size() << endl; 
    const std::vector<const EventData*>& thisEventData = thisHistory->eventData();
    std::map<const EventData*, const EventData*> eventMap;
    for (const EventData* event : thisEventData) {
      if (eventMap.find(event) != eventMap.end()) continue;
      EventData* newED = new EventData(*event);
      eventMap[event] = newED;
      allEventData.push_back(newED);
    }
    //cout << "---> Creating new data N = " << thisHistory->nData() << endl; 
   
    for (unsigned int i = 0; i < thisHistory->nData(); i++) {
     //cout << "------> Creating new data " << i << endl; 
      allData.push_back(new Data(*thisHistory->data(i), eventMap[thisHistory->data(i)->eventData()], nullptr, -1));
     //cout << "------> done Creating new data " << i << endl; 
      if (!cellInfo->shape(thisHistory->data(i)->gain())) {
        const ShapeInfo* thisShape = thisHistory->cellInfo()->shape(thisHistory->data(i)->gain());
        cellInfo->setShape(thisHistory->data(i)->gain(), thisShape ? new ShapeInfo(*thisShape) : nullptr);
      }
     //cout << "------> done shape " << i << endl; 
    }
     //cout << "---> done Creating new data, deleting treeAcc history" << endl; 
    delete thisHistory;
  }
  //cout << "--->returning new history..." << endl; 
  return (cellInfo ? new History(allData, *cellInfo, allEventData, i) : nullptr); 
}
      
        
const CellInfo* MultiTreeAccessor::getCellInfo(unsigned int i) const 
{
  resetCache();
  for (const TreeAccessor* accessor : m_accessors) {
    const HistoryContainer* cont = accessor->historyContainer(i);
    if (cont && cont->cellInfo()) return new CellInfo(*cont->cellInfo());
  }
  return nullptr;
}


bool MultiTreeAccessor::writeToFile(const TString& fileName) const
{
  std::vector<const Accessor*> accessors;
  for (unsigned int i = 0; i < m_accessors.size(); i++)
    accessors.push_back(m_accessors[i]);
  cout << "Merging data..." << endl;
  TreeAccessor* singleContainer = TreeAccessor::merge(accessors, fileName);
  if (!singleContainer) return false;
  delete singleContainer;
  return true;
}


std::vector<MultiTreeAccessor*> MultiTreeAccessor::filterComponents(const FilterList& filterList, const DataTweaker& tweaker) const
{
  std::vector< std::vector<const TreeAccessor*> > filteredAccessors(filterList.size()); 
  
  for (unsigned int i = 0; i < nAccessors(); i++) {
    const TreeAccessor* treeAcc = dynamic_cast<const TreeAccessor*>(&accessor(i));
    cout << "Processing data " << i << " of " << nAccessors();
    if (treeAcc) cout << " (fileName = " << treeAcc->fileName() << ")";
    cout << endl;
    FilterList thisFilterList;
    //
    for (unsigned int f = 0; f < filterList.size(); f++) {
      std::string pathname = (string)filterList.fileName(f);
      if( pathname.find("eos/atlas/") < pathname.length() ){
	int nslashes = 0, slpos = 0;
	for( int k1 = 0; k1 < (int)pathname.length(); k1++ ){
	  if( nslashes > 2 )break;
	  if( pathname[k1] != '/' )continue;
	  nslashes++;
	  slpos = k1;
	}
	pathname = pathname.substr( 0, slpos );
      }
      TString thisFN = Form("%s_filter%d.root", pathname.c_str(), i );//filterList.fileName(f).Data(), i);
      //if (treeAcc) thisFN = treeAcc->fileName() + Form("_filter%d.root", f);
      thisFilterList.add(filterList.filterParams(f), thisFN);
    }
    std::vector<TreeAccessor*> filteredTreeAccs = TreeAccessor::filter(accessor(i), thisFilterList, tweaker);
    if (filteredTreeAccs.size() != filterList.size()) {
      cout << "Filtering failed, exiting" << endl;
      return std::vector<MultiTreeAccessor*>();
    }
    for (unsigned int f = 0; f < filteredTreeAccs.size(); f++) filteredAccessors[f].push_back(filteredTreeAccs[f]);
    //if (treeAcc) treeAcc->resetCache(); // to save memory
  }

  std::vector<MultiTreeAccessor*> result;
  for (unsigned int f = 0; f < filteredAccessors.size(); f++) result.push_back(new MultiTreeAccessor(filteredAccessors[f]));
  return result;
}

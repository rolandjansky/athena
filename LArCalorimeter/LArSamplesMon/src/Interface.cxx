/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Interface.h"

#include "LArSamplesMon/TreeAccessor.h"
#include "LArSamplesMon/MultiTreeAccessor.h"
#include "LArCafJobs/SimpleShape.h"
#include "LArCafJobs/ShapeInfo.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/RunData.h"
#include "LArSamplesMon/FilterParams.h"
#include "LArSamplesMon/DataTweaker.h"
#include "LArSamplesMon/UniformShapeErrorGetter.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <iomanip>
#include <fstream>
#include <map>

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


Interface* Interface::open(const TString& fileName) 
{
  TreeAccessor* accessor = TreeAccessor::open(fileName);
  return (accessor ? new Interface(*accessor) : nullptr);
}


Interface* Interface::open(const std::vector<TString>& fileNames) 
{
  MultiTreeAccessor* accessor = MultiTreeAccessor::open(fileNames);
  return (accessor ? new Interface(*accessor) : nullptr);
}


Interface* Interface::openList(const TString& fileList) 
{
  MultiTreeAccessor* accessor = MultiTreeAccessor::openList(fileList);
  return (accessor ? new Interface(*accessor) : nullptr);
}


Interface* Interface::openWild(const TString& wcName) 
{
  MultiTreeAccessor* accessor = MultiTreeAccessor::openWild(wcName);
  return (accessor ? new Interface(*accessor) : nullptr);
}


Interface::~Interface() 
{ 
  if (m_ownShapeErrorGetter) delete m_shapeErrorGetter;
  delete m_accessor; 

  for (std::vector<unsigned int>* neighbors : m_neighborCache)
    delete neighbors;
}


void Interface::setShapeErrorGetter(const AbsShapeErrorGetter* err)
{
  if (m_ownShapeErrorGetter) delete m_shapeErrorGetter;
  m_shapeErrorGetter = err;
  m_ownShapeErrorGetter = false;
}


void Interface::setShapeError(double k)
{
  if (m_ownShapeErrorGetter) delete m_shapeErrorGetter;
  if (k == 0) { m_shapeErrorGetter = nullptr; m_ownShapeErrorGetter = false; return; }
  m_shapeErrorGetter = new UniformShapeErrorGetter(k);
  m_ownShapeErrorGetter = true;
}


void Interface::setShapeError(const TString& fileName)
{
  setShapeErrorGetter(new TreeShapeErrorGetter(fileName));
}


unsigned int Interface::size() const
{
  unsigned int n = 0;
  for (unsigned int i = 0; i < nChannels(); i++)
    n += accessor().historySize(i);
  return n;
}


const History* Interface::getCellHistory(unsigned int i) const 
{ 
  const History* history = accessor().getCellHistory(i);
  if (history) {
    history->setShapeErrorGetter(m_shapeErrorGetter);
    history->setInterface(this);
  }
  return history;
}


const History* Interface::cellHistory(unsigned int i) const 
{ 
  const History* history = accessor().cellHistory(i);
  if (history) {
    history->setShapeErrorGetter(m_shapeErrorGetter);
    history->setInterface(this);
  }
  return history;
}


const CellInfo* Interface::getCellInfo(unsigned int i) const 
{ 
  return accessor().getCellInfo(i);
}


unsigned int Interface::nFilledChannels() const
{
  unsigned int n = 0;
  for (unsigned int i = 0; i < nChannels(); i++)
    n += (accessor().historySize(i) ? 1 : 0);
  return n;
}


void Interface::printFilledRanges(unsigned int skip) const
{
  int i1 = 0, i2 = 0;
  unsigned int nNull = 0;
  unsigned int size = accessor().historySize(0);
  bool ok = (size > 0);
  bool inInterval = ok;

  for (unsigned int i = 0; i < nChannels(); i++) {
    size = accessor().historySize(i);
    if (inInterval && nNull == 0) i2 = i; 
    bool ok = (size > 0);
    if (!ok && inInterval) {
      nNull++;
      if (nNull > skip) {
        nNull = 0;
        inInterval = false;
        cout << i1 << "-" << i2 << endl;
      }
    }
    if (ok) nNull = 0;
    if (ok && !inInterval) {
      inInterval = true;
      i1 = i;
    }
  }

  if (inInterval) cout << i1 << "-" << nChannels() << endl;
}


HistoryIterator Interface::begin(unsigned int pos, double eMin, double adcMaxMin) const
{
  return HistoryIterator(*this, pos, eMin, adcMaxMin);
}


bool Interface::highEData(double eCut, TArrayI& hashes, TArrayI& indices) const
{
  std::vector<unsigned int> hashV, indexV;
  unsigned int nTot = 0;
  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    for (unsigned int j = 0; j < history->nData(); j++) {
      const Data& data = *history->data(j);
      nTot++;
      if (data.isDisconnected()) continue;
      if (nTot % 10000 == 0) cout << nTot << endl;
      if (data.energy() > eCut) {
        cout << "E = " << data.energy() << " " << i << " " << j << endl;
        hashV.push_back(i);
        indexV.push_back(j);
      }
    }
  }
  
  hashes.Set(hashV.size());
  indices.Set(indexV.size());
  
  for (unsigned int i = 0; i < hashV.size(); i++) {
    hashes[i] = hashV[i];
    indices[i] = indexV[i];
  }
  
  cout << hashV.size() << "/" << nTot << endl;  
  return true;
}


bool Interface::isValid() const
{  
  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history) continue;
    if (!history->isValid()) { 
      cout << "Invalid LArSamplesHistory at hash = " << i << endl;
      return false;
    }
  }
  
  return true;
}


Interface* Interface::merge(const Interface& other, const TString& fileName) const
{
  std::vector<const Interface*> interfaces;
  interfaces.push_back(this);
  interfaces.push_back(&other);
  return merge(interfaces, fileName);
}


Interface* Interface::merge(const std::vector<const Interface*>& interfaces, const TString& fileName)
{  
  std::vector<const Accessor*> accessors;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    accessors.push_back(&interfaces[i]->accessor());
  TreeAccessor* newAccessor = TreeAccessor::merge(accessors, fileName);
  return new Interface(*newAccessor);
}

Interface* Interface::merge(const Interface& other, const TString& fileName, const TString& LBFile) const
{
  std::vector<const Interface*> interfaces;
  interfaces.push_back(this);
  interfaces.push_back(&other);
  return merge(interfaces, fileName, LBFile);
}


Interface* Interface::merge(const std::vector<const Interface*>& interfaces, const TString& fileName, const TString& LBFile)
{  
  std::vector<const Accessor*> accessors;
  for (unsigned int i = 0; i < interfaces.size(); i++)
    accessors.push_back(&interfaces[i]->accessor());
  TreeAccessor* newAccessor = TreeAccessor::merge(accessors, fileName, LBFile);
  return new Interface(*newAccessor);
}


Interface* Interface::merge(const TString& listFileName, const TString& fileName)
{
  Interface* multi = openList(listFileName);
  if (!multi) return nullptr;
  std::vector<const Interface*> justOne;
  justOne.push_back(multi);
  return merge(justOne, fileName);
}

Interface* Interface::merge(const TString& listFileName, const TString& fileName, const TString& LBFile)
{
  Interface* multi = openList(listFileName);
  if (!multi) return nullptr;
  std::vector<const Interface*> justOne;
  justOne.push_back(multi);
  return merge(justOne, fileName, LBFile);
}


bool Interface::filterAndMerge(const TString& listFileName, const TString& outFile, const TString& filters, const TString& tweaks)
{
  FilterList filterList;
  
  TObjArray* list = filters.Tokenize(",;");
  if (list->GetEntries() == 0) {
    cout << "No filtering specified, exiting.";
    delete list;
    return 0;
  }
  
  for (int k = 0; k < list->GetEntries(); k++) {
    TObjString* tobs = (TObjString*)(list->At(k));
    TObjArray* items = tobs->String().Tokenize(":");
    if (items->GetEntries() != 2) {
      cout << "Invalid filter entry " << tobs->String() << ", exiting." << endl;
      delete list;
      delete items;
      return 0;
    }
    TString params = ((TObjString*)(items->At(0)))->String();
    TString suffix = ((TObjString*)(items->At(1)))->String();    
    FilterParams f;
    if (!f.set(params)) return 0;
    cout << "---" << endl;
    filterList.add(f, addSuffix(outFile, suffix));
    delete items;
  }
  delete list;

  
  DataTweaker tweak;
  if (!tweak.set(tweaks)) return 0;

  Interface* multi = openList(listFileName);
  if (!multi) return 0;
  const MultiTreeAccessor* mt = dynamic_cast<const MultiTreeAccessor*>(&multi->accessor());
  if (!mt) return 0;
  std::vector<MultiTreeAccessor*> filtered_mts = mt->filterComponents(filterList, tweak);
  if (filtered_mts.size() != filterList.size()) return 0;
  delete multi;
  cout << "Component filtering done!" << endl;
  // The following line should work, but doesn't... so the block of code below replaces it.
  //Interface* filtered_multi = new Interface(*filtered_mt);
  //
  for (unsigned int f = 0; f < filtered_mts.size(); f++) {
    std::vector<TString> files; 
    for (unsigned int i = 0; i < filtered_mts[f]->nAccessors(); i++) {
      files.push_back(((const TreeAccessor*)&filtered_mts[f]->accessor(i))->fileName());
      cout << "Added " << files.back() << endl;
    }
    delete filtered_mts[f];
    Interface* filtered_multi = open(files);
    //
    std::vector<const Interface*> justOne;
    justOne.push_back(filtered_multi);
    Interface* interface = merge(justOne, filterList.fileName(f));
    delete interface;
  }
  
  return true;
}


Interface* Interface::filter(const TString& sel, const TString& fileName, const TString& tweaks) const
{
  FilterParams f;
  if (!f.set(sel)) return nullptr;

  DataTweaker tweak;
  if (!tweak.set(tweaks)) return nullptr;
 
  TString thisFN = fileName;
  if (thisFN.Index(".root") < 0 && dynamic_cast<const TreeAccessor*>(&accessor())) {
    TString newFN = addSuffix(dynamic_cast<const TreeAccessor*>(&accessor())->fileName(), fileName);
    if (newFN != "") thisFN = newFN;
  }
  return filter(f, tweak, thisFN);
}


TString Interface::addSuffix(const TString& fileName, const TString& suffix)
{
  TString rootName = fileName;
  if (fileName(fileName.Length() - 5, 5) == ".root") rootName = fileName(0, fileName.Length() - 5);
  
  return  rootName + "_" + suffix + ".root";
}


Interface* Interface::filter(const FilterParams& filterParams, const DataTweaker& tweaker, const TString& fileName) const
{
  TreeAccessor* newAcc = TreeAccessor::filter(accessor(), filterParams, fileName, tweaker);
  return new Interface(*newAcc);
}


Interface* Interface::makeTemplate(const TString& fileName) const
{
  TreeAccessor* newAcc = TreeAccessor::makeTemplate(accessor(), fileName);
  return new Interface(*newAcc);
}


Interface* Interface::refit(const TString& newFileName, Chi2Params pars) const
{
  FilterParams f;
  DataTweaker tw;
  tw.setRefit(true);
  tw.setFitParams(pars);
  return filter(f, tw, newFileName);
}


HistoryIterator Interface::findEtaPhi(CaloId calo, short layer, short iEta, short iPhi, short region) const
{
  for (unsigned int i = 0; i < nChannels(); i++) {
    const CellInfo* info = cellInfo(i);
    if (!info) continue;
    if (!Id::matchCalo(info->calo(), calo)) continue;
    if (info->layer()  != layer) continue;
    if (info->iEta()   != iEta) continue;
    if (info->iPhi()   != iPhi) continue;
    if (info->region() != region) continue;
    return HistoryIterator(*this, i);
  }
  
  return HistoryIterator(*this, end());
}


HistoryIterator Interface::findFebChannel(CaloId calo, short feb, short channel) const
{
  for (unsigned int i = 0; i < nChannels(); i++) {
    const CellInfo* info = cellInfo(i);
    if (!info) continue;
    if (!Id::matchCalo(info->calo(), calo)) continue;
    if (info->feb()     != feb)     continue;
    if (info->channel() != channel) continue;
    return HistoryIterator(*this, i);
  }
  
  return HistoryIterator(*this, end());
}


HistoryIterator Interface::findFTSlotChannel(CaloId calo, short ft, short slot, short channel) const
{
  for (unsigned int i = 0; i < nChannels(); i++) {
    const CellInfo* info = cellInfo(i);
    if (!info) continue;
    if (!Id::matchCalo(info->calo(), calo))    continue;
    if (ft >= 0      && info->feedThrough() != ft)      continue;
    if (slot >= 0    && info->slot()        != slot)    continue;
    if (channel >= 0 && info->channel()     != channel) continue;
    return HistoryIterator(*this, i);
  }
  
  return HistoryIterator(*this, end());
}
      
      
TH1D* Interface::Draw(const TString& var, int nBins, double xMin, double xMax, const TString& sel, const TString& opt) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return nullptr;
  
  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(var, vars, funcs, args) || funcs.size() != 1) {
    cout << "Invalid variable specification " << var << endl;
    return nullptr;
  }

  TString title = vars[0];
  if (TString(sel) != "") title = title + ", " + sel;
  
  TH1D* h = m.dist(funcs[0], args[0], vars[0], nBins, xMin, xMax, 
		   title, vars[0], "digits", f);
  if (!h) return nullptr;
  h->Draw(opt);
  return h;
}
      
      
TH2D* Interface::Draw(const TString& varList, int nBinsX, double xMin, double xMax, 
				  int nBinsY, double yMin, double yMax, 
				  const TString& sel, const TString& opt) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return nullptr;
  
  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(varList, vars, funcs, args) || funcs.size() != 2) {
    cout << "Invalid variable specification " << varList << endl;
    return nullptr;
  }

  TString title = vars[1] + " vs. " + vars[0];

  if (TString(sel) != "") title = title + ", " + sel;
  TH2D* h = m.dist(funcs[0], args[0], funcs[1], args[1], vars[0] + "_" + vars[1], 
		   nBinsX, xMin, xMax,  nBinsY, yMin, yMax, 
		   title, vars[0], vars[1], f);
  if (!h) return nullptr;
  h->Draw(opt);
  return h;
}
      
      
TH2D* Interface::DrawPartition(PartitionId partition, const TString& var, 
					   const TString& sel, const TString& opt,
					   CombinationType comb) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return nullptr;

  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(var, vars, funcs, args) || funcs.size() != 1) {
    cout << "Invalid variable specification " << var << endl;
    return nullptr;
  }

  TString title = var;
  title = title + ", " + Id::str(partition);
  if (TString(sel) != "") title = title + ", " + sel;

  TH2D* h = m.partitionMap(funcs[0], args[0], var, partition, title, comb, f);
  if (!h) return nullptr;
  h->Draw(opt);
  return h;
}
      
      
TH2D* Interface::DrawEtaPhi(CaloId calo, short layer, const TString& var, 
					const TString& sel, const TString& opt,
					CombinationType comb) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return nullptr;

  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(var, vars, funcs, args) || funcs.size() != 1) {
    cout << "Invalid variable specification " << var << endl;
    return nullptr;
  }
  TString title = var;
  title += Form(", %s, layer %d", Id::str(calo).Data(), layer);
  if (TString(sel) != "") title = title + ", " + sel;
 
  TH2D* h = m.etaPhiMap(funcs[0], args[0], var, calo, layer, title, comb, f);
  if (!h) return nullptr;
  h->Draw(opt);
  return h;
}


bool Interface::Scan(const TString& vars, const TString& sel, unsigned int verbosity) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return 0;
  return m.dump(vars, f, verbosity);
}


bool Interface::Scan(const TString& vars, CombinationType comb, const TString& sel, const TString& ranges, unsigned int verbosity) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return 0;
  return m.dump(vars, comb, f, ranges, verbosity);
}


bool Interface::Show(unsigned int hash, unsigned int verbosity) const
{
  const History* history = cellHistory(hash);
  if (!history) return false;
  cout << history->description(verbosity) << endl;
  return true;
}

bool Interface::Show(const TString& sel, unsigned int verbosity) const
{
  FilterParams f;
  if (!f.set(sel)) return false;
  for (unsigned int i = 0; i < nChannels(); i++) {  
    const History* history = pass(i, f);
    if (!history) continue;
    History* filtered = history->filter(sel);
    TString hDesc = filtered->description(verbosity);
    delete filtered;
    if (hDesc == "") continue;
    cout << Form("Hash = %-5d : ", i) << hDesc
         << "-----------------------------------------------------------------------------" 
         << endl;
  }
  return true;
}


bool Interface::ShowEvents(const TString& sel, unsigned int verbosity) const
{
  FilterParams f;
  if (!f.set(sel)) return false;
  std::map< std::pair<unsigned int, unsigned int>, unsigned int > eventCells;
  std::map< std::pair<unsigned int, unsigned int>, double > eventEnergy;

  if (verbosity & 8) {
    for (unsigned int i = 0; i < nChannels(); i++) {  
      if ((i+1) % 50000 == 0) cout << "Cell # " << i+1 << "/" << nChannels() << endl;
      const History* history = pass(i, f);
      if (!history) continue;
      for (unsigned int j = 0; j < history->nData(); j++) {
        std::pair<unsigned int, unsigned int> ev = std::make_pair(history->data(j)->run(), history->data(j)->event());
        eventCells[ev]++;
        eventEnergy[ev] += history->data(j)->energy();
      }
    }
  }
  for (unsigned int i = 0; i < nEvents(); i++) {
    const EventData* evtData = eventData(i);
    std::pair<unsigned int, unsigned int> id(evtData->run(), evtData->event());
    TString printout = Form("%d : ", i) + evtData->description(verbosity);
    if (verbosity & 8)
      printout += Form(" : %6d LAr hits, %7.1f MeV", eventCells[id], eventEnergy[id]);
    cout << printout << endl;
  }
  
  return true;
}


bool Interface::ShowRuns(unsigned int verbosity) const
{
  std::map<unsigned int, unsigned int> events;

  if (verbosity & 8) {
    for (unsigned int i = 0; i < nEvents(); i++)
      events[eventData(i)->run()]++;
  }
  
  for (unsigned int i = 0; i < nRuns(); i++) {
    const RunData* rData = runData(i);
    TString printout = rData->description(verbosity);
    if (verbosity & 8)
      printout += Form(" : %6d events", events[rData->run()]);
    cout << printout << endl;
  }
  
  return true;
}


bool Interface::ShowStats(const TString& varList, const TString& sel, bool withErrors) const
{
  MonitorBase m(*this);
  FilterParams f;
  if (!f.set(sel)) return 0;

  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(varList, vars, funcs, args)) {
    cout << "Invalid variable specification " << varList << endl;
    return 0;
  }
  
  TVectorD mean(vars.size()), meanErr(vars.size());
  TMatrixD covMatrix(vars.size(), vars.size()), covMatrixErr(vars.size(), vars.size());
  if (!m.statParams(funcs, args, mean, meanErr, covMatrix, covMatrixErr, f)) return false;
  
  if (!withErrors) {
    cout << "---------------------------" << endl;
    for (unsigned int i = 0; i < vars.size(); i++)
      cout << Form("| %10s | %-9.4g |", vars[i].Data(), mean(i)) << endl;
    cout << "---------------------------" << endl << endl;

    cout << "|            |";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "            |";
    cout << endl << "--------------";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "-------------";
    cout << endl;
    for (unsigned int i1 = 0; i1 < vars.size(); i1++) {
      cout << Form("| %10s |", vars[i1].Data());
      for (unsigned int i2 = 0; i2 < vars.size(); i2++)
        cout << Form(" %-9.4g |", covMatrix(i1, i2));
      cout << endl;
    }
    cout << "--------------";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "-------------";
    cout << endl;
  }
  else {
    cout << "---------------------------" << endl;
    for (unsigned int i = 0; i < vars.size(); i++)
      cout << Form("| %10s | %-9.4g +/- %-9.4g |", vars[i].Data(), mean(i), meanErr(i)) << endl;
    cout << "---------------------------" << endl << endl;

    cout << "|            |";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "                          |";
    cout << endl << "--------------";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "---------------------------";
    cout << endl;
    for (unsigned int i1 = 0; i1 < vars.size(); i1++) {
      cout << Form("| %10s |", vars[i1].Data());
      for (unsigned int i2 = 0; i2 < vars.size(); i2++)
        cout << Form(" %-9.4g +/- %-9.4g  |", covMatrix(i1, i2), covMatrixErr(i1, i2));
      cout << endl;
    }
    cout << "--------------";
    for (unsigned int i = 0; i < vars.size(); i++) cout << "---------------------------";
    cout << endl;
  }
  
  return true;
}


bool Interface::neighbors(const CellInfo& cell, double dRCut, std::vector<unsigned int>& hashes) const
{
  for (unsigned int i = 0; i < nChannels(); i++) {
    const CellInfo* otherCell = cellInfo(i);
    if (!otherCell) continue;
    if (cell.position().DeltaR(otherCell->position()) > dRCut) { delete otherCell; continue; }
    //cout << "Adding hash = " << i << " " << otherCell->location(3) << endl;
    hashes.push_back(i);
    delete otherCell;
  }  
  return true;
}


bool Interface::firstNeighbors(unsigned int hash, std::vector<unsigned int>& hashes, short layer) const
{
  const CellInfo* cell = cellInfo(hash);
  if (!cell) return true;
  if (!Id::matchCalo(cell->calo(), HEC)) return false; // for now!  
  if (layer < 0) return true;
  std::vector<unsigned int> allHashes;
  const std::vector<unsigned int>* cache = m_neighborCache[hash];
  if (cache) 
    allHashes = *cache;
  else {
    if (!neighbors(*cell, 0.15, allHashes)) return false;
    m_neighborCache[hash] = new std::vector<unsigned int>(allHashes);
  }
  if (layer == -2) { hashes = allHashes; return true; }
  for (unsigned int h : allHashes) {
    const CellInfo* info = cellInfo(h);
    if (!info) continue;
    if (info->layer() == layer) hashes.push_back(h);
    delete info;
  }
  return true;
}


bool Interface::data(const std::vector<unsigned int>& hashes,const EventData& event, std::vector<const Data*>& data) const
{
  if (hashes != m_neighborHistoryPos) {
    for (const History* history : m_neighborHistories)
      delete history;
    m_neighborHistories.clear();
    m_neighborHistoryPos.clear();
    for (std::vector<unsigned int>::const_iterator hash = hashes.begin(); hash != hashes.end(); hash++) {
      const History* history = AbsLArCells::newCellHistory(*hash);// bypasses history caching in order not to invalidate cell
      m_neighborHistories.push_back(history);
      m_neighborHistoryPos.push_back(*hash);
    }
  }

  for (const History* history : m_neighborHistories) {
    if (!history) continue;
    const Data* dataForEvent = history->data_for_event(event);
    if (dataForEvent) data.push_back(new Data(*dataForEvent));
  }
  return true;
}


bool Interface::dumpEventTuple(const TString& variables, const TString& fileName) const
{
  std::vector<float*> floatVars;
  std::vector<int*> intVars;
  std::vector<std::vector<float>*> floatVects;
  std::vector<std::vector<int>*> intVects;
  std::map<TString, unsigned int> varIndex;
 
  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  if (!MonitorBase::parseVariables(variables, vars, funcs, args)) return false;

  cout << "Making trees..." << endl;

  TFile* flatFile = TFile::Open(fileName + "_tmpFlatFile.root", "RECREATE");
  TTree* flatTree = new TTree("flatTree", "Flat tree");
  
  TFile* eventFile = TFile::Open(fileName, "RECREATE");
  TTree* eventTree = new TTree("eventTree", "Event tree");
  
  for (unsigned int j = 0; j < vars.size(); j++) {
    unsigned int index = 0;
    if (funcs[j].isNull()) return false;
    if (funcs[j].isInt()) {
      int* varCont = new int(0);
      std::vector<int>* vectCont = new std::vector<int>();      
      index = intVars.size();
      intVars.push_back(varCont);
      intVects.push_back(vectCont);
      flatTree->Branch(vars[j], varCont);
      eventTree->Branch(vars[j], vectCont);
    }
    else {
      float* varCont = new float(0);
      std::vector<float>* vectCont = new std::vector<float>();      
      index = floatVars.size();
      floatVars.push_back(varCont);
      floatVects.push_back(vectCont);
      flatTree->Branch(vars[j], varCont);
      eventTree->Branch(vars[j], vectCont);
    }
    varIndex[vars[j]] = index;
    cout << vars[j] << " -> " << index << endl;
  }
  std::map< unsigned int, std::map< unsigned int, std::vector<long long> > > runEventIndices;
  cout << "Making flat ntuple" << endl;
  unsigned int count = 0;
  for (HistoryIterator iter = begin(); iter.isValid(); iter.next()) {
    const History* hist = iter.history();
    count++;
    if (count % 100 == 0) cout << "Processing entry " << count << " (hash = " << iter.pos() << "), size = " << hist->nData() << endl;
    for (unsigned int k = 0; k < hist->nData(); k++) {
      for (unsigned int j = 0; j < vars.size(); j++) {
        if (funcs[j].isInt())
          *intVars[varIndex[vars[j]]] = int(funcs[j].intVal(*hist->data(k), args[j]));
        else
          *floatVars[varIndex[vars[j]]] = funcs[j].doubleVal(*hist->data(k), args[j]);
      }
      runEventIndices[hist->data(k)->run()][hist->data(k)->event()].push_back(flatTree->GetEntries());
      flatTree->Fill();
    }
  }
  
  cout << "Making event tuple" << endl;
  unsigned int runCount = 0;
  for (const auto& run : runEventIndices) {
    runCount++;
    cout << "Processing run " << run.first << " (" << runCount << " of " << runEventIndices.size() << ")" << endl;
    unsigned int eventCount = 0;
    for (const auto& event : run.second) {
      eventCount++;
      if (eventCount % 1000 == 0) 
        cout << "  processing event " << event.first << " (" << eventCount << " of " << run.second.size() << "), size = " << event.second.size() << endl;
      for (unsigned int j = 0; j < intVects.size(); j++) intVects[j]->clear();
      for (unsigned int j = 0; j < floatVects.size(); j++) floatVects[j]->clear();
      for (long long index : event.second) {
        flatTree->GetEntry(index);
       for (unsigned int j = 0; j < vars.size(); j++) {
         if (funcs[j].isInt())
           intVects[varIndex[vars[j]]]->push_back(*intVars[varIndex[vars[j]]]);
         else
           floatVects[varIndex[vars[j]]]->push_back(*floatVars[varIndex[vars[j]]]);
        }
      }
      eventTree->Fill();
    }
  }
  
  cout << "Writing data..." << endl;  
  flatFile->cd();
  flatTree->Write();
  eventFile->cd();
  eventTree->Write();  
  
  cout << "Cleaning up..." << endl;
  delete eventTree;
  delete eventFile;
  
  delete flatTree;
  delete flatFile;
  for (unsigned int j = 0; j < intVects.size(); j++) delete intVects[j];
  for (unsigned int j = 0; j < floatVects.size(); j++) delete floatVects[j];      
  for (unsigned int j = 0; j < intVars.size(); j++) delete intVars[j];
  for (unsigned int j = 0; j < floatVars.size(); j++) delete floatVars[j];      
  
  cout << "Done!" << endl;
  return true;
}

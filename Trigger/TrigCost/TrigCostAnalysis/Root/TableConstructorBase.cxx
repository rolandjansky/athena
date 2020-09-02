/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "../TrigCostAnalysis/TableConstructorBase.h"

#include <sstream>

#include "TFile.h"
#include "TH1.h"
#include "TKey.h"

TableConstructorBase::TableConstructorBase(const std::string& name) : 
  m_msgStream(nullptr, name.c_str()),
  m_expectedHistograms(),
  m_histogramMap(),
  m_baseEntry() {
}


TH1* TableConstructorBase::hist(const std::string& name) const {
  const auto mapIt = m_histogramMap.find(name);
  if (mapIt != m_histogramMap.end()) {
    return mapIt->second;
  }
  throw std::runtime_error("Unable to get histogram " + name);
  return nullptr;
}


std::string TableConstructorBase::getTable(TFile* file, const TString& rootDir, const float walltime) {

  std::vector<TableEntry> tableEntries;

  TIter nextKey( gDirectory->GetListOfKeys() );
  TKey* key = nullptr;
  while ( (key = (TKey*)nextKey()) ) {
    TObject* obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
      TString histDir = rootDir + obj->GetName();
      file->cd(histDir);
      tableEntries.push_back( getTableEntry(obj->GetName()) );
    } else {
      msg() << MSG::ERROR << "Expecting one TDirectory per entry" << endmsg;
    }
  }

  tablePostProcessing(tableEntries, walltime);

  std::stringstream ss;
  ss << tableEntries.front().getHeaderRow();

  for (const TableEntry& te : tableEntries) {
    ss << te.getRow();
  }
  return ss.str();
}


void TableConstructorBase::addExpectedHistogram(const TString& name) {
  m_expectedHistograms.insert(name);
}


TableEntry& TableConstructorBase::getBaseEntry() {
  return m_baseEntry;
}


void TableConstructorBase::getHistograms(const std::string& name) {
  m_histogramMap.clear();
  TIter nextKey( gDirectory->GetListOfKeys() );
  TKey* key = nullptr;
  while ( (key = (TKey*)nextKey()) ) {
    TObject* obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
      bool found = false;
      for (const TString exp : m_expectedHistograms) {
        const TString objName( obj->GetName() );
        const TString expName = TString(name) + "_" + exp;
        if (objName.EndsWith(expName)) {
          m_histogramMap.insert( std::make_pair(std::string(exp), dynamic_cast<TH1*>(obj)) );
          found = true;
          break;
        }
      }
      if (!found) {
        msg() << MSG::ERROR << "Was not expecting histogram " << obj->GetName() << endmsg;
        for (const TString exp : m_expectedHistograms) {
          const TString expName = TString(name) + "_" + exp;
          msg() << MSG::ERROR << "  -- Expected " << expName << endmsg;
        }
      }
    } else {
      msg() << MSG::ERROR << "Expecting only Histograms, found " << obj->GetName() << endmsg;
    }
  }
  if (m_histogramMap.size() != m_expectedHistograms.size()) {
    msg() << MSG::ERROR << "Did not find expected number of histograms" << endmsg;
  }
}


float TableConstructorBase::histGetXWeightedIntegral(const std::string& name, bool isLog) {
  TH1* histo = hist(name);
  float total = 0;
  for (int i = 1; i <= histo->GetNbinsX(); ++i) {
    if (isLog) {
      total += histo->GetBinContent(i) * histo->GetXaxis()->GetBinCenterLog(i);
    } else {
      total += histo->GetBinContent(i) * histo->GetXaxis()->GetBinCenter(i);
    }
  }
  return total;
} 


MsgStream& TableConstructorBase::msg() const {
  return m_msgStream;
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/AbsLArCells.h"

#include "LArSamplesMon/History.h"
#include "LArCafJobs/CellInfo.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


//std::vector<CellInfo*> AbsLArCells::m_cellInfoCache(Definitions::nChannels,nullptr);

AbsLArCells::AbsLArCells():
  m_pos(nChannels() + 1), 
  m_cellCache(nullptr),  
  m_cellInfoCache(nChannels(),nullptr)
{ }

AbsLArCells::~AbsLArCells()
{
  AbsLArCells::resetCache();
  resetCellInfoCache();
}


void AbsLArCells::resetCache() const
{
  if (m_cellCache) {
    delete m_cellCache;
    m_cellCache = nullptr;
  }
  m_pos = nChannels() + 1;
}


const History* AbsLArCells::newCellHistory(unsigned int i) const 
{ 
  const History* history = getCellHistory(i);
  if (!history) return nullptr;
  if (!m_cellInfoCache[i]) {
    const CellInfo* ci=history->cellInfo();
    if (ci) {
      m_cellInfoCache[i]=new CellInfo(*ci,false);
    }
  }
  //  m_cellInfoCache[i] = (history->cellInfo() ? new CellInfo(*history->cellInfo(), false) : new CellInfo());
  return history;
}


const History* AbsLArCells::cellHistory(unsigned int i) const 
{ 
  if (m_pos == i) return m_cellCache;
  resetCache();
  const History* history = newCellHistory(i);
  if (!history) return nullptr;
  m_cellCache = history;
  m_pos = i;
  return m_cellCache;
}


const CellInfo* AbsLArCells::cellInfo(unsigned int i) const
{
  const CellInfo* info = cellInfoCache(i);
  if (info) return (info->isValid() ? new CellInfo(*info) : nullptr);
  info = getCellInfo(i);
  if (info)  m_cellInfoCache[i] =  new CellInfo(*info, false); 
  //m_cellInfoCache[i] = (info ? new CellInfo(*info, false) : new CellInfo());
  return info;
}


const CellInfo* AbsLArCells::cellInfoCache(unsigned int i) const
{
  return m_cellInfoCache[i];
}


const CellInfo* AbsLArCells::getCellInfo(unsigned int i) const
{
  const History* history = this->getCellHistory(i);
  if (!history) return nullptr;
  if (!history->cellInfo()) { delete history; return nullptr; }
  CellInfo* info = new CellInfo(*history->cellInfo());
  delete history;
  return info;
}


const History* AbsLArCells::pass(unsigned int i, const FilterParams& f) const 
{ 
  //std::cout << "Called AbsLArCells with hash " << i  << std::endl;
  if (!f.passHash(i)) return nullptr;
  const CellInfo* info = cellInfo(i);
  if (!info) {
    return nullptr;
  }
  //std::cout << "Called AbsLArCells::pass on a cell belonging to " << Id::str(info->calo()) << std::endl;
  bool result = f.passCell(*info);
  delete info;  
  return result ? cellHistory(i) : nullptr;
}


void AbsLArCells::resetCellInfoCache()
{
  unsigned int i = 0;
  for (std::vector<CellInfo*>::iterator cellInfo = m_cellInfoCache.begin();
       cellInfo != m_cellInfoCache.end(); ++cellInfo, i++)
    if (*cellInfo) {
      delete *cellInfo;
      *cellInfo = 0;
    }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/HistoryContainer.h"

#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ClassCounts.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace LArSamples;


HistoryContainer::HistoryContainer(CellInfo* cellInfo) 
  : m_cellInfo(cellInfo) 
{ 
  ClassCounts::incrementInstanceCount("HistoryContainer"); 
}

HistoryContainer::HistoryContainer(const HistoryContainer& other) 
  : m_data(other.m_data), 
    m_cellInfo(0)
{ 
  ClassCounts::incrementInstanceCount("HistoryContainer"); 
  if (other.m_cellInfo) m_cellInfo = new CellInfo(*other.m_cellInfo); 
}


HistoryContainer::~HistoryContainer()
{
  ClassCounts::decrementInstanceCount("HistoryContainer"); 
  for (std::vector<const DataContainer*>::iterator data = m_data.begin(); 
       data != m_data.end(); data++) 
    if (*data) delete *data;
  if (m_cellInfo) delete m_cellInfo;
}
  

double HistoryContainer::footprint() const
{
  double fp = sizeof(*this);
  if (m_cellInfo) fp += m_cellInfo->footprint();

  for (unsigned int i = 0; i < nDataContainers(); i++)
    fp += dataContainer(i)->footprint();

  return fp;
}


bool HistoryContainer::isValid() const
{
  if (!m_cellInfo || !m_cellInfo->isValid()) return false;
  if (nDataContainers() == 0) return false;
  
  for (std::vector<const DataContainer*>::const_iterator data = m_data.begin(); 
       data != m_data.end(); data++) 
    if (!(*data)->isValid()) return false;
  
  return true;
}

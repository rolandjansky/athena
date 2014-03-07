/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/ClassCounts.h"

#include <iostream>

using std::cout;
using std::endl;

using namespace LArSamples;

std::map<TString, int>* ClassCounts::m_counts = 0;

ClassCounts::ClassCounts(const TString& className)
  : m_className(className)
{
  counts()[className] = 0;
}


ClassCounts::~ClassCounts()
{
  m_counts->erase(className());
  if (m_counts->size() == 0) {
    delete m_counts;
    m_counts = 0;
  }
}


std::map<TString, int>& ClassCounts::counts()
{
  if (!m_counts) m_counts = new std::map<TString, int>();
  return *m_counts;
}


void ClassCounts::printCountsTable()
{
  cout << "Class instance counts : " << endl;
  if (!m_counts) return;
  for (std::map<TString, int>::const_iterator count = counts().begin();
       count != counts().end(); count++)
    cout << Form("%20s : %-d", count->first.Data(), count->second) << endl;
}

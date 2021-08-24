/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/FilterList.h"

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
    
using namespace LArSamples;


bool FilterList::pass(unsigned int hashId, const History& history, unsigned int k) const
{
  for (unsigned int i = 0; i < size(); i++)
    if (filterParams(i).pass(hashId, history, k)) return true;
  return false;
}


bool FilterList::passHash(unsigned int hashId) const
{
  for (unsigned int i = 0; i < size(); i++)
    if (filterParams(i).passHash(hashId)) return true;
  return false;
}


bool FilterList::passEvent(const Data& data) const
{
  for (unsigned int i = 0; i < size(); i++)
    if (filterParams(i).passEvent(data)) return true;
  return false;
}


bool FilterList::passCell(const CellInfo& info) const
{
  for (unsigned int i = 0; i < size(); i++)
    if (filterParams(i).passCell(info)) return true;
  return false;
}


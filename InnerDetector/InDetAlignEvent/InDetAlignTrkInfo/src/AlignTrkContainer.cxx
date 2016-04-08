/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignTrkContainer.cxx
// Richard Hawkings, started 13/4/04

#include <iostream>
#include "InDetAlignTrkInfo/AlignTrkContainer.h"

AlignTrkContainer::AlignTrkContainer()
  : DataVector<AlignTrk> () {}

AlignTrkContainer::~AlignTrkContainer() {}

void AlignTrkContainer::print() const
{
  std::cout << "Print contents of AlignTrkContainer" << std::endl;
  for (DataVector<AlignTrk>::const_iterator t=this->begin(); 
       t!=this->end(); ++t) (**t).print();
}

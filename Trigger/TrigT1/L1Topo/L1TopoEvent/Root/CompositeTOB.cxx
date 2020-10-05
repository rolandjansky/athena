// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/CompositeTOB.h"

thread_local TCS::Heap<TCS::CompositeTOB> TCS::CompositeTOB::fg_heap("Composite");

using namespace std;

TCS::CompositeTOB::CompositeTOB() : GenericTOB()
{}

TCS::CompositeTOB::CompositeTOB(const CompositeTOB & other) : GenericTOB(other)
{
   m_components.insert(m_components.end(), other.m_components.begin(), other.m_components.end());
}


TCS::CompositeTOB::CompositeTOB(const std::vector<GenericTOB*> & candidates)
{
   m_components.insert(m_components.end(), candidates.begin(), candidates.end());
}

TCS::CompositeTOB::CompositeTOB(GenericTOB* candidate) {
   m_components.push_back(candidate);
}

TCS::CompositeTOB::CompositeTOB(GenericTOB* candidate1, GenericTOB* candidate2) {
   m_components.push_back(candidate1);
   m_components.push_back(candidate2);
}

TCS::CompositeTOB::~CompositeTOB() = default;

TCS::CompositeTOB*
TCS::CompositeTOB::createOnHeap(const CompositeTOB& tob) {
   return fg_heap.create(tob);
}

void
TCS::CompositeTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::CompositeTOB::print(std::ostream &o) const {
   o << "composite tob (" << components().size() << " candidates): ";
   for(const_iterator c = components().begin();
       c != components().end(); ++c) {
      o << endl << "   " << **c;
   }
}

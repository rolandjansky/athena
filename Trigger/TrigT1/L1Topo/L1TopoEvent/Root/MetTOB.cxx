/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MetTOB.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoEvent/MetTOB.h"

unsigned int TCS::MetTOB::fg_instances = 0;
TCS::Heap<TCS::MetTOB> TCS::MetTOB::fg_heap("Met");

unsigned int TCS::MetTOB::g_nBitsEx = 15;
unsigned int TCS::MetTOB::g_nBitsEy = 15;
unsigned int TCS::MetTOB::g_nBitsEt = 15;

// default constructor
TCS::MetTOB::MetTOB() : BaseTOB(0) {
   ++fg_instances;
}

// constructor with initial values
TCS::MetTOB::MetTOB(int ex, int ey, unsigned int et) : BaseTOB(0)
   , m_Ex( sizeCheckM(ex, nBitsEx()) )
   , m_Ey( sizeCheckM(ey, nBitsEy()) )
   , m_Et( sizeCheck(et, nBitsEt()) )
{
   ++fg_instances;
}

// constructor with initial values
TCS::MetTOB::MetTOB(const MetTOB & met) : BaseTOB(0)
   , m_Ex( met.m_Ex )
   , m_Ey( met.m_Ey )
   , m_Et( met.m_Et )
{
   ++fg_instances;
}

// destructor
TCS::MetTOB::~MetTOB() {
   --fg_instances;
}


TCS::MetTOB*
TCS::MetTOB::createOnHeap(const MetTOB& cl) {
   return fg_heap.create(cl);
}

void
TCS::MetTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::MetTOB::print(std::ostream &o) const {
   o << "MET signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", sum Et: " << Et();
}

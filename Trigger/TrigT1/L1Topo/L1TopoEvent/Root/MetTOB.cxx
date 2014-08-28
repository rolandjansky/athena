//  MetTOB.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/MetTOB.h"

unsigned int TCS::MetTOB::fg_instances = 0;
TCS::Heap<TCS::MetTOB> TCS::MetTOB::fg_heap;

unsigned int TCS::MetTOB::g_nBitsEx = 15;
unsigned int TCS::MetTOB::g_nBitsEy = 15;

// default constructor
TCS::MetTOB::MetTOB() : BaseTOB() {
   ++fg_instances;
}

// constructor with initial values
TCS::MetTOB::MetTOB(int ex, int ey) : BaseTOB()
   , m_Ex( sizeCheck(ex, nBitsEx()) )
   , m_Ey( sizeCheck(ey, nBitsEy()) )
{
   ++fg_instances;
}

// constructor with initial values
TCS::MetTOB::MetTOB(const MetTOB & met) : BaseTOB()
   , m_Ex( met.m_Ex )
   , m_Ey( met.m_Ey )
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
    o << "MET signed Ex: " << Ex() << ", signed Ey: " << Ey();
}

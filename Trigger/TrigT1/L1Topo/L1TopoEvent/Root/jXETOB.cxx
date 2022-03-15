// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jXETOB.h"

thread_local TCS::Heap<TCS::jXETOB> TCS::jXETOB::fg_heap("jXETOB");

const unsigned int TCS::jXETOB::g_nBitsEx = 15;
const unsigned int TCS::jXETOB::g_nBitsEy = 15;
const unsigned int TCS::jXETOB::g_nBitsEt = 15;

// default constructor
TCS::jXETOB::jXETOB() : BaseTOB(0,"jXETOB")
{}

// constructor with initial values
TCS::jXETOB::jXETOB(int ex, int ey, unsigned int et) : BaseTOB(0,"jXETOB")
   , m_Ex( ex )
   , m_Ey( ey )
   , m_Et( et )
{}

// constructor with initial values
TCS::jXETOB::jXETOB(const jXETOB & met) : BaseTOB(0,"jXETOB")
   , m_Ex( met.m_Ex )
   , m_Ey( met.m_Ey )
   , m_Et( met.m_Et )
   , m_Et2( met.m_Et2)
{}

// destructor
TCS::jXETOB::~jXETOB() = default;

TCS::jXETOB*
TCS::jXETOB::createOnHeap(const jXETOB& cl) {
   return fg_heap.create(cl);
}

void
TCS::jXETOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::jXETOB::print(std::ostream &o) const {
   o << "jXE signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", sum Et: " << Et();
}

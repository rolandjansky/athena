// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jXETOB.h"

thread_local TCS::Heap<TCS::jXETOB> TCS::jXETOB::fg_heap("jXETOB");

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
TCS::jXETOB::jXETOB(const jXETOB & jxe) : BaseTOB(0,"jXETOB")
   , m_Ex( jxe.m_Ex )
   , m_Ey( jxe.m_Ey )
   , m_Et( jxe.m_Et )
   , m_Et2( jxe.m_Et2 )
   , m_ExDouble( jxe.m_ExDouble )
   , m_EyDouble( jxe.m_EyDouble )
   , m_EtDouble( jxe.m_EtDouble ) 
{}

// destructor
TCS::jXETOB::~jXETOB() = default;

TCS::jXETOB*
TCS::jXETOB::createOnHeap(const jXETOB& jxe) {
   return fg_heap.create(jxe);
}

void
TCS::jXETOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::jXETOB::print(std::ostream &o) const {
   o << "jXE signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", sum Et: " << Et();
}

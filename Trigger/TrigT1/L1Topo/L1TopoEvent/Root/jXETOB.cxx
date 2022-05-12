// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jXETOB.h"

thread_local TCS::Heap<TCS::jXETOB> TCS::jXETOB::fg_heap("jXETOB");

// default constructor
TCS::jXETOB::jXETOB() : BaseTOB(0,"jXETOB")
{}

// constructor with initial values
TCS::jXETOB::jXETOB(int ex, int ey, unsigned int et, inputTOBType_t tobType) : BaseTOB(0,"jXETOB")
   , m_Ex( ex )
   , m_Ey( ey )
   , m_Et( et )
   , m_tobType( tobType )
{}


// copy constructor
TCS::jXETOB::jXETOB(const TCS::jXETOB & jxe) = default;


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
   o << "jXE type: " << tobType() << ", signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", Et: " << Et();
}

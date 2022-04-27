// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gXETOB.h"

thread_local TCS::Heap<TCS::gXETOB> TCS::gXETOB::fg_heap("gXETOB");

// default constructor
TCS::gXETOB::gXETOB() : BaseTOB(0,"gXETOB")
{}

// constructor with initial values
TCS::gXETOB::gXETOB(int ex, int ey, unsigned int et, inputTOBType_t tobType) : BaseTOB(0,"gXETOB")
   , m_Ex( ex )
   , m_Ey( ey )
   , m_Et( et )
   , m_tobType( tobType )
{}


// copy constructor
TCS::gXETOB::gXETOB(const TCS::gXETOB & gxe) = default;


// destructor
TCS::gXETOB::~gXETOB() = default;

TCS::gXETOB*
TCS::gXETOB::createOnHeap(const gXETOB& gxe) {
   return fg_heap.create(gxe);
}

void
TCS::gXETOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::gXETOB::print(std::ostream &o) const {
   o << "gXE type: " << tobType() << ",  signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", Et: " << Et();
}

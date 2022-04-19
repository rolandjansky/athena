// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gXETOB.h"

thread_local TCS::Heap<TCS::gXETOB> TCS::gXETOB::fg_heap("gXETOB");

// default constructor
TCS::gXETOB::gXETOB() : BaseTOB(0,"gXETOB")
{}

// constructor with initial values
TCS::gXETOB::gXETOB(int ex, int ey, unsigned int et) : BaseTOB(0,"gXETOB")
   , m_Ex( ex )
   , m_Ey( ey )
   , m_Et( et )
{}

// constructor with initial values
TCS::gXETOB::gXETOB(const gXETOB & gxe) : BaseTOB(0,"gXETOB")
   , m_Ex( gxe.m_Ex )
   , m_Ey( gxe.m_Ey )
   , m_Et( gxe.m_Et )
   , m_Et2( gxe.m_Et2 )
   , m_ExDouble( gxe.m_ExDouble )
   , m_EyDouble( gxe.m_EyDouble )
   , m_EtDouble( gxe.m_EtDouble ) 
{}

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
   o << "gXE signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", Et: " << Et();
}

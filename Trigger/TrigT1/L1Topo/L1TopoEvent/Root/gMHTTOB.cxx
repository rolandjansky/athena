// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gMHTTOB.h"

thread_local TCS::Heap<TCS::gMHTTOB> TCS::gMHTTOB::fg_heap("gMHTTOB");

// default constructor
TCS::gMHTTOB::gMHTTOB() : BaseTOB(0,"gMHTTOB")
{}

// constructor with initial values
TCS::gMHTTOB::gMHTTOB(int ex, int ey, unsigned int et) : BaseTOB(0,"gMHTTOB")
   , m_Ex( ex )
   , m_Ey( ey )
   , m_Et( et )
{}

// constructor with initial values
TCS::gMHTTOB::gMHTTOB(const gMHTTOB & gmht) : BaseTOB(0,"gMHTTOB")
   , m_Ex( gmht.m_Ex )
   , m_Ey( gmht.m_Ey )
   , m_Et( gmht.m_Et )
   , m_Et2( gmht.m_Et2 )
   , m_ExDouble( gmht.m_ExDouble )
   , m_EyDouble( gmht.m_EyDouble )
   , m_EtDouble( gmht.m_EtDouble ) 
{}

// destructor
TCS::gMHTTOB::~gMHTTOB() = default;

TCS::gMHTTOB*
TCS::gMHTTOB::createOnHeap(const gMHTTOB& gmht) {
   return fg_heap.create(gmht);
}

void
TCS::gMHTTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::gMHTTOB::print(std::ostream &o) const {
   o << "gMHT signed Ex: " << Ex() << ", signed Ey: " << Ey() << ", Et: " << Et();
}

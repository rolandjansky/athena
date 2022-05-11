// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jTETOB.h"

thread_local TCS::Heap<TCS::jTETOB> TCS::jTETOB::fg_heap("jTETOB");

// default constructor
TCS::jTETOB::jTETOB() : BaseTOB(0,"jTETOB")
{}

// constructor with initial values
TCS::jTETOB::jTETOB(unsigned int sumEt, inputTOBType_t tobType) : BaseTOB(0,"jTETOB")
   , m_sumEt( sumEt )
   , m_tobType( tobType )
{}


// copy constructor
TCS::jTETOB::jTETOB(const TCS::jTETOB & jte) = default;


// destructor
TCS::jTETOB::~jTETOB() = default;

TCS::jTETOB*
TCS::jTETOB::createOnHeap(const jTETOB& jte) {
   return fg_heap.create(jte);
}

void
TCS::jTETOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::jTETOB::print(std::ostream &o) const {
   o << "jTE type: " << tobType() << ",  sumEt: " << sumEt();
}

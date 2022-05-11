// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gTETOB.h"

thread_local TCS::Heap<TCS::gTETOB> TCS::gTETOB::fg_heap("gTETOB");

// default constructor
TCS::gTETOB::gTETOB() : BaseTOB(0,"gTETOB")
{}

// constructor with initial values
TCS::gTETOB::gTETOB(unsigned int sumEt, inputTOBType_t tobType) : BaseTOB(0,"gTETOB")
   , m_sumEt( sumEt )
   , m_tobType( tobType )
{}


// copy constructor
TCS::gTETOB::gTETOB(const TCS::gTETOB & gte) = default;


// destructor
TCS::gTETOB::~gTETOB() = default;

TCS::gTETOB*
TCS::gTETOB::createOnHeap(const gTETOB& gte) {
   return fg_heap.create(gte);
}

void
TCS::gTETOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::gTETOB::print(std::ostream &o) const {
   o << "gTE type: " << tobType() << ",  sumEt: " << sumEt();
}

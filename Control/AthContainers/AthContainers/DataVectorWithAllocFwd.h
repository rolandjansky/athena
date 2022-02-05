// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/DataVectorWithAllocFwd.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2021
 * @brief 
 */


#ifndef ATHCONTAINERS_DATAVECTORWITHALLOCFWD_H
#define ATHCONTAINERS_DATAVECTORWITHALLOCFWD_H


#include <type_traits>


namespace SG {
template <class T, class VETO> class ArenaHeapSTLAllocator;
namespace detail {
class DataVectorAllocDummy {};
}
}
template <class DV, class ALLOC = SG::ArenaHeapSTLAllocator<std::remove_const_t<typename DV::base_value_type>, SG::detail::DataVectorAllocDummy> > class DataVectorWithAlloc;



#endif // not ATHCONTAINERS_DATAVECTORWITHALLOCFWD_H

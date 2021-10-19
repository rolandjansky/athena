// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthAllocators/maybeUnprotect.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2021
 * @brief Hook for unprotecting an arena.
 */


#ifndef ATHALLOCATORS_MAYBEUNPROTECT_H
#define ATHALLOCATORS_MAYBEUNPROTECT_H


namespace SG {


/**
 * @brief Hook for unprotecting an arena.
 *
 * Sometimes we need to ensure that an arena is unprotected before we start
 * destroying an object that contains the arena.  To do that without
 * making assumptions about whether the arena supports an unprotect
 * operation, call this function.
 *
 * This is the generic version, which is a no-op.  It will be specialized
 * as appropriate for individual allocators.
 */
template <class ALLOC>
void maybeUnprotect (ALLOC&)
{
}


} // namespace SG


#endif // not ATHALLOCATORS_MAYBEUNPROTECT_H

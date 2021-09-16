// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthAllocators/preDelete.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2021
 * @brief 
 */


#ifndef ATHALLOCATORS_PREDELETE_H
#define ATHALLOCATORS_PREDELETE_H


namespace SG {


template <class ALLOC>
void preDeleteAlloc (ALLOC&)
{
}


#endif // not ATHALLOCATORS_PREDELETE_H

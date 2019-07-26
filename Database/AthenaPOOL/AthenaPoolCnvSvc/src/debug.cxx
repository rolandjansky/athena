// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaPoolCnvSvc/src/debug.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief A couple hooks for debugging.
 */


#include "AthenaPoolCnvSvc/debug.h"


namespace AthenaPoolCnvSvc {


/**
 * @brief Called by templated converters at the start of @c createTransient.
 * @param clid The CLID of the class being converted.
 */
// Use [[maybe_unused]] rather than suppressing the param; otherwise, we won't
// be able to see it in the debugger.
void debugCreateTransient ([[maybe_unused]] CLID clid)
{
}


/**
 * @brief Called by templated converters at the start of @c createPersistent.
 * @param clid The CLID of the class being converted.
 */
// Use [[maybe_unused]] rather than suppressing the param; otherwise, we won't
// be able to see it in the debugger.
void debugCreatePersistent ([[maybe_unused]] CLID clid)
{
}


} // namespace AthenaPoolCnvSvc

// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaPoolCnvSvc/src/debug.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief A couple hooks for debugging.
 */


#include "AthenaPoolCnvSvc/debug.h"
#include "CxxUtils/unused.h"


namespace AthenaPoolCnvSvc {


/**
 * @brief Called by templated converters at the start of @c createTransient.
 * @param clid The CLID of the class being converted.
 */
// Use ATH_UNUSED rather than suppressing the param; otherwise, we won't
// be able to see it in the debugger.
void debugCreateTransient (CLID ATH_UNUSED(clid))
{
}


/**
 * @brief Called by templated converters at the start of @c createPersistent.
 * @param clid The CLID of the class being converted.
 */
// Use ATH_UNUSED rather than suppressing the param; otherwise, we won't
// be able to see it in the debugger.
void debugCreatePersistent (CLID ATH_UNUSED(clid))
{
}


} // namespace AthenaPoolCnvSvc

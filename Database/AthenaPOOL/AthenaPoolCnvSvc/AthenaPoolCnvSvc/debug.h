// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaPoolCnvSvc/debug.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief A couple hooks for debugging.
 *
 * It can be a bit of a pain to set breakpoints in heavily-templated code.
 * So the templated converter classes will call these out-of-line functions
 * at the start of createTransient() and createPersistent().
 * This provides an easy spot to set a breakpoint.
 */


#ifndef ATHENAPOOLCNVSVC_DEBUG_H
#define ATHENAPOOLCNVSVC_DEBUG_H


#include "GaudiKernel/ClassID.h"


namespace AthenaPoolCnvSvc {


/**
 * @brief Called by templated converters at the start of @c createTransient.
 * @param clid The CLID of the class being converted.
 */
void debugCreateTransient (CLID clid);


/**
 * @brief Called by templated converters at the start of @c createPersistent.
 * @param clid The CLID of the class being converted.
 */
void debugCreatePersistent (CLID clid);


} // namespace AthenaPoolCnvSvc


#endif // not ATHENAPOOLCNVSVC_DEBUG_H

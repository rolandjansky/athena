// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/error.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Helper for emitting error messages.
 *
 * This defines the macro @c ATHCONTAINER_ERROR, which emits an error
 * through the message service if we're built for Athena and prints
 * it directly if we're built standalone.
 *
 * We also define @c AthContainers_detail::typeinfoName.
 */


#ifndef ATHCONTAINERS_ERROR_H
#define ATHCONTAINERS_ERROR_H


#ifdef XAOD_STANDALONE

#include <iostream>
#include <string>
#include <typeinfo>

#define ATHCONTAINERS_ERROR(ctx, msg) \
  std::cout << ctx << "ERROR " << __FILE__ << ":" << __LINE__ << " " \
  << __func__ << ": ERROR: " << msg << "\n"


namespace AthContainers_detail {

std::string typeinfoName (const std::type_info& ti);

} // namespace AthContainers_detail


#else

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/System.h"

#define ATHCONTAINERS_ERROR(ctx, msg) \
  errorcheck::ReportMessage(MSG::ERROR, ERRORCHECK_ARGS, ctx).msgstream() << msg

namespace AthContainers_detail {

using System::typeinfoName;

} // namespace AthContainers_detail

#endif // not XAOD_STANDALONE


#endif // not ATHCONTAINERS_ERROR_H

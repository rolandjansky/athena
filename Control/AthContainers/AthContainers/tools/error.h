// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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

#include <string>
#include <typeinfo>

#define ATHCONTAINERS_ERROR(ctx, msg) \
  AthContainers_detail::reportErrorStandalone(ctx, __FILE__, __LINE__, msg)


namespace AthContainers_detail {

void reportErrorStandalone (const std::string& context,
                            const std::string& file, int line,
                            const std::string& msg);

std::string typeinfoName (const std::type_info& ti);

} // namespace AthContainers_detail


#else

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/System.h"

#define ATHCONTAINERS_ERROR(ctx, msg) \
  errorcheck::ReportMessage(MSG::ERROR, __LINE__, __FILE__, "", ATLAS_PACKAGE_NAME, ctx).msgstream() << msg

namespace AthContainers_detail {

using System::typeinfoName;

} // namespace AthContainers_detail

#endif // not XAOD_STANDALONE


#endif // not ATHCONTAINERS_ERROR_H

/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file CxxUtils/nodiscard.h
 * @brief Macro wrapping the [[nodiscard]] attribute.
 *
 *  cppyy will generate wrappers both for functions returning a value
 *  and the value being discarded.  If we have [[nodiscard]] on,
 *  then we'll get warnings when cling compiles these wrappers.
 *  Since these wrappers are automatically generated by cppyy,
 *  we can't really avoid these other than by disabling them
 *  for the cling case.
 */

#ifndef __CLING__
#define ATLAS_NODISCARD [[nodiscard]]
#else
#define ATLAS_NODISCARD
#endif

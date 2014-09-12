// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/DVLNoBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Marker for the base of the inheritance hierarchy.
 */


#ifndef ATHCONTAINERS_TOOLS_DVLNOBASE_H
#define ATHCONTAINERS_TOOLS_DVLNOBASE_H


namespace DataModel_detail {


/**
 * @brief Marker for the base of the inheritance hierarchy.
 *
 * Normally, @c DataVectorBase<T>::Base is the class from which
 * @c DataVector<T> should derive.
 * If, however, @c T has not had a base declared to @c DataVector,
 * then @c DataVectorBase<T>::Base will be @c NoBase.
 * (And similarly for @c DataList.)
 */
struct NoBase {};


} // namespace DataModel_detail



#endif // not ATHCONTAINERS_TOOLS_DVLNOBASE_H

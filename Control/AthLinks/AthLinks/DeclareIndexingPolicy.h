// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DeclareIndexingPolicy.h 723806 2016-02-13 16:18:51Z ssnyder $
#ifndef ATHLINKS_DECLAREINDEXINGPOLICY_H
# define ATHLINKS_DECLAREINDEXINGPOLICY_H

/** \file DeclareIndexingPolicy.h
 * @brief declare to which family the container belongs as far
 * as its indexing policy is concerned
 
 * @author Paolo Calafiura - ATLAS Collaboration
 * $Id: DeclareIndexingPolicy.h 723806 2016-02-13 16:18:51Z ssnyder $
 */

#include "AthLinks/tools/DefaultIndexingPolicy.h"
#include "AthLinks/tools/ForwardIndexingPolicy.h"
#include "AthLinks/tools/MapIndexingPolicy.h"
#include "AthLinks/tools/SetIndexingPolicy.h"
#include "AthLinks/tools/IdentContIndexingPolicy.h"

/** \def CONTAINER_INDEXING_POLICY(CONTAINER, POLICY)
 * @brief declare the indexing policy to be used for a container
 * @param CONTAINER ...
 * @param POLICY    the indexing policy for CONTAINER
 */
#define CONTAINER_INDEXING_POLICY(CONTAINER, POLICY) \
template <> \
struct DefaultIndexingPolicy < CONTAINER > { \
  typedef POLICY< CONTAINER > type; \
};

/** \def CONTAINER_IS_SEQUENCE(CONTAINER) \
 * @brief declare that CONTAINER is a STL sequence (like vector, list)
 */
#define CONTAINER_IS_SEQUENCE(CONTAINER) \
  CONTAINER_INDEXING_POLICY(CONTAINER, SG::ForwardIndexingPolicy)

/** \def CONTAINER_IS_MAP(CONTAINER) \
 * @brief declare that CONTAINER behaves like a STL map
 */
#define CONTAINER_IS_MAP(CONTAINER) \
  CONTAINER_INDEXING_POLICY(CONTAINER, SG::MapIndexingPolicy)

/** \def CONTAINER_IS_MAP(CONTAINER) \
 * @brief declare that CONTAINER behaves like a STL set
 */
#define CONTAINER_IS_SET(CONTAINER) \
  CONTAINER_INDEXING_POLICY(CONTAINER, SG::SetIndexingPolicy)

/** \def CONTAINER_IS_IDENTCONT(CONTAINER)
 * @brief declare that CONTAINER is an identifiable container
 */
#define CONTAINER_IS_IDENTCONT( CONTAINER )                          \
  CONTAINER_INDEXING_POLICY( CONTAINER, SG::IdentContIndexingPolicy )

#endif // ATHLINKS_DECLAREINDEXINGPOLICY_H

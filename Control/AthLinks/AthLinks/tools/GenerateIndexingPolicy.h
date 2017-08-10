/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 type generator that associates a known container with an IndexingPolicy. 
 Used by ElementLink
 ----------------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: GenerateIndexingPolicy.h,v 1.3 2007-03-03 17:56:15 schaffer Exp $

#ifndef ATHLINKS_TOOLS_GENERATEINDEXINGPOLICY_H
# define ATHLINKS_TOOLS_GENERATEINDEXINGPOLICY_H

#include "AthLinks/tools/ForwardIndexingPolicy.h"
#include "AthLinks/tools/DefaultIndexingPolicy.h"
#include "AthLinks/tools/IdentContIndexingPolicy.h"
#include "AthLinks/tools/IsSTLSequence.h"


namespace SG {
  template <class CONTAINER>
  struct GenerateIndexingPolicy {
    private:

    typedef IsSTLSequence<CONTAINER>   isSTLSequence_t;

    BOOST_STATIC_CONSTANT(bool, m_isSTLSequence = isSTLSequence_t::value);

  public:
    typedef typename boost::detail::if_true<(m_isSTLSequence)>::template
    then<
      ForwardIndexingPolicy<CONTAINER>,      
      //else
      typename DefaultIndexingPolicy<CONTAINER>::type
    >::type type;    
  };
}

#endif // ATHLINKS_TOOLS_GENERATEINDEXINGPOLICIES_H

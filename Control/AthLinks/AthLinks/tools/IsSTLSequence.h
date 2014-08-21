// Yo emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 determines whether CONTAINER is one of supported ATLAS sequences Requires
 CONTAINER to define value_type
 ----------------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: IsSTLSequence.h,v 1.5 2008-11-17 19:24:52 ssnyder Exp $

#ifndef ATHLINKS_TOOLS_ISSTLSEQUENCE_H
# define ATHLINKS_TOOLS_ISSTLSEQUENCE_H

// INCLUDES
#include "AthenaKernel/tools/type_tools.h"
#include <vector>
#include <list>
#include <deque>
#include <boost/config.hpp>
#include <boost/concept_check.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/ice.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/integral_constant.hpp>



// CLASS DECLARATIONS
namespace SG {
  // Some parts of Athena define FLAG as a macro.
  template <class CONTAINER, class FLAG_>
  struct TestSequenceTag
  {
    typedef boost::false_type type;
  };

  template <class CONTAINER>
  struct TestSequenceTag<CONTAINER, typename CONTAINER::isSequence>
  {
    typedef typename CONTAINER::isSequence type;
  };

  template <class CONTAINER>
  struct IsSTLSequence {
  private:
    // Causes trouble with fwd decls?  cf xAODTruth?
    //BOOST_CONCEPT_ASSERT((boost::ContainerConcept<CONTAINER>));
    typedef typename CONTAINER::value_type value_type;

    // vectors
    typedef typename boost::is_base_of< std::vector<value_type>, CONTAINER> isVector;

    // lists
    typedef typename boost::is_base_of< std::list<value_type>, CONTAINER> isList;

    // queues
    typedef typename boost::is_base_of< std::deque<value_type>, CONTAINER> isDeque;

    // explicit tag, by adding
    //   typedef type_tools::true_tag isSequence;
    // to a class.
    typedef typename TestSequenceTag<CONTAINER, boost::true_type>::type hasSequenceTag;

    //putting it all together
    typedef typename 
    boost::type_traits::ice_or< isDeque::value,
				isList::value,
				isVector::value,
                                hasSequenceTag::value > isSequence;
  public:
    BOOST_STATIC_CONSTANT(bool, value = isSequence::value);
  };
}
#endif // ATHLINKS_TOOLS_ISSTLSEQUENCE_H

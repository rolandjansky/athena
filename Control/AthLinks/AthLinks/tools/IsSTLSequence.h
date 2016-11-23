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

#include <vector>
#include <list>
#include <deque>
#include <type_traits>



// CLASS DECLARATIONS
namespace SG {
  // Some parts of Athena define FLAG as a macro.
  template <class CONTAINER, class FLAG_>
  struct TestSequenceTag
  {
    typedef std::false_type type;
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
    typedef typename std::is_base_of< std::vector<value_type>, CONTAINER> isVector;

    // lists
    typedef typename std::is_base_of< std::list<value_type>, CONTAINER> isList;

    // queues
    typedef typename std::is_base_of< std::deque<value_type>, CONTAINER> isDeque;

    // explicit tag, by adding
    //   typedef type_tools::true_tag isSequence;
    // to a class.
    typedef typename TestSequenceTag<CONTAINER, std::true_type>::type hasSequenceTag;

  public:
    //putting it all together
    static const bool value =
      isDeque::value ||
      isList::value ||
      isVector::value ||
      hasSequenceTag::value;
  };
}
#endif // ATHLINKS_TOOLS_ISSTLSEQUENCE_H

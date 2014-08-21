// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: findInContainer.h,v 1.3 2007-06-14 22:38:50 ssnyder Exp $
/**
 * @file  AthLinks/tools/findInContainer.h
 * @author scott snyder
 * @date Feb, 2007
 * @brief Define a specializable method for finding the index of an object
 *        within a container.
 */

#ifndef ATHLINKS_FINDINCONTAINER_H
#define ATHLINKS_FINDINCONTAINER_H


namespace SG {


/**
 * @brief Find the index of an object within a container.
 * @param data The container to search.
 * @param element The element for which to search.
 * @param[out] index The found index of the element.
 * @return true if the element was found; false otherwise.
 *
 * By default, this function will do an exhaustive search through
 * the container to find the element.  However, this function may
 * be specialized if this can be done more efficiently for some
 * specific container.
 */
template <typename CONT, typename ELT>
bool
findInContainer(const CONT& data,
                const ELT& element,
                typename CONT::size_type& index)
{
  typedef typename CONT::const_iterator const_iterator;
  const_iterator end = data.end();
  index = 0;
  for (const_iterator it = data.begin(); it != end; ++it) {
    if (*it == element) return true;
    ++index;
  }
  return false;
}


} // namespace SG


#endif // ATHLINKS_FINDINCONTAINER_H

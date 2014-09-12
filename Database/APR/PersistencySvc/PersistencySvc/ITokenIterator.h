/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_ITOKENITERATOR_H
#define INCLUDE_PERSISTENCYSVC_ITOKENITERATOR_H


// forward declarations
class Token;

namespace pool {

  /** @class ITokenIterator ITokenIterator.h PersistencySvc/ITokenIterator.h
   *
   *  ITokenIterator is the interface class for iterators of tokens
   *
   */

  class ITokenIterator {
  public:
    /// Empty destructor
    virtual ~ITokenIterator() {}

    /** Returns the pointer to next token.
     *  Token ownership is passed to the user.
     *  if no other token is available in the iteration
     *  sequence, 0 is returned.
     */
    virtual Token* next() = 0;
  };

}

#endif

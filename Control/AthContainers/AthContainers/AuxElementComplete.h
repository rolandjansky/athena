// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxElementComplete.h 770199 2016-08-25 21:16:39Z ssnyder $
/**
 * @file AthContainers/AuxElementComplete.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Wrapper to automatically create a private store for an element.
 */


#ifndef ATHCONTAINERS_AUXELEMENTCOMPLETE_H
#define ATHCONTAINERS_AUXELEMENTCOMPLETE_H


#include "AthContainers/AuxElement.h"


namespace SG {


/**
 * @brief Wrapper to automatically create a private store for an element.
 *
 * A class deriving from @c SG::AuxElement can be given a private store
 * by calling @c makePrivateStore; after that, it can have
 * auxiliary data attached to it.  Using this wrapper makes a class
 * that will automatically create a private store when it is constructed.
 *
 *@code
 *  class MyClass : public SG::AuxElement { ... };
 *  typedef SG::AuxElementComplete<MyClass> MyClassComplete;
 *
 *  MyClassComplete obj; // obj gets a private store.
 *  obj.auxdata<int> ("mydata") = 1;
 *  MyClassComplete obj2 (obj);  // aux data gets copied.
 @endcode
 */
template <class T>
class AuxElementComplete
  : public T
{
public:
  /**
   * @brief Default constructor.
   *
   * A private store is created for this object.
   */
  AuxElementComplete();


  /**
   * @brief Generic constructor.
   * @param u1 Constructor argument.
   *
   * A private store is created for this object.
   */
  template <class U1>
  AuxElementComplete (const U1& u1);


  /**
   * @brief Copy constructor.
   * @param other The object being copied.
   *
   * A private store is created for this object.
   */
  AuxElementComplete (const AuxElementComplete& other);


  // Declare default assignment explicitly to prevent coverity warning.
  AuxElementComplete& operator= (const AuxElementComplete& other) = default;
};


} // namespace SG


#include "AthContainers/AuxElementComplete.icc"


#endif // not ATHCONTAINERS_AUXELEMENTCOMPLETE_H

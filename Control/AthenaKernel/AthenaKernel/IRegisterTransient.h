// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthenaKernel/IRegisterTransient.h
 * @author scott snyder
 * @date Mar, 2008
 * @brief Interface for registering a transient object in t2p map.
 */


#ifndef ATHENAKERNEL_IREGISTERTRANSIENT_H
#define ATHENAKERNEL_IREGISTERTRANSIENT_H


namespace SG {


/**
 * @brief Interface for registering a transient object in t2p map.
 *
 * (See @c T2pMap.)
 */
class IRegisterTransient
{
public:
  /// Destructor.
  virtual ~IRegisterTransient() {}


  /**
   * @brief Register a transient object in a t2p map.
   * @param trans The object to register.
   */
  virtual void registerTransient (void* trans) = 0;
};


} // namespace SG


#endif // not ATHENAKERNEL_IREGISTERTRANSIENT_H

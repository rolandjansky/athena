// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IPositionSeek.h 457858 2011-09-08 14:29:29Z mnowak $
/**
 * @file  IPositionSeek.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2005
 * @brief Abstract interface for seeking within an event stream.
 */

#ifndef INCLUDE_PERSISTENCYSVC_IPOSITIONSEEK_H
#define INCLUDE_PERSISTENCYSVC_IPOSITIONSEEK_H

namespace pool {


/**
 * @class IPositionSeek
 * @brief Abstract interface for seeking inside a collection.
 */
class IPositionSeek
{
public:
  /**
   * @brief Destructor.
   */
  virtual ~IPositionSeek () {};


  /**
   * @brief Seek to a given position in the collection
   * @param position  The position to which to seek.
   * @returns True if successful, false otherwise.
   */
  virtual bool seek (long long int position) = 0;
};


} // namespace pool


#endif // not INCLUDE_PERSISTENCYSVC_IPOSITIONSEEK_H

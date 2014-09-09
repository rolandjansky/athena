// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScanForEL.h,v 1.3 2007-10-30 21:19:52 ssnyder Exp $

/**
 * @file AthenaROOTAccess/ScanForEL.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Set up streamer hooks for @c ElementLink classes.
 */

#ifndef ATHENAROOTACCESS_SCANFOREL_H
#define ATHENAROOTACCESS_SCANFOREL_H


class TClass;


namespace AthenaROOTAccess {


/**
 * @brief Set up streamer hooks for @c ElementLink classes.
 *
 * When an @c ElementLink class (that is not T/P separated) is read,
 * we need to arrange to call @c toTransient() and @c source() on it.
 *
 * This is done by associating an @c ELStreamer with each @c ElementLink class.
 *
 * On a call to @c initialize(), @c ScanForEL will scan all currently
 * known classes and install streamers for all ElementLink (and related)
 * classes.  It will further hook itself into class creation,
 * so that it will automatically scan any classes defined in the future.
 */
class ScanForEL
{
public:
  /**
   * @brief Initialize the streamers.
   * Scan all known classes, and also hook into class creation
   * so that we'll scan all future classes.
   */
  static void initialize();
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_SCANFOREL_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: InitHist.h,v 1.3 2007-10-30 16:49:30 ssnyder Exp $

/**
 * @file  RootUtils/InitHist.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Initialize Root's getline history.
 */

#ifndef ROOTUTILS_INITHIST_H
#define ROOTUTILS_INITHIST_H


namespace RootUtils {


/**
 * @brief Initialize Root's getline history.
 *        You'll need this if you use @c TTreeViewer using python
 *        as your interactive shell.  @c TTreeViewer pushes information
 *        onto the getline history list.  However, getline is initialized
 *        only when Cint is used as the shell.  If that isn't the case,
 *        then such attempts to push to the history will crash.
 */
class InitHist
{
public:
  /**
   * @brief Initialize Root's getline history.
   *
   * This could (almost) just as well be a free function.
   * The only reason to put it inside a class is that Root doesn't
   * auto-load functions, only classes.
   */
  static void initialize();
};


} // namespace RootUtils


#endif // not ROOTUTILS_INITHIST_H

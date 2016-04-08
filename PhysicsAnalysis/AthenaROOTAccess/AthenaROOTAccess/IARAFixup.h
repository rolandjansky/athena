// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/IARAFixup.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2015
 * @brief Interface to allow running fixups on a branch's data after reading.
 */


#ifndef ATHNEAROOTACCESS_IARAFIXUP_H
#define ATHNEAROOTACCESS_IARAFIXUP_H


namespace AthenaROOTAccess {


/**
 * @brief Interface to allow running fixups on a branch's data after reading.
 *
 * Some Athena types have a form of schema evolution that involves running
 * a fixup in the converter class itself (muons, i'm looking at you...). 
 * As this isn't done via a TP converter class, our generic mechanism won't
 * handle it.  So we need to allow for special-case bits of code to be run
 * after a branch is read.
 */
class IARAFixup
{
public:
  virtual ~IARAFixup() {}

  /// Run fixup on an object that has just been read in, before associating
  /// aux data.
  virtual void fixupBeforeAux (void* /*data*/, long long /*entry*/) {}

  /// Run fixup on an object that has just been read in, after associating
  /// aux data.
  virtual void fixupAfterAux (void* /*data*/, long long /*entry*/) {}
};


} // namespace AthenaROOTAccess


#endif // not ATHNEAROOTACCESS_IARAFIXUP_H

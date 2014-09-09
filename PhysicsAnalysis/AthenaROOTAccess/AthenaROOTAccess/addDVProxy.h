// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: addDVProxy.h,v 1.1 2008-04-18 02:52:56 ssnyder Exp $

/**
 * @file AthenaROOTAccess/addDVProxy.h
 * @author scott snyder
 * @date Apr 2008
 * @brief Helper: Set up collection proxy for a @c DataVector class.
 */


#ifndef ATHENAROOTACCESS_ADDDVPROXY_H
#define ATHENAROOTACCESS_ADDDVPROXY_H


class TClass;


namespace AthenaROOTAccess {


/**
 * @brief Helper: Set up collection proxy for a @c DataVector class.
 * @param cl The class to test.
 *
 * If @a cl is a @c DataVector class, or derives from one, then we attach
 * an appropriate Root collection proxy to it.  This is required for Root
 * to recognize the class as a collection in, eg, @c TTree::Draw.
 */
struct addDVProxy
{
  static void add (TClass* cl);
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_ADDDVPROXY_H

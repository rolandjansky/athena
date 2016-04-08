// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetCollectionHelper.h 588111 2014-03-18 09:02:18Z krasznaa $
#ifndef TRIGJETUTILS_JETCOLLECTIONHELPER_H
#define TRIGJETUTILS_JETCOLLECTIONHELPER_H

// Gaudi/Athena include(s):
#include "GaudiKernel/StatusCode.h"

// Forward declaration(s):
class JetCollection;

/// Copy of the parts of JetCollectionHelper needed by this package
///
/// Since the code of these functions needs to interact with the internals
/// of JetCollection, they had to be put into a struct called
/// JetCollectionHelper. Because that is a friend of JetCollection.
///
/// $Revision: 588111 $
/// $Date: 2014-03-18 10:02:18 +0100 (Tue, 18 Mar 2014) $
///
struct JetCollectionHelper {

   /// The "old" JetCollectionHelper::swapMomentMap function
   static StatusCode swapMomentMap( JetCollection* jets );
   /// The "old" JetCollectionHelper::resetJetId function
   static void resetJetId( JetCollection* jets, std::size_t dist );

}; // struct JetCollectionHelper

#endif // TRIGJETUTILS_JETCOLLECTIONHELPER_H

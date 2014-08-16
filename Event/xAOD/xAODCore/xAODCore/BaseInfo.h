// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BaseInfo.h 611441 2014-08-12 14:53:49Z krasznaa $
#ifndef XAODCORE_BASEINFO_H
#define XAODCORE_BASEINFO_H

/// @file xAODCore/CLASS_DEF.h
/// @author Scott Snyder <snyder@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// @short File providing the different SG_BASE macros
///
/// In Athena the inheritance relationships of the EDM classes are declared
/// to StoreGate using some simple macros that are used like:
///
/// <code>
///    #include "xAODCore/BaseInfo.h"<br/>
///    SG_BASE( xAOD::HighLevelClass, xAOD::LowLevelClass );<br/>
///    SG_BASES2( xAOD::LowLevelClass, SG::IAuxStore, SG::IAuxStoreIO );
/// </code>
///
/// In ROOT we don't need this extra information, there we rely purely on the
/// ROOT dictionaries of the EDM classes.
///
/// All the xAOD packages should use this header for declaring SG inheritance
/// relationships for their classes, instead of pulling in these macros from
/// SGTools directly.
///
/// $Revision: 611441 $
/// $Date: 2014-08-12 16:53:49 +0200 (Tue, 12 Aug 2014) $

#ifdef XAOD_STANDALONE

// Dummy macro definitions
#ifndef SG_BASE
#   define SG_BASE( D, B ) class xAODCoreDummy
#endif // not SG_BASE
#ifndef SG_BASES1
#   define SG_BASES1( D, B ) class xAODCoreDummy
#endif // not SG_BASES1
#ifndef SG_BASES2
#   define SG_BASES2( D, B1, B2 ) class xAODCoreDummy
#endif // not SG_BASES2
#ifndef SG_BASES3
#   define SG_BASES3( D, B1, B2, B3 ) class xAODCoreDummy
#endif // not SG_BASES3
#ifndef SG_ADD_BASE
#   define SG_ADD_BASE( D, B ) class xAODCoreDummy
#endif // not SG_ADD_BASE

#else // not XAOD_STANDALONE

// Pull in the proper macro definitions
#include "SGTools/BaseInfo.h"

#endif // not XAOD_STANDALONE

#endif // not XAODCORE_BASEINFO_H

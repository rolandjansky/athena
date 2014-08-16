// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CLASS_DEF.h 611401 2014-08-12 12:24:53Z krasznaa $
#ifndef XAODCORE_CLASS_DEF_H
#define XAODCORE_CLASS_DEF_H

/// @file xAODCore/CLASS_DEF.h
/// @author Scott Snyder <snyder@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// @short File providing the CLASS_DEF macro
///
/// In Athena all the EDM classes are identified using an ATLAS specific
/// system that assigns "class IDs" to the different classes. We do this by
/// using the CLASS_DEF macro in the headers of the EDM classes like:
///
/// <code>
///    #include "xAODCore/CLASS_DEF.h"<br/>
///    CLASS_DEF( xAOD::ClassName, 123456, 1 )
/// </code>
///
/// Since we don't need this infrastructure for ROOT (as there we just rely on
/// the ROOT dictionaries of all the EDM classes), the macro only does anything
/// meaningful in Athena. In the standalone build it evaluates to nothing.
///
/// All xAOD packages should use this header for declaring class IDs for their
/// EDM classes instead of using SGTools directly.
///
/// $Revision: 611401 $
/// $Date: 2014-08-12 14:24:53 +0200 (Tue, 12 Aug 2014) $

#ifdef XAOD_STANDALONE

// Dummy macro definition
#ifndef CLASS_DEF
#   define CLASS_DEF( CLASS, CLID, VERS )
#endif // not CLASS_DEF

#else // not XAOD_STANDALONE

// Pull in the proper macro definition
#include "SGTools/CLASS_DEF.h"

#endif // not XAOD_STANDALONE

#endif // not XAODCORE_CLASS_DEF_H

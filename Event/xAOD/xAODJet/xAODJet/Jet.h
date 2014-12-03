// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet.h 631914 2014-11-28 16:08:50Z krasznaa $
#ifndef XAODJET_JET_H
#define XAODJET_JET_H

// Local include(s):
#include "xAODJet/versions/Jet_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Definition of the current "jet version"
   typedef Jet_v1 Jet;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::Jet, 147270755, 1 )

#endif // XAODJET_JET_H

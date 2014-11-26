// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuon.h 631391 2014-11-26 21:43:36Z gwatts $
#ifndef XAODTRIGMUON_L2STANDALONEMUON_H
#define XAODTRIGMUON_L2STANDALONEMUON_H

// Local include(s):
#include "xAODTrigMuon/versions/L2StandAloneMuon_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Define the latest version of the muon SA class
  typedef L2StandAloneMuon_v1 L2StandAloneMuon;
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuon, 65772484, 1 )

#endif // XAODTRIGMUON_L2STANDALONEMUON_H

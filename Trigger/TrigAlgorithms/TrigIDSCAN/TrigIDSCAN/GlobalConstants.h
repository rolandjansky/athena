/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: GlobalConstants.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// Description: contains global constantds
// 
// date: 28/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_GLOBALCONSTANTS_H )
#define IDSCAN_GLOBALCONSTANTS_H


#ifndef M_2PI
#define M_2PI   6.28318530717958647692
#endif

namespace TrigIdScan {

  const long   IdScan_MaxNumLayers     = 19;

  const double IdScan_maxEta = 3.0;
  
} //namespace


#endif

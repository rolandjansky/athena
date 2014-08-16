// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Utils.h 573032 2013-11-29 13:33:57Z krasznaa $
#ifndef XAODCORE_TOOLS_UTILS_H
#define XAODCORE_TOOLS_UTILS_H

// System include(s):
#include <string>

// ROOT include(s):
#include <Rtypes.h>

namespace xAOD {

   namespace Utils {

      /// Function creating a human-readable elapsed time printout
      std::string timeToString( ::Double_t secs );

      /// Function for printing data sizes in human-readable format
      std::string sizeToString( ::Long64_t bytes );

      /// Function for printing data processing speeds in a human-readable format
      std::string speedToString( ::Double_t bytespersec );

   } // namespace Utils

} // namespace xAOD

#endif // XAODCORE_TOOLS_UTILS_H

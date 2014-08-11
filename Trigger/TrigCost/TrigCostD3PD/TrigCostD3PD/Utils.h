// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $
#ifndef D3PDREADER_UTILS_H
#define D3PDREADER_UTILS_H

// ROOT include(s):
#include <TString.h>

namespace D3PDReader {

   /// Function creating a human-readable elapsed time printout
   ::TString TimeToString( ::Double_t secs );

   /// Function for printing data sizes in human-readable format
   ::TString SizeToString( ::Long64_t bytes );

   /// Function for printing data processing speeds in a human-readable format
   ::TString SpeedToString( ::Double_t bytespersec );

} // namespace D3PDReader

#endif // D3PDREADER_UTILS_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataBackend.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATABACKEND_H
#define ANALYSISCALIBRATIONDATABACKEND_H

class TFile;

namespace Analysis {

  class CalibrationDataContainer;

  // Add a general container to the CDI file, in the directory as constructed
  // from the combination of tagger / jet author / operating point / flavour.
  // 
  // The last argument can be used to indicate whether the specified object
  // is to regarded as the reference for parton showering and hadronisation.
  // Notes on this:
  // - for a given parton showering and hadronisation choice, there can only
  //   be a single such reference in each directory
  // - for this to work, the container's getHadronisation() method needs to
  //   return a meaningful value AND be identified as an efficiency object

  Int_t addContainer(Analysis::CalibrationDataContainer* cnt, TFile* file,
		     const char* tagger, const char* jetAuthor,
		     const char* OP, const char* flavour,
		     const char* name = 0, bool isReference = false);
}

#endif

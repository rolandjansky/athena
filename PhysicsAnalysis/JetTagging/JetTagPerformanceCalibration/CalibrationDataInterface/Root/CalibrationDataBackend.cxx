/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 'Back-end' utilities for the CalibrationDataInterface code
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataContainer.h"
#include "CalibrationDataInterface/CalibrationDataBackend.h"

#include <iostream>
#include <string>

#include "TFile.h"
#include "TMap.h"
#include "TObjString.h"
#include "TDirectory.h"

// Name of the object holding the MC hadronisation references
// (this is used for the application of MC-to-MC calibration scale factors)

static const std::string hadronisationRefs("MChadronisation_ref");

// Back-end method for addition of a container to an existing file.
// Notes:
//    - "name" should be the full name (i.e., include the "_Eff" or "_SF" suffix)
//    - this function will change the global gDirectory. If the working directory is to be
//      preserved then the variable needs to be saved by the user prior to the function call
 
Int_t
Analysis::addContainer(Analysis::CalibrationDataContainer* cnt, TFile* file, const char* tagger,
		       const char* jetAuthor, const char* OP, const char* flavour, const char* name,
		       bool isReference) {
  file->cd("/"); TDirectory* dir = gDirectory;
  // top level: tagger
  TDirectory* taggerDir = dir->GetDirectory(tagger);
  if (! taggerDir) taggerDir = dir->mkdir(tagger);
  // then: jet author
  TDirectory* authorDir = taggerDir->GetDirectory(jetAuthor);
  if (! authorDir) authorDir = taggerDir->mkdir(jetAuthor);
  // then: operating point
  TDirectory* OPDir = authorDir->GetDirectory(OP);
  if (! OPDir) OPDir = authorDir->mkdir(OP);
  // finally: flavour
  TDirectory* flavourDir = OPDir->GetDirectory(flavour);
  if (! flavourDir) flavourDir = OPDir->mkdir(flavour);

  Int_t nbytes = flavourDir->WriteTObject (cnt, name, "SingleKey");

  if (isReference) {
    // first retrieve the hadronisation reference for this container
    std::string ref = cnt->getHadronisation();
    // make sure that the name is well defined
    const char* cntname = (name) ? name : cnt->GetName();
    if (ref == "")
      std::cerr << "addContainer(): container does not have a MC hadronisation reference!" << std::endl;
    else if (std::string(cntname).find("_Eff") == std::string::npos)
      std::cerr << "addContainer(): container is not a MC efficiency object!" << std::endl;
    else {
      // retrieve the map (or create it if it doesn't already exist)
      TMap* map;
      flavourDir->GetObject(hadronisationRefs.c_str(), map);
      if (!map) map = new TMap;
      // add (or replace, if it already exists) the histogram name for this hadronisation reference
      if (TPair* p = (TPair*) map->FindObject(ref.c_str())) map->DeleteEntry(p->Key());
      map->Add(new TObjString(ref.c_str()), new TObjString(cntname));
      // save the map to file (deleting any previous version)
      flavourDir->WriteTObject (map, hadronisationRefs.c_str(), "WriteDelete SingleKey");
    }
  }

  return nbytes;
}


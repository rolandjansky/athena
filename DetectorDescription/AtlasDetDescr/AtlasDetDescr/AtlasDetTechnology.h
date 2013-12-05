/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AtlasDetTechnology.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ATLASDETDESCR_ATLASDETTECHNOLOGY_H
#define ATLASDETDESCR_ATLASDETTECHNOLOGY_H 1

#include <assert.h>

// use these macros to check a given AtlasDetTechnology for its validity

#define validAtlasDetTechnology(detTechn) ( (detTechn<AtlasDetDescr::fNumAtlasDetTechnologies) && (detTechn>=AtlasDetDescr::fFirstAtlasDetTechnology) )
#define assertAtlasDetTechnology(detTechn) ( assert(validAtlasDetTechnology(detTechn)) )

namespace AtlasDetDescr {

 /** @enum AtlasDetTechnology
   
     A simple enum of ATLAS detector technologies.

     @author Elmar.Ritsch -at- cern.ch
   */

   enum AtlasDetTechnology {       
        // Unset
            fUndefined               = 0,
        // first Geometry element in enum, used in e.g. loops
            fFirstAtlasDetTechnology = 1,
        // Inner Detector
            fFirstAtlasIDTechnology  = 1,
            fAtlasPixel              = 1,
            fAtlasSCT                = 2,
            fAtlasTRT                = 3,
            fLastAtlasIDTechnology   = 3,
        // Calorimeter
            fFirstAtlasCaloTechnology= 4,
            fAtlasLArEMB             = 4,
            fAtlasLArEMEC            = 5,
            fAtlasLArHEC             = 6,
            fAtlasLArFCAL            = 7,
            fAtlasTile               = 8,
            fLastAtlasCaloTechnology = 8,
        // Muon Spectrometer
            fFirstAtlasMSTechnology  = 9,
            fAtlasMDT                = 9,
            fAtlasCSC                = 10,
            fAtlasRPC                = 11,
            fAtlasTGC                = 12,
            fAtlasSTGC               = 13,
            fAtlasMM                 = 14,
            fLastAtlasMSTechnology   = 14,
        // number of defined detector technologies
            fNumAtlasDetTechnologies = 15
   };

} // end of namespace

#endif // ATLASDETDESCR_ATLASDETTECHNOLOGY

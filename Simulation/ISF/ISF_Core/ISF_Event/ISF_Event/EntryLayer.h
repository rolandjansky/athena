/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_ENTRYLAYER_H
#define ISF_EVENT_ENTRYLAYER_H 1

#include <assert.h>

// use these macros to check a given EntryLayer id for its validity
#define validEntryLayer(layer) ( (layer>ISF::fUnsetEntryLayer) && (layer<ISF::fNumAtlasEntryLayers) )
#define assertEntryLayer(layer) ( assert(validEntryLayer(layer)) )


namespace ISF {

 /** @enum EntryLayer
   
     Identifiers for the TrackRecordCollections on the boundaries between
       CaloEntry: Inner Detector - Calorimeter
       MuonEntry: Calorimeter - Muon Spectrometer
       MuonExit : Muon Spectrometer - Cavern

     @author Elmar.Ritsch -at- cern.ch
   */

   enum EntryLayer {       
        // Not yet set
            fUnsetEntryLayer  = 0,
        // first Geometry element in enum, used in e.g. loops
            fFirstAtlasEntryLayer = 1,
        // actual list of entry layers
            fAtlasCaloEntry       = 1,
            fAtlasMuonEntry       = 2,
            fAtlasMuonExit        = 3,
        // number of defined EntryLayers
            fNumAtlasEntryLayers  = 4
   };

} // end of namespace

#endif  // ISF_EVENT_ENTRYLAYER_H

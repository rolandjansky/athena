/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SurfaceCollection.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_SURFACECOLLECTION_H
#define TRKSURFACES_SURFACECOLLECTION_H


#include "Identifier/Identifier.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
// Trk
#include "TrkSurfaces/Surface.h"
// STL
#include <map>
#include <utility>

   /** - <b>SurfaceCollection</b>
     
     DataVector filled with pointers to Surfaces and an iterator
     as well as a const iterator typedef.
     
     - <b>SurfaceMap</b>
     
     Standard STL map using the Identifier as map key for surfaces created directly form GeoModel
     
     
      - <b>TrachingSurfaceMap</b>
     
     Standard STL multimap using const double as map key for surfaces created through TrkGeometryBuilder
      
     @author Andreas.Salzburger@cern.ch */

   /** DataVector of const Surface / 24.08.2004
     % clid.py SurfaceCollection
     1185938832 SurfaceCollection */

   typedef DataVector<const Trk::Surface>                                   SurfaceCollection;
   typedef DataVector<const Trk::Surface>::const_iterator                   SurfaceCollectionIterator;

   // map of const  Surface*
   typedef std::map<const Identifier, const Trk::Surface*>                  SurfaceMap;
   typedef std::map<const Identifier, const Trk::Surface*>::const_iterator  SurfaceMapIterator;
   // map of const Surface* type2
   typedef std::multimap<double, const Trk::Surface*>                 TrackingSurfaceMap;
   typedef std::multimap<double, const Trk::Surface*>::const_iterator TrackingSurfaceMapIterator;
       
CLASS_DEF(SurfaceCollection , 1185938832 , 1 )

#endif // TRKSURFACES_SURFACECOLLECTION_H



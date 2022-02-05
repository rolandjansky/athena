/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-------------------------------------------------------------------------------------------//
//                                                                                           //
// Insert a volume into the detector store:                                                  //
//                                                                                           //
// Joe Boudreau October 2003                                                                 //
//                                                                                           //
//-------------------------------------------------------------------------------------------//

#ifndef STOREDPHYSVOL_H 
#define STOREDPHYSVOL_H

/// Ensure that the extensions for the Vector3D are properly loaded
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#ifndef BUILDVP1LIGHT
    #include "AthenaKernel/CLASS_DEF.h"
#endif
#include <set>
#include <string>

class GeoFullPhysVol;
class StoredPhysVol {

public:


  // ---------------------------For you: ----------------------------------------------------//
  //                                                                                         //
  // Standard Constructor                                                                    //
  StoredPhysVol(GeoFullPhysVol* physVol);                                                    //
  //                                                                                         //
  // Standard Destructor                                                                     //
  virtual ~StoredPhysVol();                                                                  //
  //                                                                                         //
  // Return the World physical volume:                                                       //
  GeoFullPhysVol *getPhysVol();                                                              //
  const GeoFullPhysVol *getPhysVol() const;                                                  //
  //                                                                                         //
  //                                                                                         //
  //-----------------------------------------------------------------------------------------//
  
 private:
  
  GeoFullPhysVol                                           *m_physVol;

};

#ifndef BUILDVP1LIGHT
 CLASS_DEF(StoredPhysVol, 9877, 1)
#endif

#endif 




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelKernel/GeoVPhysVol.h"
#include "CLIDSvc/CLASS_DEF.h"
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

CLASS_DEF(StoredPhysVol, 9877, 1)

#endif 




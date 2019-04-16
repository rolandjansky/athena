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

#ifndef STOREDALIGNX_H 
#define STOREDALIGNX_H

#ifndef BUILDVP1LIGHT
    #include "AthenaKernel/CLASS_DEF.h"
#endif
#include <set>
#include <string>

class GeoAlignableTransform;
class StoredAlignX {

public:


  // ---------------------------For you: ----------------------------------------------------//
  //                                                                                         //
  // Standard Constructor                                                                    //
  StoredAlignX(GeoAlignableTransform* alignX);                                               //
  //                                                                                         //
  // Standard Destructor                                                                     //
  virtual ~StoredAlignX();                                                                   //
  //                                                                                         //
  // Return the World physical volume:                                                       //
  GeoAlignableTransform *getAlignX();                                                        //
  const GeoAlignableTransform *getAlignX() const;                                            //
  //                                                                                         //
  //                                                                                         //
  //-----------------------------------------------------------------------------------------//
  
 private:
  
  GeoAlignableTransform                                           *m_alignX;

};

#ifndef BUILDVP1LIGHT
 CLASS_DEF(StoredAlignX, 9878, 1)
#endif

#endif // class




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H
#define INDETREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H

#include <vector>

namespace InDetDD {

    class SiDetectorElement;

    /** @class SiDetectorElementCollection
      
       Class to hold the SiDetectorElement objects to be put in the detector store

       @author Grant Gorfine
    */

    class SiDetectorElementCollection : public std::vector<SiDetectorElement *>
{};

} // namespace InDetDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDetDD::SiDetectorElementCollection , 1330395642 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDetDD::SiDetectorElementCollection, 1098715294 );

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H

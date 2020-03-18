/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H
#define INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H

#include <vector>
#include "InDetIdentifier/TRT_ID.h"

class IdentifierHash;
class Identifier;

namespace InDetDD {

  class TRT_BaseElement;

  /// Class to hold collection of TRT detector elements. 

  class TRT_DetElementCollection : public std::vector<TRT_BaseElement *>
  {
    public:

      const TRT_BaseElement* getDetectorElement(const IdentifierHash& hash) const;
  };

} // namespace InDetDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDetDD::TRT_DetElementCollection , 1136885219, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDetDD::TRT_DetElementCollection, 1334649756 );

#endif // INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H

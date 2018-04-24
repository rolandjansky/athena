/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H
#define INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H

#include <vector>

namespace InDetDD {

class TRT_BaseElement;

/// Class to hold collection of TRT detector elements. 

class TRT_DetElementCollection : public std::vector<const TRT_BaseElement *>
{};

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_TRT_DETELEMENTCOLLECTION_H

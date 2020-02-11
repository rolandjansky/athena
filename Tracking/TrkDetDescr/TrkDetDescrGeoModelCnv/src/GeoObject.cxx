/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoObject.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrGeoModelCnv/GeoObject.h"
 
void Trk::GeoObject::setEmbedLevel(int elevel) {embedLevel=elevel;}
void Trk::GeoObject::setMaterial(const GeoMaterial* mat)  {material=mat;}
void Trk::GeoObject::addClone(Amg::Transform3D transf) { transform.push_back(transf); }

//

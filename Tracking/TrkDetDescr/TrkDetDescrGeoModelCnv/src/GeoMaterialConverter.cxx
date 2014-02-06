/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoMaterialConverter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
// GeoModelKernel
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

double Trk::GeoMaterialConverter::s_densityCnvFactor = 1./CLHEP::gram;

Trk::Material Trk::GeoMaterialConverter::convert(const GeoMaterial* gm) const {
  // get the obvious things 
  float x0   = gm->getRadLength();
  float l0   = gm->getIntLength();
  float rho  = gm->getDensity() * s_densityCnvFactor;
  //float dEdX = gm->getDeDxMin();
  float A    = 0.;
  float Z    = 0.;
  // get the number of elements that build the material 
  int numberOfElements = gm->getNumElements();
  // loop over the elements
  for (int iEl=0; iEl<numberOfElements; iEl++){
        const GeoElement* geoEl = gm->getElement(iEl);
        float fraction = gm->getFraction(iEl);
        A += fraction*(geoEl->getA()/(CLHEP::gram));
        Z += fraction*(geoEl->getZ());             
   }
  // return the result
  return Trk::Material(x0,l0,A,Z,rho);

}

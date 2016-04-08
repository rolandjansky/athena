/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Csc.h"
#include "MuonGeoModel/CSC_Technology.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/CscComponent.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/CscMultiLayer.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include <cassert>
// for cutouts:
#include "GeoModelKernel/GeoShapeIntersection.h"

#define skip_csc false

namespace MuonGM {

Csc::Csc(Component* ss): DetectorElement(ss->name)
{
  CscComponent* s = (CscComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  thickness = s->GetThickness();
  maxwLength = s->maxwdy;
  excent=s->excent;
  physicalLength = s->dy;
  length = physicalLength;
  double num = longWidth*(excent - physicalLength);
  if (fabs(num) < 1e-10) {
    upWidth = 0;
  } else {
    upWidth = num/(excent-maxwLength);
  }

  layer = new CscMultiLayer(s->name);
  layer->width = width;
  layer->longWidth = longWidth;
  layer->upWidth = upWidth;
  layer->excent = excent;
  layer->length = length;
  layer->physicalLength = physicalLength;
  layer->maxwLength = maxwLength;
  
  index = s->index;
}


Csc::~Csc()
{
  delete layer;
  layer = 0;
}


GeoFullPhysVol *Csc::build(int minimalgeo)
{
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}


GeoFullPhysVol*
Csc::build(int minimalgeo, int cutoutson, std::vector<Cutout*> vcutdef)
{
  GeoFullPhysVol* pcsc   = NULL;
  GeoLogVol* lcsc   = NULL;
  const GeoMaterial* mcsc = matManager->getMaterial("std::Air");

  if (excent == length) {
    // CSC is a simple traezoid	
    const GeoShape* sCSS = new GeoTrd(thickness/2.,thickness/2.,
                                      width/2.,longWidth/2.,length/2.);
    lcsc = new GeoLogVol(logVolName, sCSS, mcsc);

  } else {
    // CSC is a union of two trapezoids 
    GeoTrd* downTrd = new GeoTrd(thickness/2., thickness/2., width/2., 
                                 longWidth/2.,  maxwLength/2.);
    GeoTrd* upTrd = new GeoTrd(thickness/2., thickness/2., longWidth/2., 
                                 upWidth/2., (physicalLength-maxwLength)/2.);
    const GeoShape* sCSL =
      & ( (downTrd->add( (*upTrd) << HepGeom::TranslateZ3D(physicalLength/2.) ) ) 
      << HepGeom::TranslateZ3D((maxwLength - physicalLength)/2.) );
    lcsc = new GeoLogVol(logVolName, sCSL, mcsc);
  }

  pcsc = new GeoFullPhysVol(lcsc);
  if (minimalgeo == 1) return pcsc;
    
  GeoVPhysVol* lay = layer->build(cutoutson, vcutdef);
  if (!skip_csc) pcsc->add(lay);
  
  return pcsc;	
}


void Csc::print()
{
  std::cout << " Csc:: Csc " << name << " : " << std::endl;
}

} // namespace MuonGM

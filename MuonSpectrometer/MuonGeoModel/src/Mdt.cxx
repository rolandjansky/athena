/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Mdt.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MultiLayer.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoShape.h"

#define verbose_mdt false

namespace MuonGM {

Mdt::Mdt(Component* ss, std::string lVName): DetectorElement(ss->name)
{
   logVolName = lVName;
   MdtComponent* s = (MdtComponent*)ss;
   MDT* thism = (MDT*)(MYSQL::GetPointer()->GetTechnology(s->name));

   width = s->dx1;
   longWidth = s->dx2;
   thickness = s->GetThickness();
   length = s->dy;
   m_component = s;
   m_component->cutoutTubeXShift = 0.;
   layer = new MultiLayer(s->name);
   layer->logVolName = lVName;
   layer->cutoutNsteps = 0;
   layer->width = width;
   layer->longWidth = longWidth;
   tubePitch = thism->pitch;
   layer->length = length;
   layer->nrOfTubes = (int)(layer->length/thism->pitch);
   if (longWidth  > width) layer->nrOfSteps = int(length/s->tubelenStepSize);
   tubelenStepSize = s->tubelenStepSize;
   index = s->index;
   m_geo_version = 400;
   layer->setGeoVersion(m_geo_version);
}


Mdt::~Mdt()
{
  delete layer;
  layer = 0;
}
    
    
GeoFullPhysVol* Mdt::build()
{
  layer->cutoutNsteps = 1;
  return layer->build();
}


GeoFullPhysVol* Mdt::build(std::vector<Cutout*> vcutdef)
{

  int igeometry_ref = 405;

  int Ncuts = vcutdef.size();
  if (Ncuts > 0) {
    if (verbose_mdt) std::cout << " mdt cutouts are on " << std::endl;
    int cutoutNtubes[5];           // Number of tubes in sub-multilayer[i]
    bool cutoutFullLength[5];      // True if this region is outside the cutout
    double cutoutXtubes[5];        // Location of tube center within sub-ml[i] along x-amdb
    double cutoutTubeLength[5];    // Tube length
    double cutoutYmax[5];
    for (int i = 0; i < 5; i++) {
      cutoutNtubes[i] = 0;
      cutoutFullLength[i] = true;
      cutoutXtubes[i] = 0.;
      cutoutTubeLength[i] = width;
      cutoutYmax[i] = 0.;
    }
    
    // Order cutouts by increasing dy    
    for (int i = 0; i < Ncuts; i++) {
      for (int j = i+1; j < Ncuts; j++) {
        if (vcutdef[j]->dy < vcutdef[i]->dy) {
          Cutout* c = vcutdef[i];
          vcutdef[i] = vcutdef[j];
          vcutdef[j] = c;
        }
      }
    }

    // Set up cut location code 
    double top = length - tubePitch/2.;
    int cutLocationCode[3] = {0, 0, 0};
    for (int i = 0; i < Ncuts; i++) {
      if (vcutdef[i]->dy <= 0) cutLocationCode[i] = -1;
      if (vcutdef[i]->dy + vcutdef[i]->lengthY >= top) cutLocationCode[i] = 1;
    }

    // Calculate quantities needed by multilayer
    double twidth;
    double xmin;
    double xmax;
    bool cutAtAngle = false;
    int Nsteps = 0;
    for (int i = 0; i < Ncuts; i++) {
      Cutout* c = vcutdef[i];
      if (c->dead1 >1.) cutAtAngle = true;
      twidth = width + (longWidth - width)*c->dy/length;
      xmin = -twidth/2. < c->dx - c->widthXs/2. ? -twidth/2. : c->dx + c->widthXs/2.;
      xmax = twidth/2. > c->dx + c->widthXs/2. ? twidth/2. : c->dx - c->widthXs/2.;
      if (cutLocationCode[i] == -1) {
        cutoutYmax[Nsteps] = c->lengthY;
        cutoutTubeLength[Nsteps] = xmax - xmin; 
        cutoutXtubes[Nsteps] = (xmax + xmin)/2.;
        cutoutFullLength[Nsteps] = false;
        Nsteps++;
      } else if (cutLocationCode[i] == 1) {
        cutoutYmax[Nsteps] = c->dy;
        Nsteps++;
        cutoutTubeLength[Nsteps] = xmax - xmin;
        cutoutXtubes[Nsteps] = (xmax + xmin)/2.;
        cutoutFullLength[Nsteps] = false;
      } else {
        cutoutYmax[Nsteps] = c->dy;
        Nsteps++;
        cutoutYmax[Nsteps] = c->dy + c->lengthY;
        cutoutTubeLength[Nsteps] = xmax - xmin;
        cutoutXtubes[Nsteps] = (xmax + xmin)/2.;
        cutoutFullLength[Nsteps] = false;
        Nsteps++;
      }                                 
    }
    cutoutYmax[Nsteps] = top;
    Nsteps++;

    double regionLength = 0.;
    double low = 0.;
    double tol = 6.0;     // Extra length to avoid trucation of tubes
    int fullLengthCounter = 0;
    int tubeCounter = 0;
    for (int i = 0; i < Nsteps; i++) {
      if (cutoutFullLength[i]) fullLengthCounter++;
      if (getGeoVersion() < igeometry_ref)
	{
	  regionLength = cutoutYmax[i] - low + tol;
	  cutoutNtubes[i] = int(regionLength/tubePitch);
	}
      else 
	{
	  regionLength = cutoutYmax[i] - low;
	  cutoutNtubes[i] = int(regionLength/tubePitch);
	  if ((regionLength/tubePitch - cutoutNtubes[i])>0.5) cutoutNtubes[i]+=1; 
	}
      if (fullLengthCounter > 1) cutoutNtubes[i]++;
      low = cutoutYmax[i];
      tubeCounter += cutoutNtubes[i];
    }
    if (tubeCounter > layer->nrOfTubes) --cutoutNtubes[Nsteps-1];

    if (verbose_mdt) for (int i = 0; i < Nsteps; i++) 
      std::cout << " cutoutYmax[" << i << "] = " << cutoutYmax[i] 
                << " cutoutTubeLength[" << i << "] = " << cutoutTubeLength[i]
                << " cutoutXtubes[" << i << "] = " << cutoutXtubes[i]
                << " cutoutFullLength[" << i << "] = " << cutoutFullLength[i]
                << " cutoutNtubes[" << i << "] = " << cutoutNtubes[i]
                << std::endl; 

    // Pass information to multilayer and MdtComponent
    layer->cutoutNsteps = Nsteps;
    m_component->cutoutTubeXShift = 0.;
    for (int i = 0; i < 5; i++) {
      layer->cutoutNtubes[i] = cutoutNtubes[i];
      layer->cutoutTubeLength[i] = cutoutTubeLength[i];
      layer->cutoutFullLength[i] = cutoutFullLength[i];
      layer->cutoutXtubes[i] = cutoutXtubes[i];
      layer->cutoutYmax[i] = cutoutYmax[i];
      if (!cutoutFullLength[i]) m_component->cutoutTubeXShift = cutoutXtubes[i];
      // For now assume multiple cutouts have same width and take only the last value 
    }
    
    if (getGeoVersion() < igeometry_ref)
      {
	// do nothing - preserve frozen Tier0 policy for layout < r.0404
	layer->cutoutAtAngle = false;
      }
    else 
      {
	layer->cutoutAtAngle = cutAtAngle;
      }
    return layer->build();

  } else {        
    return build();
  }
}


void Mdt::print()
{
  std::cout << "Mdt " << name.c_str() << " :" << std::endl;
}

void Mdt::setGeoVersion(int ver)
{
  m_geo_version = ver;   
  layer->setGeoVersion(m_geo_version);
}

} // namespace MuonGM

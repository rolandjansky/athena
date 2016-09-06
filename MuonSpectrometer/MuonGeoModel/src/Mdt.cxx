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

    // encoding BMG chambers in Nsteps: Nsteps negative => BMG chamber
    // a/ 1st digit: eta index (1 to 3)
    // b/ 2nd digit: 1 == A side, 2 == C side
    // c/ 3rd digit: multilayer (1 or 2)
    // d/ last 2 digits: sector (12 or 14)

    if( logVolName.find("MDT10") != std::string::npos ) {
      // multilayer 1 of BMG1A12, BMG2A12, BMG3A12, BMG1C14, BMG2C14, BMG3C14
      if( vcutdef[0]->icut == 1 ) { // these are BMG1A12 and BMG1C14
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -11112; // cut angle for A side positive BMG1A12
        else Nsteps = -12114; // cut angle for C side negative BMG1C14
      } else if( vcutdef[0]->icut == 2 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -21112; // cut angle for A side positive BMG2A12
        else Nsteps = -22114; // cut angle for C side negative BMG2C14
      } else if( vcutdef[0]->icut == 3 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -31112; // cut angle for A side positive BMG3A12
        else Nsteps = -32114; // cut angle for C side negative BMG3C14
      } else {
        std::cout << "massive error with MDT10 (BMG chambers)" << std::endl;
        std::abort();
      }
    } else if( logVolName.find("MDT11") != std::string::npos ) {
      // multilayer 1 of BMG1A14, BMG2A14, BMG3A14, BMG1C12, BMG2C12, BMG3C12
      if( vcutdef[0]->icut == 1 ) { // these are BMG1A12 and BMG1C14
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -11114; // cut angle for A side positive BMG1A14
        else Nsteps = -12112; // cut angle for C side negative BMG1C12
      } else if( vcutdef[0]->icut == 2 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -21114; // cut angle for A side positive BMG2A14
        else Nsteps = -22112; // cut angle for C side negative BMG2C12
      } else if( vcutdef[0]->icut == 3 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -31114; // cut angle for A side positive BMG3A14
        else Nsteps = -32112; // cut angle for C side negative BMG3C12
      } else {
        std::cout << "massive error with MDT10 (BMG chambers)" << std::endl;
        std::abort();
      }
    } else if( logVolName.find("MDT12") != std::string::npos ) {
      // multilayer 2 of BMG1A12, BMG2A12, BMG3A12, BMG1C14, BMG2C14, BMG3C14
      if( vcutdef[0]->icut == 1 ) { // these are BMG1A12 and BMG1C14
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -11212; // cut angle for A side positive BMG1A12
        else Nsteps = -12214; // cut angle for C side negative BMG1C14
      } else if( vcutdef[0]->icut == 2 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -21212; // cut angle for A side positive BMG2A12
        else Nsteps = -22214; // cut angle for C side negative BMG2C14
      } else if( vcutdef[0]->icut == 3 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -31212; // cut angle for A side positive BMG3A12
        else Nsteps = -32214; // cut angle for C side negative BMG3C14
      } else {
        std::cout << "massive error with MDT10 (BMG chambers)" << std::endl;
        std::abort();
      }
    } else if( logVolName.find("MDT13") != std::string::npos ) {
      // multilayer 2 of BMG1A14, BMG2A14, BMG3A14, BMG1C12, BMG2C12, BMG3C12
      if( vcutdef[0]->icut == 1 ) { // these are BMG1A12 and BMG1C14
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -11214; // cut angle for A side positive BMG1A14
        else Nsteps = -12212; // cut angle for C side negative BMG1C12
      } else if( vcutdef[0]->icut == 2 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -21214; // cut angle for A side positive BMG2A14
        else Nsteps = -22212; // cut angle for C side negative BMG2C12
      } else if( vcutdef[0]->icut == 3 ) {
        if ( vcutdef[0]->dead1 > 0. ) Nsteps = -31214; // cut angle for A side positive BMG3A14
        else Nsteps = -32212; // cut angle for C side negative BMG3C12
      } else {
        std::cout << "massive error with MDT10 (BMG chambers)" << std::endl;
        std::abort();
      }
    }

    if ( logVolName.find("MDT10") != std::string::npos
       || logVolName.find("MDT11") != std::string::npos
       || logVolName.find("MDT12") != std::string::npos
       || logVolName.find("MDT13") != std::string::npos ) {
       for (int i = 0; i < 5; i++) {
           cutoutNtubes[i] = 0;
           cutoutFullLength[i] = true;
           cutoutXtubes[i] = 0.;
           cutoutTubeLength[i] = width;
           cutoutYmax[i] = 0.;
        }
    }

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

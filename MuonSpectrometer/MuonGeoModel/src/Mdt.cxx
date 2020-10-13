/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Mdt.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/MultiLayer.h"
#include "MuonGeoModel/MdtComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoShape.h"

#include <TString.h> // for Form
#include "float.h"

#define verbose_mdt false

namespace MuonGM {

float round(const float toRound, const unsigned int decimals) {
  unsigned int factor = std::pow(10,decimals);
  return std::round(toRound*factor)/factor;
}

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

  int Ncuts = vcutdef.size();
  if (Ncuts > 0) {
    if (verbose_mdt) std::cout << " mdt cutouts are on " << std::endl;

    bool cutoutsVsX=false;
    bool cutoutsVsY=false;
    // first check whether there are several cutouts with different amdb x or y coordinates
    float lastX(FLT_MAX);
    float lastY(FLT_MAX);
    for (int i = 0; i < Ncuts; ++i) {
      if (lastX!=FLT_MAX && lastX*vcutdef[i]->dx<0) cutoutsVsX=true;
      lastX = vcutdef[i]->dx;
      if (lastY!=FLT_MAX && lastY*vcutdef[i]->dy<0) cutoutsVsY=true;
      lastY = vcutdef[i]->dy;
    }
    if (cutoutsVsX&&cutoutsVsY) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMdt::build() - Found more than one cutout in amdb-x direction and more than one cutout in amdb-y direction, currently not supported", __FILE__, __LINE__));
    }

    if (!cutoutsVsX) { // nominal case (used for BMS/BOG/BMG etc.)

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
      if (round(vcutdef[i]->dy + vcutdef[i]->lengthY,2) >= round(top,2)) cutLocationCode[i] = 1;
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
    int fullLengthCounter = 0;
    int tubeCounter = 0;
    for (int i = 0; i < Nsteps; i++) {
      if (cutoutFullLength[i]) fullLengthCounter++;

      regionLength = cutoutYmax[i] - low;
      cutoutNtubes[i] = int(regionLength/tubePitch);
      if ((regionLength/tubePitch - cutoutNtubes[i])>0.5) cutoutNtubes[i]+=1;

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
    
    layer->cutoutAtAngle = cutAtAngle;

  } else {
    // there are several cutouts along the amdb-x coordinate

    if (longWidth!=width) {
      throw std::runtime_error(Form("File: %s, Line: %d\nMdt::build() - only support cutouts along amdb-x for rectangular chambers", __FILE__, __LINE__));
    }

    std::vector<std::pair<double,double>> nonCutoutXSteps;
    std::vector<std::pair<double,double>> nonCutoutYSteps;

    // Order cutouts by increasing dx
    for (int i = 0; i < Ncuts; i++) {
      for (int j = i+1; j < Ncuts; j++) {
        if (vcutdef[j]->dx < vcutdef[i]->dx) {
          Cutout* c = vcutdef[i];
          vcutdef[i] = vcutdef[j];
          vcutdef[j] = c;
        }
      }
    }

    // in amdb-coordinates
    double xminChamber = round(-width/2,2);
    double xmaxChamber = round(width/2,2);
    double yminChamber = 0;
    double ymaxChamber = round(length-tubePitch/2,2);

    double latestXMax=xminChamber;

    for (int i = 0; i < Ncuts; ++i) {
      Cutout* c = vcutdef[i];
      double lowerX = round(c->dx - c->widthXs/2,2);
      double xmin = (lowerX <= xminChamber) ? xminChamber : lowerX;
      if (xmin<latestXMax) {
        throw std::runtime_error(Form("File: %s, Line: %d\nMdt::build() - cannot have cutouts along amdb-x which overlap in amdb-x", __FILE__, __LINE__));
      }
      if (i==0 && xmin>xminChamber) {
        // we start with a full slice without cutout
        nonCutoutXSteps.push_back(std::make_pair(xminChamber,lowerX));
        nonCutoutYSteps.push_back(std::make_pair(yminChamber,ymaxChamber));
      }
      double upperX = round(c->dx + c->widthXs/2,2);
      double xmax = (upperX >= xmaxChamber) ? xmaxChamber : upperX;

      double ymin = round(c->dy+c->lengthY,2) < ymaxChamber ? c->dy+c->lengthY : 0;
      double ymax = ymaxChamber <= round(c->dy+c->lengthY,2) ? c->dy : ymaxChamber;

      if (latestXMax<xmin) {
        // there is a full slice between latestXMax and xmin
        nonCutoutXSteps.push_back(std::make_pair(latestXMax,xmin));
        nonCutoutYSteps.push_back(std::make_pair(yminChamber,ymaxChamber));
      }

      nonCutoutXSteps.push_back(std::make_pair(xmin,xmax));
      nonCutoutYSteps.push_back(std::make_pair(ymin,ymax));


      if (i==Ncuts-1 && xmax<xmaxChamber) {
        // we end with a full slice without cutout
        nonCutoutXSteps.push_back(std::make_pair(xmax,xmaxChamber));
        nonCutoutYSteps.push_back(std::make_pair(yminChamber,ymaxChamber));
      }

      latestXMax = xmax;
    }

    // Pass information to multilayer and MdtComponent
    m_component->cutoutTubeXShift = 0;
    layer->cutoutNsteps = nonCutoutXSteps.size();
    layer->m_nonCutoutXSteps = nonCutoutXSteps;
    layer->m_nonCutoutYSteps = nonCutoutYSteps;
    layer->cutoutAtAngle = false;
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

} // namespace MuonGM

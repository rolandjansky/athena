/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MultiLayer.h"
#include "MuonGeoModel/DriftTube.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Mdt.h"
#include "MuonGeoModel/MDT_Technology.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoGenericFunctions/Variable.h"
// for cutouts
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoTube.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <vector>
#include <cassert>
using namespace GeoXF;

#define verbose_multilayer false

namespace MuonGM {

MultiLayer::MultiLayer(std::string n): DetectorElement(n),
  nrOfLayers(0), nrOfTubes(0), tubePitch(0.), width(0.), length(0.), thickness(0.),
  mdtthickness(0.), longWidth(0.), nrOfSteps(0), cutoutNsteps(0), cutoutAtAngle(false)
{
   MYSQL* mysql = MYSQL::GetPointer();
   MDT* md = (MDT*)mysql->GetTechnology(name);
   if (md != NULL) {
     nrOfLayers = md->numOfLayers;
     mdtthickness = md->totalThickness;
     tubePitch = md->pitch;
     for (int i = 0; i < 4; i++) {
       yy[i] = 0.;
       xx[i] = 0.;
     }

     for (int i = 0; i < nrOfLayers; i++) {
       yy[i] = md->y[i];
       xx[i] = md->x[i];
     }
     nrOfSteps=1;

   } else {
     std::cerr << "Multilayer constructor - a problem here !!! MDT named " << name
               << " not found \n Cannot init correctly !";
     assert(0);
   }
}


GeoFullPhysVol* MultiLayer::build()
{
    DriftTube tube(name+" DriftTube");
    double eps = 0.001;
    double tuberad = tube.outerRadius;
    if (verbose_multilayer)
      std::cout << " MultiLayer::build of " << name 
                << " logVolName = " << logVolName
                << " tube radius, pitch = " << tuberad 
                << " , " << tubePitch << std::endl;

    const GeoShape* slay = NULL;
    const GeoShape* sfoamup = NULL;   // do them both together when there are cutouts
    const GeoShape* sfoamlow = NULL;  // do them both together when there are cutouts
    // so calculate other foam-related info first:
    double foamthicknesslow = yy[0] - tuberad;
    double foamthicknessup;
    if (yy[3]) foamthicknessup = mdtthickness - (yy[3]+tuberad);
    else foamthicknessup = mdtthickness - (yy[2]+tuberad);

    if (foamthicknessup > foamthicknesslow) {
      foamthicknesslow = 0.;
      if (fabs(foamthicknessup - 15*Gaudi::Units::mm) < 0.1) foamthicknessup = 15*Gaudi::Units::mm;
      else if (fabs(foamthicknessup - 30.75*Gaudi::Units::mm) < 0.1) foamthicknessup = 30.75*Gaudi::Units::mm;
      else if (fabs(foamthicknessup - 30.00*Gaudi::Units::mm) < 0.1) foamthicknessup = 30.00*Gaudi::Units::mm;
      else if (fabs(foamthicknessup - 10.00*Gaudi::Units::mm) < 0.1
               && logVolName.find("BMG") != std::string::npos ) foamthicknessup = 10.00*Gaudi::Units::mm;
      else if ( logVolName == "BME1MDT09" || logVolName == "BME2MDT09" ) { //@@
	foamthicknesslow = 0.;
	foamthicknessup  = 0.;
      } else {
        std::cout << " problem with thickness of " << std::abs(foamthicknessup)
                  << " for foam in LogVolName " << logVolName << std::endl;
      }

    } else {
      foamthicknessup = 0.;
      if (fabs(foamthicknesslow - 15*Gaudi::Units::mm) < 0.1) foamthicknesslow = 15*Gaudi::Units::mm;
      else if (fabs(foamthicknesslow - 30.75*Gaudi::Units::mm) < 0.1) foamthicknesslow = 30.75*Gaudi::Units::mm;
      else if (fabs(foamthicknesslow - 30.00*Gaudi::Units::mm) < 0.1) foamthicknesslow = 30.00*Gaudi::Units::mm;
      else if (fabs(foamthicknesslow - 10.00*Gaudi::Units::mm) < 0.1
               && logVolName.find("BMG") != std::string::npos ) foamthicknesslow = 10.00*Gaudi::Units::mm;
      else if ( logVolName == "BME1MDT09" || logVolName == "BME2MDT09" ) { //@@
	foamthicknesslow = 0.;
	foamthicknessup  = 0.;
      } else {
        std::cout << " problem with thickness of " << std::abs(foamthicknesslow)
                  << " for foam in LogVolName " << logVolName << std::endl;
      }
    }

    // Build the layer envelope into which the MDTs are placed
    double TrdDwoverL= (longWidth-width)/length;
    if (cutoutNsteps == 1 || cutoutAtAngle || (cutoutNsteps < -10000 && cutoutNsteps > -40000 ) ) { 
      // No cutouts - layer is a simple box or trapezoid
      slay = new GeoTrd(mdtthickness/2, mdtthickness/2, width/2, 
                        longWidth/2, length/2);
    } else {
      // Layer to be built as a boolean of boxes and trapezoids to represent cutouts
      if (verbose_multilayer) std::cout << name << " has " << cutoutNsteps
                                        << " cutout steps " << std::endl;
      double submlthick = mdtthickness/2.;
      double submlwidths = width/2.;
      double submlwidthl = width/2.;
      double lengthPos = 0.;
      double sum_len = 0;

      for (int isub = 0; isub < cutoutNsteps; isub++) {
        if (verbose_multilayer) std::cout << " cutout region " << isub << " has "
                                          << cutoutNtubes[isub] << " tubes " << std::endl;
        if (cutoutNtubes[isub] <= 0) {
          std::cout << " Skipping cutout region " << isub << " with " << cutoutNtubes[isub]
                    << " tubes in LogVolName " << logVolName << std::endl;
          continue;
        }

        double submllength = (cutoutNtubes[isub]*tubePitch)/2.;
        if (cutoutFullLength[isub] && isub != cutoutNsteps - 1) submllength += tubePitch/4.;
        submlwidthl += submllength*TrdDwoverL;
        lengthPos = -length/2. + sum_len + submllength;
        sum_len += 2.*submllength;
        double widthPos = cutoutXtubes[isub];
        GeoTrf::Transform3D submlpos = GeoTrf::Translate3D(0.,widthPos,lengthPos);

        const GeoTrd* tempSLay = NULL;
        const GeoShape* tempSLay1 = NULL;
        const GeoTrd* tempSFoamup = NULL;
        const GeoShape* tempSFoamup1 = NULL;
        const GeoTrd* tempSFoamlow = NULL;
        const GeoShape* tempSFoamlow1 = NULL;

        if (submlthick*submlwidthl*submllength > 0) {
          if (verbose_multilayer) std::cout << " LogVolName " << logVolName
                                            << " cut step " << isub 
                                            << " thick = "  << submlthick 
                                            << " short width = " << submlwidths 
                                            << " long width = " << submlwidthl 
                                            << " length = " << submllength 
                                            << " translated to " << widthPos 
                                            << " , " << lengthPos << std::endl; 
          if (cutoutFullLength[isub]) {
            tempSLay = new GeoTrd(submlthick, submlthick, submlwidths, 
                                  submlwidthl, submllength);
          } else {
            tempSLay = new GeoTrd(submlthick, submlthick, cutoutTubeLength[isub]/2.,
                                  cutoutTubeLength[isub]/2., submllength);
          }
          tempSLay1 = &( (*tempSLay)<<submlpos);
        } else {
          std::cout << " problem with shape of temporary trapezoid in LogVolName "
                  << logVolName << " thick,width,length " << submlthick
                  << " " << submlwidths << " " << submllength << std::endl;
        }

        if (foamthicknessup > foamthicknesslow) {
          if (foamthicknessup*submlwidths*submllength > 0) {
            if (cutoutFullLength[isub]) {
              tempSFoamup = new GeoTrd(foamthicknessup/2., foamthicknessup/2.,
                                       submlwidths, submlwidthl, submllength);
            } else {
              tempSFoamup = new GeoTrd(foamthicknessup/2., foamthicknessup/2.,
                                       cutoutTubeLength[isub]/2., cutoutTubeLength[isub]/2., 
                                       submllength);
            }

            tempSFoamup1 = &( (*tempSFoamup)<<submlpos);
          } else {
            std::cout << " problem with shape of upper foam trapezoid in LogVolName "
                      << logVolName << " thick,width,length " << foamthicknessup
                      << " " << submlwidths << " " << submllength << std::endl;
          }
        } else {
          if (foamthicknesslow*submlwidths*submllength > 0) {
            if (cutoutFullLength[isub]) {
              tempSFoamlow = new GeoTrd(foamthicknesslow/2., foamthicknesslow/2., 
                                        submlwidths, submlwidthl, submllength);
            } else {
              tempSFoamlow = new GeoTrd(foamthicknesslow/2., foamthicknesslow/2.,
                                        cutoutTubeLength[isub]/2., cutoutTubeLength[isub]/2.,
                                        submllength);
            }
            tempSFoamlow1 = &( (*tempSFoamlow)<<submlpos);
          } else {
            std::cout << " problem with shape of lower foam trapezoid in LogVolName "
                      << logVolName << " thick,width,length " << foamthicknesslow
                      << " " << submlwidths << " " << submllength << std::endl;
          }
        }

        submlwidths = submlwidthl;

        if (slay) {
          if (verbose_multilayer) std::cout << " Layer " << slay << " in " << logVolName
                                            << " exists - add step section to it " << std::endl;
          slay = &(slay->add(*tempSLay1));
          if (foamthicknessup > 0.) sfoamup = &(sfoamup->add(*tempSFoamup1));
          if (foamthicknesslow > 0.) sfoamlow = &(sfoamlow->add(*tempSFoamlow1));

        } else {
          if (verbose_multilayer) std::cout << " Layer slay in " << logVolName
                                            << " does not yet exist - create it " << std::endl;
          slay = tempSLay1;
          if (foamthicknessup > 0.) sfoamup = tempSFoamup1;
          if (foamthicknesslow > 0.) sfoamlow = tempSFoamlow1;
        }
      } // Loop over cutout steps
    } // End of cutout block

    // Add/subtract cylinders at ends of layers 2 and 4 to accommodate the last MDT

    const GeoShape* stube = NULL;
    double tL = longWidth/2.0 - (tubePitch/2.)*TrdDwoverL;
    stube = new GeoTube(0.0, tubePitch/2., tL);
    stube = & ( (*stube) << GeoTrf::RotateX3D(90.*Gaudi::Units::deg) );
    const GeoShape* stubewithcut = NULL;
    if (cutoutNsteps > 1) {
      double toptubelength = cutoutTubeLength[cutoutNsteps-1];
      if (cutoutFullLength[cutoutNsteps-1]) toptubelength = longWidth;
      stubewithcut = new GeoTube(0.0, tubePitch/2., toptubelength/2.0);
      stubewithcut = & ( (*stubewithcut) << GeoTrf::RotateX3D(90.*Gaudi::Units::deg) );
    }

    GeoShape* sbox = new GeoTrd(mdtthickness, mdtthickness, longWidth, 
                                longWidth, tubePitch/2.);
    GeoShape* sboxf = new GeoTrd(mdtthickness, mdtthickness, longWidth, 
                                 longWidth, tubePitch/4.+1*Gaudi::Units::mm);
    slay = &(slay->subtract( (*sbox)<<GeoTrf::Translate3D(0.,0.,length/2.)));

    for (int i = 0; i < nrOfLayers; i++) {
      if (xx[i] > tubePitch/2. + 10.*Gaudi::Units::mm) {
        // subtract tube at the start
        if (verbose_multilayer) std::cout << " Cutting tube at xx = " << yy[i]
                                          << " z = " << -length/2. << std::endl;
        slay = &(slay->subtract( (*stube)<<GeoTrf::Translate3D(-mdtthickness/2.+yy[i],0.,-length/2.) ));
        // add tube at the end
        // distinguish stations with/without cutouts
        if (cutoutNsteps == 1) {
          // no cutouts
          if (verbose_multilayer) std::cout << " Adding tube at xx = " << yy[i]
                                            << " z = " << length/2. << std::endl;
          slay = &(slay->add( (*stube)<<GeoTrf::Translate3D(-mdtthickness/2.+yy[i],0.,length/2.-tubePitch/2.) ));
        } else {
          // there are cutouts
          if (verbose_multilayer) std::cout << " Adding tube at xx = " << yy[i]
                                            << " y(cutout!) = " << cutoutXtubes[cutoutNsteps-1]
                                            << " z = " << length/2. << std::endl;
          slay = &(slay->add( (*stubewithcut)
                             <<GeoTrf::Translate3D(-mdtthickness/2.+yy[i],
                                              cutoutXtubes[cutoutNsteps-1],
                                              length/2.-tubePitch/2.) ));
        }      
      }
    } // Loop over layers
 
    const GeoMaterial* mlay = matManager->getMaterial("std::Air");
    GeoLogVol* llay = new GeoLogVol(logVolName, slay, mlay);
    GeoFullPhysVol* play = new GeoFullPhysVol(llay);

    double foamposition = 0.;
    const GeoShape* sfoam = NULL;
    const GeoMaterial* mfoam = NULL;
    GeoLogVol* lfoam = NULL;
    GeoPhysVol* pfoam = NULL;

    if (foamthicknesslow != 0) {
      foamposition = -(mdtthickness - foamthicknesslow)/2.;
      if (sfoamlow) {
        sfoam = sfoamlow;
      } else {
        sfoam = new GeoTrd(foamthicknesslow/2.-eps, foamthicknesslow/2.-eps,
                           width/2.-eps, longWidth/2.-eps, length/2.);
      }        
      sfoam = &(sfoam->subtract( (*sboxf)<<GeoTrf::Translate3D(0.,0.,length/2.-tubePitch/4.)));
      mfoam = matManager->getMaterial("muo::Foam");
      lfoam = new GeoLogVol("MultiLayerFoam", sfoam, mfoam);

    } else if (foamthicknessup != 0) {
      foamposition = (mdtthickness - foamthicknessup)/2.;
      if (sfoamup) {
        sfoam = sfoamup; 
      } else {
        sfoam = new GeoTrd(foamthicknessup/2.-eps, foamthicknessup/2.-eps, 
                           width/2.-eps, longWidth/2.-eps, length/2.);
      }
      sfoam = &(sfoam->subtract( (*sboxf)<<GeoTrf::Translate3D(0.,0.,length/2.-tubePitch/4.)));
      mfoam = matManager->getMaterial("muo::Foam");
      lfoam = new GeoLogVol("MultiLayerFoam", sfoam, mfoam);

    } else if (logVolName == "BME1MDT09" || logVolName == "BME2MDT09") {
      lfoam = 0;
    } else {
      std::cout<<" no foam thickeness, while it was expected "<<std::endl;
      throw std::runtime_error("ATTENTION:  no foam"); 
    }   
    
    if ( logVolName != "BME1MDT09" && logVolName != "BME2MDT09" ) {  //@@
      pfoam = new GeoPhysVol(lfoam);
      GeoTransform* xf = new GeoTransform (GeoTrf::TranslateX3D(foamposition));
      GeoNameTag* nt = new GeoNameTag(name+" MultiLayerFoam");
      play->add(new GeoIdentifierTag(0));
      play->add(nt);
      play->add(xf);
      play->add(pfoam);
    }
    // Calculation of tube lengths and their positions in layers

    double diff = (longWidth - width)*(length - tubePitch/2.)/length;
    int nrTubesPerStep = nrOfTubes/nrOfSteps; 
    std::vector<GeoVPhysVol*> tubeVector;
    std::vector<bool> internalCutout;
    std::vector<double> tubeDX;
    std::vector<double> tubeL;
    std::vector<int> Ntubes;

    // No cutouts
    if (cutoutNsteps <= 1) {
      for (int j = 0; j < nrOfSteps; j++) {
        tube.length=width+j*diff/nrOfSteps;

        if (verbose_multilayer)std::cout<<" logVolName "<<logVolName<<" step = "<<j
                                        <<" tube length = "<<tube.length<<std::endl;
        tubeVector.push_back(tube.build());
      }

    // Cutouts	
    } else { 
      if (verbose_multilayer)std::cout<<" logVolName "<<logVolName
                                      <<" cutoutNsteps ="<< cutoutNsteps
                                      <<" nsteps "<<nrOfSteps<<std::endl;
      int ntubes = 0;

      // Loop over non-cutout steps

      for (int j = 0; j < nrOfSteps; j++) {
        if (verbose_multilayer) std::cout << " Building tube vectors for step " << j << std::endl;
        double tlength = width + j*diff/nrOfSteps;
        double tlen = 0.;
        double previousTlen = -1.;
	int nTubeToSwitch[5] = {0};
        if (cutoutNsteps > 5) std::abort();
	for (int ii = 0; ii < cutoutNsteps; ii++) {
          if (verbose_multilayer) std::cout << " Building tube vectors for cutout step " << ii << std::endl;
          nTubeToSwitch[ii] = cutoutNtubes[ii] - 1;
          if (ii > 0) {
            for (int k = ii-1; k >= 0; k--) {
              nTubeToSwitch[ii] += cutoutNtubes[k];
            }
          }
          if (verbose_multilayer) std::cout<<" nTubeToSwitch["<<ii<<"]="<<nTubeToSwitch[ii]<<std::endl;
        }
                                                                                                  
        // Loop over tubes in non-cutout step
        for (int it = 0; it < nrTubesPerStep; it++) {
          tlen = tlength;    // Calculated tube length within non-cutout step
          double dx = 0.;

          // For each tube within non-cutout step, find which cutout step it is in      
          int weAreInCutStep=0;
          if (cutoutNsteps > 5) std::abort();
          for (int ic = 0; ic < cutoutNsteps; ic++) {
            if (verbose_multilayer) std::cout << " Loop over cuts ic " << ic
                                              << " FullLength " << cutoutFullLength[ic]
                                              << " ymax " << cutoutYmax[ic] << std::endl;
            if (it + nrTubesPerStep*j > nTubeToSwitch[ic]) weAreInCutStep++;
          }
                                                                                                                          
          if (verbose_multilayer) std::cout << " Tube " << it << " is in cut step "
                                            << weAreInCutStep << std::endl;

          // Override original tube length with cutout tube length and position
          if (nrOfSteps == 1 || !cutoutFullLength[weAreInCutStep]) {
            /*
            // DHW: possible fix?
            double xmin;
            double xmax;
            if (cutoutXtubes[weAreInCutStep]+cutoutTubeLength[weAreInCutStep]/2. < tlength/2. - 20.) {
              xmax = cutoutXtubes[weAreInCutStep] + cutoutTubeLength[weAreInCutStep]/2.;
              xmin = -tlength/2.;
            } else {
              xmax = tlength/2.;
              xmin = cutoutXtubes[weAreInCutStep] - cutoutTubeLength[weAreInCutStep]/2.;
            }
            tlen = xmax - xmin; 
            dx = (xmax + xmin)/2.;
            */
            tlen = cutoutTubeLength[weAreInCutStep];
            dx = cutoutXtubes[weAreInCutStep];
            if (longWidth > width) tlen -= 3;    // temporary fix - see above
//            if (longWidth > width) { 
//              double smallshift = 2.;
//              tlen -= smallshift;        // temporary fix - see above
//              if (cutoutXtubes[weAreInCutStep]+cutoutTubeLength[weAreInCutStep]/2. < tlength/2. - 20.) {
//                dx += smallshift/2.;
//              } else {
//                dx -= smallshift/2.;
//              }
//            }
          }
          if (std::abs(tlen-previousTlen) > 0.001) {
            tube.length=tlen;
            tubeVector.push_back(tube.build());
            if (weAreInCutStep < 1) {
              internalCutout.push_back(false);
            } else {
              internalCutout.push_back(!cutoutFullLength[weAreInCutStep] &&
                                       cutoutYmax[weAreInCutStep] < length - tubePitch/2.);
            }
            tubeDX.push_back(dx);
            tubeL.push_back(tlen);
            previousTlen = tlen;
            if (ntubes > 0) Ntubes.push_back(ntubes);     
            ntubes = 1;

          } else {
            ntubes++;
          }

          if ((j*nrOfSteps + it) == nrOfTubes - 1) Ntubes.push_back(ntubes);
          if (verbose_multilayer) std::cout << " ntubes = " << ntubes << std::endl;
        } // Loop over tubes in non-cutout steps
      } // Loop over non-cutout steps
    } // End cutout section

    // Placement of MDTs in layers

    double lstart;
    double tstart;
    GeoSerialDenominator* sd = new GeoSerialDenominator("DriftTube");
    play->add(sd);

    if (cutoutNsteps < -10000 && cutoutNsteps > -40000) {

      bool internalCutoutBMG[3] = {false, true, false};
      std::array< std::array<int,3>, 4> NtubesBMG;

      if ( cutoutNsteps == -11112 ) // BMG1A12 - ML1
         NtubesBMG = {{ {{23,  7, 24}}, {{24,  7, 23}}, {{24,  7, 23}}, {{25,  7, 22}} }};
      else if ( cutoutNsteps == -11212 ) // BMG1A12 - ML2
         NtubesBMG = {{ {{29,  7, 18}}, {{30,  7, 17}}, {{30,  7, 17}}, {{31,  7, 16}} }};
      else if ( cutoutNsteps == -21112 ) // BMG2A12 - ML1
         NtubesBMG = {{ {{13,  8, 33}}, {{14,  8, 32}}, {{14,  8, 32}}, {{15,  8, 31}} }};
      else if ( cutoutNsteps == -21212 ) // BMG2A12 - ML2
         NtubesBMG = {{ {{25,  8, 21}}, {{26,  8, 20}}, {{26,  8, 20}}, {{27,  8, 19}} }};
      else if ( cutoutNsteps == -31112 ) // BMG3A12 - ML1
         NtubesBMG = {{ {{14, 14, 26}}, {{16, 14, 24}}, {{16, 14, 24}}, {{18, 14, 22}} }};
      else if ( cutoutNsteps == -31212 ) // BMG3A12 - ML2
         NtubesBMG = {{ {{31, 14,  9}}, {{33, 14,  7}}, {{33, 14,  7}}, {{35, 14,  5}} }};
      else if ( cutoutNsteps == -12112 ) // BMG1C12 - ML1
         NtubesBMG = {{ {{24,  6, 24}}, {{23,  7, 24}}, {{24,  7, 23}}, {{24,  7, 23}} }};
      else if ( cutoutNsteps == -12212 ) // BMG1C12 - ML2
         NtubesBMG = {{ {{30,  7, 17}}, {{29,  8, 17}}, {{31,  7, 16}}, {{31,  6, 17}} }};
      else if ( cutoutNsteps == -22112 ) // BMG2C12 - ML1
         NtubesBMG = {{ {{13,  8, 33}}, {{13,  8, 33}}, {{14,  9, 31}}, {{15,  8, 31}} }};
      else if ( cutoutNsteps == -22212 ) // BMG2C12 - ML2
         NtubesBMG = {{ {{25,  8, 21}}, {{25,  8, 21}}, {{27,  8, 19}}, {{27,  8, 19}} }};
      else if ( cutoutNsteps == -32112 ) // BMG3C12 - ML1
         NtubesBMG = {{ {{15, 13, 26}}, {{15, 14, 25}}, {{17, 13, 24}}, {{17, 14, 23}} }};
      else if ( cutoutNsteps == -32212 ) // BMG3C12 - ML2
         NtubesBMG = {{ {{32, 14,  8}}, {{32, 14,  8}}, {{34, 14,  6}}, {{34, 14,  6}} }};
      else if ( cutoutNsteps == -11114 ) // BMG1A14 - ML1
         NtubesBMG = {{ {{24,  6, 24}}, {{23,  7, 24}}, {{24,  7, 23}}, {{24,  7, 23}} }};
      else if ( cutoutNsteps == -11214 ) // BMG1A14 - ML2
         NtubesBMG = {{ {{30,  7, 17}}, {{29,  8, 17}}, {{31,  7, 16}}, {{31,  6, 17}} }};
      else if ( cutoutNsteps == -21114 ) // BMG2A14 - ML1
         NtubesBMG = {{ {{13,  8, 33}}, {{13,  8, 33}}, {{14,  9, 31}}, {{15,  8, 31}} }};
      else if ( cutoutNsteps == -21214 ) // BMG2A14 - ML2
         NtubesBMG = {{ {{25,  8, 21}}, {{25,  8, 21}}, {{27,  8, 19}}, {{27,  8, 19}} }};
      else if ( cutoutNsteps == -31114 ) // BMG3A14 - ML1
         NtubesBMG = {{ {{19,  9, 26}}, {{19, 10, 25}}, {{21,  9, 24}}, {{21, 10, 23}} }};
      else if ( cutoutNsteps == -31214 ) // BMG3A14 - ML2
         NtubesBMG = {{ {{36, 10,  8}}, {{37,  9,  8}}, {{38, 10,  6}}, {{39,  9,  6}} }};
      else if ( cutoutNsteps == -12114 ) // BMG1C14 - ML1
         NtubesBMG = {{ {{23,  7, 24}}, {{24,  7, 23}}, {{24,  7, 23}}, {{25,  7, 22}} }};
      else if ( cutoutNsteps == -12214 ) // BMG1C14 - ML2
         NtubesBMG = {{ {{29,  7, 18}}, {{30,  7, 17}}, {{30,  7, 17}}, {{31,  7, 16}} }};
      else if ( cutoutNsteps == -22114 ) // BMG2C14 - ML1
         NtubesBMG = {{ {{13,  8, 33}}, {{14,  8, 32}}, {{14,  8, 32}}, {{15,  8, 31}} }};
      else if ( cutoutNsteps == -22214 ) // BMG2C14 - ML2
         NtubesBMG = {{ {{25,  8, 21}}, {{26,  8, 20}}, {{26,  8, 20}}, {{27,  8, 19}} }};
      else if ( cutoutNsteps == -32114 ) // BMG3C14 - ML1
         NtubesBMG = {{ {{18, 10, 26}}, {{20, 10, 24}}, {{20, 10, 24}}, {{22, 10, 22}} }};
      else if ( cutoutNsteps == -32214 ) // BMG3C14 - ML2
         NtubesBMG = {{ {{36,  9,  9}}, {{37, 10,  7}}, {{38,  9,  7}}, {{39, 10,  5}} }};
      else {
         NtubesBMG = {{ {{ 0,  0,  0}}, {{ 0,  0,  0}}, {{ 0,  0,  0}}, {{ 0,  0,  0}} }};
         std::cout << "massive error placing tubes for BMG chambers" << std::endl;
         std::abort();
      }

      for (int i = 0; i < nrOfLayers; i++) {
	  if (verbose_multilayer) std::cout << "Tube Layers n. " << i <<std::endl;
        tstart = -mdtthickness/2. + yy[i];
        double loffset = 0.;
        int nttot = 0;
        for (unsigned int j = 0; j < sizeof(NtubesBMG[i])/sizeof(int); j++) {
          // for BMGs tubeVector should be of size 1 with the one default tube
          GeoVPhysVol* tV = tubeVector[0];
          int nt = NtubesBMG[i][j];

	    if (verbose_multilayer) std::cout<< "cutout region "<<j<<" n. of tubes affected should be "<< NtubesBMG[i][j]<<" internal cutout "<<internalCutoutBMG[j] <<std::endl;

          // ususal tube placement
          if (nt > 0 && !internalCutoutBMG[j]) { 
            loffset = nttot*tubePitch;
            lstart = loffset - length/2. + xx[i];
            GeoGenfun::Variable K;
            GeoGenfun::GENFUNCTION f = tubePitch*K + lstart;
            TRANSFUNCTION t = GeoTrf::TranslateY3D(0.)*GeoTrf::RotateX3D(90*Gaudi::Units::deg)*
                              GeoTrf::TranslateX3D(tstart)*Pow(GeoTrf::TranslateY3D(1.0),f);
            GeoSerialTransformer* s = new GeoSerialTransformer(tV,&t,nt);
            play->add(new GeoSerialIdentifier(100*(i+1)+nttot + 1));
            play->add(s);

            nttot = nttot + nt;
            if (verbose_multilayer)
              std::cout << " placing " << nt << " starting at t = " << tstart << " , y = " 
                        << lstart << " with " << nttot << " tubes so far " << std::endl;
          } else {
            nttot = nttot + nt;
            if (verbose_multilayer)
              std::cout << " *NOT* placing " << nt << " starting at t = " << tstart << " , y = " 
                        << lstart << " with " << nttot << " tubes so far " << std::endl;
          }
        }
      } // Loop over layers

    } else if (cutoutNsteps > 1) {
      bool arrowpointoutwards=false;
      bool cutAtAngle = cutoutAtAngle;
      if (xx[1]-xx[0]>0.) 
	{
	  // arrow pointing outwards: like MDT 2 
	  arrowpointoutwards = true;
	}
      for (int i = 0; i < nrOfLayers; i++) {
	if (verbose_multilayer) std::cout<<"Tube Layers n. "<<i<<std::endl;
        tstart = -mdtthickness/2. + yy[i];
        double loffset = 0.;
        int nttot = 0;
        bool nextTimeSubtract = false;
        for (unsigned int j = 0; j < tubeVector.size(); j++) {
          GeoVPhysVol* tV = tubeVector[j];
          double dy = tubeDX[j];
          int nt = Ntubes[j];

	  if (arrowpointoutwards && cutAtAngle)
	    {
	      if (j<tubeVector.size()-1)
		if (internalCutout[j+1])
		  {
		    // next region is the one with the cutout:
		    // for tubeLayer 3 (and 4) must increase the number of tubes in this region by one
		    if (i>1) nt+=1;
		  }
	      if (j>0)
		if (internalCutout[j-1])
		  {
		    // previous region is the one with the cutout:
		    // for tubeLayer 3 (and 4) must decrease the number of tubes in this region by one
		    if (i>1) nt-=1;
		  }
	    }
	  if (verbose_multilayer) std::cout<<"staircasing or cutout region "<<j<<" n. of tubes affected should be "<< Ntubes[j]<<" and are "<<nt<<" internal cutout "<<internalCutout[j]<<" next time subtract "<<nextTimeSubtract<<std::endl;

          if (nt > 0) { 
            loffset = nttot*tubePitch;
            lstart = loffset - length/2. + xx[i];
            GeoGenfun::Variable K;
            GeoGenfun::GENFUNCTION f = tubePitch*K + lstart;
            TRANSFUNCTION t = GeoTrf::TranslateY3D(dy)*GeoTrf::RotateX3D(90*Gaudi::Units::deg)*
                              GeoTrf::TranslateX3D(tstart)*Pow(GeoTrf::TranslateY3D(1.0),f);
            GeoSerialTransformer* s = new GeoSerialTransformer(tV,&t,nt);
            play->add(new GeoSerialIdentifier(100*(i+1)+nttot + 1));
            play->add(s);

            nttot = nttot + nt;
            if (verbose_multilayer)
              std::cout << " placing " << nt << " tubes of length " << tubeL[j] 
                        << " starting at t = " << tstart << " , y = " << lstart 
                        << " and x = " << dy << " with " << nttot 
                        << " tubes so far " << std::endl;
          } 
        }
      } // Loop over layers

    } else if (nrOfSteps == 1) {
      // At this point no cutouts and only one non-cutout step
      for (int i = 0; i < nrOfLayers; i++) {
        tstart = -mdtthickness/2. + yy[i];
        lstart = -length/2. + xx[i];
//        std::cout << " Tubes starting at t = " << tstart << " , y = " << lstart
//                  << std::endl;
        GeoGenfun::Variable K;
        GeoGenfun::GENFUNCTION f = tubePitch*K + lstart;
        TRANSFUNCTION t = GeoTrf::RotateX3D(90*Gaudi::Units::deg)*GeoTrf::TranslateX3D(tstart)*
                          Pow(GeoTrf::TranslateY3D(1.0),f);
        GeoVPhysVol* tV = tubeVector[0];	
        GeoSerialTransformer* s = new GeoSerialTransformer(tV,&t,nrOfTubes);
	play->add(new GeoSerialIdentifier(100*(i+1)+1));
        play->add(s);
      }
    } else {
      // Here, no cutouts but multiple non-cutout steps
      lstart = 0.;
      for (int i = 0; i < nrOfLayers; i++) {
        tstart = -mdtthickness/2. + yy[i];
        double loffset = 0.;
        for (int j = 0; j < nrOfSteps; j++) {
          GeoVPhysVol* tV = tubeVector[j];
          loffset = j*nrTubesPerStep*tubePitch;
          lstart = loffset - length/2. + xx[i]; 
          GeoGenfun::Variable K;
          GeoGenfun::GENFUNCTION f = tubePitch*K + lstart;
          TRANSFUNCTION t = GeoTrf::RotateX3D(90*Gaudi::Units::deg)*GeoTrf::TranslateX3D(tstart)*
                            Pow(GeoTrf::TranslateY3D(1.0),f);
          GeoSerialTransformer* s = new GeoSerialTransformer(tV,&t,nrTubesPerStep);
          play->add(new GeoSerialIdentifier(100*(i+1)+j*nrTubesPerStep + 1));
          play->add(s);      
        }
      }
    }
 
    return play;	
}


void MultiLayer::print()
{
  std::cout << "Multi Layer " << name.c_str() << " :" << std::endl;

}
} // namespace MuonGM


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonGeoModel/Tgc.h"
#include "MuonGeoModel/TGC_Technology.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/TgcComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GaudiKernel/SystemOfUnits.h"

#define skip_tgc false

namespace MuonGM {

Tgc::Tgc(Component* ss): DetectorElement(ss->name),
  irad(0.), orad(0.), dphi(0.)
{
  TgcComponent* s = (TgcComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  length = s->dy;
  thickness = s->GetThickness();
  index = s->index;
}


GeoFullPhysVol* Tgc::build(int minimalgeo)
{
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}


GeoFullPhysVol* 
Tgc::build(int minimalgeo, int cutoutson, std::vector<Cutout*> vcutdef)
{
  MYSQL* mysql = MYSQL::GetPointer();	
  TGC* t = (TGC*) mysql->GetTechnology(name);
  thickness = t->thickness;

  // Build TGC mother volume out of G10
  const GeoShape* strd = new GeoTrd(thickness/2, thickness/2, width/2, 
                                    longWidth/2, length/2);
  if (cutoutson && vcutdef.size() > 0) {
    Cutout* cut = 0;
    GeoShape* cutoutShape = 0;
    GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
    for (unsigned i = 0; i < vcutdef.size(); i++) {
      cut = vcutdef[i];
      cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                               cut->widthXs/2., cut->widthXl/2., cut->lengthY/2.);
      cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);
      strd = &(strd->subtract( (*cutoutShape) << cutTrans) );
    }
//    std::cout << " Tgc cutouts" << std::endl;
//      for (unsigned i = 0; i < vcutdef.size(); i++) std::cout
//         << " x = " << vcutdef[i]->dx << " y = " << vcutdef[i]->dy
//         << " width = " << vcutdef[i]->widthXs << " length = " << vcutdef[i]->lengthY
//         << std::endl;
  }

  const GeoMaterial* mtrd = matManager->getMaterial("std::G10");
  GeoLogVol* ltrd = new GeoLogVol(logVolName, strd, mtrd);
  GeoFullPhysVol* ptrd = new GeoFullPhysVol(ltrd);

  if (minimalgeo == 1) return ptrd;

  double newpos = -thickness/2.;
  if (!skip_tgc) ptrd->add(new GeoSerialIdentifier(0));

  int igl = 0;
  int iSenLyr = 0;

  // Loop over TGC layers
  for (int i = 0; i < t->nlayers; i++) {
    double widthActive;
    double longWidthActive;
    double lengthActive;

    if (t->materials[i] == "muo::TGCGas") { 
      // sensitive volume with wire/button support
      igl++;
      if (!skip_tgc) ptrd->add(new GeoIdentifierTag(igl));

      widthActive = width - (t->frame_ab)*2;
      longWidthActive = longWidth - (t->frame_ab)*2;
      lengthActive = length - (t->frame_h)*2;
//      std::cout << " Gas in loop " << i << " : frame_ab = " << t->frame_ab
//                << " frame_h = " << t->frame_h << " tilt angle = " << t->angleTilt
//                << " wire width support = " << t->widthWireSupport
//                << " offset wire support = " << t->offsetWireSupport[iSenLyr]
//                << " wire support distance = " << t->distanceWireSupport << std::endl;
 
      const GeoShape* sGasVolume = new GeoTrd(t->tck[i]/2, t->tck[i]/2, widthActive/2, 
                                              longWidthActive/2, lengthActive/2);

      if (t->widthWireSupport != 0. && t->radiusButton != 0.) { 
        // No supports for CTB simulation since their parameters are not in the DB
	// Build wire supports and button supports in sensitive volume

	// wire supports
	GeoTrd* strdsup = new GeoTrd(t->tck[i]/2, t->tck[i]/2, t->widthWireSupport/2,
                                     t->widthWireSupport/2, lengthActive/2-0.1*Gaudi::Units::mm);
	// button supports
	GeoTube* stubesup = new GeoTube(0., t->radiusButton,
					    t->tck[i]/2.+0.005*Gaudi::Units::mm);
	GeoTrf::RotateY3D rotY(3.14159264/2.*Gaudi::Units::rad);
	  
	int iymin = int( -(widthActive/2. + lengthActive*tan(t->angleTilt) - t->widthWireSupport/2.
			  + t->offsetWireSupport[iSenLyr])/t->distanceWireSupport );
	int iymax = int(  (widthActive/2. + lengthActive*tan(t->angleTilt) - t->widthWireSupport/2.
			  - t->offsetWireSupport[iSenLyr])/t->distanceWireSupport );

        for (int isup = iymin; isup <= iymax; isup++) { // loop over wire supports
	  // place wire supports
	  double sign = 1.;
	  if (t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*isup > 0.) {
            sign = -1.;
	  } else if (t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*isup == 0.){
            sign = 0;
	  }
	  GeoTrf::RotateX3D rotX(sign*t->angleTilt);
	  GeoTrf::Translate3D vtransWS(0.,
				       t->offsetWireSupport[iSenLyr] + t->distanceWireSupport*isup + lengthActive/2.*tan(sign*t->angleTilt),
				       0.);

          sGasVolume = &(sGasVolume->subtract((*strdsup) << GeoTrf::Transform3D(vtransWS*rotX)));

	  // place button supports
          int izmin, izmax;
          izmax = int( (lengthActive/2.-t->radiusButton)/(t->pitchButton[1]/2.) );
          izmin = -izmax;

          int nBS = 0;
          double yposCentre[2], angleTiltButton[2];
          if (isup < iymax) { // button support in rectangular sensitive area
            double yLongBase, yShortBase;
            yLongBase = ( (t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*(isup+1))
                          +(t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*isup)    )/2.;
            if (t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*(isup+1) < 0.) {
              yShortBase = yLongBase + lengthActive*tan( t->angleTilt);
            } else if (t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*(isup) > 0.) {
              yShortBase = yLongBase + lengthActive*tan(-t->angleTilt);
            } else {
              yShortBase = yLongBase;
            }

            nBS = 1;
            yposCentre[0] = yLongBase;
            angleTiltButton[0] = atan((yShortBase - yLongBase)/lengthActive);

          } else if (isup == iymax) {
            // place extra wire supports for TGC01, TGC06 and TGC12
            if (name == "TGC01" || name == "TGC06" || name == "TGC12") {
              for (int iside = 0; iside < 2; iside++) {
                int isupy;
                if (iside == 0) {
                  isupy = iymin-1;
                  sign = 1.;
                } else {
                  isupy = iymax+1;
                  sign = -1.;
		}
                double lengthWireSupportEx;
                lengthWireSupportEx = lengthActive/(longWidthActive/2.-widthActive/2.)
		                         *( longWidthActive/2.
					   -fabs( t->distanceWireSupport*isupy
						 +t->offsetWireSupport[iSenLyr]));
                GeoTrd* strdsupex = new GeoTrd(t->tck[i]/2, t->tck[i]/2,
                                               t->widthWireSupport/2,
                                               t->widthWireSupport/2,
                                               lengthWireSupportEx/2-0.1*Gaudi::Units::mm);
                GeoTrf::Translate3D vtrans(0.,
					   t->offsetWireSupport[iSenLyr]+t->distanceWireSupport*isupy + lengthActive/2.*tan(sign*t->angleTilt),
					   (lengthActive-lengthWireSupportEx)/2.);

                sGasVolume = &(sGasVolume->subtract((*strdsupex) << GeoTrf::Transform3D(vtrans * rotX)));
              }
            } // End special case for TGC01, 06, 12

            // button supports in trapezoidal sensitive area
            double widthLeftTrd = (t->distanceWireSupport*iymin
	  			 + t->offsetWireSupport[iSenLyr]
				       +lengthActive*tan( t->angleTilt))
		                      -(-widthActive/2.);
            if (widthLeftTrd > 8.*Gaudi::Units::cm) {
              double yLongBase;
              if ((name == "TGC01" || name == "TGC06" || name == "TGC12") &&
	   	      t->distanceWireSupport*(iymin-1)+t->offsetWireSupport[iSenLyr]
		      > -longWidthActive/2.) {
		    yLongBase = ( ( t->distanceWireSupport*(iymin-1)
				   +t->offsetWireSupport[iSenLyr])
				 +( t->distanceWireSupport*iymin
				   +t->offsetWireSupport[iSenLyr])   )/2.;
              } else {
		    yLongBase = ( ( t->distanceWireSupport*iymin
				   +t->offsetWireSupport[iSenLyr])
				 +(-longWidthActive/2.)           )/2.;
              }
              double yShortBase= ( ( t->distanceWireSupport*iymin
					+t->offsetWireSupport[iSenLyr]
					+lengthActive*tan( t->angleTilt))
				      +(-widthActive/2.)              )/2.;
              yposCentre[nBS] = yLongBase;
              angleTiltButton[nBS] = atan((yShortBase-yLongBase)/lengthActive);
              nBS++;
            }

            double widthRightTrd = widthActive/2.
		                      -( t->distanceWireSupport*iymax 
					+t->offsetWireSupport[iSenLyr]
					+lengthActive*tan(-t->angleTilt));
	      
            if (widthRightTrd > 8.*Gaudi::Units::cm) {
              double yLongBase;
              if ((name == "TGC01" || name == "TGC06" || name == "TGC12") &&
		      t->distanceWireSupport*(iymax+1)+t->offsetWireSupport[iSenLyr]
		      < longWidthActive/2.) {
		    yLongBase = ( ( t->distanceWireSupport*(iymax+1)
				   +t->offsetWireSupport[iSenLyr])
				 +( t->distanceWireSupport*iymax
				   +t->offsetWireSupport[iSenLyr])   )/2.;
              } else {
		    yLongBase = ( ( t->distanceWireSupport*iymax
				   +t->offsetWireSupport[iSenLyr])
				 + longWidthActive/2.             )/2.;
              }
              double yShortBase= ( ( t->distanceWireSupport*iymax
					+t->offsetWireSupport[iSenLyr]
					+lengthActive*tan(-t->angleTilt))
				      + widthActive/2.                )/2.;
              yposCentre[nBS] = yLongBase;
              angleTiltButton[nBS] = atan((yShortBase-yLongBase)/lengthActive);
              nBS++;
            }
	  
            if (nBS == 0) { // no button support in trapezoidal sensitive area
              break;        // leave isup loop
            }
          } // isup, iymax
          int totNBS = 0;
          for (int iBS = 0; iBS < nBS; iBS++) {
            for (int isupz = izmin; isupz <= izmax; isupz++) {
              totNBS += nBS;
              double yposleft = yposCentre[iBS] - t->pitchButton[0]/2.
                     + (lengthActive/2.-t->pitchButton[1]/2.*isupz)*tan(angleTiltButton[iBS]);
              GeoTrf::Translate3D vtransBS(0., yposleft + t->pitchButton[0]*std::abs(isupz%2),
					   t->pitchButton[1]/2.*isupz);
              sGasVolume = &(sGasVolume->subtract( (*stubesup) << GeoTrf::Transform3D(vtransBS*rotY)));
            }
          }
        } // loop over wire supports
      } // No supports for CTB Simulation

      if (cutoutson && vcutdef.size() > 0) {
        // Make cutout in gas volume a bit larger so that G10 of mother volume
        // makes a gas boundary
        Cutout* cut = 0;
        GeoShape* cutoutShape = 0;
        GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
        for (unsigned i = 0; i < vcutdef.size(); i++) {
          cut = vcutdef[i];
          cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                   cut->widthXs/2. + t->frame_ab/2., 
                                   cut->widthXl/2. + t->frame_ab/2., 
                                   cut->lengthY/2. + t->frame_h/2.);
          cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);
          sGasVolume = &(sGasVolume->subtract( (*cutoutShape) << cutTrans) );
        }
      }

      GeoLogVol* ltrdtmp = new GeoLogVol(t->materials[i], sGasVolume,
                                         matManager->getMaterial(t->materials[i]));
      GeoPhysVol* ptrdtmp = new GeoPhysVol(ltrdtmp);
      GeoNameTag* ntrdtmp = new GeoNameTag(name+t->materials[i]);
      GeoTransform* ttrdtmp = new GeoTransform(GeoTrf::TranslateX3D(newpos + (t->tck[i]/2)));

      // Place gas volume inside G10 mother volume so that
      // subtractions from gas volume now become G10
      if (!skip_tgc) { 
        ptrd->add(ntrdtmp);
        ptrd->add(ttrdtmp);
        ptrd->add(ptrdtmp);
      }

      iSenLyr++;

    } else if (t->materials[i] != "std:G10") {
      // passive structure, Copper, Honeycomb,
      // except for G10 which is material of the mother volume
      const GeoShape* strdtmp = new GeoTrd(t->tck[i]/2, t->tck[i]/2, width/2, 
                                   longWidth/2, length/2);
      if (cutoutson && vcutdef.size() > 0) {
        Cutout* cut = 0;
        GeoShape* cutoutShape = 0;
        GeoTrf::Transform3D cutTrans{GeoTrf::Transform3D::Identity()};
        for (unsigned i = 0; i < vcutdef.size(); i++) {
          cut = vcutdef[i];
          cutoutShape = new GeoTrd(thickness/2.+1., thickness/2.+1.,
                                   cut->widthXs/2., cut->widthXl/2., cut->lengthY/2.);
          cutTrans = GeoTrf::Translate3D(0.0, cut->dx, -length/2 + cut->dy + cut->lengthY/2.);
          strdtmp = &(strdtmp->subtract( (*cutoutShape) << cutTrans) );
        }
      }
      GeoLogVol* ltrdtmp = new GeoLogVol(t->materials[i], strdtmp,
                                         matManager->getMaterial(t->materials[i]));
      GeoPhysVol* ptrdtmp = new GeoPhysVol(ltrdtmp);
      GeoNameTag* ntrdtmp = new GeoNameTag(name+t->materials[i]);
      GeoTransform* ttrdtmp = new GeoTransform(GeoTrf::TranslateX3D(newpos+ (t->tck[i]/2)));

      if (!skip_tgc) { 
        ptrd->add(ntrdtmp);
        ptrd->add(ttrdtmp);
        ptrd->add(ptrdtmp);
      }
    } // Active or passive layer

    newpos += t->tck[i];
  } // Loop over tgc layers
        
  return ptrd;	
}


void Tgc::print()
{
  std::cout << " Tgc " << name << " :" << std::endl;
}

} // namespace MuonGM

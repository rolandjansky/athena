/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModelFactory.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"

#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <iostream>
#include <fstream>
#include <string>

#include <math.h>
#include <algorithm>

#include <list>
#include <map>

#include <stdlib.h>



void AFP_GeoModelFactory::AddShortHamburgPipe(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
	char szLabel[32];

	sprintf(szLabel,"%s_LogShortHamburgPipe",pszStationName);
	GeoShape* pShortHamburgPipeSol=GetSolidShortHamburgPipe();
        const GeoLogVol* hamburgPipeSLog = new GeoLogVol(szLabel, pShortHamburgPipeSol, m_MapMaterials[std::string(m_CfgParams.hbpcfg.setMaterialToBeryllium ? "Beryllium" : "Steel")]);
	GeoPhysVol* hamburgPipeSPhys   = new GeoPhysVol(hamburgPipeSLog);

	sprintf(szLabel,"%s_ShortHamburgPipe",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransInMotherVolume*HepGeom::RotateZ3D((DETXSIDE==+1)? -180*CLHEP::deg:0.0*CLHEP::deg)));
	pPhysMotherVol->add(hamburgPipeSPhys);
}

void AFP_GeoModelFactory::AddLongHamburgPipe(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
	char szLabel[32];

	sprintf(szLabel,"%s_LogLongHamburgPipe",pszStationName);
	GeoShape* pLongHamburgPipeSol=GetSolidLongHamburgPipe();
        const GeoLogVol* hamburgPipeSLog = new GeoLogVol(szLabel, pLongHamburgPipeSol, m_MapMaterials[std::string(m_CfgParams.hbpcfg.setMaterialToBeryllium ? "Beryllium" : "Steel")]);
	GeoPhysVol* hamburgPipeSPhys   = new GeoPhysVol(hamburgPipeSLog);

	sprintf(szLabel,"%s_LongHamburgPipe",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransInMotherVolume*HepGeom::RotateZ3D((DETXSIDE==+1)? -180*CLHEP::deg:0.0*CLHEP::deg)));
	pPhysMotherVol->add(hamburgPipeSPhys);
}

GeoShape* AFP_GeoModelFactory::GetSolidLongHamburgPipe()
{
	// Hamburg beampipe
	double thickness = 3*CLHEP::mm; // thickess of walls
	double thickness2 = 2*CLHEP::mm; // thickness of the flat plane (floor ?)

	// Common box with round corners
	const GeoBox* halftubeFull = new GeoBox(35*CLHEP::mm+0.5*thickness,50*CLHEP::mm+thickness,420*CLHEP::mm);
	const GeoBox* halftubeIn0 = new GeoBox(35*CLHEP::mm,50*CLHEP::mm,420*CLHEP::mm-thickness);
	HepGeom::Transform3D moveHTubeIn = HepGeom::Translate3D(-thickness,0,0);
	const GeoShapeShift& halftubeIn = (*halftubeIn0)<<moveHTubeIn;
	GeoShapeSubtraction* halftube0 = new GeoShapeSubtraction(halftubeFull,&halftubeIn);
	const GeoTubs* cornerCutterH10 = new GeoTubs(18.05*CLHEP::mm+thickness, 35*CLHEP::mm, 420.1*CLHEP::mm, 0*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerCutterH20 = new GeoTubs(18.05*CLHEP::mm+thickness, 35*CLHEP::mm, 420.1*CLHEP::mm, 270*CLHEP::deg, 90*CLHEP::deg);
	HepGeom::Transform3D moveCutterH1 = HepGeom::Translate3D(17*CLHEP::mm, 32*CLHEP::mm, 0);
	HepGeom::Transform3D moveCutterH2 = HepGeom::Translate3D(17*CLHEP::mm, -32*CLHEP::mm, 0);
	const GeoShapeShift& cornerCutterH1 = (*cornerCutterH10)<<moveCutterH1;
	const GeoShapeShift& cornerCutterH2 = (*cornerCutterH20)<<moveCutterH2;
	GeoShapeSubtraction* halftube01 = new GeoShapeSubtraction(new GeoShapeSubtraction(halftube0, &cornerCutterH2), &cornerCutterH1);
	const GeoTubs* cornerAddH10 = new GeoTubs(18*CLHEP::mm, 18*CLHEP::mm+thickness, 420.1*CLHEP::mm, 0*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerAddH20 = new GeoTubs(18*CLHEP::mm, 18*CLHEP::mm+thickness, 420.1*CLHEP::mm, 270*CLHEP::deg, 90*CLHEP::deg);
	const GeoShapeShift& cornerAddH1 = (*cornerAddH10)<<moveCutterH1;
	const GeoShapeShift& cornerAddH2 = (*cornerAddH20)<<moveCutterH2;
	GeoShapeSubtraction* halftube02 = new GeoShapeSubtraction(new GeoShapeSubtraction(halftube01, &cornerAddH2), &cornerAddH1);
	HepGeom::Transform3D moveHTube = HepGeom::Translate3D(35*CLHEP::mm+2*thickness2,0,0);
	const GeoShapeShift& halftube = (*halftube02)<<moveHTube;

	// Plane covering the middle part (detector housing)
	const GeoBox* plane0 = new GeoBox(0.5*thickness2, 50*CLHEP::mm+thickness, 350*CLHEP::mm);
	HepGeom::Transform3D movePlane = HepGeom::Translate3D(0.5*thickness2,0,0);
	const GeoShapeShift& plane = (*plane0)<<movePlane;

	// Floor thinning
	const GeoTrd* floor0 = new GeoTrd(360*CLHEP::mm, 360*CLHEP::mm, 8*CLHEP::mm, 20*CLHEP::mm, 0.5*(thickness2-0.3*CLHEP::mm));
	HepGeom::Transform3D rotateFloor = HepGeom::RotateY3D(90*CLHEP::deg);
	HepGeom::Transform3D moveFloor = HepGeom::Translate3D(0.5*thickness2+0.3*CLHEP::mm,0,0);
	const GeoShapeShift& floor = (*floor0)<<rotateFloor<<moveFloor;

	// Thin window
        const GeoTrd* window10 = new GeoTrd(11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 14*CLHEP::mm, 26*CLHEP::mm, 0.5*(thickness-HBPTHINWINDOWTHICKNESS));
        const GeoTrd* window20 = new GeoTrd(11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 26*CLHEP::mm, 14*CLHEP::mm, 0.5*(thickness-HBPTHINWINDOWTHICKNESS));
	const GeoBox* windowCutSide0 = new GeoBox(11.1*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 6*CLHEP::mm, 0.5*(thickness-0.25*CLHEP::mm)+0.1*CLHEP::mm);
	const GeoBox* windowCutCorner0 = new GeoBox(35*CLHEP::mm, 20*CLHEP::mm, 0.5*(thickness-0.25*CLHEP::mm)+0.1*CLHEP::mm);
	HepGeom::Transform3D moveCutSide = HepGeom::Translate3D(0,HBPWINDOWCUTYSIGN*20*CLHEP::mm,0);
	const GeoShapeShift& windowCutSide = (*windowCutSide0)<<moveCutSide;
	HepGeom::Transform3D moveCutCorner = HepGeom::Translate3D(11*CLHEP::mm-0.5*(thickness2-0.3*CLHEP::mm)+0.5*sqrt(2)*20*CLHEP::mm,HBPWINDOWCUTYSIGN*(2*CLHEP::mm+0.5*sqrt(2)*20*CLHEP::mm),0);
	HepGeom::Transform3D rotateCutCorner = HepGeom::RotateZ3D(HBPWINDOWCUTYSIGN*-45*CLHEP::deg);
	const GeoShapeShift& windowCutCorner = (*windowCutCorner0)<<rotateCutCorner<<moveCutCorner;
	const GeoShapeSubtraction& window11 = window10->subtract(windowCutCorner);
	const GeoShapeSubtraction* window12 = new GeoShapeSubtraction(&window11, &windowCutSide);
	const GeoShapeSubtraction& window21 = window20->subtract(windowCutCorner);
	const GeoShapeSubtraction* window22 = new GeoShapeSubtraction(&window21, &windowCutSide);
        HepGeom::Transform3D moveWindow1 = HepGeom::Translate3D(-11*CLHEP::mm+0.5*(thickness2+0.3*CLHEP::mm), HBPWINDOWCUTYSIGN*6*CLHEP::mm, 350*CLHEP::mm+0.5*thickness+HBPTHINWINDOWTHICKNESS);
        HepGeom::Transform3D moveWindow2 = HepGeom::Translate3D(-11*CLHEP::mm+0.5*(thickness2+0.3*CLHEP::mm), HBPWINDOWCUTYSIGN*6*CLHEP::mm, -350*CLHEP::mm-0.5*thickness-HBPTHINWINDOWTHICKNESS);
	const GeoShapeShift& window1 = (*window12)<<moveWindow1;
	const GeoShapeShift& window2 = (*window22)<<moveWindow2;

	// Boxes with round corners on ends
	const GeoBox* endsFull0 = new GeoBox(25.85*CLHEP::mm, 50*CLHEP::mm+thickness, 35*CLHEP::mm);
	const GeoBox* endsIn0 = new GeoBox(25.85*CLHEP::mm-0.5*thickness, 50*CLHEP::mm, 35*CLHEP::mm-thickness);
	HepGeom::Transform3D moveEndIn = HepGeom::Translate3D(thickness,0,0);
	const GeoShapeShift& endsIn = (*endsIn0)<<moveEndIn;
	GeoShapeSubtraction* ends0 = new GeoShapeSubtraction(endsFull0,&endsIn);
	const GeoTubs* cornerCutter10 = new GeoTubs(9*CLHEP::mm+thickness, 20*CLHEP::mm, 35.1*CLHEP::mm, 90*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerCutter20 = new GeoTubs(9*CLHEP::mm+thickness, 20*CLHEP::mm, 35.1*CLHEP::mm, 180*CLHEP::deg, 90*CLHEP::deg);
	HepGeom::Transform3D moveCutter1 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness)*CLHEP::mm, 41*CLHEP::mm, 0);
	HepGeom::Transform3D moveCutter2 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness)*CLHEP::mm, -(41*CLHEP::mm), 0);
	HepGeom::Transform3D moveAdd1 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness), 41*CLHEP::mm, 0);
	HepGeom::Transform3D moveAdd2 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness), -(41*CLHEP::mm), 0);
	const GeoShapeShift& cornerCutter1 = (*cornerCutter10)<<moveCutter1;
	const GeoShapeShift& cornerCutter2 = (*cornerCutter20)<<moveCutter2;
	GeoShapeSubtraction* ends01 = new GeoShapeSubtraction(new GeoShapeSubtraction(ends0, &cornerCutter2), &cornerCutter1);
	const GeoTubs* cornerAdd10 = new GeoTubs(9*CLHEP::mm, 9*CLHEP::mm+thickness, 35*CLHEP::mm, 90*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerAdd20 = new GeoTubs(9*CLHEP::mm, 9*CLHEP::mm+thickness, 35*CLHEP::mm, 180*CLHEP::deg, 90*CLHEP::deg);
	const GeoShapeShift& cornerAdd1 = (*cornerAdd10)<<moveAdd1;
	const GeoShapeShift& cornerAdd2 = (*cornerAdd20)<<moveAdd2;
	GeoShapeSubtraction* ends = new GeoShapeSubtraction(new GeoShapeSubtraction(ends01, &cornerAdd2), &cornerAdd1);
	HepGeom::Transform3D moveEnd1 = HepGeom::Translate3D(-25.9*CLHEP::mm+thickness2, 0, 385.1*CLHEP::mm);
	HepGeom::Transform3D rotateEnd2 = HepGeom::RotateX3D(180*CLHEP::deg);
	const GeoShapeShift& end1 = (*ends)<<moveEnd1;
	const GeoShapeShift& end2 = (*ends)<<moveEnd1<<rotateEnd2;

	// Holes for the beam pipe
	const GeoTube* hole10 = new GeoTube(0, 40*CLHEP::mm-0.1*CLHEP::mm, 0.55*thickness);
	const GeoTube* hole20 = new GeoTube(0, 40*CLHEP::mm-0.1*CLHEP::mm, 0.55*thickness);
	HepGeom::Transform3D moveHole1 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, 420*CLHEP::mm-0.5*thickness);
	HepGeom::Transform3D moveHole2 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, -(420*CLHEP::mm-0.5*thickness));
	const GeoShapeShift& hole1 = (*hole10)<<moveHole1;
	const GeoShapeShift& hole2 = (*hole20)<<moveHole2;


	// Beam pipe in and out
	const GeoTube* tube10 = new GeoTube(40*CLHEP::mm, 40*CLHEP::mm+thickness, 75*CLHEP::mm);
	const GeoTube* tube20 = new GeoTube(40*CLHEP::mm, 40*CLHEP::mm+thickness, 75*CLHEP::mm);
	HepGeom::Transform3D moveTube1 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, 495.05*CLHEP::mm);
	HepGeom::Transform3D moveTube2 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, -495.05*CLHEP::mm);
	const GeoShapeShift& tube1 = (*tube10)<<moveTube1;
	const GeoShapeShift& tube2 = (*tube20)<<moveTube2;

	// Putting all together
	GeoShapeUnion* hamburgPipe01 = new GeoShapeUnion(&halftube, &plane);
	GeoShapeUnion* hamburgPipe02 = new GeoShapeUnion(hamburgPipe01, &end1);
	GeoShapeUnion* hamburgPipe03 = new GeoShapeUnion(hamburgPipe02, &end2);
	GeoShapeSubtraction* hamburgPipe04 = new GeoShapeSubtraction(hamburgPipe03, &hole1);
	GeoShapeSubtraction* hamburgPipe05 = new GeoShapeSubtraction(hamburgPipe04, &hole2);
	GeoShapeUnion* hamburgPipe06 = new GeoShapeUnion(hamburgPipe05, &tube1);
	GeoShapeUnion* hamburgPipe07 = new GeoShapeUnion(hamburgPipe06, &tube2);
	GeoShapeSubtraction* hamburgPipe08 = new GeoShapeSubtraction(hamburgPipe07, &window1);
	GeoShapeSubtraction* hamburgPipe09 = new GeoShapeSubtraction(hamburgPipe08, &window2);
	GeoShapeSubtraction* hamburgPipe10 = new GeoShapeSubtraction(hamburgPipe09, &floor);

	return hamburgPipe10;
}

GeoShape* AFP_GeoModelFactory::GetSolidShortHamburgPipe()
{
	// Hamburg beampipe thicknesses are the same as for the long pipe
	double thickness = 3*CLHEP::mm; // thickess of walls
	double thickness2 = 2*CLHEP::mm; // thickness of the flat plane

	// Common box with round corners
	const GeoBox* halftubeSFull = new GeoBox(35*CLHEP::mm+0.5*thickness,50*CLHEP::mm+thickness,120*CLHEP::mm);
	const GeoBox* halftubeSIn0 = new GeoBox(35*CLHEP::mm,50*CLHEP::mm,120*CLHEP::mm-thickness);
	HepGeom::Transform3D moveHSTubeIn = HepGeom::Translate3D(-thickness,0,0);
	const GeoShapeShift& halftubeSIn = (*halftubeSIn0)<<moveHSTubeIn;
	GeoShapeSubtraction* halftubeS0 = new GeoShapeSubtraction(halftubeSFull,&halftubeSIn);
	const GeoTubs* cornerCutterHS10 = new GeoTubs(18.05*CLHEP::mm+thickness, 35*CLHEP::mm, 120.1*CLHEP::mm, 0*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerCutterHS20 = new GeoTubs(18.05*CLHEP::mm+thickness, 35*CLHEP::mm, 120.1*CLHEP::mm, 270*CLHEP::deg, 90*CLHEP::deg);
	HepGeom::Transform3D moveCutterHS1 = HepGeom::Translate3D(17*CLHEP::mm, 32*CLHEP::mm, 0);
	HepGeom::Transform3D moveCutterHS2 = HepGeom::Translate3D(17*CLHEP::mm, -32*CLHEP::mm, 0);
	const GeoShapeShift& cornerCutterHS1 = (*cornerCutterHS10)<<moveCutterHS1;
	const GeoShapeShift& cornerCutterHS2 = (*cornerCutterHS20)<<moveCutterHS2;
	GeoShapeSubtraction* halftubeS01 = new GeoShapeSubtraction(new GeoShapeSubtraction(halftubeS0, &cornerCutterHS2), &cornerCutterHS1);
	const GeoTubs* cornerAddHS10 = new GeoTubs(18*CLHEP::mm, 18*CLHEP::mm+thickness, 120.1*CLHEP::mm, 0*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerAddHS20 = new GeoTubs(18*CLHEP::mm, 18*CLHEP::mm+thickness, 120.1*CLHEP::mm, 270*CLHEP::deg, 90*CLHEP::deg);
	const GeoShapeShift& cornerAddHS1 = (*cornerAddHS10)<<moveCutterHS1;
	const GeoShapeShift& cornerAddHS2 = (*cornerAddHS20)<<moveCutterHS2;
	GeoShapeSubtraction* halftubeS02 = new GeoShapeSubtraction(new GeoShapeSubtraction(halftubeS01, &cornerAddHS2), &cornerAddHS1);
	HepGeom::Transform3D moveHSTube = HepGeom::Translate3D(35*CLHEP::mm+2*thickness2,0,0);
	const GeoShapeShift& halftubeS = (*halftubeS02)<<moveHSTube;

	// Plane covering the middle part (detector housing)
	const GeoBox* planeS0 = new GeoBox(0.5*thickness2, 50*CLHEP::mm+thickness, 50*CLHEP::mm);
	HepGeom::Transform3D movePlaneS = HepGeom::Translate3D(0.5*thickness2,0,0);
	const GeoShapeShift& planeS = (*planeS0)<<movePlaneS;

	// Floor thinning
	const GeoTrd* floor0 = new GeoTrd(60*CLHEP::mm, 60*CLHEP::mm, 8*CLHEP::mm, 20*CLHEP::mm, 0.5*(thickness2-0.3*CLHEP::mm));
	HepGeom::Transform3D rotateFloor = HepGeom::RotateY3D(90*CLHEP::deg);
	HepGeom::Transform3D moveFloor = HepGeom::Translate3D(0.5*thickness2+0.3*CLHEP::mm,0,0);
	const GeoShapeShift& floor = (*floor0)<<rotateFloor<<moveFloor;

	// Thin window
        const GeoTrd* window10 = new GeoTrd(11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 14*CLHEP::mm, 26*CLHEP::mm, 0.5*(thickness-HBPTHINWINDOWTHICKNESS));
        const GeoTrd* window20 = new GeoTrd(11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 11*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 26*CLHEP::mm, 14*CLHEP::mm, 0.5*(thickness-HBPTHINWINDOWTHICKNESS));
	const GeoBox* windowCutSide0 = new GeoBox(11.1*CLHEP::mm+0.5*(thickness2-0.3*CLHEP::mm), 6*CLHEP::mm, 0.5*(thickness-0.25*CLHEP::mm)+0.1*CLHEP::mm);
	const GeoBox* windowCutCorner0 = new GeoBox(35*CLHEP::mm, 20*CLHEP::mm, 0.5*(thickness-0.25*CLHEP::mm)+0.1*CLHEP::mm);
	HepGeom::Transform3D moveCutSide = HepGeom::Translate3D(0,HBPWINDOWCUTYSIGN*20*CLHEP::mm,0);
	const GeoShapeShift& windowCutSide = (*windowCutSide0)<<moveCutSide;
	HepGeom::Transform3D moveCutCorner = HepGeom::Translate3D(11*CLHEP::mm-0.5*(thickness2-0.3*CLHEP::mm)+0.5*sqrt(2)*20*CLHEP::mm,HBPWINDOWCUTYSIGN*(2*CLHEP::mm+0.5*sqrt(2)*20*CLHEP::mm),0);
	HepGeom::Transform3D rotateCutCorner = HepGeom::RotateZ3D(HBPWINDOWCUTYSIGN*-45*CLHEP::deg);
	const GeoShapeShift& windowCutCorner = (*windowCutCorner0)<<rotateCutCorner<<moveCutCorner;
	const GeoShapeSubtraction& window11 = window10->subtract(windowCutCorner);
	const GeoShapeSubtraction* window12 = new GeoShapeSubtraction(&window11, &windowCutSide);
	const GeoShapeSubtraction& window21 = window20->subtract(windowCutCorner);
	const GeoShapeSubtraction* window22 = new GeoShapeSubtraction(&window21, &windowCutSide);
        HepGeom::Transform3D moveWindow1 = HepGeom::Translate3D(-11*CLHEP::mm+0.5*(thickness2+0.3*CLHEP::mm), HBPWINDOWCUTYSIGN*6*CLHEP::mm, 50*CLHEP::mm+0.5*thickness+HBPTHINWINDOWTHICKNESS);
        HepGeom::Transform3D moveWindow2 = HepGeom::Translate3D(-11*CLHEP::mm+0.5*(thickness2+0.3*CLHEP::mm), HBPWINDOWCUTYSIGN*6*CLHEP::mm, -50*CLHEP::mm-0.5*thickness-HBPTHINWINDOWTHICKNESS);
	const GeoShapeShift& window1 = (*window12)<<moveWindow1;
	const GeoShapeShift& window2 = (*window22)<<moveWindow2;


	// Boxes with round corners on ends
	const GeoBox* endsSFull0 = new GeoBox(25.85*CLHEP::mm, 50*CLHEP::mm+thickness, 35*CLHEP::mm);
	const GeoBox* endsSIn0 = new GeoBox(25.85*CLHEP::mm-0.5*thickness, 50*CLHEP::mm, 35*CLHEP::mm-thickness);
	HepGeom::Transform3D moveEndSIn = HepGeom::Translate3D(thickness,0,0);
	const GeoShapeShift& endsSIn = (*endsSIn0)<<moveEndSIn;
	GeoShapeSubtraction* endsS0 = new GeoShapeSubtraction(endsSFull0,&endsSIn);
	const GeoTubs* cornerCutterS10 = new GeoTubs(9*CLHEP::mm+thickness, 20*CLHEP::mm, 35.1*CLHEP::mm, 90*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerCutterS20 = new GeoTubs(9*CLHEP::mm+thickness, 20*CLHEP::mm, 35.1*CLHEP::mm, 180*CLHEP::deg, 90*CLHEP::deg);
	HepGeom::Transform3D moveCutterS1 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness)*CLHEP::mm, 41*CLHEP::mm, 0);
	HepGeom::Transform3D moveCutterS2 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness)*CLHEP::mm, -(41*CLHEP::mm), 0);
	HepGeom::Transform3D moveAddS1 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness), 41*CLHEP::mm, 0);
	HepGeom::Transform3D moveAddS2 = HepGeom::Translate3D(-(16.85*CLHEP::mm-thickness), -(41*CLHEP::mm), 0);
	const GeoShapeShift& cornerCutterS1 = (*cornerCutterS10)<<moveCutterS1;
	const GeoShapeShift& cornerCutterS2 = (*cornerCutterS20)<<moveCutterS2;
	GeoShapeSubtraction* endsS01 = new GeoShapeSubtraction(new GeoShapeSubtraction(endsS0, &cornerCutterS2), &cornerCutterS1);
	const GeoTubs* cornerAddS10 = new GeoTubs(9*CLHEP::mm, 9*CLHEP::mm+thickness, 35*CLHEP::mm, 90*CLHEP::deg, 90*CLHEP::deg);
	const GeoTubs* cornerAddS20 = new GeoTubs(9*CLHEP::mm, 9*CLHEP::mm+thickness, 35*CLHEP::mm, 180*CLHEP::deg, 90*CLHEP::deg);
	const GeoShapeShift& cornerAddS1 = (*cornerAddS10)<<moveAddS1;
	const GeoShapeShift& cornerAddS2 = (*cornerAddS20)<<moveAddS2;
	GeoShapeSubtraction* endsS = new GeoShapeSubtraction(new GeoShapeSubtraction(endsS01, &cornerAddS2), &cornerAddS1);
	HepGeom::Transform3D moveEndS1 = HepGeom::Translate3D(-25.9*CLHEP::mm+thickness2, 0, 85.1*CLHEP::mm);
	HepGeom::Transform3D rotateEndS2 = HepGeom::RotateX3D(180*CLHEP::deg);
	const GeoShapeShift& endS1 = (*endsS)<<moveEndS1;
	const GeoShapeShift& endS2 = (*endsS)<<moveEndS1<<rotateEndS2;

	// Holes for the beam pipe
	const GeoTube* holeS10 = new GeoTube(0, 40*CLHEP::mm-0.05*CLHEP::mm, 0.55*thickness);
	const GeoTube* holeS20 = new GeoTube(0, 40*CLHEP::mm-0.05*CLHEP::mm, 0.55*thickness);
	HepGeom::Transform3D moveholeS1 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, 120*CLHEP::mm-0.5*thickness);
	HepGeom::Transform3D moveholeS2 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, -(120*CLHEP::mm-0.5*thickness));
	const GeoShapeShift& holeS1 = (*holeS10)<<moveholeS1;
	const GeoShapeShift& holeS2 = (*holeS20)<<moveholeS2;


	// Beam pipe in and out
	const GeoTube* tubeS10 = new GeoTube(40*CLHEP::mm, 40*CLHEP::mm+thickness, 75*CLHEP::mm);
	const GeoTube* tubeS20 = new GeoTube(40*CLHEP::mm, 40*CLHEP::mm+thickness, 75*CLHEP::mm);
	HepGeom::Transform3D movetubeS1 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, 195.05*CLHEP::mm);
	HepGeom::Transform3D movetubeS2 = HepGeom::Translate3D(15*CLHEP::mm+2*thickness2, 0, -195.05*CLHEP::mm);
	const GeoShapeShift& tubeS1 = (*tubeS10)<<movetubeS1;
	const GeoShapeShift& tubeS2 = (*tubeS20)<<movetubeS2;

	// Putting all together
	GeoShapeUnion* hamburgPipeS01 = new GeoShapeUnion(&halftubeS, &planeS);
	GeoShapeUnion* hamburgPipeS02 = new GeoShapeUnion(hamburgPipeS01, &endS1);
	GeoShapeUnion* hamburgPipeS03 = new GeoShapeUnion(hamburgPipeS02, &endS2);
	GeoShapeSubtraction* hamburgPipeS04 = new GeoShapeSubtraction(hamburgPipeS03, &holeS1);
	GeoShapeSubtraction* hamburgPipeS05 = new GeoShapeSubtraction(hamburgPipeS04, &holeS2);
	GeoShapeUnion* hamburgPipeS06 = new GeoShapeUnion(hamburgPipeS05, &tubeS1);
	GeoShapeUnion* hamburgPipeS07 = new GeoShapeUnion(hamburgPipeS06, &tubeS2);
        GeoShapeSubtraction* hamburgPipeS08 = new GeoShapeSubtraction(hamburgPipeS07, &window1);
        GeoShapeSubtraction* hamburgPipeS09 = new GeoShapeSubtraction(hamburgPipeS08, &window2);
        GeoShapeSubtraction* hamburgPipeS10 = new GeoShapeSubtraction(hamburgPipeS09, &floor);

	return hamburgPipeS10;
}

GeoShape* AFP_GeoModelFactory::GetSolidShortWindowPlates()
{
    double plateXHalfLength = 0.5*52/sin(m_CfgParams.hbpcfg.windowPlateAngle*CLHEP::deg)*CLHEP::mm;
    const GeoBox* windowPlate = new GeoBox(plateXHalfLength, 53*CLHEP::mm, 0.5*m_CfgParams.hbpcfg.windowPlateThickness*CLHEP::mm);
    HepGeom::Transform3D moveXPlate1 = HepGeom::Translate3D(-plateXHalfLength, 0, 0);
    HepGeom::Transform3D moveZPlate1 = HepGeom::Translate3D(0, 0, 75*CLHEP::mm);
    HepGeom::Transform3D rotatePlate1 = HepGeom::RotateY3D((90-m_CfgParams.hbpcfg.windowPlateAngle)*CLHEP::deg);
    HepGeom::Transform3D moveXPlate2 = HepGeom::Translate3D(-plateXHalfLength, 0, 0);
    HepGeom::Transform3D moveZPlate2 = HepGeom::Translate3D(0, 0, -75*CLHEP::mm);
    HepGeom::Transform3D rotatePlate2 = HepGeom::RotateY3D(-(90-m_CfgParams.hbpcfg.windowPlateAngle)*CLHEP::deg);
    const GeoShapeShift& windowPlate1 = (*windowPlate)<<moveXPlate1<<rotatePlate1<<moveZPlate1;
    const GeoShapeShift& windowPlate2 = (*windowPlate)<<moveXPlate2<<rotatePlate2<<moveZPlate2;

    return new GeoShapeUnion(&windowPlate1, &windowPlate2);
}

GeoShape* AFP_GeoModelFactory::GetSolidLongWindowPlates()
{
    double plateXHalfLength = 0.5*52/sin(m_CfgParams.hbpcfg.windowPlateAngle*CLHEP::deg)*CLHEP::mm;
    const GeoBox* windowPlate = new GeoBox(plateXHalfLength, 53*CLHEP::mm, 0.5*m_CfgParams.hbpcfg.windowPlateThickness*CLHEP::mm);
    HepGeom::Transform3D moveXPlate1 = HepGeom::Translate3D(-plateXHalfLength, 0, 0);
    HepGeom::Transform3D moveZPlate1 = HepGeom::Translate3D(0, 0, 350*CLHEP::mm);
    HepGeom::Transform3D rotatePlate1 = HepGeom::RotateY3D((90-m_CfgParams.hbpcfg.windowPlateAngle)*CLHEP::deg);
    HepGeom::Transform3D moveXPlate2 = HepGeom::Translate3D(-plateXHalfLength, 0, 0);
    HepGeom::Transform3D moveZPlate2 = HepGeom::Translate3D(0, 0, -350*CLHEP::mm);
    HepGeom::Transform3D rotatePlate2 = HepGeom::RotateY3D(-(90-m_CfgParams.hbpcfg.windowPlateAngle)*CLHEP::deg);
    const GeoShapeShift& windowPlate1 = (*windowPlate)<<moveXPlate1<<rotatePlate1<<moveZPlate1;
    const GeoShapeShift& windowPlate2 = (*windowPlate)<<moveXPlate2<<rotatePlate2<<moveZPlate2;

    return new GeoShapeUnion(&windowPlate1, &windowPlate2);
}

void AFP_GeoModelFactory::AddShortWindowPlates(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
        char szLabel[32];

        sprintf(szLabel,"%s_LogShortWindowPlates",pszStationName);
        GeoShape* pShortWindowPlatesSol=GetSolidShortWindowPlates();
        const GeoLogVol* windowPlatesSLog = new GeoLogVol(szLabel, pShortWindowPlatesSol, m_MapMaterials[std::string(m_CfgParams.hbpcfg.setMaterialToBeryllium ? "Beryllium" : "Steel")]);
        GeoPhysVol* windowPlatesSPhys   = new GeoPhysVol(windowPlatesSLog);

        sprintf(szLabel,"%s_ShortWindowPlates",pszStationName);
        pPhysMotherVol->add(new GeoNameTag(szLabel));
        pPhysMotherVol->add(new GeoTransform(TransInMotherVolume*HepGeom::RotateZ3D((DETXSIDE==+1)? -180*CLHEP::deg:0.0*CLHEP::deg)));
        pPhysMotherVol->add(windowPlatesSPhys);
}

void AFP_GeoModelFactory::AddLongWindowPlates(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
        char szLabel[32];

        sprintf(szLabel,"%s_LogLongWindowPlates",pszStationName);
        GeoShape* pLongWindowPlatesSol=GetSolidLongWindowPlates();
        const GeoLogVol* windowPlatesSLog = new GeoLogVol(szLabel, pLongWindowPlatesSol, m_MapMaterials[std::string(m_CfgParams.hbpcfg.setMaterialToBeryllium ? "Beryllium" : "Steel")]);
        GeoPhysVol* windowPlatesPhys   = new GeoPhysVol(windowPlatesSLog);

        sprintf(szLabel,"%s_LongWindowPlates",pszStationName);
        pPhysMotherVol->add(new GeoNameTag(szLabel));
        pPhysMotherVol->add(new GeoTransform(TransInMotherVolume*HepGeom::RotateZ3D((DETXSIDE==+1)? -180*CLHEP::deg:0.0*CLHEP::deg)));
        pPhysMotherVol->add(windowPlatesPhys);
}

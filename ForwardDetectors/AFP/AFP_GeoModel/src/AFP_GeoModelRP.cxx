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
#include "GeoModelKernel/GeoPgon.h"
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

#define RPOT_MAINTUBUS_WNDCUTUP 1
#define RPOT_MAINTUBUS_LENGTH (70.0*CLHEP::mm)
#define RPOT_MAINTUBUS_INNERRADIUS (71.0*CLHEP::mm)
#define RPOT_MAINTUBUS_THICKNESS (2.0*CLHEP::mm)
#define RPOT_MAINTUBUS_BPTHICKNESS (0.3*CLHEP::mm) //thickness of the bottom part
#define RPOT_MAINTUBUS_WNDWIDTH (20.0*CLHEP::mm)//24
#define RPOT_MAINTUBUS_WNDHEIGHT (20.0*CLHEP::mm)//30
#define RPOT_MAINTUBUS_WNDTHICKNESS (0.3*CLHEP::mm)
#define RPOT_MAINTUBUS_FLOORPARTLENGTH (20.0*CLHEP::mm)
#define RPOT_MAINTUBUS_FLOORPARTTHICKNESS (2.0*CLHEP::mm)

#define RPOT_FLOOR_THICKNESS (2.0*CLHEP::mm)
#define RPOT_FLOOR_WNDWIDTH (20.0*CLHEP::mm)
#define RPOT_FLOOR_WNDTHICKNESS (0.3*CLHEP::mm)
#define RPOT_FLOOR_WNDFACET (45*CLHEP::deg)

#define RPOT_FLANGE_THICKNESS (20.0*CLHEP::mm)
#define RPOT_FLANGE_OUTERRADIUS (125.0*CLHEP::mm)

void AFP_GeoModelFactory::AddRomanPot(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
	const double fMainTubusSteelPartLength=RPOT_MAINTUBUS_LENGTH-RPOT_MAINTUBUS_FLOORPARTLENGTH;

	char szLabel[32];
	double fLength,fRMin,fRMax;
	GeoShapeShift* pMoveCut;
	HepGeom::Transform3D TransCut;
	GeoShape* pSolCut;

	//HepGeom::Transform3D TransRPot=TransInMotherVolume*HepGeom::TranslateX3D(-(0.5*RPOT_MAINTUBUS_LENGTH+RPOT_FLOOR_WNDTHICKNESS));
	HepGeom::Transform3D TransRPot=TransInMotherVolume*HepGeom::TranslateX3D(-(0.5*fMainTubusSteelPartLength+RPOT_MAINTUBUS_FLOORPARTLENGTH+RPOT_FLOOR_WNDTHICKNESS));

	//Main tubus -------------------------------------------------------------------------------------------------
	fLength=fMainTubusSteelPartLength;
	fRMin=RPOT_MAINTUBUS_INNERRADIUS;
	fRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS;

	GeoTube* pSolTubus=new GeoTube(fRMin, fRMax, 0.5*fLength);
	sprintf(szLabel,"%s_LogRPMainTubus",pszStationName);
	GeoLogVol* pLogTubus=new GeoLogVol(szLabel,pSolTubus,m_MapMaterials[std::string("Steel")]);
	sprintf(szLabel,"%s_RPMainTubus",pszStationName);
	GeoFullPhysVol* pPhysTubus=new GeoFullPhysVol(pLogTubus);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);

	//Main tubus - floor (bottom) part ---------------------------------------------------------------------------
	fLength=RPOT_MAINTUBUS_FLOORPARTLENGTH;
	fRMin=RPOT_MAINTUBUS_INNERRADIUS;
	fRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_BPTHICKNESS;

	sprintf(szLabel,"%s_LogRPMainTubusFloorPart",pszStationName);
	pSolTubus=new GeoTube(fRMin, fRMax, 0.5*fLength);
	sprintf(szLabel,"%s_LogRPMainTubusFloorPart",pszStationName);
	pLogTubus=new GeoLogVol(szLabel,pSolTubus,m_MapMaterials[std::string("Steel")]); //XXX
	pPhysTubus=new GeoFullPhysVol(pLogTubus);
	sprintf(szLabel,"%s_RPMainTubusFloorPart",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::TranslateX3D(0.5*fLength+0.5*fMainTubusSteelPartLength)*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);

	//--upper mass
	fLength=RPOT_MAINTUBUS_FLOORPARTLENGTH;
	fRMin=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_BPTHICKNESS;
	fRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS;
	double fPhi=asin(0.5*RPOT_FLOOR_WNDWIDTH/fRMin);
	double fDPhi=CLHEP::pi-2*fPhi;
	GeoTubs* pSolMass=new GeoTubs(fRMin,fRMax,0.5*fLength,fPhi,fDPhi);

	sprintf(szLabel,"%s_LogRPMainTubusUMass",pszStationName);
	pLogTubus=new GeoLogVol(szLabel,pSolMass,m_MapMaterials[std::string("Steel")]); //XXX
	pPhysTubus=new GeoFullPhysVol(pLogTubus);
	sprintf(szLabel,"%s_LogRPMainTubusUMass",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::TranslateX3D(0.5*fLength+0.5*fMainTubusSteelPartLength)*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);

	//-lower mass
	fLength=RPOT_MAINTUBUS_FLOORPARTLENGTH;
	fRMin=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_BPTHICKNESS;
	fRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS;
	double fSPhi=CLHEP::pi+fPhi;
	pSolMass=new GeoTubs(fRMin,fRMax,0.5*fLength,fSPhi,fDPhi);

	sprintf(szLabel,"%s_LogRPMainTubusLMass",pszStationName);
	pLogTubus=new GeoLogVol(szLabel,pSolMass,m_MapMaterials[std::string("Steel")]); //XXX
	pPhysTubus=new GeoFullPhysVol(pLogTubus);
	sprintf(szLabel,"%s_LogRPMainTubusLMass",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::TranslateX3D(0.5*fLength+0.5*fMainTubusSteelPartLength)*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);

//	//cut volume 1
//	double fPhi1=asin(0.5*RPOT_FLOOR_WNDWIDTH/(RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS));
//	double fPhi2=asin((RPOT_MAINTUBUS_WNDHEIGHT-0.5*RPOT_FLOOR_WNDWIDTH)/(RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS));
//	double fTubsRMin=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_WNDTHICKNESS;
//	double fTubsRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS+1.0;
//	double fTubsHZLength=0.5*RPOT_MAINTUBUS_WNDWIDTH;

//#ifdef RPOT_MAINTUBUS_WNDCUTUP
//	double fTubsSPhi=-fPhi1;
//	double fTubsDPhi=fPhi1+fPhi2;
//	double falpha=+atan((RPOT_MAINTUBUS_WNDHEIGHT-RPOT_FLOOR_WNDWIDTH)/RPOT_MAINTUBUS_WNDWIDTH);
//	double fd=RPOT_MAINTUBUS_WNDWIDTH/sin(fabs(falpha));
//	HepGeom::Transform3D AuxTransCutWnd=HepGeom::Translate3D(0.0,-(0.5*RPOT_FLOOR_WNDWIDTH-RPOT_MAINTUBUS_WNDHEIGHT),-0.5*RPOT_MAINTUBUS_WNDWIDTH);
//	HepGeom::Vector3D<double> vecX=0.5*fd*(HepGeom::RotateX3D(falpha)*HepGeom::Vector3D<double>(0,+1.0,+1.0));
//#else
//	double fTubsSPhi=-fPhi2;
//	double fTubsDPhi=fPhi1+fPhi2;
//	double falpha=-atan((RPOT_MAINTUBUS_WNDHEIGHT-RPOT_FLOOR_WNDWIDTH)/RPOT_MAINTUBUS_WNDWIDTH);
//	double fd=RPOT_MAINTUBUS_WNDWIDTH/sin(fabs(falpha));
//	HepGeom::Transform3D AuxTransCutWnd=HepGeom::Translate3D(0.0,0.5*RPOT_FLOOR_WNDWIDTH-RPOT_MAINTUBUS_WNDHEIGHT,-0.5*RPOT_MAINTUBUS_WNDWIDTH);
//	HepGeom::Vector3D<double> vecX=0.5*fd*(HepGeom::RotateX3D(falpha)*HepGeom::Vector3D<double>(0,-1.0,+1.0));
//#endif

//	GeoShape* pSolCut=new GeoTubs(fTubsRMin,fTubsRMax,fTubsHZLength,fTubsSPhi,fTubsDPhi);
//	GeoShape* pSolCut2=new GeoBox(RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS+2.0,0.5*fd,0.5*fd);
//	HepGeom::Transform3D TransCutWnd=AuxTransCutWnd*HepGeom::Translate3D(vecX)*HepGeom::RotateX3D(falpha);
//	GeoShapeShift* pMoveCutWnd=new GeoShapeShift(pSolCut2, TransCutWnd);
//	GeoShapeSubtraction* pSolCut3=new GeoShapeSubtraction(pSolCut, pMoveCutWnd);

//	HepGeom::Transform3D TransCutTubus=HepGeom::TranslateZ3D(0.5*fLength-fTubsHZLength);
//	pMoveCut=new GeoShapeShift(pSolCut3, TransCutTubus);
//	GeoShapeSubtraction* pSolModTubus=new GeoShapeSubtraction(pSolTubus, pMoveCut);

//	//cut volume 2
//#ifdef RPOT_MAINTUBUS_WNDCUTUP
//	//fTubsSPhi=180.0*CLHEP::deg-0.5*fPhi2-fPhi1;
//	fTubsSPhi=180.0*CLHEP::deg-fPhi1;
//#else
//	fTubsSPhi=180.0*CLHEP::deg+fPhi1;
//#endif

//	pSolCut=new GeoTubs(fTubsRMin,fTubsRMax,fTubsHZLength,fTubsSPhi,fTubsDPhi);
//	pSolCut3=new GeoShapeSubtraction(pSolCut, pMoveCutWnd);
//	pMoveCut=new GeoShapeShift(pSolCut3, TransCutTubus);
//	GeoShapeSubtraction* pSolModTubus2=new GeoShapeSubtraction(pSolModTubus, pMoveCut);

	//-------------- old stuff
	//        GeoLogVol* pLogTubus=NULL; GeoFullPhysVol* pPhysTubus=NULL;
	//        if(m_CfgParams.bVP1Mode) pLogTubus=new GeoLogVol(szLabel,pSolCut3,m_MapMaterials[std::string("Steel")]);
	//        else  pLogTubus=new GeoLogVol(szLabel,pSolCut3,m_MapMaterials[std::string("Steel")]);
	//        pPhysTubus=new GeoFullPhysVol(pLogTubus);
	//        pPhysMotherVol->add(new GeoNameTag(szLabel));
	//        pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::RotateY3D(90*CLHEP::deg)*TransCutTubus));
	//        pPhysMotherVol->add(pPhysTubus);
	//--------------

	//Floor tubus -------------------------------------------------------------------------------------------------
	fLength=RPOT_FLOOR_THICKNESS;
	fRMin=0.0*CLHEP::mm;
	fRMax=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS-RPOT_MAINTUBUS_WNDTHICKNESS;
	pSolTubus=new GeoTube(fRMin, fRMax, 0.5*fLength);

	//cut volume
	double fTrdHXLength1=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS;
	double fTrdHXLength2=RPOT_MAINTUBUS_INNERRADIUS+RPOT_MAINTUBUS_THICKNESS;
	double fTrdHYLength2=0.5*RPOT_FLOOR_WNDWIDTH;
	double fTrdHYLength1=0.5*RPOT_FLOOR_WNDWIDTH-RPOT_FLOOR_WNDTHICKNESS*tan(RPOT_FLOOR_WNDFACET);
	double fTrdHZLength=0.5*(RPOT_FLOOR_THICKNESS-RPOT_FLOOR_WNDTHICKNESS);
	pSolCut=new GeoTrd(fTrdHXLength1,fTrdHXLength2,fTrdHYLength1,fTrdHYLength2,fTrdHZLength);
	TransCut=HepGeom::TranslateZ3D(0.5*fLength-fTrdHZLength);
	pMoveCut=new GeoShapeShift(pSolCut, TransCut);
	GeoShapeSubtraction* pSolFloor=new GeoShapeSubtraction(pSolTubus, pMoveCut);

	sprintf(szLabel,"%s_LogRPFloorTubus",pszStationName);
	pLogTubus=new GeoLogVol(szLabel,pSolFloor,m_MapMaterials[std::string("Beryllium")]);
	pPhysTubus=new GeoFullPhysVol(pLogTubus);
	sprintf(szLabel,"%s_RPFloorTubus",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::TranslateX3D(0.5*fMainTubusSteelPartLength+RPOT_MAINTUBUS_FLOORPARTLENGTH+0.5*RPOT_FLOOR_THICKNESS)*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);

	//Flange tubus -------------------------------------------------------------------------------------------------
	fLength=RPOT_FLANGE_THICKNESS;
	fRMin=RPOT_MAINTUBUS_INNERRADIUS;
	fRMax=RPOT_FLANGE_OUTERRADIUS;

	pSolTubus=new GeoTube(fRMin, fRMax, 0.5*fLength);
	sprintf(szLabel,"%s_LogRPFlangeTubus",pszStationName);
	pLogTubus=new GeoLogVol(szLabel,pSolTubus,m_MapMaterials[std::string("Steel")]);
	pPhysTubus=new GeoFullPhysVol(pLogTubus);
	sprintf(szLabel,"%s_RPFlangeTubus",pszStationName);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(TransRPot*HepGeom::TranslateX3D(-0.5*fMainTubusSteelPartLength-0.5*RPOT_FLANGE_THICKNESS)*HepGeom::RotateY3D(90*CLHEP::deg)));
	pPhysMotherVol->add(pPhysTubus);


}

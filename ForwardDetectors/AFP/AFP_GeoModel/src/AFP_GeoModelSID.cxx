/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModelFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
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
#include "GeoModelKernel/GeoDefinitions.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <list>
#include <map>
#include <stdlib.h>

void AFP_GeoModelFactory::addSiDetector(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume)
{
	char szLabel[32];
	GeoLogVol* pLogElement=NULL;
	GeoFullPhysVol* pPhysElement;
	AFP_CONSTANTS AfpConstants;
	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg[eStation];
    
	//create (constant) solids
	GeoShape* pSolidSIDPlate=createSolidSIDPlate();
	GeoShape* pSolidSIDSensor=new GeoBox(0.5*AfpConstants.SiT_Pixel_length_totx,0.5*AfpConstants.SiT_Pixel_length_toty,0.5*AfpConstants.SiT_Pixel_thickness);
	GeoShape* pSolidSIDVacuumSensor=new GeoBox(0.5*180.0*CLHEP::mm,0.5*160.0*CLHEP::mm,0.5*AfpConstants.Stat_GlobalVacuumSensorThickness);

	//add global vacuum sensitive volume (ID=11)
	int nSpecVacSensorID=AfpConstants.Stat_GlobalVacuumSensorID;
	sprintf(szLabel,"%s_LogSIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
	GeoLogVol* pLogSIDVacuumSensor=new GeoLogVol(szLabel,pSolidSIDVacuumSensor,m_MapMaterials[std::string("std::Vacuum")]);
	GeoFullPhysVol* pPhysSIDVacuumSensor=new GeoFullPhysVol(pLogSIDVacuumSensor);
	sprintf(szLabel,"%s_SIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(Amg::CLHEPTransformToEigen(m_pGeometry->getSIDTransform(ESTT_VACUUMSENSOR,pszStationName,nSpecVacSensorID))));
	pPhysMotherVol->add(pPhysSIDVacuumSensor);
    
    
	for(int i=0;i<m_pGeometry->getSIDPlatesCnt(eStation);i++)
	{
		// create SID plate
		sprintf(szLabel,"%s_LogSIDPlate[%i]",pszStationName,i);
		pLogElement=new GeoLogVol(szLabel,pSolidSIDPlate,m_MapMaterials[std::string("CE7")]);
		pPhysElement=new GeoFullPhysVol(pLogElement);
		sprintf(szLabel,"%s_SIDPlate[%i]",pszStationName,i);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		pPhysMotherVol->add(new GeoTransform(Amg::CLHEPTransformToEigen(TransInMotherVolume*m_pGeometry->getSIDTransform(ESTT_PLATE,pszStationName,i))));
		pPhysMotherVol->add(pPhysElement);

		// create SID chip
		GeoShape* pSolidFEI4Chip=new GeoBox(0.5*sidcfg.vecChipXLength[i],0.5*sidcfg.vecChipYLength[i],0.5*AfpConstants.SiT_Chip_thickness);
		sprintf(szLabel,"%s_LogSIDChip[%i]",pszStationName,i);
		pLogElement=new GeoLogVol(szLabel,pSolidFEI4Chip,m_MapMaterials[std::string("CE7")]);
		pPhysElement=new GeoFullPhysVol(pLogElement);
		sprintf(szLabel,"%s_SIDChip[%i]",pszStationName,i);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		pPhysMotherVol->add(new GeoTransform(Amg::CLHEPTransformToEigen(TransInMotherVolume*m_pGeometry->getSIDTransform(ESTT_FEI4CHIP,pszStationName,i))));
		pPhysMotherVol->add(pPhysElement);

		// create SID sensor (pixel area)
		sprintf(szLabel,"%s_LogSIDSensor[%i]",pszStationName,i);
		pLogElement=new GeoLogVol(szLabel,pSolidSIDSensor,m_MapMaterials[std::string("Silicon")]);
		pPhysElement=new GeoFullPhysVol(pLogElement);
		sprintf(szLabel,"%s_SIDSensor[%i]",pszStationName,i);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		pPhysMotherVol->add(new GeoTransform(Amg::CLHEPTransformToEigen(TransInMotherVolume*m_pGeometry->getSIDTransform(ESTT_SENSOR,pszStationName,i))));
		pPhysMotherVol->add(pPhysElement);
		
		if(m_CfgParams.sidcfg[eStation].bAddVacuumSensors)
		{
			// create logic SID Vacuum Layer
			sprintf(szLabel,"%s_LogSIDVacuumSensor[%i]",pszStationName,i);
			GeoLogVol* pLogSIDVacuumSensor=new GeoLogVol(szLabel,pSolidSIDVacuumSensor,m_MapMaterials[std::string("std::Vacuum")]);
			GeoFullPhysVol* pPhysSIDVacuumSensor=new GeoFullPhysVol(pLogSIDVacuumSensor);

			sprintf(szLabel,"%s_SIDVacuumSensor[%i]",pszStationName,i);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			pPhysMotherVol->add(new GeoTransform(Amg::CLHEPTransformToEigen(m_pGeometry->getSIDTransform(ESTT_VACUUMSENSOR,pszStationName,i))));
			pPhysMotherVol->add(pPhysSIDVacuumSensor);
		}
	}
}


GeoShape* AFP_GeoModelFactory::createSolidSIDPlate()
{
	MsgStream LogStream(Athena::getMessageSvc(), "AFP_GeoModelFactory::CreateSolidSIDPlate");

	double fdelta=0.01*CLHEP::mm;//0.01*CLHEP::mm;
	AFP_CONSTANTS AfpConstants;

	GeoBox* pMainPlate=new GeoBox(0.5*AfpConstants.SiT_Plate_Main_length_x,0.5*AfpConstants.SiT_Plate_Main_length_y,0.5*AfpConstants.SiT_Plate_Main_thickness);

	//cut side edges
	double fd=std::sqrt(AfpConstants.SiT_Plate_CutEdge_length_x*AfpConstants.SiT_Plate_CutEdge_length_x+AfpConstants.SiT_Plate_CutEdge_length_y*AfpConstants.SiT_Plate_CutEdge_length_y);
	double falpha=std::atan(AfpConstants.SiT_Plate_CutEdge_length_y/AfpConstants.SiT_Plate_CutEdge_length_x);
	double fgamma=45.0*CLHEP::deg-falpha; double fbeta=90.0*CLHEP::deg-falpha;

	//--lower cut
	HepGeom::Vector3D<double> vecL=(fd/std::sqrt(2.0))*(HepGeom::RotateZ3D(-fgamma)*HepGeom::Vector3D<double>(1.0,0.0,0.0));
	HepGeom::Vector3D<double> vecB=AfpConstants.SiT_Plate_CutEdge_length_x*HepGeom::Vector3D<double>(1.0,0.0,0.0);
	HepGeom::Vector3D<double> vecX=vecL-vecB;
	GeoBox* pPlateCut=new GeoBox(0.5*fd, 0.5*fd,0.5*(AfpConstants.SiT_Plate_CutEdge_thickness+fdelta));
	HepGeom::Transform3D TransCut=HepGeom::Translate3D(vecX.x()+0.5*AfpConstants.SiT_Plate_Main_length_x,vecX.y()-0.5*AfpConstants.SiT_Plate_Main_length_y,0.0)*HepGeom::RotateZ3D(-fbeta);
	GeoShapeShift* pShiftCut=new GeoShapeShift(pPlateCut, Amg::CLHEPTransformToEigen(TransCut));
	GeoShapeSubtraction* pShape1=new GeoShapeSubtraction(pMainPlate, pShiftCut);

	//--upper cut
	vecL=(fd/std::sqrt(2.0))*(HepGeom::RotateZ3D(+fgamma)*HepGeom::Vector3D<double>(1.0,0.0,0.0));
	vecX=vecL-vecB;
	pPlateCut=new GeoBox(0.5*fd, 0.5*fd,0.5*(AfpConstants.SiT_Plate_CutEdge_thickness+fdelta));
	TransCut=HepGeom::Translate3D(vecX.x()+0.5*AfpConstants.SiT_Plate_Main_length_x,vecX.y()+0.5*AfpConstants.SiT_Plate_Main_length_y,0.0)*HepGeom::RotateZ3D(+fbeta);
	pShiftCut=new GeoShapeShift(pPlateCut,Amg::CLHEPTransformToEigen(TransCut));
	GeoShapeSubtraction* pShape2=new GeoShapeSubtraction(pShape1, pShiftCut);

	//cut sensitive area
	pPlateCut=new GeoBox(0.5*(AfpConstants.SiT_Plate_Window_length_x+fdelta),0.5*AfpConstants.SiT_Plate_Window_length_y,0.5*AfpConstants.SiT_Plate_Window_thickness);
	TransCut=HepGeom::Translate3D(AfpConstants.SiT_Plate_Window_x+fdelta,AfpConstants.SiT_Plate_Window_y,-(0.5*AfpConstants.SiT_Plate_Main_thickness-0.5*AfpConstants.SiT_Plate_Window_thickness));
	pShiftCut=new GeoShapeShift(pPlateCut,Amg::CLHEPTransformToEigen(TransCut));
	GeoShapeSubtraction* pShape3=new GeoShapeSubtraction(pShape2, pShiftCut);

	return pShape3;
}

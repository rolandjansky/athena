/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_GeoModel/AFP_GeoModelFactory.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
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
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelUtilities/GeoModelTool.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoOpticalPhysVol.h"
#include "GeoModelUtilities/GeoOpticalSurface.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

#include <iostream>
#include <fstream>
#include <string>

#include <math.h>
#include <algorithm>

#include <list>
#include <map>

#include <stdlib.h>


void AFP_GeoModelFactory::InitializeTDParameters()
{
#ifdef USE_TDLBARS
	int i,j;
	double fLastXDim, fLastYDim;
	double fBeta=90.0*CLHEP::deg-TD_NOMINALSLOPE;

	//z-dir
	m_QuarticDimensions[0].arrLBarZDim.resize(TD_COLUMNSCNT);
    m_QuarticDimensions[0].arrLBarZDim[0]=6*CLHEP::mm; m_QuarticDimensions[0].arrLBarZGap[0]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrLBarZDim[1]=6*CLHEP::mm; m_QuarticDimensions[0].arrLBarZGap[1]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrLBarZDim[2]=6*CLHEP::mm; m_QuarticDimensions[0].arrLBarZGap[2]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrLBarZDim[3]=6*CLHEP::mm; m_QuarticDimensions[0].arrLBarZGap[3]=0*CLHEP::mm;

	//vert x-dir
	m_QuarticDimensions[0].arrVertBarXDim.resize(TD_TRAINSCNT);
    m_QuarticDimensions[0].arrVertBarXDim[0]=6*CLHEP::mm; m_QuarticDimensions[0].arrVertBarXGap[0]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrVertBarXDim[1]=4*CLHEP::mm; m_QuarticDimensions[0].arrVertBarXGap[1]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrVertBarXDim[2]=2*CLHEP::mm; m_QuarticDimensions[0].arrVertBarXGap[2]=0*CLHEP::mm;

	//horz y-dir
	m_QuarticDimensions[0].arrHorzBarYDim.resize(TD_TRAINSCNT);
    m_QuarticDimensions[0].arrHorzBarYDim[0]=6*CLHEP::mm; m_QuarticDimensions[0].arrHorzBarYGap[0]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrHorzBarYDim[1]=4*CLHEP::mm; m_QuarticDimensions[0].arrHorzBarYGap[1]=1*CLHEP::mm;
    m_QuarticDimensions[0].arrHorzBarYDim[2]=2*CLHEP::mm; m_QuarticDimensions[0].arrHorzBarYGap[2]=0*CLHEP::mm;

	//horz x-dim
	m_QuarticDimensions[0].arrHorzBarXDim.resize(TD_TRAINSCNT);
	fLastXDim=TD_FIRSTBARHORZLENGTH;
	for(i=0;i<TD_TRAINSCNT;i++){
		if(i==0) m_QuarticDimensions[0].arrHorzBarXDim[i]=fLastXDim;
		else m_QuarticDimensions[0].arrHorzBarXDim[i]=fLastXDim-(m_QuarticDimensions[0].arrVertBarXGap[i-1]+m_QuarticDimensions[0].arrVertBarXDim[i]);
	}

	//vert y-dir
	m_QuarticDimensions[0].arrVertBarYDim.resize(TD_COLUMNSCNT*TD_TRAINSCNT);
	for(i=0;i<TD_TRAINSCNT;i++){
		if(i==0)fLastYDim=TD_FIRSTBARVERTLENGTH;
		else fLastYDim=m_QuarticDimensions[0].arrVertBarYDim[(i-1)*TD_COLUMNSCNT+0];

		for(j=0;j<TD_COLUMNSCNT;j++){
			if(i==0) m_QuarticDimensions[0].arrVertBarYDim[i*TD_COLUMNSCNT+j]=fLastYDim;
			else m_QuarticDimensions[0].arrVertBarYDim[i*TD_COLUMNSCNT+j]=fLastYDim-(m_QuarticDimensions[0].arrLBarZDim[j-1]+m_QuarticDimensions[0].arrLBarZGap[j-1])*tan(fBeta)-(m_QuarticDimensions[0].arrHorzBarYDim[i-1]+m_QuarticDimensions[0].arrHorzBarYGap[i-1]);

			fLastYDim=m_QuarticDimensions[0].arrVertBarYDim[i*TD_COLUMNSCNT+j];
		}
	}
#else
	int i,j;
	double fLastYDim, fMaxYDim;

	for(j=0;j<QUARTICCNT;j++){
		fMaxYDim=m_CfgParams.tdcfg.QuarticConf[j].fQuarticLength;

		//x-dir
		m_QuarticDimensions[j].arrTDQuarticXDim.resize(m_CfgParams.tdcfg.QuarticConf[j].nRowsCnt);
		m_QuarticDimensions[j].arrTDQuarticGapXDim.resize(m_CfgParams.tdcfg.QuarticConf[j].nRowsCnt);
		for(i=0;i<m_CfgParams.tdcfg.QuarticConf[j].nRowsCnt;i++){
			m_QuarticDimensions[j].arrTDQuarticXDim[i]=m_CfgParams.tdcfg.QuarticConf[j].vecBarXDim[i];
			m_QuarticDimensions[j].arrTDQuarticGapXDim[i]=(i<m_CfgParams.tdcfg.QuarticConf[j].nRowsCnt-1)? m_CfgParams.tdcfg.QuarticConf[j].vecXGaps[i]:0.0*CLHEP::mm;
		}

		//z-dir
		m_QuarticDimensions[j].arrTDQuarticZDim.resize(m_CfgParams.tdcfg.QuarticConf[j].nColsCnt);
		m_QuarticDimensions[j].arrTDQuarticGapZDim.resize(m_CfgParams.tdcfg.QuarticConf[j].nColsCnt);
		for(i=0;i<m_CfgParams.tdcfg.QuarticConf[j].nColsCnt;i++){
			m_QuarticDimensions[j].arrTDQuarticZDim[i]=m_CfgParams.tdcfg.QuarticConf[j].vecBarZDim[i];
			m_QuarticDimensions[j].arrTDQuarticGapZDim[i]=(i<m_CfgParams.tdcfg.QuarticConf[j].nColsCnt-1)? m_CfgParams.tdcfg.QuarticConf[j].vecZGaps[i]:0.0*CLHEP::mm;
		}

		//y-dir
		fLastYDim=fMaxYDim;
		m_QuarticDimensions[j].arrTDQuarticYDim.resize(m_CfgParams.tdcfg.QuarticConf[j].nColsCnt);
		for(i=0;i<m_CfgParams.tdcfg.QuarticConf[j].nColsCnt;i++){
			if(i==0) m_QuarticDimensions[j].arrTDQuarticYDim[i]=fLastYDim;
			else m_QuarticDimensions[j].arrTDQuarticYDim[i]=fLastYDim-(m_QuarticDimensions[j].arrTDQuarticZDim[i]+m_QuarticDimensions[j].arrTDQuarticGapZDim[i])/tan(m_CfgParams.tdcfg.QuarticConf[j].fSlope);

			fLastYDim=m_QuarticDimensions[j].arrTDQuarticYDim[i];
		}
	}
#endif

	// Surface definition (preliminary)
	m_pOpticalSurface = new GeoOpticalSurface("TDQuarticOpticalSurface", GeoOpticalSurface::unified, GeoOpticalSurface::polished, GeoOpticalSurface::dielectric_dielectric, GeoOpticalSurface::polished);
}

void AFP_GeoModelFactory::AddLBarTimingDetector(const int /*nQuarticID*/, GeoOpticalPhysVol* /*pPhysMotherVol*/, const char* /*pszStationName*/, HepGeom::Transform3D& /*TransInMotherVolume*/, GeoBorderSurfaceContainer* /*bsContainer*/)
{
#ifdef USE_TDLBARS
	double fz1,fz2,fy1,fy2,fyd,fzd,falpha;
	double fXShift,fYShift,fZShift;
	falpha=TD_NOMINALSLOPE;
	fyd=m_QuarticDimensions[0].arrVertBarYDim[0];
	fzd=m_QuarticDimensions[0].arrLBarZDim[0];
	fy1=0.5*fzd/tan(falpha);
	fy2=0.5*fyd-fy1;
	fz1=fzd/sin(falpha);
	fz2=fy2*cos(falpha);

	fXShift=m_QuarticDimensions[0].arrVertBarXDim[0]+TD_SLIMBARBORDERWIDTH;
	fYShift=0.5*fy2*sin(falpha);
	fZShift=fz1+fz2;


#endif
}

StatusCode AFP_GeoModelFactory::AddTimingDetector(const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* pBSContainer)
{
#ifdef USE_TDLBARS
#else
	int i,j,k,l;
	char szLabel[64];
	int nRowsCnt=m_CfgParams.tdcfg.QuarticConf[nQuarticID].nRowsCnt;
	int nColsCnt=m_CfgParams.tdcfg.QuarticConf[nQuarticID].nColsCnt;
	double fSlope=m_CfgParams.tdcfg.QuarticConf[nQuarticID].fSlope;

	std::vector<double> arrTDQuarticXDim=m_QuarticDimensions[nQuarticID].arrTDQuarticXDim;
	std::vector<double> arrTDQuarticGapXDim=m_QuarticDimensions[nQuarticID].arrTDQuarticGapXDim;
	std::vector<double> arrTDQuarticYDim=m_QuarticDimensions[nQuarticID].arrTDQuarticYDim;
	std::vector<double> arrTDQuarticZDim=m_QuarticDimensions[nQuarticID].arrTDQuarticZDim;
	std::vector<double> arrTDQuarticGapZDim=m_QuarticDimensions[nQuarticID].arrTDQuarticGapZDim;

	MsgStream LogStream(Athena::getMessageSvc(), "AFP_GeoModelFactory::AddTimingDetector");

	HepGeom::Point3D<double> TrackPointInFirstBarCS, TrackPointInAtlasCS;
	TrackPointInFirstBarCS=HepGeom::Point3D<double>(-0.5*arrTDQuarticXDim[0],0.5*arrTDQuarticYDim[0],-0.5*arrTDQuarticZDim[0]);

	//total transform of quartic in station
	std::map<int,GeoShape*>* pMapSolidTDQuartics=CreateSolidTDQuartics(nQuarticID);
	std::map<int,GeoShape*>* pMapSolidTDQuarticVacBorders=CreateSolidTDQuarticVacBorders(nQuarticID);
	//GeoBox* pQuarticFirstElement=(GeoBox*)m_MapShapes["TDQuarticFirstElement"];
	GeoBox* pQuarticFirstElement=(GeoBox*)(pMapSolidTDQuartics->operator[](0));

	double fXShift=DETXSIDE*((m_CfgParams.tdcfg.QuarticConf[nQuarticID].fCoverThickness+m_CfgParams.tdcfg.QuarticConf[nQuarticID].fCoverDistanceToFloor)+(pQuarticFirstElement->getXHalfLength()+TD_SLIMBARBORDERWIDTH));
	double fYShift=-(pQuarticFirstElement->getZHalfLength()+TD_SLIMBARBORDERWIDTH)*cos(fSlope)-(pQuarticFirstElement->getYHalfLength()+TD_SLIMBARBORDERWIDTH)*sin(fSlope);
	double fZShift=(pQuarticFirstElement->getZHalfLength()+TD_SLIMBARBORDERWIDTH)/sin(fSlope)+(pQuarticFirstElement->getYHalfLength()+TD_SLIMBARBORDERWIDTH)*cos(fSlope)-(pQuarticFirstElement->getZHalfLength()+TD_SLIMBARBORDERWIDTH)*sin(fSlope); //=l1+l2-l3
	HepGeom::Transform3D QrtTotTransform=TransInMotherVolume*HepGeom::Translate3D(fXShift,fYShift,fZShift)*HepGeom::RotateX3D(-(90.0*CLHEP::deg-fSlope));

	//add quartic bars and borders
	HepGeom::Transform3D QrtBarTransformInQuartic;

	for(l=0,i=0;i<nColsCnt;i++){
		for(j=0;j<nRowsCnt;j++){
			if(i!=0 || j!=0){
				fXShift=fZShift=0.0*CLHEP::mm;

				if(i<(int)arrTDQuarticYDim.size()){
					fYShift=0.5*(arrTDQuarticYDim[i]-arrTDQuarticYDim[0]);
				}
				else {
					LogStream<<MSG::ERROR<<"Q"<<nQuarticID<<": "<< "Index outside array bounds (arrTDQuarticYDim size: "<<arrTDQuarticYDim.size()<<"vs. colscnt "<<nColsCnt<<")"<< endreq;
					CHECK(StatusCode::FAILURE);
				}

				for(k=1;k<=j;k++){
					if(k<(int)arrTDQuarticXDim.size() && ((k-1)<(int)arrTDQuarticGapXDim.size())){
						fXShift+=DETXSIDE*(0.5*arrTDQuarticXDim[k-1]+0.5*arrTDQuarticXDim[k]+arrTDQuarticGapXDim[k-1]);
					}
					else {
						LogStream<<MSG::ERROR<<"Q"<<nQuarticID<<": "<< "Index outside array bounds (arrTDQuarticXDim size: "<<arrTDQuarticXDim.size()<<"vs. rowscnt "<<nRowsCnt<<")"<< endreq;
						LogStream<<MSG::ERROR<< "                               (arrTDQuarticGapXDim size: "<<arrTDQuarticGapXDim.size()<<"vs. rowscnt "<<nRowsCnt<<")"<< endreq;
						CHECK(StatusCode::FAILURE);
					}
				}

				for(k=1;k<=i;k++){
					if(k<(int)arrTDQuarticZDim.size() && ((k-1)<(int)arrTDQuarticGapZDim.size())){
						fZShift+=0.5*arrTDQuarticZDim[k-1]+0.5*arrTDQuarticZDim[k]+arrTDQuarticGapZDim[k-1];
					}
					else {
						LogStream<<MSG::ERROR<<"Q"<<nQuarticID<<": "<< "Index outside array bounds (arrTDQuarticZDim size: "<<arrTDQuarticZDim.size()<<"vs. colscnt "<<nColsCnt<<")"<< endreq;
						LogStream<<MSG::ERROR<< "                               (arrTDQuarticGapZDim size: "<<arrTDQuarticGapZDim.size()<<"vs. colscnt "<<nColsCnt<<")"<< endreq;
						CHECK(StatusCode::FAILURE);
					}
				}
				QrtBarTransformInQuartic=HepGeom::Translate3D(fXShift,fYShift,fZShift);
			}
			else{
				QrtBarTransformInQuartic=HepGeom::Translate3D();
			}

			/*
			if(i==0 && j==0){
				TrackPointInAtlasCS=pPhysMotherVol->getX()*QrtTotTransform*QrtBarTransformInQuartic*TrackPointInFirstBarCS;
				LogStream<<MSG::INFO<<"TrackPointInAtlasCS (station "<<pszStationName<<"): "<<TrackPointInAtlasCS<<endreq;
			}*/

			//quartic bar
			sprintf(szLabel,"%s_Q%i_LogTDQuarticBar[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			GeoLogVol* pLogTDQuartic=new GeoLogVol(szLabel,(*pMapSolidTDQuartics)[l],m_MapMaterials[std::string("Quartz")]);
			GeoOpticalPhysVol* pPhysTDQuartic=new GeoOpticalPhysVol(pLogTDQuartic);

			sprintf(szLabel,"%s_Q%i_TDQuarticBar[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			pPhysMotherVol->add(new GeoTransform(QrtTotTransform*QrtBarTransformInQuartic));
			pPhysMotherVol->add(pPhysTDQuartic);

			//vacuum border of quartic bar
			sprintf(szLabel,"%s_Q%i_LogTDQuarticBarVacBorder[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			GeoLogVol* pLogTDQuarticVacBorder=new GeoLogVol(szLabel,(*pMapSolidTDQuarticVacBorders)[l],m_MapMaterials[std::string("OpticalVacuum")]);
			GeoOpticalPhysVol* pPhysTDQuarticVacBorder=new GeoOpticalPhysVol(pLogTDQuarticVacBorder);

			sprintf(szLabel,"%s_Q%i_TDQuarticBarVacBorder[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			pPhysMotherVol->add(new GeoTransform(QrtTotTransform*QrtBarTransformInQuartic));
			pPhysMotherVol->add(pPhysTDQuarticVacBorder);

			sprintf(szLabel,"%s_Q%i_TDQuarticBarOptSurLayer[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			pBSContainer->push_back(GeoBorderSurface(szLabel, pPhysTDQuartic, pPhysTDQuarticVacBorder, m_pOpticalSurface));

			l++;
		}
	}

	//add sensors
	HepGeom::Transform3D SensTotTransform;
	std::map<int,GeoShape*>* pMapSolidSensElements=CreateSolidTDSensors(nQuarticID);
	//GeoBox* pSensorFirstElement=(GeoBox*)m_MapShapes["TDSensorFirstElement"];
	GeoBox* pSensorFirstElement=(GeoBox*)(pMapSolidSensElements->operator[](0));
    //fYShift=-(pQuarticFirstElement->getYHalfLength()+TDSLIMBARBORDERWIDTH+pSensorFirstElement->getYHalfLength()+0.0*CLHEP::mm); //assume no glue between bars and sensors! (TDSLIMBARBORDERWIDTH)
    fYShift=-(pQuarticFirstElement->getYHalfLength()+pSensorFirstElement->getYHalfLength()+0.0*CLHEP::mm);

	for(l=0,i=0;i<nColsCnt;i++){
		for(j=0;j<nRowsCnt;j++){
			fXShift=fZShift=0.0*CLHEP::mm;
			for(k=1;k<=j;k++) fXShift+=DETXSIDE*(0.5*arrTDQuarticXDim[k-1]+0.5*arrTDQuarticXDim[k]+arrTDQuarticGapXDim[k-1]);
			for(k=1;k<=i;k++) fZShift+=0.5*arrTDQuarticZDim[k-1]+0.5*arrTDQuarticZDim[k]+arrTDQuarticGapZDim[k-1];

			SensTotTransform=QrtTotTransform*HepGeom::Translate3D(fXShift,fYShift,fZShift);
			sprintf(szLabel,"%s_Q%i_LogTDSensor[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			GeoLogVol* pLogTDSensor=new GeoLogVol(szLabel,(*pMapSolidSensElements)[l++],m_MapMaterials[std::string("SiliconPMT")]);
			GeoFullPhysVol* pPhysTDSensor=new GeoFullPhysVol(pLogTDSensor);

			sprintf(szLabel,"%s_Q%i_TDSensor[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			pPhysMotherVol->add(new GeoTransform(SensTotTransform));
			pPhysMotherVol->add(pPhysTDSensor);
		}
	}

	//add blinds
	for(l=0,i=0;i<nColsCnt;i++){
		for(j=0;j<nRowsCnt;j++){
			fXShift=fZShift=0.0*CLHEP::mm;
			for(k=1;k<=j;k++) fXShift+=DETXSIDE*(0.5*arrTDQuarticXDim[k-1]+0.5*arrTDQuarticXDim[k]+arrTDQuarticGapXDim[k-1]);
            fYShift=(arrTDQuarticYDim[i]-pQuarticFirstElement->getYHalfLength())+TD_SLIMBARBORDERWIDTH+pSensorFirstElement->getYHalfLength()+0.0*CLHEP::mm;
			for(k=1;k<=i;k++) fZShift+=0.5*arrTDQuarticZDim[k-1]+0.5*arrTDQuarticZDim[k]+arrTDQuarticGapZDim[k-1];

			SensTotTransform=QrtTotTransform*HepGeom::Translate3D(fXShift,fYShift,fZShift);
			sprintf(szLabel,"%s_Q%i_LogTDBlind[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			GeoLogVol* pLogTDBlind=new GeoLogVol(szLabel,(*pMapSolidSensElements)[l++],m_MapMaterials[std::string("Silicon")]);
			GeoFullPhysVol* pPhysTDBlind=new GeoFullPhysVol(pLogTDBlind);

			sprintf(szLabel,"%s_Q%i_TDBlind[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			pPhysMotherVol->add(new GeoTransform(SensTotTransform));
			pPhysMotherVol->add(pPhysTDBlind);
		}
	}

	//add cover
	if(m_CfgParams.tdcfg.bAddCover){
		GeoShape* pAuxShape=NULL;
		GeoBox* pCoverMainElement=NULL;
		GeoShape* pSolidTDCover=CreateSolidTDCover(nQuarticID,&pAuxShape);
		pCoverMainElement=(GeoBox*)pAuxShape;
		//GeoBox* pCoverMainElement=(GeoBox*)m_MapShapes["TDCoverMainElement"];

		fXShift=-DETXSIDE*(pQuarticFirstElement->getXHalfLength()+TD_SLIMBARBORDERWIDTH-pCoverMainElement->getXHalfLength()+m_CfgParams.tdcfg.QuarticConf[nQuarticID].fCoverThickness);
		fYShift=-(pQuarticFirstElement->getYHalfLength()+TD_SLIMBARBORDERWIDTH-pCoverMainElement->getYHalfLength()+TDDETECTORHEIGHT);
		fZShift=-(pCoverMainElement->getZHalfLength()+pQuarticFirstElement->getZHalfLength()+TD_SLIMBARBORDERWIDTH);

		HepGeom::Transform3D CoverTotTransform=QrtTotTransform*HepGeom::Translate3D(fXShift,fYShift,fZShift);

		sprintf(szLabel,"%s_Q%i_LogTDCover",pszStationName,nQuarticID);
		GeoLogVol* pLogTDCover=new GeoLogVol(szLabel,pSolidTDCover,m_MapMaterials[std::string("Steel")]);
		GeoFullPhysVol* pPhysTDCover=new GeoFullPhysVol(pLogTDCover);

		sprintf(szLabel,"%s_Q%i_TDCover",pszStationName,nQuarticID);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		pPhysMotherVol->add(new GeoTransform(CoverTotTransform));
		pPhysMotherVol->add(pPhysTDCover);
	}
#endif

	return StatusCode::SUCCESS;
}

std::map<int,GeoShape*>*  AFP_GeoModelFactory::CreateSolidTDQuartics(const int nQuarticID)
{
	int i, j, k;
	double fCut=2.0*TD_SLIMBARBORDERWIDTH; //both sides
	GeoBox* pElement=NULL;
	std::map<int,GeoShape*>* pMapElements=new std::map<int,GeoShape*>();

#ifdef USE_TDLBARS
	for(k=0,i=0;i<TD_TRAINSCNT;i++){
		for(j=0;j<TD_COLUMNSCNT;j++){
			pElement=new GeoBox(0.5*(m_QuarticDimensions[nQuarticID].arrVertBarXDim[i]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrVertBarYDim[i*TD_COLUMNSCNT+j]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrLBarZDim[j]-fCut));
			pMapElements->insert(std::pair<int,GeoShape*>(k,pElement));
			//if(i==0 && j==0) m_MapShapes["TDQuarticFirstElement"]=pElement;
			k++;
		}
	}
#else
	for(k=0,i=0;i<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nColsCnt;i++){
		for(j=0;j<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nRowsCnt;j++){
			pElement=new GeoBox(0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticXDim[j]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticYDim[i]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticZDim[i]-fCut));
			pMapElements->insert(std::pair<int,GeoShape*>(k,pElement));
			//if(i==0 && j==0) m_MapShapes["TDQuarticFirstElement"]=pElement;
			k++;
		}
	}
#endif

	return pMapElements;
}

std::map<int,GeoShape*>*  AFP_GeoModelFactory::CreateSolidTDQuarticVacBorders(const int nQuarticID)
{
	std::map<int,GeoShape*>* pMapElements=new std::map<int,GeoShape*>();

#ifdef USE_TDLBARS
#else
	int i, j, k;
	double fCut=2.0*TD_SLIMBARBORDERWIDTH; //both sides
	GeoShapeSubtraction* pBoxSubtraction=NULL;
	GeoBox* pElement1=NULL, *pElement2=NULL;
	HepGeom::Transform3D SubTrans=HepGeom::Transform3D();
	GeoShapeShift *pSubShift;

	for(k=0,i=0;i<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nColsCnt;i++){
		for(j=0;j<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nRowsCnt;j++){
			pElement1=new GeoBox(0.5*m_QuarticDimensions[nQuarticID].arrTDQuarticXDim[j],0.5*m_QuarticDimensions[nQuarticID].arrTDQuarticYDim[i],0.5*m_QuarticDimensions[nQuarticID].arrTDQuarticZDim[i]);
			pElement2=new GeoBox(0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticXDim[j]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticYDim[i]-fCut),0.5*(m_QuarticDimensions[nQuarticID].arrTDQuarticZDim[i]-fCut));

			pSubShift=new GeoShapeShift(pElement2,SubTrans);
			pBoxSubtraction=new GeoShapeSubtraction(pElement1,pSubShift);

			pMapElements->insert(std::pair<int,GeoShape*>(k,pBoxSubtraction));
			//if(i==0 && j==0) m_MapShapes["TDQuarticVacBorderFirstElement"]=pBoxSubtraction;
			k++;
		}
	}
#endif

	return pMapElements;
}

std::map<int,GeoShape*>* AFP_GeoModelFactory::CreateSolidTDSensors(const int nQuarticID)
{
	std::map<int,GeoShape*>* pMapElements=new std::map<int,GeoShape*>();

#ifdef USE_TDLBARS
#else
	int i,j,k;
	GeoBox* pElement=NULL;

	const double cfYDim=TDSENSORTHICKNESS;
	for(k=0,i=0;i<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nColsCnt;i++){
		for(j=0;j<m_CfgParams.tdcfg.QuarticConf[nQuarticID].nRowsCnt;j++){
			pElement=new GeoBox(0.5*m_QuarticDimensions[nQuarticID].arrTDQuarticXDim[j],0.5*cfYDim,0.5*m_QuarticDimensions[nQuarticID].arrTDQuarticZDim[i]);
			pMapElements->insert(std::pair<int,GeoShape*>(k,pElement));
			//if(i==0 && j==0) m_MapShapes["TDSensorFirstElement"]=pElement;
			k++;
		}
	}
#endif

	return pMapElements;
}

GeoShape* AFP_GeoModelFactory::CreateSolidTDCover(const int nQuarticID,GeoShape** ppMainElement)
{

#ifdef USE_TDLBARS
#else
	int i;
	double fXDim, fYDim, fZDim, fAuxDim;
	//double fMainXDim;
	double fMainYDim, fMainZDim;
	GeoBox* pElement=NULL, *pMainElement=NULL;
	GeoShapeUnion* pBoxUnion=NULL, *pAuxBoxUnion;
	HepGeom::Transform3D TransBox;
	GeoShapeShift* pMoveBox=NULL;
	int nColsCnt=m_CfgParams.tdcfg.QuarticConf[nQuarticID].nColsCnt;
	int nRowsCnt=m_CfgParams.tdcfg.QuarticConf[nQuarticID].nRowsCnt;
	double fSlope=m_CfgParams.tdcfg.QuarticConf[nQuarticID].fSlope;
	double fCovThickness=m_CfgParams.tdcfg.QuarticConf[nQuarticID].fCoverThickness;

	std::vector<double> arrTDQuarticXDim=m_QuarticDimensions[nQuarticID].arrTDQuarticXDim;
	std::vector<double> arrTDQuarticGapXDim=m_QuarticDimensions[nQuarticID].arrTDQuarticGapXDim;
	std::vector<double> arrTDQuarticYDim=m_QuarticDimensions[nQuarticID].arrTDQuarticYDim;
	std::vector<double> arrTDQuarticZDim=m_QuarticDimensions[nQuarticID].arrTDQuarticZDim;
	std::vector<double> arrTDQuarticGapZDim=m_QuarticDimensions[nQuarticID].arrTDQuarticGapZDim;

	//main side (long side close to the highest quartic rods)
	fXDim=fYDim=fZDim=0.0*CLHEP::mm;
	for(i=0;i<nRowsCnt;i++) fXDim+=arrTDQuarticXDim[i]+arrTDQuarticGapXDim[i]; fXDim+=2.0*fCovThickness;
	fYDim=arrTDQuarticYDim[0]+arrTDQuarticZDim[0]/tan(fSlope)+TDDETECTORHEIGHT;//-fCovThickness/tan(fSlope);
	fZDim=fCovThickness;
	pMainElement=new GeoBox(0.5*fXDim,0.5*fYDim,0.5*fZDim);
	//fMainXDim=fXDim;
	fMainYDim=fYDim; fMainZDim=fZDim;

	//add bottom part
	fXDim=fYDim=fZDim=0.0*CLHEP::mm;
	for(i=0;i<nRowsCnt;i++) fXDim+=arrTDQuarticXDim[i]+arrTDQuarticGapXDim[i]; fXDim+=2.0*fCovThickness;
	fYDim=fCovThickness;
	for(i=0;i<nColsCnt;i++) fZDim+=arrTDQuarticZDim[i]+arrTDQuarticGapZDim[i]; fZDim+=2.0*fCovThickness;
	pElement=new GeoBox(0.5*fXDim,0.5*fYDim,0.5*fZDim);

	TransBox=HepGeom::Translate3D(0.0*CLHEP::mm,-(0.5*fMainYDim+0.5*fYDim),0.5*fZDim-0.5*fMainZDim);
	pMoveBox=new GeoShapeShift(pElement, TransBox);
	pBoxUnion=new GeoShapeUnion(pMainElement, pMoveBox);

	//add shaped side close to beam
	fXDim=fYDim=fZDim=0.0*CLHEP::mm;
	fXDim=fCovThickness;
	fYDim=arrTDQuarticYDim[nColsCnt-1]+TDDETECTORHEIGHT-0.1*CLHEP::mm;
	for(i=0;i<nColsCnt;i++) fZDim+=arrTDQuarticZDim[i]+arrTDQuarticGapZDim[i]-0.1*CLHEP::mm;
	pElement=new GeoBox(0.5*fXDim,0.5*fYDim,0.5*fZDim);

	double fL=sqrt(fYDim*fYDim+fZDim*fZDim);
	//pAuxElement=new GeoBox(0.5*fXDim,0.5*(fMainYDim-fYDim)+fCovThickness,0.5*fZDim);
	GeoBox* pAuxElement=new GeoBox(0.5*fXDim,0.5*(arrTDQuarticYDim[0]+arrTDQuarticZDim[0]/tan(fSlope)-arrTDQuarticYDim[nColsCnt-1]),0.5*fZDim);
	GeoBox* pCutter=new GeoBox(0.5*fXDim+0.01*CLHEP::mm,0.5*fL,0.5*fL);
	TransBox=HepGeom::Translate3D(0.0*CLHEP::mm,0.5*fL*sin(fSlope),0.5*fL*cos(fSlope))*HepGeom::RotateX3D(90.0*CLHEP::deg-fSlope);
	pMoveBox=new GeoShapeShift(pCutter, TransBox);
	GeoShapeSubtraction* pAuxBoxSub=new GeoShapeSubtraction(pAuxElement, pMoveBox);

	TransBox=HepGeom::Translate3D(0.0*CLHEP::mm,pElement->getYHalfLength()+pAuxElement->getYHalfLength(),0.0*CLHEP::mm);
	pMoveBox=new GeoShapeShift(pAuxBoxSub, TransBox);
	pAuxBoxUnion=new GeoShapeUnion(pElement, pMoveBox);

	TransBox=HepGeom::Translate3D(-DETXSIDE*(pMainElement->getXHalfLength()-pElement->getXHalfLength()),pElement->getYHalfLength()-pMainElement->getYHalfLength(),pElement->getZHalfLength()+pMainElement->getZHalfLength());
	pMoveBox=new GeoShapeShift(pAuxBoxUnion, TransBox);
	pBoxUnion=new GeoShapeUnion(pBoxUnion, pMoveBox);

	//add shaped side far from beam
	TransBox=HepGeom::Translate3D(DETXSIDE*(pMainElement->getXHalfLength()-pElement->getXHalfLength()+fCovThickness),pElement->getYHalfLength()-pMainElement->getYHalfLength(),pElement->getZHalfLength()+pMainElement->getZHalfLength());
	pMoveBox=new GeoShapeShift(pAuxBoxUnion, TransBox);
	pBoxUnion=new GeoShapeUnion(pBoxUnion, pMoveBox);

	//add upper side
	fXDim=fYDim=fZDim=0.0*CLHEP::mm; fAuxDim=0.0*CLHEP::mm;
	for(i=0;i<nRowsCnt;i++) fXDim+=arrTDQuarticXDim[i]+arrTDQuarticGapXDim[i]; fXDim+=2*fCovThickness;
	fYDim=fCovThickness;
	for(i=0;i<nColsCnt;i++) fAuxDim+=arrTDQuarticZDim[i]+arrTDQuarticGapZDim[i]; fAuxDim+=fCovThickness; fZDim=fAuxDim/sin(fSlope);
	pElement=new GeoBox(0.5*fXDim,0.5*fYDim,0.5*fZDim);

	//double fAux=0.5*fYDim*(1/sin(fSlope)+1/tan(fSlope)-sin(fSlope));
	double fAux=0.5*fYDim*(1/sin(fSlope)+1/tan(fSlope));
	//double fAux=0.0*CLHEP::mm;
	double fYShift=0.5*fMainYDim-0.5*fZDim*cos(fSlope)+fAux;
	double fZShift=0.5*fZDim*sin(fSlope)+0.5*fYDim*(1-cos(fSlope));

	TransBox=HepGeom::Translate3D(0.0*CLHEP::mm,fYShift,fZShift)*HepGeom::RotateX3D(90.0*CLHEP::deg-fSlope);
	pMoveBox=new GeoShapeShift(pElement, TransBox);
	pBoxUnion=new GeoShapeUnion(pBoxUnion, pMoveBox);

	//add opposite side (close to the lowest quartic rods)
	fXDim=fYDim=fZDim=0.0*CLHEP::mm;
	for(i=0;i<nRowsCnt;i++) fXDim+=arrTDQuarticXDim[i]+arrTDQuarticGapXDim[i]; fXDim+=2*fCovThickness;
	fYDim=arrTDQuarticYDim[nColsCnt-1]+TDDETECTORHEIGHT;//-fCovThickness/tan(fSlope);
	fZDim=fCovThickness;
	pElement=new GeoBox(0.5*fXDim,0.5*fYDim,0.5*fZDim);

	fAuxDim=0.0*CLHEP::mm; //slight cut due to vp1;
	for(i=0;i<nColsCnt;i++) fAuxDim+=arrTDQuarticZDim[i]+arrTDQuarticGapZDim[i];
	TransBox=HepGeom::Translate3D(0.0*CLHEP::mm,0.5*(fYDim-fMainYDim),0.5*fMainZDim+0.5*fZDim+fAuxDim);
	pMoveBox=new GeoShapeShift(pElement, TransBox);
	pBoxUnion=new GeoShapeUnion(pBoxUnion, pMoveBox);

	//m_MapShapes["TDCoverMainElement"]=pMainElement;
	*ppMainElement=pMainElement;
#endif

	return pBoxUnion;

}

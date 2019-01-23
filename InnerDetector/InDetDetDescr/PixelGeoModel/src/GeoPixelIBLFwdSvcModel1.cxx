/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Build IBL fwd services (wavy shape)
// This is built one time per layer. 

#include "GeoPixelIBLFwdSvcModel1.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <algorithm>
#include <iostream> 
#include <iomanip> 
using std::max;

GeoPixelIBLFwdSvcModel1::GeoPixelIBLFwdSvcModel1(int /*section*/): 
  m_supportPhysA(0),
  m_supportPhysC(0),
  m_xformSupportA(0),
  m_xformSupportC(0)
{
}

GeoVPhysVol* GeoPixelIBLFwdSvcModel1::Build()
{

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services"<<endmsg;

  //  double safety = 0.01*Gaudi::Units::mm;

  // IBL layer shift ( 2mm shift issue )
  double layerZshift = m_gmt_mgr->PixelLayerGlobalShift();
  int nSectors = m_gmt_mgr->NPixelSectors();
  double phiOfModuleZero =  m_gmt_mgr->PhiOfModuleZero();  
  double layerRadius = m_gmt_mgr->PixelLayerRadius();

  // check if sectors are properly defined
  if(nSectors==0) return 0;
  double angle=360./(double)nSectors*Gaudi::Units::deg;
  
  // Defines the IBL_Fwd02 section in the IBL services area
  double innerRadius = 33.;
  double outerRadius = 42.499;
  double zMin = 834.607;
  double zMax = 3290.795;

  double zLength = zMax-zMin;
  double halfLength = zLength*.5;
  double zMiddle = (zMin+zMax)*.5;

  // Define IBL fwd svc section for side A and C
  std::ostringstream lnameA;
  lnameA<<"Brl0A_FwdSvc";
  std::ostringstream lnameC;
  lnameC<<"Brl0C_FwdSvc";

  // Build encompassing volume for both A and C sides (assemblies)
  const GeoTube* supportShapeA = new GeoTube(innerRadius,outerRadius,halfLength);
  const GeoTube* supportShapeC = new GeoTube(innerRadius,outerRadius,halfLength);
  const GeoMaterial* ether = m_mat_mgr->getMaterial("special::Ether");  
  //  const GeoMaterial* ether = m_mat_mgr->getMaterial("std::Air");  
  GeoLogVol* supportLogVol_A = new GeoLogVol(lnameA.str(),supportShapeA,ether);
  GeoLogVol* supportLogVol_C = new GeoLogVol(lnameC.str(),supportShapeC,ether);

  // Shift in phi
  double pi = 3.141592654; 
  //  double deltaPhi = pi/28.;

  // Data taken fron CATIA desgin file
  double devTotalLength = 2460.188;

  // Cable bundle sizes
  double rminCable = 0.;
  double rmaxCable = 8.*.5;
  double surfCable = rmaxCable*rmaxCable*pi;

  // Cooling tube sizes
  double rminCooling = 0.;
  double rmaxCooling = 1.75*.5;
  double surfCooling = rmaxCooling*rmaxCooling*pi;

  // IPT clip starting position
  //  double zpos0 = 903.;
  double zpos = 0.;
  //  double zposRing = 0.;
  double totalLength=0.;
  //  double hermJunction = .4;
  double breakAngle = 11.*Gaudi::Units::deg;

  double cooling_radius = 35.1;
  double cooling_angle = -2.154*Gaudi::Units::deg;

  if(m_gmt_mgr->PixelStaveAxe()==1)   
    {
      cooling_radius = 34.7 + layerRadius-33.25;
      cooling_angle = -.1*Gaudi::Units::deg;
    }

  double cable_radius = 36.501;
  //  double cable_radius = innerRadius+rmaxCable+0.001;

  // Eta steps
  int nbSteps = 3;
  double etaSteps[3] = {3.8, 4.3, 4.8 };
  
  // Z steps
  //  double zDelta1 = 50;
  std::vector<double> zSteps;
  bool bFirstLin = true;
  for(int i=0; i<nbSteps; i++){
    double tmp = 2.*atan(exp(etaSteps[i]));
    double tmp2 = tan(tmp);
    double z = fabs(cable_radius/tmp2);
    
    if(i==0) {
      if(z>zMin){
	zSteps.push_back(zMin+0.001);
	zSteps.push_back(z);
      }
      else {
	//	bFirstLin = false;
	zSteps.push_back(zMin+0.001);
	zSteps.push_back(zMin+10.);
      }
    }
    else
      zSteps.push_back(z);

    //    std::cout<<"Eta/Z : "<<etaSteps[i]<<" "<<z<<std::endl;
  }
  zSteps.push_back(zMax-0.001);
  nbSteps+=1;
  if(bFirstLin)nbSteps++;

  //  Loop over the wavy shape sections to build a cable and a cooling pipe
  const GeoShape * gblShapeCableA = 0;
  const GeoShape * gblShapeCoolingA = 0;
  const GeoShape * gblShapeCableC = 0;
  const GeoShape * gblShapeCoolingC = 0;
  
//   deltaPhi = (2.*pi)/28.;
//   deltaPhi = .2815;
//   double deltaMiddle_all = cable_radius*tan(deltaPhi);

  double deltaMiddle_all = 18.;

  //  deltaMiddle_all = 10.5.;
  //  std::cout<<"IBL fwd : "<<deltaMiddle_all<<std::endl;

  zpos = zSteps[0];
  int angleSign = 1;
  for(int iStep=0; iStep<nbSteps-1; iStep++)
    {
      double zInit = zSteps[iStep];
      double zFinal = zSteps[iStep+1];
      double zMid = (zInit+zFinal)*.5;
      double zHalfLength = zMid-zInit;
      double zLength = zFinal-zInit;

      //      std::cout<<"--- STEP "<<std::setprecision(13)<<iStep<<"  "<<zInit<<" "<<zFinal<<std::endl;

//       double deltaPhiLoc = fabs(atan(deltaMiddle_all/(zHalfLength));      
//       double cableHalfLength = zHalfLength / cos(deltaPhiLoc);
//       double cableLength = zHalfLength / cos(deltaPhiLoc);
//       double deltaMiddleLoc = deltaMiddle_all*.5;

      double deltaPhiLoc = fabs(atan(deltaMiddle_all/zLength));      
      double cableHalfLength = (zLength / cos(deltaPhiLoc))*.5;
      double deltaMiddleLoc = deltaMiddle_all*.5;

      //      std::cout<<"         "<<zHalfLength<<"/"<<cableHalfLength<<"   "<<deltaMiddleLoc<<std::endl;

      if((iStep==0&&bFirstLin)||iStep==nbSteps-2) {
	// linear section 
	zpos += zHalfLength;

	// Cable
	const GeoTube* cableShape = new GeoTube(rminCable, rmaxCable, zHalfLength);	
	double angle = 0.; //11.*Gaudi::Units::deg;
	GeoTrf::Transform3D trfA1 = GeoTrf::RotateZ3D(angle)*GeoTrf::TranslateZ3D(zpos-zMiddle);
	gblShapeCableA = addShape(gblShapeCableA, cableShape, trfA1 );
	GeoTrf::Transform3D trfC1 = GeoTrf::RotateZ3D(angle)*GeoTrf::TranslateZ3D(zMax-(zpos-zMin)-zMiddle);
	gblShapeCableC = addShape(gblShapeCableC, cableShape, trfC1 );

	// Cooling
	const GeoTube* coolingShape = new GeoTube(rminCooling, rmaxCooling, zHalfLength);	
	gblShapeCoolingA = addShape(gblShapeCoolingA, coolingShape, trfA1 );
	gblShapeCoolingC = addShape(gblShapeCoolingC, coolingShape, trfC1 );

	zpos += zHalfLength;
	totalLength+=2.*zHalfLength;
      }
      else {

	// First section
	zpos += zHalfLength;

	// Cable
	const GeoTube* cableShape = new GeoTube(rminCable, rmaxCable, cableHalfLength);
	double angle= 0.; 
	GeoTrf::Transform3D trfA1 = GeoTrf::RotateZ3D(angle)*GeoTrf::TranslateY3D(deltaMiddleLoc)* GeoTrf::TranslateZ3D(zpos-zMiddle)*GeoTrf::RotateX3D(-angleSign*deltaPhiLoc);
	gblShapeCableA = addShape(gblShapeCableA, cableShape, trfA1 );
	GeoTrf::Transform3D trfC1 = GeoTrf::RotateZ3D(angle)*GeoTrf::TranslateY3D(deltaMiddleLoc)* GeoTrf::TranslateZ3D(zMax-(zpos-zMin)-zMiddle)*GeoTrf::RotateX3D(angleSign*deltaPhiLoc);
	gblShapeCableC = addShape(gblShapeCableC, cableShape, trfC1 );
	
	// Cooling
	const GeoTube* coolingShape = new GeoTube(rminCooling, rmaxCooling, cableHalfLength);	
	gblShapeCoolingA = addShape(gblShapeCoolingA, coolingShape, trfA1 );
	gblShapeCoolingC = addShape(gblShapeCoolingC, coolingShape, trfC1 );

 	zpos += zHalfLength;
	totalLength+=2.*cableHalfLength;
	angleSign*=-1;
      }
     
      
    }

  //  std::cout<<"--- MATERIAL  : length "<<devTotalLength<<"   surf "<<surfCable<<std::endl;

  // Material (material budget computed based on the detailed description of a cable bundle and cooling pipe
  const GeoMaterial* cableMat = m_mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cable_Wvy_M1",surfCable*devTotalLength);

  //  std::cout<<"--- MATERIAL  : cable defined "<<(cableMat==0)<<std::endl;

  GeoLogVol * cable_logA = new GeoLogVol("IBL_Fwd02_Cable_A",gblShapeCableA,cableMat);
  GeoLogVol * cable_logC = new GeoLogVol("IBL_Fwd02_Cable_C",gblShapeCableC,cableMat);

  const GeoMaterial* coolingMat = m_mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cooling_Wvy_M1",surfCooling*devTotalLength);
  //  std::cout<<"--- MATERIAL  : cooling defined "<<(cableMat==0)<<std::endl;
  GeoLogVol * cooling_logA = new GeoLogVol("IBL_Fwd02_Cooling_A",gblShapeCoolingA,coolingMat);
  GeoLogVol * cooling_logC = new GeoLogVol("IBL_Fwd02_Cooling_C",gblShapeCoolingC,coolingMat);

  // ----------- Create the PhysVol object
  m_supportPhysA = new GeoPhysVol(supportLogVol_A);
  m_supportPhysC = new GeoPhysVol(supportLogVol_C);

  // -------------- Alignement of the services with the previous services (Fwd01 area)
  GeoPhysVol* cablePhysVolA = new GeoPhysVol(cable_logA);
  GeoPhysVol* cablePhysVolC = new GeoPhysVol(cable_logC);
  GeoPhysVol* coolingPhysVolA = new GeoPhysVol(cooling_logA);
  GeoPhysVol* coolingPhysVolC = new GeoPhysVol(cooling_logC);

  //  std::cout<<"--- LOOP over sectors"<<std::endl;

  //  nSectors = 1;

  // Loop over the sectors to place the 14 bundles and cooling pipes
  for(int ii=0; ii<nSectors; ii++)
    {
      m_gmt_mgr->SetPhi(ii);    
      
      // cooling transform 
      double phiOfCooling = phiOfModuleZero+ cooling_angle + ii*angle;
      
      std::ostringstream tmp1; 
      tmp1 << "IBL_Fwd02_Cooling_AC" << ii;
      GeoNameTag * tag1 = new GeoNameTag(tmp1.str());
      GeoTransform* xformA1 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCooling)*GeoTrf::TranslateX3D(cooling_radius));
      m_supportPhysA->add(tag1);
      m_supportPhysA->add(xformA1);
      m_supportPhysA->add(coolingPhysVolA);
      
      GeoTransform* xformC1 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCooling)*GeoTrf::TranslateX3D(cooling_radius));
      m_supportPhysC->add(tag1);
      m_supportPhysC->add(xformC1);
      m_supportPhysC->add(coolingPhysVolC);

      double phiOfCable = phiOfCooling + angle*.5;

      std::ostringstream tmp2; 
      tmp2 << "IBL_Fwd02_Cable_AC" << ii;
      GeoNameTag * tag2 = new GeoNameTag(tmp2.str());
      GeoTransform* xformA2 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCable)*GeoTrf::TranslateX3D(cable_radius)*GeoTrf::RotateZ3D(breakAngle));
      m_supportPhysA->add(tag2);
      m_supportPhysA->add(xformA2);
      m_supportPhysA->add(cablePhysVolA);
      
      GeoTransform* xformC2 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCable)*GeoTrf::TranslateX3D(cable_radius)*GeoTrf::RotateZ3D(breakAngle));
      m_supportPhysC->add(tag2);
      m_supportPhysC->add(xformC2);
      m_supportPhysC->add(cablePhysVolC);
    }

  // Global transforms that include the IBL shift in Z
  double middleA = zMiddle+layerZshift;
  double middleC = -zMiddle+layerZshift;

  GeoTrf::Transform3D supportTrfA = GeoTrf::TranslateZ3D(middleA);
  m_xformSupportA = new GeoTransform(supportTrfA);
  
  GeoTrf::Transform3D supportTrfC = GeoTrf::TranslateZ3D(middleC);
  m_xformSupportC = new GeoTransform(supportTrfC);

  return 0;
}


const GeoShape * GeoPixelIBLFwdSvcModel1::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}

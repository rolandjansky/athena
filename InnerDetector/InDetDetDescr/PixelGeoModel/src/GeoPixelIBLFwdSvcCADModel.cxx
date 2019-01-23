/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Build IBL fwd services (wavy shape)
// This is built one time per layer. 

#include "GeoPixelIBLFwdSvcCADModel.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"

#if defined(GEOTORUS) && GEOTORUS==1
    #include "GeoModelKernel/GeoTorus.h"
#endif

#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <algorithm>
#include <iostream> 
#include <iomanip> 
using std::max;

GeoPixelIBLFwdSvcCADModel::GeoPixelIBLFwdSvcCADModel(int /*section*/): 
  m_supportPhysA(0),
  m_supportPhysC(0),
  m_xformSupportA(0),
  m_xformSupportC(0)
{
}

GeoVPhysVol* GeoPixelIBLFwdSvcCADModel::Build()
{

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - CAD tool design - Build()"<<endmsg;
  
#if !defined(GEOTORUS) || GEOTORUS==0

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - CAD tool design - Torus object is not defined "<<GEOTORUS<<endmsg;
  return 0;

#else

  m_gmt_mgr->msg(MSG::INFO) <<"Build IBL fwd services - CAD tool design - Torus object is defined"<<endmsg;

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
  GeoLogVol* supportLogVol_A = new GeoLogVol(lnameA.str(),supportShapeA,ether);
  GeoLogVol* supportLogVol_C = new GeoLogVol(lnameA.str(),supportShapeC,ether);

  // Data taken fron CATIA desgin file
  double delta = 18.;
  double deltaJunction = 0.;
  double lgFwdSvc[4]={511., 561., 560., 706. };
  double devLgFwdSvc[4]={512., 562., 562., 707. };
  double devTotalLength = 2460.188;
  double pi = Gaudi::Units::pi;

  // Cable bundle sizes
  double rminCable = 0.;
  double rmaxCable = 8.*.5;
  double surfCable = rmaxCable*rmaxCable*pi;

  // Cooling tube sizes
  double rminCooling = 0.;
  double rmaxCooling = 1.75*.5;
  double surfCooling = rmaxCooling*rmaxCooling*pi;

  // IPT clip starting position
  double zpos0 = 903.;
  double zpos = 0.;
  double zposRing = 0.;
  double totalLength=0.;
  double hermJunction = .4;
  double breakAngle = 11.*Gaudi::Units::deg;

  //  Loop over the wavy shape sections to build a cable and a cooling pipe
  const GeoShape * gblShapeCableA = 0;
  const GeoShape * gblShapeCoolingA = 0;
  const GeoShape * gblShapeCableC = 0;
  const GeoShape * gblShapeCoolingC = 0;
  for(int i=0; i<4; i++)
    {
      totalLength+=devLgFwdSvc[i];

      double initHalfLength = lgFwdSvc[i]*.5;
      zpos += initHalfLength;
      double corHalfLength = initHalfLength-deltaJunction;
      double radius = .5*(corHalfLength*corHalfLength+delta*delta)/delta;
      double theta = asin(corHalfLength/radius);

      if(i==0){
        double tubeLength=deltaJunction+zpos0-zMin+hermJunction;
        double zposTrans=zMin+tubeLength*.5-zpos0;
	const GeoTube* tubeShape = new GeoTube(0., rmaxCable, tubeLength*.5);
	GeoTrf::Transform3D trfA =  GeoTrf::TranslateZ3D(zpos0+zposTrans-zMiddle);
	gblShapeCableA = addShape(gblShapeCableA, tubeShape, trfA );
	GeoTrf::Transform3D trfC =  GeoTrf::RotateY3D(pi)*GeoTrf::TranslateZ3D(zpos0+zposTrans-zMiddle);
	gblShapeCableC = addShape(gblShapeCableC, tubeShape, trfC );
	
	//	std::cout<<"IBL_fwd : junction "<<std::setprecision(15)<<i<<" : "<<zpos0+zposTrans-tubeLength*.5<<" "<<zpos0+zposTrans+tubeLength*.5<<" // "<<zMin<<std::endl;
	
	const GeoTube* tubeShape_cooling = new GeoTube(0., rmaxCooling, tubeLength*.5);
	gblShapeCoolingA = addShape(gblShapeCoolingA, tubeShape_cooling, trfA );
	gblShapeCoolingC = addShape(gblShapeCoolingC, tubeShape_cooling, trfC );
      }

      zposRing += 2.*initHalfLength;
      //      std::cout<<"Torus radius : "<<radius<<"   "<<perimeter*(2.*theta)/(2.*pi)+2*deltaJunction<<" / "<<devLgFwdSvc[i]<<std::endl; 

      double rtor = radius;
      const GeoTorus* torusShape = new GeoTorus(rminCable,rmaxCable,rtor,pi*.5-theta,2.*theta);
      double angle=(i%2==0)?breakAngle:pi-breakAngle;
      GeoTrf::Transform3D trfA = GeoTrf::RotateZ3D(angle)*GeoTrf::RotateY3D(-pi*.5)*GeoTrf::TranslateY3D(-radius+delta)* GeoTrf::TranslateX3D(zpos0+zpos-zMiddle);
      gblShapeCableA = addShape(gblShapeCableA, torusShape, trfA );
      GeoTrf::Transform3D trfC = GeoTrf::RotateZ3D(angle)*GeoTrf::RotateY3D(pi*.5)*GeoTrf::TranslateY3D(-radius+delta)* GeoTrf::TranslateX3D(zpos0+zpos-zMiddle);
      gblShapeCableC = addShape(gblShapeCableC, torusShape, trfC );

      const GeoTorus* torusShape_cooling = new GeoTorus(rminCooling,rmaxCooling,rtor,pi*.5-theta,2.*theta);
      gblShapeCoolingA = addShape(gblShapeCoolingA, torusShape_cooling, trfA );
      gblShapeCoolingC = addShape(gblShapeCoolingC, torusShape_cooling, trfC );
      
      zpos += initHalfLength;

      if(i==3){
	zpos=zpos0+zpos-deltaJunction;
	double tubeLength=zMax-zpos+hermJunction;
	double zposTrans=zMax-tubeLength*.5;
	const GeoTube* tubeShape = new GeoTube(0., rmaxCable, tubeLength*.5);
	GeoTrf::Transform3D trfA =  GeoTrf::TranslateZ3D(zposTrans-zMiddle);
 	gblShapeCableA = addShape(gblShapeCableA, tubeShape, trfA );
	GeoTrf::Transform3D trfC =  GeoTrf::RotateY3D(pi)*GeoTrf::TranslateZ3D(zposTrans-zMiddle);
 	gblShapeCableC = addShape(gblShapeCableC, tubeShape, trfC );

 	const GeoTube* tubeShape_cooling = new GeoTube(0., rmaxCooling, tubeLength*.5);
 	gblShapeCoolingA = addShape(gblShapeCoolingA, tubeShape_cooling, trfA );
 	gblShapeCoolingC = addShape(gblShapeCoolingC, tubeShape_cooling, trfC );
      }

    }


  // Material (material budget computed based on the detailed description of a cable bundle and cooling pipe
  const GeoMaterial* cableMat = m_mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cable_Wvy",surfCable*devTotalLength);
  GeoLogVol * cable_logA = new GeoLogVol("IBL_Fwd02_Cable_A",gblShapeCableA,cableMat);
  GeoLogVol * cable_logC = new GeoLogVol("IBL_Fwd02_Cable_C",gblShapeCableC,cableMat);

  const GeoMaterial* coolingMat = m_mat_mgr->getMaterialForVolume("pix::IBL_Fwd02_Cooling_Wvy",surfCooling*devTotalLength);
  GeoLogVol * cooling_logA = new GeoLogVol("IBL_Fwd02_Cooling_A",gblShapeCoolingA,coolingMat);
  GeoLogVol * cooling_logC = new GeoLogVol("IBL_Fwd02_Cooling_C",gblShapeCoolingC,coolingMat);

  // ----------- Create the PhysVol object
  m_supportPhysA = new GeoPhysVol(supportLogVol_A);
  m_supportPhysC = new GeoPhysVol(supportLogVol_C);

  // -------------- Alignement of the services with the previous services (Fwd01 area)
  double cooling_radius = 35.1;
  double cooling_angle = -2.154*Gaudi::Units::deg;

  if(m_gmt_mgr->PixelStaveAxe()==1)   
    {
      cooling_radius = 34.7 + layerRadius-33.25;
      cooling_angle = -.1*Gaudi::Units::deg;
    }

  double cable_radius = 36.501;
  GeoPhysVol* cablePhysVolA = new GeoPhysVol(cable_logA);
  GeoPhysVol* cablePhysVolC = new GeoPhysVol(cable_logC);
  GeoPhysVol* coolingPhysVolA = new GeoPhysVol(cooling_logA);
  GeoPhysVol* coolingPhysVolC = new GeoPhysVol(cooling_logC);

  //    nSectors=1;

  // Loop over the sectors to place the 14 bundles and cooling pipes
  for(int ii=0; ii<nSectors; ii++)
    {
      m_gmt_mgr->SetPhi(ii);    
      
      // cooling transform 
      double phiOfCooling = phiOfModuleZero+ cooling_angle + ii*angle;
      
      std::ostringstream tmp1; 
      tmp1 << "IBL_Fwd02_Cooling_A" << ii;
      GeoNameTag * tag1 = new GeoNameTag(tmp1.str());
      GeoTransform* xformA1 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCooling)*GeoTrf::TranslateX3D(cooling_radius));
      m_supportPhysA->add(tag1);
      m_supportPhysA->add(xformA1);
      m_supportPhysA->add(coolingPhysVolA);
      
      std::ostringstream tmp1C; 
      tmp1C << "IBL_Fwd02_Cooling_C" << ii;
      GeoNameTag * tag1C = new GeoNameTag(tmp1C.str());
      GeoTransform* xformC1 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCooling)*GeoTrf::TranslateX3D(cooling_radius));
      m_supportPhysC->add(tag1C);
      m_supportPhysC->add(xformC1);
      m_supportPhysC->add(coolingPhysVolC);
      
      double phiOfCable = phiOfCooling + angle*.5;
      
      std::ostringstream tmp2; 
      tmp2 << "IBL_Fwd02_Cable_A" << ii;
      GeoNameTag * tag2 = new GeoNameTag(tmp2.str());
      GeoTransform* xformA2 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCable)*GeoTrf::TranslateX3D(cable_radius));
      m_supportPhysA->add(tag2);
      m_supportPhysA->add(xformA2);
      m_supportPhysA->add(cablePhysVolA);
      
      std::ostringstream tmp2C; 
      tmp2C << "IBL_Fwd02_Cable_C" << ii;
      GeoNameTag * tag2C = new GeoNameTag(tmp2C.str());
      GeoTransform* xformC2 = new GeoTransform(GeoTrf::RotateZ3D(phiOfCable)*GeoTrf::TranslateX3D(cable_radius));
      m_supportPhysC->add(tag2C);
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

#endif

}


const GeoShape * GeoPixelIBLFwdSvcCADModel::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}

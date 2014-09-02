/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/FeetToroidBuilderRDB.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "MuonGeoModel/ArrayFunction.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoSerialTransformer.h" 
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/GenericFunctions/Variable.hh"

#include <stdexcept>
#include <vector>
#include <iomanip>  

#include <sstream>
typedef std::stringstream  my_sstream;
typedef std::ostringstream my_osstream;

using namespace Genfun;
using namespace GeoXF;

namespace MuonGM {
FeetToroidBuilderRDB::FeetToroidBuilderRDB( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
				    std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
    //  std::cout<<" FeetToroidBuilderRDB constructor "<<std::endl;
    
  MsgStream log( Athena::getMessageSvc(), "MuGM:FeetToroidBuildRDB" );
  log << MSG::INFO << "Fetching data with tag <" << geoTag << ">" << endreq;

  m_Abrt = pRDBAccess->getRecordsetPtr("ABRT", geoTag, geoNode);
  if (m_Abrt->size() == 0) {
      log<<MSG::WARNING<<"Table ABRT not found in tag <"  <<  geoTag <<  ">"  <<" reading table ABRT-00" <<endreq;
      m_Abrt = pRDBAccess->getRecordsetPtr("ABRT","ABRT-00");
  }
  m_Feet = pRDBAccess->getRecordsetPtr("FEET", geoTag, geoNode);
  if (m_Feet->size() == 0) {
      log<<MSG::WARNING<<"Table FEET not found in tag <"  <<  geoTag <<  ">"  <<" reading table FEET-00" <<endreq;
      m_Feet = pRDBAccess->getRecordsetPtr("FEET","FEET-00");
  }
  m_Aect = pRDBAccess->getRecordsetPtr("AECT", geoTag, geoNode);
  if (m_Aect->size() == 0) {
      log<<MSG::WARNING<<"Table ABRT not found in tag <"  <<  geoTag <<  ">"  <<" reading table AECT-00" <<endreq;
      m_Aect = pRDBAccess->getRecordsetPtr("AECT","AECT-00");
  }
  m_Rail = pRDBAccess->getRecordsetPtr("RAIL", geoTag, geoNode);
  if (m_Rail->size() == 0) {
      log<<MSG::WARNING<<"Table RAIL not found in tag <"  <<  geoTag <<  ">"  <<" reading table RAIL-00" <<endreq;
      m_Rail = pRDBAccess->getRecordsetPtr("RAIL","RAIL-00");
  }

  //  std::cout <<" FeetToroidBuilderRDB : maxDim "<< maxDim <<" nFeet "<< nFeet <<" nVertex "<< nVertex <<std::endl;

  std::string Iron = "Iron";
  std::string Aluminium = "Alum";

}

void FeetToroidBuilderRDB::buildStandardFeet( GeoPhysVol* container ) 
{
//------------------------------------------------------------------------------------------
//  Builds Standard Feet (all but the two outermost feet pairs) -- sideplates, bottom plate, 
//  inner and top vertical plates  
//------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//------------ Feet General Parameters ---------------------------------//
  double zPositionFeet[maxDim];
  zPositionFeet[0] = -(*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
  zPositionFeet[1] = -(*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
  zPositionFeet[2] = -(*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
  zPositionFeet[3] = -(*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
  zPositionFeet[4] =  (*m_Feet)[0]->getFloat("ZPOSFEE1") * mm;
  zPositionFeet[5] =  (*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
  zPositionFeet[6] =  (*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
  zPositionFeet[7] =  (*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
  zPositionFeet[8] =  (*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;

   const double thicknessSidePlate   = (*m_Feet)[0]->getFloat("MAINPLDZ") * mm;
   const double thicknessBottomPlate = (*m_Feet)[0]->getFloat("MNPLPEHE") * mm;
   const double thicknessLargePlate  = (*m_Feet)[0]->getFloat("VCNPLXWI") * mm;
   const double thicknessOuterPlate  = (*m_Feet)[0]->getFloat("SLACPLYH") * mm;

   const std::string feetMaterial = getMaterial( "Iron" );
//------------ Standard Feet Parameters ----------------------------------------------
   const double zWidth = (*m_Feet)[0]->getFloat("MNPLZSEP") * mm;

   double xPosVertexStandFeet[maxDim], yPosVertexStandFeet[maxDim];

   xPosVertexStandFeet[0] = (*m_Feet)[0]->getFloat("MAINPLXB") * mm;
   yPosVertexStandFeet[0] = (*m_Feet)[0]->getFloat("MAINPLYB") * mm;
   xPosVertexStandFeet[1] = (*m_Feet)[0]->getFloat("MAINPLXC") * mm;
   yPosVertexStandFeet[1] = (*m_Feet)[0]->getFloat("MAINPLYC") * mm;
   xPosVertexStandFeet[2] = (*m_Feet)[0]->getFloat("MAINPLXE") * mm;
   yPosVertexStandFeet[2] = (*m_Feet)[0]->getFloat("MAINPLYE") * mm;  
   xPosVertexStandFeet[3] = (*m_Feet)[0]->getFloat("MAINPLXF") * mm;
   yPosVertexStandFeet[3] = (*m_Feet)[0]->getFloat("MAINPLYF") * mm;
   xPosVertexStandFeet[4] = (*m_Feet)[0]->getFloat("MAINPLXG") * mm;
   yPosVertexStandFeet[4] = (*m_Feet)[0]->getFloat("MAINPLYG") * mm;
   xPosVertexStandFeet[5] = (*m_Feet)[0]->getFloat("MAINPLXH") * mm;
   yPosVertexStandFeet[5] = (*m_Feet)[0]->getFloat("MAINPLYH") * mm;
   xPosVertexStandFeet[6] = (*m_Feet)[0]->getFloat("MAINPLXI") * mm;
   yPosVertexStandFeet[6] = (*m_Feet)[0]->getFloat("MAINPLYI") * mm;
   xPosVertexStandFeet[7] = (*m_Feet)[0]->getFloat("MAINPLXJ") * mm;
   yPosVertexStandFeet[7] = (*m_Feet)[0]->getFloat("MAINPLYJ") * mm;
   xPosVertexStandFeet[8] = (*m_Feet)[0]->getFloat("MAINPLXA") * mm;
   yPosVertexStandFeet[8] = (*m_Feet)[0]->getFloat("MAINPLYA") * mm;

   const double xPosVertexC1StandFeet	     = (*m_Feet)[0]->getFloat("MAIPLXC1") * mm;
   const double yPosVertexC1StandFeet	     = (*m_Feet)[0]->getFloat("MAIPLYC1") * mm;
   const double xPosFoot		     = (*m_Feet)[0]->getFloat("STDFOOXP") * mm;
   const double yPosFoot		     = (*m_Feet)[0]->getFloat("STDFOOYP") * mm;
   const double xSecCentreCutoutStandFeet    = (*m_Feet)[0]->getFloat("MPLXEXTR") * mm;
   const double ySecCentreCutoutStandFeet    = (*m_Feet)[0]->getFloat("MPLYEXTR") * mm;
   const double SecradiusCutout   	     = (*m_Feet)[0]->getFloat("MPLREXTR") * mm;
   const double HeightCutCom	  	     = (*m_Feet)[0]->getFloat("MPLAHCEC") * mm;
   const double xCentreCutoutStandFeet       = (*m_Feet)[0]->getFloat("MAPLRCXC") * mm;
   const double yCentreCutoutStandFeet       = (*m_Feet)[0]->getFloat("MAPLRCYC") * mm;
   const double radiusCutout    	     = (*m_Feet)[0]->getFloat("MNPLRCRA") * mm;
   const double phiMinCutout    	     =  135.   * deg; // fstd PHMINCUT
   const double phiMaxCutout     	     =  292.5  * deg; // fstd PHMAXCUT
   const double yMinLargePlate   	     = (*m_Feet)[0]->getFloat("VCNPLDYE") * mm;  
   const double yHeightLargePlate 	     = (*m_Feet)[0]->getFloat("VCNPLYHE") * mm;  
   const double xPosLargePlateStandFeet      = (*m_Feet)[0]->getFloat("VECPXPOS") * mm;
   const double thicknessInnerPlate	     = (*m_Feet)[0]->getFloat("UCNPLXWI") * mm;
   const double zWidthBotP 		     = (*m_Feet)[0]->getFloat("GRNDPLZL") * mm;
   const double zWidthMiniP 		     = (*m_Feet)[0]->getFloat("MINCPLZL")* mm;
   const double yHeightMiniP		     = (*m_Feet)[0]->getFloat("MINCPLYH")* mm;
   const double xlengthMiniP		     = (*m_Feet)[0]->getFloat("MINCPLXW")* mm;
   const double xPosMiniConnPlateStandFeet   = (*m_Feet)[0]->getFloat("MICPXPOS") * mm;
   const double yPosMiniConnPlateStandFeet   = (*m_Feet)[0]->getFloat("MICPYPOS") * mm;
   const double zWidthOuterP  		     = (*m_Feet)[0]->getFloat("SLACPLZL") * mm;
   const double lengthOuterPlate   	     = (*m_Feet)[0]->getFloat("SLACPLXW") * mm;

   // std::cout<<" FeetToroidBuilderRDBn:: Oracle reading finished " <<std::endl;
//----------------------------------------------------------------------------//
  
  const double tolerance = 0.5*mm;
  
  for (int i = 0; i < nVertex; i++)
  {
  xPosVertexStandFeet[i] = xPosVertexStandFeet[i] + xPosFoot;
  yPosVertexStandFeet[i] = yPosVertexStandFeet[i] + yPosFoot;
  }

  // auxiliary arrays to store z, x positions, and rotation angles for all volumes to be replicated 
  const int nZPosMax = 2 * ( maxDim - 2 );
  const int nXPosMax = 2;
  double zPosAux[ nZPosMax * nXPosMax ],  xPosAux[ nZPosMax * nXPosMax ],  
     rotAngleAux[ nZPosMax * nXPosMax ];
  
  //------------------------------------------------------------------------------------------
  // Standard Feet properties
      
  const double xPosMiniP    = xPosMiniConnPlateStandFeet + xPosFoot;
  const double yPosMiniP    = yPosMiniConnPlateStandFeet + yPosFoot;
  
  const double yMinLargeP   = yMinLargePlate + yPosFoot;
  
  const double xPosVertexC1 = xPosVertexC1StandFeet + xPosFoot;
  const double yPosVertexC1 = yPosVertexC1StandFeet + yPosFoot;
  
  // polygonal shape of side-plate envelope
  const double* xPosVertex = xPosVertexStandFeet;         // x,y positions of vertices
  const double* yPosVertex = yPosVertexStandFeet; 
  
  // cutout circle (segment) for Barrel Toroid coils .  
  const double xPosCentreCutout    = xCentreCutoutStandFeet + xPosFoot;  // x-position of centre
  const double yPosCentreCutout    = yCentreCutoutStandFeet + yPosFoot;  // y-position of centre 
  const double xPosSecCentreCutout = xSecCentreCutoutStandFeet + xPosFoot; // x-position of centre second circle for cutout
  const double yPosSecCentreCutout = ySecCentreCutoutStandFeet + yPosFoot; // y-position of centre second circle for cutout   
  
// large vertical plate (between the two side plates) 
  const double xMeanLargePlate     = xPosLargePlateStandFeet + xPosFoot; // x position of centre

//y-position of small inner vertical plate (on top; where Voussoirs are mounted)
  const double yPosInnerPlateStd   = 0.5 * ( yPosVertex[4] + yPosVertex[5] );

//------------------------------------------------------------------------------------------
//  Build side plates 
//  - the base shape is a union of three trapezoids, from which a protruding edge 
//    (by means of a box), and the space for the Barrel Toroid coil (by means of a two tubes 
//    and two boxes) are cut out  
//------------------------------------------------------------------------------------------

  double pDy = thicknessSidePlate/2;

//------------------------------------------------------------------------------------------
// base shape
//------------------------------------------------------------------------------------------
// first (base) trapezoid (vertices 1,2,8,9, counter-clockwise) 
// - note that axes don't coincide with global coordinates
  double pDzTrap1    = 0.5 * ( xPosVertex[0] - xPosVertex[8] );
  double pDx1Trap1   = 0.5 * ( yPosVertex[7] - yPosVertex[8] );
  double pDx3Trap1   = 0.5 * ( yPosVertex[1] - yPosVertex[0] ); 
  double pThetaTrap1 = atan( 0.5 * ( pDx3Trap1 - pDx1Trap1 ) / pDzTrap1 );

  GeoTrap* sTrap1 = new GeoTrap( pDzTrap1, pThetaTrap1, 0,
                                 pDy, pDx1Trap1 - tolerance, pDx1Trap1 - tolerance, 0,
                                 pDy, pDx3Trap1 - tolerance, pDx3Trap1 - tolerance, 0 );
  
// positioning (not the final one)
  HepTransform3D trlTrap1 = HepTranslate3D( yPosVertex[8] + 0.5 * ( pDx1Trap1 + pDx3Trap1 ), 0,
                                            xPosVertex[8] + pDzTrap1 );

//------------------------------------------------------------------------------------------
// second (middle) trapezoid: the vertices are 2,3; the intersection point of the
// lines through (3,6), (7,8); and 8 (counter-clockwise) 
// - note that axes don't coincide with global coordinates
  double dx12 = fabs( xPosVertex[1] - xPosVertex[2] );
  double dy12 = fabs( yPosVertex[1] - yPosVertex[2] );
  double beta = atan( dx12/dy12 );  // inclination angle (~22.5 deg)

  double dx17      = fabs( xPosVertex[1] - xPosVertex[7] );
  double dy17      = fabs( yPosVertex[1] - yPosVertex[7] );
  double aux       = dy17 + dx17 * tan(beta);
  double cosBeta   = cos(beta);
  double sinBeta   = sin(beta);
  double pDzTrap2  = 0.5 * ( dx17/cosBeta - aux * sinBeta );
  double pDx3Trap2 = 0.5 * dy12/cosBeta;
 
  double dx25      = fabs( xPosVertex[2] - xPosVertex[5] );
  double dy25      = fabs( yPosVertex[2] - yPosVertex[5] );
  double gamma1    = atan( dy25/dx25 );
  double gamma2    = atan( dy17/dx17 );
  double pDx1Trap2 = pDx3Trap2 - pDzTrap2 * ( tan( gamma2 + beta ) + tan( gamma1 - beta ) ); 

  double tanTheta    = 0.5 * ( pDx1Trap2 - pDx3Trap2 )/pDzTrap2 + tan( gamma1 - beta );
  double pThetaTrap2 = atan(tanTheta);

  GeoTrap* sTrap2 = new GeoTrap( pDzTrap2, pThetaTrap2, 0,
                                 pDy, pDx1Trap2 - tolerance, pDx1Trap2 - tolerance, 0,
                                 pDy, pDx3Trap2 - tolerance, pDx3Trap2 - tolerance, 0 );
  
// position/orientation relative to first trapezoid
  double pDxMeanTrap2  = 0.5 * ( pDx1Trap2 + pDx3Trap2 ); 
  HepTransform3D trlTrap2 = HepTranslate3D( yPosVertex[1] + dy17/2 + pDxMeanTrap2 * cosBeta, 0,
                                            xPosVertex[1] - dx17/2 - pDxMeanTrap2 * sinBeta );
  trlTrap2 = trlTrap2 * trlTrap1.inverse();
  HepTransform3D xfTrap2 = trlTrap2 * HepRotateY3D(beta);
  
//----------------------------------------------------------------------------------------
// third (top) trapezoid (vertices 3,4,5,6, counter-clockwise)
// - note that axes don't coincide with global coordinates
  double pDzTrap3    = 0.5 * ( xPosVertex[3] - xPosVertex[4] );
  double pDx1Trap3   = 0.5 * ( yPosVertex[4] - yPosVertex[5] );
  double pDx3Trap3   = 0.5 * ( yPosVertex[3] - yPosVertex[2] ); 
  double pThetaTrap3 = atan( 0.5 * ( pDx1Trap3 - pDx3Trap3 )/pDzTrap3 );

  GeoTrap* sTrap3 = new GeoTrap( pDzTrap3, pThetaTrap3, 0,
                                 pDy, pDx1Trap3 - tolerance, pDx1Trap3 - tolerance, 0,
                                 pDy, pDx3Trap3 - tolerance, pDx3Trap3 - tolerance, 0 );

// position relative to first trapezoid
  HepTransform3D xfTrap3 = HepTranslate3D( yPosVertex[4] - 0.5 * ( pDx1Trap3 + pDx3Trap3 ), 0,
                                           xPosVertex[4] + pDzTrap3 );
  xfTrap3 = xfTrap3 * trlTrap1.inverse();

// MiniConn Plates
  //ss 12/02/2007 never used GeoBox* sMiniConnPlate = new GeoBox( yHeightMiniP/2 - tolerance, zWidthMiniP/2, xlengthMiniP/2 );
//empty MiniConn plates
  GeoBox* sEmptyMiniConnPlate = new GeoBox( yHeightMiniP/2, zWidthMiniP/4, xlengthMiniP/2
					    + tolerance );
  
  HepTransform3D trlMiniConnPlate = HepTranslate3D( yPosMiniP, 0, xPosMiniP );
  trlMiniConnPlate = trlMiniConnPlate * trlTrap1.inverse();
  HepTransform3D xfMiniConnPlate = trlMiniConnPlate * HepRotateY3D(beta);

//------------------------------------------------------------------------------------------
// shapes to be cut out
//------------------------------------------------------------------------------------------
// box to cut out the protruding edge of the middle trapezoid (uses vertices 6,7)
  double dx56   = fabs( xPosVertex[5] - xPosVertex[6] );
  double dy56   = fabs( yPosVertex[5] - yPosVertex[6] );
  double dxBox1 = 2 * sqrt( dx56 * dx56 + dy56 * dy56 ); // some 'large enough' value
  double dzBox1 = pDzTrap2;                              // some 'large enough' value
  
  GeoBox* sEmptyBox1 = new GeoBox( dxBox1, 2 * pDy, dzBox1 );    
  
// position/orientation relative to first trapezoid
  double alpha1 = atan( dx56/dy56 );    // inclination angle of cutout box
  HepTransform3D trlEmptyBox1 = HepTranslate3D( yPosVertex[5] - dzBox1 * sin(alpha1), 0, 
                                                xPosVertex[5] - dzBox1 * cos(alpha1) );
  trlEmptyBox1 = trlEmptyBox1 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox1 = trlEmptyBox1 * HepRotateY3D(alpha1);

  
//------------------------------------------------------------------------------------------
// tube to cut out the space for the Barrel Toroid coils
  GeoTube* sEmptyTube1 = new GeoTube(0, radiusCutout, 2 * pDy );

// position/orientation relative to first trapezoid
  HepTransform3D trlEmptyTube1 = HepTranslate3D( yPosCentreCutout, 0, xPosCentreCutout );
  trlEmptyTube1 = trlEmptyTube1 * trlTrap1.inverse();                                           
  HepTransform3D xfEmptyTube1 = trlEmptyTube1 * HepRotateX3D( M_PI/2 ) * HepRotateY3D( M_PI );
  
//-------------------------------------------------------------------------------------------
// tube to Second cut out the space for the Barrel Toroid coil (2) ---?
  GeoTube* sEmptyTube2 = new GeoTube(0, SecradiusCutout, 2 * pDy );
  
// position/orientation relative to fist trapezoid
  HepTransform3D trlEmptyTube2 = HepTranslate3D( yPosSecCentreCutout, 0, xPosSecCentreCutout );
  trlEmptyTube2 = trlEmptyTube2 * trlTrap1.inverse();
  HepTransform3D xfEmptyTube2 = trlEmptyTube2 * HepRotateX3D( M_PI/2 ) * HepRotateY3D( M_PI );
  
  
//------------------------------------------------------------------------------------------
// boxes to cut out the space for the Barrel Toroid coils
//  some 'large enough' values
  GeoBox* sEmptyBox2 = new GeoBox( SecradiusCutout, 2 * pDy, pDzTrap2 ); 
  
// position/orientation relative to first trapezoid
  double alpha2 = phiMaxCutout - 3 * M_PI/2;
  HepTransform3D trlEmptyBox2 = HepTranslate3D( yPosSecCentreCutout + pDzTrap2 * sin(alpha2), 0, 
                                                xPosSecCentreCutout + pDzTrap2 * cos(alpha2) );
  trlEmptyBox2 = trlEmptyBox2 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox2 = trlEmptyBox2 * HepRotateY3D(alpha2);

// some 'large enough' values  
  GeoBox* sEmptyBox3 = new GeoBox( radiusCutout/2, 2 * pDy, pDzTrap2 );  
  double alpha3 = phiMinCutout - M_PI/2;
  HepTransform3D trlEmptyBox3 = HepTranslate3D( HepRotateY3D(alpha3) * 
                                HepPoint3D( radiusCutout/2, 0, pDzTrap2 ) ) * 
                                HepTranslate3D( yPosCentreCutout, 0, xPosCentreCutout );
  trlEmptyBox3 = trlEmptyBox3 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox3 = trlEmptyBox3 * HepRotateY3D(alpha3);

  GeoBox* sEmptyBox4 = new GeoBox( HeightCutCom/2, 2 * pDy, 
                                   (xPosSecCentreCutout - xPosCentreCutout)/2 );
  HepTransform3D trlEmptyBox4 = HepTranslate3D( yPosSecCentreCutout - 
                                SecradiusCutout + HeightCutCom/2, 0,
				(xPosSecCentreCutout + xPosCentreCutout)/2 );
  
  trlEmptyBox4 = trlEmptyBox4 * trlTrap1.inverse();
  
//------------------------------------------------------------------------------------------
// unite the three trapezoids to obtain the side-plate base shape, cut out the 
// tube and various boxes
//------------------------------------------------------------------------------------------
  const GeoShape& sSidePlate = sTrap1->add( (*sTrap2) << xfTrap2 ).
                                       subtract( (*sEmptyBox1) << xfEmptyBox1 ).
                                       subtract( (*sEmptyBox2) << xfEmptyBox2 ).
                                       subtract( (*sEmptyBox3) << xfEmptyBox3 ).
                                       subtract( (*sEmptyTube1) << xfEmptyTube1 ).
				       subtract( (*sEmptyTube2) << xfEmptyTube2 ).
				       subtract( (*sEmptyBox4) << trlEmptyBox4 ).
				       subtract( (*sEmptyMiniConnPlate) << xfMiniConnPlate ).
//  				       add( (*sMiniConnPlate) << xfMiniConnPlate ).
                                       add( (*sTrap3) << xfTrap3 );
  
  GeoLogVol*  lSidePlate = new GeoLogVol( "StdFeetSidePlate", 
                                          &sSidePlate, 
                                          theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pSidePlate = new GeoPhysVol(lSidePlate);

//------------------------------------------------------------------------------------------
// replicate and position
//------------------------------------------------------------------------------------------
// array of z positions
  const int nZPosSidePlate = nZPosMax;
  double zPosSidePlate[nZPosSidePlate];
  for ( int i = 1; i < nFeet - 1; i++ ) 
  {  
     zPosSidePlate[ 2 * i - 2 ] = zPositionFeet[i] - zWidth/2;
     zPosSidePlate[ 2 * i - 1 ] = zPositionFeet[i] + zWidth/2;
// DHW     zPosSidePlate[ 2 * i - 2 ] = zPositionFeet[i] - zWidth/2 + thicknessSidePlate/2;    
//         zPosSidePlate[ 2 * i - 1 ] = zPositionFeet[i] + zWidth/2 - thicknessSidePlate/2;    
  }

// array of x positions
  const int nXPos = nXPosMax;
// final x-position of first trapezoid centre
  double xMeanSidePlate = 0.5 * ( xPosVertex[0] + xPosVertex[8] );    
  double xPosSidePlate[nXPos] = { -xMeanSidePlate, xMeanSidePlate };

// array of rotation angles (to flip shapes around x-axis)
  double rotAngleSidePlate[nXPos] = { M_PI, 0 };
    
// fill auxiliary arrays 
  for ( int i = 0; i < nZPosSidePlate * nXPos; i++ )  
  {
     int ii = i % nZPosSidePlate, 
         jj = i / nZPosSidePlate;
     zPosAux[i]     = zPosSidePlate[ii];
     xPosAux[i]     = xPosSidePlate[jj];
     rotAngleAux[i] = rotAngleSidePlate[jj]; 
  }

  GENFUNCTION fRotSidePlate  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosSidePlate * nXPos ); 
  GENFUNCTION fTrlZSidePlate = ArrayFunction( zPosAux,     zPosAux     + nZPosSidePlate * nXPos ); 
  GENFUNCTION fTrlXSidePlate = ArrayFunction( xPosAux,     xPosAux     + nZPosSidePlate * nXPos ); 
  
// final y-pos. of first trapezoid centre
  double yMean = yPosVertex[8] + 0.5 * ( pDx1Trap1 + pDx3Trap1 );            
  TRANSFUNCTION XFSidePlate = Pow( HepTranslateZ3D(1.0), fTrlZSidePlate ) * 
                              Pow( HepTranslateX3D(1.0), fTrlXSidePlate ) * 
                              HepTranslateY3D(yMean) *     // const. displacement along y
                              HepRotateX3D( M_PI/2 ) *     // const. rotations to bring shape into...
                              HepRotateY3D( M_PI/2 ) *     // ...correct orientation  
                              Pow( HepRotateX3D(1.0), fRotSidePlate ); 

  GeoSerialTransformer* sxSidePlate = new GeoSerialTransformer( pSidePlate, &XFSidePlate, 
                                                                nZPosSidePlate * nXPos );
  container->add(sxSidePlate);

//------------------------------------------------------------------------------------------
//  Build large vertical plates, located between the side-plate pairs
//------------------------------------------------------------------------------------------

  
  double dyLargePlate = 0.5 * yHeightLargePlate;
  double dzLargePlate = zWidth/2 - thicknessSidePlate;
  
  GeoBox* sLargePlate = new GeoBox( thicknessLargePlate/2, dyLargePlate, dzLargePlate );
  GeoLogVol*  lLargePlate = new GeoLogVol( "StdFeetLargePlate", 
                                           sLargePlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pLargePlate = new GeoPhysVol(lLargePlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of z positions (will be used later also for inner vertical, cover, and bottom plates)
  const int nZPosPlate = maxDim - 2;
  double zPosPlate[nZPosPlate];
  for ( int i = 0; i < nZPosPlate; i++ )  zPosPlate[i] = zPositionFeet[ i + 1 ]; 

// array of x positions
  double xPosLargePlate[nXPos] = { -xMeanLargePlate, xMeanLargePlate };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosLargePlate[jj];
  }

  GENFUNCTION fTrlZLargePlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXLargePlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  double yPosLargePlate = yMinLargeP + dyLargePlate;
  TRANSFUNCTION XFLargePlate = Pow( HepTranslateZ3D(1.0), fTrlZLargePlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXLargePlate ) * 
                               HepTranslateY3D(yPosLargePlate); 

  GeoSerialTransformer* sxLargePlate = new GeoSerialTransformer( pLargePlate, &XFLargePlate, 
                                                                 nZPosPlate * nXPos );
  container->add(sxLargePlate);

//------------------------------------------------------------------------------------------
//  Build outer inclined (cover) plates, which cover the Barrel Toroid coil cutouts
//------------------------------------------------------------------------------------------
  
  GeoBox* sCoverPlate = new GeoBox( thicknessOuterPlate/2 - tolerance, lengthOuterPlate/2, 
                                    zWidthOuterP/2 );
  GeoLogVol*  lCoverPlate = new GeoLogVol( "StdFeetCoverPlate", 
                                           sCoverPlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pCoverPlate = new GeoPhysVol(lCoverPlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of x positions
  double DIAG = sqrt ( thicknessOuterPlate * thicknessOuterPlate + lengthOuterPlate * 
                       lengthOuterPlate );
  double alphaCP = atan(thicknessOuterPlate/lengthOuterPlate);
  double betaCP  = atan(dy12/dx12);
  
  double xMeanCoverPlate = xPosVertexC1 + (yHeightMiniP * sinBeta) - 
                                                            (DIAG * cos(alphaCP + betaCP))/2;
  double xPosCoverPlate[nXPos] = { -xMeanCoverPlate, xMeanCoverPlate };

// array of inclination angles (different for left and right cover plates)
  double rotAngleCoverPlate[nXPos] = { -beta, beta };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i]     = zPosPlate[ii];
     xPosAux[i]     = xPosCoverPlate[jj];
     rotAngleAux[i] = rotAngleCoverPlate[jj]; 
  }

  GENFUNCTION fRotCoverPlate  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlZCoverPlate = ArrayFunction( zPosAux,     zPosAux     + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXCoverPlate = ArrayFunction( xPosAux,     xPosAux     + nZPosPlate * nXPos ); 
  
  double yPosCoverPlate = yPosVertexC1 + (DIAG * sin(betaCP + alphaCP))/2 - (yHeightMiniP * cosBeta);
  TRANSFUNCTION XFCoverPlate = Pow( HepTranslateZ3D(1.0), fTrlZCoverPlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXCoverPlate ) * 
                               HepTranslateY3D(yPosCoverPlate) * 
                               Pow( HepRotateZ3D(1.0), fRotCoverPlate ); 

  GeoSerialTransformer* sxCoverPlate = new GeoSerialTransformer( pCoverPlate, &XFCoverPlate, 
                                                                 nZPosPlate * nXPos );
  container->add(sxCoverPlate);

//------------------------------------------------------------------------------------------
//  Build inner (top) vertical plates, on which the Feet Voussoirs are mounted
//------------------------------------------------------------------------------------------
  
  GeoBox* sInnerPlate = new GeoBox( thicknessInnerPlate/2 - tolerance, pDx1Trap3, zWidth/2 );

  GeoLogVol*  lInnerPlate = new GeoLogVol( "StdFeetInnerPlate", 
                                           sInnerPlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pInnerPlate = new GeoPhysVol(lInnerPlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of x positions
  double xPosInnerPlate[nXPos] = { -xPosVertex[4] + thicknessInnerPlate/2, 
                                    xPosVertex[4] - thicknessInnerPlate/2 };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosInnerPlate[jj];
  }

  GENFUNCTION fTrlZInnerPlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXInnerPlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  TRANSFUNCTION XFInnerPlate = Pow( HepTranslateZ3D(1.0), fTrlZInnerPlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXInnerPlate ) * 
                               HepTranslateY3D(yPosInnerPlateStd); 

  GeoSerialTransformer* sxInnerPlate = new GeoSerialTransformer( pInnerPlate, &XFInnerPlate, 
                                                                 nZPosPlate * nXPos );
  container->add(sxInnerPlate);

//------------------------------------------------------------------------------------------
//  Build bottom plates
//------------------------------------------------------------------------------------------
  
  GeoBox* sBottomPlate = new GeoBox( pDzTrap1, thicknessBottomPlate/2 - tolerance, zWidthBotP/2 );
  GeoLogVol*  lBottomPlate = new GeoLogVol( "StdFeetBottomPlate", 
                                            sBottomPlate, 
                                            theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pBottomPlate = new GeoPhysVol(lBottomPlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of x positions
  double xPosBottomPlate[nXPos] = { -0.5 * ( xPosVertex[0] + xPosVertex[8] ), 
                                     0.5 * ( xPosVertex[0] + xPosVertex[8] ) };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosBottomPlate[jj];
  }

  GENFUNCTION fTrlZBottomPlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXBottomPlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  double yPosBottomPlate = yPosVertex[0] - thicknessBottomPlate/2;
  TRANSFUNCTION XFBottomPlate = Pow( HepTranslateZ3D(1.0), fTrlZBottomPlate ) * 
                                Pow( HepTranslateX3D(1.0), fTrlXBottomPlate ) * 
                                HepTranslateY3D(yPosBottomPlate); 

  GeoSerialTransformer* sxBottomPlate = new GeoSerialTransformer( pBottomPlate, &XFBottomPlate, 
                                        nZPosPlate * nXPos );
  container->add(sxBottomPlate);

}

void FeetToroidBuilderRDB::buildExtremityFeet( GeoPhysVol* container ) 
{
//------------------------------------------------------------------------------------------
//  Builds Extremity Feet (i.e., the two outermost feet pairs) -- sideplates, bottom plate, 
//  inner and top vertical plates  
//------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int maxDimExtr = 12; 
  const int nVertexExtr= 12;

//------------ Feet General Parameters ---------------------------------//
   double zPositionFeet[maxDim];
   zPositionFeet[0] = -(*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
   zPositionFeet[1] = -(*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[2] = -(*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[3] = -(*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[4] =  (*m_Feet)[0]->getFloat("ZPOSFEE1") * mm;
   zPositionFeet[5] =  (*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[6] =  (*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[7] =  (*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[8] =  (*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
                                                                              //
   const double thicknessSidePlate   = (*m_Feet)[0]->getFloat("MAINPLDZ") * mm;               //
   const double thicknessBottomPlate = (*m_Feet)[0]->getFloat("MNPLPEHE") * mm;               //
   const double thicknessLargePlate  = (*m_Feet)[0]->getFloat("EXVCPTHI") * mm;               //
   const double thicknessOuterPlate  = (*m_Feet)[0]->getFloat("SLACPLYH") * mm;               //
                                                                                 //
   const std::string feetMaterial = getMaterial( "Iron" );
//------------ Extremity Feet Parameters -------------------------------//
   const double zWidth = (*m_Feet)[0]->getFloat("EXMPZSEP") * mm;
 
   double xPosVertexExtrFeet[maxDimExtr], yPosVertexExtrFeet[maxDimExtr];

	xPosVertexExtrFeet[0] = (*m_Feet)[0]->getFloat("MAINPLXB") * mm;
	yPosVertexExtrFeet[0] = (*m_Feet)[0]->getFloat("MAINPLYB") * mm;
	xPosVertexExtrFeet[1] = (*m_Feet)[0]->getFloat("MAINPLXC") * mm;
	yPosVertexExtrFeet[1] = (*m_Feet)[0]->getFloat("MAINPLYC") * mm;
	xPosVertexExtrFeet[2] = (*m_Feet)[0]->getFloat("MAINPLXE") * mm;
	yPosVertexExtrFeet[2] = (*m_Feet)[0]->getFloat("MAINPLYE") * mm;
	xPosVertexExtrFeet[3] = (*m_Feet)[0]->getFloat("EXMPLAXF") * mm;
	yPosVertexExtrFeet[3] = (*m_Feet)[0]->getFloat("EXMPLAYF") * mm;
	xPosVertexExtrFeet[4] = (*m_Feet)[0]->getFloat("EXMPLXF1") * mm; // ?
	yPosVertexExtrFeet[4] = (*m_Feet)[0]->getFloat("EXMPLYF1") * mm;
	xPosVertexExtrFeet[5] = (*m_Feet)[0]->getFloat("EXMPLXF2") * mm; // ?
	yPosVertexExtrFeet[5] = (*m_Feet)[0]->getFloat("EXMPLYF2") * mm;
	xPosVertexExtrFeet[6] = (*m_Feet)[0]->getFloat("EXMPLXF6") * mm;
	yPosVertexExtrFeet[6] = (*m_Feet)[0]->getFloat("EXMPLYF6") * mm;
	xPosVertexExtrFeet[7] = (*m_Feet)[0]->getFloat("EXMPLXF7") * mm;
	yPosVertexExtrFeet[7] = (*m_Feet)[0]->getFloat("EXMPLYF7") * mm;
	xPosVertexExtrFeet[8] = (*m_Feet)[0]->getFloat("EXMPLAXH") * mm;
	yPosVertexExtrFeet[8] = (*m_Feet)[0]->getFloat("EXMPLAYH") * mm;
	xPosVertexExtrFeet[9] = (*m_Feet)[0]->getFloat("EXMPLAXI") * mm;
	yPosVertexExtrFeet[9] = -812.6 * cm;
	xPosVertexExtrFeet[10]= (*m_Feet)[0]->getFloat("MAINPLXJ")* mm;
	yPosVertexExtrFeet[10]= (*m_Feet)[0]->getFloat("MAINPLYJ")* mm;
	xPosVertexExtrFeet[11]= (*m_Feet)[0]->getFloat("MAINPLXA")* mm;
	yPosVertexExtrFeet[11]= (*m_Feet)[0]->getFloat("MAINPLYA")* mm;
        
   const double xPosFoot 	      = (*m_Feet)[0]->getFloat("STDFOOXP") * mm;
   const double yPosFoot	      = (*m_Feet)[0]->getFloat("STDFOOYP") * mm;	
   const double xPosVertexC1ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXC1") * mm;
   const double yPosVertexC1ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYC1") * mm;
   const double xPosVertexF3ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXF3") * mm;
   const double yPosVertexF3ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYF3") * mm;
   const double xPosVertexF4ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXF4") * mm;
   const double yPosVertexF4ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYF4") * mm;
   const double xPosVertexF5ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXF5") * mm;
   const double yPosVertexF5ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYF5") * mm;
   const double xPosVertexDExtrFeet   = (*m_Feet)[0]->getFloat("EXMPLAXD") * mm;
   const double yPosVertexDExtrFeet   = (*m_Feet)[0]->getFloat("EXMPLAYD") * mm;
   const double xPosVertexC3ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXC3") * mm;
   const double yPosVertexC3ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYC3") * mm;
   const double xPosVertexC4ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXC4") * mm;
   const double yPosVertexC4ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYC4") * mm;
   const double xPosVertexC5ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLXC5") * mm;
   const double yPosVertexC5ExtrFeet  = (*m_Feet)[0]->getFloat("EXMPLYC5") * mm;
   const double xPosVertexC1aExtrFeet = (*m_Feet)[0]->getFloat("EXMPXC1A") * mm;
   const double yPosVertexC1aExtrFeet = (*m_Feet)[0]->getFloat("EXMPYC1A") * mm;
   const double xPosVertexC1bExtrFeet = (*m_Feet)[0]->getFloat("EXMPXC1B") * mm;
   //const double yPosVertexC1bExtrFeet = (*m_Feet)[0]->getFloat("EXMPYC1B") * mm;
   
   const double SecradiusCutExtr      = (*m_Feet)[0]->getFloat("EXMPRCRA") * mm;
   //const double xPosSecCentreCutout   = (*m_Feet)[0]->getFloat("EXMPRCXC") * mm;
   //const double yPosSecCentreCutout   = (*m_Feet)[0]->getFloat("EXMPRCYC") * mm;
   
   const double xCentreCutoutExtrFeet = (*m_Feet)[0]->getFloat("MAPLRCXC") * mm;
   const double yCentreCutoutExtrFeet = (*m_Feet)[0]->getFloat("MAPLRCYC") * mm;
   const double radiusCutout          = (*m_Feet)[0]->getFloat("MNPLRCRA") * mm;
   const double yMinLargePlateExtr    = (*m_Feet)[0]->getFloat("EXVCPLAY") * mm; //-988.5
   const double yMaxLargePlate        = -840.5 * cm; 
   const double xPosLargePlateExtrFeet= (*m_Feet)[0]->getFloat("EXVCPLAX") * mm;
   const double lengthInclinedPlate   =  74.21 * cm; //????
//   const double angleInclinedPlateExtrFeet  = ANGIPL;
   const double thicknessInnerPlate   = (*m_Feet)[0]->getFloat("UCNPLXWI")* mm;
   const double yHeightInnerPlateExtr = (*m_Feet)[0]->getFloat("UCNPLYHE")* mm;
   const double yPosCentreInnerTopPlateExtrFeet = (*m_Feet)[0]->getFloat("UPCPYPOS") * mm;
   const double lengthOuterPlate      = (*m_Feet)[0]->getFloat("SLACPLXW") * mm;
   const double yoffsetCutCoverPlateExtr        = (*m_Feet)[0]->getFloat("EXSPRCOF") * mm;
   const double radiusCutCoverPlateExtr         = (*m_Feet)[0]->getFloat("EXSPRCRA") * mm;	 
   const double zWidthMiniPExtr       = (*m_Feet)[0]->getFloat("EXMCPZLE") * mm;
   const double yHeightMiniPExtr      = (*m_Feet)[0]->getFloat("EXMCPYHE") * mm; 
   const double xlengthMiniPExtr      = (*m_Feet)[0]->getFloat("MINCPLXW") * mm;
   const double xPosMiniConnPlateExtrFeet       = (*m_Feet)[0]->getFloat("EXMCPXPO") * mm;
   const double yPosMiniConnPlateExtrFeet       = (*m_Feet)[0]->getFloat("EXMCPYPO") * mm;
   const double zWidthBotExtr	      = (*m_Feet)[0]->getFloat("EXGPZLEN") * mm;		
   const double zWidthOuterPExtr      = (*m_Feet)[0]->getFloat("EXSCPZLE") * mm;
   const double yHeightCutInclinedPlate         = (*m_Feet)[0]->getFloat("EXVCPYCU") * mm;
   const double zLengthCutInclinedPlate         = (*m_Feet)[0]->getFloat("EXVCPXCU") * mm;
   const double xWidthTopPlateExtr    = (*m_Feet)[0]->getFloat("EXCFVXWI") * mm;
   const double yHeightTopPlateExtr   = (*m_Feet)[0]->getFloat("EXCFVYHE") * mm;
   const double zLengthTopPlateExtr   = (*m_Feet)[0]->getFloat("EXCFVZLE") * mm;
//----------------------------------------------------------------------------//
  

  const double tolerance = 0.5*mm;

  // auxiliary arrays to store z, x positions, and rotation angles for all volumes to be replicated 
  const int nZPosMax = 4;
  const int nXPosMax = 2;
  double zPosAux[ nZPosMax * nXPosMax ],  xPosAux[ nZPosMax * nXPosMax ],  rotAngleAux[ nZPosMax * nXPosMax ];
  
  //------------------------------------------------------------------------------------------
  // Feet general properties
  
  for (int i = 0; i < nVertexExtr; i++)
  {
  xPosVertexExtrFeet[i] = xPosVertexExtrFeet[i] + xPosFoot;
  yPosVertexExtrFeet[i] = yPosVertexExtrFeet[i] + yPosFoot;
  }
  yPosVertexExtrFeet[9] = yPosVertexExtrFeet[9] - yPosFoot;
    
  const double yPosCentrInPlateExtr = yPosCentreInnerTopPlateExtrFeet + yPosFoot;
  
  //------------------------------------------------------------------------------------------
  // Extremity Feet properties  
  const double xPosMiniPExtr   = xPosMiniConnPlateExtrFeet + xPosFoot;
  const double yPosMiniPExtr   = yPosMiniConnPlateExtrFeet + yPosFoot;
  
  const double xPosVertC1Extr = xPosVertexC1ExtrFeet + xPosFoot;
  const double yPosVertC1Extr = yPosVertexC1ExtrFeet + yPosFoot;
  const double xPosVertF3Extr = xPosVertexF3ExtrFeet + xPosFoot;
  const double yPosVertF3Extr = yPosVertexF3ExtrFeet + yPosFoot;
  const double xPosVertF4Extr = xPosVertexF4ExtrFeet + xPosFoot;
  const double yPosVertF4Extr = yPosVertexF4ExtrFeet + yPosFoot;
  const double xPosVertF5Extr = xPosVertexF5ExtrFeet + xPosFoot;
  const double yPosVertF5Extr = yPosVertexF5ExtrFeet + yPosFoot;
  const double xPosVertDExtr  = xPosVertexDExtrFeet + xPosFoot;
  const double yPosVertDExtr  = yPosVertexDExtrFeet + yPosFoot;
  const double xPosVertC3Extr = xPosVertexC3ExtrFeet + xPosFoot;
  const double yPosVertC3Extr = yPosVertexC3ExtrFeet + yPosFoot;
  const double xPosVertC4Extr = xPosVertexC4ExtrFeet + xPosFoot;
  const double yPosVertC4Extr = yPosVertexC4ExtrFeet + yPosFoot;
  const double xPosVertC5Extr = xPosVertexC5ExtrFeet + xPosFoot;
  const double yPosVertC5Extr = yPosVertexC5ExtrFeet + yPosFoot;
  const double xPosVertC1aExtr= xPosVertexC1aExtrFeet + xPosFoot;
  const double yPosVertC1aExtr= yPosVertexC1aExtrFeet + yPosFoot;
  const double xPosVertC1bExtr= xPosVertexC1bExtrFeet + xPosFoot;
  //const double yPosVertC1bExtr= yPosVertexC1bExtrFeet + yPosFoot;
      
  // polygonal shape of side-plate envelope
  const double* xPosVertex = xPosVertexExtrFeet;         // x,y positions of vertices
  const double* yPosVertex = yPosVertexExtrFeet;
  
  // cutout circle (segment) for Barrel Toroid coils
  const double xPosCentreCutout = xCentreCutoutExtrFeet + xPosFoot;  // x-position of centre
  const double yPosCentreCutout = yCentreCutoutExtrFeet + yPosFoot;  // y-position of centre

  const double yPosSecCentreCut = yPosCentreCutout + yPosFoot;
  const double xPosSecCentreCut = xPosCentreCutout + xPosFoot;
  
  // large vertical plate (between the two side plates)
  const double yMinLargePlate  = yMinLargePlateExtr + yPosFoot; 
  const double xMeanLargePlate = (xPosLargePlateExtrFeet - thicknessLargePlate/2) + xPosFoot;  // x position of centre
  
  // large inclined plate (between the two side plates)
//  const double angleInclinedPlate  = angleInclinedPlateExtrFeet;  // inclination angle

//------------------------------------------------------------------------------------------
//  Build side plate: the base shape is a union of two trapezoids and two boxes, from 
//  which a protruding edge (by means of a box), and the space for the Barrel Toroid coil  
//  (by means of a tube and three boxes) are cut out  
//------------------------------------------------------------------------------------------

  double pDy = thicknessSidePlate/2;

//------------------------------------------------------------------------------------------
// base shape
//------------------------------------------------------------------------------------------
// first (base) trapezoid (vertices 1, 2, 11, 12, counter-clockwise) 
// - note that axes don't coincide with global coordinates
  double pDzTrap1    = 0.5 * ( xPosVertex[0]  - xPosVertex[11] );
  double pDx1Trap1   = 0.5 * ( yPosVertex[10] - yPosVertex[11] );
  double pDx3Trap1   = 0.5 * ( yPosVertex[1]  - yPosVertex[0] ); 
  double pThetaTrap1 = atan( 0.5 * ( pDx3Trap1 - pDx1Trap1 )/pDzTrap1 );

  GeoTrap* sTrap1 = new GeoTrap( pDzTrap1, pThetaTrap1, 0,
                                 pDy - tolerance, pDx1Trap1 - tolerance, pDx1Trap1 - tolerance, 0,
                                 pDy - tolerance, pDx3Trap1 - tolerance, pDx3Trap1 - tolerance, 0 );
  
  // positioning (not the final one)
  HepTransform3D trlTrap1 = HepTranslate3D( yPosVertex[11] + 0.5 * ( pDx1Trap1 + pDx3Trap1 ), 0,
                                            xPosVertex[11] + pDzTrap1 );

  
  //------------------------------------------------------------------------------------------
  // second (middle) trapezoid: the vertices are 2, 3; the intersection point of the
  // lines through (3,9), (10,11); and 11 (counter-clockwise) 
  // - note that axes don't coincide with global coordinates
  double dx12 = fabs( xPosVertex[1] - xPosVertex[2] );
  double dy12 = fabs( yPosVertex[2] - yPosVertex[1] );
  double beta = atan( dx12/dy12 );  // inclination angle (~22.5 deg)

  double dx110     = fabs( xPosVertex[1] - xPosVertex[10] );
  double dy110     = fabs( yPosVertex[1] - yPosVertex[10] );
  double aux       = dy110 + dx110 * tan(beta);
  double cosBeta   = cos(beta);
  double sinBeta   = sin(beta);
  double pDzTrap2  = 0.5 * ( dx110/cosBeta - aux * sinBeta );
  double pDx3Trap2 = 0.5 * dy12/cosBeta;

  double dx28      = fabs( xPosVertex[2] - xPosVertex[8] );
  double dy28      = fabs( yPosVertex[2] - yPosVertex[8] );
  double gamma1    = atan( dy28/dx28 );
  double gamma2    = atan( dy110/dx110 );
  double pDx1Trap2 = pDx3Trap2 - pDzTrap2 * ( tan( gamma2 + beta ) + tan( gamma1 - beta ) ); 

  double tanTheta    = 0.5 * ( pDx1Trap2 - pDx3Trap2 )/pDzTrap2 + tan( gamma1 - beta );
  double pThetaTrap2 = atan(tanTheta);

  GeoTrap* sTrap2 = new GeoTrap( pDzTrap2, pThetaTrap2, 0,
                                 pDy - tolerance, pDx1Trap2 - tolerance, pDx1Trap2 - tolerance, 0,
                                 pDy - tolerance, pDx3Trap2 - tolerance, pDx3Trap2 - tolerance, 0 );
  
  // position/orientation relative to first trapezoid
  double pDxMeanTrap2  = 0.5 * ( pDx1Trap2 + pDx3Trap2 ); 
  HepTransform3D trlTrap2 = HepTranslate3D( yPosVertex[1] + dy110/2 + pDxMeanTrap2 * cosBeta, 0,
                                            xPosVertex[1] - dx110/2 - pDxMeanTrap2 * sinBeta );
  trlTrap2 = trlTrap2 * trlTrap1.inverse();
  HepTransform3D xfTrap2 = trlTrap2 * HepRotateY3D(beta);

  
  //----------------------------------------------------------------------------------------
  // first top box (vertices 7, (x8,y3), (x4,y3), (x4,y7), counter-clockwise) 
  // - note that axes don't coincide with global coordinates
  double pDxTopBox1 = 0.5 * ( yPosVertex[6] - yPosVertex[2] );
  double pDzTopBox1 = 0.5 * ( xPosVertex[3] - xPosVertex[7] );

  GeoBox* sTopBox1 = new GeoBox( pDxTopBox1 - tolerance, pDy, pDzTopBox1 );
  
  // position/orientation relative to first trapezoid
  HepTransform3D xfTopBox1 = HepTranslate3D( yPosVertex[2] + pDxTopBox1, 0, xPosVertex[7] + pDzTopBox1 );
  xfTopBox1 = xfTopBox1 * trlTrap1.inverse();

  
  //----------------------------------------------------------------------------------------
  // second top box (vertices 4, 5, (x5,y6), (x4,y6), counter-clockwise) 
  // - note that axes don't coincide with global coordinates
  double pDxTopBox2  = 0.5 * ( yPosVertex[4] - yPosVertex[5] );
  double pDzTopBox2  = 0.5 * ( xPosVertex[3] - xPosVertex[4] );

  GeoBox* sTopBox2 = new GeoBox( pDxTopBox2 - tolerance, pDy, pDzTopBox2 );
  
  //position/orientation relative to first trapezoid
  HepTransform3D xfTopBox2 = HepTranslate3D( yPosVertex[5] + pDxTopBox2, 0, xPosVertex[5] + pDzTopBox2 );
  xfTopBox2 = xfTopBox2 * trlTrap1.inverse();
  
  //----------------------------------------------------------------------------------------------
  // third top (middle) box (vertices (x4,y6),(x4,y7),(xf3,y7),(xf3,y6) )
  // - note that axes don't coincide with global coordinates
  double pDzTopBox3  = 0.5 * ( xPosVertex[3] - xPosVertF3Extr );
  double pDxTopBox3  = 0.5 * ( yPosVertex[5] - yPosVertex[6] );
  
  GeoBox* sTopBox3 = new GeoBox( pDxTopBox3 - tolerance, pDy, pDzTopBox3 );
  
  // position/orientation relative to first trapezoid
  HepTransform3D xfTopBox3 = HepTranslate3D( yPosVertex[6] + pDxTopBox3, 0, xPosVertF3Extr + pDzTopBox3 );
  xfTopBox3 = xfTopBox3 * trlTrap1.inverse();  
  
  //-----------------------------------------------------------------------------------------------------
  // last top (bottom) box (vertices (x8,y3),8,(x4,y8),3)
  // note that axes don't coincide with global coordinates
  double pDzTopBox4  = 0.5 * ( xPosVertex[2] - xPosVertex[7] );
  double pDxTopBox4  = 0.5 * ( yPosVertex[2] - yPosVertex[7] );
  
  GeoBox* sTopBox4 = new GeoBox( pDxTopBox4 - tolerance, pDy - tolerance, pDzTopBox4 - tolerance );
  
  // position/orientation relative to first trapezoid
  HepTransform3D xfTopBox4 = HepTranslate3D( yPosVertex[7] + pDxTopBox4, 0, xPosVertex[7] + pDzTopBox4 );
  xfTopBox4 = xfTopBox4 * trlTrap1.inverse();
  
  //------------------------
  // first cutout from first top box
  // double pDyF1F2	= 0.5 * (yPosVertex[4] - yPosVertex[5]);
  double pDxF3F2	= 0.5 * (-xPosVertF3Extr + xPosVertex[5]);
  double pDyF2F3	= 0.5 * (yPosVertex[5] - yPosVertF3Extr);
  double alphaF2F3	= atan( pDyF2F3/pDxF3F2 );
  
  GeoPara* sEmptyPara = new GeoPara( pDyF2F3, 2*pDy, pDxF3F2, 0, alphaF2F3, 0 );

//position/orientation relative to first trapezoid
  HepTransform3D trlEmptyPara = HepTranslate3D( yPosVertex[5],  0, (xPosVertex[5] + xPosVertF3Extr)/2 );
  trlEmptyPara = trlEmptyPara * trlTrap1.inverse();
  
  
//-----------------------------------------
//second cutout from first top box
  double pDxF4F3	= 0.5 * (xPosVertF3Extr - xPosVertF4Extr);
  double pDxF5F4	= xPosVertF4Extr - xPosVertF5Extr;
  double pDzF5F4	= 0.5 * (yPosVertF5Extr - yPosVertF4Extr);
  
  GeoTrd* sEmptyTrap	= new GeoTrd( pDxF4F3 + pDxF5F4, pDxF4F3, 2*pDy, 2*pDy, pDzF5F4 + tolerance);
  
  //position/orientation relative to first trapezoid
  HepTransform3D trlEmptyTrap = HepTranslate3D( (yPosVertF4Extr + yPosVertF5Extr)/2, 0, 
                                                (xPosVertF3Extr + xPosVertF4Extr)/2 );
  trlEmptyTrap = trlEmptyTrap * trlTrap1.inverse(); 
  HepTransform3D xfEmptyTrap = trlEmptyTrap * HepRotateY3D( 3*M_PI/2 );
  

  //MiniConnExtr plates
  //ss 12/02/2007 never used GeoBox* sMiniConnPlateExtr = new GeoBox(yHeightMiniPExtr/2, zWidthMiniPExtr/2, xlengthMiniPExtr/2 );
  GeoBox* sEmptyMiniConnPlateExtr = new GeoBox( yHeightMiniPExtr/2 + tolerance, zWidthMiniPExtr/4,
						xlengthMiniPExtr/2 + tolerance );
  
  HepTransform3D trlMiniConnPlateExtr = HepTranslate3D( yPosMiniPExtr, 0, xPosMiniPExtr );
  trlMiniConnPlateExtr = trlMiniConnPlateExtr * trlTrap1.inverse();
  HepTransform3D xfMiniConnPlateExtr = trlMiniConnPlateExtr * HepRotateY3D(beta);

  //------------------------------------------------------------------------------------------
  // shapes to be cut out
  //------------------------------------------------------------------------------------------
  // box to cut out the protruding edge of the middle trapezoid (uses vertices 9,10)
  double dx89        = fabs( xPosVertex[8] - xPosVertex[9] );
  double dy89        = fabs( yPosVertex[8] - yPosVertex[9] );
  double dxEmptyBox1 = 2 * sqrt( dx89 * dx89 + dy89 * dy89 ); // some 'large enough' value
  double dzEmptyBox1 = pDzTrap2;                          // some 'large enough' value
  
  GeoBox* sEmptyBox1 = new GeoBox( dxEmptyBox1, 2*pDy, dzEmptyBox1 );    
  
  // position/orientation relative to first trapezoid
  double alpha1 = atan( dx89/dy89 );    // inclination angle of cutout box
  HepTransform3D trlEmptyBox1 = HepTranslate3D( yPosVertex[8] - dzEmptyBox1 * sin(alpha1), 0, 
                                                xPosVertex[8] - dzEmptyBox1 * cos(alpha1) );
  trlEmptyBox1 = trlEmptyBox1 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox1 = trlEmptyBox1 * HepRotateY3D(alpha1);

  
  //------------------------------------------------------------------------------------------
  // tubes to cut out the space for the Barrel Toroid coils
  GeoTube* sEmptyTube = new GeoTube(0, radiusCutout, 2*pDy );

  // position/orientation relative to first trapezoid
  HepTransform3D trlEmptyTube = HepTranslate3D( yPosCentreCutout, 0, xPosCentreCutout );
  trlEmptyTube = trlEmptyTube * trlTrap1.inverse();                                           
  HepTransform3D xfEmptyTube = trlEmptyTube * HepRotateX3D( M_PI/2 ) * HepRotateY3D( M_PI );
  
  GeoTube* sEmptyTube2 = new GeoTube( 0, SecradiusCutExtr, 2*pDy );
  HepTransform3D trlEmptyTube2 = HepTranslate3D( yPosSecCentreCut, 0, xPosSecCentreCut );
  trlEmptyTube2 = trlEmptyTube2 * trlTrap1.inverse();
  HepTransform3D xfEmptyTube2 = trlEmptyTube2 * HepRotateX3D( M_PI/2 ) * HepRotateY3D( M_PI );

  
  //------------------------------------------------------------------------------------------
  // first box to cut out space for the Barrel Toroid Coils 
  double lengthC1C1a = sqrt ( (xPosVertC1Extr - xPosVertC1aExtr) * (xPosVertC1Extr - xPosVertC1aExtr) +
			      (yPosVertC1Extr - yPosVertC1aExtr) * (yPosVertC1Extr - yPosVertC1aExtr) );
  //double dxC1C1a     = xPosVertC1Extr - xPosVertC1aExtr; 
  
  GeoBox* sEmptyBox2 = new GeoBox( radiusCutout - tolerance, 2 * pDy, lengthC1C1a/2 );  // some 'large enough' values 

  // position/orientation relative to first trapezoid
  HepTransform3D trlEmptyBox2 = HepTranslate3D( HepRotateY3D(beta) * HepPoint3D( radiusCutout, 0, -lengthC1C1a/2 ) ) *
						HepTranslate3D( yPosVertC1Extr, 0, xPosVertC1Extr );
  trlEmptyBox2 = trlEmptyBox2 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox2 = trlEmptyBox2 * HepRotateY3D(beta);


//------------------------------------------------------------------------------------------
// second box to cut out space for the Barrel Toroid Coils
  
  GeoBox* sEmptyBox3 = new GeoBox( radiusCutout/2, 2*pDy, (xPosVertC1aExtr - xPosVertC1bExtr)/2 );  
  
// position/orientation relative to first trapezoid
  HepTransform3D trlEmptyBox3 = HepTranslate3D( yPosVertC1aExtr + radiusCutout/2, 0, 
                                               (xPosVertC1aExtr + xPosVertC1bExtr)/2 );
  trlEmptyBox3 = trlEmptyBox3 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox3 = trlEmptyBox3;
  
//------------------------------------------------------------------------------------------
// third box to cut out the remaining space for the Barrel Toroid Coils 

  double lengthC5C4   = sqrt( (xPosVertC5Extr - xPosVertC4Extr) * (xPosVertC5Extr - xPosVertC4Extr) + 
                              (yPosVertC4Extr - yPosVertC5Extr) *
			      (yPosVertC4Extr - yPosVertC5Extr) );
  //double dyC5C4	      = yPosVertC4Extr - yPosVertC5Extr;
  double alphaC4C5    = atan( (xPosVertC4Extr - xPosVertC5Extr)/(yPosVertC4Extr - yPosVertC5Extr) );


  GeoBox* sEmptyBox4 = new GeoBox(lengthC5C4/2 - tolerance, 3 * pDy, radiusCutout/2 );  
  
// position/orientation relative to first trapezoid
  HepTransform3D trlEmptyBox4 = HepTranslate3D( HepRotateY3D(-alphaC4C5) * 
                                HepPoint3D( -lengthC5C4/2, 0, radiusCutout/2 ) ) *
                                HepTranslate3D( yPosVertC4Extr, 0, xPosVertC4Extr );
  trlEmptyBox4 = trlEmptyBox4 * trlTrap1.inverse();  
  HepTransform3D xfEmptyBox4 = trlEmptyBox4 * HepRotateY3D(-alphaC4C5);  

//------------------------------------------------------------------------------------------------
// last box to cut out the remaining space for the Barrel Toroid Coils
  
  double lengthDC3	= sqrt( (xPosVertDExtr - xPosVertC3Extr) * (xPosVertDExtr - xPosVertC3Extr) + 
                                (yPosVertDExtr - yPosVertC3Extr) *
				(yPosVertDExtr - yPosVertC3Extr) );
//   double lengthDC1	= sqrt( (xPosVertDExtr - xPosVertC1Extr) * (xPosVertDExtr - xPosVertC1Extr) + 
//                                 (yPosVertDExtr - yPosVertC1Extr) *
// 				(yPosVertDExtr - xPosVertC1Extr) );
//   double dxDC3		= xPosVertDExtr - xPosVertC3Extr;
  double alphaDC3	= atan( (yPosVertDExtr - yPosVertC3Extr)/(xPosVertDExtr - xPosVertC3Extr) );
  
  GeoBox* sEmptyBox5    = new GeoBox( radiusCutout, 2 * pDy, lengthDC3/2 - tolerance );
  
// position/orientation relative to first trapezoid
  HepTransform3D trlEmptyBox5 = HepTranslate3D( HepRotateY3D(alphaDC3) * HepPoint3D( -radiusCutout, 0, -lengthDC3/2 ) ) *
				HepTranslate3D( yPosVertDExtr, 0, xPosVertDExtr );
  trlEmptyBox5 = trlEmptyBox5 * trlTrap1.inverse();
  HepTransform3D xfEmptyBox5 = trlEmptyBox5 * HepRotateY3D(alphaDC3);

  
//------------------------------------------------------------------------------------------
// unite the two trapezoids and two top boxes to obtain the side-plate base shape, 
// cut out the tube and various boxes
//------------------------------------------------------------------------------------------
  const GeoShape& sSidePlate = sTrap1->add( (*sTrap2) << xfTrap2 ).
                                       subtract( (*sEmptyBox1) << xfEmptyBox1 ).
                                       subtract( (*sEmptyBox2) << xfEmptyBox2 ).
                                       subtract( (*sEmptyBox3) << xfEmptyBox3 ).
                                       subtract( (*sEmptyBox4) << xfEmptyBox4 ).
				       subtract( (*sEmptyBox5) << xfEmptyBox5 ).
                                       subtract( (*sEmptyTube) << xfEmptyTube ).
				       subtract( (*sEmptyTube2) << xfEmptyTube2 ).
				       subtract( (*sEmptyMiniConnPlateExtr) << xfMiniConnPlateExtr ).
//  				       add( (*sMiniConnPlateExtr) << xfMiniConnPlateExtr ).
                                       add( (*sTopBox1) << xfTopBox1 ).
                                       add( (*sTopBox2) << xfTopBox2 ).
				       add( (*sTopBox3) << xfTopBox3 ).
				       add( (*sTopBox4) << xfTopBox4 ).
				       subtract( (*sEmptyPara) << trlEmptyPara ).
				       subtract( (*sEmptyTrap) << xfEmptyTrap );

  GeoLogVol*  lSidePlate = new GeoLogVol( "ExtrFeetSidePlate", 
                                          &sSidePlate, 
                                          theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetSidePlate "<<std::endl;
  GeoPhysVol* pSidePlate = new GeoPhysVol(lSidePlate);


//------------------------------------------------------------------------------------------
// replicate and position
//------------------------------------------------------------------------------------------
// array of z positions
  const int nZPosSidePlate = nZPosMax;
  double zPosSidePlate[nZPosSidePlate];
  for ( int i = 0; i < 2; i++ ) 
  {  
     double zPos = ( i  ?  zPositionFeet[0]  :  zPositionFeet[ nFeet - 1 ] );
     zPosSidePlate[ 2*i ]     = zPos - zWidth/2;
     zPosSidePlate[ 2*i + 1 ] = zPos + zWidth/2;
// DHW zPosSidePlate[ 2*i ]     = zPos - zWidth/2 + thicknessSidePlate/2;    
//     zPosSidePlate[ 2*i + 1 ] = zPos + zWidth/2 - thicknessSidePlate/2;    
  }

// array of x positions
  const int nXPos = nXPosMax;
// final x-position of first trapezoid centre
  double xMeanSidePlate = 0.5 * ( xPosVertex[0] + xPosVertex[11] );
  double xPosSidePlate[nXPos] = { -xMeanSidePlate, xMeanSidePlate };

// array of rotation angles (to flip shapes around x-axis)
  double rotAngleSidePlate[nXPos] = { M_PI, 0 };
    
// fill auxiliary arrays 
  for ( int i = 0; i < nZPosSidePlate * nXPos; i++ )  
  {
     int ii = i % nZPosSidePlate, 
         jj = i / nZPosSidePlate;
     zPosAux[i]     = zPosSidePlate[ii];
     xPosAux[i]     = xPosSidePlate[jj];
     rotAngleAux[i] = rotAngleSidePlate[jj]; 
  }

  GENFUNCTION fRotSidePlate  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosSidePlate * nXPos ); 
  GENFUNCTION fTrlZSidePlate = ArrayFunction( zPosAux,     zPosAux     + nZPosSidePlate * nXPos ); 
  GENFUNCTION fTrlXSidePlate = ArrayFunction( xPosAux,     xPosAux     + nZPosSidePlate * nXPos ); 
  
  double yMean = yPosVertex[11] + 0.5 * ( pDx1Trap1 + pDx3Trap1 );            // final y-pos. of first trapezoid centre
  TRANSFUNCTION XFSidePlate = Pow( HepTranslateZ3D(1.0), fTrlZSidePlate ) * 
                              Pow( HepTranslateX3D(1.0), fTrlXSidePlate ) * 
                              HepTranslateY3D(yMean) *                       // const. displacement along y
                              HepRotateX3D( M_PI/2 ) *                       // const. rotations to bring shape into...
                              HepRotateY3D( M_PI/2 ) *                       // ...correct orientation  
                              Pow( HepRotateX3D(1.0), fRotSidePlate ); 

   GeoSerialTransformer* sxSidePlate = new GeoSerialTransformer( pSidePlate, &XFSidePlate, nZPosSidePlate * nXPos );
   container->add(sxSidePlate);

//------------------------------------------------------------------------------------------
//  Build large vertical plates, located between the side-plate pairs
//------------------------------------------------------------------------------------------
  
  double dyLargePlate = 0.5 * ( yMaxLargePlate - yMinLargePlate );
  double dzLargePlate = zWidth/2 - thicknessSidePlate;
  
  GeoBox* sLargePlate = new GeoBox( thicknessLargePlate/2, dyLargePlate, dzLargePlate - tolerance );

  GeoLogVol*  lLargePlate = new GeoLogVol( "ExtrFeetLargePlate", 
                                           sLargePlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetLargePlate "<<std::endl;
  GeoPhysVol* pLargePlate = new GeoPhysVol(lLargePlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of z positions (will be used later also for inner vertical, cover, and bottom plates)
  const int nZPosPlate = 2;
  double zPosPlate[nZPosPlate] = { zPositionFeet[0], zPositionFeet[ nFeet - 1 ] };

// array of x positions
  double xPosLargePlate[nXPos] = { -xMeanLargePlate, xMeanLargePlate };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosLargePlate[jj];
  }

  GENFUNCTION fTrlZLargePlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXLargePlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  double yPosLargePlate = yMinLargePlate + dyLargePlate;
  TRANSFUNCTION XFLargePlate = Pow( HepTranslateZ3D(1.0), fTrlZLargePlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXLargePlate ) * 
                               HepTranslateY3D(yPosLargePlate); 

  GeoSerialTransformer* sxLargePlate = new GeoSerialTransformer( pLargePlate, &XFLargePlate, nZPosPlate * nXPos );
  container->add(sxLargePlate);

//------------------------------------------------------------------------------------------
//  Build inner inclined plates, located between the side-plate pairs 
//------------------------------------------------------------------------------------------  

  const GeoShape* sInclPlate = new GeoBox( thicknessLargePlate/2, lengthInclinedPlate/2, dzLargePlate - tolerance );

  GeoBox* sEmptyBoxInclPlate = new GeoBox( thicknessLargePlate, yHeightCutInclinedPlate/2 + tolerance, 
                                           zLengthCutInclinedPlate/2 );
  
//position of EmptyBox centre relative to centre of Inclined Plate
  HepTransform3D trlEmptyBoxInclPlate = HepTranslate3D( 0, lengthInclinedPlate/2 - yHeightCutInclinedPlate/2 , 0 );
  
  const GeoShape& sInclinedPlate = sInclPlate->subtract( (*sEmptyBoxInclPlate) << trlEmptyBoxInclPlate );
  
  GeoLogVol*  lInclinedPlate = new GeoLogVol( "ExtrFeetInclinedPlate", 
                                              &sInclinedPlate, 
                                              theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetInclinedPlate "<<std::endl;
  GeoPhysVol* pInclinedPlate = new GeoPhysVol(lInclinedPlate);

//------------------------------------------------------------------------------------------
// replicate and position   
// array of x positions
  
  double xMeanInclinedPlate = -(lengthInclinedPlate * sinBeta)/2 - (thicknessLargePlate * cosBeta)/2 + 
                                xMeanLargePlate + thicknessLargePlate/2;
  
  double xPosInclinedPlate[nXPos] = { -xMeanInclinedPlate, xMeanInclinedPlate };

// array of inclination angles (different for left and right plates)
  double rotAngleInclinedPlate[nXPos] = { -beta, beta };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i]     = zPosPlate[ii];
     xPosAux[i]     = xPosInclinedPlate[jj];
     rotAngleAux[i] = rotAngleInclinedPlate[jj]; 
  }

  GENFUNCTION fRotInclinedPlate  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlZInclinedPlate = ArrayFunction( zPosAux,     zPosAux     + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXInclinedPlate = ArrayFunction( xPosAux,     xPosAux     + nZPosPlate * nXPos ); 
  
  double yMeanInclinedPlate = yPosLargePlate + dyLargePlate + (lengthInclinedPlate * cosBeta)/2 - 
                              (thicknessLargePlate * sinBeta)/2;
  
  TRANSFUNCTION XFInclinedPlate = Pow( HepTranslateZ3D(1.0), fTrlZInclinedPlate ) * 
                                  Pow( HepTranslateX3D(1.0), fTrlXInclinedPlate ) * 
                                  HepTranslateY3D(yMeanInclinedPlate) * 
                                  Pow( HepRotateZ3D(1.0), fRotInclinedPlate ); 

  GeoSerialTransformer* sxInclinedPlate = new GeoSerialTransformer( pInclinedPlate, 
                                                                    &XFInclinedPlate, 
                                                                    nZPosPlate * nXPos );
  container->add(sxInclinedPlate);

//------------------------------------------------------------------------------------------
//  Build outer inclined (cover) plates, which cover the Barrel Toroid coil cutouts
//------------------------------------------------------------------------------------------  

  const GeoShape* sCoverPlateExtr = new GeoBox( thicknessOuterPlate/2 - tolerance, lengthOuterPlate/2, 
                                                zWidthOuterPExtr/2 );

  GeoTube* sEmptyCoverTube        = new GeoTube( 0, radiusCutCoverPlateExtr, 2*thicknessOuterPlate );
  
//position of EmptyCover centre relative to centre of CoverPlate
  HepTransform3D trlEmptyCoverTube;
  trlEmptyCoverTube = HepTranslateY3D( -yoffsetCutCoverPlateExtr );
  HepTransform3D xfEmptyCoverTube = trlEmptyCoverTube * HepRotateY3D( M_PI/2 );
  
  const GeoShape& sCoverPlate = sCoverPlateExtr->subtract( (*sEmptyCoverTube) << xfEmptyCoverTube );

  GeoLogVol*  lCoverPlate = new GeoLogVol( "ExtrFeetCoverPlate", 
                                           &sCoverPlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetCoverPlate "<<std::endl;
  GeoPhysVol* pCoverPlate = new GeoPhysVol(lCoverPlate);

//------------------------------------------------------------------------------------------
// replicate and position   
  
  double DIAGExtr    = sqrt ( thicknessOuterPlate * thicknessOuterPlate + lengthOuterPlate * lengthOuterPlate );
  double alphaCPExtr = atan(thicknessOuterPlate/lengthOuterPlate);
  double betaCPExtr  = atan(dy12/dx12);

  // array of x positions
  double xMeanCoverPlate = xPosVertC1Extr + (yHeightMiniPExtr * sin(beta)) - (DIAGExtr * cos(betaCPExtr + alphaCPExtr))/2;
  double xPosCoverPlate[nXPos] = { -xMeanCoverPlate, xMeanCoverPlate };

  // array of inclination angles (different for left and right cover plates)
  double rotAngleCoverPlate[nXPos] = { -beta, beta };

  // fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i]     = zPosPlate[ii];
     xPosAux[i]     = xPosCoverPlate[jj];
     rotAngleAux[i] = rotAngleCoverPlate[jj]; 
  }

  GENFUNCTION fRotCoverPlate  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlZCoverPlate = ArrayFunction( zPosAux,     zPosAux     + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXCoverPlate = ArrayFunction( xPosAux,     xPosAux     + nZPosPlate * nXPos ); 
  
  double yPosCoverPlate = yPosVertC1Extr + (DIAGExtr *  sin(betaCPExtr + alphaCPExtr))/2 - yHeightMiniPExtr * cos(beta);
  TRANSFUNCTION XFCoverPlate = Pow( HepTranslateZ3D(1.0), fTrlZCoverPlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXCoverPlate ) * 
                               HepTranslateY3D(yPosCoverPlate) * 
                               Pow( HepRotateZ3D(1.0), fRotCoverPlate ); 

  GeoSerialTransformer* sxCoverPlate = new GeoSerialTransformer( pCoverPlate, &XFCoverPlate, nZPosPlate * nXPos );
  container->add(sxCoverPlate);

//------------------------------------------------------------------------------------------
//  Build inner (top) vertical plates, on which the Top Voussoirs plates are mounted
//------------------------------------------------------------------------------------------
  
  GeoBox* sInnerTopPlate = new GeoBox( thicknessInnerPlate/2 - tolerance, yHeightInnerPlateExtr/2, zWidth/2 );
 
  GeoLogVol*  lInnerTopPlate = new GeoLogVol( "ExtrFeetInnerPlate", 
                                           sInnerTopPlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetInnerPlate "<<std::endl;
  GeoPhysVol* pInnerTopPlate = new GeoPhysVol(lInnerTopPlate);

//------------------------------------------------------------------------------------------
// replicate and position array of x positions
  double xPosInnerTopPlate[nXPos] = { -xPosVertex[6] + thicknessInnerPlate/2, xPosVertex[6] - thicknessInnerPlate/2 };

// fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosInnerTopPlate[jj];
  }

  GENFUNCTION fTrlZInnerTopPlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXInnerTopPlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  TRANSFUNCTION XFInnerTopPlate = Pow( HepTranslateZ3D(1.0), fTrlZInnerTopPlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXInnerTopPlate ) * 
                               HepTranslateY3D(yPosCentrInPlateExtr); 

  GeoSerialTransformer* sxInnerTopPlate = new GeoSerialTransformer( pInnerTopPlate, &XFInnerTopPlate, 
                                                                    nZPosPlate * nXPos );
  container->add(sxInnerTopPlate);

  //------------------------------------------------------------------------------------------
  //Build inner Top Voussoirs plates
  //------------------------------------------------------------------------------------------

  GeoBox* sInnerTopVoussPlate     = new GeoBox( xWidthTopPlateExtr/2 - tolerance, yHeightTopPlateExtr/2, 
                                                zLengthTopPlateExtr/2 );

  GeoLogVol*  lInnerTopVoussPlate = new GeoLogVol( "ExtrFeetInnerTopVoussPlate", 
                                           sInnerTopVoussPlate, 
                                           theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetInnerTopVoussPlate "<<std::endl;
  GeoPhysVol* pInnerTopVoussPlate = new GeoPhysVol(lInnerTopVoussPlate);

  //------------------------------------------------------------------------------------------
  // replicate and position   

  // array of x positions
  double xPosInnerTopVoussPlate[nXPos] = { -xPosVertex[6] + thicknessInnerPlate + xWidthTopPlateExtr/2,
					    xPosVertex[6] - thicknessInnerPlate - xWidthTopPlateExtr/2 };

  // fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosInnerTopVoussPlate[jj];
  }

  GENFUNCTION fTrlZInnerTopVoussPlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXInnerTopVoussPlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
 
  TRANSFUNCTION XFInnerTopVoussPlate = Pow( HepTranslateZ3D(1.0), fTrlZInnerTopVoussPlate ) * 
                               Pow( HepTranslateX3D(1.0), fTrlXInnerTopVoussPlate ) * 
                               HepTranslateY3D(yPosCentrInPlateExtr); 

  GeoSerialTransformer* sxInnerTopVoussPlate = new GeoSerialTransformer( pInnerTopVoussPlate, 
                                                                         &XFInnerTopVoussPlate, nZPosPlate * nXPos );
  container->add(sxInnerTopVoussPlate);

//------------------------------------------------------------------------------------------
//  Build bottom plates
//------------------------------------------------------------------------------------------
  
  GeoBox* sBottomPlate = new GeoBox( pDzTrap1, thicknessBottomPlate/2 - tolerance, zWidthBotExtr/2 );

  GeoLogVol*  lBottomPlate = new GeoLogVol( "ExtrFeetBottomPlate", 
                                            sBottomPlate, 
                                            theMaterialManager->getMaterial(feetMaterial) );
  // std::cout<<" FeetToroidBuilderRDBn::buildExtremityFeet  ExtrFeetBottomPlate "<<std::endl;
  GeoPhysVol* pBottomPlate = new GeoPhysVol(lBottomPlate);

  //------------------------------------------------------------------------------------------
  // replicate and position   

  // array of x positions
  double xPosBottomPlate[nXPos] = { -0.5 * ( xPosVertex[0] + xPosVertex[11] ), 
                                     0.5 * ( xPosVertex[0] + xPosVertex[11] ) };

  // fill auxiliary arrays 
  for ( int i = 0; i < nZPosPlate * nXPos; i++ )  
  {
     int ii = i % nZPosPlate, 
         jj = i / nZPosPlate;
     zPosAux[i] = zPosPlate[ii];
     xPosAux[i] = xPosBottomPlate[jj];
  }

  GENFUNCTION fTrlZBottomPlate = ArrayFunction( zPosAux, zPosAux + nZPosPlate * nXPos ); 
  GENFUNCTION fTrlXBottomPlate = ArrayFunction( xPosAux, xPosAux + nZPosPlate * nXPos ); 
  
  double yPosBottomPlate = yPosVertex[0] - thicknessBottomPlate/2;
  TRANSFUNCTION XFBottomPlate = Pow( HepTranslateZ3D(1.0), fTrlZBottomPlate ) * 
                                Pow( HepTranslateX3D(1.0), fTrlXBottomPlate ) * 
                                HepTranslateY3D(yPosBottomPlate); 

  GeoSerialTransformer* sxBottomPlate = new GeoSerialTransformer( pBottomPlate, &XFBottomPlate, nZPosPlate * nXPos );
  container->add(sxBottomPlate);

  // std::cout<<" FeetToroidBuilderRDB::buildExtremityFeet  sxBottomPlate  "<<std::endl;
}

void FeetToroidBuilderRDB::buildFeetGirders( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds Feet Girders (i.e., 'bridges' between feet pairs adjacent in z)   
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int NumGir = 3;

//------------ Feet General Parameters ---------------------------------//
   double zPositionFeet[maxDim];
     zPositionFeet[0] = -(*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
     zPositionFeet[1] = -(*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
     zPositionFeet[2] = -(*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
     zPositionFeet[3] = -(*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
     zPositionFeet[4] =  (*m_Feet)[0]->getFloat("ZPOSFEE1") * mm;
     zPositionFeet[5] =  (*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
     zPositionFeet[6] =  (*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
     zPositionFeet[7] =  (*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
     zPositionFeet[8] =  (*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;  

   const std::string feetMaterial = getMaterial( "Iron" );

   //const double zWidthStandFeet = (*m_Feet)[0]->getFloat("MNPLZSEP") * mm;

//------------------ Extremity Feet Parameters -------------------------------//

//   const double zWidthExtrFeet = (*m_Feet)[0]->getFloat("EXMPZSEP") * mm;

//------------------ Standard Feet Girder Parameters -------------------------//
   const double zTotalLengthGirder12		= (*m_Feet)[0]->getFloat("G12UPXWI") * mm;
   const double zInnerPlateSmall2LargeGirder12	= (*m_Feet)[0]->getFloat("G12IPSLX") * mm;
   const double zInnerPlateLarge2LargeGirder12	= (*m_Feet)[0]->getFloat("G12IPLLX") * mm;
   const double zInnerPlateLarge2SmallGirder12	= (*m_Feet)[0]->getFloat("G12IPLSX") * mm;
   const double zLengthSideBottomPlateGirder12	= (*m_Feet)[0]->getFloat("G12RPXWI") * mm;
   const double zLengthCentralBotPlateGirder12	= (*m_Feet)[0]->getFloat("G12LPXW1") * mm;      

   const double zTotalLengthGirder23		= (*m_Feet)[0]->getFloat("G23UPXWI") * mm;
   const double zInnerPlateSmall2LargeGirder23	= (*m_Feet)[0]->getFloat("G23IPSLX") * mm;
   const double zInnerPlateLarge2LargeGirder23	= (*m_Feet)[0]->getFloat("G23IPLLX") * mm;
   const double zInnerPlateLarge2SmallGirder23	= (*m_Feet)[0]->getFloat("G23IPLSX") * mm;
   const double zLengthSideBottomPlateGirder23	= (*m_Feet)[0]->getFloat("G23RPXWI") * mm;
   const double zLengthCentralBotPlateGirder23	= (*m_Feet)[0]->getFloat("G23LPXW1") * mm;

   const double zTotalLengthGirder34		= (*m_Feet)[0]->getFloat("G34UPXWI") * mm;
   const double zInnerPlateSmall2LargeGirder34	= (*m_Feet)[0]->getFloat("G34IPSLX") * mm;
   const double zInnerPlateLarge2LargeGirder34  = (*m_Feet)[0]->getFloat("G34IPLLX") * mm;
   const double zInnerPlateLarge2SmallGirder34	= (*m_Feet)[0]->getFloat("G34IPLSX") * mm;
   const double zLengthSideBottomPlateGirder34	= (*m_Feet)[0]->getFloat("G34RPXWI") * mm;
   const double zLengthCentralBotPlateGirder34	= (*m_Feet)[0]->getFloat("G34LPXW1") * mm;   

   const double zoffsetInSPG	= (*m_Feet)[0]->getFloat("G12IPSXO") * mm;
   const double thicknessInnerPG= (*m_Feet)[0]->getFloat("G12IPXWI") * mm;
   const double zWidthUpMPG	= (*m_Feet)[0]->getFloat("G12UMPWI") * mm;
   const double zWidthSMPG	= (*m_Feet)[0]->getFloat("G12SMPWI") * mm;
   const double yTotHeightG	= (*m_Feet)[0]->getFloat("G12LPYHT") * mm;
   const double xWidthTopPG	= (*m_Feet)[0]->getFloat("G12UPZLE") * mm;
   const double xLengthUpMPG	= (*m_Feet)[0]->getFloat("G12UMPZL") * mm;
   const double xWidthBotPG	= (*m_Feet)[0]->getFloat("G12BPZLE") * mm;
   const double thicknessSPG	= (*m_Feet)[0]->getFloat("G12RPZLE") * mm;
   const double thicknessSMPG	= (*m_Feet)[0]->getFloat("G12SMPZL") * mm;
   const double yHeightUpMPG	= (*m_Feet)[0]->getFloat("G12UMPYH") * mm;
   const double yHeightTopPG	= (*m_Feet)[0]->getFloat("G12UPYTH") * mm;
   const double yHeightBotPG	= (*m_Feet)[0]->getFloat("G12LPYTH") * mm;
   const double zLengthSlBotPG	= (*m_Feet)[0]->getFloat("G12LPX12") * mm;
   const double xWidthInPG	= (*m_Feet)[0]->getFloat("G12BPZLE") * mm;
   const double yHeightInLPG	= (*m_Feet)[0]->getFloat("G12LPYH1") * mm;
   const double yHeightInSPG	= (*m_Feet)[0]->getFloat("G12LPYH2") * mm;
   const double yHeightSMPG	= (*m_Feet)[0]->getFloat("G12RPYDI") * mm;
   const double xMeanStd	= (*m_Feet)[0]->getFloat("GIRDXPOS") * mm;
   const double yPosFoot	= (*m_Feet)[0]->getFloat("STDFOOYP") * mm;
   const double G12UPypo	= (*m_Feet)[0]->getFloat("G12UPYPO") * mm;
   const double GirdYHei        = (*m_Feet)[0]->getFloat("GIRDYHEI") * mm;
//----------------------------------------------------------------------------//  

  const double tolerance = 0.5*mm;
  
  double zTotLengthG[NumGir];
  double zInnerS2LG[NumGir];
  double zInnerL2LG[NumGir];
  double zInnerL2SG[NumGir];
  double zLengthSBotPG[NumGir];
  double zLengthCBotPG[NumGir];

  // auxiliary arrays to store z, x positions, and rotation angles for all volumes to be replicated 
  const int nZPos = 2;
  const int nXPos = 2;

  double zPosAux[ nZPos * nXPos ],  xPosAux[ nZPos * nXPos ];//,  rotAngleAux[ nZPos * nXPos ];
    
  //------------------------------------------------------------------------------------------
  //  Girder12 properties 
  zTotLengthG[2]   = zTotalLengthGirder12;
  zInnerS2LG[2]    = zInnerPlateSmall2LargeGirder12;
  zInnerL2LG[2]	   = zInnerPlateLarge2LargeGirder12;
  zInnerL2SG[2]	   = zInnerPlateLarge2SmallGirder12;
  zLengthSBotPG[2] = zLengthSideBottomPlateGirder12;
  zLengthCBotPG[2] = zLengthCentralBotPlateGirder12;
  
  //   Girder23 properties
  zTotLengthG[1]   = zTotalLengthGirder23;
  zInnerS2LG[1]	   = zInnerPlateSmall2LargeGirder23;
  zInnerL2LG[1]	   = zInnerPlateLarge2LargeGirder23;
  zInnerL2SG[1]	   = zInnerPlateLarge2SmallGirder23;
  zLengthSBotPG[1] = zLengthSideBottomPlateGirder23;
  zLengthCBotPG[1] = zLengthCentralBotPlateGirder23;
  
  //   Girder34 properties
  zTotLengthG[0]   = zTotalLengthGirder34;
  zInnerS2LG[0]	   = zInnerPlateSmall2LargeGirder34;
  zInnerL2LG[0]	   = zInnerPlateLarge2LargeGirder34;
  zInnerL2SG[0]	   = zInnerPlateLarge2SmallGirder34;
  zLengthSBotPG[0] = zLengthSideBottomPlateGirder34;
  zLengthCBotPG[0] = zLengthCentralBotPlateGirder34;

    // for side plate of girder
  const double yHeightTopSPG	= yHeightSMPG;
  const double yHeightBotSPG	= yTotHeightG - yHeightSMPG;
  
  // for slanted bottom plate    
  double deltaH = yHeightInLPG - yHeightInSPG;
  double alphaG = atan( deltaH/zLengthSlBotPG );
  
//  const double yMeanStd	= HeightG + yHeightTopPG/2 + yPosFoot + yHeightGirder;
//    const double yMeanStd	= G12UPypo + yPosFoot;
    const double yMeanStd = G12UPypo + yPosFoot + GirdYHei;

  //------------------------------------------------------------------------------------------
  //  Loop on Standard Feet. Do everything within the loop: 
  //   - determine z-length and z-position of Girder centre
  //   - build Standard Feet Girder
  //   - replicate the built shapes (front/back (z), right/left (x))
  //------------------------------------------------------------------------------------------
  
  for ( int i = 1; i < 4; i++ )            // girders at +/-z are identical 
  {
     double zMinStd   = zPositionFeet[i];     
     double zMaxStd   = zPositionFeet[ i + 1 ];     
//   double deltaZStd = zMaxStd - zMinStd - zWidthStandFeet; 
     double zMeanStd  = 0.5 * ( zMinStd + zMaxStd );

     int i3;
     i3 = i - 1;
  
     // use loop variable to label (slightly) different girder volumes
     my_osstream os;
     os << std::setw(2) << std::setfill('0') << i;


  //------------------------------------------------------------------------------------------
  //  Build Feet Girder12
  //------------------------------------------------------------------------------------------

    // top plate
    const GeoShape* sTopPlateGirder	= new GeoBox( xWidthTopPG/2, yHeightTopPG/2 - tolerance, zTotLengthG[i3]/2 );
    
    // two small plates
    GeoBox* sSmallPlateGirder		= new GeoBox( xWidthInPG/2 - tolerance, yHeightInSPG/2 - tolerance, thicknessInnerPG/2 );
    
    //two large plates
    GeoBox* sLargePlateGirder		= new GeoBox( xWidthInPG/2 - tolerance, yHeightInLPG/2 - tolerance, thicknessInnerPG/2 );
    
    //two side (Top) Plates
    GeoBox* sSideTopPlateGirder		= new GeoBox( thicknessSPG/2 - tolerance, yHeightTopSPG/2 - tolerance, zTotLengthG[i3]/2 );
    
    //two side (bottom) plates
    GeoBox* sSideBotPlateGirder		= new GeoBox( thicknessSPG/2 - tolerance, yHeightBotSPG/2 - tolerance, zLengthSBotPG[i3]/2 );
    
    // two upper mini plates
    GeoBox* sUpperMiniPlateGirder	= new GeoBox( xLengthUpMPG/2, yHeightUpMPG/2 - tolerance, zWidthUpMPG/2 );
    
    //four side mini plates
    GeoBox* sMiniPlateGirder		= new GeoBox( thicknessSMPG/2 - tolerance, yHeightSMPG/2 - tolerance, zWidthSMPG/2 );
    
    //bottom plate
    GeoBox* sBottomPlateGirder		= new GeoBox( xWidthBotPG/2 - tolerance, yHeightBotPG/2 - tolerance, zLengthCBotPG[i3]/2 );
    
    //slanted plates
    GeoPara* sSlantedPlateGirder_1	= new GeoPara( (zLengthSlBotPG - tolerance)/(2 * cos(alphaG)), yHeightBotPG/2, xWidthBotPG/2,
						     -alphaG, 0, 0 );
    GeoPara* sSlantedPlateGirder_2	= new GeoPara( (zLengthSlBotPG - tolerance)/(2 * cos(alphaG)), yHeightBotPG/2, xWidthBotPG/2,
						     alphaG, 0, 0 );
    //two upper bottom plate
    GeoBox* sUpBotPlateGirder		= new GeoBox( xWidthBotPG/2 - tolerance, yHeightBotPG/2 - tolerance, 
							zTotLengthG[i3]/4 - zLengthCBotPG[i3]/4 - zLengthSlBotPG/2 - tolerance );


    //position of shapes' centres relative to center of top plate
    
    //two small plates
    HepTransform3D trlSmallRPlateGirder		= HepTranslate3D( 0, -yHeightInSPG/2 - yHeightTopPG/2,
								 -zTotLengthG[i3]/2 + zoffsetInSPG + thicknessInnerPG/2);
    HepTransform3D trlSmallLPlateGirder		= HepTranslate3D( 0, -yHeightInSPG/2 - yHeightTopPG/2,
								 -zTotLengthG[i3]/2 + zoffsetInSPG + thicknessInnerPG/2
								 + zInnerS2LG[i3] + zInnerL2LG[i3] + zInnerL2SG[i3]);
    
    //two large plates
    HepTransform3D trlLargeRPlateGirder		= HepTranslate3D( 0, -yHeightInLPG/2 - yHeightTopPG/2, 
								 -zTotLengthG[i3]/2 + zoffsetInSPG + zInnerS2LG[i3] 
								    + thicknessInnerPG/2 );
    HepTransform3D trlLargeLPlateGirder		= HepTranslate3D( 0, -yHeightInLPG/2 - yHeightTopPG/2, 
								 -zTotLengthG[i3]/2 + zoffsetInSPG + zInnerS2LG[i3]
								  + thicknessInnerPG/2 + zInnerL2LG[i3]);
    
    //two upper mini plates
    HepTransform3D trlUpperMiniRPlateGirder	= HepTranslate3D( 0, yHeightUpMPG/2 + yHeightTopPG/2, zTotLengthG[i3]/2 - zWidthUpMPG/2 );
    HepTransform3D trlUpperMiniLPlateGirder 	= HepTranslate3D( 0, yHeightUpMPG/2 + yHeightTopPG/2, -zTotLengthG[i3]/2 + zWidthUpMPG/2 );
    
    //two side (top) plates
    HepTransform3D trlInnerSideTopPlateGirder	= HepTranslate3D( -xWidthBotPG/2 - thicknessSPG/2, -yHeightTopSPG/2 - yHeightTopPG/2, 0 );
    HepTransform3D trlOuterSideTopPlateGirder	= HepTranslate3D( xWidthBotPG/2 + thicknessSPG/2, -yHeightTopSPG/2 - yHeightTopPG/2, 0 );
    
    //two side (bottom) plates
    HepTransform3D trlInnerSideBotPlateGirder	= HepTranslate3D( -xWidthBotPG/2 - thicknessSPG/2,
								 -yHeightTopSPG - yHeightBotSPG/2 - yHeightTopPG/2, 0 );
    HepTransform3D trlOuterSideBotPlateGirder	= HepTranslate3D( xWidthBotPG/2 + thicknessSPG/2,
								 -yHeightTopSPG - yHeightBotSPG/2 - yHeightTopPG/2, 0 );
    
    //four side mini plates
    HepTransform3D trlInnerMiniRPlateGirder	= HepTranslate3D( -xWidthBotPG/2 - thicknessSPG - thicknessSMPG/2, -yHeightTopSPG/2
								    - yHeightTopPG/2, zTotLengthG[i3]/2 - zWidthSMPG/2 );
    HepTransform3D trlInnerMiniLPlateGirder	= HepTranslate3D( -xWidthBotPG/2 - thicknessSPG - thicknessSMPG/2, -yHeightTopSPG/2
								    -yHeightTopPG/2, -zTotLengthG[i3]/2 + zWidthSMPG/2 );
    HepTransform3D trlOuterMiniRPlateGirder	= HepTranslate3D( xWidthBotPG/2 + thicknessSPG + thicknessSMPG/2, -yHeightTopSPG/2
								    - yHeightTopPG/2, zTotLengthG[i3]/2 - zWidthSMPG/2 );
    HepTransform3D trlOuterMiniLPlateGirder	= HepTranslate3D( xWidthBotPG/2 + thicknessSPG + thicknessSMPG/2, -yHeightTopSPG/2
								    -yHeightTopPG/2, -zTotLengthG[i3]/2 + zWidthSMPG/2 );
    
    //bottom plate
    HepTransform3D trlBottomPlateGirder		= HepTranslate3D( 0, -yHeightInLPG - yHeightBotPG/2 - yHeightTopPG/2, 0 );
    
    //slanted plate
    HepTransform3D trlSlantedPlateGirder_1	= HepTranslate3D( 0, -deltaH/2 - yHeightInSPG - yHeightTopPG/2 - yHeightBotPG/2,
								 zLengthCBotPG[i3]/2 + zLengthSlBotPG/2 ) *
								 HepRotateX3D(-alphaG) * HepRotateY3D( M_PI/2 );
    HepTransform3D trlSlantedPlateGirder_2	= HepTranslate3D( 0, -deltaH/2 - yHeightInSPG - yHeightTopPG/2 - yHeightBotPG/2,
								 -zLengthCBotPG[i3]/2 - zLengthSlBotPG/2 ) * 
								HepRotateX3D(alphaG) * HepRotateY3D( M_PI/2 );

    //two upper bottom plates
    HepTransform3D trlUpBotRPlateGirder		= HepTranslate3D( 0, -yHeightInSPG - yHeightBotPG/2 - yHeightTopPG/2, 
								    zTotLengthG[i3]/4 + zLengthSlBotPG/2 + zLengthCBotPG[i3]/4 );
    HepTransform3D trlUpBotLPlateGirder		= HepTranslate3D( 0, -yHeightInSPG - yHeightBotPG/2 - yHeightTopPG/2,
								    -zTotLengthG[i3]/4 - zLengthSlBotPG/2 - zLengthCBotPG[i3]/4 );

    
    const GeoShape& sStdGirder = sTopPlateGirder->add( (*sSmallPlateGirder)	 << trlSmallRPlateGirder ).
						    add( (*sSmallPlateGirder)	 << trlSmallLPlateGirder ).
						    add( (*sLargePlateGirder)	 << trlLargeRPlateGirder ).
						    add( (*sLargePlateGirder)	 << trlLargeLPlateGirder ).
						    add( (*sUpperMiniPlateGirder)<< trlUpperMiniRPlateGirder ).
						    add( (*sUpperMiniPlateGirder)<< trlUpperMiniLPlateGirder ).
						    add( (*sSideTopPlateGirder)	 << trlInnerSideTopPlateGirder ).
						    add( (*sSideTopPlateGirder)	 << trlOuterSideTopPlateGirder ).
						    add( (*sSideBotPlateGirder)	 << trlInnerSideBotPlateGirder ).
						    add( (*sSideBotPlateGirder)	 << trlOuterSideBotPlateGirder ).
						    add( (*sMiniPlateGirder)	 << trlInnerMiniRPlateGirder ).
						    add( (*sMiniPlateGirder)	 << trlInnerMiniLPlateGirder ).
						    add( (*sMiniPlateGirder)	 << trlOuterMiniRPlateGirder ).
						    add( (*sMiniPlateGirder)	 << trlOuterMiniLPlateGirder ).
						    add( (*sBottomPlateGirder)	 << trlBottomPlateGirder ).
						    add( (*sUpBotPlateGirder)	 << trlUpBotRPlateGirder ).
						    add( (*sSlantedPlateGirder_1)<< trlSlantedPlateGirder_1 ).
						    add( (*sSlantedPlateGirder_2)<< trlSlantedPlateGirder_2 ).
						    add( (*sUpBotPlateGirder)	 << trlUpBotLPlateGirder );

     GeoLogVol*  lStdGirder = new GeoLogVol( "StdFeetGirder" + os.str(), 
                                             &sStdGirder, 
                                             theMaterialManager->getMaterial(feetMaterial) );
     GeoPhysVol* pStdGirder = new GeoPhysVol(lStdGirder);
     
     //---------------------------------------------------------------------------------------
     // replicate and position 
     //---------------------------------------------------------------------------------------

     // array of z-positions of girder centres
     double zPos[nZPos] = { -zMeanStd, zMeanStd };

     // array of x positions
     double xPos[nXPos] = { -xMeanStd, xMeanStd };
    
     // fill auxiliary arrays 
     for ( int j = 0; j < nZPos * nXPos; j++ )  
     {
        int jj = j % nZPos, 
            kk = j / nZPos;
        zPosAux[j] = zPos[jj];
        xPosAux[j] = xPos[kk];
     }

     GENFUNCTION fTrlZ = ArrayFunction( zPosAux, zPosAux + nZPos * nXPos ); 
     GENFUNCTION fTrlX = ArrayFunction( xPosAux, xPosAux + nZPos * nXPos ); 
  
     TRANSFUNCTION XFStdGirder = Pow( HepTranslateZ3D(1.0), fTrlZ ) * 
                                 Pow( HepTranslateX3D(1.0), fTrlX ) * 
                                 HepTranslateY3D(yMeanStd);             // const. displacement along y

     GeoSerialTransformer* sxStdGirder = new GeoSerialTransformer( pStdGirder, &XFStdGirder, nZPos * nXPos );
     container->add(sxStdGirder);

     // std::cout<<" FeetToroidBuilderRDB::buildFeetGirders   sxStdGirder "<<std::endl;

  }   
  

}




void FeetToroidBuilderRDB::buildFeetRailSupports( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds Rail Supports on Feet    
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 


//------------ Feet General Parameters ---------------------------------//
   double zPositionFeet[maxDim];
   zPositionFeet[0] = -(*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
   zPositionFeet[1] = -(*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[2] = -(*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[3] = -(*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[4] =  (*m_Feet)[0]->getFloat("ZPOSFEE1") * mm;
   zPositionFeet[5] =  (*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[6] =  (*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[7] =  (*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[8] =  (*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
                                                               
   const std::string feetMaterial = getMaterial( "Iron" );
//----------------------------------------------------------------------------//
   const double xPosFoot	     = (*m_Feet)[0]->getFloat("STDFOOXP") * mm;
   const double yPosFoot	     = (*m_Feet)[0]->getFloat("STDFOOYP") * mm;
   //const double zWidthStandFeet      = (*m_Feet)[0]->getFloat("MNPLZSEP") * mm;
   const double yPosVertex4StandFeet = (*m_Feet)[0]->getFloat("MAINPLYF") * mm;

//------------------ Extremity Feet Parameters -------------------------------//
   //const double zWidthExtrFeet      = (*m_Feet)[0]->getFloat("EXMPZSEP") * mm;
   const double yPosVertex4ExtrFeet = (*m_Feet)[0]->getFloat("EXMPLAYF") * mm;

//------------------ Feet Rail Support Parameters ----------------------------//
   const double xPosRailSuppFeet   = (*m_Feet)[0]->getFloat("EXRSPOSX") * mm;
   const double xWidth             = (*m_Feet)[0]->getFloat("XWIDTH")   * mm;
   const double thicknessMidPlate  = (*m_Feet)[0]->getFloat("MIDLLYTH") * mm;
   const double zWidthMidPlate	   = (*m_Feet)[0]->getFloat("CENTRLEZ") * mm;
   const double zWidthTopPlate	   = (*m_Feet)[0]->getFloat("UPPERLEZ") * mm;
   const double zWidthBotPlate	   = (*m_Feet)[0]->getFloat("LOWERLEZ") * mm;
   //const double zWidthCentralPlateRailSuppFeet	   = (*m_Feet)[0]->getFloat("CENTRLEZ") * mm;
   const double xCentrWidth	   = (*m_Feet)[0]->getFloat("CENTRXTH") * mm;
   const double yHeightCentrTB     = 29.0 * cm;
   const double yHeightVertPlate   = (*m_Feet)[0]->getFloat("CENTRHEY") * mm;
   const double thicknessVerPlate  = (*m_Feet)[0]->getFloat("VERTIZTH") * mm;
   const double thicknessBotBot    = (*m_Feet)[0]->getFloat("EXTREHEY") * mm;
   const double zLengthBotBot	   = (*m_Feet)[0]->getFloat("EXTRELEZ") * mm;
   const double yHeightStd	   = (*m_Feet)[0]->getFloat("TOTALHEY") * mm;
   const double thicknessTopPlate  = (*m_Feet)[0]->getFloat("UPPERHEY") * mm;
   const double thicknessBotPlate  = (*m_Feet)[0]->getFloat("LOWERHEY") * mm;
   const double zWidthTopPlateExtr = (*m_Feet)[0]->getFloat("EXRSUZLE") * mm;
   const double deltaTopBotPlateExtr   = (*m_Feet)[0]->getFloat("EXRSLDZL")* mm;
   const double thicknessBotBotExtr= (*m_Feet)[0]->getFloat("EXRSEYHE") * mm;
   const double yHeightCentrPlateExtr  = (*m_Feet)[0]->getFloat("EXRSCYHE") * mm;
   const double zLengthCentrPlateExtr  = (*m_Feet)[0]->getFloat("EXRSCZLE") * mm;
   const double zWidthMidPlateExtr = (*m_Feet)[0]->getFloat("EXRSMZLE") * mm;
   const double xCentrWidthExtr    = (*m_Feet)[0]->getFloat("EXRSCXWI") * mm;
   const double xWidthMidPlateExtr = (*m_Feet)[0]->getFloat("EXRSMXWI") * mm;
   const double xWidthVertPlateExtr= (*m_Feet)[0]->getFloat("EXRSVXWI") * mm;
   const double yHeightExtr        = (*m_Feet)[0]->getFloat("EXRSTYHE")   * mm;
   const double thicknessBotPlateExtr  = (*m_Feet)[0]->getFloat("EXRSLYHE")   * mm;
   const double zLengthBotBotExtr  = (*m_Feet)[0]->getFloat("EXRSEZOF")   * mm;
   const double zoffsetRSExtr	   = (*m_Feet)[0]->getFloat("EXRSZOFF")   * mm;
   const double zoffsetVertRSExtr  = (*m_Feet)[0]->getFloat("EXRSVZI1") * mm;
   const double zoffsetBotBotExtr  = (*m_Feet)[0]->getFloat("EXRSEZOF") * mm;
   const double zLengthMidCutExtr  = (*m_Feet)[0]->getFloat("EXRSMCWI") * mm;
   const double DepthMidCutExtr	   = (*m_Feet)[0]->getFloat("EXRSMCDE") * mm;
   const double zoffsetMidCutExtr  = (*m_Feet)[0]->getFloat("EXRSMCZO") * mm;
   const double radiusBotCutOutExtr= (*m_Feet)[0]->getFloat("EXRSC2Z2") * mm;
   const double zLengthBotCutOutExtr   = 55.0 * cm;
   const double yHeightBotCutOutExtr   = 32.0 * cm;
   const double zoffsetBotCutExtr  = (*m_Feet)[0]->getFloat("EXRSC1ZP") * mm;

// auxiliary arrays to store z, x positions, and rotation angles for all volumes to be replicated 
  const int nZPosMax = maxDim - 2;
  const int nXPosMax = 2;
  double zPosAux1[ nZPosMax * nXPosMax ], zPosAux2[ nZPosMax * nXPosMax ],  
         xPosAux1[ nZPosMax * nXPosMax ],  rotAngleAux[ nZPosMax * nXPosMax ],
	 xPosAux2[ nZPosMax * nXPosMax ], xPosAux[ nZPosMax * nXPosMax ];
    
  //------------------------------------------------------------------------------------------
  // common Rail Support properties      
  const double zposVertPlateExtr = zLengthCentrPlateExtr/2 - zoffsetVertRSExtr;
  const double zoffsetExtr	 = zLengthCentrPlateExtr/2 - zoffsetVertRSExtr - zWidthMidPlateExtr/2 - zoffsetRSExtr;
  
  const double zWidthBotPlateExtr = zWidthTopPlateExtr - deltaTopBotPlateExtr; 

//

  // common positioning variable for rail support centre
  const double xMean = xPosRailSuppFeet + xPosFoot;
  const double xMeanMinusRailSuppExtr = xPosRailSuppFeet + xPosFoot;
  const double xMeanPlusRailSuppExtr  = xPosRailSuppFeet + xPosFoot;
  
  const double tolerance = 0.5 * mm;    

  // positioning variable for rail support centre
  const double yMeanStd = yPosVertex4StandFeet + yHeightStd/2 + yPosFoot - thicknessBotBot/2;  // from Standard Foot top pos.    

  // positioning variable for rail support centre
  const double yMeanExtr = yPosVertex4ExtrFeet + yHeightExtr/2 + yPosFoot
			     - thicknessBotBotExtr/2;  // from Extremity Foot top pos.

  //-------------------------------------------------------------------------------------------
  // Build Standard Feet Rail Support seven boxes
  //--------------------------------------------
  
  // build base shape
  //-----------------------------
  
  // middle plate
  const GeoShape* sMiddleRailSuppPlateStd = new GeoBox( xWidth/2, thicknessMidPlate/2, zWidthMidPlate/2 );
  
  // top plate
  GeoBox* sTopRailSuppPlate		  = new GeoBox( xWidth/2, thicknessTopPlate/2 - tolerance, zWidthTopPlate/2 );

// two bottom plate
  GeoBox* sBotRailSuppPlate		  = new GeoBox( xWidth/2, thicknessBotPlate/2 - tolerance, 
                                                        zWidthBotPlate/2 );
  GeoBox* sBotBotRailSuppPlate		  = new GeoBox( xWidth/2, thicknessBotBot/2 - tolerance, 
                                                        zLengthBotBot/2 - tolerance );
  
  // two central plates
  GeoBox* sTopBotCentrRailSuppPlate	  = new GeoBox( xCentrWidth/2, yHeightCentrTB/2 - tolerance, zWidthMidPlate/2 - tolerance );
  
  // two Vertical plates
  GeoBox* sVertRailSuppPlate	 	 = new GeoBox( xWidth/2, yHeightVertPlate/2 - tolerance, thicknessVerPlate/2 - tolerance );

    // position of shapes' centres relative to centre of middle plate
    
    //top plate
    HepTransform3D trlTopRailSuppPlate  = HepTranslate3D( 0, yHeightCentrTB + thicknessMidPlate/2 + thicknessTopPlate/2, 0 );
    
    //two bottom plates
    HepTransform3D trlBotRailSuppPlate  = HepTranslate3D( 0, -yHeightCentrTB - thicknessMidPlate/2 - thicknessBotPlate/2, 0 );
    HepTransform3D trlBotBotRailPlate   = HepTranslate3D( 0, -yHeightCentrTB - thicknessMidPlate/2 - thicknessBotPlate - 
							thicknessBotBot/2, 0 );
							
    // two vertical plates
    HepTransform3D trlVert1RailPlate    = HepTranslate3D( 0, 0, zWidthMidPlate/2 + thicknessVerPlate/2 );
    HepTransform3D trlVert2RailPlate	= HepTranslate3D( 0, 0, -zWidthMidPlate/2 - thicknessVerPlate/2 );
    
    // two central plates
    HepTransform3D trlTopCentrRailPlate	= HepTranslate3D( 0, yHeightCentrTB/2 + thicknessMidPlate/2, 0 );
    HepTransform3D trlBotCentrRailPlate = HepTranslate3D( 0, -yHeightCentrTB/2 - thicknessMidPlate/2, 0 );
    

  //------------------------------------------------------------------------------------------
  //  Build Standard Feet Rail Support
  //------------------------------------------------------------------------------------------
   
  
  //add shapes' to middle plate

const GeoShape& sRailSuppStd = sMiddleRailSuppPlateStd->add( (*sTopRailSuppPlate) << trlTopRailSuppPlate ).
		                                        add( (*sBotRailSuppPlate) << trlBotRailSuppPlate ).
   						        add( (*sBotBotRailSuppPlate) << trlBotBotRailPlate ).
						        add( (*sTopBotCentrRailSuppPlate) << trlTopCentrRailPlate ).
						        add( (*sTopBotCentrRailSuppPlate) << trlBotCentrRailPlate ).
						        add( (*sVertRailSuppPlate) << trlVert1RailPlate ).
						        add( (*sVertRailSuppPlate) << trlVert2RailPlate );


  
  GeoLogVol*  lRailSuppStd = new GeoLogVol( "StdFeetRailSupport", 
                                            &sRailSuppStd, 
                                            theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pRailSuppStd = new GeoPhysVol(lRailSuppStd);


  //------------------------------------------------------------------------------------------
  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions of rail support centres
  double zPosStd[nZPosMax];
  for ( int i = 1; i < nFeet - 1; i++ )   zPosStd[ i - 1 ] = zPositionFeet[i];    

  // array of x positions
  double xPos[nXPosMax] = { -xMean, xMean };
  
  // fill auxiliary arrays 
  for ( int i = 0; i < nZPosMax * nXPosMax; i++ )  
  {
     int ii = i % nZPosMax, 
         jj = i / nZPosMax;
     zPosAux1[i] = zPosStd[ii];
     xPosAux[i]  = xPos[jj];
  }

  GENFUNCTION fTrlZStd = ArrayFunction( zPosAux1, zPosAux1 + nZPosMax * nXPosMax ); 
  GENFUNCTION fTrlXStd = ArrayFunction( xPosAux,  xPosAux  + nZPosMax * nXPosMax ); 
  
  TRANSFUNCTION XFRailSuppStd = Pow( HepTranslateZ3D(1.0), fTrlZStd ) * 
                                Pow( HepTranslateX3D(1.0), fTrlXStd ) * 
                                HepTranslateY3D(yMeanStd);               // const. displacement along y

  GeoSerialTransformer* sxRailSuppStd = new GeoSerialTransformer( pRailSuppStd, &XFRailSuppStd, nZPosMax * nXPosMax );
  container->add(sxRailSuppStd);

  // std::cout<<" FeetToroidBuilderRDB::buildFeetRailSupports  sxRailSuppStd  "<<std::endl;


  //------------------------------------------------------------------------------------------
  //  Build Extremity Feet Rail Supports Minus
  //------------------------------------------------------------------------------------------
   
  
  // build base shape
  //-----------------------------
  
  // middle plate
  const GeoShape* sCentralMinusRailSuppPlateExtr = new GeoBox( xCentrWidthExtr/2, yHeightCentrPlateExtr/2, zLengthCentrPlateExtr/2 );
  
  // top plate
  GeoBox* sTopRailSuppPlateExtr		  = new GeoBox( xWidth/2, thicknessTopPlate/2 - tolerance, zWidthTopPlateExtr/2 );

// two bottom plate
  GeoBox* sBotRailSuppPlateExtr		 = new GeoBox( xWidth/2, thicknessBotPlateExtr/2 - tolerance,
                                                       zWidthBotPlateExtr/2 );
  GeoBox* sBotBotRailSuppPlateExtr	 = new GeoBox( xWidth/2, thicknessBotBotExtr/2 - tolerance, 
                                                       zLengthBotBotExtr/2 );
  
  // cutout in bottom plates
  GeoBox* sBotBoxCutRailSuppPlateExtr	  = new GeoBox( radiusBotCutOutExtr/2 + tolerance, yHeightBotCutOutExtr/2, zLengthBotCutOutExtr/2 );
  GeoTube* sBotTubeCutRailSuppPlateExtr	  = new GeoTube( 0, radiusBotCutOutExtr, yHeightBotCutOutExtr/2 );
  
  // two middle plates
  GeoBox* sMidRailSuppPlateExtr		  = new GeoBox( xWidthMidPlateExtr/2 - tolerance, thicknessMidPlate/2, zWidthMidPlateExtr/2 );
  
  // cutout in middle plate
  GeoTrd* sMidCutRailSuppPlateExtr	  = new GeoTrd( zLengthMidCutExtr/2, zLengthMidCutExtr/2 - DepthMidCutExtr,
							 thicknessMidPlate, thicknessMidPlate, DepthMidCutExtr/2 + tolerance);
  
  // four Vertical plates
  GeoBox* sVertRailSuppPlateExtr	  = new GeoBox( xWidthVertPlateExtr/2, yHeightCentrPlateExtr/2 - tolerance,
							 thicknessVerPlate/2 - tolerance );

    // position of shapes' centres relative to centre of central plate
    
    //top plate
    HepTransform3D trlTopMinusRailSuppPlateExtr  = HepTranslate3D( 0, (yHeightExtr - thicknessTopPlate
						 - thicknessBotBotExtr)/2, 0 );
    
    //two bottom plates
    HepTransform3D trlBotMinusRailSuppPlateExtr  = HepTranslate3D( 0, (-yHeightExtr +
			 thicknessBotPlateExtr +thicknessBotBotExtr)/2, 0 );
    HepTransform3D trlBotBotMinusRailPlateExtr   = HepTranslate3D( 0, -yHeightExtr/2 - 
							thicknessBotBotExtr, -zoffsetBotBotExtr );
							
    // cutout from Bottom plates
    HepTransform3D trlBotBoxCutMinusRailSuppPlateExtr = HepTranslate3D( -xWidth/2 + radiusBotCutOutExtr/2, -yHeightCentrPlateExtr/2 -
								    thicknessBotPlateExtr, -zoffsetBotCutExtr );
    HepTransform3D trlBotTube1CutMinusRailSuppPlateExtr= HepTranslate3D( -xWidth/2, -yHeightCentrPlateExtr/2 - thicknessBotPlateExtr,
								    -zoffsetBotCutExtr - zLengthBotCutOutExtr/2 ) *
								    HepRotateX3D( M_PI/2 );
    HepTransform3D trlBotTube2CutMinusRailSuppPlateExtr= HepTranslate3D( -xWidth/2, -yHeightCentrPlateExtr/2 - thicknessBotPlateExtr,
								    -zoffsetBotCutExtr + zLengthBotCutOutExtr/2 ) *
								    HepRotateX3D( M_PI/2 );
    // four vertical plates
    HepTransform3D trlVert1InnerMinusRailPlateExtr    = HepTranslate3D( -xWidthVertPlateExtr/2 - xCentrWidthExtr/2,
								   0, zposVertPlateExtr - zWidthMidPlateExtr - thicknessVerPlate );
    HepTransform3D trlVert2InnerMinusRailPlateExtr    = HepTranslate3D( -xWidthVertPlateExtr/2 - xCentrWidthExtr/2,
								   0, zposVertPlateExtr );
    HepTransform3D trlVert1OuterMinusRailPlateExtr	 = HepTranslate3D( xWidthVertPlateExtr/2 + xCentrWidthExtr/2,
								   0, zposVertPlateExtr - zWidthMidPlateExtr - thicknessVerPlate );
    HepTransform3D trlVert2OuterMinusRailPlateExtr	 = HepTranslate3D( xWidthVertPlateExtr/2 + xCentrWidthExtr/2,
								   0, zposVertPlateExtr );
  
      
    // two middle plates
    HepTransform3D trlInnerMidMinusRailPlateExtr	 = HepTranslate3D( -xWidthMidPlateExtr/2 - xCentrWidthExtr/2, 0, 
								     zposVertPlateExtr - zWidthMidPlateExtr/2 - thicknessVerPlate/2 );
    HepTransform3D trlOuterMidMinusRailPlateExtr	 = HepTranslate3D( xWidthMidPlateExtr/2 + xCentrWidthExtr/2, 0,
								     zposVertPlateExtr - zWidthMidPlateExtr/2 - thicknessVerPlate/2 );
								     
    // cutout from middle plate
    HepTransform3D trlMidCutMinusRailPlateExtr	 = HepTranslate3D( -xWidthMidPlateExtr - xCentrWidthExtr/2 + DepthMidCutExtr/2, 0,
								    -zoffsetMidCutExtr ) * HepRotateY3D( M_PI/2 );
    
 //add shapes' to central plate

const GeoShape& sMinusRailSuppExtr = sCentralMinusRailSuppPlateExtr->add( (*sTopRailSuppPlateExtr) << trlTopMinusRailSuppPlateExtr ).
		                                    add( (*sBotRailSuppPlateExtr) << trlBotMinusRailSuppPlateExtr ).
   						    add( (*sBotBotRailSuppPlateExtr) << trlBotBotMinusRailPlateExtr ).
						    subtract( (*sBotBoxCutRailSuppPlateExtr) << trlBotBoxCutMinusRailSuppPlateExtr ).
						    subtract( (*sBotTubeCutRailSuppPlateExtr) << trlBotTube1CutMinusRailSuppPlateExtr ).
						    subtract( (*sBotTubeCutRailSuppPlateExtr) << trlBotTube2CutMinusRailSuppPlateExtr ).
						    add( (*sMidRailSuppPlateExtr) << trlInnerMidMinusRailPlateExtr ).
						    add( (*sMidRailSuppPlateExtr) << trlOuterMidMinusRailPlateExtr ).
						    subtract( (*sMidCutRailSuppPlateExtr) << trlMidCutMinusRailPlateExtr ).
						    add( (*sVertRailSuppPlateExtr) << trlVert1InnerMinusRailPlateExtr ).
						    add( (*sVertRailSuppPlateExtr) << trlVert2InnerMinusRailPlateExtr ).
						    add( (*sVertRailSuppPlateExtr) << trlVert1OuterMinusRailPlateExtr ).
						    add( (*sVertRailSuppPlateExtr) << trlVert2OuterMinusRailPlateExtr );




  GeoLogVol*  lMinusRailSuppExtr = new GeoLogVol( "ExtrFeetMinusRailSupport", 
                                             &sMinusRailSuppExtr, 
                                             theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pMinusRailSuppExtr = new GeoPhysVol(lMinusRailSuppExtr);


  //------------------------------------------------------------------------------------------
  // replicate and position
  //------------------------------------------------------------------------------------------

  // array of z-positions of rail support/extension centres
  const int nZPosExtr = 1;
  const int nXPosMaxExtr = 1;  
  double zPosMinus2RailSuppExtr[nZPosExtr] = { (zPositionFeet[0] + zoffsetExtr) }; 
  double zPosMinus1RailSuppExtr[nZPosExtr] = { (zPositionFeet[ nFeet - 1 ] - zoffsetExtr)};

  double xPosMinus1RailSuppExtr[nXPosMaxExtr] = { -xMeanMinusRailSuppExtr };
  double xPosMinus2RailSuppExtr[nXPosMaxExtr] = { xMeanMinusRailSuppExtr };
  
  double rotAngle[nZPosExtr] = {M_PI};
  
  // fill auxiliary arrays 
  for ( int i = 0; i < nZPosExtr; i++ )  
  {
      //int j = i / nZPosExtr;
     zPosAux1[i]    = zPosMinus2RailSuppExtr[i];
     zPosAux2[i]    = zPosMinus1RailSuppExtr[i];
     xPosAux1[i]    = xPosMinus1RailSuppExtr[i];
     xPosAux2[i]    = xPosMinus2RailSuppExtr[i];
     rotAngleAux[i] = rotAngle[i];
  }

  GENFUNCTION fRotMinusRailSuppExtr  = ArrayFunction( rotAngleAux, rotAngleAux + nZPosExtr * nXPosMaxExtr );
  GENFUNCTION fTrlZMinus2RailSuppExtr= ArrayFunction( zPosAux1,    zPosAux1    + nZPosExtr * nXPosMaxExtr ); 
  GENFUNCTION fTrlZMinus1RailSuppExtr= ArrayFunction( zPosAux2,	   zPosAux2    + nZPosExtr * nXPosMaxExtr );
  GENFUNCTION fTrlXMinus1RailSuppExtr= ArrayFunction( xPosAux1,    xPosAux1    + nZPosExtr * nXPosMaxExtr ); 
  GENFUNCTION fTrlXMinus2RailSuppExtr= ArrayFunction( xPosAux2,    xPosAux2    + nZPosExtr * nXPosMaxExtr );
  
  TRANSFUNCTION XFMinus2RailSuppExtr = Pow( HepTranslateZ3D(1.0), fTrlZMinus1RailSuppExtr ) * 
                            	      Pow( HepTranslateX3D(1.0), fTrlXMinus2RailSuppExtr ) * 
                            	      HepTranslateY3D(yMeanExtr); 			// const. displacement along y
				      
  TRANSFUNCTION XFMinus1RailSuppExtr = Pow( HepTranslateZ3D(1.0), fTrlZMinus2RailSuppExtr ) *
				       Pow( HepTranslateX3D(1.0), fTrlXMinus1RailSuppExtr ) *
				       HepTranslateY3D(yMeanExtr) *
				       Pow( HepRotateY3D(1.0), fRotMinusRailSuppExtr);  // const. displacement along y


  GeoSerialTransformer* sxMinus1RailSuppExtr = new GeoSerialTransformer( pMinusRailSuppExtr, 
                                                                   &XFMinus1RailSuppExtr,
								   1 ); 
  GeoSerialTransformer* sxMinus2RailSuppExtr = new GeoSerialTransformer( pMinusRailSuppExtr,
								   &XFMinus2RailSuppExtr,
								   1 );

  container->add(sxMinus1RailSuppExtr);
  container->add(sxMinus2RailSuppExtr);

  // std::cout<<" FeetToroidBuilderRDB::buildFeetRailSupports  sxMinus1RailSuppExtr  "<<std::endl;


 //------------------------------------------------------------------------------------------
 //  Build Extremity Feet Rail Supports Plus
 //------------------------------------------------------------------------------------------

  // build base shape
  //-----------------------------
      
  // middle plate
  const GeoShape* sCentralPlusRailSuppPlateExtr = new GeoBox( xCentrWidthExtr/2, yHeightCentrPlateExtr/2, zLengthCentrPlateExtr/2 );
									     
  // position of shapes' centres relative to centre of central plate
									     
  //top plate
  HepTransform3D trlTopPlusRailSuppPlateExtr  = HepTranslate3D( 0, (yHeightExtr - thicknessTopPlate -
						thicknessBotBotExtr)/2, 0 );
										     
  //two bottom plates
  HepTransform3D trlBotPlusRailSuppPlateExtr  = HepTranslate3D( 0, (-yHeightExtr + thicknessBotPlateExtr +
						    thicknessBotBotExtr)/2, 0 );
  HepTransform3D trlBotBotPlusRailPlateExtr   = HepTranslate3D( 0, -yHeightExtr/2 - 
				    			    thicknessBotBotExtr/2, -zoffsetBotBotExtr );

  // cutout from Bottom plates
  HepTransform3D trlBotBoxCutPlusRailSuppPlateExtr = HepTranslate3D( xWidth/2 - radiusBotCutOutExtr/2, -yHeightCentrPlateExtr/2 -
								    thicknessBotPlateExtr, -zoffsetBotCutExtr );
  HepTransform3D trlBotTube1CutPlusRailSuppPlateExtr= HepTranslate3D( xWidth/2, -yHeightCentrPlateExtr/2 - thicknessBotPlateExtr,
								    -zoffsetBotCutExtr - zLengthBotCutOutExtr/2 ) *
							        HepRotateX3D( M_PI/2 );
  HepTransform3D trlBotTube2CutPlusRailSuppPlateExtr= HepTranslate3D( xWidth/2, -yHeightCentrPlateExtr/2 - thicknessBotPlateExtr,
							        -zoffsetBotCutExtr + zLengthBotCutOutExtr/2 ) *
	    							    HepRotateX3D( M_PI/2 );
  // four vertical plates
 HepTransform3D trlVert1InnerPlusRailPlateExtr    = HepTranslate3D( -xWidthVertPlateExtr/2 - xCentrWidthExtr/2,
							       0, zposVertPlateExtr - zWidthMidPlateExtr - thicknessVerPlate );
 HepTransform3D trlVert2InnerPlusRailPlateExtr    = HepTranslate3D( -xWidthVertPlateExtr/2 - xCentrWidthExtr/2,
								   0, zposVertPlateExtr );
 HepTransform3D trlVert1OuterPlusRailPlateExtr    = HepTranslate3D( xWidthVertPlateExtr/2 + xCentrWidthExtr/2,
				    				   0, zposVertPlateExtr - zWidthMidPlateExtr - thicknessVerPlate );
 HepTransform3D trlVert2OuterPlusRailPlateExtr	 = HepTranslate3D( xWidthVertPlateExtr/2 + xCentrWidthExtr/2,
				    				   0, zposVertPlateExtr );
																																										     
																																										         
 // two middle plates
 HepTransform3D trlInnerMidPlusRailPlateExtr	 = HepTranslate3D( -xWidthMidPlateExtr/2 - xCentrWidthExtr/2, 0, 
							          zposVertPlateExtr - zWidthMidPlateExtr/2 - thicknessVerPlate/2 );
 HepTransform3D trlOuterMidPlusRailPlateExtr	 = HepTranslate3D( xWidthMidPlateExtr/2 + xCentrWidthExtr/2, 0,
					      		          zposVertPlateExtr - zWidthMidPlateExtr/2 - thicknessVerPlate/2 );
 // cutout from middle plate
 HepTransform3D trlMidCutPlusRailPlateExtr	 = HepTranslate3D( xWidthMidPlateExtr + xCentrWidthExtr/2 - DepthMidCutExtr/2, 0,
							    -zoffsetMidCutExtr ) * HepRotateY3D( -M_PI/2 );
					        
 //add shapes' to central plate
			     
const GeoShape& sPlusRailSuppExtr = sCentralPlusRailSuppPlateExtr->add( (*sTopRailSuppPlateExtr) << trlTopPlusRailSuppPlateExtr ).
	    	                                add( (*sBotRailSuppPlateExtr) << trlBotPlusRailSuppPlateExtr ).
						add( (*sBotBotRailSuppPlateExtr) << trlBotBotPlusRailPlateExtr ).
						subtract( (*sBotBoxCutRailSuppPlateExtr) << trlBotBoxCutPlusRailSuppPlateExtr ).
						subtract( (*sBotTubeCutRailSuppPlateExtr) << trlBotTube1CutPlusRailSuppPlateExtr ).
						subtract( (*sBotTubeCutRailSuppPlateExtr) << trlBotTube2CutPlusRailSuppPlateExtr ).
					        add( (*sMidRailSuppPlateExtr) << trlInnerMidPlusRailPlateExtr ).
					        add( (*sMidRailSuppPlateExtr) << trlOuterMidPlusRailPlateExtr ).
						subtract( (*sMidCutRailSuppPlateExtr) << trlMidCutPlusRailPlateExtr ).
						add( (*sVertRailSuppPlateExtr) << trlVert1InnerPlusRailPlateExtr ).
						add( (*sVertRailSuppPlateExtr) << trlVert2InnerPlusRailPlateExtr ).
						add( (*sVertRailSuppPlateExtr) << trlVert1OuterPlusRailPlateExtr ).
						add( (*sVertRailSuppPlateExtr) << trlVert2OuterPlusRailPlateExtr );

  GeoLogVol*  lPlusRailSuppExtr = new GeoLogVol( "ExtrFeetPlusRailSupport", 
                                           &sPlusRailSuppExtr, 
                                            theMaterialManager->getMaterial(feetMaterial) );
  GeoPhysVol* pPlusRailSuppExtr = new GeoPhysVol(lPlusRailSuppExtr);
											      
										      
//------------------------------------------------------------------------------------------
// replicate and position
//------------------------------------------------------------------------------------------
												    
// array of z-positions of rail support/extension centres
    const int nZPosPlusExtr = 1;
    const int nXPosPlusExtr = 1;
//    const int nXPosMaxExtr  = 1;
    double zPosPlus2RailSuppExtr[nZPosPlusExtr] = { zPositionFeet[0] + zoffsetExtr }; 
    double zPosPlus1RailSuppExtr[nZPosPlusExtr] = { zPositionFeet[ nFeet - 1 ] - zoffsetExtr };
    double xPosPlus2RailSuppExtr[nXPosMaxExtr] = { xMeanPlusRailSuppExtr };
    double xPosPlus1RailSuppExtr[nXPosMaxExtr] = { -xMeanPlusRailSuppExtr };
    double rotAnglePlus[nZPosPlusExtr] = {M_PI};
  // fill auxiliary arrays 
 for ( int i = 1; i < nZPosPlusExtr; i++ )  
 {
    int j = i / nZPosPlusExtr;
    zPosAux1[i]    = zPosPlus2RailSuppExtr[i];
    zPosAux2[i]    = zPosPlus1RailSuppExtr[i];
    xPosAux1[i]     = xPosPlus1RailSuppExtr[j];
    xPosAux2[i]     = xPosPlus2RailSuppExtr[j];
    rotAngleAux[i] = rotAnglePlus[i];
 }
 
  GENFUNCTION fRotPlusRailSuppExtr   = ArrayFunction( rotAngleAux, rotAngleAux + nZPosPlusExtr * nXPosPlusExtr );
  GENFUNCTION fTrlZPlus2RailSuppExtr = ArrayFunction( zPosAux1,    zPosAux1    + nZPosPlusExtr * nXPosPlusExtr ); 
  GENFUNCTION fTrlZPlus1RailSuppExtr = ArrayFunction( zPosAux2,    zPosAux2    + nZPosPlusExtr * nXPosPlusExtr );
  GENFUNCTION fTrlXPlus1RailSuppExtr = ArrayFunction( xPosAux1,    xPosAux1    + nZPosPlusExtr * nXPosPlusExtr ); 
  GENFUNCTION fTrlXPlus2RailSuppExtr = ArrayFunction( xPosAux2,    xPosAux2    + nZPosPlusExtr * nXPosPlusExtr );
  
  
      
  TRANSFUNCTION XFPlus2RailSuppExtr = Pow( HepTranslateZ3D(1.0), fTrlZPlus1RailSuppExtr ) * 
                            	      Pow( HepTranslateX3D(1.0), fTrlXPlus1RailSuppExtr ) * 
                                      HepTranslateY3D(yMeanExtr);                       // const. displacement along y
				
  TRANSFUNCTION XFPlus1RailSuppExtr = Pow( HepTranslateZ3D(1.0), fTrlZPlus2RailSuppExtr ) *
 				      Pow( HepTranslateX3D(1.0), fTrlXPlus2RailSuppExtr ) *
				      HepTranslateY3D(yMeanExtr) * 
				      Pow( HepRotateY3D(1.0), fRotPlusRailSuppExtr );
								
  GeoSerialTransformer* sxPlus1RailSuppExtr = new GeoSerialTransformer( pPlusRailSuppExtr, 
                                                                 &XFPlus1RailSuppExtr,
								  1 ); 
//                                                                nZPosPlusExtr * nXPosPlusExtr );
								
  GeoSerialTransformer* sxPlus2RailSuppExtr = new GeoSerialTransformer( pPlusRailSuppExtr,
								 &XFPlus2RailSuppExtr,
								  1 );
//								 nZPosPlusExtr * nXPosPlusExtr );

 container->add(sxPlus2RailSuppExtr);
 container->add(sxPlus1RailSuppExtr);
 // std::cout<<" FeetToroidBuilderRDB::buildFeetRailSupports  sxPlus2RailSuppExtr  "<<std::endl;

}

void FeetToroidBuilderRDB::buildFeetVoussoirs( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds Feet Voussoirs (i.e., 'bridges' between two feet opposite in x) 
  //   - shapes are slightly different for Standard and Extremity Feet    
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//------------ Feet General Parameters ---------------------------------//
   double zPositionFeet[maxDim];
   zPositionFeet[0] = -(*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
   zPositionFeet[1] = -(*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[2] = -(*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[3] = -(*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[4] =  (*m_Feet)[0]->getFloat("ZPOSFEE1") * mm;
   zPositionFeet[5] =  (*m_Feet)[0]->getFloat("ZPOSFEE2") * mm;
   zPositionFeet[6] =  (*m_Feet)[0]->getFloat("ZPOSFEE3") * mm;
   zPositionFeet[7] =  (*m_Feet)[0]->getFloat("ZPOSFEE4") * mm;
   zPositionFeet[8] =  (*m_Feet)[0]->getFloat("ZPOSFEE5") * mm;
//----------------------------------------------------------------------------//
   const double thicknessInnerPlateStandFeet = (*m_Feet)[0]->getFloat("UCNPLXWI") * mm;
   const double xPosVertex5StandFeet         = (*m_Feet)[0]->getFloat("MAINPLXH") * mm;
//------------------ Feet Voussoir Parameters --------------------------------//
   const double yMin                 = -654.0 * cm;
   const double yMax                 = -569.0 * cm;
   const double zWidth               = (*m_Abrt)[0]->getFloat("VOUBLZLE") * mm;
   const double zWidthStepVouss	      = (*m_Abrt)[0]->getFloat("VOUBLZLS") * mm;
   const double alphaStep		      = 15.0 * deg;
   const double xLength	      = (*m_Abrt)[0]->getFloat("VOUSBLYW") * mm;
   const double thicknessStepVouss     = (*m_Abrt)[0]->getFloat("VOURECSL") * mm;
   const double xPosFoot		      = (*m_Feet)[0]->getFloat("STDFOOXP") * mm;
   //const double yPosFoot		      = (*m_Feet)[0]->getFloat("STDFOOYP") * mm;
   const double distInnerRib    = 20.0 + (*m_Abrt)[0]->getFloat("VOUCRCYR") * mm;
   const double distHoleCentre  = (*m_Abrt)[0]->getFloat("VOUSCYPO") * mm; //??
   const double diameterHoleVoussFeet         = 2.0 * (*m_Abrt)[0]->getFloat("VOURCUTR") * mm;
   const double thicknessTop    = (*m_Abrt)[0]->getFloat("VOUSPLOX") * mm; //???
   const double thicknessCent   = (*m_Abrt)[0]->getFloat("VOUBZWTH") * mm;
   const double thicknessBot    = (*m_Abrt)[0]->getFloat("VOUSPLIX") * mm; //???
   const double thicknessRib         = (*m_Abrt)[0]->getFloat("VOURPYWI") * mm;
   const double thicknessOutRibStd = (*m_Abrt)[0]->getFloat("VOUBLYWS") * mm;
   //const double ThetaAngleVoussFeet	      = 15.0 * deg;

// for Voussoir on Standard Feet

   const double xWidthConnVoussFeet	        = (*m_Feet)[0]->getFloat("CNFEVOXW") * mm;
   const double xWidthConnVoussFeetVertPlate    = (*m_Feet)[0]->getFloat("CNFEVOXT") * mm;
   const double yHeightConnVoussFeetPlate       = (*m_Feet)[0]->getFloat("CNFEVOYH") * mm;
   const double yHeightConnVoussFeetHorPlate    = (*m_Feet)[0]->getFloat("CNFEVOYT") * mm;
   const double zLenthConnVoussFeetCentrPlate   = (*m_Feet)[0]->getFloat("CNFEVOZT") * mm;
   
   const std::string voussMaterial = getMaterial( "Alum" ); 
//----------------------------------------------------------------------------//

  const double tolerance = 0.5*mm;
  
  // auxiliary array to store z for all volumes to be replicated 
  const int nZPosMax = maxDim;
  const int nZPosConnMax = maxDim - 2;
  const int nXPosMax = 2;
  double* zPosAux;
  double zPosAux1[nZPosConnMax * nXPosMax], xPosAux[nZPosConnMax * nXPosMax]/*, zPosAux2[nZPosMax * nXPosMax], xPosAux1[nZPosMax * nXPosMax]*/;
  
  //------------------------------------------------------------------------------------------
  // common Feet/Voussoir properties 
  const double yHeight = yMax - yMin;            // total y-height  
  const double yHeightCent   = yHeight - thicknessTop - thicknessBot;  // height of central plate
  const double radiusHole    = 0.5 * diameterHoleVoussFeet;  // radius of cutout hole
    
  // positioning variables
  const double yMean          = 0.5 * ( yMax + yMin );  
  
  //------------------------------------------------------------------------------------------
  // Feet Voussoir properties
  const double xLengthStd         = 2 * ( xPosVertex5StandFeet - thicknessInnerPlateStandFeet - xWidthConnVoussFeet);  // total length
  const double xLengthCentStd     = xLength - 2 * thicknessOutRibStd;  // length of central plate

  // positioning variables
  const double distOutRibStd    = 0.5 * ( xLength - 2 * thicknessOutRibStd - thicknessStepVouss);
  const double distOutRib  = 0.5 * ( xLength - thicknessOutRibStd - thicknessStepVouss );
  
  // Standard Conn Feet Voussoir properties
  const double xLengthConnStd		= xWidthConnVoussFeet;			// total length
  const double thicknessVertConnStd	= xWidthConnVoussFeetVertPlate;		// thickness of Vertical plate
  const double yHeightVertConnStd	= yHeightConnVoussFeetPlate;		// total height 
  const double thicknessHorConnStd	= yHeightConnVoussFeetHorPlate;		// thickness of top and bottom plates
  const double thicknessCentrConnStd	= zLenthConnVoussFeetCentrPlate;	// thickness of central plate
  const double yHeightCentrConnStd	= yHeightVertConnStd - 2 * thicknessHorConnStd;	//height of central plate
  
  // positioning variables of Conn Feet Plates
  const double distCentrConnStd		= 0.5 * ( xLengthStd + xLengthConnStd);
  //const double distInnerVertConnStd	= 0.5 * ( xLengthStd + thicknessVertConnStd);
  //const double distOuterVertConnStd	= 0.5 * ( xLengthStd + 2 * xLengthConnStd - thicknessVertConnStd);
    
//------------------------------------------------------------------------------------------
  //  Build Standard and Extremity Feet Voussoir (Central part)
  //------------------------------------------------------------------------------------------
  
  //------------------------------------------------------------------------------------------
  // build base shape
  //------------------------------------------------------------------------------------------
  // central plate
  const GeoShape* sCentPlateStd = new GeoBox( xLengthCentStd/2 - thicknessStepVouss, yHeightCent/2, thicknessCent/2 );

  // top plate
  GeoBox* sTopPlateStd = new GeoBox( xLength/2 - tolerance, thicknessTop/2 - tolerance, zWidth/2 );
  
  // bottom plate
  GeoBox* sBotPlateStd = new GeoBox(xLength/2 - tolerance, thicknessBot/2 - tolerance, zWidth/2 );
  
  //empty trapezoids  from top and bottom plates. 1-st trapezoid
  double xTopmin	= xLengthCentStd/2 - zWidthStepVouss/tan(alphaStep);
  double xTopmax	= xLengthCentStd/2;
  double yTopVouss	= yHeight;
  double zTopVouss	= zWidthStepVouss/2 + tolerance;
  GeoTrd* sEmptyTrap1   = new GeoTrd( xTopmin, xTopmax, yTopVouss, yTopVouss, zTopVouss );
  
  //empty trapezoid from top and bottom plates. 2-nd trapezoid
  double xBotmin	= xTopmax;
  double xBotmax	= xTopmin;
  double yBotVouss	= yHeight;
  double zBotVouss	= zTopVouss;
  GeoTrd* sEmptyTrap2   = new GeoTrd( xBotmin, xBotmax, yBotVouss, yBotVouss, zBotVouss );
  
  // inner rib pairs
  GeoBox* sInnerRib = new GeoBox( thicknessRib/2, yHeightCent/2 - thicknessStepVouss - tolerance, 
				    zWidth/2 - zWidthStepVouss);  
  
  // outmost rib pair
  GeoBox* sOuterRibStd = new GeoBox( thicknessOutRibStd/2 + thicknessStepVouss/2 - tolerance, yHeightCent/2,
					 zWidth/2 ); 
 
  //empty trapezoids at outer rib
  double xOutmin	= yHeightCent/2 - thicknessStepVouss;
  double xOutmax	= yHeightCent/2;
  double yOutVoussmax	= zWidth/2 - thicknessCent/2;
  // double yOutVoussmin	= yOutVoussmax * (zWidthStepVouss - thicknessStepVouss * tan(alphaStep) )/zWidthStepVouss;
  double zOutVouss	= thicknessStepVouss/2 + tolerance;
  
  GeoTrd* sEmptyOuterRibTrap	= new GeoTrd( yOutVoussmax, yOutVoussmax, xOutmin, xOutmax, zOutVouss );
  
  // hole shape
  GeoTube* sHole = new GeoTube( 0, radiusHole, thicknessCent );
  
  //------------------------------------------------------------------------------------------
  // positions of shapes' centres relative to centre of central plate    
  HepTransform3D trlInnerRib[2];   // two inner rib pairs
  trlInnerRib[0] = HepTranslateX3D( -distInnerRib );
  trlInnerRib[1] = HepTranslateX3D( distInnerRib );

  
  HepTransform3D trlOutRibStd[2];   // outmost rib pair
  trlOutRibStd[0] = HepTranslateX3D( -distOutRib );
  trlOutRibStd[1] = HepTranslateX3D( distOutRib );
  
  HepTransform3D trlHole[2];   // hole pair
  trlHole[0] = HepTranslateX3D( -distHoleCentre );
  trlHole[1] = HepTranslateX3D( distHoleCentre );
   
  //position of empty shapes' centres relative to centre of central plate
  
  HepTransform3D trlEmptyOutRibTrap[4];
  trlEmptyOutRibTrap[0] = HepTranslate3D( distOutRibStd, 0, zWidth/2 + tolerance/2 ) * HepRotateY3D(3*M_PI/2);
  trlEmptyOutRibTrap[1] = HepTranslate3D( -distOutRibStd, 0, zWidth/2 + tolerance/2 ) * HepRotateY3D(M_PI/2);
  trlEmptyOutRibTrap[2] = HepTranslate3D( -distOutRibStd, 0, -zWidth/2 - tolerance/2 ) * HepRotateY3D(M_PI/2);
  trlEmptyOutRibTrap[3] = HepTranslate3D( distOutRibStd, 0, -zWidth/2 - tolerance/2 ) * HepRotateY3D(3*M_PI/2);
    
    // position of top/bottom plates and two empty trpezoids
    HepTransform3D trlTopPlate	    = HepTranslate3D( 0, yHeightCent/2 + thicknessTop/2, 0 );
    HepTransform3D trlBotPlate	    = HepTranslate3D( 0, -yHeightCent/2 - thicknessBot/2, 0 );
    
    HepTransform3D trlEmptyTrap1 = HepTranslate3D( 0, 0, zWidth/2 - zWidthStepVouss/2 );
    HepTransform3D trlEmptyTrap2 = HepTranslate3D( 0, 0, -zWidth/2 + zWidthStepVouss/2 );

  //------------------------------------------------------------------------------------------
  // add shapes to central plate, subtract holes
  
  // add inner ribs 
  for ( int i = 0; i < 2; i++ )
  {
     sCentPlateStd = &( sCentPlateStd->add( (*sInnerRib) << trlInnerRib[i] ) ); 
  }
  // add outer rib pair
  for ( int i = 0; i < 2; i++ )
  {
     sCentPlateStd = &( sCentPlateStd->add( (*sOuterRibStd) << trlOutRibStd[i] ) );
  } 
  // subtract holes
  for ( int i = 0; i < 2; i++ )
  {
     sCentPlateStd = &( sCentPlateStd->subtract( (*sHole) << trlHole[i] ) );
  } 
  
  //subtract shapes from outer rib
 
  for ( int i = 0; i < 4; i++ )
  {
     sCentPlateStd = &( sCentPlateStd->subtract( (*sEmptyOuterRibTrap) << trlEmptyOutRibTrap[i] ) );
  } 
  
  // add all top/bottom plates
  const GeoShape& sVoussoirStd = sCentPlateStd->add( (*sTopPlateStd) << trlTopPlate ).
						add( (*sBotPlateStd) << trlBotPlate ).
						subtract( (*sEmptyTrap1) << trlEmptyTrap1 ).
						subtract( (*sEmptyTrap2) << trlEmptyTrap2 );
  
  
  GeoLogVol*  lVoussoirStd = new GeoLogVol( "StdFeetVoussoir", 
                                            &sVoussoirStd, 
                                            theMaterialManager->getMaterial(voussMaterial) );
  GeoPhysVol* pVoussoirStd = new GeoPhysVol(lVoussoirStd);


  //------------------------------------------------------------------------------------------
  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions of Standard Feet Voussoir centres 
  double zPosStd[nZPosMax];
  for ( int i = 0; i < nFeet; i++ )  { zPosStd[ i ] = zPositionFeet[i];}


  // fill auxiliary array 
  zPosAux = zPosStd;

  GENFUNCTION fTrlZStd = ArrayFunction( zPosAux, zPosAux + nZPosMax ); 
  
  TRANSFUNCTION XFVoussoirStd = Pow( HepTranslateZ3D(1.0), fTrlZStd ) * 
                                HepTranslateY3D(yMean);                  // const. displacement along y

  GeoSerialTransformer* sxVoussoirStd = new GeoSerialTransformer( pVoussoirStd, &XFVoussoirStd, nZPosMax );
  container->add(sxVoussoirStd);
  // std::cout<<" FeetToroidBuilderRDB::buildFeetVoussoirs  sxVoussoirStd "<<std::endl;

  //------------------------------------------------------------------------------------------
  // Build Conn Feet Voussoirs placed at the edge (by X) of Standard Feet Voussoir
  //------------------------------------------------------------------------------------------
  

  //------------------------------------------------------------------------------------------
  // build base shape
  //------------------------------------------------------------------------------------------
    const GeoShape* sCentrConnPlateStd = new GeoBox( xLengthConnStd/2 - thicknessVertConnStd - tolerance, yHeightCentrConnStd/2 - tolerance,
						 thicknessCentrConnStd/2 );
    
    // top plate
    GeoBox* sTopConnPlateStd = new GeoBox( xLengthConnStd/2 - thicknessVertConnStd - tolerance, thicknessHorConnStd/2 - tolerance, zWidth/2 );
    
    // bottom plate
    GeoBox* sBotConnPlateStd = new GeoBox( xLengthConnStd/2 - thicknessVertConnStd - tolerance, thicknessHorConnStd/2 - tolerance, zWidth/2 );
    
    // vertical plates
    GeoBox* sVertConnPlateStd	= new GeoBox( thicknessVertConnStd/2 - tolerance, yHeightVertConnStd/2, zWidth/2 );

    HepTransform3D trlTopConnPlate = HepTranslate3D( 0, yHeightCentrConnStd/2 + thicknessHorConnStd/2, 0 );
    HepTransform3D trlBotConnPlate = HepTranslate3D( 0, -yHeightCentrConnStd/2 - thicknessHorConnStd/2, 0 ); 
    
    // two inner and two outer Vertical Conn Plates
    HepTransform3D trlVertConnPlateStd[4];
    trlVertConnPlateStd[0] = HepTranslate3D( -xLengthConnStd/2 + thicknessVertConnStd/2, 0, 0 );
    trlVertConnPlateStd[1] = HepTranslate3D( xLengthConnStd/2 - thicknessVertConnStd/2, 0, 0 );

    
  // add top/bottom/vertical plates to the central conn plate
  const GeoShape& sConnVoussoirStd = sCentrConnPlateStd->add( (*sTopConnPlateStd) << trlTopConnPlate ).
						         add( (*sBotConnPlateStd) << trlBotConnPlate ).
    						         add( (*sVertConnPlateStd) << trlVertConnPlateStd[0] ).
						         add( (*sVertConnPlateStd) << trlVertConnPlateStd[1] );
  
  GeoLogVol*  lConnVoussoirStd = new GeoLogVol( "ConnFeetVoussoir", 
                                             &sConnVoussoirStd, 
                                             theMaterialManager->getMaterial(voussMaterial) );
  GeoPhysVol* pConnVoussoirStd = new GeoPhysVol(lConnVoussoirStd);

  //------------------------------------------------------------------------------------------
  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions of Standard Feet Voussoir centres 
  double zPosConn[nZPosConnMax];
  for (int i = 1; i < nFeet - 1; i++ ) {zPosConn[i-1] = zPositionFeet[i];}
    //array of x position
  const double xMeanConn = distCentrConnStd + xPosFoot;
  double xPosConn[nXPosMax] = {-xMeanConn, xMeanConn};


  // fill auxiliary arrays
  for ( int i = 0;  i < nZPosConnMax * nXPosMax; i++ )
  {
    int ii = i % nZPosConnMax,
        jj = i / nZPosConnMax;
    zPosAux1[i] = zPosConn[ii];
    xPosAux[i] = xPosConn[jj];
  }

  GENFUNCTION fTrlZConn = ArrayFunction( zPosAux1, zPosAux1 + nZPosConnMax  * nXPosMax); 
  GENFUNCTION fTrlXConn = ArrayFunction( xPosAux, xPosAux + nZPosConnMax * nXPosMax);
  
  double yMeanConn = yMean + (thicknessTop - thicknessBot)/2;
  TRANSFUNCTION XFConnVoussoirStd = Pow( HepTranslateZ3D(1.0), fTrlZConn ) * 
				    Pow( HepTranslateX3D(1.0), fTrlXConn ) *
                                    HepTranslateY3D(yMeanConn);                  // const. displacement along y

  GeoSerialTransformer* sxConnVoussoirStd = new GeoSerialTransformer( pConnVoussoirStd, &XFConnVoussoirStd, nZPosConnMax * nXPosMax );
  container->add(sxConnVoussoirStd);
  // std::cout<<" FeetToroidBuilderRDB::buildFeetVoussoirs  sxConnVoussoirStd "<<std::endl;

//   /* ss 12/02/2007 - valgrind complains: this goes to the end and, eventually, the volume built is not added to the container
//   //-----------------------------------------------------------------------------------------------------------------------
//   // Build Trapezoidal elements of inner rib
//   //-----------------------------------------------------------------------------------------------------------------------
  
//     //trapezoids at top and bottom edge of inner rib
//     double xInnermin	= thicknessRib/2;
//     double xInnermax	= thicknessRib/2 + thicknessStepVouss;
//     double yInnerVouss	= zWidth/2 - zWidthStepVouss;
//     double zInnerVouss	= thicknessStepVouss/2;
    
//     const GeoShape* sTopInnerRibTrap = new GeoTrd( xInnermin, xInnermax, yInnerVouss, yInnerVouss, zInnerVouss );
    
//     GeoTrd* sBotInnerRibTrap = new GeoTrd( xInnermax, xInnermin, yInnerVouss, yInnerVouss, zInnerVouss );

//     HepTransform3D trlBot2TopInnerRibTrap = HepTranslate3D( 0, 0, -yHeightCent + thicknessStepVouss );
    
//     const GeoShape& sTrapElementInnerRibVoussoir = sTopInnerRibTrap -> add( (*sBotInnerRibTrap) << trlBot2TopInnerRibTrap );


//     GeoLogVol*  lTrapElementInnerRibVoussoir = new GeoLogVol( "TrapElementInnerRibVoussoir", 
//                                              &sTrapElementInnerRibVoussoir, 
//                                              theMaterialManager->getMaterial(voussMaterial) );
//     GeoPhysVol* pTrapElementInnerRibVoussoir = new GeoPhysVol(lTrapElementInnerRibVoussoir);
    
//   //----------------------------------------------
//   // replication and position
//   //----------------------------------------------
//   // array of z-positions of Standard Feet Voussoir centres 
//     double zPosInnerRib[nZPosMax];
//     for (int i = 0; i < nFeet; i++ ) {zPosInnerRib[i] = zPositionFeet[i + 1];}
//     //array of x position
//     const double xMeanInnerRib = distInnerRib;
//     double xPosInnerRib[nXPosMax] = {-xMeanInnerRib, xMeanInnerRib};
    
//     // fill auxiliary arrays
//     for ( int i = 0;  i < nZPosMax * nXPosMax; i++ )
//     {
//         int ii = i % nZPosMax,
//             jj = i / nZPosMax;
//         zPosAux2[i] = zPosInnerRib[ii];
//         xPosAux1[i] = xPosInnerRib[jj];
//     }
    
//     GENFUNCTION fTrlZInnerRib = ArrayFunction( zPosAux2, zPosAux2 + nZPosMax  * nXPosMax); 
//     GENFUNCTION fTrlXInnerRib = ArrayFunction( xPosAux1, xPosAux1 + nZPosMax * nXPosMax);
    
//     double yMeanInnerRib = yMean + (yHeightCent - thicknessStepVouss)/2;
//     TRANSFUNCTION XFTrapElementInnerRibVoussoir = Pow( HepTranslateZ3D(1.0), fTrlZInnerRib ) * 
//                                                   Pow( HepTranslateX3D(1.0), fTrlXInnerRib ) *
//                                                   HepTranslateY3D(yMeanInnerRib)  * // const. displacement along y
//                                                   HepRotateX3D(3*M_PI/2);
    
//     GeoSerialTransformer* sxTrapElementInnerRibVoussoir = new GeoSerialTransformer( 
//         pTrapElementInnerRibVoussoir,
//         &XFTrapElementInnerRibVoussoir, nZPosMax * nXPosMax );
    
//     // container->add(sxTrapElementInnerRibVoussoir);
//     // std::cout<<" FeetToroidBuilderRDB::buildFeetVoussoirs  sxTrapElementInnerRibVoussoir "<<std::endl;
//     */
}



std::string FeetToroidBuilderRDB::getMaterial( std::string materialName ) 
{
  MsgStream log(Athena::getMessageSvc(), "MuonGeoModel");
  if ( materialName == "Alum" )
  {
    return "std::Aluminium";
  }
  else if ( materialName == "Iron" )  
  {
    return "std::Iron";
  }  
  else if ( materialName == "Fe50"  ||  materialName == "Al67" )   
  {
    return "toro::" + materialName;
  }
  else 
  {
    log  <<  " FeetToroidBuilderRDBn::getMaterial: material "  <<  materialName  <<  " not defined! "  
         <<  " Take Aluminium instead." 
         <<  endreq;
    return "std::Aluminium";           
  }  
}  


} // namespace MuonGM

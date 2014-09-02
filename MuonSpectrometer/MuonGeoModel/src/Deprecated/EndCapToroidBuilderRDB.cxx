/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/EndCapToroidBuilderRDB.h"
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
#include "GeoModelKernel/GeoShapeIntersection.h"
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
EndCapToroidBuilderRDB::EndCapToroidBuilderRDB( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
				    std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
    //   std::cout<<" EndCapToroidBuilderRDB constructor "<<std::endl;
   
  MsgStream log( Athena::getMessageSvc(), "MuGM:EndCapToroBuildRDB" );
  log  <<  MSG::INFO  <<  "Fetching data with tag <"  <<  geoTag <<  ">"  <<  endreq;
  
  m_Abrt = pRDBAccess->getRecordsetPtr("ABRT", geoTag, geoNode);
  if (m_Abrt->size() == 0) {
      log<<MSG::WARNING<<"Table ABRT not found in tag <"  <<  geoTag <<  ">"  <<" reading table ABRT-00" <<endreq;
      m_Abrt = pRDBAccess->getRecordsetPtr("ABRT","ABRT-00");
  }
  m_Aect = pRDBAccess->getRecordsetPtr("AECT", geoTag, geoNode);
  if (m_Aect->size() == 0) {
      log<<MSG::WARNING<<"Table ABRT not found in tag <"  <<  geoTag <<  ">"  <<" reading table AECT-00" <<endreq;
      m_Aect = pRDBAccess->getRecordsetPtr("AECT","AECT-00");
  }
  m_Feet = pRDBAccess->getRecordsetPtr("FEET", geoTag, geoNode);
  if (m_Feet->size() == 0) {
      log<<MSG::WARNING<<"Table FEET not found in tag <"  <<  geoTag <<  ">"  <<" reading table FEET-00" <<endreq;
      m_Feet = pRDBAccess->getRecordsetPtr("FEET","FEET-00");
  }
  m_Rail = pRDBAccess->getRecordsetPtr("RAIL", geoTag, geoNode);
  if (m_Rail->size() == 0) {
      log<<MSG::WARNING<<"Table RAIL not found in tag <"  <<  geoTag <<  ">"  <<" reading table RAIL-00" <<endreq;
      m_Rail = pRDBAccess->getRecordsetPtr("RAIL","RAIL-00");
  }
  m_Ecst = pRDBAccess->getRecordsetPtr("ECST", geoTag, geoNode);
  if (m_Ecst->size() == 0) {
      log<<MSG::WARNING<<"Table ECST not found in tag <"  <<  geoTag <<  ">"  <<" reading table ECST-00" <<endreq;
      m_Ecst = pRDBAccess->getRecordsetPtr("ECST","ECST-00");
  }

   std::string Iron = "Iron";
   std::string Aluminium = "Aluminium";


}

 void EndCapToroidBuilderRDB::buildECTVacuumVessel( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds ECT Vacuum Vessel (end-plates, wall segments, staytubes)
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

   const double zMinECTVessel           = (*m_Aect)[0]->getFloat("CRYOZMIN") * mm;                
   const double zDeltaECTVessel         = (*m_Aect)[0]->getFloat("CRYOZEXT") * mm;                
   const double rMinECTVessel           = (*m_Aect)[0]->getFloat("CRYOR0")   * mm;                
   const double deltaYToRailECTVessel   = (*m_Aect)[0]->getFloat("CRYOFLTY") * mm;                
   const double outerThicknessECTVessel = (*m_Aect)[0]->getFloat("CRYOTHI3") * mm;                
   const double innerThicknessECTVessel = (*m_Aect)[0]->getFloat("CRYOTHI2") * mm;                
   const double zThicknessECTVessel     = (*m_Aect)[0]->getFloat("CRYOTHI1") * mm;                
   const double rDistInnerCastECTVessel = (*m_Aect)[0]->getFloat("CRYOT2")   * mm;                
   const double rDistOuterCastECTVessel = (*m_Aect)[0]->getFloat("CRYOT1")   * mm;                
   const double widthInnerCastECTVessel = (*m_Aect)[0]->getFloat("CRYOS2")   * mm;                
   const double widthOuterCastECTVessel = (*m_Aect)[0]->getFloat("CRYOS1")   * mm;                
   const double rDistStaytubeECTVessel  = (*m_Aect)[0]->getFloat("CRYOTTU0") * mm;                
   const double rInStaytubeECTVessel    = (*m_Aect)[0]->getFloat("CRYORTU0") * mm;                
   const double rOutStaytubeECTVessel   = (*m_Aect)[0]->getFloat("CRYORTU1") * mm;                
   const double angleStaytubeECTVessel  = (*m_Aect)[0]->getFloat("CRYODPHT")   * deg;             
   const double radiusHoleECTVessel     = (*m_Aect)[0]->getFloat("CRYORTU1") * mm;                
   // const double lengthVertexCutECTVessel= (*m_Aect)[0]->getFloat("CRYOEDGE") * mm;		  
                                                                              //
   const std::string vesselMaterial = getMaterial(" Aluminium "); 
//----------------------------------------------------------------------------//
  
  
  const double phi0    = M_PI/8;
  const double cosPhi0 = cos(phi0);
  //const double sinPhi0 = sin(phi0);
  const double tanPhi0 = tan(phi0);
  
  // auxiliary arrays to store phi values, z positions for all volumes to be replicated 
  const int nPhiSectors = 8;
  const int nPosMax     = 4;
  double phiAux[ nPhiSectors * nPosMax ],  posAux[ nPhiSectors * nPosMax ];

  
  // Vacuum Vessel properties 
  const double zMin              = zMinECTVessel;
  const double zMax              = zMinECTVessel + zDeltaECTVessel;
  const double rMin              = rMinECTVessel;
  const double deltaZ            = zMax - zMin;
  const double zMean             = 0.5 * ( zMin + zMax );
  const double deltaYToRail      = deltaYToRailECTVessel;
  const double thicknessEndplate = zThicknessECTVessel;
  const double rOuterCast        = rDistOuterCastECTVessel;
  const double rInnerCast        = rDistInnerCastECTVessel;
  const double widthOuterCast    = widthOuterCastECTVessel;
  const double widthInnerCast    = widthInnerCastECTVessel;
  const double rStaytubeHole     = radiusHoleECTVessel;
  const double distStaytube      = rDistStaytubeECTVessel;
  const double angleStaytube     = phi0 + angleStaytubeECTVessel;
  const double rInStaytube       = rInStaytubeECTVessel;
  const double rOutStaytube      = rOutStaytubeECTVessel;
  const double thicknessWall     = outerThicknessECTVessel;
  const double thicknessCentTube = innerThicknessECTVessel;
  //  const double lengthVertCut	 = lengthVertexCutECTVessel;

  // Calculate angle (=alpha) between top and side plates of outer castellation
  double ll1   = widthOuterCast/2;
  double ll2   = rOuterCast - rInnerCast/cosPhi0;
  double l1    = sqrt( ll1*ll1 + ll2*ll2 );
  double l2    = rInnerCast * tanPhi0 -  widthInnerCast/2;
  double beta1 = atan( ll1/ll2 );
  double beta2 = M_PI/2 - beta1;
  double beta3 = beta2 + phi0;
  double l3    = sqrt( l1 * l1  +  l2 * l2  -  2 * l1 * l2 * cos(beta3) );  // outside length of 
                                                                            // side plate
  const double alpha = beta2 + asin( sin(beta3) * l2/l3 );

  //------------------------------------------------------------------------------------------
  //  Build endplate standard segments
  //  There are two segments (where the ECT rests on the rails) which have an extra 
  //  horizontal cut-out and have to be treated separately (see below) 
  //------------------------------------------------------------------------------------------

  //----------------------------------------------------------------------------------------
  // start from octogonal segment (opening angle = 45 deg)
  GeoPgon* sEndplatePgon = new GeoPgon( -phi0, 2 * phi0, 1 );
  sEndplatePgon->addPlane( -thicknessEndplate/2, rMin/2, rOuterCast/cosPhi0 );
  sEndplatePgon->addPlane(  thicknessEndplate/2, rMin/2, rOuterCast/cosPhi0 );

  // empty trapezoid to cut out the space between outer castellations
  double pDzOuterTrap  = rOuterCast/cosPhi0 - rInnerCast;   
  double pDy           = deltaZ/2;
  double pDx3OuterTrap = widthInnerCast/2;
  double pDx1OuterTrap = pDx3OuterTrap + 2 * pDzOuterTrap * tan( alpha + phi0 - M_PI/2 );
  
  GeoTrd* sEmptyOuterTrap = new GeoTrd( pDx1OuterTrap, pDx3OuterTrap, pDy, pDy, pDzOuterTrap );

  // empty tube to cut out the central tube
  GeoTube* sEmptyCentTube = new GeoTube( 0, rMin, thicknessEndplate ); 

  // tube to cut out the staytube
  GeoTube* sEmptyStaytube = new GeoTube( 0, rStaytubeHole, thicknessEndplate );
  
  // box to cut out vetex of outer castellation
//  GeoBox* sEmptyBoxOutCast = new GeoBox( lengthVertCut/2, lengthVertCut, deltaZ/2 );
  

  //----------------------------------------------------------------------------------------
  // cut out trapezoids, central tube, and staytube hole
  
  // trapezoid positioning wrt. endplate segment (trapezoid axes don't coincide with polygon ones)
  double xPosTrap = rOuterCast;
  double yPosTrap = rOuterCast * tanPhi0;

  HepTransform3D xfLeftTrap  = HepTranslate3D( xPosTrap,  yPosTrap, 0 ) * 
                               HepRotateZ3D(  phi0 - M_PI/2 ) * HepRotateX3D( M_PI/2 );
  HepTransform3D xfRightTrap = HepTranslate3D( xPosTrap, -yPosTrap, 0 ) * 
                               HepRotateZ3D( -phi0 - M_PI/2 ) * HepRotateX3D( M_PI/2 );                               
   
  // staytube positioning wrt. endplate segment
  HepTransform3D xfStaytubeHole = HepTranslate3D( distStaytube * cos(angleStaytube),
                                                  distStaytube * sin(angleStaytube),
                                                  0 );
  // empty box position wrt. endplate segment
//  HepTransform3D xfLeftEmptyBoxOutCast = HepTranslate3D( rOuterCast, widthOuterCast/2, 0 ) * HepRotateZ3D( M_PI/4 );
  
//  HepTransform3D xfRightEmptyBoxOutCast = HepTranslate3D( rOuterCast, -widthOuterCast/2, 0 ) * HepRotateZ3D( -M_PI/4 );

                                
  const GeoShape& sEndplateStdSegment = sEndplatePgon->subtract( (*sEmptyStaytube)  << xfStaytubeHole ).
                                                       subtract(  *sEmptyCentTube ).
//						       subtract( (*sEmptyBoxOutCast ) << xfLeftEmptyBoxOutCast ).
//						       subtract( (*sEmptyBoxOutCast ) << xfRightEmptyBoxOutCast ).
                                                       subtract( (*sEmptyOuterTrap) << xfLeftTrap ).
                                                       subtract( (*sEmptyOuterTrap) << xfRightTrap );

  GeoLogVol*  lEndplateStdSegment = new GeoLogVol( "ECTEndplateStdSegment", 
                                                   &sEndplateStdSegment, 
                                                   theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pEndplateStdSegment = new GeoPhysVol(lEndplateStdSegment);


  //------------------------------------------------------------------------------------------
  // Replicate and position
  
  // array of phi values
  const int nStdSectors = 6;
  double phiStdSector[nStdSectors];
  for ( int i = 0; i < nStdSectors - 1; i++ )  phiStdSector[i] = 2 * i * phi0;  
  phiStdSector[ nStdSectors - 1 ] = 12 * phi0; 
  
  // array of z positions
  const int nZPosEndplate = 4;
  double zPosEndplate[nZPosEndplate] = { -zMean - deltaZ/2 + thicknessEndplate/2, 
                                         -zMean + deltaZ/2 - thicknessEndplate/2,
                                          zMean - deltaZ/2 + thicknessEndplate/2,
                                          zMean + deltaZ/2 - thicknessEndplate/2 };
  // fill auxiliary arrays
  for ( int i = 0; i < nStdSectors * nZPosEndplate; i++ )  
  {
     int ii = i % nStdSectors, 
         jj = i / nStdSectors;
     phiAux[i] = phiStdSector[ii]; 
     posAux[i] = zPosEndplate[jj];
  }

  GENFUNCTION fRotStd = ArrayFunction( phiAux, phiAux + nStdSectors * nZPosEndplate ); 
  GENFUNCTION fTrlStd = ArrayFunction( posAux, posAux + nStdSectors * nZPosEndplate ); 
  
  TRANSFUNCTION XFEndplateStdSegment = Pow( HepRotateZ3D(1.0), fRotStd ) * Pow( HepTranslateZ3D(1.0), fTrlStd ); 

  GeoSerialTransformer* sxEndplateStdSegment = new GeoSerialTransformer( pEndplateStdSegment, 
                                                                         &XFEndplateStdSegment, 
                                                                         nStdSectors * nZPosEndplate );
  container->add(sxEndplateStdSegment);
  
  //------------------------------------------------------------------------------------------
  //  Build endplate segments resting on rails ("segment6", "segment8"), which have extra 
  //  horizontal cut-outs wrt. standard segments
  //------------------------------------------------------------------------------------------

  // box to cut out the extra space for the rail support 
  // (will be used later also for the wall segments)
  double cutLengthOut = widthOuterCast/2 - sqrt(2) * deltaYToRail + rOuterCast; 
  double dXRailBox = cutLengthOut / sqrt(2);
  double dYRailBox = widthOuterCast;     // some 'large enough' value
  double dZRailBox = deltaZ/2;           // some 'large enough' value
  
  GeoBox* sEmptyRailBox = new GeoBox( dXRailBox, dYRailBox, dZRailBox );

  // box positioning wrt. endplate segment
  HepTransform3D xfRailBox6 = HepTranslate3D( rOuterCast,  widthOuterCast/2, 0 ) * HepRotateZ3D(  M_PI/4 );
  HepTransform3D xfRailBox8 = HepTranslate3D( rOuterCast, -widthOuterCast/2, 0 ) * HepRotateZ3D( -M_PI/4 );
  
  
  //----------------------------------------------------------------------------------------
  // cut out trapezoids, central tube, staytube hole, and space for rail support 
  const GeoShape& sEndplateRailSegment6 = sEndplatePgon->subtract( (*sEmptyStaytube)  << xfStaytubeHole ). 
                                                         subtract(  *sEmptyCentTube ).
                                                         subtract( (*sEmptyOuterTrap) << xfLeftTrap ).
                                                         subtract( (*sEmptyOuterTrap) << xfRightTrap ).
                                                         subtract( (*sEmptyRailBox)   << xfRailBox6 );
  const GeoShape& sEndplateRailSegment8 = sEndplatePgon->subtract( (*sEmptyStaytube)  << xfStaytubeHole ). 
                                                         subtract(  *sEmptyCentTube ).
                                                         subtract( (*sEmptyOuterTrap) << xfLeftTrap ).
                                                         subtract( (*sEmptyOuterTrap) << xfRightTrap ).
                                                         subtract( (*sEmptyRailBox)   << xfRailBox8 );

  GeoLogVol*  lEndplateRailSegment6 = new GeoLogVol( "ECTEndplateRailSegment6", 
                                                     &sEndplateRailSegment6, 
                                                     theMaterialManager->getMaterial(vesselMaterial) );
  GeoLogVol*  lEndplateRailSegment8 = new GeoLogVol( "ECTEndplateRailSegment8", 
                                                     &sEndplateRailSegment8, 
                                                     theMaterialManager->getMaterial(vesselMaterial) );

  GeoPhysVol* pEndplateRailSegment6 = new GeoPhysVol(lEndplateRailSegment6);
  GeoPhysVol* pEndplateRailSegment8 = new GeoPhysVol(lEndplateRailSegment8);


  //------------------------------------------------------------------------------------------
  // Replicate and position
  GENFUNCTION fTrlRail = ArrayFunction( zPosEndplate, zPosEndplate + nZPosEndplate ); 
  
  TRANSFUNCTION XFEndplateRailSegment6 = HepRotateZ3D( 10 * phi0 ) * Pow( HepTranslateZ3D(1.0), fTrlRail ); 
  TRANSFUNCTION XFEndplateRailSegment8 = HepRotateZ3D( 14 * phi0 ) * Pow( HepTranslateZ3D(1.0), fTrlRail ); 

  GeoSerialTransformer* sxEndplateRailSegment6 = new GeoSerialTransformer( pEndplateRailSegment6, 
                                                                           &XFEndplateRailSegment6, 
                                                                           nZPosEndplate );
  GeoSerialTransformer* sxEndplateRailSegment8 = new GeoSerialTransformer( pEndplateRailSegment8, 
                                                                           &XFEndplateRailSegment8, 
                                                                           nZPosEndplate );
  container->add(sxEndplateRailSegment6);
  container->add(sxEndplateRailSegment8);

  //------------------------------------------------------------------------------------------
  //  Build wall standard segments
  //  There are two segments (where the ECT rests on the rails) which have an extra 
  //  horizontal cut-out and have to be treated separately (see below) 
  //----------------------------------------------------------------------------------------
  // start from equilateral triangular prism (opening angle = 45 deg)
  GeoPgon* sWallPgon = new GeoPgon( -phi0, 2 * phi0, 1 );
  sWallPgon->addPlane( -deltaZ/2 + thicknessEndplate, rMin, rOuterCast/cosPhi0 );
  sWallPgon->addPlane(  deltaZ/2 - thicknessEndplate, rMin, rOuterCast/cosPhi0 );

  // quarter of 'large enough' octogonal prism to cut out most of the inner volume 
  GeoPgon* sEmptyPgon = new GeoPgon( -2 * phi0, 4 * phi0, 2 );
  sEmptyPgon->addPlane( -deltaZ/2, 0, ( rInnerCast - thicknessWall )/cosPhi0 );
  sEmptyPgon->addPlane(  deltaZ/2, 0, ( rInnerCast - thicknessWall )/cosPhi0 );

  // trapezoid to cut out the remaining inner volume (= inside of outer castellations)
  double gamma         = ( M_PI - alpha )/2;
  double pDx1InnerTrap = widthOuterCast/2 - thicknessWall * tan(gamma); 
  double pDzInnerTrap  = rOuterCast/4;                                   // some 'large enough' value  
  double pDx3InnerTrap = pDx1InnerTrap + 2 * pDzInnerTrap * tan( alpha - M_PI/2 ); 

  GeoTrd* sEmptyInnerTrap = new GeoTrd( pDx1InnerTrap, pDx3InnerTrap, pDy, pDy, pDzInnerTrap );


  //----------------------------------------------------------------------------------------
  // cut out inner prism and trapezoids
  // - use sEmptyOuterTrap shape from above to cut out space between outer castellations

  // trapezoid positioning wrt. wall segment (trapezoid axes don't coincide with polygon ones)
  HepTransform3D xfInnerTrap = HepTranslateX3D( rOuterCast - thicknessWall - pDzInnerTrap ) * 
                               HepRotateZ3D( -M_PI/2 ) * HepRotateX3D( M_PI/2 );

  const GeoShape& sWallStdSegment = sWallPgon->subtract(  *sEmptyPgon ).
                                               subtract( (*sEmptyOuterTrap) << xfLeftTrap ).
                                               subtract( (*sEmptyOuterTrap) << xfRightTrap ).
                                               subtract( (*sEmptyInnerTrap) << xfInnerTrap );

  GeoLogVol*  lWallStdSegment = new GeoLogVol( "ECTWallStdSegment", 
                                               &sWallStdSegment, 
                                               theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pWallStdSegment = new GeoPhysVol(lWallStdSegment);


  //------------------------------------------------------------------------------------------
  // Replicate and position
  
  // array of z positions
  const int nZPosWall = 2;
  double zPosWall[nZPosWall] = { -zMean, zMean };
  
  // fill auxiliary arrays
  for ( int i = 0; i < nStdSectors * nZPosWall; i++ )  
  {
     int ii = i % nStdSectors, 
         jj = i / nStdSectors;
     phiAux[i] = phiStdSector[ii];
     posAux[i] = zPosWall[jj];    
  }

  GENFUNCTION fRotWallStd = ArrayFunction( phiAux, phiAux + nStdSectors * nZPosWall ); 
  GENFUNCTION fTrlWallStd = ArrayFunction( posAux, posAux + nStdSectors * nZPosWall ); 
  
  TRANSFUNCTION XFWallStdSegment = Pow( HepRotateZ3D(1.0), fRotWallStd ) * Pow( HepTranslateZ3D(1.0), fTrlWallStd ); 

  GeoSerialTransformer* sxWallStdSegment = new GeoSerialTransformer( pWallStdSegment, 
                                                                     &XFWallStdSegment, 
                                                                     nStdSectors * nZPosWall );
  container->add(sxWallStdSegment);

  //------------------------------------------------------------------------------------------
  //  Build wall segments resting on rails ("segment6", "segment8"), which have extra 
  //  horizontal cut-outs wrt. standard segments
  //------------------------------------------------------------------------------------------
  // cut edge from empty inner trapezoid by means of above sEmptyRailBox 
  
  // box positioning wrt. empty inner trapezoid (box axes don't coincide with trapezoid ones) 
  // (positioning wrt. wall segment is the same as for endplates --> xfRailBox6 above)
  double delta = thicknessEndplate * ( 1/cos(gamma) - 1 );
  HepTransform3D shift          = HepTranslate3D( -delta * sin(gamma), 0 , -delta * cos(gamma) ); 
  HepTransform3D xfInnerRailBox = shift * HepTranslate3D( -pDx1InnerTrap, 0, -pDzInnerTrap ) *
                                  HepRotateY3D( -M_PI/4 ) * HepRotateX3D(  M_PI/2 );

  const GeoShape& sEmptyBevelledTrap = sEmptyInnerTrap->subtract( (*sEmptyRailBox) << xfInnerRailBox );

  //----------------------------------------------------------------------------------------
  // cut out inner prism, outer trapezoids, and bevelled trapezoid from wall polygon segment
  // - need only one 'master' shape (e.g. segment6; segment8 can be obtained via reflection) 
  const GeoShape& sWallRailSegment = sWallPgon->subtract( (*sEmptyOuterTrap)   << xfLeftTrap ).
                                                subtract( (*sEmptyOuterTrap)   << xfRightTrap ).
                                                subtract( (*sEmptyRailBox)     << xfRailBox6 ).
                                                subtract(  *sEmptyPgon ).
                                                subtract(   sEmptyBevelledTrap << xfInnerTrap );

  GeoLogVol*  lWallRailSegment = new GeoLogVol( "ECTWallRailSegment", 
                                                &sWallRailSegment, 
                                                theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pWallRailSegment = new GeoPhysVol(lWallRailSegment);


  //------------------------------------------------------------------------------------------
  // Replicate and position
  GENFUNCTION fTrlWallRail = ArrayFunction( zPosWall, zPosWall + nZPosWall ); 
  
  TRANSFUNCTION XFWallRailSegment6 = HepRotateZ3D( 10 * phi0 ) * Pow( HepTranslateZ3D(1.0), fTrlWallRail ); 
  TRANSFUNCTION XFWallRailSegment8 = HepRotateZ3D(  6 * phi0 ) * Pow( HepTranslateZ3D(1.0), fTrlWallRail ) * 
                                     HepRotateY3D(M_PI); 

  GeoSerialTransformer* sxWallRailSegment6 = new GeoSerialTransformer( pWallRailSegment, 
                                                                       &XFWallRailSegment6, 
                                                                       nZPosWall );
  GeoSerialTransformer* sxWallRailSegment8 = new GeoSerialTransformer( pWallRailSegment, 
                                                                       &XFWallRailSegment8, 
                                                                       nZPosWall );
  container->add(sxWallRailSegment6);
  container->add(sxWallRailSegment8);

  //------------------------------------------------------------------------------------------
  //  Build central tube 
  //------------------------------------------------------------------------------------------
  
  GeoTube* sCentTube = new GeoTube( rMin, rMin + thicknessCentTube, deltaZ/2 - thicknessEndplate );

  GeoLogVol*  lCentTube = new GeoLogVol( "ECTCentralTube", sCentTube, theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pCentTube = new GeoPhysVol(lCentTube);

  //------------------------------------------------------------------------------------------
  // Replicate and position  (same z position as for wall segments)
  TRANSFUNCTION XFCentTube = Pow( HepTranslateZ3D(1.0), fTrlWallRail ); 

  GeoSerialTransformer* sxCentTube = new GeoSerialTransformer( pCentTube, &XFCentTube, nZPosWall );

  container->add(sxCentTube);
  
  //------------------------------------------------------------------------------------------
  //  Build staytubes 
  //------------------------------------------------------------------------------------------

  GeoTube* sStaytube = new GeoTube( rInStaytube, rOutStaytube, deltaZ/2 - thicknessEndplate );

  GeoLogVol*  lStaytube = new GeoLogVol( "ECTStaytube", sStaytube, theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pStaytube = new GeoPhysVol(lStaytube);

  //------------------------------------------------------------------------------------------
  // Replicate and position  (same z position as for wall segments)
  
  // fill auxiliary arrays
  for ( int i = 0; i < nPhiSectors * nZPosWall; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = 2 * ii * phi0;
     posAux[i] = zPosWall[jj]; 
  }

  GENFUNCTION fRotStaytube = ArrayFunction( phiAux, phiAux + nPhiSectors * nZPosWall ); 
  GENFUNCTION fTrlStaytube = ArrayFunction( posAux, posAux + nPhiSectors * nZPosWall ); 
  
  TRANSFUNCTION XFStaytube = Pow( HepRotateZ3D(1.0), fRotStaytube ) * 
                             Pow( HepTranslateZ3D(1.0), fTrlStaytube ) * 
                             xfStaytubeHole; 

  GeoSerialTransformer* sxStaytube = new GeoSerialTransformer( pStaytube, &XFStaytube, nPhiSectors * nZPosWall );

  container->add(sxStaytube);

}


 void EndCapToroidBuilderRDB::buildECTConductorBox( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds ECT Conductor and Coil Casing
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

   const double zMinECTVessel        = (*m_Aect)[0]->getFloat("CRYOZMIN") * mm;                
   const double zDeltaECTVessel      = (*m_Aect)[0]->getFloat("CRYOZEXT") * mm; 
   const double zMinECTCondBox       = (*m_Aect)[0]->getFloat("CNBXZMIN") * mm;                
   const double zMaxECTCondBox       = (*m_Aect)[0]->getFloat("CNBXZMAX") * mm;                
   const double rMinECTCondBox       = (*m_Aect)[0]->getFloat("CNBXTMIN") * mm;                
   const double rMaxECTCondBox       = (*m_Aect)[0]->getFloat("CNBXTMAX") * mm;                
   const double thicknessECTCondBox  = (*m_Aect)[0]->getFloat("CNBXTHIC") * mm;                
   const double lCornerCutECTCondBox = (*m_Aect)[0]->getFloat("CNBXEDGE") * mm;                
                                                                              //
   const std::string condMaterial = getMaterial( "Aluminium" ); 
//----------------------------------------------------------------------------//
  

  const double phi0 = M_PI/8;
  
  // Conductor Box properties 
  const double deltaZ       = zMaxECTCondBox - zMinECTCondBox; 
  const double distToCorner = lCornerCutECTCondBox; 
  const double deltaR       = rMaxECTCondBox - rMinECTCondBox;
  const double thickness    = thicknessECTCondBox;

  const double zMin	    = zMinECTVessel;
  const double zMax	    = zMinECTVessel + zDeltaECTVessel; 
  const double zMean	    = (zMin + zMax)/2;  		//
  // positioning variables
//  const double zMean = 0.5 * ( zMaxECTCondBox + zMinECTCondBox ); //v Saclay izpol'zuetsia (CryoXmin + CryoZmax)/2
  const double rMean = 0.5 * ( rMaxECTCondBox + rMinECTCondBox );


  //------------------------------------------------------------------------------------------
  //  Build Conductor Box as a large box with bevelled corners 
  //------------------------------------------------------------------------------------------

  // large box 
  const GeoShape* sConductorBox = new GeoBox( deltaR/2, thickness/2, deltaZ/2 );

  // 'large enough' empty box to cut edges from large box
  double lDiag = sqrt(2) * distToCorner;
  GeoBox* sEmptyBox = new GeoBox( lDiag/2, thickness, lDiag ); 
                                                                      

  //----------------------------------------------------------------------------------------
  // cut away edges from large box

  // positioning of empty box centres wrt. large box centre
  HepTransform3D trlEmptyBox[4] = { HepTranslate3D(  deltaR/2, 0, -deltaZ/2 ),
                                    HepTranslate3D( -deltaR/2, 0, -deltaZ/2 ),
                                    HepTranslate3D( -deltaR/2, 0,  deltaZ/2 ),
                                    HepTranslate3D(  deltaR/2, 0,  deltaZ/2 ) };
  for ( int i = 0; i < 4; i++ )
  {
     HepTransform3D xfEmptyBox = trlEmptyBox[i] * HepRotateY3D( ( 2*i + 1 ) * M_PI/4 );
     sConductorBox = &( sConductorBox->subtract( (*sEmptyBox) << xfEmptyBox ) );
  }

  GeoLogVol*  lConductorBox = new GeoLogVol( "ECTConductorBox", 
                                             sConductorBox, 
                                             theMaterialManager->getMaterial(condMaterial) );
  GeoPhysVol* pConductorBox = new GeoPhysVol(lConductorBox);


  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of phi values
  const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = 2 * i * phi0;  
  
  // array of z positions
  const int nZPos = 2;
  double zPos[nZPos] = { -zMean, zMean };
    
  // auxiliary arrays to store phi values, z positions for all volumes to be replicated 
  double phiAux[ nPhiSectors * nZPos ],  posAux[ nPhiSectors * nZPos ];
  for ( int i = 0; i < nPhiSectors * nZPos; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = phiVal[ii]; 
     posAux[i] = zPos[jj];
  }

  GENFUNCTION fRot = ArrayFunction( phiAux, phiAux + nPhiSectors * nZPos ); 
  GENFUNCTION fTrl = ArrayFunction( posAux, posAux + nPhiSectors * nZPos ); 
  
  TRANSFUNCTION XFConductorBox = Pow( HepRotateZ3D(1.0), fRot ) * 
                                 Pow( HepTranslateZ3D(1.0), fTrl ) * HepTranslateX3D(rMean); 
  
  GeoSerialTransformer* sxConductorBox = new GeoSerialTransformer( pConductorBox, 
                                                                   &XFConductorBox, 
                                                                   nPhiSectors * nZPos );
  container->add(sxConductorBox);

}

 void EndCapToroidBuilderRDB::buildECTKeystoneBox( GeoPhysVol* container ) 
{

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

   const double tolerance = 0.1 * mm;

   const double zMinECTVessel         = (*m_Aect)[0]->getFloat("CRYOZMIN") * mm;  
   const double zoffsetKeystone	      = (*m_Aect)[0]->getFloat("SUPPLZOF") * mm;
   const double zExtKeystone	      = (*m_Aect)[0]->getFloat("SUPPLZEX") * mm;
   const double zThicknessECTKeystone = (*m_Aect)[0]->getFloat("SUPPLTHZ") * mm;                   
   const double rMinInnerECTKeystone  = (*m_Aect)[0]->getFloat("SUPPLTMI") * mm;
   const double rThicknessECTKeystone = (*m_Aect)[0]->getFloat("SUPPLTHT") * mm;
   const double rMinOuterECTKeystone  = (*m_Aect)[0]->getFloat("SUPPLTMA") * mm;
   const double widthOuterECTKeystone   = (*m_Aect)[0]->getFloat("SUPPLATX_3") * mm;//2828.5 * mm;                   // etkb->widout!!! etogo u Saclay net!!!
   const double widthBot		= (*m_Aect)[0]->getFloat("SUPPLATX_0") * mm;
   const double radiusHoleECTKeystone   = (*m_Aect)[0]->getFloat("SUPPLRHO") * mm; 
   const double rDistStaytubeECTVessel  = (*m_Aect)[0]->getFloat("CRYOTTU0") * mm;
                                                                              //
   const std::string keystoneMaterial = getMaterial( "Aluminium" );
//----------------------------------------------------------------------------//
  

  const double phi0 = M_PI/8;
  
  // Keystone Box properties 
  const double zMinECTKeystone = zMinECTVessel + zoffsetKeystone;
  const double zMaxECTKeystone = zMinECTKeystone + zExtKeystone;
  const double deltaZOut    = zMaxECTKeystone - zMinECTKeystone; 
  const double deltaZIn     = deltaZOut - zThicknessECTKeystone; 
  const double rMinIn       = rMinInnerECTKeystone; 
  const double rMaxIn       = rMinInnerECTKeystone + rThicknessECTKeystone; 
  const double rMinOut      = rMinOuterECTKeystone; 
  const double rMaxOut      = rMinOuterECTKeystone + rThicknessECTKeystone; 
  const double deltaROut    = rMaxOut - rMinIn; 
  const double deltaRIn     = rMinOut - rMaxIn; 
  const double thicknessTop = rMaxOut - rMinOut;
  const double thicknessBot = rMaxIn  - rMinIn; 
  const double widthTop     = widthOuterECTKeystone; 
//  const double widthBot     = widthTop - 2 * deltaROut * tan(phi0); 
  const double holeRadius   = radiusHoleECTKeystone; 
  const double rDistHole    = rDistStaytubeECTVessel;

  // positioning variables
  const double zMean = 0.5 * ( zMaxECTKeystone + zMinECTKeystone ); 
  const double rMean = 0.5 * ( rMaxOut +  rMinIn );


  //------------------------------------------------------------------------------------------
  //  Build Keystone Box as trapezoidal shape; 
  //  inner empty space is cut out by a box, holes in the vertical plates by a tube 
  //------------------------------------------------------------------------------------------

  // main (outer) trapezoid 
  GeoTrd* sOuterTrap = new GeoTrd( widthTop/2 - tolerance, widthBot/2 - tolerance, deltaZOut/2, deltaZOut/2, deltaROut/2 );
  
  // 'large enough' empty box to cut out inner empty space
  GeoBox*  sEmptyBox  = new GeoBox( widthTop/2, deltaZIn/2, deltaRIn/2 );

  // 'large enough' empty tube to cut out holes in the vertical plates
  GeoTube* sEmptyTube = new GeoTube( 0, holeRadius, deltaZOut );
                                                                      

  //----------------------------------------------------------------------------------------
  // cut out inner space and holes in vertical plates from main trapezoid

  // positioning of empty box and tube wrt. main trapezoid
  HepTransform3D trlEmptyBox = HepTranslateZ3D( thicknessBot/2 - thicknessTop/2 ); 
  HepTransform3D xfEmptyTube = HepTranslateZ3D( rMean - rDistHole ) * HepRotateX3D( M_PI/2 );
  
  const GeoShape& sKeystoneBox = sOuterTrap->subtract( (*sEmptyBox)  << trlEmptyBox ).
                                             subtract( (*sEmptyTube) << xfEmptyTube );

  GeoLogVol*  lKeystoneBox = new GeoLogVol( "ECTKeystoneBox", 
                                            &sKeystoneBox, 
                                            theMaterialManager->getMaterial(keystoneMaterial) );
  GeoPhysVol* pKeystoneBox = new GeoPhysVol(lKeystoneBox);


  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of phi values
  const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = ( 2*i + 1 ) * phi0;  
  
  // array of z positions
  const int nZPos = 2;
  double zPos[nZPos] = { -zMean, zMean };
    
  // auxiliary arrays to store phi values, z positions for all volumes to be replicated 
  double phiAux[ nPhiSectors * nZPos ],  posAux[ nPhiSectors * nZPos ];
  for ( int i = 0; i < nPhiSectors * nZPos; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = phiVal[ii]; 
     posAux[i] = zPos[jj];
  }

  GENFUNCTION fRot = ArrayFunction( phiAux, phiAux + nPhiSectors * nZPos ); 
  GENFUNCTION fTrl = ArrayFunction( posAux, posAux + nPhiSectors * nZPos ); 
  
  TRANSFUNCTION XFKeystoneBox = Pow( HepRotateZ3D(1.0), fRot ) * 
                                Pow( HepTranslateZ3D(1.0), fTrl ) * 
                                HepTranslateX3D(rMean) * HepRotateZ3D( -M_PI/2 ) * HepRotateX3D( M_PI/2 ); 
  
  GeoSerialTransformer* sxKeystoneBox = new GeoSerialTransformer( pKeystoneBox, 
                                                                  &XFKeystoneBox, 
                                                                  nPhiSectors * nZPos );
  container->add(sxKeystoneBox);

}

 void EndCapToroidBuilderRDB::buildECTServiceTower( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds ECT Vacuum Vessel (end-plates, wall segments, staytubes)
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const double SrvTu1Dz = (*m_Aect)[0]->getFloat("SRVTU1DZ") * mm;
  const double CryoZmoy = (*m_Aect)[0]->getFloat("CRYOZMOY") * mm;
  const double SrvTu1He = (*m_Aect)[0]->getFloat("SRVTU1HE") * mm;
  const double SrvTu1oW = (*m_Aect)[0]->getFloat("SRVTU1OW") * mm;
  const double SrvTu1iW = (*m_Aect)[0]->getFloat("SRVTU1IW") * mm;
  //const double SrvTu1Ed = (*m_Aect)[0]->getFloat("SRVTU1ED") * mm;
  const double SrvTu2Dz = (*m_Aect)[0]->getFloat("SRVTU2DZ") * mm;
  const double SrvTu2He = (*m_Aect)[0]->getFloat("SRVTU2HE") * mm;
  const double SrvTu2oR = (*m_Aect)[0]->getFloat("SRVTU2OR") * mm;
  const double SrvTu2iR = (*m_Aect)[0]->getFloat("SRVTU2IR") * mm;
  const double SrvTu3Dz = (*m_Aect)[0]->getFloat("SRVTU3DZ") * mm; 
  const double SrvTu3He = (*m_Aect)[0]->getFloat("SRVTU3HE") * mm;
  const double X1temp   = (*m_Aect)[0]->getFloat("X1TEMP") * mm;
  const double X0temp   = (*m_Aect)[0]->getFloat("X0TEMP") * mm;
  const double SToThic1 = (*m_Ecst)[0]->getFloat("STOTHIC1") * mm;
  const double SToThic2 = (*m_Ecst)[0]->getFloat("STOTHIC2") * mm;
  const double SToThic3 = (*m_Ecst)[0]->getFloat("STOTHIC3") * mm;
  //const double STOThic4 = (*m_Ecst)[0]->getFloat("STOTHIC4") * mm;
  const double SToLeng1 = (*m_Ecst)[0]->getFloat("STOLENG1") * mm;
  const double SToLengt = (*m_Ecst)[0]->getFloat("STOLENGT") * mm;
  const double SToLenga = (*m_Ecst)[0]->getFloat("STOLENGA") * mm;
  //const double SToXpref = (*m_Ecst)[0]->getFloat("STOXPREF") * mm;
  //const double SToYpref = (*m_Ecst)[0]->getFloat("STOYPREF") * mm;
  const double SToHeigh = (*m_Ecst)[0]->getFloat("STOHEIGH") * mm;
  const double SToHeig1 = (*m_Ecst)[0]->getFloat("STOHEIG1") * mm;
  const double SToHeig2 = (*m_Ecst)[0]->getFloat("STOHEIG2") * mm;
  const double SToHeig3 = (*m_Ecst)[0]->getFloat("STOHEIG3") * mm;
  const double SToHeig4 = (*m_Ecst)[0]->getFloat("STOHEIG4") * mm;
  const double SToDzBot = (*m_Ecst)[0]->getFloat("STODZBOT") * mm;
  const double SToDzIn1 = (*m_Ecst)[0]->getFloat("STODZIN1") * mm;
  const double SToDzIn2 = (*m_Ecst)[0]->getFloat("STODZIN2") * mm;
  const double SToDzIn3 = (*m_Ecst)[0]->getFloat("STODZIN3") * mm;
  const double SToDzIn4 = (*m_Ecst)[0]->getFloat("STODZIN4") * mm;
  const double SToDzIn5 = (*m_Ecst)[0]->getFloat("STODZIN5") * mm;
  const double SToDzTop = (*m_Ecst)[0]->getFloat("STODZTOP") * mm;
  const double SToAngle = (*m_Ecst)[0]->getFloat("STOANGLE") * deg;
  const double SToYpos  = (*m_Ecst)[0]->getFloat("STOYPOS") * mm;
  const double SToZpos  = (*m_Ecst)[0]->getFloat("STOZPOS") * mm;  
//   const double SToGeCxz = (*m_Ecst)[0]->getFloat("STOGECXZ") * mm;
//   const double SToGeChe = (*m_Ecst)[0]->getFloat("STOGECHE") * mm;
//   const double SToCLCzp = (*m_Ecst)[0]->getFloat("STOCLCZP") * mm;
//   const double SToUeCzp = (*m_Ecst)[0]->getFloat("STOUECZP") * mm;
//   const double SToUeCyp = (*m_Ecst)[0]->getFloat("STOUECYP") * mm;
  const double CryoT1   = (*m_Aect)[0]->getFloat("CRYOT1") * mm;

   const std::string endcaptowerMaterial = getMaterial( "Aluminium" );

  const double tolerance = 0.5 * mm;

//-----------------------
  GeoBox* sTowerBox1 = new GeoBox( SToDzBot, SToThic1 + SToLeng1/2, SToHeig1/2 );
  GeoBox* sTowerBox2 = new GeoBox( SToDzIn1, SToThic1 + SToLeng1/2, ( SToHeig2 - SToHeig1 - 
				   tan(SToAngle) * ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) )/2 );
  GeoTrd* sTowerTrap = new GeoTrd( SToDzIn1, SToDzIn2 + SToDzIn3, SToThic1 + SToLeng1/2,
				   SToThic1 + SToLeng1/2, tan(SToAngle) * 
				   ( SToDzIn1 - SToDzIn2 - SToDzIn3 )/2 );
  GeoBox* sTowerBox3 = new GeoBox( SToDzIn3, SToThic1 + SToLeng1/2, ( SToHeigh - SToHeig3 - SToHeig4 
				   - SToHeig2 )/2 );
  GeoBox* sEmptyTowerBoxHalf = new GeoBox( SToDzIn1 + tolerance, SToLeng1,
					    SToHeigh/2  + tolerance );
  GeoBox* sTowerMiddelBox = new GeoBox( SToDzIn5, SToLengt/2, SToHeig3/2 );
  GeoTrd* sTowerMiddelTrap= new GeoTrd( SToLengt/2, SToLenga/2, SToHeig3/2, SToHeig3/2, ( SToDzIn4 - 
					SToDzIn5 )/2 );
  GeoBox* sTowerTopBox = new GeoBox( SToDzTop, SToLengt/2, SToHeig4/2 - tolerance );				
  GeoBox* sEmptyTowerBox1 = new GeoBox( SToDzBot - SToThic2, SToLeng1/2, ( SToHeig1 + SToThic2 )/2 + 
					tolerance );
  GeoBox* sEmptyTowerBox2 = new GeoBox( SToDzIn1 - SToThic2, SToLeng1/2, ( SToHeig2 - SToHeig1 -
                                        tan(SToAngle) * ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) )/2 -
					SToThic2 * ( 1 + tan(SToAngle/4) )/2 );
  GeoTrd* sEmptyTowerTrap = new GeoTrd( SToDzIn1 - SToThic2, SToDzIn2 + SToDzIn3 - tan(SToAngle/2) 
					* SToThic2, SToLeng1/2, SToLeng1/2, ( tan(SToAngle) *
                                        ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) - SToThic2 * (1 -
					tan(SToAngle/4) ) )/2 + tolerance );
  GeoBox* sEmptyTowerBox3 = new GeoBox( SToDzIn3 - SToThic2, SToLeng1/2, ( SToHeigh - SToHeig3 - 
					SToHeig4 - SToHeig2 + SToThic2 )/2 + tolerance );
  GeoBox* sEmptyTowerMiddelBox = new GeoBox( SToDzIn3 - SToThic2, SToLengt/2 - SToThic3, SToHeig3 +
						tolerance );
						
//--------------------------------------------------------------------------------------------------					
  HepTransform3D trlTowerBox2 = HepTranslateZ3D( ( SToHeig2 - tan(SToAngle) * ( SToDzIn1 - SToDzIn2 -
						     SToDzIn3 ) )/2 );
  HepTransform3D trlTowerTrap = HepTranslateZ3D(  SToHeig2 - ( tan(SToAngle) *
                                		 ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) + SToHeig1 )/2 );
  HepTransform3D trlTowerBox3 = HepTranslateZ3D( ( SToHeig2 + SToHeigh - SToHeig3 - SToHeig4
                                		  - SToHeig1 )/2  );
  HepTransform3D trlTowerMiddelBox = HepTranslateZ3D( SToHeigh - SToHeig4 - SToHeig3/2 -
							     SToHeig1/2 );
  HepTransform3D trlTowerMiddelTrap= HepTranslate3D( (SToDzIn5 + SToDzIn4)/2, 0, 
						     SToHeigh - SToHeig4 - SToHeig3/2 - SToHeig1/2 )  
						     * HepRotateY3D(M_PI/2) * HepRotateZ3D(M_PI/2);
  HepTransform3D trlTowerTopBox = HepTranslateZ3D( SToHeigh - SToHeig4/2 - SToHeig1/2 );
  
  HepTransform3D trlEmptyTowerMiddelBox = HepTranslateZ3D( SToHeigh - SToHeig4 - SToHeig3/2 -
                                                             SToHeig1/2 );						     
  HepTransform3D trlEmptyTowerBox1 = HepTranslateZ3D( SToThic2/2 ); 
  HepTransform3D trlEmptyTowerBox2 = HepTranslateZ3D( SToThic2 * ( 1 - tan(SToAngle/4) )/2 
						      + ( SToHeig2 - tan(SToAngle) * 
						      ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) )/2 );  
  HepTransform3D trlEmptyTowerBox3 = HepTranslateZ3D( ( SToHeigh - SToHeig3 - SToHeig4 )/2 +
						      SToHeig2/2 - SToHeig1/2 - SToThic2/2 );
  HepTransform3D trlEmptyTowerBoxHalf = HepTranslate3D( -SToDzIn1, 0, 
							( SToHeigh - SToHeig1 )/2 );  
  HepTransform3D trlEmptyTowerTrap = HepTranslateZ3D( SToHeig2 - SToThic2 * ( 1 + tan(SToAngle/4) )/2
						     - ( SToHeig1 + tan(SToAngle) *
                                                      ( SToDzIn1 - SToDzIn2 - SToDzIn3 ) )/2 );       					
//--------------------------------------------------------------------------------------------------					
  const GeoShape& sEndCapTower = sTowerBox1->add( (*sTowerBox2) << trlTowerBox2 ).
					     add( (*sTowerTrap) << trlTowerTrap ).
					     add( (*sTowerBox3) << trlTowerBox3 ).
					     add( (*sTowerMiddelBox) << trlTowerMiddelBox ).
					     add( (*sTowerMiddelTrap) << trlTowerMiddelTrap ).
					     add( (*sTowerTopBox) << trlTowerTopBox ).
				 subtract( (*sEmptyTowerBox1) << trlEmptyTowerBox1 ).
                                 subtract( (*sEmptyTowerBox2) << trlEmptyTowerBox2 ).
				 subtract( (*sEmptyTowerTrap) << trlEmptyTowerTrap ).
				 subtract( (*sEmptyTowerBox3) << trlEmptyTowerBox3 ).
				 subtract( (*sEmptyTowerMiddelBox) << trlEmptyTowerMiddelBox ).
				 subtract( (*sEmptyTowerBoxHalf) << trlEmptyTowerBoxHalf );

  GeoLogVol*  lEndCapTower = new GeoLogVol( "ECTTower",
                                            &sEndCapTower,
                                            theMaterialManager->getMaterial(endcaptowerMaterial) );
  GeoPhysVol* pEndCapTower = new GeoPhysVol(lEndCapTower);			

  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of z positions
  const int nZPos = 2;
  double zPos[nZPos] = { -SToZpos - CryoZmoy, SToZpos + CryoZmoy };

  // array of rotation angles (flipping around y-axis)
  double rotAngle[nZPos] = { 0, M_PI };

  GENFUNCTION fRot = ArrayFunction( rotAngle, rotAngle + nZPos );
  GENFUNCTION fTrl = ArrayFunction( zPos,     zPos     + nZPos );

  TRANSFUNCTION XFEndCapTower = Pow( HepTranslateZ3D(1.0), fTrl ) *
                        	HepTranslateY3D(SToYpos + SToHeig1/2) *             // constant displacement
	                        Pow( HepRotateY3D(1.0), fRot ) *
				HepRotateX3D(-M_PI/2) * HepRotateZ3D(-M_PI/2);

  GeoSerialTransformer* sxEndCapTower = new GeoSerialTransformer( pEndCapTower, &XFEndCapTower, nZPos );

  container->add(sxEndCapTower);
  
//----------------needfull volumes -----

  GeoBox* sBottomTowerBox = new GeoBox( SToLeng1/2, SToThic2/2, SToDzBot/2 - SToThic2 );
  GeoShape* sBackTowerWall= new GeoBox( SToLeng1/2, SToHeigh/2, SToThic2/2 );
  GeoBox* sSrvTurBox1     = new GeoBox( SrvTu1oW/2, SrvTu1He/2, SrvTu1oW/2 );
  GeoBox* sSrvTurBox2     = new GeoBox( X1temp/2, SrvTu1He/2, X1temp/2 );
  GeoBox* sEmptySrvTurBox1= new GeoBox( SrvTu1iW/2, SrvTu1He/2, SrvTu1iW/2 );
  GeoBox* sEmptySrvTurBox2= new GeoBox( X0temp/2, SrvTu1He/2, X0temp/2 );
  GeoTube* sTubeBotSrvTur = new GeoTube( SrvTu2iR, SrvTu2oR, SrvTu2He/2 - tolerance );
  GeoTube* sTubeMidSrvTur = new GeoTube( SrvTu2iR, SrvTu2oR, (SrvTu2Dz - SrvTu3Dz)/2 + SrvTu2oR -
					    tolerance );
  GeoTube* sTubeTopSrvTur = new GeoTube( SrvTu2iR, SrvTu2oR, SrvTu3He/2 + SrvTu2oR - tolerance );
  GeoBox* sEmptyEdgeTubeBox = new GeoBox( SrvTu2oR * sqrt(2) + tolerance,
					    SrvTu2oR * sqrt(2) + tolerance,
					    SrvTu2oR * sqrt(2) + tolerance );

  const GeoShape& sSrvTurOut = sSrvTurBox1->intersect( (*sSrvTurBox2) << HepRotateY3D(M_PI/4) );
  const GeoShape& sSrvTurIn  = sEmptySrvTurBox1->intersect( (*sEmptySrvTurBox2) <<
							     HepRotateY3D(M_PI/4) );
  const GeoShape& sBotSrvTur = sTubeBotSrvTur->subtract( (*sEmptyEdgeTubeBox) <<
						HepTranslate3D( 0, SrvTu2oR, SrvTu2He/2 ) 
						* HepRotateX3D(M_PI/4) );
  const GeoShape& sMidSrvTur = sTubeMidSrvTur->subtract( (*sEmptyEdgeTubeBox) <<
						HepTranslate3D( 0, -SrvTu2oR, (SrvTu2Dz - SrvTu3Dz)/2
						+ SrvTu2oR ) * HepRotateX3D(M_PI/4) ).
						subtract( (*sEmptyEdgeTubeBox) <<
                                                HepTranslate3D( 0, SrvTu2oR, -(SrvTu2Dz - SrvTu3Dz)/2
						- SrvTu2oR ) * HepRotateX3D(-M_PI/4) );  
  const GeoShape& sTopSrvTur = sTubeTopSrvTur->subtract( (*sEmptyEdgeTubeBox) <<
                                                HepTranslate3D( 0, -SrvTu2oR, -SrvTu3He/2 - SrvTu2oR ) 
						* HepRotateX3D(M_PI/4) );

//--------------------Bottom Tower Plate-------------------------------------------------------

  const GeoShape& sBottomTower = sBottomTowerBox->subtract( (sSrvTurOut) << 
							    HepTranslateZ3D( 0. ) );
							    
  GeoLogVol*  lBottomTower = new GeoLogVol( "ECTBottomTower",
                                            &sBottomTower,
                                            theMaterialManager->getMaterial(endcaptowerMaterial) );
  GeoPhysVol* pBottomTower = new GeoPhysVol(lBottomTower);
  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of z positions
  double zPosBot[nZPos] = { -SToZpos - CryoZmoy + SToDzBot/2, SToZpos + CryoZmoy - SToDzBot/2 };

  GENFUNCTION fTrlBot = ArrayFunction( zPosBot,     zPosBot     + nZPos );

  TRANSFUNCTION XFBottomTower = Pow( HepTranslateZ3D(1.0), fTrlBot ) *
                                HepTranslateY3D(SToYpos + SToThic2/2) *             // constant displacement
                                Pow( HepRotateY3D(1.0), fRot ); 

  GeoSerialTransformer* sxBottomTower = new GeoSerialTransformer( pBottomTower, &XFBottomTower
								, nZPos );

  container->add(sxBottomTower);

//-------------------Back Tower Plate--------------------------------------------------------
  GeoLogVol*  lBackTowerWall = new GeoLogVol( "ECTBackTowerWall",
                                            &*sBackTowerWall,
                                            theMaterialManager->getMaterial(endcaptowerMaterial) );
  GeoPhysVol* pBackTowerWall = new GeoPhysVol(lBackTowerWall);
  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of z positions 
  double zPosBackWall[nZPos] = { -SToZpos - CryoZmoy + SToThic2/2,
				 SToZpos + CryoZmoy - SToThic2/2 };

  GENFUNCTION fTrlBackWall = ArrayFunction( zPosBackWall,     zPosBackWall     + nZPos );

  TRANSFUNCTION XFBackTowerWall = Pow( HepTranslateZ3D(1.0), fTrlBackWall ) *
                                HepTranslateY3D(SToYpos + SToHeigh/2) *             // constant displacement
                                Pow( HepRotateY3D(1.0), fRot );

  GeoSerialTransformer* sxBackTowerWall = new GeoSerialTransformer( pBackTowerWall, &XFBackTowerWall
								, nZPos );

  container->add(sxBackTowerWall);
  
//---------------Service Turret-----------------------
						    
  const GeoShape& sSrvTurret   = sSrvTurOut.subtract( (sSrvTurIn) <<
  					HepTranslateY3D( ( SrvTu1iW - SrvTu1oW )/2 ) ).
					add( (sBotSrvTur) << HepTranslate3D( 0, SrvTu1He/2 + 
					SrvTu2He/2, ( SrvTu2Dz-SrvTu1Dz ) ) *
					HepRotateX3D(-M_PI/2) ).
					add( (sMidSrvTur) << HepTranslate3D( 0., SrvTu1He/2 +
                                        SrvTu2He - SrvTu2oR, -(SrvTu2Dz - SrvTu3Dz)/2 + 
					( SrvTu2Dz-SrvTu1Dz ) ) ).
					add( (sTopSrvTur) << HepTranslate3D( 0., SrvTu1He/2 +
					SrvTu2He + SrvTu3He/2 - SrvTu2oR, 
					-(SrvTu2Dz - SrvTu3Dz)+ ( SrvTu2Dz-SrvTu1Dz ) ) 
					* HepRotateX3D(-M_PI/2) );
						
  GeoLogVol*  lSrvTurret = new GeoLogVol( "ECTServiceTurretTower",
                                            &sSrvTurret,
                                            theMaterialManager->getMaterial(endcaptowerMaterial) );
  GeoPhysVol* pSrvTurret = new GeoPhysVol(lSrvTurret);
  //------------------------------------------------------------------------------------------
  // Replicate and position

  // array of z positions
  double zPosSrvTurret[nZPos] = { -SToZpos - CryoZmoy + SToDzBot/2,
				    SToZpos + CryoZmoy - SToDzBot/2 };

  GENFUNCTION fTrlSrvTurret = ArrayFunction( zPosSrvTurret,  zPosSrvTurret     + nZPos );

  TRANSFUNCTION XFSrvTurret = Pow( HepTranslateZ3D(1.0), fTrlSrvTurret ) *
                                HepTranslateY3D( CryoT1 + SrvTu1He/2 ) *             // constant displacement
                                Pow( HepRotateY3D(1.0), fRot );

  GeoSerialTransformer* sxSrvTurret = new GeoSerialTransformer( pSrvTurret, &XFSrvTurret
                                                                , nZPos );

  container->add(sxSrvTurret);

}

std::string EndCapToroidBuilderRDB::getMaterial( std::string materialName ) 
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
    log  <<  " EndCapToroidBuilderRDB::getMaterial: material "  <<  materialName  <<  " not defined! "  
         <<  " Take Aluminium instead." 
         <<  endreq;
    return "std::Aluminium";           
  }  
}  


} // namespace MuonGM 


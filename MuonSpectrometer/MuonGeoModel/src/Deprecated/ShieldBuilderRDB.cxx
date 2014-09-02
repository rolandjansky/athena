/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/ShieldBuilderRDB.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "MuonGeoModel/ArrayFunction.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
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

typedef std::stringstream  my_sstream;
typedef std::ostringstream my_osstream;
using namespace Genfun;
using namespace GeoXF;


namespace MuonGM {


ShieldBuilderRDB::ShieldBuilderRDB( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
                                    std::string    geoNode)     :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
  m_msgSvc = Athena::getMessageSvc();
  MsgStream log(m_msgSvc, "MuGM:ShieldBuildRDB" );
  log  <<  MSG::INFO  <<  "Fetching data with tag <"  <<  geoTag  <<"> node <"<<geoNode<<">" <<  endreq;
  m_Jdsh = pRDBAccess->getRecordsetPtr( "JDSH",geoTag, geoNode);
  if (m_Jdsh->size() == 0 ) 
  {
      log<<MSG::WARNING<<"Table JDSH not found in tag <"  <<  geoTag <<  ">"  <<" reading table JDSH-00" <<endreq;
      m_Jdsh = pRDBAccess->getRecordsetPtr( "JDSH","JDSH-00" );
  }
  m_Jfsh = pRDBAccess->getRecordsetPtr( "JFSH",geoTag, geoNode);
  if (m_Jfsh->size() == 0)
  {
      log<<MSG::WARNING<<"Table JFSH not found in tag <"  <<  geoTag <<  ">"  <<" reading table JFSH-00" <<endreq;
      m_Jfsh = pRDBAccess->getRecordsetPtr( "JFSH","JFSH-00" );
  }
  m_Jtsh = pRDBAccess->getRecordsetPtr( "JTSH",geoTag, geoNode);
  if (m_Jtsh->size() == 0)
  {
      log<<MSG::WARNING<<"Table JTSH not found in tag <"  <<  geoTag <<  ">"  <<" reading table JTSH-00" <<endreq;
      m_Jtsh = pRDBAccess->getRecordsetPtr( "JTSH","JTSH-00" );
  }
  

}

void ShieldBuilderRDB::buildDiskShielding( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds Disk Shielding JD (after the LAr end-cap)
  //  References: drawings
  //    - ATLJD___0038, ATLJT___0044, ATLJT___0046 (back and front disks)
  //    - ATLJD___0076, ATLJT___0080-81            (plugs, tube)
  //    - ATLJD___0063, ATLJD___0066, ATLJD___0069 (cone segments - brass, poly, lead)
  //    - ATLJD___0035, ATLJT___0085-88            (hub and hub cladding)
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//---------- JD Parameters from AMDB Q -------------------------------------------------------
   const double FDinnRad = (*m_Jdsh)[0]->getFloat("FDINNRAD") * mm;
   const double FDoutRad = (*m_Jdsh)[0]->getFloat("FDOUTRAD") * mm;
   const double FDthickn = (*m_Jdsh)[0]->getFloat("FDTHICKN") * mm;
   const double BDinnRad = (*m_Jdsh)[0]->getFloat("BDINNRAD") * mm;
   const double BDoutRad = (*m_Jdsh)[0]->getFloat("BDOUTRAD") * mm;
   const double BDthickn = (*m_Jdsh)[0]->getFloat("BDTHICKN") * mm;
   const double NomiZpos = (*m_Jdsh)[0]->getFloat("NOMIZPOS") * mm;
   const double CABCinRa = (*m_Jdsh)[0]->getFloat("CABCINRA") * mm;
   const double CABCouRa = (*m_Jdsh)[0]->getFloat("CABCOURA") * mm;
   const double CABCthi1 = (*m_Jdsh)[0]->getFloat("CABCTHI1") * mm;
   const double CABCthic = (*m_Jdsh)[0]->getFloat("CABCTHIC") * mm;
   const double CABCslAn = (*m_Jdsh)[0]->getFloat("CABCSLAN") * deg;
   const double CABCslLe = (*m_Jdsh)[0]->getFloat("CABCSLLE") * mm;
   const double CALCthic = (*m_Jdsh)[0]->getFloat("CALCTHIC") * mm;
   const double CALCleng = (*m_Jdsh)[0]->getFloat("CALCLENG") * mm;
   const double CALCwidt = (*m_Jdsh)[0]->getFloat("CALCWIDT") * mm;
   const double CAPCthic = (*m_Jdsh)[0]->getFloat("CAPCTHIC") * mm;
   const double CAPCleng = (*m_Jdsh)[0]->getFloat("CAPCLENG") * mm;
   const double CAPCwidt = (*m_Jdsh)[0]->getFloat("CAPCWIDT") * mm;
   const double CACOprAn = (*m_Jdsh)[0]->getFloat("CACOPRAN") * deg;
   //const double TUBEinRa = (*m_Jdsh)[0]->getFloat("TUBEINRA") * mm;
   const double TUBEOuR1 = (*m_Jdsh)[0]->getFloat("TUBEOUR1") * mm;
   const double TUBEOuR2 = (*m_Jdsh)[0]->getFloat("TUBEOUR2") * mm;
   const double TUBEslen = (*m_Jdsh)[0]->getFloat("TUBESLEN") * mm;
   const double TUBEleng = (*m_Jdsh)[0]->getFloat("TUBELENG") * mm;
   const double JDSH_Tube_BackDisk_innerRadius               =  275.0  * mm;
   const double PLUGinR1 = (*m_Jdsh)[0]->getFloat("PLUGINR1") * mm;
   const double PLUGouRa = (*m_Jdsh)[0]->getFloat("PLUGOURA") * mm;
   const double PLUGleng = (*m_Jdsh)[0]->getFloat("PLUGLENG") * mm;
   const double PLUGtiAn = (*m_Jdsh)[0]->getFloat("PLUGTIAN") * deg;
   const double SWHinnRa = (*m_Jdsh)[0]->getFloat("SWHINNRA") * mm;
   const double SWHoutR1 = (*m_Jdsh)[0]->getFloat("SWHOUTR1") * mm;
   const double SWHoutR2 = (*m_Jdsh)[0]->getFloat("SWHOUTR2") * mm;
   const double SWHlengt = (*m_Jdsh)[0]->getFloat("SWHLENGT") * mm;
                                                                            
   const std::string materialShieldSteel = getMaterial( "ShieldSteel" );    
   const std::string materialShieldBrass = getMaterial( "ShieldBrass" );    
   const std::string materialPolyboron   = getMaterial( "Polyboron" );      
   const std::string materialLead        = getMaterial( "Lead" );           
//                                                                          
//--------------------------------------------------------------------------------------------

  const double tolerance = 0.5 * mm;
  
  const int nZPos = 2;

  // array of rotation angles and generating function to produce mirror images
  double rotAngle[nZPos] = { 0, M_PI };
  GENFUNCTION fRot = ArrayFunction( rotAngle, rotAngle + nZPos ); 

  // array of phi values (centres of phi sectors)
  const double phi0Cone = CACOprAn;
  const int nPhiSectors = 16;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = 2 * i * phi0Cone;  

  // auxiliary arrays to store phi values, positions, rotation angles for all volumes to be replicated 
  double phiAux[ nPhiSectors * nZPos ], posAux[ nPhiSectors * nZPos ], rotAux[ nPhiSectors * nZPos ];
  
  
  // JD Shielding properties
//  const double rInTube               = JDSH_Tube_innerRadius; 
//  const double lengthTube            = JDSH_Tube_length;
  const double rInTubeBackDisk       = JDSH_Tube_BackDisk_innerRadius;
                                                              
  const double alpha    = CABCslAn;
  const double sinAlpha = sin(alpha);
  const double cosAlpha = cos(alpha);
  //const double tanAlpha = sinAlpha / cosAlpha;
  
  const double tanPhi0 = tan(phi0Cone);
   
  //------------------------------------------------------------------------------------------
  //  Build JD Front and Back Disks as cylindrical tubes
  //------------------------------------------------------------------------------------------
  // Front Disk
  GeoTube*    sFrontDisk = new GeoTube( FDinnRad, FDoutRad, FDthickn/2 ); 
  GeoLogVol*  lFrontDisk = new GeoLogVol( "DiskShieldingFrontDisk", 
                                          sFrontDisk, 
                                          theMaterialManager->getMaterial(materialShieldSteel) );
  GeoPhysVol* pFrontDisk = new GeoPhysVol(lFrontDisk);

  // Back Disk
  GeoTube*    sBackDisk = new GeoTube( BDinnRad,  BDoutRad, BDthickn/2 ); 
  GeoLogVol*  lBackDisk = new GeoLogVol( "DiskShieldingBackDisk", 
                                         sBackDisk, 
                                         theMaterialManager->getMaterial(materialShieldSteel) );
  GeoPhysVol* pBackDisk = new GeoPhysVol(lBackDisk);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // Front Disk
  // array of z positions  
  double zPosFrontDisk[nZPos] = { -NomiZpos - FDthickn/2,
                                   NomiZpos + FDthickn/2 };

  GENFUNCTION   fFrontDisk  = ArrayFunction( zPosFrontDisk, zPosFrontDisk + nZPos ); 
  TRANSFUNCTION XFFrontDisk = Pow( HepTranslateZ3D(1.0), fFrontDisk );
  
  GeoSerialTransformer* sxFrontDisk = new GeoSerialTransformer( pFrontDisk, &XFFrontDisk, nZPos );
  container->add(sxFrontDisk);
  // std::cout<<" 1  "<<std::endl;

  // Back Disk
  // array of z positions  
  double zPosBackDisk[nZPos] = { -NomiZpos - FDthickn - BDthickn/2,
                                  NomiZpos + FDthickn + BDthickn/2 };

  GENFUNCTION   fBackDisk  = ArrayFunction( zPosBackDisk, zPosBackDisk + nZPos ); 
  TRANSFUNCTION XFBackDisk = Pow( HepTranslateZ3D(1.0), fBackDisk );
  
  GeoSerialTransformer* sxBackDisk = new GeoSerialTransformer( pBackDisk, &XFBackDisk, nZPos );
  container->add(sxBackDisk);
  // std::cout<<" 2  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JD Plugs and Tube 
  //------------------------------------------------------------------------------------------
      
  //------------------------------------------------------------------------------------------
  //  Build JD Plugs (i.e., brass core of JD Central Tube) as a tube with conical cut-out,
  //  together with a small tube-shaped extension (within the clearing of the Central Tube 
  //  back disk; cf. drawing ATLJD___0081).
  //------------------------------------------------------------------------------------------
  // tube-shaped plugs extension
  GeoTube*    sPlugsExt = new GeoTube( PLUGinR1, rInTubeBackDisk, TUBEslen/2 - tolerance );
  GeoLogVol*  lPlugsExt = new GeoLogVol( "DiskShieldingPlugsExtension", 
                                         sPlugsExt, 
                                         theMaterialManager->getMaterial(materialShieldBrass) );
  GeoPhysVol* pPlugsExt = new GeoPhysVol(lPlugsExt);

  // tube-shaped core with conical cut-out  
  GeoTube* sCylinder = new GeoTube( 0, PLUGouRa, PLUGleng/2 - tolerance );

  // empty cone to cut out additional central clearing 
  const double tanBeta       = tan(PLUGtiAn);
  const double dzEmptyCone   = PLUGleng + 2 * PLUGinR1/tanBeta;
  const double rMaxEmptyCone = dzEmptyCone * tanBeta;
  GeoCons* sEmptyCone = new GeoCons( 0, 0, tolerance, rMaxEmptyCone, dzEmptyCone/2, 0, 2*M_PI ); 

  // perform cut-out
  HepTransform3D xfEmptyCone = HepRotateX3D(M_PI);
  const GeoShape& sPlugs = sCylinder->subtract( (*sEmptyCone) << xfEmptyCone );

  GeoLogVol*  lPlugs = new GeoLogVol( "DiskShieldingPlugs", 
                                      &sPlugs, 
                                      theMaterialManager->getMaterial(materialShieldBrass) );
  GeoPhysVol* pPlugs = new GeoPhysVol(lPlugs);

  // replicate and position both volumes
  //------------------------------------------------------------------------------------------
  // arrays of z positions
  double zPosPlugsExt[nZPos] = { -NomiZpos - TUBEslen/2,
                                  NomiZpos + TUBEslen/2  };
  double zPosPlugs[nZPos]    = { -NomiZpos - TUBEslen - PLUGleng/2,
                                  NomiZpos + TUBEslen + PLUGleng/2 };

  GENFUNCTION   fTrlPlugsExt = ArrayFunction( zPosPlugsExt, zPosPlugsExt + nZPos ); 
  TRANSFUNCTION XFPlugsExt   = Pow( HepTranslateZ3D(1.0), fTrlPlugsExt );

  GeoSerialTransformer* sxPlugsExt = new GeoSerialTransformer( pPlugsExt, &XFPlugsExt, nZPos );
  container->add(sxPlugsExt);
  // std::cout<<" 3  "<<std::endl;

  GENFUNCTION   fTrlPlugs = ArrayFunction( zPosPlugs, zPosPlugs + nZPos ); 
  TRANSFUNCTION XFPlugs   = Pow( HepTranslateZ3D(1.0), fTrlPlugs ) * 
                            Pow( HepRotateX3D(1.0), fRot );
  
  GeoSerialTransformer* sxPlugs = new GeoSerialTransformer( pPlugs, &XFPlugs, nZPos );
  container->add(sxPlugs);
  // std::cout<<" 4  "<<std::endl;
      
  //------------------------------------------------------------------------------------------
  //  Build JD steel Tube, consisting of back disk and main tube; don't build front disk 
  //  for the time being (cf. drawings ATLJD___0076, ATLJD___0081) 
  //------------------------------------------------------------------------------------------
  // back disk
  GeoTube*    sTubeBackDisk = new GeoTube( rInTubeBackDisk, TUBEOuR1, TUBEslen/2 );
  GeoLogVol*  lTubeBackDisk = new GeoLogVol( "DiskShieldingTubeBackDisk", 
                                             sTubeBackDisk, 
                                             theMaterialManager->getMaterial(materialShieldSteel) );
  GeoPhysVol* pTubeBackDisk = new GeoPhysVol(lTubeBackDisk);

  // main tube
  GeoTube*    sMainTube = new GeoTube( PLUGouRa, TUBEOuR2, TUBEleng/2 );
  GeoLogVol*  lMainTube = new GeoLogVol( "DiskShieldingMainTube", 
                                         sMainTube, 
                                         theMaterialManager->getMaterial(materialShieldSteel) );
  GeoPhysVol* pMainTube = new GeoPhysVol(lMainTube);

  // replicate and position both volumes
  //------------------------------------------------------------------------------------------
  // arrays of z positions
  double zPosTubeBackDisk[nZPos] = { -NomiZpos - TUBEslen/2,
                                      NomiZpos + TUBEslen/2  };
  double zPosMainTube[nZPos]     = { -NomiZpos - TUBEslen - TUBEleng/2,
                                      NomiZpos + TUBEslen + TUBEleng/2 };

  GENFUNCTION   fTrlTubeBackDisk = ArrayFunction( zPosTubeBackDisk, zPosTubeBackDisk + nZPos ); 
  TRANSFUNCTION XFTubeBackDisk   = Pow( HepTranslateZ3D(1.0), fTrlTubeBackDisk );

  GeoSerialTransformer* sxTubeBackDisk = new GeoSerialTransformer( pTubeBackDisk, &XFTubeBackDisk, nZPos );
  container->add(sxTubeBackDisk);
  // std::cout<<" 5  "<<std::endl;

  GENFUNCTION   fTrlMainTube = ArrayFunction( zPosMainTube, zPosMainTube + nZPos ); 
  TRANSFUNCTION XFMainTube   = Pow( HepTranslateZ3D(1.0), fTrlMainTube );
  
  GeoSerialTransformer* sxMainTube = new GeoSerialTransformer( pMainTube, &XFMainTube, nZPos );
  container->add(sxMainTube);
  // std::cout<<" 6  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JD Cone, consisting of three layers (brass, polyboron, lead), where each layer 
  //  consists of 16 phi segments. 
  //------------------------------------------------------------------------------------------
      
  //------------------------------------------------------------------------------------------
  //  Build JD Brass Cone 
  //  A single Brass Cone segment is built from a trapezoidal base shape, where 
  //  a box  cuts out the space for the poly and lead cones.
  //------------------------------------------------------------------------------------------
 const double dRBrassCone = CABCouRa - CABCinRa;
  GeoTrd* sBrassConeTrd = new GeoTrd( CABCthic/2, CABCthic/2,
                                      CAPCwidt/2 - dRBrassCone * tanPhi0, CAPCwidt/2, 
                                      dRBrassCone/2 );

  // empty box to cut out the space for the poly and lead cones
  const double dThickness = CABCthic - CABCthi1;
  const double dXEmptyBox = dThickness * cosAlpha;
  const double dZEmptyBox = CABCslLe - dThickness * sinAlpha;
  GeoBox* sEmptyBox = new GeoBox( dXEmptyBox, CAPCwidt, dZEmptyBox );

  // perform cut-out 
  //------------------------------------------------------------------------------------------
  HepTransform3D xfEmptyBox   = HepTranslate3D( CABCthic/2, 0, dRBrassCone/2 ) * 
                                HepRotateY3D( -alpha );
  const GeoShape& sBrassCone = sBrassConeTrd->subtract( (*sEmptyBox)  << xfEmptyBox );

  GeoLogVol*  lBrassCone = new GeoLogVol( "DiskShieldingBrassCone", 
                                          &sBrassCone, 
                                          theMaterialManager->getMaterial(materialShieldBrass) );
  GeoPhysVol* pBrassCone = new GeoPhysVol(lBrassCone);

  //  replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions
  const double zValBrassCone = NomiZpos + FDthickn + BDthickn + CABCthic/2;
  double zPosBrassCone[nZPos] = { -zValBrassCone, zValBrassCone };
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nZPos; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = phiVal[ii];         // can be used for all JD Cone volumes in common
     posAux[i] = zPosBrassCone[jj];
     rotAux[i] = rotAngle[jj];       // can be used for all JD Cone volumes in common
  }

  GENFUNCTION fPhiReplication = ArrayFunction( phiAux, phiAux + nPhiSectors * nZPos ); 
  GENFUNCTION fRotCone        = ArrayFunction( rotAux, rotAux + nPhiSectors * nZPos ); 
  GENFUNCTION fTrlBrassCone   = ArrayFunction( posAux, posAux + nPhiSectors * nZPos ); 

  TRANSFUNCTION XFBrassCone = Pow( HepRotateZ3D(1.0), fPhiReplication ) * 
                              Pow( HepTranslateZ3D(1.0), fTrlBrassCone ) *
                              Pow( HepRotateY3D(1.0), fRotCone ) *
                              HepTranslateY3D( CABCinRa + dRBrassCone/2 ) * 
                              HepRotateZ3D( M_PI/2 ) *  
                              HepRotateY3D( M_PI/2 ); 
  GeoSerialTransformer* sxBrassCone = new GeoSerialTransformer( pBrassCone, 
                                                                &XFBrassCone, 
                                                                nPhiSectors * nZPos );
  container->add(sxBrassCone);
  // std::cout<<" 7  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JD Poly Cone, consisting of trapezoidal segments 
  //------------------------------------------------------------------------------------------
  const double dZPolyCone = CAPCleng;
  GeoTrd* sPolyCone = new GeoTrd( CAPCthic/2, CAPCthic/2,
                                  CAPCwidt/2 - dZPolyCone * tanPhi0, CAPCwidt/2, 
                                  dZPolyCone/2 );

  GeoLogVol*  lPolyCone = new GeoLogVol( "DiskShieldingPolyCone", 
                                         sPolyCone, 
                                         theMaterialManager->getMaterial(materialPolyboron) );
  GeoPhysVol* pPolyCone = new GeoPhysVol(lPolyCone);

  //  replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions
  const double zOffsetAfterDisks = zValBrassCone - CABCthic/2 + CABCthi1;
  const double zValPolyCone   = zOffsetAfterDisks + 
                                0.5 * ( CAPCthic * cosAlpha + dZPolyCone * sinAlpha );
  double zPosPolyCone[nZPos]  = { -zValPolyCone,  zValPolyCone };
    
  // re-define auxiliary array posAux 
  for ( int i = 0; i < nPhiSectors * nZPos; i++ )  
  {
     int jj = i / nPhiSectors;
     posAux[i] = zPosPolyCone[jj];
  }

  GENFUNCTION fTrlPolyCone = ArrayFunction( posAux, posAux + nPhiSectors * nZPos ); 

  const double yOffsetPolyCone = CABCouRa - 
                                 0.5 * ( -CAPCthic * sinAlpha + dZPolyCone * cosAlpha );
  TRANSFUNCTION XFPolyCone = Pow( HepRotateZ3D(1.0), fPhiReplication ) * 
                             Pow( HepTranslateZ3D(1.0), fTrlPolyCone ) *
                             Pow( HepRotateY3D(1.0), fRotCone ) *
                             HepTranslateY3D(yOffsetPolyCone) * 
                             HepRotateZ3D( M_PI/2 ) *  
                             HepRotateY3D( M_PI/2 - alpha ); 
  GeoSerialTransformer* sxPolyCone = new GeoSerialTransformer( pPolyCone, 
                                                               &XFPolyCone, 
                                                               nPhiSectors * nZPos );
  container->add(sxPolyCone);
  // std::cout<<" 8  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JD Lead Cone, consisting of trapezoidal segments 
  //------------------------------------------------------------------------------------------
  const double dZLeadCone = CALCleng;
  GeoTrd* sLeadCone = new GeoTrd( CALCthic/2, CALCthic/2,
                                  CALCwidt/2 - dZLeadCone * tanPhi0, CALCwidt/2, 
                                  dZLeadCone/2 );

  GeoLogVol*  lLeadCone = new GeoLogVol( "DiskShieldingLeadCone", 
                                         sLeadCone, 
                                         theMaterialManager->getMaterial(materialLead) );
  GeoPhysVol* pLeadCone = new GeoPhysVol(lLeadCone);

  //  replicate and position
  //------------------------------------------------------------------------------------------
  // array of z-positions
  const double zValLeadCone  = zOffsetAfterDisks + CAPCthic * cosAlpha + 
                               0.5 * ( CALCthic * cosAlpha + dZLeadCone * sinAlpha ) + 
			       ( dZPolyCone - dZLeadCone ) * sinAlpha;
  double zPosLeadCone[nZPos] = { -zValLeadCone,  zValLeadCone };
    
  // re-define auxiliary array posAux 
  for ( int i = 0; i < nPhiSectors * nZPos; i++ )  
  {
     int jj = i / nPhiSectors;
     posAux[i] = zPosLeadCone[jj];
  }

  GENFUNCTION fTrlLeadCone = ArrayFunction( posAux, posAux + nPhiSectors * nZPos ); 

  const double yOffsetLeadCone = CABCouRa - 
                                 0.5 * ( -CALCthic * sinAlpha + dZLeadCone * cosAlpha ) -
				 ( dZPolyCone - dZLeadCone ) * cosAlpha + 
				 CAPCthic * sinAlpha;
  TRANSFUNCTION XFLeadCone = Pow( HepRotateZ3D(1.0), fPhiReplication ) * 
                             Pow( HepTranslateZ3D(1.0), fTrlLeadCone ) *
                             Pow( HepRotateY3D(1.0), fRotCone ) *
                             HepTranslateY3D(yOffsetLeadCone) * 
                             HepRotateZ3D( M_PI/2 ) *  
                             HepRotateY3D( M_PI/2 - alpha ); 
  GeoSerialTransformer* sxLeadCone = new GeoSerialTransformer( pLeadCone, 
                                                               &XFLeadCone, 
                                                               nPhiSectors * nZPos );
  container->add(sxLeadCone);
  // std::cout<<" 9  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JD Small Wheel Hub, consisting of cone-shaped brass core and two layers of 
  //  cladding (polyboron, lead)
  //  Approximate as three concentric GeoCons's.
  //------------------------------------------------------------------------------------------
      
  //------------------------------------------------------------------------------------------
  //  Build JD Small Wheel Hub brass cone as a GeoCons
  //------------------------------------------------------------------------------------------
  GeoCons* sHubBrassCone = new GeoCons( SWHinnRa,   SWHinnRa, 
                                        SWHoutR2, SWHoutR1, SWHlengt/2, 
                                        0, 2*M_PI );

  GeoLogVol*  lHubBrassCone = new GeoLogVol( "DiskShieldingHubBrassCone", 
                                             sHubBrassCone, 
                                             theMaterialManager->getMaterial(materialShieldBrass) );
  GeoPhysVol* pHubBrassCone = new GeoPhysVol(lHubBrassCone);
      
  //------------------------------------------------------------------------------------------
  //  Build JD Small Wheel Hub polyboron cladding as a GeoCons  
  //------------------------------------------------------------------------------------------
  const double rOut1PolyCladding = SWHoutR1 + CAPCthic;
  const double rOut2PolyCladding = SWHoutR2 + CAPCthic;
  
  GeoCons* sPolyCladding = new GeoCons( SWHoutR2, SWHoutR1, 
                                        rOut2PolyCladding,  rOut1PolyCladding, SWHlengt/2, 
                                        0, 2*M_PI );

  GeoLogVol*  lPolyCladding = new GeoLogVol( "DiskShieldingPolyCladding", 
                                             sPolyCladding, 
                                             theMaterialManager->getMaterial(materialPolyboron) );
  GeoPhysVol* pPolyCladding = new GeoPhysVol(lPolyCladding);
      
  //------------------------------------------------------------------------------------------
  //  Build JD Small Wheel Hub lead cladding as a GeoCons 
  //------------------------------------------------------------------------------------------
  const double rOut1LeadCladding = rOut1PolyCladding + CALCthic;
  const double rOut2LeadCladding = rOut2PolyCladding + CALCthic;
  
  GeoCons* sLeadCladding = new GeoCons( rOut2PolyCladding, rOut1PolyCladding, 
                                        rOut2LeadCladding, rOut1LeadCladding, SWHlengt/2, 
                                        0, 2*M_PI );

  GeoLogVol*  lLeadCladding = new GeoLogVol( "DiskShieldingLeadCladding", 
                                             sLeadCladding, 
                                             theMaterialManager->getMaterial(materialLead) );
  GeoPhysVol* pLeadCladding = new GeoPhysVol(lLeadCladding);

  //------------------------------------------------------------------------------------------
  //  Replicate and position all JD Small Wheel Hub volumes
  //------------------------------------------------------------------------------------------
  // array of z-positions
  const double zValSmallWheelHub  = NomiZpos + FDthickn + BDthickn + 
                                    CABCthic + SWHlengt/2;
  double zPosSmallWheelHub[nZPos] = { -zValSmallWheelHub, zValSmallWheelHub };
    
  GENFUNCTION   fTrlSmallWheelHub = ArrayFunction( zPosSmallWheelHub, zPosSmallWheelHub + nZPos ); 
  TRANSFUNCTION XFSmallWheelHub   = Pow( HepTranslateZ3D(1.0), fTrlSmallWheelHub ) *
                                    Pow( HepRotateX3D(1.0), fRot ); 

  GeoSerialTransformer* sxSmallWheelHub = new GeoSerialTransformer( pHubBrassCone, &XFSmallWheelHub, nZPos );
  GeoSerialTransformer* sxPolyCladding  = new GeoSerialTransformer( pPolyCladding, &XFSmallWheelHub, nZPos );
  GeoSerialTransformer* sxLeadCladding  = new GeoSerialTransformer( pLeadCladding, &XFSmallWheelHub, nZPos );

  container->add(sxSmallWheelHub);
  container->add(sxPolyCladding);
  container->add(sxLeadCladding);
  // std::cout<<" 10  "<<std::endl;

}

void ShieldBuilderRDB::buildForwardShielding( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds JF Shielding (JF main cylinder and plug, JF octogon; TX1STM parts)
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//---------- JF Parameters from AMDB Q -------------------------------------------------------
   const double JFCMClen = (*m_Jfsh)[0]->getFloat("JFCMCLEN") * mm;           
   //const double JFCMCir1 = (*m_Jfsh)[0]->getFloat("JFCMCIR1") * mm;           
   const double JFCMCouR = (*m_Jfsh)[0]->getFloat("JFCMCOUR") * mm;           
   const double JFCMChSl = (*m_Jfsh)[0]->getFloat("JFCMCHSL") * deg;          
   const double JFCMCzof = (*m_Jfsh)[0]->getFloat("JFCMCZOF") * mm;           
   const double PLUGleng = (*m_Jfsh)[0]->getFloat("PLUGLENG") * mm;           
   const double PLUGinRa = (*m_Jfsh)[0]->getFloat("PLUGINRA") * mm;           
   const double PLUGouRa = (*m_Jfsh)[0]->getFloat("PLUGOURA") * mm;           
   const double JFSOCmRa = (*m_Jfsh)[0]->getFloat("JFSOCMRA") * mm;           
   const double JFSOClen = (*m_Jfsh)[0]->getFloat("JFSOCLEN") * mm;           
   const double JFSOCzof = (*m_Jfsh)[0]->getFloat("JFSOCZOF") * mm;           
   const double TX1e1oRa = (*m_Jfsh)[0]->getFloat("TX1E1ORA") * mm;           
   const double TX1e1iRa = (*m_Jfsh)[0]->getFloat("TX1E1IRA") * mm;           
   const double TX1e1iLe = (*m_Jfsh)[0]->getFloat("TX1E1ILE") * mm;           
   const double JFSH_tx1stm_element2_outerRadius1 =  1400.0 * mm;           
   const double TX1e2oRa = (*m_Jfsh)[0]->getFloat("TX1E2ORA") * mm;           
   const double TX1e2iRa = (*m_Jfsh)[0]->getFloat("TX1E2IRA") * mm;           
   const double TX1e2iLe = (*m_Jfsh)[0]->getFloat("TX1E2ILE") * mm;           
   const double TX1STzof = (*m_Jfsh)[0]->getFloat("TX1STZOF") * mm;           
                                                                            
   const std::string materialJFShield = getMaterial( "ShieldIron" );        

//---------- -- ECT Vacuum Vessel Parameters --------------------------------------------
   const double zMaxECTVessel = 12906 * mm;
  
  const double phi0      = M_PI/8; 
  const double cosPhi0   = cos(phi0);
  const double tolerance = 1.0 * mm;
  
  const int nZPos = 2;
  
  // array of rotation angles and generating function to produce mirror images
  double rotAngle[nZPos] = { 0, M_PI };
  GENFUNCTION fRot = ArrayFunction( rotAngle, rotAngle + nZPos ); 
  
  // JF Shielding properties
  // const double rInner1JFC2MainCylinder = JFSH_JFCMainCylinder_innerRadius1;
  const double rOuter1TX1STMElem2      = JFSH_tx1stm_element2_outerRadius1;

  //------------------------------------------------------------------------------------------
  //  Build JF Main Cylinder Plug 
  //------------------------------------------------------------------------------------------
  GeoTube* sPlug = new GeoTube( PLUGinRa, PLUGouRa, PLUGleng/2 ); 
  
  GeoLogVol*  lPlug = new GeoLogVol( "ForwardShieldingPlug", 
                                     sPlug, 
                                     theMaterialManager->getMaterial(materialJFShield) );
  GeoPhysVol* pPlug = new GeoPhysVol(lPlug);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosPlug[nZPos] = { -zMaxECTVessel - JFCMCzof + PLUGleng/2,
                              zMaxECTVessel + JFCMCzof - PLUGleng/2 };

  GENFUNCTION   fPlug  = ArrayFunction( zPosPlug, zPosPlug + nZPos ); 
  TRANSFUNCTION XFPlug = Pow( HepTranslateZ3D(1.0), fPlug );
  
  GeoSerialTransformer* sxPlug = new GeoSerialTransformer( pPlug, &XFPlug, nZPos );
  container->add(sxPlug);
  // std::cout<<" 11  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JF 'extended' Main Cylinder as a tube with conical cut-out.
  //  Consists of parts JFC1, JFC2, JFC3 (cf. drawings ATLJF___0006, ATLJF___0003, and 
  //  ATLJF___0004) together with the part of TX1STM within the JF Octogon (cf. ATLJF___0010).
  //------------------------------------------------------------------------------------------
  double lengthExtCylinder = JFCMClen + JFSOClen;
  GeoTube* sCylinder = new GeoTube( PLUGinRa, JFCMCouR, lengthExtCylinder/2 - tolerance );

  // empty cone to cut out additional central clearing 
  const double tanAlpha      = tan(JFCMChSl);
  const double dzEmptyCone   = lengthExtCylinder + 2 * PLUGinRa/tanAlpha;
  const double rMaxEmptyCone = dzEmptyCone * tanAlpha;
  GeoCons* sEmptyCone = new GeoCons( 0, 0, tolerance, rMaxEmptyCone, dzEmptyCone/2, 0, 2*M_PI ); 

  // perform cut-out
  HepTransform3D xfEmptyCone = HepRotateX3D(M_PI);
  const GeoShape& sExtCylinder = sCylinder->subtract( (*sEmptyCone) << xfEmptyCone );

  GeoLogVol*  lExtCylinder = new GeoLogVol( "ForwardShieldingMainCylinder", 
                                            &sExtCylinder, 
                                            theMaterialManager->getMaterial(materialJFShield) );
  GeoPhysVol* pExtCylinder = new GeoPhysVol(lExtCylinder);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosExtCylinder[nZPos] = { -zMaxECTVessel - JFCMCzof - lengthExtCylinder/2,
                                     zMaxECTVessel + JFCMCzof + lengthExtCylinder/2 };

  GENFUNCTION   fTrlExtCylinder = ArrayFunction( zPosExtCylinder, zPosExtCylinder + nZPos ); 
  TRANSFUNCTION XFExtCylinder   = Pow( HepTranslateZ3D(1.0), fTrlExtCylinder ) * 
                                  Pow( HepRotateX3D(1.0), fRot );
  
  GeoSerialTransformer* sxExtCylinder = new GeoSerialTransformer( pExtCylinder, &XFExtCylinder, nZPos );
  container->add(sxExtCylinder);
  // std::cout<<" 12  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build Main JF Octogon as an octogon with cylindrical cut-out.
  //------------------------------------------------------------------------------------------
  GeoPgon* sOctogon = new GeoPgon( phi0, 2*M_PI, 8 );
  sOctogon->addPlane( -JFSOClen/2, JFCMCouR/2, JFSOCmRa/cosPhi0 );
  sOctogon->addPlane(  JFSOClen/2, JFCMCouR/2, JFSOCmRa/cosPhi0 );

  // empty tube to cut out the central clearing
  GeoTube* sEmptyTube = new GeoTube( 0, JFCMCouR + tolerance, JFSOClen ); 
  
  // perform cut-out
  const GeoShape& sJFOctogon = sOctogon->subtract( *sEmptyTube );
  GeoLogVol*  lJFOctogon = new GeoLogVol( "ForwardShieldingOctogon", 
                                          &sJFOctogon, 
                                          theMaterialManager->getMaterial(materialJFShield) );
  GeoPhysVol* pJFOctogon = new GeoPhysVol(lJFOctogon);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosJFOctogon[nZPos] = { -zMaxECTVessel - JFSOCzof - JFSOClen/2,
                                   zMaxECTVessel + JFSOCzof + JFSOClen/2 };

  GENFUNCTION   fTrlJFOctogon = ArrayFunction( zPosJFOctogon, zPosJFOctogon + nZPos ); 
  TRANSFUNCTION XFJFOctogon   = Pow( HepTranslateZ3D(1.0), fTrlJFOctogon );
  
  GeoSerialTransformer* sxJFOctogon = new GeoSerialTransformer( pJFOctogon, &XFJFOctogon, nZPos );
  container->add(sxJFOctogon);
  // std::cout<<" 13  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build TX1STM part outside the JF Octogon as a tube and a cone.
  //------------------------------------------------------------------------------------------

  //------------------------------------------------------------------------------------------
  //  Build TX1STM tube-shaped part 
  //------------------------------------------------------------------------------------------
  GeoTube* sTX1STMTube = new GeoTube( TX1e1iRa, TX1e1oRa, TX1e1iLe/2 ); 
  
  GeoLogVol*  lTX1STMTube = new GeoLogVol( "ForwardShieldingTX1STMTube", 
                                           sTX1STMTube, 
                                           theMaterialManager->getMaterial(materialJFShield) );
  GeoPhysVol* pTX1STMTube = new GeoPhysVol(lTX1STMTube);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosTX1STMTube[nZPos] = { -zMaxECTVessel - TX1STzof - TX1e1iLe/2,
                                    zMaxECTVessel + TX1STzof + TX1e1iLe/2 };

  GENFUNCTION   fTX1STMTube  = ArrayFunction( zPosTX1STMTube, zPosTX1STMTube + nZPos ); 
  TRANSFUNCTION XFTX1STMTube = Pow( HepTranslateZ3D(1.0), fTX1STMTube );
  
  GeoSerialTransformer* sxTX1STMTube = new GeoSerialTransformer( pTX1STMTube, &XFTX1STMTube, nZPos );
  container->add(sxTX1STMTube);
  // std::cout<<" 14  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build TX1STM cone-shaped part 
  //------------------------------------------------------------------------------------------
  GeoCons* sTX1STMCone = new GeoCons( TX1e2iRa,  TX1e2iRa, 
                                      TX1e2oRa, rOuter1TX1STMElem2, 
                                      TX1e2iLe/2, 
                                      0, 2*M_PI ); 
                                      
  GeoLogVol*  lTX1STMCone = new GeoLogVol( "ForwardShieldingTX1STMCone", 
                                           sTX1STMCone, 
                                           theMaterialManager->getMaterial(materialJFShield) );
  GeoPhysVol* pTX1STMCone = new GeoPhysVol(lTX1STMCone);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  const double zOffsetTX1STMCone = TX1STzof + TX1e1iLe;
  double zPosTX1STMCone[nZPos]   = { -zMaxECTVessel - zOffsetTX1STMCone - TX1e2iLe/2,
                                      zMaxECTVessel + zOffsetTX1STMCone + TX1e2iLe/2 };

  GENFUNCTION   fTrlTX1STMCone = ArrayFunction( zPosTX1STMCone, zPosTX1STMCone + nZPos ); 
  TRANSFUNCTION XFTX1STMCone   = Pow( HepTranslateZ3D(1.0), fTrlTX1STMCone ) * 
                                 Pow( HepRotateX3D(1.0), fRot );
  
  GeoSerialTransformer* sxTX1STMCone = new GeoSerialTransformer( pTX1STMCone, &XFTX1STMCone, nZPos );
  container->add(sxTX1STMCone);
  // std::cout<<" 15  "<<std::endl;

}

void ShieldBuilderRDB::buildToroidShielding( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds JTT Shielding (within the ECT Central Tube)
  //  References: drawings ATLJT___0001, ATLJT___0014-16, ATLJT___0018-19
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//---------- JT Parameters from AMDB Q -------------------------------------------------------
//                                                                          
   const double OPlength = (*m_Jtsh)[0]->getFloat("OPLENGTH") * mm;          
   const double OPinnRad = (*m_Jtsh)[0]->getFloat("OPINNRAD") * mm;          
   const double OPoutRad = (*m_Jtsh)[0]->getFloat("OPOUTRAD") * mm;          
   const double IPlength = (*m_Jtsh)[0]->getFloat("IPLENGTH") * mm;          
   const double IPinnRa1 = (*m_Jtsh)[0]->getFloat("IPINNRA1") * mm;          
   const double IPoutRad = (*m_Jtsh)[0]->getFloat("IPOUTRAD") * mm;          
   const double IPtiltAn = (*m_Jtsh)[0]->getFloat("IPTILTAN") * deg;         
   const double IPzoffse = (*m_Jtsh)[0]->getFloat("IPZOFFSE") * mm;          
   const double PRlength = (*m_Jtsh)[0]->getFloat("PRLENGTH") * mm;          
   const double PRinnRad = (*m_Jtsh)[0]->getFloat("PRINNRAD") * mm;          
   const double PRoutRad = (*m_Jtsh)[0]->getFloat("PROUTRAD") * mm;          
   const double PRexttAn = (*m_Jtsh)[0]->getFloat("PREXTTAN") * deg;         
   const double PRipexle = (*m_Jtsh)[0]->getFloat("PRIPEXLE") * mm;          
   const double zpositio = (*m_Jtsh)[0]->getFloat("ZPOSITIO") * mm;          
                                                                            
   const std::string materialJTPlugs = getMaterial( "ShieldBrass" );        
   const std::string materialJTRings = getMaterial( "Polyboron" );          
//                                                                          
//--------------------------------------------------------------------------------------------
  
  const double tolerance = 0.5 * mm; 
  
  const int nZPos = 2;

  // array of rotation angles and generating function to produce mirror images
  double rotAngle[nZPos] = { 0, M_PI };
  GENFUNCTION fRot = ArrayFunction( rotAngle, rotAngle + nZPos ); 
  
  // JT Shielding properties
  const double zPosJTOuterPlugs      = zpositio + PRipexle; 
   
  //------------------------------------------------------------------------------------------
  //  Build JTT Outer Plugs as cylindrical tube 
  //------------------------------------------------------------------------------------------
  GeoTube* sOuterPlugs = new GeoTube( OPinnRad, OPoutRad, OPlength/2 ); 
  
  GeoLogVol*  lOuterPlugs = new GeoLogVol( "ToroidShieldingOuterPlugs", 
                                           sOuterPlugs, 
                                           theMaterialManager->getMaterial(materialJTPlugs) );
  GeoPhysVol* pOuterPlugs = new GeoPhysVol(lOuterPlugs);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosOuterPlugs[nZPos] = { -zPosJTOuterPlugs - OPlength/2,
                                    zPosJTOuterPlugs + OPlength/2 };

  GENFUNCTION   fOuterPlugs  = ArrayFunction( zPosOuterPlugs, zPosOuterPlugs + nZPos ); 
  TRANSFUNCTION XFOuterPlugs = Pow( HepTranslateZ3D(1.0), fOuterPlugs );
  
  GeoSerialTransformer* sxOuterPlugs = new GeoSerialTransformer( pOuterPlugs, &XFOuterPlugs, nZPos );
  container->add(sxOuterPlugs);
  // std::cout<<" 16  "<<std::endl;
 
  //------------------------------------------------------------------------------------------
  //  Build JTT Inner Plugs as cylindrical tube with conical cut-out 
  //------------------------------------------------------------------------------------------
  GeoTube* sInnerTube = new GeoTube( 0, IPoutRad, IPlength/2 - tolerance );

  // empty cone to cut out additional central clearing 
  const double tanAlpha      = tan(IPtiltAn);
  const double dzEmptyCone   = IPlength + 2 * IPinnRa1/tanAlpha;
  const double rMaxEmptyCone = dzEmptyCone * tanAlpha;
  GeoCons* sEmptyCone = new GeoCons( 0, 0, tolerance, rMaxEmptyCone, dzEmptyCone/2, 0, 2*M_PI ); 

  // perform cut-out
  HepTransform3D xfEmptyCone = HepRotateX3D(M_PI);
  const GeoShape& sInnerPlugs = sInnerTube->subtract( (*sEmptyCone) << xfEmptyCone );

  GeoLogVol*  lInnerPlugs = new GeoLogVol( "ToroidShieldingInnerPlugs", 
                                           &sInnerPlugs, 
                                           theMaterialManager->getMaterial(materialJTPlugs) );
  GeoPhysVol* pInnerPlugs = new GeoPhysVol(lInnerPlugs);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions

  //.xu +-40.
  double zPosInnerPlugs[nZPos] = { -zpositio - IPzoffse - IPlength/2 - 40.,
                                    zpositio + IPzoffse + IPlength/2 + 40.};

  MsgStream log(m_msgSvc, "MuGM:ShieldBuildRDB" ); //log  <<  MSG::INFO << endreq;

  GENFUNCTION   fInnerPlugs  = ArrayFunction( zPosInnerPlugs, zPosInnerPlugs + nZPos ); 
  TRANSFUNCTION XFInnerPlugs = Pow( HepTranslateZ3D(1.0), fInnerPlugs ) * 
                               Pow( HepRotateX3D(1.0), fRot );
  
  GeoSerialTransformer* sxInnerPlugs = new GeoSerialTransformer( pInnerPlugs, &XFInnerPlugs, nZPos );
  container->add(sxInnerPlugs);
  // std::cout<<" 17  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build JTT Poly Rings as a tube and a cone.
  //------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------
  //  Build Poly Rings tube-shaped part 
  //------------------------------------------------------------------------------------------
  GeoTube*    sPolyRingsTube = new GeoTube( PRinnRad, PRoutRad, PRlength/2 ); 
  GeoLogVol*  lPolyRingsTube = new GeoLogVol( "ToroidShieldingPolyRingsTube", 
                                              sPolyRingsTube, 
                                              theMaterialManager->getMaterial(materialJTRings) );
  GeoPhysVol* pPolyRingsTube = new GeoPhysVol(lPolyRingsTube);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  const double zOffsetPolyRingsTube = OPlength - PRlength;
  double zPosPolyRingsTube[nZPos]   = { -zPosJTOuterPlugs - zOffsetPolyRingsTube - PRlength/2,
                                         zPosJTOuterPlugs + zOffsetPolyRingsTube + PRlength/2 };

  GENFUNCTION   fPolyRingsTube  = ArrayFunction( zPosPolyRingsTube, zPosPolyRingsTube + nZPos ); 
  TRANSFUNCTION XFPolyRingsTube = Pow( HepTranslateZ3D(1.0), fPolyRingsTube );
  
  GeoSerialTransformer* sxPolyRingsTube = new GeoSerialTransformer( pPolyRingsTube, &XFPolyRingsTube, nZPos );
  container->add(sxPolyRingsTube);
  // std::cout<<" 18  "<<std::endl;

  //------------------------------------------------------------------------------------------
  //  Build Poly Rings cone-shaped part 
  //------------------------------------------------------------------------------------------
  const double tanBeta = tan(PRexttAn);
  const double deltaZ  = ( PRoutRad - PRinnRad ) / tanBeta;
  GeoCons* sPolyRingsCone = new GeoCons( PRinnRad,  PRinnRad, 
                                         PRoutRad, PRinnRad + tolerance, 
                                         deltaZ/2, 
                                         0, 2*M_PI ); 
                                      
  GeoLogVol*  lPolyRingsCone = new GeoLogVol( "ToroidShieldingPolyRingsCone", 
                                              sPolyRingsCone, 
                                              theMaterialManager->getMaterial(materialJTRings) );
  GeoPhysVol* pPolyRingsCone = new GeoPhysVol(lPolyRingsCone);

  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of z positions
  double zPosPolyRingsCone[nZPos] = { -zPosJTOuterPlugs - zOffsetPolyRingsTube + deltaZ/2,
                                       zPosJTOuterPlugs + zOffsetPolyRingsTube - deltaZ/2 };

  GENFUNCTION   fTrlPolyRingsCone = ArrayFunction( zPosPolyRingsCone, zPosPolyRingsCone + nZPos ); 
  TRANSFUNCTION XFPolyRingsCone   = Pow( HepTranslateZ3D(1.0), fTrlPolyRingsCone ) * 
                                    Pow( HepRotateX3D(1.0), fRot );
  
  GeoSerialTransformer* sxPolyRingsCone = new GeoSerialTransformer( pPolyRingsCone, &XFPolyRingsCone, nZPos );
  container->add(sxPolyRingsCone);
  // std::cout<<" 19  "<<std::endl;

}

std::string ShieldBuilderRDB::getMaterial( std::string materialName ) 
{
  MsgStream log(m_msgSvc, "MuonGeoModel" );
   
  if ( materialName == "Lead" )   
  {
    return "std::Lead";
  }
  else if ( materialName == "ShieldSteel"  ||  materialName == "ShieldIron"   ||  
            materialName == "ShieldBrass"  ||  materialName == "Polylithium"  || materialName == "Polyboron" )   
  {
    return "shield::" + materialName;
  }
  else 
  {
    log  <<  MSG::WARNING
         <<  " ShieldBuilderRDB::getMaterial: Material "  <<  materialName  <<  " not defined! "  
         <<  " Take Iron instead." 
         <<  endreq;
    return "std::Iron";           
  }  
}   

} // namespace MuonGM

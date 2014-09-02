/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/BarrelToroidBuilderRDB.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
                                                        
#include "MuonGeoModel/ArrayFunction.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoCons.h"
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
BarrelToroidBuilderRDB::BarrelToroidBuilderRDB( StoreGateSvc  *pDetStore,
						IRDBAccessSvc *pRDBAccess, 
                                		std::string    geoTag,
						std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
  MsgStream log( Athena::getMessageSvc(), "MuGM:BarrelToroBuildRDB" );

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
  
  std::string Iron = "Iron";
  std::string Aluminium = "Aluminium";


}

  void BarrelToroidBuilderRDB::buildBTVacuumVessel( GeoPhysVol* container ) 
{
  //------------------------------------------------------------------------------------------
  //  Builds BT Vacuum Vessel (tubes, ribs, and voussoir attachments)
  //------------------------------------------------------------------------------------------

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int maxDim = 8; 

//---------
   const double rMinBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMIN") * mm;                        
   const double rMaxBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMAX") * mm;  
   const double zMaxBTVessel      = (*m_Abrt)[0]->getFloat("CRYOZMAX") * mm;         
   const double rCurvBTVessel     = (*m_Abrt)[0]->getFloat("CRYORCUR") * mm;                        
   const double radiusBTVessel    = (*m_Abrt)[0]->getFloat("CRYORADI") * mm;       
   const double thicknessBTVessel = (*m_Abrt)[0]->getFloat("CRYORADT") * mm;                        
   const double zPosBTCryoring    = -1030.0 * mm; //(*m_Abrt)[0]->getFloat("CRYRNGZM") * mm;
   const double radiusBTCryoring  = (*m_Abrt)[0]->getFloat("CRYRNGRA") * mm;
                                                                              //
   const std::string vesselMaterial = getMaterial("Iron"); 
//------------
   int nBTRibs = 7;                                                  
   double zPosBTRib[maxDim];                                                  //
 
 for (int i = 0; i<nBTRibs; i++)
    zPosBTRib[i] = (*m_Abrt)[0]->getFloat("ZRIB",i) * mm;        
                                                                       //
   const double heightBTRib = (*m_Abrt)[0]->getFloat("CRYRIBYW") * mm;                              
   const double zWidthBTRib = (*m_Abrt)[0]->getFloat("CRYRIBZL") * mm; 
   const double CryRiWYp    = (*m_Abrt)[0]->getFloat("CRYRIWYP") * mm;
   const double CryRiWYn    = (*m_Abrt)[0]->getFloat("CRYRIWYN") * mm;
   const double CryRiWXp    = (*m_Abrt)[0]->getFloat("CRYRIWXP") * mm;
   const double CryRiWXn    = (*m_Abrt)[0]->getFloat("CRYRIWXN") * mm;
   const double CryRiWTh    = (*m_Abrt)[0]->getFloat("CRYRIWTH") * mm;
                                                                                
//----------------------------------------------------------------------------//

   const double DiamHoleOutLongTube   = (*m_Abrt)[0]->getFloat("CRYATTD0") * mm; //use also for building vosoir attachment
   const double DiamHoleInLongTube    = (*m_Abrt)[0]->getFloat("CRYATTD1") * mm; //see up...
   const double lengthTubeVoussAtt    = (*m_Abrt)[0]->getFloat("CRYATTXH") * mm;
   const double thicknessTubeVoussAtt = (*m_Abrt)[0]->getFloat("CRYATTTH") * mm;
   const double PosEdgeTubeVoussAtt   = (*m_Abrt)[0]->getFloat("CRYATTRX") * mm;
   const double lengthConeVoussAtt    = (*m_Abrt)[0]->getFloat("CRYATTXS") * mm;
   const double CryAtWiY	      = (*m_Abrt)[0]->getFloat("CRYATWIY") * mm;
   const double CryAtWXp	      = (*m_Abrt)[0]->getFloat("CRYATWXP") * mm;
   const double CryAtWXn	      = (*m_Abrt)[0]->getFloat("CRYATWXN") * mm;
   const double CryAtWBo	      = (*m_Abrt)[0]->getFloat("CRYATWBO") * mm;
   const double CryAtWTh	      = (*m_Abrt)[0]->getFloat("CRYATWTH") * mm;
   const double CryAtWZe	      = (*m_Abrt)[0]->getFloat("CRYATWZE") * mm;
   const double CryAtWRa	      = (*m_Abrt)[0]->getFloat("CRYATWRA") * mm;
   const double CryAtWYc	      = (*m_Abrt)[0]->getFloat("CRYATWYC") * mm;
   

//!!! Fe50 (diluted iron, rho=1/2*rho_fe) ia seichas ispol'zuiu iron                                                                              //
   const std::string voussAttMaterial = getMaterial("Iron"); 
   const std::string voussoirMaterial = getMaterial("Aluminium");
//----------------------------------------------------------------------------//

   int nBTVouss = 8;                                               //
   double zPosBTVouss[maxDim];                                                
   for ( int i = 0; i < nBTVouss; i++ )  zPosBTVouss[i] = (*m_Abrt)[0]->getFloat("ZVOUSS",i) * mm; 
                                                                              
   //never used   const double zWidthBTVouss = (*m_Abrt)[0]->getFloat("VOUBLZLE") * mm;                            
                                                                              
//----------------------------------------------------------------------------//
  
  const double tolerance = 0.5 * mm;//50

  const double phi0    = M_PI/8;
  const double sinPhi0 = sin(phi0);
  const double cosPhi0 = cos(phi0);
  const double tanPhi0 = tan(phi0);

  // array of phi values (centres of phi sectors)
  const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = ( 2*i + 1 ) * phi0;  

  // auxiliary arrays to store phi values, positions, rotation angles for all volumes to be replicated 
  const int nPosMax = maxDim;
  double phiAux[ nPhiSectors * nPosMax ], pos1Aux[ nPhiSectors * nPosMax ], pos2Aux[ nPhiSectors * nPosMax ], 
         rotAngleAux[ nPhiSectors * nPosMax ], pos3Aux[ nPhiSectors * nPosMax];

  //------------------------------------------------------------------------------------------
  //  Build BT Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------


  // Vacuum Vessel properties 
  const double distToCorner = sqrt(2) * rCurvBTVessel * tanPhi0; 
  const double rIn          = radiusBTVessel - thicknessBTVessel; 
  const double rOut         = radiusBTVessel; 
  const double distToOut    = sqrt(2) * ( rCurvBTVessel + rOut ) * tanPhi0; 
  const double zMaxL        = 2 * ( zMaxBTVessel - distToOut ); 
  const double deltaR       = rMaxBTVessel - rMinBTVessel;
  const double zMaxS        = deltaR - 2 * distToOut; 
  const double zMaxC        = 2 * ( rCurvBTVessel + rOut ) * tanPhi0; 
  const double rMean        = 0.5 * ( rMinBTVessel + rMaxBTVessel );  
  
  // Hole diameter at long tube & voussoir attachments properties 
  const double HoleDiam         = DiamHoleOutLongTube;
  const double HoleDiamIn       = DiamHoleInLongTube;
  const double lengthTubeVAt    = lengthTubeVoussAtt;
  const double thicknessTubeVAt = thicknessTubeVoussAtt;
  const double PosEdgeTubeVAt   = PosEdgeTubeVoussAtt;
  const double lengthConeVAt    = lengthConeVoussAtt;
      
  // positioning variables
  const double xLong   = deltaR/2 - radiusBTVessel;
  const double zShort  = zMaxBTVessel - radiusBTVessel;
  const double xCorner = xLong  - distToCorner/2;
  const double zCorner = zShort - distToCorner/2;

  //------------------------------------------------------------------------------------------
  // volumes for boolean subtraction:

  // (large enough) empty box to be subtracted from sides
  const double deltaXBox = 2 * rOut/cosPhi0;
  const double deltaYBox = 2 * rOut;
  const double deltaZBox = 2 * rOut * sinPhi0;
  const double xPosBox   = rOut;  
  
  GeoBox* sEmptyBox = new GeoBox( deltaXBox , deltaYBox, deltaZBox );

  HepRotateY3D    rotLeftBox(-phi0), rotRightBox(phi0);
  HepTranslateX3D trlBox(xPosBox);
  
  // (large enough) empty box to cut out the Rib clearances
  GeoBox* sEmptyRibBox = new GeoBox( rOut/4, heightBTRib/2, zWidthBTRib/2 );

  HepTransform3D trlEmptyRibBox[7];
  for (int i=0; i < nBTRibs; i++)
  {
  trlEmptyRibBox[i] = HepTranslateZ3D(zPosBTRib[i]) * HepTranslateX3D(rOut);
  }
  
  GeoTube* sEmptyTubeVo = new GeoTube( 0., HoleDiam/2, rOut );

  HepTransform3D trlEmptyTubeVo[8];
  for (int i=0; i < 8; i++)
  {
  trlEmptyTubeVo[i] = HepTranslateZ3D(zPosBTVouss[i]) * HepTranslateX3D(rOut)
		      * HepRotateY3D(M_PI/2);
  }
  
  GeoTube* sEmptyTubeCryR = new GeoTube( 0, radiusBTCryoring, rOut );
    
  HepTransform3D trlEmptyTubeCryR1 = HepTranslate3D( rOut * sinPhi0, rOut * cosPhi0, zPosBTCryoring ) * 
				     HepRotateY3D(M_PI/2) * HepRotateX3D(-M_PI/2 + phi0);
  HepTransform3D trlEmptyTubeCryR2 = HepTranslate3D(rOut * sinPhi0, -rOut * cosPhi0, zPosBTCryoring ) *
				     HepRotateY3D(M_PI/2) * HepRotateX3D(M_PI/2 - phi0);
  
  //------------------------------------------------------------------------------------------
  // Build long upper and lower Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------

  const GeoShape* sLongTubeIn = new GeoTube( rIn, rOut, zMaxL/2 + tolerance ); 

  const GeoShape* sLongTubeOut= new GeoTube( rIn, rOut, zMaxL/2 + tolerance );
                                                                      
  //------------------------------------------------------------------------------------------
  // cut out for Voussoir ....
  for ( int i = 0; i < nBTVouss; i++ )
  {
    sLongTubeIn = &( sLongTubeIn->subtract( (*sEmptyTubeVo) << trlEmptyTubeVo[i] ) );
  } 
  //cut out for ribs 
  for (int i = 0; i < nBTRibs; i++)
  {
    sLongTubeIn = &( sLongTubeIn->subtract( (*sEmptyRibBox) << trlEmptyRibBox[i] ) ); 
  } 
  for (int i = 0; i < nBTRibs; i++)
  {
    sLongTubeOut = &( sLongTubeOut->subtract( (*sEmptyRibBox) << trlEmptyRibBox[i] ) );
  }
  
  //------------------------------------------------------------------------------------------
  // cut away left and right sides
  HepTransform3D trlLeft  = trlBox * HepTranslateZ3D(-zMaxL/2), 
                 trlRight = trlBox * HepTranslateZ3D( zMaxL/2);
  
  const GeoShape& sBevelledLongTubeIn = sLongTubeIn->subtract( (*sEmptyBox) << (trlLeft  * rotLeftBox) ).
                                                     subtract( (*sEmptyBox) << (trlRight * rotRightBox) );
  const GeoShape& sBevelledLongTubeOut= sLongTubeOut->subtract( (*sEmptyBox) << (trlLeft * rotLeftBox) ).
						      subtract( (*sEmptyBox) << (trlRight * rotRightBox) ).
						      subtract( (*sEmptyTubeCryR) << trlEmptyTubeCryR1 ).
						      subtract( (*sEmptyTubeCryR) << trlEmptyTubeCryR2 );

//inside long tube
  GeoLogVol*  lBevelledLongTubeIn = new GeoLogVol( "BTBevelledLongTubeIn", 
                                                 &sBevelledLongTubeIn, 
                                                 theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pBevelledLongTubeIn = new GeoPhysVol(lBevelledLongTubeIn);
    
//outside long tube  
  GeoLogVol*  lBevelledLongTubeOut = new GeoLogVol( "BTBevelledLongTubeOut",
						 &sBevelledLongTubeOut,
						 theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pBevelledLongTubeOut = new GeoPhysVol(lBevelledLongTubeOut);

  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosLongTube = 1;
  double xPosLongTubeIn[nPosLongTube]  = { rMean - xLong };  // x positions
  double xPosLongTubeOut[nPosLongTube] = { rMean + xLong };
  double angleLongTube[nPosLongTube] = { M_PI };                       // rotation angles (around z)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosLongTube; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = xPosLongTubeIn[jj];
     pos3Aux[i]     = xPosLongTubeOut[jj];
     rotAngleAux[i] = angleLongTube[jj];
  }

  GENFUNCTION fLongTube       = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosLongTube ); 
  GENFUNCTION fTrlLongTubeIn  = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosLongTube ); 
  GENFUNCTION fTrlLongTubeOut = ArrayFunction( pos3Aux,     pos3Aux     + nPhiSectors * nPosLongTube );
  GENFUNCTION fRotLongTube = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosLongTube ); 

  TRANSFUNCTION XFLongTubeIn = Pow( HepRotateZ3D(1.0), fLongTube ) * 
                               Pow( HepTranslateX3D(1.0), fTrlLongTubeIn );

  TRANSFUNCTION XFLongTubeOut= Pow( HepRotateZ3D(1.0), fLongTube ) * 
			       Pow( HepTranslateX3D(1.0), fTrlLongTubeOut ) *
			       Pow (HepRotateZ3D(1.0), fRotLongTube );
			       
  GeoSerialTransformer* sxLongTubeIn = new GeoSerialTransformer( pBevelledLongTubeIn,
                                                                 &XFLongTubeIn,
                                                                 nPhiSectors * nPosLongTube );
								 
  GeoSerialTransformer* sxLongTubeOut= new GeoSerialTransformer( pBevelledLongTubeOut,
								 &XFLongTubeOut,
								 nPhiSectors * nPosLongTube );
  container->add(sxLongTubeIn);
  container->add(sxLongTubeOut);

//--------------VoussoirAttWing---------

  GeoShape* sVoussoirAttWingBox = new GeoBox( CryAtWiY, CryAtWZe/2 + CryAtWTh, ( CryAtWXp - CryAtWXn
  					      - CryAtWBo - tolerance )/2 );
  GeoTrd* sVoussoirAttWingTrap1 = new GeoTrd( CryAtWiY, CryAtWiY - CryAtWBo, CryAtWZe/2 + CryAtWTh,
					      CryAtWZe/2 + CryAtWTh, CryAtWBo/2 );
  GeoTrd* sVoussoirAttWingTrap2 = new GeoTrd( CryAtWiY -( 2*radiusBTVessel + CryAtWXn )
					      *sin(phi0), CryAtWiY, CryAtWZe/2 + CryAtWTh, CryAtWZe/2
					      + CryAtWTh, radiusBTVessel + CryAtWXn/2 - tolerance );
  GeoBox* sEmptyVoussoirAttWingBox1 = new GeoBox( 2*radiusBTVessel, CryAtWZe/2, 3*radiusBTVessel );
  GeoBox* sEmptyVoussoirAttWingBox2 = new GeoBox( CryAtWYc, CryAtWZe, radiusBTVessel );
  GeoTubs* sEmptyVoussoirAttWingTube1 = new GeoTubs( CryAtWRa, 2*CryAtWRa, CryAtWZe, M_PI + phi0,
						   6 * phi0 );
  GeoTube* sEmptyVoussoirAttWingTube2 = new GeoTube( 0, radiusBTVessel + tolerance, CryAtWZe );

  const GeoShape& sVoussoirAttWing = sVoussoirAttWingBox->add( (*sVoussoirAttWingTrap1) << 
							  HepTranslateZ3D( ( CryAtWXp - 
							  CryAtWXn )/2 ) ).
							  add( (*sVoussoirAttWingTrap2) <<
							  HepTranslateZ3D( -( ( CryAtWXp - 
							  CryAtWBo )/2
							  + radiusBTVessel ) ) ).
							  subtract( (*sEmptyVoussoirAttWingBox1) << 
							  HepTranslateZ3D( ( CryAtWBo - CryAtWXp - 
							  CryAtWXn )/2 ) ).
							  subtract( (*sEmptyVoussoirAttWingBox2) << 
							  HepTranslateZ3D( ( CryAtWBo - CryAtWXp -
							  CryAtWXn )/2 
							  - radiusBTVessel )  ).
							  subtract( (*sEmptyVoussoirAttWingTube1) <<
							  HepTranslateZ3D( ( CryAtWBo - CryAtWXp -
                                                          CryAtWXn )/2 ) * HepRotateX3D(M_PI/2) ).
							  subtract( (*sEmptyVoussoirAttWingTube2) <<
							  HepTranslateZ3D( ( CryAtWBo - CryAtWXp -
                                                          CryAtWXn )/2 ) * HepRotateX3D(M_PI/2) );

  GeoLogVol* lVoussoirAttWing = new GeoLogVol( "BTVoussoirAttWing", &sVoussoirAttWing,
                                               theMaterialManager->getMaterial(voussoirMaterial) );
  GeoPhysVol* pVoussoirAttWing = new GeoPhysVol(lVoussoirAttWing);

  // replicate and position
  for (int i = 0; i < nPhiSectors * nBTVouss; i++ )
  {
    int ii = i % nPhiSectors,
        jj = i / nPhiSectors;
    phiAux[i] = phiVal[ii];
    pos1Aux[i] = zPosBTVouss[jj];
  }

  GENFUNCTION fVoussoirAttWing    = ArrayFunction( phiAux, phiAux + nPhiSectors * nBTVouss );
  GENFUNCTION fTrlVoussoirAttWing = ArrayFunction( pos1Aux, pos1Aux + nPhiSectors * nBTVouss );

  TRANSFUNCTION XFVoussoirAttWing = Pow( HepRotateZ3D(1.0), fVoussoirAttWing ) *
                              Pow( HepTranslateZ3D(1.0), fTrlVoussoirAttWing ) *
                              HepTranslateX3D( rMean - (CryAtWBo - CryAtWXp - CryAtWXn)/2 - xLong )
                              * HepRotateX3D(M_PI/2) * HepRotateY3D(M_PI/2);
  GeoSerialTransformer* sxVoussoirAttWing = new GeoSerialTransformer( pVoussoirAttWing,
                                                                &XFVoussoirAttWing,
                                                                nPhiSectors * nBTVouss );
  container->add(sxVoussoirAttWing);

  //------------------------------------------------------------------------------------------
  // Build short left and right Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------

  GeoTube* sShortTube = new GeoTube( rIn, rOut, zMaxS/2 - tolerance );

  //------------------------------------------------------------------------------------------
  // cut away left and right sides
  trlLeft  = trlBox * HepTranslateZ3D(-zMaxS/2); 
  trlRight = trlBox * HepTranslateZ3D( zMaxS/2);
  const GeoShape& sBevelledShortTube = sShortTube->subtract( (*sEmptyBox) << (trlLeft  * rotLeftBox) ).
                                                   subtract( (*sEmptyBox) << (trlRight * rotRightBox) );
  GeoLogVol*  lBevelledShortTube = new GeoLogVol( "BTBevelledShortTube", 
                                                  &sBevelledShortTube, 
                                                  theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pBevelledShortTube = new GeoPhysVol(lBevelledShortTube);

  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosShortTube = 2;
  double zPosShortTube[nPosShortTube]  = { -zShort, zShort };  // z positions
  double angleShortTube[nPosShortTube] = { -M_PI/2, M_PI/2 };  // rotation angles (around y)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosShortTube; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = zPosShortTube[jj];
     rotAngleAux[i] = angleShortTube[jj];
  }

  GENFUNCTION fShortTube    = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosShortTube ); 
  GENFUNCTION fTrlShortTube = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosShortTube ); 
  GENFUNCTION fRotShortTube = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosShortTube ); 

  TRANSFUNCTION XFShortTube = Pow( HepRotateZ3D(1.0), fShortTube ) * 
                              Pow( HepTranslateZ3D(1.0), fTrlShortTube ) * HepTranslateX3D(rMean) *
                              Pow( HepRotateY3D(1.0), fRotShortTube );
  GeoSerialTransformer* sxShortTube = new GeoSerialTransformer( pBevelledShortTube, 
                                                                &XFShortTube, 
                                                                nPhiSectors * nPosShortTube );
  container->add(sxShortTube);


  //------------------------------------------------------------------------------------------
  // Build corner Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------

  GeoTube* sCornerTube = new GeoTube( rIn, rOut - tolerance, zMaxC/2 );

  //------------------------------------------------------------------------------------------
  // cut away left and right sides
  trlLeft  = trlBox * HepTranslateZ3D(-zMaxC/2); 
  trlRight = trlBox * HepTranslateZ3D( zMaxC/2);
  const GeoShape& sBevelledCornerTube = sCornerTube->subtract( (*sEmptyBox) << (trlLeft  * rotLeftBox) ).
                                                     subtract( (*sEmptyBox) << (trlRight * rotRightBox) );
  GeoLogVol*  lBevelledCornerTube = new GeoLogVol( "BTBevelledCornerTube", 
                                                   &sBevelledCornerTube,
                                                   theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pBevelledCornerTube = new GeoPhysVol(lBevelledCornerTube);

  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosCornerTube = 4;
  double zPosCornerTube[nPosCornerTube]  = { zCorner, zCorner, -zCorner, -zCorner };  // z positions
  double xPosCornerTube[nPosCornerTube]  = { rMean - xCorner, rMean + xCorner,  
                                             rMean + xCorner, rMean - xCorner };      // x positions
  double angleCornerTube[nPosCornerTube] = { M_PI/4, 3*M_PI/4, 5*M_PI/4, 7*M_PI/4 };  // rotation angles (around y)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosCornerTube; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = zPosCornerTube[jj];
     pos2Aux[i]     = xPosCornerTube[jj];
     rotAngleAux[i] = angleCornerTube[jj];
  }

  GENFUNCTION fCornerTube     = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosCornerTube ); 
  GENFUNCTION fTrl1CornerTube = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosCornerTube ); 
  GENFUNCTION fTrl2CornerTube = ArrayFunction( pos2Aux,     pos2Aux     + nPhiSectors * nPosCornerTube ); 
  GENFUNCTION fRotCornerTube  = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosCornerTube ); 

  TRANSFUNCTION XFCornerTube = Pow( HepRotateZ3D(1.0), fCornerTube ) * 
                               Pow( HepTranslateZ3D(1.0), fTrl1CornerTube ) *
                               Pow( HepTranslateX3D(1.0), fTrl2CornerTube ) *
                               Pow( HepRotateY3D(1.0), fRotCornerTube );
  GeoSerialTransformer* sxCornerTube = new GeoSerialTransformer( pBevelledCornerTube,  
                                                                 &XFCornerTube, 
                                                                 nPhiSectors * nPosCornerTube );   
  container->add(sxCornerTube);
  
  
  //------------------------------------------------------------------------------------------
  //  Build BT Vacuum Vessel ribs
  //  - account for correct concave profile at sides touching the Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------
   
  // rib property
  const double lengthBTRib = deltaR - 4 * rOut;
  
  //------------------------------------------------------------------------------------------
  // volumes for boolean subtraction:

  // (large enough) empty tube to cut out the concave profile from the sides
  double alpha   = asin( 0.5 * heightBTRib/rOut );
  double sagitta = rOut - 0.5 * heightBTRib/tan(alpha);

  GeoShape* sEmptyTube = new GeoTube( 0, rOut, 2*zWidthBTRib - tolerance );
  
  // (large enough) empty box to cut out the inner Rib volume
  GeoBox* sEmptyInnerBox = new GeoBox( lengthBTRib/2 + rOut/2 , 
                                       heightBTRib/2 - thicknessBTVessel, 
                                       zWidthBTRib/2 - thicknessBTVessel );

  //------------------------------------------------------------------------------------------
  // rib envelope
  GeoBox* sRibOuterBox = new GeoBox( lengthBTRib/2 + sagitta, heightBTRib/2, zWidthBTRib/2 ); 
  
  // cut away concave profile on left and right sides, and cut out inner volume
  const GeoShape& sRibEnvelope = sRibOuterBox->subtract( (*sEmptyTube) << HepTranslateX3D( -lengthBTRib/2 - rOut ) ).
                                               subtract( (*sEmptyTube) << HepTranslateX3D(  lengthBTRib/2 + rOut ) ).
                                               subtract( *sEmptyInnerBox );

  GeoLogVol*  lRibEnvelope = new GeoLogVol( "BTRibEnvelope", 
                                            &sRibEnvelope, 
                                            theMaterialManager->getMaterial(vesselMaterial) );
  GeoPhysVol* pRibEnvelope = new GeoPhysVol(lRibEnvelope);

  //------------------------------------------------------------------------------------------
  // replicate and position
  for ( int i = 0; i < nPhiSectors * nBTRibs; i++ )  // fill auxiliary arrays 
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]  = phiVal[ii]; 
     pos1Aux[i] = zPosBTRib[jj];
  }

  GENFUNCTION fRibEnvelope    = ArrayFunction( phiAux,  phiAux  + nPhiSectors * nBTRibs ); 
  GENFUNCTION fTrlRibEnvelope = ArrayFunction( pos1Aux, pos1Aux + nPhiSectors * nBTRibs ); 

  TRANSFUNCTION XFRibEnvelope = Pow( HepRotateZ3D(1.0), fRibEnvelope ) * 
                                Pow( HepTranslateZ3D(1.0), fTrlRibEnvelope ) 
				* HepTranslateX3D(rMean);
  GeoSerialTransformer* sxRibEnvelope = new GeoSerialTransformer( pRibEnvelope, 
                                                                  &XFRibEnvelope, 
                                                                  nPhiSectors * nBTRibs );
  container->add(sxRibEnvelope);

//Build BT Vacuum Vessel wings ribs
// DHW - June 2008: temporarily take 25 mm off of CryRiWTh
  double dhw = 25.;
  GeoShape* sTrapWingRib   = new GeoTrd( CryRiWYn/2, CryRiWYp/2, zWidthBTRib/2 + CryRiWTh - dhw,
					zWidthBTRib/2 + CryRiWTh - dhw, (CryRiWXp - CryRiWXn)/2 );
  GeoBox* sBoxWingRib      = new GeoBox( CryRiWYn/2, zWidthBTRib/2 + CryRiWTh - dhw,
					(rOut + CryRiWXn)/2 );
  GeoBox* sEmptyBoxWingRib = new GeoBox( 2 * rOut, zWidthBTRib/2 + tolerance, 2 * rOut );
  GeoTube* sEmptyTubeWingRib= new GeoTube( 0, rOut + tolerance, zWidthBTRib);
  
  const GeoShape& sWingRib = sTrapWingRib->add( (*sBoxWingRib) << HepTranslateZ3D(
					   -(rOut + CryRiWXp)/2) ).
					   subtract( (*sEmptyBoxWingRib) << HepTranslateZ3D(
					   -(CryRiWXp + CryRiWXn)/2) ).
					   subtract( (*sEmptyTubeWingRib) << HepTranslateZ3D(
					   -(CryRiWXp + CryRiWXn + 2*rOut)/2) * 
					   HepRotateX3D(M_PI/2) );
  
  GeoLogVol* lWingRib = new GeoLogVol( "BTWingRib", &sWingRib,
					theMaterialManager->getMaterial(voussoirMaterial) );
  GeoPhysVol* pWingRib = new GeoPhysVol(lWingRib);
					
  // replicate and position
  for (int i = 0; i < nPhiSectors * nBTRibs; i++ )
  {
    int ii = i % nPhiSectors,
        jj = i / nPhiSectors;
    phiAux[i] = phiVal[ii];
    pos1Aux[i] = zPosBTRib[jj];
  }
  
  GENFUNCTION fWingRib    = ArrayFunction( phiAux, phiAux + nPhiSectors * nBTRibs );
  GENFUNCTION fTrlWingRib = ArrayFunction( pos1Aux, pos1Aux + nPhiSectors * nBTRibs );
  
  TRANSFUNCTION XFWingRibIn = Pow( HepRotateZ3D(1.0), fWingRib ) * 
			      Pow( HepTranslateZ3D(1.0), fTrlWingRib ) *
			      HepTranslateX3D( rMinBTVessel + 2*rOut + (CryRiWXp + CryRiWXn)/2)
			      * HepRotateX3D(M_PI/2) * HepRotateY3D(M_PI/2);
  GeoSerialTransformer* sxWingRibIn = new GeoSerialTransformer( pWingRib,
							        &XFWingRibIn,
							        nPhiSectors * nBTRibs );
  TRANSFUNCTION XFWingRibOut = Pow( HepRotateZ3D(1.0), fWingRib )*
			       Pow( HepTranslateZ3D(1.), fTrlWingRib ) * 
			       HepTranslateX3D( rMaxBTVessel - 2*rOut - (CryRiWXp + CryRiWXn)/2)
			       * HepRotateX3D(M_PI/2) * HepRotateY3D(-M_PI/2);
  GeoSerialTransformer* sxWingRibOut = new GeoSerialTransformer( pWingRib,
								 &XFWingRibOut,
								 nPhiSectors * nBTRibs );
  container->add(sxWingRibIn);							      
  container->add(sxWingRibOut);
  //------------------------------------------------------------------------------------------
  //  Build BT Voussoir Attachments
  //  - trapezoidal shapes, embracing the long tubes; material: diluted Iron (approximation)
  //  - account for correct concave profile at sides touching the Vacuum Vessel tubes
  //------------------------------------------------------------------------------------------
  
  GeoTube* sVoussAttTube  = new GeoTube( HoleDiam/2 - thicknessTubeVAt, HoleDiam/2,
					(lengthTubeVAt - lengthConeVAt)/2 );
//never used   GeoCons* sVoussAttCone1 = new GeoCons( HoleDiam/2 - thicknessTubeVAt, HoleDiamIn/2 -thicknessTubeVAt,
//never used 					 HoleDiam/2, HoleDiamIn/2,
//never used 					 lengthConeVAt/2, 0., M_PI );
  GeoCons* sVoussAttCone2 = new GeoCons( HoleDiam/2 - thicknessTubeVAt, HoleDiamIn/2 - thicknessTubeVAt,
					 HoleDiam/2, HoleDiamIn/2,
					 lengthConeVAt/2, 0, 2*M_PI);

  //------------------------------------------------------------------------------------------
  // cut out concave profile on bottom side 
  // - note that empty tube axes don't coincide with trapezoid ones
  double xPosVoussAtt = PosEdgeTubeVAt - lengthTubeVAt/2 - lengthConeVAt/2;
  
  HepTransform3D xfEmptyTube = HepTranslateZ3D( -( xPosVoussAtt + xLong - rMean ) )
					        * HepRotateX3D( M_PI/2 );
  const GeoShape& sVoussAtt  = sVoussAttTube->subtract( (*sEmptyTube) << xfEmptyTube ).
//    					      add( (*sVoussAttCone1) << HepTranslateZ3D( lengthTubeVAt/2 ) ).
					      add( (*sVoussAttCone2) << HepTranslateZ3D( lengthTubeVAt/2 ) );
					    
  GeoLogVol*  lVoussAtt = new GeoLogVol( "BTVoussoirAttachment", 
                                         &sVoussAtt, 
                                         theMaterialManager->getMaterial(voussAttMaterial) );
  GeoPhysVol* pVoussAtt = new GeoPhysVol(lVoussAtt);

  //------------------------------------------------------------------------------------------
  // replicate and position
 
//  double xPosVoussAtt = PosEdgeTubeVAt - lengthTubeVAt/2 - lengthConeVAt/2;
  
  for ( int i = 0; i < nPhiSectors * nBTVouss; i++ )  // fill auxiliary arrays 
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]  = phiVal[ii]; 
     pos1Aux[i] = zPosBTVouss[jj];
  }

  GENFUNCTION fVoussAtt     = ArrayFunction( phiAux,  phiAux  + nPhiSectors * nBTVouss ); 
  GENFUNCTION fTrlVoussAtt1 = ArrayFunction( pos1Aux, pos1Aux + nPhiSectors * nBTVouss ); 

  TRANSFUNCTION XFVoussAtt = Pow( HepRotateZ3D(1.0), fVoussAtt ) * 
                             Pow( HepTranslateZ3D(1.0), fTrlVoussAtt1 ) *
			     HepTranslateX3D(xPosVoussAtt) *
                             HepRotateX3D( -M_PI/2 ) * HepRotateY3D(M_PI/2);
  GeoSerialTransformer* sxVoussAtt = new GeoSerialTransformer( pVoussAtt, &XFVoussAtt, nPhiSectors * nBTVouss );
  container->add(sxVoussAtt);

}
 
void BarrelToroidBuilderRDB::buildBTColdMass( GeoPhysVol* container ) 
{
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  Builds BT Cold Mass (coils, coil casings, and ribs)                                   //
  ////////////////////////////////////////////////////////////////////////////////////////////

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int maxDim = 7; 


//---------- BTCB -- BT Conductor and Coil Casing Parameters -----------------//
//                                                                            //
                                                                              //
   const double widthBTColdMass  = (*m_Abrt)[0]->getFloat("COMARTHI") * mm;                                 //btcb->xwidth
   const double heightBTColdMass = (*m_Abrt)[0]->getFloat("COMAYTHI") * mm;                                 //btcb->ywidth
                                                                              // 
   const std::string conductorMaterial = getMaterial("Aluminium"); 
//                                                                            //
//----------------------------------------------------------------------------//
  
//---------- BTVV -- BT Vacuum Vessel Parameters -----------------------------//
//                                                                            //
                                                                              //
   const double rMinBTVessel      = (*m_Abrt)[0]->getFloat("COMARMIN") * mm;  //CryoRmin/101
   const double rMaxBTVessel      = (*m_Abrt)[0]->getFloat("COMARMAX") * mm;  //CryiRmax/102
   const double zMaxBTVessel      = (*m_Abrt)[0]->getFloat("COMAZMAX") * mm;  //CryoZmax/103
   const double rCurvBTVessel     = (*m_Abrt)[0]->getFloat("COMARCUI") * mm;  //CryoRcurv/104
//   const double radiusBTVessel    = (*m_Abrt)[0]->getFloat("CRYORADI") * mm;  //  
//----------------------------------------------------------------------------//

//---------- BTRC -- BT Rib and Rib Cold Mass Parameters ---------------------//
//                                                                            //
                                                                              //
   int nBTRibs = 7;                                                           //btrc->nrib
   double zPosBTRib[maxDim];                                                  //
   for ( int i = 0; i < nBTRibs; i++ )  zPosBTRib [i] = (*m_Abrt)[0]->getFloat("ZRIB",i) * mm; //
                                                                              // 
   const double heightBTRibColdMass = (*m_Abrt)[0]->getFloat("COMARIBY") * mm;//ColdMassRibY/173
   const double zWidthBTRibColdMass = (*m_Abrt)[0]->getFloat("COMARIBZ") * mm;// ColdMassRibZ/174
                                                                              //
//----------------------------------------------------------------------------//
  

  const double phi0    = M_PI/8;
//never used   const double sinPhi0 = sin(phi0);
//never used   const double cosPhi0 = cos(phi0);
  const double tanPhi0 = tan(phi0);

  // array of phi values (centres of phi sectors)
  const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = ( 2*i + 1 ) * phi0;  

  // auxiliary arrays to store phi values, positions, rotation angles for all volumes to be replicated 
  const int nPosMax = maxDim;
  double phiAux[ nPhiSectors * nPosMax ], pos1Aux[ nPhiSectors * nPosMax ], pos2Aux[ nPhiSectors * nPosMax ], 
         rotAngleAux[ nPhiSectors * nPosMax ];


  // Cold Mass properties 
  const double deltaL       = widthBTColdMass * tanPhi0; 
  const double distToCorner = sqrt(2) * rCurvBTVessel * tanPhi0; 
  const double rMean        = 0.5 * ( rMinBTVessel + rMaxBTVessel );  
  const double deltaR       = rMaxBTVessel - rMinBTVessel;
  const double lMeanLong    = 2 * ( zMaxBTVessel - widthBTColdMass/2 - distToCorner ); 
  const double lMeanShort   = deltaR - 2 * ( widthBTColdMass/2 + distToCorner ); 
  const double lMeanCorner  = 2 * rCurvBTVessel * tanPhi0; 

  // positioning variables
  const double xLong   = deltaR/2 - widthBTColdMass/2;
  const double zShort  = zMaxBTVessel - widthBTColdMass/2;
  const double xCorner = xLong  - distToCorner/2;
  const double zCorner = zShort - distToCorner/2;


  //------------------------------------------------------------------------------------------
  // Build BT Coil+Coil Casing, using trapezoidal shapes everywhere as an approximation. 
  // Note that GeoTrd axes definition does not coincide with the coordinates used here.   
  //  --> Shapes need to be rotated first, then put into right orientation.
  //------------------------------------------------------------------------------------------

  //------------------------------------------------------------------------------------------
  // Build long upper and lower Conductor Box segments 
  //------------------------------------------------------------------------------------------

  GeoTrd* sLongSegment = new GeoTrd( lMeanLong/2 - deltaL/2, lMeanLong/2 + deltaL/2,
                                     heightBTColdMass/2, heightBTColdMass/2, widthBTColdMass/2 );
  
  GeoLogVol*  lLongSegment = new GeoLogVol( "BTColdLongSegment", 
                                            sLongSegment, 
                                            theMaterialManager->getMaterial(conductorMaterial) );
  GeoPhysVol* pLongSegment = new GeoPhysVol(lLongSegment);
      
  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosLongSegment = 2;
  double xPosLongSegment[nPosLongSegment]  = { rMean - xLong, rMean + xLong };  // x positions
  double angleLongSegment[nPosLongSegment] = { -M_PI/2, M_PI/2 };               // rotation angles (around y)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosLongSegment; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = xPosLongSegment[jj];
     rotAngleAux[i] = angleLongSegment[jj];
  }

  GENFUNCTION fLongSegment    = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosLongSegment ); 
  GENFUNCTION fTrlLongSegment = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosLongSegment ); 
  GENFUNCTION fRotLongSegment = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosLongSegment ); 

  TRANSFUNCTION XFLongSegment = Pow( HepRotateZ3D(1.0), fLongSegment ) * 
                                Pow( HepTranslateX3D(1.0), fTrlLongSegment ) * 
                                Pow( HepRotateY3D(1.0), fRotLongSegment );
  GeoSerialTransformer* sxLongSegment = new GeoSerialTransformer( pLongSegment, 
                                                                  &XFLongSegment, 
                                                                  nPhiSectors * nPosLongSegment );

  container->add(sxLongSegment);


  //------------------------------------------------------------------------------------------
  // Build short left and right Conductor Box segments
  //------------------------------------------------------------------------------------------

  GeoTrd* sShortSegment = new GeoTrd( lMeanShort/2 - deltaL/2, lMeanShort/2 + deltaL/2,
                                      heightBTColdMass/2, heightBTColdMass/2, widthBTColdMass/2 );
  
  GeoLogVol*  lShortSegment = new GeoLogVol( "BTColdShortSegment", 
                                             sShortSegment, 
                                             theMaterialManager->getMaterial(conductorMaterial) );
  GeoPhysVol* pShortSegment = new GeoPhysVol(lShortSegment);
      
  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosShortSegment = 2;
  double zPosShortSegment[nPosShortSegment]  = { -zShort, zShort };  // z positions
  double angleShortSegment[nPosShortSegment] = { M_PI, 0 };          // rotation angles (around y)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosShortSegment; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = zPosShortSegment[jj];
     rotAngleAux[i] = angleShortSegment[jj];
  }

  GENFUNCTION fShortSegment    = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosShortSegment ); 
  GENFUNCTION fTrlShortSegment = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosShortSegment ); 
  GENFUNCTION fRotShortSegment = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosShortSegment ); 

  TRANSFUNCTION XFShortSegment = Pow( HepRotateZ3D(1.0), fShortSegment ) * 
                                 Pow( HepTranslateZ3D(1.0), fTrlShortSegment ) * HepTranslateX3D(rMean) *
                                 Pow( HepRotateY3D(1.0), fRotShortSegment );
  GeoSerialTransformer* sxShortSegment = new GeoSerialTransformer( pShortSegment, 
                                                                   &XFShortSegment, 
                                                                   nPhiSectors * nPosShortSegment );
  container->add(sxShortSegment);


  //------------------------------------------------------------------------------------------
  // Build corner Conductor Box segments
  //------------------------------------------------------------------------------------------

  GeoTrd* sCornerSegment = new GeoTrd( lMeanCorner/2 - deltaL/2, lMeanCorner/2 + deltaL/2,
                                       heightBTColdMass/2, heightBTColdMass/2, widthBTColdMass/2 );

  GeoLogVol*  lCornerSegment = new GeoLogVol( "BTColdCornerSegment", 
                                              sCornerSegment, 
                                              theMaterialManager->getMaterial(conductorMaterial) );
  GeoPhysVol* pCornerSegment = new GeoPhysVol(lCornerSegment);
      
  //------------------------------------------------------------------------------------------
  // replicate and position
  const int nPosCornerSegment = 4;
  double zPosCornerSegment[nPosCornerSegment]  = { -zCorner, zCorner, zCorner, -zCorner }; // z positions
  double xPosCornerSegment[nPosCornerSegment]  = { rMean + xCorner, rMean + xCorner,  
                                                   rMean - xCorner, rMean - xCorner };     // x positions
  double angleCornerSegment[nPosCornerSegment] = { 3*M_PI/4, M_PI/4, -M_PI/4, -3*M_PI/4 }; // rotation angles (around y)
    
  // fill auxiliary arrays 
  for ( int i = 0; i < nPhiSectors * nPosCornerSegment; i++ )  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]      = phiVal[ii]; 
     pos1Aux[i]     = zPosCornerSegment[jj];
     pos2Aux[i]     = xPosCornerSegment[jj];
     rotAngleAux[i] = angleCornerSegment[jj];
  }

  GENFUNCTION fCornerSegment     = ArrayFunction( phiAux,      phiAux      + nPhiSectors * nPosCornerSegment ); 
  GENFUNCTION fTrl1CornerSegment = ArrayFunction( pos1Aux,     pos1Aux     + nPhiSectors * nPosCornerSegment ); 
  GENFUNCTION fTrl2CornerSegment = ArrayFunction( pos2Aux,     pos2Aux     + nPhiSectors * nPosCornerSegment ); 
  GENFUNCTION fRotCornerSegment  = ArrayFunction( rotAngleAux, rotAngleAux + nPhiSectors * nPosCornerSegment ); 

  TRANSFUNCTION XFCornerSegment = Pow( HepRotateZ3D(1.0), fCornerSegment ) * 
                                  Pow( HepTranslateZ3D(1.0), fTrl1CornerSegment ) *
                                  Pow( HepTranslateX3D(1.0), fTrl2CornerSegment ) *
                                  Pow( HepRotateY3D(1.0), fRotCornerSegment );
  GeoSerialTransformer* sxCornerSegment = new GeoSerialTransformer( pCornerSegment, 
                                                                    &XFCornerSegment, 
                                                                    nPhiSectors * nPosCornerSegment );
  container->add(sxCornerSegment);
  
  
  //------------------------------------------------------------------------------------------
  //  Build BT Rib Cold Mass
  // - Cold Mass length defined by Vacuum Vessel and Coil Casing dimensions 
  //------------------------------------------------------------------------------------------
   
  const double lengthBTRib = deltaR - 2 * widthBTColdMass;

  GeoBox*     sRibColdMass = new GeoBox( lengthBTRib/2, heightBTRibColdMass/2, zWidthBTRibColdMass ); 
  
  GeoLogVol*  lRibColdMass = new GeoLogVol( "BTColdRib", 
                                            sRibColdMass, 
                                            theMaterialManager->getMaterial(conductorMaterial) );
  GeoPhysVol* pRibColdMass = new GeoPhysVol(lRibColdMass);

  //------------------------------------------------------------------------------------------
  // replicate and position
  for ( int i = 0; i < nPhiSectors * nBTRibs; i++ )  // fill auxiliary arrays 
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i]  = phiVal[ii]; 
     pos1Aux[i] = zPosBTRib[jj];
  }

  GENFUNCTION fRibColdMass    = ArrayFunction( phiAux,  phiAux  + nPhiSectors * nBTRibs ); 
  GENFUNCTION fTrlRibColdMass = ArrayFunction( pos1Aux, pos1Aux + nPhiSectors * nBTRibs ); 

  TRANSFUNCTION XFRibColdMass = Pow( HepRotateZ3D(1.0), fRibColdMass ) * 
                                Pow( HepTranslateZ3D(1.0), fTrlRibColdMass ) * HepTranslateX3D(rMean);
  GeoSerialTransformer* sxRibColdMass = new GeoSerialTransformer( pRibColdMass, &XFRibColdMass, nPhiSectors * nBTRibs );
  container->add(sxRibColdMass);

}

  void BarrelToroidBuilderRDB::buildBTVoussoirs( GeoPhysVol* container ) 
{
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  Builds BT Voussoirs (warm support structure)                                          //
  ////////////////////////////////////////////////////////////////////////////////////////////

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int maxDim = 8; 


//---------- BTVV -- BT Vacuum Vessel Parameters -----------------------------//
//                                                                            //
                                                                              //
//never used    const double rMinBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMIN") * mm;  //CryoRmin/101
//never used    const double rMaxBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMAX") * mm;  //CryiRmax/102
//                                                                            //
//----------------------------------------------------------------------------//

//---------- BTVS -- BT Voussoir Parameters ----------------------------------//
//                                                                            //
                                                                              //
   int nBTVouss = 8;                                               //btvs->nvouss
   double zPosBTVouss[maxDim];                                                //
   for ( int i = 0; i < nBTVouss; i++ )  zPosBTVouss[i] = (*m_Abrt)[0]->getFloat("ZVOUSS", i) * mm; //
                                                                              //
   const double tRMinBTVouss                 = (*m_Abrt)[0]->getFloat("VOUSMBRA") * mm;           //btvs->trmin est' srednee snach etih dvuh per. ->
//never used    const double tRBTVouss                    = (*m_Abrt)[0]->getFloat("VOUSSRAD") * mm; 
   const double tWidthBTVouss		     = (*m_Abrt)[0]->getFloat("VOUSBLXH") * mm;//
   
   const std::string voussoirMaterial = getMaterial("Aluminium"); 
                                                                              //
//----------------------------------------------------------------------------//

   const double zWidth               = (*m_Abrt)[0]->getFloat("VOUBLZLE") * mm;
   const double zWidthStepVouss	      = (*m_Abrt)[0]->getFloat("VOUBLZLS") * mm;
   const double alphaStep		      = 15.0 * deg;
   const double xLength	      = (*m_Abrt)[0]->getFloat("VOUSBLYW") * mm;
   const double thicknessStepVouss     = (*m_Abrt)[0]->getFloat("VOURECSL") * mm;
//never used    const double xPosFoot		      = (*m_Feet)[0]->getFloat("STDFOOXP") * mm;
//never used    const double yPosFoot		      = (*m_Feet)[0]->getFloat("STDFOOYP") * mm;
   const double distInnerRib    = 25.0 + (*m_Abrt)[0]->getFloat("VOUCRCYR") * mm;
   const double distHoleCentre  = (*m_Abrt)[0]->getFloat("VOUSCYPO") * mm; //??
   const double diameterHoleVoussFeet         = 2.0 * (*m_Abrt)[0]->getFloat("VOURCUTR") * mm;
   const double thicknessTop    = (*m_Abrt)[0]->getFloat("VOUSPLOX") * mm; //???
   const double thicknessCent   = (*m_Abrt)[0]->getFloat("VOUBZWTH") * mm;
   const double thicknessBot    = (*m_Abrt)[0]->getFloat("VOUSPLIX") * mm; //???
   const double thicknessRib         = (*m_Abrt)[0]->getFloat("VOURPYWI") * mm;
   const double thicknessOutRibStd = (*m_Abrt)[0]->getFloat("VOUBLYWS") * mm;
//never used    const double ThetaAngleVoussFeet	      = 15.0 * deg;
//!!

//---------- BTVH -- BT Voussoir Head Parameters -----------------------------//
//                                                                            //
                                                                              //
   const double tHeightVHead         = (*m_Abrt)[0]->getFloat("CNBCOYEX") * mm;                    
   const double tRMaxBTVoussHead     = (*m_Abrt)[0]->getFloat("CNBXMBRA") * mm;                    
   const double xWidthMinVHead	     = (*m_Abrt)[0]->getFloat("CNBCOXIN") * mm;                       
   const double xWidthMaxVHead 	     = (*m_Abrt)[0]->getFloat("CNBCOXSU") * mm;                       
   const double CnbEaXtp	     = (*m_Abrt)[0]->getFloat("CNBEAXTP") * mm;
//never used    const double CnbEaYtp	     = (*m_Abrt)[0]->getFloat("CNBEAYTP") * mm;
   const double CnbEaXbt	     = (*m_Abrt)[0]->getFloat("CNBEAXBT") * mm;
//never used    const double CnbEaYbt	     = (*m_Abrt)[0]->getFloat("CNBEAYBT") * mm;
   const double CnbEaCxi	     = (*m_Abrt)[0]->getFloat("CNBEACXI") * mm;
//never used    const double CnbECYil	     = (*m_Abrt)[0]->getFloat("CNBECYIL") * mm;
   const double CnbECXol	     = (*m_Abrt)[0]->getFloat("CNBECXOL") * mm;
   const double CnbECXou	     = (*m_Abrt)[0]->getFloat("CNBECXOU") * mm;
//never used    const double CnbECYol	     = (*m_Abrt)[0]->getFloat("CNBECYOL") * mm;
//never used    const double CnbECYou	     = (*m_Abrt)[0]->getFloat("CNBECYOU") * mm;
//never used    const double CnbECYiu	     = (*m_Abrt)[0]->getFloat("CNBECYIU") * mm;
   const double CnbEaCPl	     = (*m_Abrt)[0]->getFloat("CNBEACPL") * mm;
   const double CnbECZpo	     = (*m_Abrt)[0]->getFloat("CNBECZPO") * mm;
   const double CnbIECZp	     = (*m_Abrt)[0]->getFloat("CNBIECZP") * mm;
   const double CnbCaDma	     = (*m_Abrt)[0]->getFloat("CNBCADMA") * mm;
   const double CnbCaDmi	     = (*m_Abrt)[0]->getFloat("CNBCADMI") * mm;
   const double CnbCaZin	     = (*m_Abrt)[0]->getFloat("CNBCAZIN") * mm;
   const double CnbCaZex	     = (*m_Abrt)[0]->getFloat("CNBCAZEX") * mm;
   const double CnboxZex	     = (*m_Abrt)[0]->getFloat("CNBOXZEX") * mm;

//----------------------------------------------------------------------------
  
    
  const double tolerance = 0.5 * mm;

  const double phi0 = M_PI/8;

  // array of phi values (centres of phi sectors)
    const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = (2*i + 1) * phi0;  

  // auxiliary arrays to store phi values and positions for all volumes to be replicated 
  const int nPosMax = maxDim;
  double phiAux[ nPhiSectors * nPosMax ], posAux[ nPhiSectors * nPosMax ];

  const double XminEdge = (CnbEaXbt - xWidthMinVHead/2)/cos(phi0);
  const double xLengthWallEdge= (CnbEaXbt - CnbECXol)/cos(phi0);
  const double DeltaX = ( (tRMaxBTVoussHead - tHeightVHead) * sin(phi0) -
                       (xWidthMinVHead/2)*cos(phi0) - XminEdge - xLength/2 );
//edge box
//  GeoBox* sEdgeBox = new Box( xLengthWallEdge/2, yHeight/2, CnboxZex/2 );
  
//  double distEdgeBox = xLength/2 + DeltaX + xLengthWallEdge/2)
  
//    HepTransform3D trlEdgeBox[2];   
//    trlEdgeBox[0] = HepTranslateX3D( -distEdgeBox );
//    trlEdgeBox[1] = HepTranslateX3D( distEdgeBox );
                    
 //------------------------------------------------------------------------------------------
  // common Feet/Voussoir properties 
  const double yHeight = tWidthBTVouss;            // total y-height  
  const double yHeightCent   = yHeight - thicknessTop - thicknessBot;  // height of central plate
  const double radiusHole    = 0.5 * diameterHoleVoussFeet;  // radius of cutout hole
    
  // positioning variables
  // Feet Voussoir properties
//  const double xLengthStd         = 2 * ( xPosVertex5StandFeet - thicknessInnerPlateStandFeet - xWidthConnVoussFeet);  // total length
  const double xLengthCentStd     = xLength - 2 * thicknessOutRibStd;  // length of central plate

  // positioning variables
  const double distOutRibStd    = 0.5 * ( xLength - 2 * thicknessOutRibStd - thicknessStepVouss);
  const double distOutRib  = 0.5 * ( xLength - thicknessOutRibStd - thicknessStepVouss );
  
  //------------------------------------------------------------------------------------------
  // central plate
  const GeoShape* sCentPlateStd = new GeoBox( xLengthCentStd/2 - thicknessStepVouss, yHeightCent/2, thicknessCent/2 );

  // top plate
  GeoBox* sTopPlateStd = new GeoBox( xLength/2 - tolerance, thicknessTop/2 - tolerance, zWidth/2 );
  
  // bottom plate
  GeoBox* sBotPlateStd = new GeoBox(xLength/2 - tolerance, thicknessBot/2 - tolerance, zWidth/2 );

//edge box
  GeoBox* sEdgeBox = new GeoBox( xLengthWallEdge/2 - tolerance, yHeight/2, CnboxZex/2 );
  
  double distEdgeBox = xLength/2 + DeltaX + xLengthWallEdge/2;
      
  HepTransform3D trlEdgeBox[2];
  trlEdgeBox[0] = HepTranslate3D( -distEdgeBox, (thicknessTop - thicknessBot)/2, 0 );
  trlEdgeBox[1] = HepTranslate3D( distEdgeBox, (thicknessTop - thicknessBot)/2, 0 );
  
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
//never used   double yOutVoussmin	= yOutVoussmax * (zWidthStepVouss - thicknessStepVouss * tan(alphaStep) )/zWidthStepVouss;
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
  //add EdgeWalls
  for ( int i = 0; i < 2; i++ )
    {
     sCentPlateStd = &( sCentPlateStd->add( (*sEdgeBox) << trlEdgeBox[i] ) );
    }
  //subtract shapes from outer rib
 
  for ( int i = 0; i < 4; i++ )
  {
     sCentPlateStd = &( sCentPlateStd->subtract( (*sEmptyOuterRibTrap) << trlEmptyOutRibTrap[i] ) );
  } 
  
  
  //-----------------------------------------------------------------------------------------------------------------------
  // Build Trapezoidal elements of inner rib
  //-----------------------------------------------------------------------------------------------------------------------
  
    //trapezoids at top and bottom edge of inner rib
//     double xInnermin	= thicknessRib/2;
//     double xInnermax	= thicknessRib/2 + thicknessStepVouss;
//     double yInnerVouss	= zWidth/2 - zWidthStepVouss;
//     double zInnerVouss	= thicknessStepVouss/2;
    
    //ss 12/02/2007 never user - memory never released const GeoShape* sTopInnerRibTrap = new GeoTrd( xInnermin, xInnermax, yInnerVouss, yInnerVouss, zInnerVouss );
    
    //ss 12/02/2007 never user - memory never released GeoTrd* sBotInnerRibTrap = new GeoTrd( xInnermax, xInnermin, yInnerVouss, yInnerVouss, zInnerVouss );

    HepTransform3D trlBot2TopInnerRibTrap = HepTranslate3D( 0, 0, -yHeightCent + thicknessStepVouss );
    
//never used     const GeoShape& sTrapElementInnerRibVoussoir = sTopInnerRibTrap -> add( (*sBotInnerRibTrap) << trlBot2TopInnerRibTrap );


//      GeoLogVol*  lTrapElementInnerRibVoussoir = new GeoLogVol( "TrapElementInnerRibVoussoir", 
//                                              &sTrapElementInnerRibVoussoir, 
//                                              theMaterialManager->getMaterial(voussMaterial) );
//      GeoPhysVol* pTrapElementInnerRibVoussoir = new GeoPhysVol(lTrapElementInnerRibVoussoir);


  // centre of Voussoir central plate
  // (= reference point due to the assembly process chosen below)
  const double delta   = 0.5 * fabs( thicknessBot - thicknessTop );
  const double rCentre = 0.5 * ( yHeight + 2 * tRMinBTVouss ) - delta;
  const double centreXPos = rCentre * cos(phi0);
  const double centreYPos = -rCentre * sin(phi0);

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!! correction introduced for DC2, to void overlaps with BIR chambers (P03 layout) !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//const double dc2Corr = zWidthSlimRib/2 - zWidth/2;

//  GeoBox* sFatRib  = new GeoBox( thicknessFatRib/2, zWidth/2 + dc2Corr, tWidthCent/2 - tolerance );

  // positioning of Voussoir Head wrt. central plate   
//HepTransform3D trlVoussHead = HepTranslate3D( (tRMaxBTVoussHead - tHeightVHead/2) * sin(phi0),
//		      lengthHeadtoVous * cos(phi0),
//                                              0 ); 

const GeoShape& sBTVoussoir = sCentPlateStd->add( (*sTopPlateStd) << trlTopPlate ).
					       add( (*sBotPlateStd) << trlBotPlate ).
					       subtract( (*sEmptyTrap1) << trlEmptyTrap1 ).
					       subtract( (*sEmptyTrap2) << trlEmptyTrap2 );

  GeoLogVol*  lBTVoussoir = new GeoLogVol( "BTVoussoir", 
                                             &sBTVoussoir, 
                                             theMaterialManager->getMaterial(voussoirMaterial) );
  GeoPhysVol* pBTVoussoir = new GeoPhysVol(lBTVoussoir);

//edge of BTVoussoir

//  const double XminEdge = (CnbEaXbt - xWidthMinVHead/2)/cos(phi0);
  const double XmaxEdge = (CnbEaXtp - xWidthMaxVHead/2)/cos(phi0);
  const double XminEmptyEdge1 = (CnbECXol - CnbEaCxi)/cos(phi0);
  const double XmaxEmptyEdge1 = (CnbECXou - CnbEaCxi)/cos(phi0);
  const double XminEmptyEdge2 = (CnbECXol - CnbEaCxi - CnbEaCPl)/cos(phi0);
  const double XmaxEmptyEdge2 = (CnbECXou - CnbEaCxi - CnbEaCPl)/cos(phi0);
		
//  const double DeltaX = ( (tRMaxBTVoussHead - tHeightVHead) * sin(phi0) - 
//			(xWidthMinVHead/2)*cos(phi0) - XminEdge - xLength/2 );
  
  
  const GeoShape* sEdgeVoussoir = new GeoTrd( XminEdge + DeltaX + xLength/2,
					      XmaxEdge + DeltaX + xLength/2, 
					      CnboxZex/2, CnboxZex/2,
					      yHeight/2);
  
  GeoTrd* sEmptyTrap1EdgeBTVoussoir = new GeoTrd( XminEmptyEdge1 + DeltaX + xLength/2 + xLengthWallEdge,
						  XmaxEmptyEdge1 + DeltaX + xLength/2 + xLengthWallEdge,
						  CnboxZex/2, CnboxZex/2, yHeightCent/2 ); 
  GeoTrd* sEmptyTrap2EdgeBTVoussoir = new GeoTrd( XminEmptyEdge2 + DeltaX + xLength/2 + xLengthWallEdge,
						  XmaxEmptyEdge2 + DeltaX + xLength/2 + xLengthWallEdge,
						  CnboxZex/2, CnboxZex/2, yHeightCent/2 + tolerance );
    
  GeoBox* sEmptyBoxEdgeBTVoussoir = new GeoBox( DeltaX + xLength/2 + xLengthWallEdge, CnboxZex, yHeight);
  
  const GeoShape& sEdgeBTVoussoir = sEdgeVoussoir->subtract( (*sEmptyBoxEdgeBTVoussoir)
				    << HepTranslate3D(0.,0.,0.) ).
				    subtract( (*sEmptyTrap1EdgeBTVoussoir)
				    << HepTranslate3D(0., CnbECZpo, -delta) ).
				    subtract( (*sEmptyTrap1EdgeBTVoussoir)
				    << HepTranslate3D(0., -CnbECZpo, -delta) ).
				    subtract( (*sEmptyTrap2EdgeBTVoussoir)
				    << HepTranslate3D(0., CnbIECZp, -delta) ).
				    subtract( (*sEmptyTrap2EdgeBTVoussoir)
				    << HepTranslate3D(0., -CnbIECZp, -delta) );
  
  GeoLogVol* lEdgeBTVoussoir = new GeoLogVol( "EdgeBTVoussoir", &sEdgeBTVoussoir,
					     theMaterialManager->getMaterial(voussoirMaterial) );
  GeoPhysVol* pEdgeBTVoussoir = new GeoPhysVol(lEdgeBTVoussoir);


  // Voussoir Head properties 
//never used   const double lengthHeadtoVous = (rCentre/cos(phi0)) + tHeightVHead/2 - tRMaxBTVoussHead;

  //------------------------------------------------------------------------------------------
  // build Voussoir Head as trapezoidal shape 
  const GeoShape* sVoussHead = new GeoTrd( xWidthMinVHead/2 - tolerance, xWidthMaxVHead/2 - tolerance,
                                   CnboxZex/2, CnboxZex/2, tHeightVHead/2 );  
  GeoCons* sEmptyHeadCons   = new GeoCons( 0., 0., CnbCaDma/2, CnbCaDmi/2, CnbCaZin/2 + tolerance,
					 0., 2*M_PI );
  GeoTube* sEmptyHeadTube    = new GeoTube( 0., CnbCaDmi/2, CnbCaZex);

  const GeoShape& sHeadBTVoussoir = sVoussHead->subtract( (*sEmptyHeadCons)
				 << HepTranslateZ3D(CnbCaZin/2 - tHeightVHead/2) ).
				 subtract( (*sEmptyHeadTube) 
				 << HepTranslateZ3D(-tHeightVHead/2) ).
				 subtract( (*sEmptyTrap1EdgeBTVoussoir)
				 << HepTranslate3D(centreXPos + delta*cos(phi0), CnbECZpo,
				 -tRMaxBTVoussHead + tHeightVHead/2 + centreYPos - delta*sin(phi0))
				 * HepRotateY3D(M_PI/2 + phi0) ).
				 subtract( (*sEmptyTrap1EdgeBTVoussoir)
				 << HepTranslate3D(centreXPos + delta*cos(phi0), -CnbECZpo,
				 -tRMaxBTVoussHead + tHeightVHead/2 + centreYPos - delta*sin(phi0))
				 * HepRotateY3D(M_PI/2 + phi0) ).
				 subtract( (*sEmptyTrap1EdgeBTVoussoir)
				 << HepTranslate3D(-centreXPos - delta*cos(phi0), CnbECZpo,
				 -tRMaxBTVoussHead + tHeightVHead/2 + centreYPos - delta*sin(phi0))
				 * HepRotateY3D(-M_PI/2 - phi0) ).
				 subtract( (*sEmptyTrap1EdgeBTVoussoir)
				 << HepTranslate3D(-centreXPos + delta*cos(phi0), -CnbECZpo,
				 -tRMaxBTVoussHead + tHeightVHead/2 + centreYPos - delta*sin(phi0))
				 * HepRotateY3D(-M_PI/2 - phi0) );

  GeoLogVol* lHeadBTVoussoir = new GeoLogVol( "HeadBTVoussoir", &sHeadBTVoussoir,
					      theMaterialManager->getMaterial(voussoirMaterial) );

  GeoPhysVol* pHeadBTVoussoir = new GeoPhysVol(lHeadBTVoussoir);

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!! correction introduced for DC2, to avoid overlaps with BIR chambers (P03 layout) !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  GeoTrap* sBotSidePlate = new GeoTrap( tThicknessBot/2, thetaBotSide, 0,
//                                        zWidth/2 + dc2Corr, dX1BotSide, dX1BotSide, 0,
//                                        zWidth/2 + dc2Corr, dX3BotSide, dX3BotSide, 0 );


  
  // fill auxiliary arrays
  for ( int i = 0; i < nPhiSectors * nBTVouss; i++ )   
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = phiVal[ii]; 
     posAux[i] = zPosBTVouss[jj];
  }

  GENFUNCTION fVouss    = ArrayFunction( phiAux, phiAux + nPhiSectors * nBTVouss ); 
  GENFUNCTION fTrlVouss = ArrayFunction( posAux, posAux + nPhiSectors * nBTVouss ); 
  
  // central plate and Voussoir Head
  TRANSFUNCTION XFBTVoussoir = Pow( HepRotateZ3D(1.0), fVouss ) * 
                                 Pow( HepTranslateZ3D(1.0), fTrlVouss ) *
                                 HepTranslate3D( centreXPos, centreYPos, 0 ) *
                                 HepRotateZ3D(M_PI/2 - phi0);
				 
  GeoSerialTransformer* sxBTVoussoir = new GeoSerialTransformer( pBTVoussoir,
                                                                   &XFBTVoussoir, 
                                                                   nPhiSectors * nBTVouss );
  container->add(sxBTVoussoir);
  
  TRANSFUNCTION XFEdgeBTVoussoir = Pow( HepRotateZ3D(1.0), fVouss ) *
				    Pow( HepTranslateZ3D(1.0), fTrlVouss ) *
				    HepTranslate3D( centreXPos + delta*cos(phi0),
				    centreYPos - delta*sin(phi0), 0) * 
				    HepRotateZ3D(M_PI/2 - phi0) *
				    HepRotateX3D(M_PI/2);
  
  GeoSerialTransformer* sxEdgeBTVoussoir = new GeoSerialTransformer( pEdgeBTVoussoir,
								     &XFEdgeBTVoussoir,
								     nPhiSectors * nBTVouss );
  container->add(sxEdgeBTVoussoir);

  TRANSFUNCTION XFHeadBTVoussoir = Pow( HepRotateZ3D(1.0), fVouss ) *
				    Pow( HepTranslateZ3D(1.0), fTrlVouss ) *
				    HepTranslateX3D(tRMaxBTVoussHead - tHeightVHead/2) *
				    HepRotateX3D(M_PI/2) * HepRotateY3D(M_PI/2);

  GeoSerialTransformer* sxHeadBTVoussoir = new GeoSerialTransformer( pHeadBTVoussoir,
								     &XFHeadBTVoussoir,
								     nPhiSectors * nBTVouss );
				    
  container->add(sxHeadBTVoussoir);

}

  void BarrelToroidBuilderRDB::buildBTStruts( GeoPhysVol* container ) 
{
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  Builds BT Struts (warm support structure)                                             //
  ////////////////////////////////////////////////////////////////////////////////////////////

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

  const int maxDim = 8; 


//---------- BTVV -- BT Vacuum Vessel Parameters -----------------------------//
//                                                                            //
                                                                              //
   const double rMinBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMIN") * mm;  //CryoRmin/101
   const double rMaxBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMAX") * mm;  //CryiRmax/102
   const double radiusBTVessel    = (*m_Abrt)[0]->getFloat("CRYORADI") * mm;  // 
//                                                                            //
//----------------------------------------------------------------------------//

//---------- BTVS -- BT Voussoir Parameters ----------------------------------//
//                                                                            //
                                                                              //
   int nBTVouss = 8;                                               //btvs->nvouss
   double zPosBTVouss[maxDim];                                                //
   for ( int i = 0; i < nBTVouss; i++ )  zPosBTVouss[i] = (*m_Abrt)[0]->getFloat("ZVOUSS",i) * mm; //
                                                                              //
//----------------------------------------------------------------------------//
  
//---------- BTST -- BT Strut Parameters -------------------------------------//
//                                                                            //
                                                                              //
   const double tRMinBTStrut           = (*m_Abrt)[0]->getFloat("STRTRMIN") * mm;
   const double tRMaxBTStrut           = (*m_Abrt)[0]->getFloat("STRTRMAX") * mm;
   const double tThicknessBTStrut      = (*m_Abrt)[0]->getFloat("STRTRTHI") * mm;
   const double outerZWidthBTStrut     = (*m_Abrt)[0]->getFloat("STRTZWID") * mm;
   const double innerZThicknessBTStrut = (*m_Abrt)[0]->getFloat("STRTZTHI") * mm;
   const double lengthBTStrut          = (*m_Abrt)[0]->getFloat("STRTYLEN") * mm;
//never used    const double dist2centre	       = (*m_Abrt)[0]->getFloat("STRTRMOY") * mm;
   const double StrWRmax	       = (*m_Abrt)[0]->getFloat("STRWRMAX") * mm;
   const double StrWYmax	       = (*m_Abrt)[0]->getFloat("STRWYMAX") * mm;
   const double StrWRmin	       = (*m_Abrt)[0]->getFloat("STRWRMIN") * mm;
   const double StrWYmin	       = (*m_Abrt)[0]->getFloat("STRWYMIN") * mm;
   const double StrWZthi	       = (*m_Abrt)[0]->getFloat("STRWZTHI") * mm;
   const double StrWYthi	       = (*m_Abrt)[0]->getFloat("STRWYTHI") * mm;
   const double StrWZlen	       = (*m_Abrt)[0]->getFloat("STRWZLEN") * mm;
   const double StrtYlnP               = (*m_Abrt)[0]->getFloat("STRTYLNP") * mm;
                                                                              //
   const std::string strutMaterial = getMaterial("Aluminium"); 
                                                                              //
//----------------------------------------------------------------------------//
  
  const double tolerance = 0.5*mm;

  const double phi0 = M_PI/8;

  // array of phi values (centres of phi sectors)
  const int nPhiSectors = 8;
  double phiVal[nPhiSectors];
  for ( int i = 0; i < nPhiSectors; i++ )  phiVal[i] = ( 2*i + 1 ) * phi0;  

  // auxiliary arrays to store phi values and positions for all volumes to be replicated 
  const int nPosMax = maxDim;
  double phiAux[ nPhiSectors * nPosMax ], posAux[ nPhiSectors * nPosMax ];//, posAux1[ nPhiSectors * nPosMax ];
  

  // basic Strut properties 
  const double rOut             = radiusBTVessel; 
  const double halfLength       = lengthBTStrut/2;
  const double halfLengthbtubes = ( rMaxBTVessel - rOut ) * sin(phi0) - rOut;
  const double tWidth           = tRMaxBTStrut - tRMinBTStrut;
  const double tThickness       = tThicknessBTStrut;
  const double outerZWidth      = outerZWidthBTStrut;
  const double innerZThickness  = innerZThicknessBTStrut;
  
  // positioning variables for strut centre
  const double deltaR     = rMaxBTVessel - rMinBTVessel;
  const double rMean      = 0.5 * ( rMaxBTVessel + rMinBTVessel );
  const double centreXPos = deltaR/2 - rOut - ( halfLengthbtubes + rOut ) * sin(phi0); 
  const double centreYPos = -( halfLengthbtubes + rOut ) * cos(phi0);


  //------------------------------------------------------------------------------------------
  // Build Strut as bar with "I"-shape profile 
  // - let Struts touch the Vacuum Vessel as an approximation
  // - account for correct concave profile at sides touching the Vacuum Vessel 
  //------------------------------------------------------------------------------------------

//  double alpha   = asin( tWidth/2/rOut );
//never used   double sagitta = rOut - tWidth/2/tan(alpha);

  // inner vertical plate
  GeoBox* sVertBox = new GeoBox( halfLength - StrWYthi, tWidth/2 - tThickness - tolerance, innerZThickness/2 ); 
  
  // outer horizontal plates 
  GeoBox* sHorizBox = new GeoBox( halfLength - StrWYthi, tThickness/2, outerZWidth/2 ); 

  // empty tube to cut out the concave profiles at the sides
  //  GeoTube* sEmptyTube = new GeoTube( 0, rOut, outerZWidth/2 + tolerance );
  GeoBox* sEdgeBox = new GeoBox( StrWYthi/2, tWidth/2 + StrWZthi, StrWZlen/2 + StrWZthi );
  

  //------------------------------------------------------------------------------------------
  // unite horizontal and vertical plates, cut out left and right profiles
  const GeoShape& sStrut = sVertBox->add( (*sHorizBox) << HepTranslateY3D(  tWidth/2 - tThickness/2 ) ).
                                     add( (*sHorizBox) << HepTranslateY3D( -tWidth/2 + tThickness/2 ) ).
                                     add( (*sEdgeBox) << HepTranslateX3D( -halfLength + StrWYthi/2 ) ).
                                     add( (*sEdgeBox) << HepTranslateX3D( halfLength - StrWYthi/2 ) );
//                                     subtract( (*sEmptyTube) << HepTranslateX3D( -halfLength - rOut ) ).
//                                     subtract( (*sEmptyTube) << HepTranslateX3D(  halfLength + rOut ) );

  GeoLogVol*  lStrut = new GeoLogVol( "BTStrut", &sStrut, 
                                      theMaterialManager->getMaterial(strutMaterial) );
  GeoPhysVol* pStrut = new GeoPhysVol(lStrut);

  //----------------StrutWing-------------

  const double HeightTrap1 = StrWRmax - tRMaxBTStrut * cos(phi0) - sin(phi0) * StrtYlnP/2;
  const double HeightTrap2 = tRMaxBTStrut * cos(phi0) - tRMinBTStrut * cos(phi0);
  const double HeightTrap3 = tRMinBTStrut * cos(phi0) + sin(phi0) * StrtYlnP/2 - StrWRmin;

  GeoShape* sStrutWingTrap1  = new GeoTrd( tRMaxBTStrut * sin(phi0) - cos(phi0) * StrtYlnP/2,
                                           StrWYmax, StrWZthi/2 + StrWZlen/2, StrWZthi/2 + StrWZlen/2,
					   HeightTrap1/2 );
  GeoTrd* sStrutWingTrap2    = new GeoTrd( tRMinBTStrut * sin(phi0) - cos(phi0) * StrtYlnP/2,
					   tRMaxBTStrut * sin(phi0) - cos(phi0) * StrtYlnP/2,
					   StrWZthi/2 + StrWZlen/2, StrWZthi/2 + StrWZlen/2,
					   HeightTrap2/2 );
  GeoTrd* sStrutWingTrap3    = new GeoTrd( StrWYmin, tRMinBTStrut * sin(phi0) - 
					   cos(phi0) * StrtYlnP/2,
					   StrWZthi/2 + StrWZlen/2, StrWZthi/2 + StrWZlen/2, 
					   HeightTrap3/2 );
  GeoTube* sEmptyStrutWingTube = new GeoTube( 0, rOut + tolerance, StrWZthi + StrWZlen+ tolerance );
  GeoBox* sEmptyStrutWingBox   = new GeoBox( tRMaxBTStrut * sin(phi0) - cos(phi0) * StrtYlnP/2 + 
					     tolerance, StrWZlen/2 - StrWZthi/2, HeightTrap1 + 
					     HeightTrap2 + HeightTrap3 + tolerance );

  const GeoShape& sWingStrut = sStrutWingTrap1->add( (*sStrutWingTrap2) << 
						HepTranslateZ3D(-HeightTrap1/2 - HeightTrap2/2) ).
                                                add( (*sStrutWingTrap3) << 
						HepTranslateZ3D(-HeightTrap1/2 - HeightTrap2
						- HeightTrap3/2) ).
                                                subtract( (*sEmptyStrutWingTube) <<
                                                HepTranslateZ3D( -HeightTrap1/2 )
                                                * HepRotateX3D(M_PI/2) ).
                                                subtract( (*sEmptyStrutWingBox) << 
						HepTranslateZ3D( -(HeightTrap2 + HeightTrap3)/2 ) );

  GeoLogVol*  lWingStrut = new GeoLogVol( "BTWingStrut", &sWingStrut, 
                                      theMaterialManager->getMaterial(strutMaterial) );
  GeoPhysVol* pWingStrut = new GeoPhysVol(lWingStrut);

  //------------------------------------------------------------------------------------------
  // replicate and position
  for ( int i = 0; i < nPhiSectors * nBTVouss; i++ )  // fill auxiliary arrays  
  {
     int ii = i % nPhiSectors, 
         jj = i / nPhiSectors;
     phiAux[i] = phiVal[ii]; 
     posAux[i] = zPosBTVouss[jj];
   }

  GENFUNCTION fStrut    = ArrayFunction( phiAux, phiAux + nPhiSectors * nBTVouss ); 
  GENFUNCTION fTrlStrut = ArrayFunction( posAux, posAux + nPhiSectors * nBTVouss ); 
  GENFUNCTION fTrlWingStrut = ArrayFunction( posAux, posAux + nPhiSectors * nBTVouss );
  
  TRANSFUNCTION XFStrut = Pow( HepRotateZ3D(1.0), fStrut ) * 
                          Pow( HepTranslateZ3D(1.0), fTrlStrut ) *
                          HepTranslate3D( rMean + centreXPos, centreYPos, 0 ) *
                          HepRotateZ3D( -M_PI/2 - phi0 ); 
  TRANSFUNCTION XFWingStrut = Pow( HepRotateZ3D(1.0), fStrut ) *
			      Pow( HepTranslateZ3D(1.0), fTrlWingStrut ) *
			      HepTranslateX3D( rMean + deltaR/2 - radiusBTVessel + 
					       HeightTrap1/2 ) *
			      HepRotateX3D(M_PI/2)* HepRotateY3D(M_PI/2);


  GeoSerialTransformer* sxStrut = new GeoSerialTransformer(pStrut, &XFStrut, nPhiSectors * nBTVouss);  GeoSerialTransformer* sxWingStrut = new GeoSerialTransformer(pWingStrut, &XFWingStrut,
							       nPhiSectors * nBTVouss);

  container->add(sxStrut);
  container->add(sxWingStrut);
  

}

  void BarrelToroidBuilderRDB::buildBTCryoring( GeoPhysVol* container ) 
{
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  Builds BT Cryogenic Ring                                                              //
  ////////////////////////////////////////////////////////////////////////////////////////////

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//---------- BTVV -- BT Vacuum Vessel Parameters -----------------------------//
//                                                                            //
                                                                              //
   const double rMinBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMIN") * mm;  //CryoRmin/101
   const double rMaxBTVessel      = (*m_Abrt)[0]->getFloat("CRYORMAX") * mm;  //CryiRmax/102
   const double radiusBTVessel    = (*m_Abrt)[0]->getFloat("CRYORADI") * mm;  // 
   const double thicknessBTVessel = (*m_Abrt)[0]->getFloat("CRYORADT") * mm;
//                                                                            //
//----------------------------------------------------------------------------//

//---------- BTCR -- BT Cryogenic Ring Parameters ----------------------------//
//                                                                            //
                                                                              //
   const double radiusBTCryoring = (*m_Abrt)[0]->getFloat("CRYRNGRA") * mm;   //CryoRingRadius/131 
 //never used   const double zPosBTCryoring   = (*m_Abrt)[0]->getFloat("CRYRNGZM") * mm;  //(XML bug)
   const double CryRngZm	 = -1030.0 * mm;
                                                                   
//est' esche i aluminium
   const std::string cryoringMaterial = getMaterial("Iron"); 
                                                                              //
//----------------------------------------------------------------------------//
  
  
  // basic Cryoring(-tube) properties 
  const double phi0       = M_PI/8;
  const double rOut       = radiusBTVessel;
  const double rIn  	  = radiusBTCryoring - thicknessBTVessel; 
  const double halfLength = ( rMaxBTVessel - rOut ) * sin(phi0) - rOut;
  const double radius     = radiusBTCryoring;

  // positioning variables
  const double deltaR     = rMaxBTVessel - rMinBTVessel;
  const double rMean      = 0.5 * ( rMaxBTVessel + rMinBTVessel );
  const double centreXPos = deltaR/2 - rOut - ( halfLength + rOut ) * sin(phi0); 
  const double centreYPos = -( halfLength + rOut ) * cos(phi0);
  const double centreZPos = CryRngZm; 


  //------------------------------------------------------------------------------------------
  // Build Cryogenic Ring segment as tube 
  // - let Struts touch the Vacuum Vessel as an approximation
  // - account for correct concave profile at sides touching the Vacuum Vessel 
  //------------------------------------------------------------------------------------------

  double alpha   = asin( radius/rOut );
  double sagitta = rOut - radius/tan(alpha);

  GeoTube* sCryoringTube = new GeoTube( rIn, radius, halfLength + sagitta ); 

  // empty tube to cut out the concave profiles at the sides
  GeoTube* sEmptyTube = new GeoTube( 0, rOut, 2 * radius );
  

  //------------------------------------------------------------------------------------------
  // cut out left and right profiles
  
  // empty tubes have to be rotated
  HepTransform3D xfLeftTube  = HepTranslateZ3D( -halfLength - rOut ) * HepRotateX3D( M_PI/2 );
  HepTransform3D xfRightTube = HepTranslateZ3D(  halfLength + rOut ) * HepRotateX3D( M_PI/2 );

  const GeoShape& sCryoring = sCryoringTube->subtract( (*sEmptyTube) << xfLeftTube ).
                                             subtract( (*sEmptyTube) << xfRightTube );

  GeoLogVol*  lCryoring = new GeoLogVol( "BTCryoring", &sCryoring, theMaterialManager->getMaterial(cryoringMaterial) );
  GeoPhysVol* pCryoring = new GeoPhysVol(lCryoring);


  //------------------------------------------------------------------------------------------
  // replicate and position
  Variable iGen;
  GENFUNCTION f = ( 2*iGen + 1 ) * phi0;
  
  TRANSFUNCTION XFCryoring = Pow( HepRotateZ3D(1.0), f ) * 
                                  HepTranslate3D( rMean + centreXPos, centreYPos, centreZPos ) *
                                  HepRotateX3D( -M_PI/2 ) * HepRotateY3D(phi0); 
  GeoSerialTransformer* sxCryoring = new GeoSerialTransformer( pCryoring, &XFCryoring, 8 );
  container->add(sxCryoring);

}

  void BarrelToroidBuilderRDB::buildRails( GeoPhysVol* container ) 
{
  ////////////////////////////////////////////////////////////////////////////////////////////
  //  Builds Rails on Feet                                                                  //
  ////////////////////////////////////////////////////////////////////////////////////////////

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 

//---------- FRLS -- Feet Rail Parameters ------------------------------------//
//                                                                            //
                                                                              //
   const double zLengthCentralRail 	    = (*m_Rail)[0]->getFloat("CERZLENG") * mm;
   const double zLengthExtrRails	    = (*m_Rail)[0]->getFloat("EXRZLENG") * mm;
   const double yMinRails                   = (*m_Rail)[0]->getFloat("YPOS")     * mm;
   const double yheightCentrPlateRail       = (*m_Rail)[0]->getFloat("CERTHIC2") * mm;
   const double xPosRails                   = (*m_Rail)[0]->getFloat("XPOS")     * mm;
   const double x2MinTopPlateRails          = (*m_Rail)[0]->getFloat("CERWID3I") * mm;
   const double x2MaxTopPlateRails          = (*m_Rail)[0]->getFloat("CERWID3O") * mm;
   const double xWidthBotPlateRails         = (*m_Rail)[0]->getFloat("CERWIDT1") * mm;
   const double thicknessHorizPlatesRails   = (*m_Rail)[0]->getFloat("CERTHIC1") * mm;
   const double thicknessVerticalPlateRails = (*m_Rail)[0]->getFloat("CERWIDT2") * mm;
   const double zLengthExtrRibsRail	    = (*m_Rail)[0]->getFloat("CERRPEZL") * mm;
   const double zLengthCentralRibsRail	    = (*m_Rail)[0]->getFloat("CERRPSZL") * mm;
   const double x2InPosEdgeRibsRails	    = (*m_Rail)[0]->getFloat("CERRPIX1") * mm;
   const double x2OutPosEdgeRibsRails	    = (*m_Rail)[0]->getFloat("CERRPOX1") * mm;
   const double angleInnerRibsCutOut        = 55.0				 * deg;
   const double angleOuterRibsCutOut	    = 50.0				 * deg;
   const double zPosEdgeRibCentrRail1	    = (*m_Rail)[0]->getFloat("CERRPE1Z") * mm;
   const double zPosEdgeRibCentrRail2	    = (*m_Rail)[0]->getFloat("CERRPE2Z") * mm;
   const double zPosEdgeRibExtrRail1	    = (*m_Rail)[0]->getFloat("EXRRPE1Z") * mm;
   const double zPosEdgeRibExtrRail2	    = (*m_Rail)[0]->getFloat("EXRRPE2Z") * mm;
   
   const int nRibsCentralRail  = 11;
   const int nRibsExtrRails    = 17;
   
   double zPosRibsCentralRail[nRibsCentralRail];
   double zPosRibsExtrRails[nRibsExtrRails];
   
   for (int i = 0; i < nRibsCentralRail; i++ )
   {
   zPosRibsCentralRail[i] = (*m_Rail)[0]->getFloat("CERRPSZP",i) * mm;
   }

   for (int i = 0; i < nRibsExtrRails; i ++)
   {
   zPosRibsExtrRails[i]   = (*m_Rail)[0]->getFloat("EXRRPSZP",i) * mm;
   }                                                                             //
   const std::string railMaterial = getMaterial("Iron"); 
//                                                                            //
//----------------------------------------------------------------------------//
  
  const double tolerance = 0.5*mm;

  // general Ribs Rail properties
  const double xLengthRibRail   = x2InPosEdgeRibsRails + x2OutPosEdgeRibsRails
				  + thicknessVerticalPlateRails;
  const double thicknessCentrRR = zLengthCentralRibsRail;
  const double thicknessExtrRR  = zLengthExtrRibsRail;

  // positioning ribs
  for (int i = 0; i <nRibsCentralRail; i++ )
  {
  zPosRibsCentralRail[i] = zPosRibsCentralRail[i] - zLengthCentralRibsRail/2;
  }
  for (int i = 0; i<nRibsExtrRails; i++ )
  {
  zPosRibsExtrRails[i] = zPosRibsExtrRails[i] - zLengthExtrRibsRail/2;
  }
  
  // general Rail properties
  const double zLength = zLengthCentralRail + 2 * zLengthExtrRails;
  const double yMin    = yMinRails; 
  const double yMax    = yMin + yheightCentrPlateRail + 2 * thicknessHorizPlatesRails; 
  
  // top+bottom plate properties
  const double thicknessPlates    = thicknessHorizPlatesRails;  
  
  // top plate properties 
  const double xMinTopPlate       = xPosRails - x2MinTopPlateRails; 
  const double xMaxTopPlate       = xPosRails + x2MaxTopPlateRails; 
  const double xWidthTopPlate     = xMaxTopPlate - xMinTopPlate; 
  
  // bottom plate properties 
  const double xWidthBotPlate     = xWidthBotPlateRails; 
  
  // vertical plate properties 
  const double thicknessVertPlate = thicknessVerticalPlateRails;  
  const double yHeightVertPlate   = yheightCentrPlateRail; 
  
  
  const double xylengthInRibBoxCut	= ( x2InPosEdgeRibsRails - xWidthBotPlate/2 + thicknessVertPlate/2) * cos(angleInnerRibsCutOut);

  const double xylengthOutRibBoxCut	= ( x2OutPosEdgeRibsRails - xWidthBotPlate/2 + thicknessVertPlate/2) * cos(angleOuterRibsCutOut);

  // positioning variables ...
  //  ... for vertical plate centre 
  const double xMean = xPosRails;   
  const double yMean = 0.5 * ( yMax + yMin );   
  
  //  ... for top plate centre (shifted in x)
  const double xShiftTopPlate = 0.5 * ( xMaxTopPlate + xMinTopPlate ) - xMean;   


  
  //------------------------------------------------------------------------------------------
  //  Build the Rails as union of the following box shapes:
  //   - central, vertical plate
  //   - horizontal bottom plate, aligned in x with central plate
  //   - horizontal top plate, shifted outside in x wrt. the central plate
  //------------------------------------------------------------------------------------------

  // central, vertical plate
  const GeoShape* sCentPlate = new GeoBox( thicknessVertPlate/2, yHeightVertPlate/2 - tolerance, zLength/2 ); 
  
  // bottom plate
  GeoBox* sBotPlate  = new GeoBox( xWidthBotPlate/2, thicknessPlates/2 - tolerance, zLength/2 ); 
  
  // top plate
  GeoBox* sTopPlate  = new GeoBox( xWidthTopPlate/2, thicknessPlates/2 - tolerance, zLength/2 ); 
  
  // central ribs
  const GeoShape* sCentrRib  = new GeoBox( xLengthRibRail/2, yHeightVertPlate/2 - tolerance, thicknessCentrRR/2 );
  
  // extr ribs
  const GeoShape* sExtrRib   = new GeoBox(xLengthRibRail/2, yHeightVertPlate/2 - tolerance, thicknessExtrRR/2 );
  
  //empty central box from ribs
  GeoBox* sEmptyCBoxRib   = new GeoBox(thicknessVertPlate/2 + tolerance,
				       yHeightVertPlate/2, thicknessCentrRR );
  
  HepTransform3D trlEmptyCBoxRib = HepTranslateX3D( (x2OutPosEdgeRibsRails - x2InPosEdgeRibsRails)/2 );

  //empty inner&outer boxes from ribs
  GeoBox* sEmptyInBoxRib  = new GeoBox(xylengthInRibBoxCut, 2*xylengthInRibBoxCut, thicknessCentrRR );
  GeoBox* sEmptyOutBoxRib = new GeoBox(xylengthOutRibBoxCut, 2*xylengthOutRibBoxCut, thicknessCentrRR );
    
  HepTransform3D trlEmptyInBoxRib = HepTranslate3D( xLengthRibRail/2, -yHeightVertPlate/2, 0. )
						    * HepRotateZ3D(-angleInnerRibsCutOut);

  HepTransform3D trlEmptyOutBoxRib = HepTranslate3D( -xLengthRibRail/2, -yHeightVertPlate/2, 0. )
						     * HepRotateZ3D(angleOuterRibsCutOut);
  // build rails ribs
  sCentrRib = &( sCentrRib->subtract( (*sEmptyCBoxRib) << trlEmptyCBoxRib ).
			    subtract( (*sEmptyInBoxRib) << trlEmptyInBoxRib ).
			    subtract( (*sEmptyOutBoxRib) << trlEmptyOutBoxRib ) );

  const double deltax = -(x2OutPosEdgeRibsRails - x2InPosEdgeRibsRails)/2;

  sExtrRib  = &( sExtrRib->subtract( (*sEmptyCBoxRib) << trlEmptyCBoxRib ).
			   subtract( (*sEmptyInBoxRib) << trlEmptyInBoxRib ).
			   subtract( (*sEmptyOutBoxRib) << trlEmptyOutBoxRib ) );

  for (int i = 0; i < nRibsCentralRail; i++ )
  {
  sCentPlate = & (sCentPlate->add( (*sCentrRib) << HepTranslate3D( deltax, 0., zPosRibsCentralRail[i]
							 	  - zLengthCentralRail/2) ) );
  }
  
  for (int i = 0; i < nRibsExtrRails; i++ )
  {
  sCentPlate = &( sCentPlate->add( (*sExtrRib) << HepTranslate3D( deltax, 0., zPosRibsExtrRails[i]
								- zLengthCentralRail/2
								- zLengthExtrRails) ) );
  sCentPlate = &( sCentPlate->add( (*sExtrRib) << HepTranslate3D( deltax, 0., zPosRibsExtrRails[i]
								 + zLengthCentralRail/2) ) );
  }
// add ribs to the edge of central rail part and 2 extremity rail parts

  sCentPlate = &( sCentPlate->add( (*sExtrRib) << HepTranslate3D( deltax, 0., zPosEdgeRibCentrRail1 ) ).
			      add( (*sExtrRib) << HepTranslate3D( deltax, 0., zPosEdgeRibCentrRail2 ) ).
			      add( (*sExtrRib) << HepTranslate3D( deltax, 0., -zLengthCentralRail/2 - zLengthExtrRails/2 + zPosEdgeRibExtrRail1 ) ).
			      add( (*sExtrRib) << HepTranslate3D( deltax, 0., -zLengthCentralRail/2 - zLengthExtrRails/2 + zPosEdgeRibExtrRail2 ) ).
			      add( (*sExtrRib) << HepTranslate3D( deltax, 0., zLengthCentralRail/2 + zLengthExtrRails/2 + zPosEdgeRibExtrRail1 ) ).
			      add( (*sExtrRib) << HepTranslate3D( deltax, 0., zLengthCentralRail/2 + zLengthExtrRails/2 + zPosEdgeRibExtrRail2 ) ) );
						     
  //------------------------------------------------------------------------------------------
  // add bottom/top plates to central plate
  double yPosPlates = 0.5 * ( yHeightVertPlate + thicknessPlates );
  const GeoShape& sRail = sCentPlate->add( (*sBotPlate) << HepTranslateY3D( -yPosPlates ) ).
                                      add( (*sTopPlate) << HepTranslate3D( -xShiftTopPlate, yPosPlates, 0 ) );

  GeoLogVol*  lRail = new GeoLogVol( "Rail", &sRail, theMaterialManager->getMaterial(railMaterial) );
  GeoPhysVol* pRail = new GeoPhysVol(lRail);


  //------------------------------------------------------------------------------------------
  // replicate and position
  //------------------------------------------------------------------------------------------
  // array of x-positions
  const int nRails = 2;
  double xPos[nRails] = { -xMean, xMean };

  // array of rotation angles (flipping around y-axis)
  double rotAngle[nRails] = { 0, M_PI };

  GENFUNCTION fRot = ArrayFunction( rotAngle, rotAngle + nRails );
  GENFUNCTION fTrl = ArrayFunction( xPos,     xPos     + nRails );

  TRANSFUNCTION XFRail = Pow( HepTranslateX3D(1.0), fTrl ) * 
                         HepTranslateY3D(yMean) *             // constant displacement     
                         Pow( HepRotateY3D(1.0), fRot );
  
  GeoSerialTransformer* sxRail = new GeoSerialTransformer( pRail, &XFRail, nRails );
  container->add(sxRail);

} 



std::string BarrelToroidBuilderRDB::getMaterial( std::string materialName ) 
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
    log  <<  "Barrel ToroidBuilderRDB::getMaterial: material "  <<  materialName  <<  " not defined! "  
         <<  " Take Aluminium instead." 
         <<  endreq;
    return "std::Aluminium";           
  }  
}  


} // namespace MuonGM


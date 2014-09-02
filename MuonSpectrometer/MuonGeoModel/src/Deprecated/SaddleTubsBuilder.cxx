/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Writed by Andrei Barashkou JINR(Russia,Dubna) 2007.
#include "MuonGeoModel/SaddleTubsBuilder.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "MuonGeoModel/ArrayFunction.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
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
SaddleTubsBuilder::SaddleTubsBuilder( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
				    std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{

  _unavailableData = false;
//   m_Saddletuvp     = pRDBAccess->getRecordset("SADDLETUVP","SADDLETUVP-00");
//   m_Saddletuvs     = pRDBAccess->getRecordset("SADDLETUVS","SADDLETUVS-00");
//   m_Saddletuvn     = pRDBAccess->getRecordset("SADDLETUVN","SADDLETUVN-00");
// //  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
// //  m_Fbovstu   = pRDBAccess->getRecordset("FBOVSTU","FBOVSTU-TEST-00");
// //  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
// //  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
// //  m_Fetubes   = pRDBAccess->getRecordset("FETUBES","FETUBES-TEST-00");
// //  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
// //  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
//   m_Saddle     = pRDBAccess->getRecordset("SADDLE","SADDLE-00");

  m_Saddletuvp     = pRDBAccess->getRecordset("SADDLETUVP", geoTag, geoNode);
  m_Saddletuvs     = pRDBAccess->getRecordset("SADDLETUVS",geoTag, geoNode);
  m_Saddletuvn     = pRDBAccess->getRecordset("SADDLETUVN",geoTag, geoNode);
//  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
//  m_Fbovstu   = pRDBAccess->getRecordset("FBOVSTU","FBOVSTU-TEST-00");
//  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
//  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
//  m_Fetubes   = pRDBAccess->getRecordset("FETUBES","FETUBES-TEST-00");
//  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
//  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
  m_Saddle     = pRDBAccess->getRecordset("SADDLE",geoTag, geoNode);
  if (m_Saddle->size()*m_Saddletuvn->size()*m_Saddletuvs->size()*m_Saddletuvp->size() == 0) _unavailableData=true;

  std::string Iron = "Iron";
  
  std::string Aluminium = "Alum";

  MsgStream log( Athena::getMessageSvc(), "MuGM:SaddleTubsBuilder" );
  if (!_unavailableData) log << MSG::INFO << "All data fetched with tag <" << geoTag << ">" << endreq;
  else log << MSG::INFO << "No data found with tag <" << geoTag << ">" << endreq;
}

void SaddleTubsBuilder::buildSaddleTubs( GeoPhysVol* container ) 
{

  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 
  int n = 0; //need for position of volumes
  int f = 0; //for tube subtraction
  int g = 0; //for box subtraction
//  double tolerance = 0.5 * mm;
  
  int Numtubs	= (int)(*m_Saddle)[0]->getFloat("SADL_NUMTUBES");
  
  double  Xposb[100];
  double  Yposb[100];
  double  Zposb[100];
  double Xposgl[100];
  double Yposgl[100];
  double Zposgl[100];
  double   Rotx[100];
  double   Roty[100];
  double   Rotz[100];
  for (int k = 0; k < Numtubs; k++)
  {
//  data from TUVN  
    int NumVol    = (int)(*m_Saddletuvn)[k]->getFloat("NUMVOL");
    int Numembo   = (int)(*m_Saddletuvn)[k]->getFloat("NUMEMBO");
    int Numemtu   = (int)(*m_Saddletuvn)[k]->getFloat("NUMEMTU");
    //int Numembr	  = (int)(*m_Saddletuvn)[k]->getFloat("NUMEMBR");
    int SymX      = (int)(*m_Saddletuvn)[k]->getFloat("SYMX");
    int SymY      = (int)(*m_Saddletuvn)[k]->getFloat("SYMY");
    int SymZ      = (int)(*m_Saddletuvn)[k]->getFloat("SYMZ");
    
//  data from TUVS
    float Rmin       = (*m_Saddletuvs)[k]->getFloat("RMIN") * mm;
    float Rmax       = (*m_Saddletuvs)[k]->getFloat("RMAX") * mm;
    float Dz         = (*m_Saddletuvs)[k]->getFloat("DZ") * mm;  
    float AlphaStart = (*m_Saddletuvs)[k]->getFloat("ALPHASTART") * mm;
    float Alpha      = (*m_Saddletuvs)[k]->getFloat("ALPHA") * mm;  
    
//    const std::string ExFName  	   = getName(*m_Efbrvebp->"VOLNAME"); 
//    const std::string feetMaterial  = getMaterial(*m_Efbrven, "MATERIAL");
    const std::string saddleMaterial = getMaterial( "Iron" );
// //ispul'zui kol-vo ob'emov i vinisi za predeli zikla ->v nachalo
//     double Xposb[NumVol + n];
//     double Yposb[NumVol + n];
//     double Zposb[NumVol + n];
//     double Xposgl[NumVol + n];
//     double Yposgl[NumVol + n];
//     double Zposgl[NumVol + n];
//     double Rotx[NumVol + n];
//     double Roty[NumVol + n];
//     double Rotz[NumVol +n];
//std::cout <<" m: "<<m <<std::endl;
//creating box volume
//-------------------  
  GeoTubs* Tubs = new GeoTubs(Rmin , Rmax, Dz/2, AlphaStart * M_PI/180, Alpha * M_PI/180);
//-------------------  
  
  const GeoShape* sTubs = &( *Tubs );

  //GeoTube* EmptyTube[Numemtu+f]; HepTransform3D trlEmptyTube[Numemtu+f];
  //GeoBox* EmptyBox[Numembo+g]; HepTransform3D trlEmptyBox[Numembo+g];
  //GeoTube* EmptyTube[100]; HepTransform3D trlEmptyTube[100];
  //GeoBox* EmptyBox[100]; HepTransform3D trlEmptyBox[100];
/*
//tubes for subtraction
  for (int i = f; i < Numemtu + f; i++)
  {
  int Whichtube       = (*m_Saddletuvstu)[i]->getFloat("WHICHTUBE");
  double Rminetube    = (*m_Saddleetubes)[Whichtube]->getFloat("RMIN") * mm;
  double Rmaxetube    = (*m_Saddleetubes)[Whichtube]->getFloat("RMAX") * mm;
  double DZetube      = (*m_Saddleetubes)[Whichtube]->getFloat("DZ") * mm;
  double Alphastart   = (*m_Saddleetubes)[Whichtube]->getFloat("Alphastart") * deg;
  double Alpha        = (*m_Saddleetubes)[Whichtube]->getFloat("Alpha") * deg;
  double Xposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("XPOS") * mm;
  double Yposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("YPOS") * mm;
  double Zposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ZPOS") * mm;
  double RotXetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTX") * deg;
  double RotYetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTY") * deg;
  double RotZetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTZ") * deg;
                       
  EmptyTube[i] = new GeoTube(Rminetube, Rmaxetube, DZetube);
            
  trlEmptyTube[i] = HepTranslate3D(Xposetube, Yposetube, Zposetube) * HepRotateY3D(RotYetube)
                                  * HepRotateZ3D(RotZetube) * HepRotateX3D(RotXetube);

    sTubs = &( sTubs->subtract( (*EmptyTube[i]) << trlEmptyTube[i] ) );
  }
*/  
f = f + Numemtu;
  
//boxes for subtraction
/*  for (int i = g; i < Numembo + g; i++)
  {
  int WhichBox		= (*m_Saddletuvsbo)[i]->getFloat("WHICHBOX");
  double DXembox	= (*m_Saddleeboxes)[WhichBox]->getFloat("DX") * mm;
  double DYembox        = (*m_Saddleeboxes)[WhichBox]->getFloat("DY") * mm;
  double DZembox        = (*m_Saddleeboxes)[WhichBox]->getFloat("DZ") * mm;
  double Xposembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("XPOS") * mm;
  double Yposembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("YPOS") * mm;
  double Zposembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("ZPOS") * mm;
  double RotXembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("ROTX") * deg;
  double RotYembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("ROTY") * deg;
  double RotZembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("ROTZ") * deg;
  
  EmptyBox[i] = new GeoBox(DXembox/2, DYembox/2, DZembox/2);
  
  trlEmptyBox[i] = HepTranslate3D(Xposembox, Yposembox, Zposembox) * HepRotateZ3D(RotZembox)
				* HepRotateY3D(RotYembox) * HepRotateX3D(RotXembox);

    sBox = &( sBox->subtract( (*EmptyBox[i]) << trlEmptyBox[i] ) );
  }
*/  
g = g + Numembo;  
  
    for (int j = n; j < NumVol + n; j++)
      {
        Zposgl[j] = (*m_Saddletuvp)[j]->getFloat("ZPOSGL") * mm;
        Xposgl[j] = (*m_Saddletuvp)[j]->getFloat("XPOSGL") * mm;
        Yposgl[j] = (*m_Saddletuvp)[j]->getFloat("YPOSGL") * mm;
	Zposb[j]  = (*m_Saddletuvp)[j]->getFloat("ZPOSB") * mm;
        Xposb[j]  = (*m_Saddletuvp)[j]->getFloat("XPOSB") * mm;
        Yposb[j]  = (*m_Saddletuvp)[j]->getFloat("YPOSB") * mm;
        Rotx[j]   = (*m_Saddletuvp)[j]->getFloat("ROTX") * deg;
        Roty[j]   = (*m_Saddletuvp)[j]->getFloat("ROTY") * deg;
        Rotz[j]   = (*m_Saddletuvp)[j]->getFloat("ROTZ") * deg;

//Xposgl[0]=Xposgl[1]=Xposgl[2]=Xposgl[3]=Yposgl[0]=Yposgl[1]=Yposgl[2]=Yposgl[3]=0;


    HepTransform3D trlTubs = /*HepTranslate3D(0., 0., 0.) */ HepRotateY3D(Roty[j])
					    * HepRotateZ3D(Rotz[j]) * HepRotateX3D(Rotx[j]);
    
    const GeoShape& posTubsPlate = ( *sTubs << trlTubs );
    
  GeoLogVol*  lTubsPlate = new GeoLogVol( "SaddleTubs",
                                            &posTubsPlate,
                                          theMaterialManager->getMaterial(saddleMaterial) );
  GeoPhysVol* pTubsPlate = new GeoPhysVol(lTubsPlate);


//  std::cout<<" SaddleTubsBuilder constructor2 "<<std::endl;
  //-----------------------------------------------------------------------------------------------------	
  //  const int nXpos = SymX + 1.;
  int nXpos = SymX + 1;
  int nYpos = SymY + 1;
  int nZpos = SymZ + 1;

  //  double  rotAngleX[nXpos], xPosBrepPlate[nXpos], rotAngleY[nYpos], yPosBrepPlate[nYpos], rotAngleZ[nZpos], zPosBrepPlate[nZpos];

  double rotAngleX[2] = { 0, M_PI }; double xPosTubsPlate[2] = {Xposgl[j]+Xposb[j], -Xposgl[j]-Xposb[j]};
  double rotAngleY[2] = { 0, M_PI }; double yPosTubsPlate[2] = {Yposgl[j]+Yposb[j], -Yposgl[j]-Yposb[j]};
  double rotAngleZ[2] = { 0, M_PI }; double zPosTubsPlate[2] = {Zposgl[j]+Zposb[j], -Zposgl[j]-Zposb[j]};
  const int nAllpos = nXpos * nYpos * nZpos;
//   double zPosTubsAux[nAllpos], xPosTubsAux[nAllpos], yPosTubsAux[nAllpos], 
//   rotAngleTubsAuxX[nAllpos], rotAngleTubsAuxY[nAllpos], rotAngleTubsAuxZ[nAllpos];
  double zPosTubsAux[100], xPosTubsAux[100], yPosTubsAux[100], 
      rotAngleTubsAuxX[100], rotAngleTubsAuxY[100], rotAngleTubsAuxZ[100];

  for ( int w = 0; w < nAllpos; w++ )
  {
     int ww = w % (nAllpos / nZpos),
         ss = w / (nAllpos / nXpos),
         ee = w / (nAllpos / nYpos);
     //std::cout <<" ww: "<<ww <<std::endl;
     //std::cout <<" ss: "<<ss <<std::endl;
     //std::cout <<" ee: "<<ee <<std::endl;
     zPosTubsAux[w]     = zPosTubsPlate[ww];
//          std::cout <<" ZPosTubsAux: "<<zPosTubsAux[w] <<std::endl;
     xPosTubsAux[w]     = xPosTubsPlate[ss];
//          std::cout <<" XPosTubsAux: "<<xPosTubsAux[w] <<std::endl;
     yPosTubsAux[w]     = yPosTubsPlate[ee];
//          std::cout <<" YPosTubsAux: "<<yPosTubsAux[w] <<std::endl;
     rotAngleTubsAuxX[w] = rotAngleX[ss];
//          std::cout <<" rotAngleTubsAuxX: "<<rotAngleTubsAuxX[w] <<std::endl;
     rotAngleTubsAuxY[w] = rotAngleY[ee];
//          std::cout <<" rotAngleTubsAuxY: "<<rotAngleTubsAuxY[w] <<std::endl;
     rotAngleTubsAuxZ[w] = rotAngleZ[ss];
//          std::cout <<" rotAngleTubsAuxZ: "<<rotAngleTubsAuxZ[w] <<std::endl;
  }

 GENFUNCTION fTubsRotX = ArrayFunction( rotAngleTubsAuxX, rotAngleTubsAuxX + nAllpos );
 GENFUNCTION fTubsTrlX = ArrayFunction( xPosTubsAux, xPosTubsAux + nAllpos );// + nXpos * NumVol );
 GENFUNCTION fTubsRotY = ArrayFunction( rotAngleTubsAuxY, rotAngleTubsAuxY + nAllpos );
 GENFUNCTION fTubsTrlY = ArrayFunction( yPosTubsAux, yPosTubsAux + nAllpos );
 GENFUNCTION fTubsRotZ = ArrayFunction( rotAngleTubsAuxZ, rotAngleTubsAuxZ +  nAllpos );
 GENFUNCTION fTubsTrlZ = ArrayFunction( zPosTubsAux, zPosTubsAux +  nAllpos );

 TRANSFUNCTION XFTubsPlate   = Pow( HepTranslateX3D(1.0), fTubsTrlX ) *
  		               Pow( HepTranslateY3D(1.0), fTubsTrlY ) *                           
                               Pow( HepTranslateZ3D(1.0), fTubsTrlZ ) * 
                               Pow( HepRotateX3D(1.0), fTubsRotX ) *
                               Pow( HepRotateY3D(1.0), fTubsRotY ) *
  	                       Pow( HepRotateZ3D(1.0), fTubsRotZ );

GeoSerialTransformer* sxTubsPlate = new GeoSerialTransformer( pTubsPlate,
                                                              &XFTubsPlate,
                                                              nAllpos );


container->add(sxTubsPlate);
      }
      
n = n + NumVol;      
NumVol = 0;
//std::cout<<" TEto konez progi sd"<<std::endl;
  }
}
    
std::string SaddleTubsBuilder::getMaterial( std::string materialName ) 
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
   log  <<  " SaddleTubsBuilder::getMaterial: material "  <<  materialName  
	<<  " not defined! "
        <<  " Take Aluminium instead."
        <<  endreq;
   return "std::Aluminium";           
  }  
}  

} // namespace MuonGM

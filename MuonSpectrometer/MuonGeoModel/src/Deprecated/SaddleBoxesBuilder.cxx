/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Writed by Andrei Barashkou JINR(Russia,Dubna) 2007.
#include "MuonGeoModel/SaddleBoxesBuilder.h"
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
SaddleBoxesBuilder::SaddleBoxesBuilder( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
				    std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
    
  _unavailableData = false;
    
  // get tables according to MuonSpectrometer tag !!!!!!!!!!!!!!!!!!!!!!!! 
  //m_Saddlebovp     = pRDBAccess->getRecordset("SADDLEBOVP","SADDLEBOVP-00");
  //m_Saddlebovs     = pRDBAccess->getRecordset("SADDLEBOVS","SADDLEBOVS-00");
  //m_Saddlebovn     = pRDBAccess->getRecordset("SADDLEBOVN","SADDLEBOVN-00");
  // //  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
  // //  m_Fbovstu   = pRDBAccess->getRecordset("FBOVSTU","FBOVSTU-TEST-00");
  // //  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
  // //  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
  // //  m_Fetubes   = pRDBAccess->getRecordset("FETUBES","FETUBES-TEST-00");
  // //  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
  // //  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
  //   m_Saddle     = pRDBAccess->getRecordset("SADDLE","SADDLE-00");

  m_Saddlebovp     = pRDBAccess->getRecordset("SADDLEBOVP", geoTag, geoNode);
  m_Saddlebovs     = pRDBAccess->getRecordset("SADDLEBOVS", geoTag, geoNode);
  m_Saddlebovn     = pRDBAccess->getRecordset("SADDLEBOVN", geoTag, geoNode);
  //  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
  //  m_Fbovstu   = pRDBAccess->getRecordset("FBOVSTU","FBOVSTU-TEST-00");
  //  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
  //  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
  //  m_Fetubes   = pRDBAccess->getRecordset("FETUBES","FETUBES-TEST-00");
  //  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
  //  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
  m_Saddle     = pRDBAccess->getRecordset("SADDLE", geoTag, geoNode);
  if (m_Saddle->size()*m_Saddlebovp->size()*m_Saddlebovs->size()*m_Saddlebovn->size() == 0)
    _unavailableData = true;
  
  
  std::string Iron = "Iron";
  
  std::string Aluminium = "Alum";

  MsgStream log( Athena::getMessageSvc(), "MuGM:SaddleBoxesBuilder" );
  if (!_unavailableData) log << MSG::INFO << "All data fetched with tag <" << geoTag << ">" << endreq;
  else log << MSG::INFO << "No data found with tag <" << geoTag << ">" << endreq;
}

void SaddleBoxesBuilder::buildSaddleBoxes( GeoPhysVol* container ) 
{
  //std::cout<<" SaddleBoxBuilder constructor1 "<<std::endl;
    
  const StoredMaterialManager*  theMaterialManager;
  if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
  {
    return;
  } 
  int n = 0; //need for position of volumes
  int f = 0; //for tube subtraction
  int g = 0; //for box subtraction
  //  double tolerance = 0.5 * mm;
  
  int Numboxes	= (int)(*m_Saddle)[0]->getFloat("SADL_NUMBOXES");
  
  double  Xposb[100];
  double  Yposb[100];
  double  Zposb[100];
  double Xposgl[100];
  double Yposgl[100];
  double Zposgl[100];
  double   Rotx[100];
  double   Roty[100];
  double   Rotz[100];
  for (int k = 0; k < Numboxes; k++)
  {
    //  data from FBOVN  
    int NumVol    = (int)(*m_Saddlebovn)[k]->getFloat("NUMVOL");
    int Numembo   = (int)(*m_Saddlebovn)[k]->getFloat("NUMEMBO");
    int Numemtu   = (int)(*m_Saddlebovn)[k]->getFloat("NUMEMTU");
    //int Numembr	  = (int)(*m_Saddlebovn)[k]->getFloat("NUMEMBR");
    int SymX      = (int)(*m_Saddlebovn)[k]->getFloat("SYMX");
    int SymY      = (int)(*m_Saddlebovn)[k]->getFloat("SYMY");
    int SymZ      = (int)(*m_Saddlebovn)[k]->getFloat("SYMZ");
    
    //  data from FBOVS
    float Dx	    = (*m_Saddlebovs)[k]->getFloat("DX") * mm;
    float Dy      = (*m_Saddlebovs)[k]->getFloat("DY") * mm;
    float Dz      = (*m_Saddlebovs)[k]->getFloat("DZ") * mm;
    //std::cout<<" k = "<<k+1<<" out of Numboxes = "<<Numboxes<<" NumVol = "<<NumVol<<std::endl;
    
    //    const std::string ExFName  	   = getName(*m_Efbrvebp->"VOLNAME"); 
    //    const std::string feetMaterial  = getMaterial(*m_Efbrven, "MATERIAL");
    const std::string saddleMaterial = getMaterial( "Iron" );
    //ispul'zui kol-vo ob'emov i vinisi za predeli zikla ->v nachalo
    //         double Xposb[NumVol + n];
    //         double Yposb[NumVol + n];
    //         double Zposb[NumVol + n];
    //         double Xposgl[NumVol + n];
    //         double Yposgl[NumVol + n];
    //         double Zposgl[NumVol + n];
    //         double Rotx[NumVol + n];
    //         double Roty[NumVol + n];
    //         double Rotz[NumVol +n];
    //std::cout <<" m: "<<m <<std::endl;
    //creating box volume
    //-------------------  
    GeoBox* Box = new GeoBox(Dx/2 , Dy/2, Dz/2);
    //-------------------  
  
    const GeoShape* sBox = &( *Box );

    //         GeoTube* EmptyTube[Numemtu+f]; HepTransform3D trlEmptyTube[Numemtu+f];
    //         GeoBox* EmptyBox[Numembo+g]; HepTransform3D trlEmptyBox[Numembo+g];

    //tubes for subtraction
    /*  for (int i = f; i < Numemtu + f; i++)
        {
        int Whichtube       = (*m_Saddlebovstu)[i]->getFloat("WHICHTUBE");
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

        sBox = &( sBox->subtract( (*EmptyTube[i]) << trlEmptyTube[i] ) );
        }
    */  
    f = f + Numemtu;
  
    //boxes for subtraction
    /*  for (int i = g; i < Numembo + g; i++)
        {
        int WhichBox		= (*m_Saddlebovsbo)[i]->getFloat("WHICHBOX");
        double DXembox	= (*m_Saddleeboxes)[WhichBox]->getFloat("DX") * mm;
        double DYembox        = (*m_Saddleeboxes)[WhichBox]->getFloat("DY") * mm;
        double DZembox        = (*m_Saddleeboxes)[WhichBox]->getFloat("DZ") * mm;
        double Xposembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("XPOS") * mm;
        double Yposembox      = (*m_Saddleeboxes)[WhichBox]->getFloat("YPOS") * mm;
        double Zposembox      = (*m_Saddeleboxes)[WhichBox]->getFloat("ZPOS") * mm;
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
      //std::cout<<"j = "<<j+1<<" out of numvol "<<NumVol+n<<" starting j = n = "<<n<<std::endl;
      Zposgl[j] = (*m_Saddlebovp)[j]->getFloat("ZPOSGL") * mm;
      Xposgl[j] = (*m_Saddlebovp)[j]->getFloat("XPOSGL") * mm;
      Yposgl[j] = (*m_Saddlebovp)[j]->getFloat("YPOSGL") * mm;
      Zposb[j]  = (*m_Saddlebovp)[j]->getFloat("ZPOSB") * mm;
      Xposb[j]  = (*m_Saddlebovp)[j]->getFloat("XPOSB") * mm;
      Yposb[j]  = (*m_Saddlebovp)[j]->getFloat("YPOSB") * mm;
      Rotx[j]   = (*m_Saddlebovp)[j]->getFloat("ROTX") * deg;
      Roty[j]   = (*m_Saddlebovp)[j]->getFloat("ROTY") * deg;
      Rotz[j]   = (*m_Saddlebovp)[j]->getFloat("ROTZ") * deg;

      //std::cout<<"variables read "<<std::endl;
        
      HepTransform3D trlBox = HepTranslate3D(Xposb[j], Yposb[j], 0.) * HepRotateY3D(Roty[j])
                              * HepRotateZ3D(Rotz[j]) * HepRotateX3D(Rotx[j]);
    
      const GeoShape& posBoxPlate = ( *sBox << trlBox );
    
      GeoLogVol*  lBoxPlate = new GeoLogVol( "SaddleBoxes",
                                             &posBoxPlate,
                                             theMaterialManager->getMaterial(saddleMaterial) );
      GeoPhysVol* pBoxPlate = new GeoPhysVol(lBoxPlate);


      //  std::cout<<" SaddleBoxBuilder constructor2 "<<std::endl;
      //-----------------------------------------------------------------------------------------------------	
      //  const int nXpos = SymX + 1.;
      int nXpos = SymX + 1;
      int nYpos = SymY + 1;
      int nZpos = SymZ + 1;
      //std::cout<<"Here nXpos, nYpos, nZpos = "<<nXpos<<" "<<nYpos<<" "<<nZpos<<std::endl;

      double rotAngleX[2] = { 0, M_PI }; 
      double xPosBoxPlate[2] = {Xposgl[j], -Xposgl[j]};
      double rotAngleY[2] = { 0, M_PI }; 
      double yPosBoxPlate[2] = {Yposgl[j], -Yposgl[j]};
      double rotAngleZ[2] = { 0, M_PI }; double zPosBoxPlate[2] = {(Zposgl[j] + Zposb[j]), -(Zposgl[j] + Zposb[j])};

      const int nAllpos = nXpos * nYpos * nZpos;
      //std::cout<<"nAllpos = "<<nAllpos<<std::endl;
	    
      //double zPosAux[nAllpos], xPosAux[nAllpos], yPosAux[nAllpos], rotAngleAuxX[nAllpos], rotAngleAuxY[nAllpos], rotAngleAuxZ[nAllpos];
      double zPosAux[100], xPosAux[100], yPosAux[100], rotAngleAuxX[100], rotAngleAuxY[100], rotAngleAuxZ[100];

      for ( int w = 0; w < nAllpos; w++ )
      {
        //std::cout<<" w = "<<w<<std::endl;
        int ww = w % (nAllpos / nZpos),
            ss = w / (nAllpos / nXpos),
            ee = w / (nAllpos / nYpos);
        //std::cout <<" ww: "<<ww <<std::endl;
        //std::cout <<" ss: "<<ss <<std::endl;
        //std::cout <<" ee: "<<ee <<std::endl;
        //std::cout<<" ww, ss, ee = "<<ww<<" "<<ss<<" "<<ee<<std::endl;
        zPosAux[w]     = zPosBoxPlate[ww];
        //          std::cout <<" ZPosAux: "<<zPosAux[w] <<std::endl;
        xPosAux[w]     = xPosBoxPlate[ss];
        //std::cout <<" XPosAux: "<<xPosAux[w] <<std::endl;
        yPosAux[w]     = yPosBoxPlate[ee];
        //std::cout <<" YPosAux: "<<yPosAux[w] <<std::endl;
        rotAngleAuxX[w] = rotAngleX[ss];
        //          std::cout <<" rotAngleAuxX: "<<rotAngleAuxX[w] <<std::endl;
        rotAngleAuxY[w] = rotAngleY[ee];
        //          std::cout <<" rotAngleAuxY: "<<rotAngleAuxY[w] <<std::endl;
        rotAngleAuxZ[w] = rotAngleZ[ss];
        //          std::cout <<" rotAngleAuxZ: "<<rotAngleAuxZ[w] <<std::endl;
      }

      GENFUNCTION fRotX = ArrayFunction( rotAngleAuxX, rotAngleAuxX + nAllpos );
      GENFUNCTION fTrlX = ArrayFunction( xPosAux, xPosAux + nAllpos );// + nXpos * NumVol );
      GENFUNCTION fRotY = ArrayFunction( rotAngleAuxY, rotAngleAuxY );
      GENFUNCTION fTrlY = ArrayFunction( yPosAux, yPosAux + nAllpos );
      GENFUNCTION fRotZ = ArrayFunction( rotAngleAuxZ, rotAngleAuxZ +  nAllpos );
      GENFUNCTION fTrlZ = ArrayFunction( zPosAux, zPosAux +  nAllpos );

      TRANSFUNCTION XFBoxPlate   = Pow( HepTranslateX3D(1.0), fTrlX ) *
                                   Pow( HepTranslateY3D(1.0), fTrlY ) *                           
                                   Pow( HepTranslateZ3D(1.0), fTrlZ ) * 
                                   Pow( HepRotateX3D(1.0), fRotX ) *
                                   Pow( HepRotateY3D(1.0), fRotY ) *
                                   Pow( HepRotateZ3D(1.0), fRotZ );

      //std::cout<<"Do we come here ??? "<<std::endl;
      GeoSerialTransformer* sxBoxPlate = new GeoSerialTransformer( pBoxPlate,
                                                                   &XFBoxPlate,
                                                                   nAllpos );


      container->add(sxBoxPlate);
      //std::cout<<"Do we come there ??? "<<std::endl;
    }
      
    n = n + NumVol;      
    NumVol = 0;
    //std::cout<<" TEto konez progi sd"<<std::endl;
  }
}
    
std::string SaddleBoxesBuilder::getMaterial( std::string materialName ) 
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
    log  <<  " SaddleBoxesBuilder::getMaterial: material "  <<  materialName  
         <<  " not defined! "
         <<  " Take Aluminium instead."
         <<  endreq;
    return "std::Aluminium";           
  }  
}  

} // namespace MuonGM

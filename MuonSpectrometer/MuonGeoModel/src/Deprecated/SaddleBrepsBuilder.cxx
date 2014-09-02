/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Writed by Andrei Barashkou JINR(Russia,Dubna) 2007.
#include "MuonGeoModel/SaddleBrepsBuilder.h"
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
SaddleBrepsBuilder::SaddleBrepsBuilder( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
				    std::string    geoNode )    :
    m_pRDBAccess(pRDBAccess),
    m_pDetStore (pDetStore)
{

  _unavailableData = false;
  
//   m_Saddlebrvp     = pRDBAccess->getRecordset("SADDLEBRVP","SADDLEBRVP-00");
//   m_Saddlebrvbp    = pRDBAccess->getRecordset("SADDLEBRVBP","SADDLEBRVBP-00");
//   m_Saddlebrvn     = pRDBAccess->getRecordset("SADDLEBRVN","SADDLEBRVN-00");
// //  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
//   m_Saddlebrvstu   = pRDBAccess->getRecordset("SADDLEBRVSTU","SADDLEBRVSTU-00");
// //  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
// //  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
//   m_Saddleetubes   = pRDBAccess->getRecordset("SADDLEETUBES","SADDLEETUBES-00");
// //  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
// //  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
//   m_Saddle     = pRDBAccess->getRecordset("SADDLE","SADDLE-00");

  m_Saddlebrvp     = pRDBAccess->getRecordset("SADDLEBRVP", geoTag, geoNode);
  m_Saddlebrvbp    = pRDBAccess->getRecordset("SADDLEBRVBP", geoTag, geoNode);
  m_Saddlebrvn     = pRDBAccess->getRecordset("SADDLEBRVN", geoTag, geoNode);
  //  m_Fbrvsbo   = pRDBAccess->getRecordset("FBRVSBO","FBRVSBO-TEST-00");
  m_Saddlebrvstu   = pRDBAccess->getRecordset("SADDLEBRVSTU", geoTag, geoNode);
  //  m_Fbrvsbr   = pRDBAccess->getRecordset("FBRVSBR","FBRVSBR-TEST-00");
  //  m_Feboxes   = pRDBAccess->getRecordset("FEBOXES","FEBOXES-TEST-00");
  m_Saddleetubes   = pRDBAccess->getRecordset("SADDLEETUBES", geoTag, geoNode);
  //  m_Febreps   = pRDBAccess->getRecordset("FEBREPS","FEBREPS-TEST-00");
  //  m_Febrepsxy = pRDBAccess->getRecordset("FEBREPSXY","FEBREPSXY-TEST-00");
  m_Saddle     = pRDBAccess->getRecordset("SADDLE", geoTag, geoNode);
  if (m_Saddlebrvp->size()*m_Saddlebrvbp->size()*m_Saddlebrvn->size()*
      m_Saddlebrvstu->size()*m_Saddleetubes->size()*m_Saddle->size()==0) _unavailableData = true;

  std::string Iron = "Iron";
  
  std::string Aluminium = "Alum";

  MsgStream log( Athena::getMessageSvc(), "MuGM:SaddleBrepsBuilder" );
  if (!_unavailableData) log << MSG::INFO << "All data fetched with tag <" << geoTag << ">" << endreq;
  else log << MSG::INFO << "No data found with tag <" << geoTag << ">" << endreq;
}

void SaddleBrepsBuilder::buildSaddleBreps( GeoPhysVol* container ) 
{
        //std::cout<<"  constructor1 "<<std::endl;

        const StoredMaterialManager*  theMaterialManager;
        if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) ) 
        {
            return;
        } 
        int m = 0; //need for creating dif. Breps
        int n = 0; //need for position of volumes
        int f = 0; //for tube subtraction
        //int g = 0; //for box subtraction
        //int q = 0; //for brep subtraction
        //  double tolerance = 0.5 * mm;
  
        int Numbreps	= (int)(*m_Saddle)[0]->getFloat("SADL_NUMBREPS");
  
        double   Xpos[100];
        double   Ypos[100];
        double   Zpos[100];
        double Zposgl[100];
        double  Zposb[100];
        double Xposgl[100];
        double Yposgl[100];
        double   Rotx[100];
        double   Roty[100];
        double   Rotz[100];
        for (int k = 0; k < Numbreps; k++)
        {
            //  data from Saddlebrvn  
            int NumVol    = (int)(*m_Saddlebrvn)[k]->getFloat("NUMVOL");
            int Nump      = (int)(*m_Saddlebrvn)[k]->getFloat("NUMP");
            double dZ     = (*m_Saddlebrvn)[k]->getFloat("DZ") * mm;
            //    int Numembo   = (*m_Saddlebrvn)[k]->getInt("NUMEMBO");
            int Numemtu   = (int)(*m_Saddlebrvn)[k]->getFloat("NUMEMTU");
            const int SymX      = (int)(*m_Saddlebrvn)[k]->getFloat("SYMX");
            const int SymY      = (int)(*m_Saddlebrvn)[k]->getFloat("SYMY");
            const int SymZ      = (int)(*m_Saddlebrvn)[k]->getFloat("SYMZ");
            //    int Numembr	  = (*m_Saddlebrvn)[k]->getInt("NUMEMBR");
    
            //    const std::string ExFName  	   = getName(*m_Efbrvebp->"VOLNAME"); 
            //    const std::string feetMaterial  = getMaterial(*m_Efbrven, "MATERIAL");
            const std::string feetMaterial = getMaterial( "Iron" );
//             //ispul'zui kol-vo ob'emov i vinisi za predeli zikla ->v nachalo
//             double Xpos[Nump + m];
//             double Ypos[Nump + m];
//             double Zpos[NumVol + n];
//             double Zposgl[NumVol + n];
//             double Zposb[NumVol + n];
//             double Xposgl[NumVol + n];
//             double Yposgl[NumVol + n];
//             double Rotx[NumVol + n];
//             double Roty[NumVol + n];
//             double Rotz[NumVol +n];
            //std::cout <<" m: "<<m <<std::endl;
  
            for (int i = m; i < Nump + m; i++)
            {
                Xpos[i] = (*m_Saddlebrvp)[i]->getFloat("XPOS") * mm;
                Ypos[i] = (*m_Saddlebrvp)[i]->getFloat("YPOS") * mm;	
            }

            GeoSimplePolygonBrep* BrepPlate = new GeoSimplePolygonBrep(dZ/2);
            for (int i = m; i < Nump + m ; i++)
            {
                BrepPlate->addVertex(Xpos[i],Ypos[i]);		//description breps volume
            }
                                              
            m = m + Nump;

            const GeoShape* sBrepPlate = &( *BrepPlate );

            //GeoTubs* EmptyTube[Numemtu+f]; HepTransform3D trlEmptyTube[Numemtu+f];
            GeoTubs* EmptyTube[100]; HepTransform3D trlEmptyTube[100];
            //GeoBox* EmptyBox[Numembo+g]; HepTransform3D trlEmptyBox[Numembo+g];
            //GeoSimplePolygonBrep* EmptyBrep[Numembr]; HepTransform3D trlEmptyBrep[Numembr];

            //tubes for subtraction
            for (int i = f; i < Numemtu + f; i++)
            {
                int Whichtube       = (int)(*m_Saddlebrvstu)[i]->getFloat("WHICHTUBE");
                double Rminetube    = (*m_Saddleetubes)[Whichtube]->getFloat("RMIN") * mm;
                double Rmaxetube    = (*m_Saddleetubes)[Whichtube]->getFloat("RMAX") * mm;
                double DZetube      = (*m_Saddleetubes)[Whichtube]->getFloat("DZ") * mm;
                //  double Xposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("XPOS") * mm;
                //  double Yposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("YPOS") * mm;
                double Xposetube    = -(*m_Saddlebrvbp)[Whichtube]->getFloat("XPOSG") * mm;
                double Yposetube    = -(*m_Saddlebrvbp)[Whichtube]->getFloat("YPOSG") * mm;
                double Zposetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ZPOS") * mm;
                double RotXetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTX") * deg;
                double RotYetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTY") * deg;
                double RotZetube    = (*m_Saddleetubes)[Whichtube]->getFloat("ROTZ") * deg;
                       
                EmptyTube[i] = new GeoTubs(Rminetube, Rmaxetube, DZetube/2, 301.8 * M_PI/180, 32.* M_PI/180 );
            
                trlEmptyTube[i] = HepTranslate3D(Xposetube, Yposetube, Zposetube) * HepRotateY3D(RotYetube)
                                  * HepRotateZ3D(RotZetube) * HepRotateX3D(RotXetube);

                sBrepPlate = &( sBrepPlate->subtract(*EmptyTube[i]) << trlEmptyTube[i] );
            }
  
            f = f + Numemtu;
  
            //boxes for subtraction
            /*  for (int i = g; i < Numembo + g; i++)
                {
                int WhichBox		= (*m_Fbrvsbo)[i]->getInt("WHICHBOX");
                double DXembox	= (*m_Feboxes)[WhichBox]->getFloat("DX") * mm;
                double DYembox        = (*m_Feboxes)[WhichBox]->getFloat("DY") * mm;
                double DZembox        = (*m_Feboxes)[WhichBox]->getFloat("DZ") * mm;
                double Xposembox      = (*m_Feboxes)[WhichBox]->getFloat("XPOS") * mm;
                double Yposembox      = (*m_Feboxes)[WhichBox]->getFloat("YPOS") * mm;
                double Zposembox      = (*m_Feboxes)[WhichBox]->getFloat("ZPOS") * mm;
                double RotXembox      = (*m_Feboxes)[WhichBox]->getFloat("ROTX") * deg;
                double RotYembox      = (*m_Feboxes)[WhichBox]->getFloat("ROTY") * deg;
                double RotZembox      = (*m_Feboxes)[WhichBox]->getFloat("ROTZ") * deg;
  
                EmptyBox[i] = new GeoBox(DXembox/2, DYembox/2, DZembox/2);
  
                trlEmptyBox[i] = HepTranslate3D(Xposembox, Yposembox, Zposembox) * HepRotateZ3D(RotZembox)
                * HepRotateY3D(RotYembox) * HepRotateX3D(RotXembox);

                sSidePlate = &( sSidePlate->subtract( (*EmptyBox[i]) << trlEmptyBox[i] ) );
                }
  
                g = g + Numembo;  
            */
            //breps for subtraction
            /*    double Xpossub[4];
                  double Ypossub[4];

                  for (int i = q; i < Numembr + q; i++)
                  {
                  int WhichBrep          = (*m_Fbrvsbr)[i]->getInt("WHICHBREP");
                  double Dzembrep        = (*m_Febreps)[WhichBrep]->getFloat("DZ") * mm;
                  double Zposembrep      = (*m_Febreps)[WhichBrep]->getFloat("ZPOS") * mm;
                  double RotXembrep      = (*m_Febreps)[WhichBrep]->getFloat("ROTX") * deg;
                  double RotYembrep      = (*m_Febreps)[WhichBrep]->getFloat("ROTY") * deg;
                  double RotZembrep      = (*m_Febreps)[WhichBrep]->getFloat("ROTZ") * deg;
              
                  for (int j = 0; j < 4; j++)
                  {
                  Xpossub[j] = (*m_Febrepsxy)[j]->getFloat("XPOS") * mm;
                  Ypossub[j] = (*m_Febrepsxy)[j]->getFloat("YPOS") * mm;
                  }
                  EmptyBrep[i] = new GeoSimplePolygonBrep(Dzembrep/2);
                  for (int j = 0; j < 4; j++)
                  {
                  EmptyBrep[i]->addVertex(Xpossub[j],Ypossub[j]);
                  }
      
                  trlEmptyBrep[i] = HepTranslateZ3D(Zposembrep) * HepRotateZ3D(RotZembrep)
                  * HepRotateY3D(RotYembrep) * HepRotateX3D(RotXembrep); 
  
                  sSidePlate = &( sSidePlate->subtract( (*EmptyBrep[i]) << trlEmptyBrep[i] ) );                                 
                  }                             

                  q = q + Numembr;
            */     
            /*    for (int i = 0; i < SymX; i++)
                  {
                  HepTransform3D trlMirBrepPlate = HepReflectX3D(-1100.);
    
                  sBrepPlate = &( sBrepPlate->add( (*sBrepPlate) << trlMirBrepPlate ) );
                  }
            */    

            //replication and position of volume  
            for (int j = n; j < NumVol + n; j++)
            {
                Zposgl[j] = (*m_Saddlebrvbp)[j]->getFloat("ZPOSG") * mm;
                Zposb[j]  = (*m_Saddlebrvbp)[j]->getFloat("ZPOSB") * mm;
                Xposgl[j] = (*m_Saddlebrvbp)[j]->getFloat("XPOSG") * mm;
                Yposgl[j] = (*m_Saddlebrvbp)[j]->getFloat("YPOSG") * mm;
                Rotx[j] = (*m_Saddlebrvbp)[j]->getFloat("ROTX") * deg;
                Roty[j] = (*m_Saddlebrvbp)[j]->getFloat("ROTY") * deg;
                Rotz[j] = (*m_Saddlebrvbp)[j]->getFloat("ROTZ") * deg;
                Zpos[j] = Zposgl[j] + Zposb[j];

                //	    HepTransform3D trlBrepPlate = HepTranslate3D(Xposgl[j], Yposgl[j], Zpos[j]) * HepRotateZ3D(Rotz[j])
                //       		* HepRotateY3D(Roty[j]) * HepRotateX3D(Rotx[j]);
                HepTransform3D trlBrepPlate = HepTranslate3D(Xposgl[j], Yposgl[j], 0.) * HepRotateZ3D(Rotz[j])
                                              * HepRotateY3D(Roty[j]) * HepRotateX3D(Rotx[j]);
   
                const GeoShape& posBrepPlate = ( *sBrepPlate << trlBrepPlate );
    
                GeoLogVol*  lBrepPlate = new GeoLogVol( "SaddleBrepPlate",
                                                        &posBrepPlate,
                                                        theMaterialManager->getMaterial(feetMaterial) );
                /*  std::cout<<" SaddleBrepsBuilder constructor1 "<<std::endl;
                    GeoLogVol*  lBrepPlate = new GeoLogVol( "SaddleBrepPlate",
                    *&sBrepPlate,
                    theMaterialManager->getMaterial(feetMaterial) );
                */
                GeoPhysVol* pBrepPlate = new GeoPhysVol(lBrepPlate);
  
                //  std::cout<<" SaddleBrepsBuilder constructor2 "<<std::endl;
                //-----------------------------------------------------------------------------------------------------	
                //  const int nXpos = SymX + 1.;
                const int nXpos = SymX + 1;
                const int nYpos = SymY + 1;
                const int nZpos = SymZ + 1;

                //  double  rotAngleX[nXpos], xPosBrepPlate[nXpos], rotAngleY[nYpos], yPosBrepPlate[nYpos], rotAngleZ[nZpos], zPosBrepPlate[nZpos];

                double rotAngleX[2] = { 0, M_PI }; double xPosBrepPlate[2] = { Xposgl[j], -Xposgl[j] };
                double rotAngleY[1] = { 0 }; double yPosBrepPlate[1] = {Yposgl[j]};
                double rotAngleZ[2] = { 0, M_PI }; double zPosBrepPlate[2] = {Zpos[j], -Zpos[j]};
                const int nAllpos = nXpos * nYpos * nZpos;
                //                double zPosBrepsAux[nAllpos], xPosBrepsAux[nAllpos], yPosBrepsAux[nAllpos], rotAngleBrepsAuxX[nAllpos],
                //                    rotAngleBrepsAuxY[nAllpos], rotAngleBrepsAuxZ[nAllpos];
                double zPosBrepsAux[100], xPosBrepsAux[100], yPosBrepsAux[100], rotAngleBrepsAuxX[100],
                    rotAngleBrepsAuxY[100], rotAngleBrepsAuxZ[100];

                for ( int w = 0; w < nAllpos; w++ )
                {
                    int ww = w % (nAllpos / nZpos),
                        ss = w / (nAllpos / nXpos),
                        ee = w / (nAllpos / nYpos);
                    //std::cout <<" ww: "<<ww <<std::endl;
                    //std::cout <<" ss: "<<ss <<std::endl;
                    //std::cout <<" ee: "<<ee <<std::endl;
                    zPosBrepsAux[w]     = zPosBrepPlate[ww];
                    //          std::cout <<" ZPosBrepsAux: "<<zPosBrepsAux[w] <<std::endl;
                    xPosBrepsAux[w]     = xPosBrepPlate[ss];
                    //          std::cout <<" XPosBrepsAux: "<<xPosBrepsAux[w] <<std::endl;
                    yPosBrepsAux[w]     = yPosBrepPlate[ee];
                    //          std::cout <<" YPosBrepsAux: "<<yPosBrepsAux[w] <<std::endl;
                    rotAngleBrepsAuxX[w] = rotAngleX[ss];
                    //          std::cout <<" rotAngleBrepsAuxX: "<<rotAngleBrepsAuxX[w] <<std::endl;
                    rotAngleBrepsAuxY[w] = rotAngleY[ee];
                    //          std::cout <<" rotAngleBrepsAuxY: "<<rotAngleBrepsAuxY[w] <<std::endl;
                    rotAngleBrepsAuxZ[w] = rotAngleZ[ss];
                    //          std::cout <<" rotAngleBrepsAuxZ: "<<rotAngleBrepsAuxZ[w] <<std::endl;
                }

                GENFUNCTION fBrepsRotX = ArrayFunction( rotAngleBrepsAuxX, rotAngleBrepsAuxX + nAllpos );
                GENFUNCTION fBrepsTrlX = ArrayFunction( xPosBrepsAux, xPosBrepsAux + nAllpos );
                GENFUNCTION fBrepsRotY = ArrayFunction( rotAngleBrepsAuxY, rotAngleBrepsAuxY + nAllpos );
                GENFUNCTION fBrepsTrlY = ArrayFunction( yPosBrepsAux, yPosBrepsAux + nAllpos );
                GENFUNCTION fBrepsRotZ = ArrayFunction( rotAngleBrepsAuxZ, rotAngleBrepsAuxZ +  nAllpos );
                GENFUNCTION fBrepsTrlZ = ArrayFunction( zPosBrepsAux, zPosBrepsAux +  nAllpos );

                TRANSFUNCTION XFBrepPlate   = Pow( HepTranslateX3D(1.0), fBrepsTrlX ) *
                                              Pow( HepTranslateY3D(1.0), fBrepsTrlY ) *                           
                                              Pow( HepTranslateZ3D(1.0), fBrepsTrlZ ) * 
                                              Pow( HepRotateX3D(1.0), fBrepsRotX ) *
                                              Pow( HepRotateY3D(1.0), fBrepsRotY ) *
                                              Pow( HepRotateZ3D(1.0), fBrepsRotZ );

                GeoSerialTransformer* sxBrepPlate = new GeoSerialTransformer( pBrepPlate,
                                                                              &XFBrepPlate,
                                                                              nAllpos );

                container->add(sxBrepPlate);

                //  GeoSerialTransformer* sxBrepPlate = new GeoSerialTransformer( pBrepPlate,
                //                                                                &XFBrepPlate,
                //                                                                nZpos * nXpos * nYpos );


                //  container->add(pBrepPlate);
            }
      
            n = n + NumVol;      
            NumVol = 0;
            Nump	 = 0;
            dZ	 = 0.;      
            //std::cout<<" TEto konez progi sd"<<std::endl;
        }
}
    
std::string SaddleBrepsBuilder::getMaterial( std::string materialName ) 
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
        log  <<  " SaddleBrepsBuilder::getMaterial: material "  <<  materialName  
             <<  " not defined! "
             <<  " Take Aluminium instead."
             <<  endreq;
        return "std::Aluminium";           
    }  
}  

} // namespace MuonGM

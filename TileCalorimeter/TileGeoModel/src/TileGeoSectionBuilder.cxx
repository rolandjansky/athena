/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileGeoSectionBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"
#include "TileDetDescr/TileCellDim.h"

#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"

#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoCutVolAction.h"

#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoSerialTransformer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "boost/io/ios_state.hpp"
#include <stdexcept>
#include <iostream>
#include <limits>
#include <cmath>
#include <cstddef>

#include <assert.h>

using namespace GeoGenfun;
using namespace GeoXF;
  
TileGeoSectionBuilder::TileGeoSectionBuilder(const StoredMaterialManager* matManager,
					     TileDddbManager * pDbManager,
                                             int ushape,
                                             int glue,
                                             int cstube,
                                             MsgStream * log)
  : m_theMaterialManager(matManager)
  , m_dbManager(pDbManager)
  , m_log(log)
  , m_uShape(ushape)
  , m_glue(glue)
  , m_csTube(cstube)
  , m_barrelPeriodThickness(0.)
  , m_barrelGlue(0.)
  , m_extendedPeriodThickness(0.)
  , m_verbose(log->level()<=MSG::VERBOSE)
  , m_matLArServices(0)
  , m_matIronHalfDens(0)
  , m_additionalIronLayer(0.027)
{
}

TileGeoSectionBuilder::~TileGeoSectionBuilder()
{
}

void TileGeoSectionBuilder::fillSection(GeoPhysVol*&             mother,
				        int                      sec_number,
				        double                   tile_rmax,
				        double                   rminb,
				        double                   dzglue,
				        double                   delta_phi,
                                        int                      ModuleNcp,
				        double                   zlen_itc2,
					bool                     neg)
{
  (*m_log) << MSG::VERBOSE <<" TileGeoSectionBuilder::fillSection ModuleNcp= "<<ModuleNcp<< endmsg;

  double tan_delta_phi_2 = tan(delta_phi/2*Gaudi::Units::deg);
  
  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  // Obtain required materials - Air and Iron

  const GeoMaterial* matAir = m_theMaterialManager->getMaterial("std::Air");
  const GeoMaterial* matIron = m_theMaterialManager->getMaterial("std::Iron");
  const GeoMaterial* matAluminium = 0;

  // -----------------------------------------------------------------------------------------------------------------
  // Cut-outs
  //static bool First =true;
  double dX1 =0., dX2 =0., dY1 =0., dY2 =0., dZ1 =0., dZ2 =0.;
  std::string volname ="";

  // Cuting positioning
  float PosXcut =0., PosYcut =0., PosY =0., Rmore =0.; 
  float Radius =0., YcorA =0., YcorB =0., lenPla =0., Blia =0.;

  // General rotation and transformations 
  float phi = (double(ModuleNcp-1) + 0.5)*delta_phi;

  // Special module flag
  int SideFl = 1;
  if (ModuleNcp>=35 && ModuleNcp<=37) SideFl = -1;

  GeoTrf::Transform3D TransCut2(GeoTrf::Transform3D::Identity()); 
  GeoTrf::Transform3D TransCutL(GeoTrf::Transform3D::Identity());
  GeoTrf::Transform3D TransCutR(GeoTrf::Transform3D::Identity());

  const GeoShapeUnion *CutA{nullptr};
  GeoShape *CutB{nullptr};

  // ext. barrel Cuts description
  if (sec_number==2 && m_dbManager->BoolCuts() && ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62)))
   { 
      volname = "CutB"; m_dbManager->SetCurrentCuts(volname); 
      PosXcut = m_dbManager->CutsXpos(); 
      PosYcut = m_dbManager->CutsYpos();
      Rmore   = 0.8*Gaudi::Units::cm;

      // Inert materials, CutB1
      dX1 = m_dbManager->CutsDX1()+Rmore; 
      dX2 = m_dbManager->CutsDX2()+Rmore; 
      dY1 = m_dbManager->CutsDY1()+Rmore; 
      dY2 = m_dbManager->CutsDY2()+Rmore; 
      dZ1 = m_dbManager->CutsDZ1(); 

      checking("CutB1", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* CutB1 = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      //const GeoShape& CutB = *CutB1;
      CutB = CutB1;

      // Materials are in cuting region, 1up Iron plate
      volname = "Cut1up"; m_dbManager->SetCurrentCuts(volname); //>>
      PosY =  m_dbManager->CutsYpos();

      dX1 = m_dbManager->CutsDX1()+Rmore; 
      dX2 = m_dbManager->CutsDX2()+Rmore; 
      dY1 = m_dbManager->CutsDY1(); 
      dY2 = m_dbManager->CutsDY2(); 
      dZ1 = m_dbManager->CutsDZ1(); 

      checking("Cut1up", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* Cut1up = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);     

      volname = "Cut2down"; m_dbManager->SetCurrentCuts(volname); //>>
      dX1 = m_dbManager->CutsDX1()+Rmore; 
      dX2 = m_dbManager->CutsDX2()+Rmore; 
      dY1 = m_dbManager->CutsDY1(); 
      dY2 = m_dbManager->CutsDY2(); 
      dZ2 = m_dbManager->CutsDZ1(); 

      checking("Cut2down", false, 1, dX1,dX2,dY1,dY2,dZ2);
      GeoTrd* Cut1down = new GeoTrd(dX1,dX2,dY1,dY2,dZ2);
 
      GeoTrf::Translate3D yPosA(0.,0.,-dZ1-dZ2);

      const GeoShapeUnion& CutA1 = Cut1up->add(*Cut1down<<yPosA);
      CutA = &CutA1;
      
      Radius = (m_dbManager->TILBrmaximal() + m_dbManager->TILBrminimal())/2 * Gaudi::Units::cm; 

      if (ModuleNcp==35||ModuleNcp==62) { YcorA = 5*Gaudi::Units::cm;   YcorB = 5*Gaudi::Units::cm; lenPla =0.8*Gaudi::Units::cm, Blia = 17.4*Gaudi::Units::cm;}  
      if (ModuleNcp==36||ModuleNcp==61) { YcorA = 6.5*Gaudi::Units::cm; YcorB = 6*Gaudi::Units::cm; lenPla =1.7*Gaudi::Units::cm; Blia = 16.9*Gaudi::Units::cm;} 
      if (ModuleNcp==37||ModuleNcp==60) { YcorA = 8*Gaudi::Units::cm;   YcorB = 9*Gaudi::Units::cm; lenPla =2.8*Gaudi::Units::cm; Blia = 16.4*Gaudi::Units::cm;} 

      TransCut2 = GeoTrf::TranslateZ3D(-Radius) 
	* GeoTrf::RotateX3D((90-phi)*Gaudi::Units::deg) * GeoTrf::RotateY3D(180*Gaudi::Units::deg) 
	* GeoTrf::Translate3D(0.1*Gaudi::Units::cm,SideFl*17.5*Gaudi::Units::cm,-PosY+YcorA); 

      // For modules on the side C apply extra transformation
      // which implements ReflectZ(0)
      if(neg) {
	GeoTrf::Vector3D ptTmp = TransCut2*GeoTrf::Vector3D(0.,0.,0.);
	TransCut2 = GeoTrf::TranslateX3D(2*ptTmp.x())*GeoTrf::RotateZ3D(180*Gaudi::Units::deg)*TransCut2;
      }

      if (ModuleNcp>=60 && ModuleNcp<=62) {   
	TransCutL = GeoTrf::TranslateZ3D(-Radius) 
	  * GeoTrf::RotateY3D(180*Gaudi::Units::deg) * GeoTrf::RotateX3D(phi*Gaudi::Units::deg) 
	  * GeoTrf::Translate3D(-1.4*Gaudi::Units::cm,PosYcut+YcorB,-PosXcut-Blia); 

	// ReflectZ for C side
	if(neg) {
	  GeoTrf::Vector3D ptTmp = TransCutL*GeoTrf::Vector3D(0.,0.,0.);
	  TransCutL = GeoTrf::TranslateX3D(2*ptTmp.x())*GeoTrf::RotateZ3D(180*Gaudi::Units::deg)*TransCutL;
	}
      } else if (ModuleNcp>=35 && ModuleNcp<=37)  { 
	TransCutR = GeoTrf::TranslateZ3D(-Radius) 
	  * GeoTrf::RotateY3D(180*Gaudi::Units::deg) * GeoTrf::RotateX3D(phi*Gaudi::Units::deg) 
	  * GeoTrf::Translate3D(-1.4*Gaudi::Units::cm,PosYcut+YcorB,PosXcut+Blia)  
	  * GeoTrf::RotateY3D(180*Gaudi::Units::deg); 
	// ReflectZ for C side
	if(neg) {
	  GeoTrf::Vector3D ptTmp = TransCutR*GeoTrf::Vector3D(0.,0.,0.);
	  TransCutR = GeoTrf::TranslateX3D(2*ptTmp.x())*GeoTrf::RotateZ3D(180*Gaudi::Units::deg)*TransCutR;
	}
      }
      
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: CutA and CutB Ok"<< endmsg;
   } // end if,  BoolCuts

  //---------------------Girder-----------------------------------------
 
  int Id4 = m_dbManager->GetModType()%100;
  double thicknessGirderMother = 0.0;
  double specialModuleZShift = 0.0; 

  if (m_dbManager->TILBngirder() > 0)
  {
    // Mother volume for girder
    thicknessGirderMother = (m_dbManager->TILBdzmodul() - m_dbManager->TILBdzend() - m_dbManager->TILBdzend2())*Gaudi::Units::cm;
    // special module with special girder
    if ((Id4 == 7) && (sec_number == 3))
      thicknessGirderMother = (m_dbManager->TILBdzgir() - m_dbManager->TILBdzend() - m_dbManager->TILBdzend2())*Gaudi::Units::cm;

    double heightGirderMother = (tile_rmax - m_dbManager->TILBrmax())*Gaudi::Units::cm;
    double dy1GirderMother = m_dbManager->TILBrmax() * tan_delta_phi_2 * Gaudi::Units::cm;
    double dy2GirderMother = tile_rmax * tan_delta_phi_2 * Gaudi::Units::cm;
    // ps test the TILB DZGIR
    //     std::cout <<"\t\t PS Girder Module = "<<ModuleNcp<< std::endl;
    //     std::cout <<"\t\t PS thicknessGirderMother = "<<thicknessGirderMother<< std::endl;
    //ps account for special ITC modules 14,15,18,19
    if  ((Id4 == 7) && (sec_number == 3))
      {
	specialModuleZShift = 0.5*Gaudi::Units::cm*(m_dbManager->TILBdzgir() - m_dbManager->TILBdzmodul());
      }
    //
    checking("GirderMother", false, 3, 
        thicknessGirderMother/2,thicknessGirderMother/2,dy1GirderMother,dy2GirderMother,heightGirderMother/2);

    GeoTrd* girderMother = new GeoTrd(thicknessGirderMother/2,
				      thicknessGirderMother/2,
				      dy1GirderMother,
				      dy2GirderMother,
				      heightGirderMother/2);

    GeoLogVol* lvGirderMother = new GeoLogVol("GirderMother",girderMother,matAir);
    GeoPhysVol* pvGirderMother = new GeoPhysVol(lvGirderMother);

    fillGirder(pvGirderMother,
               tile_rmax,
               m_dbManager->TILBrmax(),
               tan_delta_phi_2,
               thicknessGirderMother*(1./Gaudi::Units::cm));

    GeoTransform* tfGirderMother = 0;

    if(sec_number==3)
      tfGirderMother = new GeoTransform(GeoTrf::Translate3D((m_dbManager->TILBdzend()-m_dbManager->TILBdzend2())*Gaudi::Units::cm/2, 0.,
						       (m_dbManager->TILBrmax()-m_dbManager->TILBrmin())*Gaudi::Units::cm/2));
    else
      tfGirderMother = new GeoTransform(GeoTrf::Translate3D((m_dbManager->TILBdzend()-m_dbManager->TILBdzend2())*Gaudi::Units::cm/2, 0.,
						       (m_dbManager->TILBrmax()-rminb)*Gaudi::Units::cm/2));

    mother->add(tfGirderMother);
    mother->add(pvGirderMother); 

    if(m_log->level()<=MSG::DEBUG)
      (*m_log) << MSG::DEBUG <<" _fillSection: GirderMother Ok "<< endmsg;

  } // End Girder

  //--------------------Front Plate-------------------------------------
  double thicknessFrontPlate, heightFrontPlate, dy1FrontPlate, dy2FrontPlate;
  double rless =.150; // 150 [mkm]
  int NbPeriod =0;

  if(m_dbManager->TILBdrfront() > 0)
  {
    if(sec_number==3)
    {
      //ITC coverplate
      thicknessFrontPlate = (m_dbManager->TILBdzmodul() - zlen_itc2)*Gaudi::Units::cm;

      if (thicknessFrontPlate > rless)
       {
         heightFrontPlate = m_dbManager->TILBdrfront()*Gaudi::Units::cm;
         dy1FrontPlate = (rminb*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
         dy2FrontPlate = (m_dbManager->TILBrmin()*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

	 if(m_log->level()<=MSG::DEBUG)
	   (*m_log) << MSG::DEBUG <<"   FrontPlateSh dX1,dX2= "<<thicknessFrontPlate/2<<", "<<thicknessFrontPlate/2
		    <<" dY1,dY2= "<<dy1FrontPlate<<" "<<dy2FrontPlate<<" dZ= "<<heightFrontPlate/2
		    << endmsg;

         GeoTrd* frontPlateSh = new GeoTrd(thicknessFrontPlate/2,
		                           thicknessFrontPlate/2,
					   dy1FrontPlate,
					   dy2FrontPlate,
					   heightFrontPlate/2);

         GeoLogVol* lvFrontPlateSh = new GeoLogVol("FrontPlateSh",frontPlateSh,matIron);
         GeoPhysVol* pvFrontPlateSh = new GeoPhysVol(lvFrontPlateSh);
         GeoTransform* tfFrontPlateSh = new GeoTransform(GeoTrf::Translate3D(
                    -m_dbManager->TILBdzmodul()/2*Gaudi::Units::cm+thicknessFrontPlate/2, 0., 
                    (rminb - tile_rmax)/2*Gaudi::Units::cm)); 

         mother->add(tfFrontPlateSh);
         mother->add(pvFrontPlateSh);
       } else {
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG <<"   FrontPlateSh was lost "<< endmsg;
       }
    }
    else if (sec_number==2 && (m_dbManager->BoolCuts() &&  
			      ((ModuleNcp>=35 && ModuleNcp<=37)||(ModuleNcp>=60 && ModuleNcp<=62)) ))
    { 


      std::string volname ="";
      double dXCutA = 0, dXCutB = 0;

      volname = "Cut1up"; m_dbManager->SetCurrentCuts(volname);
      dXCutA = m_dbManager->CutsDX1();

      volname = "CutB"; m_dbManager->SetCurrentCuts(volname); 
      dXCutB = m_dbManager->CutsDX1(); 
      
      thicknessFrontPlate = (m_dbManager->TILBdzmodul() - m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2())*Gaudi::Units::cm;
      heightFrontPlate = m_dbManager->TILBdrfront()*Gaudi::Units::cm;
      dy1FrontPlate = (rminb*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
      dy2FrontPlate = (m_dbManager->TILBrmin()*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

      GeoTrd* frontPlate = new GeoTrd(thicknessFrontPlate/2 -(dXCutA+dXCutB),
				      thicknessFrontPlate/2 -(dXCutA+dXCutB),
                                      dy1FrontPlate,
				      dy2FrontPlate,
				      heightFrontPlate/2);

      // Cuting of Plate
      /*
      GeoTrf::Transform3D  TCu2 = GeoTrf::RotateX3D((90-phi)*Gaudi::Units::deg) * GeoTrf::RotateY3D(180*Gaudi::Units::deg)
                           * GeoTrf::Translate3D(thicknessFrontPlate/2-dXCutA,0,0); 
      GeoTransform* TCu = new GeoTransform(TCu2);

      const GeoShape &tmp_frontPlate = frontPlate->subtract((*CutA)<<TCu2);
                                                  .subtract((*CutB)<<TransCutL); 
      */

      GeoLogVol* lvFrontPlate = new GeoLogVol("FrontPlate",frontPlate,matIron); 
      GeoPhysVol* pvFrontPlate = new GeoPhysVol(lvFrontPlate); 
      GeoTransform* tfFrontPlate = new GeoTransform(GeoTrf::Translate3D(
		    (m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2())/2*Gaudi::Units::cm+ dXCutB, 0.,
                    (m_dbManager->TILBrmin()-m_dbManager->TILBdrfront()/2-(tile_rmax + rminb)/2)*Gaudi::Units::cm));

      mother->add(tfFrontPlate);
      mother->add(pvFrontPlate);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG<<" _fillSection: FrontPlate Cut Ok "<< endmsg;

    }
    else
    {
      //Ordinary frontplate
      thicknessFrontPlate = (m_dbManager->TILBdzmodul() - m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2())*Gaudi::Units::cm;
      heightFrontPlate = m_dbManager->TILBdrfront()*Gaudi::Units::cm;
      dy1FrontPlate = (rminb*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
      dy2FrontPlate = (m_dbManager->TILBrmin()*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

      GeoTrd* frontPlate = new GeoTrd(thicknessFrontPlate/2,
				      thicknessFrontPlate/2,
				      dy1FrontPlate,
				      dy2FrontPlate,
				      heightFrontPlate/2);

      GeoLogVol* lvFrontPlate = new GeoLogVol("FrontPlate",frontPlate,matIron); 
      GeoPhysVol* pvFrontPlate = new GeoPhysVol(lvFrontPlate);
      GeoTransform* tfFrontPlate = new GeoTransform(GeoTrf::Translate3D(
                    (m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2())/2*Gaudi::Units::cm, 0.,
                    (m_dbManager->TILBrmin()-m_dbManager->TILBdrfront()/2-(tile_rmax + rminb)/2)*Gaudi::Units::cm));

      mother->add(tfFrontPlate);
      mother->add(pvFrontPlate);
    }
  } // End Front Plate

  //--------------------End Plates--------------------------------------
  double dy1EndPlate, dy2EndPlate, thicknessEndPlate, heightEndPlate;

  //VARIABLES FOR END PLATE HOLE  
  double heightEPHole = m_dbManager->TILBflangex()*Gaudi::Units::cm;
  double dyEPHole = m_dbManager->TILBflangex()*Gaudi::Units::cm/2;

  // ps . shifts for end plates in cutout regions 
  GeoTrf::Transform3D cutOutTransformation(GeoTrf::Transform3D::Identity()); 
  //first endplate
  GeoTransform* tfEndPlateSh{nullptr};

  if (m_dbManager->TILBdzend1() > 0)
  {
    if (m_dbManager->TILBdzend() < m_dbManager->TILBdzend1())
    {
 
      //Short endplate
      dy1EndPlate = rminb * tan_delta_phi_2 * Gaudi::Units::cm;
      dy2EndPlate = m_dbManager->TILBrmax() * tan_delta_phi_2 * Gaudi::Units::cm;
      thicknessEndPlate = m_dbManager->TILBdzend1() * Gaudi::Units::cm;
      heightEndPlate = (m_dbManager->TILBrmax() - rminb) * Gaudi::Units::cm;
      //
      // creating standart endplate. It is the same for 
      // both standard mosules and modules with cuts
      //
      GeoTrd* endPlateSh = new GeoTrd(thicknessEndPlate/2, 
				      thicknessEndPlate/2,
				      dy1EndPlate,
				      dy2EndPlate,
				      heightEndPlate/2);
      GeoLogVol* lvEndPlateSh = 0; 
      
      //      if ( sec_number==2 && ( (ModuleNcp==37)||( ModuleNcp==60) ) ) 
      if (sec_number==2 && ((ModuleNcp>=35 && ModuleNcp<=37)||(ModuleNcp>=60 && ModuleNcp<=62)) ) 
	{//Short endplate Cut-outs

	//
	//  shape for the cutted part
	//
	GeoTrd* endPlateShCut = new GeoTrd(thicknessEndPlate, 
					   thicknessEndPlate,
					   heightEndPlate/2.,
					   heightEndPlate/2.,
					   dy2EndPlate);

	
	double rotationAngle ;
	double shiftCutPlate ;
	int rotationSign = 1;
	if (ModuleNcp > 50) rotationSign *= -1;
	if ( neg ) rotationSign *= -1;


	if ( ( ModuleNcp == 37 ) || ( ModuleNcp == 60 ) )
	  {
	    rotationAngle = (180.0 - 25.3125 )* Gaudi::Units::deg ;  // ATLLEMS_0003 0004
	    shiftCutPlate =  38.7 * Gaudi::Units::mm;

	    cutOutTransformation = 
	      GeoTrf::Translate3D(0,0, -heightEndPlate/2.) *  
	      GeoTrf::RotateX3D(  90 * Gaudi::Units::deg ) *
	      GeoTrf::Translate3D(0.,0., -rotationSign * (dy2EndPlate + shiftCutPlate ) ) *
	      GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;
	    
	    const GeoShape & endPlateShCutted3760 = (endPlateSh->subtract( (*endPlateShCut)<< cutOutTransformation ) ) ;	    
	    lvEndPlateSh = new GeoLogVol("EndPlateSh", &(endPlateShCutted3760) , matIron);
	  }
	else if ( ( ModuleNcp == 36 ) || ( ModuleNcp == 61 ) )
	  {
	    rotationAngle = - ( 116.4832 - 90. )* Gaudi::Units::deg ;  // ATLLEMS_0005 0006
	    shiftCutPlate =  ( ( m_dbManager->TILBrmax() - rminb )*Gaudi::Units::cm - 1448.4 * Gaudi::Units::mm);

	    cutOutTransformation = 
	      GeoTrf::Translate3D( 0, 0, -heightEndPlate/2. ) *  
	      GeoTrf::Translate3D( 0, 0,  - (dy2EndPlate  - shiftCutPlate + 0.5*dy2EndPlate*(1.- std::cos(rotationAngle*Gaudi::Units::rad)))  ) *  
	      GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;

	    const GeoShape & endPlateShCutted3661 = (endPlateSh->subtract( (*endPlateShCut)<< cutOutTransformation ) ) ;	    
	    lvEndPlateSh = new GeoLogVol("EndPlateSh", &(endPlateShCutted3661) , matIron);
	  }
	else if ( ( ModuleNcp == 35 ) || ( ModuleNcp == 62 ) )
	  {
	    rotationAngle = - ( 104.0625 - 90.0 )* Gaudi::Units::deg ;  // ATLLEMS_0007 0008
	    shiftCutPlate =  ( ( m_dbManager->TILBrmax() - rminb )*Gaudi::Units::cm - 1534.6 * Gaudi::Units::mm);
	    
	    cutOutTransformation = 
	      GeoTrf::Translate3D( 0, 0, -heightEndPlate/2. ) *  
	      GeoTrf::Translate3D( 0, 0,  - (dy2EndPlate  - shiftCutPlate) ) *  
	      GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;
	    
	    const GeoShape & endPlateShCutted3562 = (endPlateSh->subtract( (*endPlateShCut)<< cutOutTransformation ) ) ;	    
	    lvEndPlateSh = new GeoLogVol("EndPlateSh", &(endPlateShCutted3562) , matIron);
	  }
	else
	  {
	    (*m_log) << MSG::ERROR <<" TileGeoSectionBuilder::fillSection . Wrong Module in cut-out region. ModuleNcp= "<<ModuleNcp<< endmsg;	
	    lvEndPlateSh = new GeoLogVol("EndPlateSh", endPlateSh , matIron);
	  }

	
	} 
      else 
	{
	  lvEndPlateSh = new GeoLogVol("EndPlateSh", endPlateSh , matIron);
	}
      



      // const GeoShape & endPlateShFinal = (endPlateSh->subtract( (*CutB)<<TransCutL ) );
      //subtract((*CutB)<<TransCutL); 

      GeoPhysVol* pvEndPlateSh = new GeoPhysVol(lvEndPlateSh);

      tfEndPlateSh = new GeoTransform(GeoTrf::Translate3D(
						     specialModuleZShift + 
						     (m_dbManager->TILBdzend1() - m_dbManager->TILBdzmodul())*Gaudi::Units::cm/2, 0.,
						     (m_dbManager->TILBrmax() - tile_rmax)*Gaudi::Units::cm/2));
      tfEndPlateSh->ref();
      
      mother->add(tfEndPlateSh);
      mother->add(pvEndPlateSh);
      tfEndPlateSh->unref();

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: ext.barrel EndPlateSh Ok "<< endmsg;
    }
    else
    {
      //Ordinary endplate
      dy1EndPlate = rminb * tan_delta_phi_2 * Gaudi::Units::cm;
      dy2EndPlate = tile_rmax * tan_delta_phi_2 * Gaudi::Units::cm;
      thicknessEndPlate = m_dbManager->TILBdzend1() * Gaudi::Units::cm;
      heightEndPlate = (tile_rmax-rminb)*Gaudi::Units::cm; 

      GeoTrd* endPlate1 = new GeoTrd(thicknessEndPlate/2,
				     thicknessEndPlate/2,
				     dy1EndPlate,
				     dy2EndPlate,
				     heightEndPlate/2);  

      GeoLogVol* lvEndPlate1 = new GeoLogVol("EndPlate1",endPlate1,matIron);
      GeoPhysVol* pvEndPlate1 = new GeoPhysVol(lvEndPlate1);

      //Position air hole
      if (m_dbManager->TILBflangex() > 0.)
      {
	GeoTrd* epHole1 = new GeoTrd (thicknessEndPlate/2,
				      thicknessEndPlate/2,
				      dyEPHole,
				      dyEPHole,
				      heightEPHole/2);

	GeoLogVol* lvEPHole1 = new GeoLogVol("EPHole1",epHole1,matAir);
	GeoPhysVol* pvEPHole1 = new GeoPhysVol(lvEPHole1);
	GeoTransform* tfEPHole1 = new GeoTransform(GeoTrf::Translate3D(0.,0.,
                                      (m_dbManager->TILBflangey()-(tile_rmax + rminb)/2)*Gaudi::Units::cm));
	pvEndPlate1->add(tfEPHole1);
	pvEndPlate1->add(pvEPHole1);
      }

      GeoTransform* tfEndPlate1 = new GeoTransform(GeoTrf::Translate3D(
                                      (m_dbManager->TILBdzend1() - m_dbManager->TILBdzmodul())*Gaudi::Units::cm/2, 0., 0.));
      mother->add(tfEndPlate1);
      mother->add(pvEndPlate1);  

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: Ordinary EndPlateSh Ok "<< endmsg;
    }
  }

  //second endplate
  GeoTransform* tfEndPlate2{nullptr};

  if (m_dbManager->TILBdzend2() > 0)
  {
      //Short endplate Cut-outs
      double radShift =lenPla;
      double rminbT=rminb + radShift;

      dy1EndPlate = rminb * tan_delta_phi_2 * Gaudi::Units::cm;
      dy2EndPlate = tile_rmax * tan_delta_phi_2 * Gaudi::Units::cm;
      thicknessEndPlate = m_dbManager->TILBdzend2() * Gaudi::Units::cm;
      heightEndPlate = (tile_rmax-rminb) * Gaudi::Units::cm;


      GeoLogVol* lvEndPlate2 = 0;
      GeoTrd* endPlate2 = new GeoTrd(thicknessEndPlate/2,
				     thicknessEndPlate/2,
				     dy1EndPlate,
				     dy2EndPlate,
				     heightEndPlate/2);  
      
      tfEndPlate2 = new GeoTransform(GeoTrf::Translate3D(
						    (-m_dbManager->TILBdzend2() + m_dbManager->TILBdzmodul())*Gaudi::Units::cm/2, 0., 0.));
      tfEndPlate2->ref();
      
      if (sec_number==2 && ((ModuleNcp>=35 && ModuleNcp<=37)||(ModuleNcp>=60 && ModuleNcp<=62)) ) 
	{//Short endplate Cut-outs
	  
	  GeoTrd* endPlate2Cut = new GeoTrd(thicknessEndPlate, 
					    thicknessEndPlate,
					    heightEndPlate/2.,
					    heightEndPlate/2.,
					    dy2EndPlate);
	  double rotationAngle ;
	  double shiftCutPlate ;
	  int rotationSign = 1;
	  if (ModuleNcp > 50) rotationSign *= -1;
	  if ( neg ) rotationSign *= -1;	  
	  
	  if ( ( ModuleNcp == 37 ) || ( ModuleNcp == 60 ) )
	    {
	      rotationAngle = - ( 115.3125 - 90.0 )* Gaudi::Units::deg ;  // ATLLEMS_0011 0012
	      shiftCutPlate =  ( ( m_dbManager->TILBrmax() - rminb )*Gaudi::Units::cm - 1364.0 * Gaudi::Units::mm);
	      
	      cutOutTransformation = 
		GeoTrf::Translate3D( 0, 0, -heightEndPlate/2. ) *  
		GeoTrf::Translate3D( 0, 0,  - (dy2EndPlate  - shiftCutPlate) ) *  
		GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;
	      
	      const GeoShape & endPlate2Cutted3760 = (endPlate2->subtract( (*endPlate2Cut)<< cutOutTransformation ) ) ;	    
	      lvEndPlate2 = new GeoLogVol("EndPlate2", &(endPlate2Cutted3760) , matIron);
	    }
	  else if ( ( ModuleNcp == 36 ) || ( ModuleNcp == 61 ) )
	    {
	      rotationAngle = - ( 109.6875 - 90.0 )* Gaudi::Units::deg ;  // ATLLEMS_0009 0010
	      shiftCutPlate =  ( ( m_dbManager->TILBrmax() - rminb )*Gaudi::Units::cm - 1464.0 * Gaudi::Units::mm);
	      
	      cutOutTransformation = 
		GeoTrf::Translate3D( 0, 0, -heightEndPlate/2. ) *  
		GeoTrf::Translate3D( 0, 0,  - (dy2EndPlate  - shiftCutPlate) ) *  
		GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;
	      
	      const GeoShape & endPlate2Cutted3661 = (endPlate2->subtract( (*endPlate2Cut)<< cutOutTransformation ) ) ;	    
	      lvEndPlate2 = new GeoLogVol("EndPlate2", &(endPlate2Cutted3661) , matIron);
	    }
	  else if ( ( ModuleNcp == 35 ) || ( ModuleNcp == 62 ) )
	    {
	      rotationAngle = - ( 104.0625 - 90.0 )* Gaudi::Units::deg ;  // ATLLEMS_0009 0010
	      shiftCutPlate =  ( ( m_dbManager->TILBrmax() - rminb )*Gaudi::Units::cm - ( 1915.0 -385.0 )* Gaudi::Units::mm); // girder is subtracted (no drawing)
	      
	      cutOutTransformation = 
		GeoTrf::Translate3D( 0, 0, -heightEndPlate/2. ) *  
		GeoTrf::Translate3D( 0, 0,  - (dy2EndPlate  - shiftCutPlate) ) *  
		GeoTrf::RotateX3D( rotationSign * rotationAngle ) ;
	      
	      const GeoShape & endPlate2Cutted3562 = (endPlate2->subtract( (*endPlate2Cut)<< cutOutTransformation ) ) ;	    
	      lvEndPlate2 = new GeoLogVol("EndPlate2", &(endPlate2Cutted3562) , matIron);
	    }
	  
	  //         dy1EndPlate = rminbT * tan_delta_phi_2 * Gaudi::Units::cm;
	  //         dy2EndPlate = tile_rmax * tan_delta_phi_2 * Gaudi::Units::cm;
	  //         thicknessEndPlate = m_dbManager->TILBdzend2() * Gaudi::Units::cm;
	  //         heightEndPlate = (tile_rmax - rminbT) * Gaudi::Units::cm;
	  
	  //         tfEndPlate2 = new GeoTransform(GeoTrf::Translate3D(
	  //                                       (-m_dbManager->TILBdzend2() + m_dbManager->TILBdzmodul())*Gaudi::Units::cm/2, 0, radShift/2*Gaudi::Units::cm));
	  
	}
      else
	{
	  lvEndPlate2 = new GeoLogVol("EndPlate2",endPlate2,matIron);
	}
      
      GeoPhysVol* pvEndPlate2 = new GeoPhysVol(lvEndPlate2);
      
      //Position air hole
      if (m_dbManager->TILBflangex() > 0)
      {
        dyEPHole = m_dbManager->TILBflangex()*Gaudi::Units::cm/2;

	GeoTrd* epHole2 = new GeoTrd (thicknessEndPlate/2,
				      thicknessEndPlate/2,
				      dyEPHole,
				      dyEPHole,
				      heightEPHole/2);

	GeoLogVol* lvEPHole2 = new GeoLogVol("EPHole2",epHole2,matAir);
	GeoPhysVol* pvEPHole2 = new GeoPhysVol(lvEPHole2);
	GeoTransform* tfEPHole2 = new GeoTransform(GeoTrf::Translate3D(0.,0.,
                                      (m_dbManager->TILBflangey()-(tile_rmax + rminbT)/2)*Gaudi::Units::cm));
	pvEndPlate2->add(tfEPHole2);
	pvEndPlate2->add(pvEPHole2);
      }

      mother->add(tfEndPlate2);
      mother->add(pvEndPlate2);
      tfEndPlate2->unref();
      
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: EndPlate2 Ok "<< endmsg;
     
  } // End Plates

  //---------------------------------------------------Absorber--------------------------------------------------------
  double heightAbsorber = (m_dbManager->TILBrmax() - m_dbManager->TILBrmin())*Gaudi::Units::cm;
  double thicknessAbsorber = (m_dbManager->TILBdzmodul() - m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2())*Gaudi::Units::cm;
  double dy1Absorber = (m_dbManager->TILBrmin()*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
  double dy2Absorber = (m_dbManager->TILBrmax()*tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

  checking("Absorber", true, 3, 
      thicknessAbsorber/2,thicknessAbsorber/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

  //----------------------------- Absorber -------------------------------------------------------------------
  double thicknessPeriod =0, thicknessAbsorber1 =0, thicknessAbsorber2 =0, thicknessAbsorber3 =0;
  double PosAbsor1 =0, PosAbsor2 =0, PosAbsor3 =0; 
  int nA1 =32, nA2 = 0, nA3 =16;

  GeoTrd *absorber{nullptr}, *absorber1{nullptr}, *absorber3{nullptr};
  GeoLogVol *lvAbsorber{nullptr}, *lvAbsorber1{nullptr}, *lvAbsorber3{nullptr};
  GeoPhysVol *pvAbsorber{nullptr}, *pvAbsorber1{nullptr}, *pvAbsorber3{nullptr};
  GeoPhysVol *pvTmp_Absorber1{nullptr}, *pvTmp_Absorber3{nullptr};
  
  // Perform different actions depending on sections
  switch (sec_number)
  {
  case 2:
    {
      //Extended barrel - consists of ordinary periods of type 1 only
      thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;

      // The period number for middle absorber
      nA2 = m_dbManager->TILBnperiod() - (nA1+nA3);

      thicknessAbsorber1 = nA1*thicknessPeriod;
      PosAbsor1 = thicknessAbsorber/2 - thicknessAbsorber1/2; 

      thicknessAbsorber2 = nA2*thicknessPeriod;
      PosAbsor2 = thicknessAbsorber/2 - thicknessAbsorber1 - thicknessAbsorber2/2; 

      thicknessAbsorber3 = nA3*thicknessPeriod;
      PosAbsor3 = thicknessAbsorber/2 - thicknessAbsorber1 - thicknessAbsorber2 - thicknessAbsorber3/2; 

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" Number of periods per Module: N= "<<nA1+nA2+nA3 
		 << " Middle absorber, numbers of periods = "<<nA2
		 << endmsg;

      // First Cut-out part
      absorber1 = new GeoTrd(thicknessAbsorber1/2, thicknessAbsorber1/2,
			    dy1Absorber, dy2Absorber,
			    heightAbsorber/2); 

      lvAbsorber1 = new GeoLogVol("Absorber",absorber1,matIron);
      pvAbsorber1 = new GeoPhysVol(lvAbsorber1);

      // absorber without Cut-out, middle part
      absorber = new GeoTrd(thicknessAbsorber2/2, thicknessAbsorber2/2,
			    dy1Absorber, dy2Absorber,
			    heightAbsorber/2); 

      lvAbsorber = new GeoLogVol("Absorber",absorber,matIron);
      pvAbsorber = new GeoPhysVol(lvAbsorber);

      //
      // Second Cut-out part
      absorber3 = new GeoTrd(thicknessAbsorber3/2, thicknessAbsorber3/2,
			    dy1Absorber, dy2Absorber,
			    heightAbsorber/2);

      lvAbsorber3 = new GeoLogVol("Absorber",absorber3,matIron);
      pvAbsorber3 = new GeoPhysVol(lvAbsorber3);
      //

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: Ex.Barrel pvAbsorber 1,3 Ok "<< endmsg;

      break;
    }
  default:
    { 
      absorber = new GeoTrd(thicknessAbsorber/2, thicknessAbsorber/2,
			    dy1Absorber, dy2Absorber,
			    heightAbsorber/2); 

      if (m_dbManager->TILBnperiod() > 1) {
          lvAbsorber = new GeoLogVol("Absorber",absorber,matIron);
      } else {
          // make C10special/Gap/Crack absorber volume from Air, Aluminium will be in period
          lvAbsorber = new GeoLogVol("Absorber",absorber,matAir);
      }
      pvAbsorber = new GeoPhysVol(lvAbsorber);
      
      if(m_log->level()<=MSG::DEBUG) {
          if (m_dbManager->TILBnperiod() > 1) {
              (*m_log) << MSG::DEBUG <<" _fillSection: default pvAbsorber  Ok "<< endmsg;
          } else {
              (*m_log) << MSG::DEBUG <<" _fillSection: special pvAbsorber made from Air  Ok "<< endmsg;
          }
      }
      
      break;
    }
  }
  //----- ------ ------- PERIODS ------ ------ ------ PERIODS ----- ------- -------
  double thicknessAbsorberChild;
  Variable periodInd;

  GeoTrd* period = 0;
  GeoLogVol* lvPeriod = 0;
  GeoPhysVol* pvPeriod = 0;
  GeoTransform* tfPeriod = 0;
  GeoSerialTransformer* stPeriod = 0;

  GeoTrd* absorberChild = 0;
  GeoLogVol* lvAbsorberChild = 0;
  GeoPhysVol* pvAbsorberChild = 0;
  GeoTransform* tfAbsorberChild = 0;

  // Perform different actions depending on sections
  switch (sec_number)
  {
  case 1:
    {
      //Barrel section
      //Divide absorber volume on two parts: first filled with
      //nrOfPeriods-1 ordinary period and second with one special period of type 2

      //First division
      thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;

      m_barrelPeriodThickness = thicknessPeriod;
      m_barrelGlue = dzglue*Gaudi::Units::cm;

      checking("Period 0", false, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2, 
			  dy1Absorber,
			  dy2Absorber,
			  heightAbsorber/2);

      lvPeriod = new GeoLogVol("Period",period,matIron);
      pvPeriod = new GeoPhysVol(lvPeriod);

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 1); // 1-period type


      thicknessAbsorberChild = thicknessPeriod*(m_dbManager->TILBnperiod()-1);
      absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				 thicknessAbsorberChild/2,
				 dy1Absorber,
				 dy2Absorber,
				 heightAbsorber/2);
      lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
      pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);

      // Place periods into Absorber Child like G4 replica
      GENFUNCTION periodPos1 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorberChild)/2;
      TRANSFUNCTION xfReplica1 = Pow(GeoTrf::TranslateX3D(1.),periodPos1);
      if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,m_dbManager->TILBnperiod()-1,
                                    (thicknessAbsorberChild - thicknessAbsorber)/2);

      stPeriod = new GeoSerialTransformer(pvPeriod,
					  &xfReplica1,
					  m_dbManager->TILBnperiod()-1);

      pvAbsorberChild->add(new GeoSerialIdentifier(0));
      pvAbsorberChild->add(stPeriod);

      // Place absorber child
      tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((thicknessAbsorberChild - thicknessAbsorber)/2,0.,0.));
      pvAbsorber->add(tfAbsorberChild);
      pvAbsorber->add(pvAbsorberChild);

      //Second division
      thicknessPeriod = (m_dbManager->TILBdzmast() + 2.*m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;

      checking("Period 1", false, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2,
			  dy1Absorber,
			  dy2Absorber,
			  heightAbsorber/2);
      lvPeriod = new GeoLogVol("Period",period,matIron);
      pvPeriod = new GeoPhysVol(lvPeriod);

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 2); // 2-period type


      thicknessAbsorberChild = thicknessPeriod;
      absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				 thicknessAbsorberChild/2,
				 dy1Absorber,
				 dy2Absorber,
				 heightAbsorber/2);
      lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
      pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);

      if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,1,
                                    (-thicknessAbsorberChild + thicknessAbsorber)/2);

      // Place period in the absorber child
      tfPeriod = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));

      pvAbsorberChild->add(new GeoIdentifierTag(m_dbManager->TILBnperiod()-1));
      pvAbsorberChild->add(tfPeriod);
      pvAbsorberChild->add(pvPeriod);

      // Place absorber child
      tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((-thicknessAbsorberChild + thicknessAbsorber)/2,0.,0.));
      pvAbsorber->add(tfAbsorberChild);
      pvAbsorber->add(pvAbsorberChild);

      break;
    }
  case 2:
    {
      //Extended barrel - consists of ordinary periods of type 1 only
      thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;

      checking("Period 2", false, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2,
			  dy1Absorber,
			  dy2Absorber,
			  heightAbsorber/2);
      lvPeriod = new GeoLogVol("Period",period,matIron);
      pvPeriod = new GeoPhysVol(lvPeriod);

      m_extendedPeriodThickness = thicknessPeriod;

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 1); // 1-period type

      // Place periods into Absorber like G4 replica
      //  - first partr of absorber
      //
      GENFUNCTION periodPos1 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorber1)/2;
      TRANSFUNCTION xfReplica1 = Pow(GeoTrf::TranslateX3D(1.),periodPos1);
      if (m_verbose) checktransfunc(thicknessAbsorber1,thicknessPeriod,nA1,
                                    (-thicknessAbsorber+thicknessAbsorber1)/2.);

      stPeriod = new GeoSerialTransformer(pvPeriod,&xfReplica1,nA1); //m_dbManager->TILBnperiod());

      pvAbsorber1->add(new GeoSerialIdentifier(0));
      pvAbsorber1->add(stPeriod);
      //

      if (m_dbManager->BoolCuts())
       { 
         if ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62))
          {
            // Cuting of (-)
            GeoCutVolAction action1(*CutA, TransCut2);
            pvAbsorber1->apply(&action1);
            pvTmp_Absorber1 = action1.getPV();
            //
	    if(m_log->level()<=MSG::DEBUG)
	      (*m_log) << MSG::DEBUG <<" _fillSection: CutA Ok "<< endmsg;
         } // end special modules
      } // end if, BoolCuts()
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: Absorber1 Ok "<< endmsg;

      // middle partr of absorber
      GENFUNCTION periodPos2 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorber2)/2;
      TRANSFUNCTION xfReplica2 = Pow(GeoTrf::TranslateX3D(1.),periodPos2);
      if (m_verbose) checktransfunc(thicknessAbsorber2,thicknessPeriod,nA2,
                                    (-thicknessAbsorber+thicknessAbsorber2)/2.+thicknessAbsorber1);

      stPeriod = new GeoSerialTransformer(pvPeriod,&xfReplica2,nA2); //m_dbManager->TILBnperiod());

      pvAbsorber->add(new GeoSerialIdentifier(nA1));
      pvAbsorber->add(stPeriod);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: pvAbsorber Ok "<< endmsg;

      // second partr of absorber
      //
      GENFUNCTION periodPos3 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorber3)/2;
      TRANSFUNCTION xfReplica3 = Pow(GeoTrf::TranslateX3D(1.),periodPos3);
      if (m_verbose) checktransfunc(thicknessAbsorber3,thicknessPeriod,nA3,
                                    (-thicknessAbsorber+thicknessAbsorber3)/2.+thicknessAbsorber2+thicknessAbsorber1);

      stPeriod = new GeoSerialTransformer(pvPeriod,&xfReplica3,nA3); //m_dbManager->TILBnperiod());

      pvAbsorber3->add(new GeoSerialIdentifier(nA1+nA2));
      pvAbsorber3->add(stPeriod);

        if (m_dbManager->BoolCuts())
	 {
	   if (ModuleNcp>=60 && ModuleNcp<=62) {
	     // Cuting of pvEBarrelModuleMotherPos (Left)
	     //
	     GeoCutVolAction action2(*CutB, TransCutL);
	     pvAbsorber3->apply(&action2);
	     pvTmp_Absorber3 = action2.getPV();
	     if(m_log->level()<=MSG::DEBUG)
	       (*m_log) << MSG::DEBUG <<" _fillSection: CutB L Ok "<< endmsg;
	   } else if (ModuleNcp>=35 && ModuleNcp<=37) { 
	     // Cuting of pvEBarrelModuleMotherPos (Right)
	     //
	     GeoCutVolAction action3(*CutB, TransCutR);
	     pvAbsorber3->apply(&action3);
	     pvTmp_Absorber3 = action3.getPV();
	     if(m_log->level()<=MSG::DEBUG)
	       (*m_log) << MSG::DEBUG <<" _fillSection: CutB R Ok "<< endmsg;
	   }
         } // end if, BoolCuts()
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG <<" _fillSection: Absorber3 Ok "<< endmsg;

      break;
    }
  case 3:
    {
      //Plug Section 1 - consists of ordinary periods of type 3
      // ps plug1 special module
      if ((m_dbManager->TILBsection() == 7)||(m_dbManager->TILBsection() == 8))
	{
	  //Divide absorber volume on two parts: first filled with
	  //nrOfPeriods-1 ordinary period of type 1 and second with one special period of type 4
	  
	  //First division
	  thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;
	  
	  checking("Period 3 (ITC1 special)", true, 4, 
		   thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);
	  
	  period = new GeoTrd(thicknessPeriod/2,
			      thicknessPeriod/2,
			      dy1Absorber,
			      dy2Absorber,
			      heightAbsorber/2);
	  lvPeriod = new GeoLogVol("Period",period,matIron);
	  pvPeriod = new GeoPhysVol(lvPeriod);
	  
 	  fillPeriod(pvPeriod,
 		     thicknessPeriod*(1./Gaudi::Units::cm),
 		     dzglue,
 		     tan_delta_phi_2,
 		     1); // 1-period type
	  
	  thicknessAbsorberChild = thicknessPeriod*(m_dbManager->TILBnperiod()-1);

	  absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				     thicknessAbsorberChild/2,
				     dy1Absorber,
				     dy2Absorber,
				     heightAbsorber/2);
	  lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
	  pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);
	  
	  // Place periods into Absorber Child like G4 replica
	  GENFUNCTION periodPosITC1sp = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorberChild)/2;
	  TRANSFUNCTION xfReplicaITC1sp = Pow(GeoTrf::TranslateX3D(1.),periodPosITC1sp);
          if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,m_dbManager->TILBnperiod()-1,
                                        (thicknessAbsorberChild - thicknessAbsorber)/2);
	  
	  stPeriod = new GeoSerialTransformer(pvPeriod,
					      &xfReplicaITC1sp,
					      m_dbManager->TILBnperiod()-1);
	  
	  pvAbsorberChild->add(new GeoSerialIdentifier(0));
	  pvAbsorberChild->add(stPeriod);
	  
	  // Place absorber child
	  tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((thicknessAbsorberChild - thicknessAbsorber)/2,0.,0.));
	  pvAbsorber->add(tfAbsorberChild);
	  pvAbsorber->add(pvAbsorberChild);
	  //
	  //Second division
	  //
	  thicknessPeriod = m_dbManager->TILBdzspac()*Gaudi::Units::cm;
	  
	  checking("Period 5 (ITC1 special)", true, 4, 
		   thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);
	  
	  period = new GeoTrd(thicknessPeriod/2,
			      thicknessPeriod/2,
			      dy1Absorber,
			      dy2Absorber,
			      heightAbsorber/2);
	  lvPeriod = new GeoLogVol("Period",period,matIron);
	  pvPeriod = new GeoPhysVol(lvPeriod);
	  
	  fillPeriod(pvPeriod,
		     thicknessPeriod*(1./Gaudi::Units::cm),
		     dzglue,
		     tan_delta_phi_2,
		     4); // 4-period type
	  
	  thicknessAbsorberChild = thicknessPeriod;
	  absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				     thicknessAbsorberChild/2,
				     dy1Absorber,
				     dy2Absorber,
				     heightAbsorber/2);
	  lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
	  pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);
	  
          if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,1,
                                        (-thicknessAbsorberChild + thicknessAbsorber)/2);

	  // Place period in the absorber child
	  tfPeriod = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	  pvAbsorberChild->add(new GeoIdentifierTag(m_dbManager->TILBnperiod()-1));
	  pvAbsorberChild->add(tfPeriod);
	  pvAbsorberChild->add(pvPeriod);
	  
	  // Place absorber child
	  tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((-thicknessAbsorberChild + thicknessAbsorber)/2,0.,0.));
	  pvAbsorber->add(tfAbsorberChild);
	  pvAbsorber->add(pvAbsorberChild);
	  
	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<" _fillSection: Absorber (ITC plug special) Ok "<< endmsg;
	}
      else
	{
	  thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;
	  
	  checking("Period 3", true, 4, 
		   thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);
	  
	  period = new GeoTrd(thicknessPeriod/2,
			      thicknessPeriod/2,
			      dy1Absorber,
			      dy2Absorber,
			      heightAbsorber/2);
	  lvPeriod = new GeoLogVol("Period",period,matIron);
	  pvPeriod = new GeoPhysVol(lvPeriod);
	  
	  fillPeriod(pvPeriod,
		     thicknessPeriod*(1./Gaudi::Units::cm),
		     dzglue,
		     tan_delta_phi_2,
		     3); // 3-period type
	  
	  // Place periods into Absorber like G4 replica
	  GENFUNCTION periodPos3 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorber)/2;
	  TRANSFUNCTION xfReplica3 = Pow(GeoTrf::TranslateX3D(1.),periodPos3);
          if (m_verbose) checktransfunc(thicknessAbsorber,thicknessPeriod,m_dbManager->TILBnperiod(),0.0);
	  
	  //ps      if( (m_dbManager->TILBsection()==7 || m_dbManager->TILBsection()==8) && m_dbManager->SCNTitem()==302) 
	  //	  if( m_dbManager->TILBsection()==7  && m_dbManager->SCNTitem() == 302) 
	  //	    NbPeriod = m_dbManager->TILBnperiod() - 1;
	  //	  else
	    NbPeriod = m_dbManager->TILBnperiod();

	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<" SCNTitem= "<<m_dbManager->SCNTitem()<<"    NbPeriod= "<<NbPeriod<< endmsg;
	  stPeriod = new GeoSerialTransformer(pvPeriod,
					      &xfReplica3, 
					      NbPeriod);//sbb
	  
	  pvAbsorber->add(new GeoSerialIdentifier(0));
	  pvAbsorber->add(stPeriod);
	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<" _fillSection: Absorber (case 3) Ok "<< endmsg;
	}
      break;
    }
  case 4:
    {
      //Plug Section 2
      //Divide absorber volume on two parts: first filled with
      //nrOfPeriods-1 ordinary period of type 1 and second with one special period of type 4

      //First division
      thicknessPeriod = 2.*(m_dbManager->TILBdzmast() + m_dbManager->TILBdzspac() + 2.*dzglue)*Gaudi::Units::cm;

      checking("Period 4", true, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2,
			  dy1Absorber,
			  dy2Absorber,
			  heightAbsorber/2);
      lvPeriod = new GeoLogVol("Period",period,matIron);
      pvPeriod = new GeoPhysVol(lvPeriod);

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 1); // 1-period type


      thicknessAbsorberChild = thicknessPeriod*(m_dbManager->TILBnperiod()-1);
      absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				 thicknessAbsorberChild/2,
				 dy1Absorber,
				 dy2Absorber,
				 heightAbsorber/2);
      lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
      pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);

      // Place periods into Absorber Child like G4 replica
      GENFUNCTION periodPos1 = (thicknessPeriod*(2*periodInd+1)-thicknessAbsorberChild)/2;
      TRANSFUNCTION xfReplica1 = Pow(GeoTrf::TranslateX3D(1.),periodPos1);
      if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,m_dbManager->TILBnperiod()-1,
                                    (thicknessAbsorberChild - thicknessAbsorber)/2);

      stPeriod = new GeoSerialTransformer(pvPeriod,
					  &xfReplica1,
					  m_dbManager->TILBnperiod()-1);

      pvAbsorberChild->add(new GeoSerialIdentifier(0));
      pvAbsorberChild->add(stPeriod);

      // Place absorber child
      tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((thicknessAbsorberChild - thicknessAbsorber)/2,0.,0.));
      pvAbsorber->add(tfAbsorberChild);
      pvAbsorber->add(pvAbsorberChild);

      //Second division
      thicknessPeriod = m_dbManager->TILBdzspac()*Gaudi::Units::cm;

      checking("Period 5", true, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2,
			  dy1Absorber,
			  dy2Absorber,
			  heightAbsorber/2);
      lvPeriod = new GeoLogVol("Period",period,matIron);
      pvPeriod = new GeoPhysVol(lvPeriod);

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 4); // 4-period type

      thicknessAbsorberChild = thicknessPeriod;
      absorberChild = new GeoTrd(thicknessAbsorberChild/2,
				 thicknessAbsorberChild/2,
				 dy1Absorber,
				 dy2Absorber,
				 heightAbsorber/2);
      lvAbsorberChild = new GeoLogVol("AbsorberChild",absorberChild,matAir);
      pvAbsorberChild = new GeoPhysVol(lvAbsorberChild);

      if (m_verbose) checktransfunc(thicknessAbsorberChild,thicknessPeriod,1,
                                    (-thicknessAbsorberChild + thicknessAbsorber)/2);

      // Place period in the absorber child
      tfPeriod = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
      pvAbsorberChild->add(new GeoIdentifierTag(m_dbManager->TILBnperiod()-1));
      pvAbsorberChild->add(tfPeriod);
      pvAbsorberChild->add(pvPeriod);

      // Place absorber child
      tfAbsorberChild = new GeoTransform(GeoTrf::Translate3D((-thicknessAbsorberChild + thicknessAbsorber)/2,0.,0.));
      pvAbsorber->add(tfAbsorberChild);
      pvAbsorber->add(pvAbsorberChild);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: Absorber (case 4) Ok "<< endmsg;
      break;
    }
  default:
    {
      // C10_missing/Gap/Crack 5,6,14,15,16

      if ((m_dbManager->TILBsection()==5)||(m_dbManager->TILBsection()==6)) //Gap/Crack
	m_dbManager->SetCurrentScin(100*m_dbManager->TILBsection() + 1);
      else if ((m_dbManager->TILBsection()==14) || (m_dbManager->TILBsection()==15) || (m_dbManager->TILBsection()==16)) 
	m_dbManager->SetCurrentScin(m_dbManager->TILBcurscint());
      else 
      {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::fillSection: Unexpected section = "
                 << m_dbManager->TILBsection() << " for Gap/Crack" << endmsg;
	return;
      }

      thicknessPeriod = thicknessAbsorber; // the same as absorner, but made from Aluminium

      checking("Period 6", true, 4, 
	       thicknessPeriod/2,thicknessPeriod/2,dy1Absorber,dy2Absorber,heightAbsorber/2);

      double dy1Period = m_dbManager->TILBflangex()/2.*Gaudi::Units::cm; // correct size from the drawings
      double dy2Period = m_dbManager->TILBflangey()/2.*Gaudi::Units::cm; // correct size from the drawings
      if (dy1Period <= 0.0 || dy2Period <= 0.0 || dy1Period > dy1Absorber || dy2Period > dy2Absorber || dy1Period >= dy2Period ) {
          dy1Period = dy1Absorber;
          dy2Period = dy2Absorber;
      }

      period = new GeoTrd(thicknessPeriod/2,
			  thicknessPeriod/2,
			  dy1Period,
			  dy2Period,
			  heightAbsorber/2);
      if (!matAluminium) matAluminium = m_theMaterialManager->getMaterial("std::Aluminium");
      lvPeriod = new GeoLogVol("Period",period,matAluminium); // note - aluminium period inside air absorber here
      pvPeriod = new GeoPhysVol(lvPeriod);

      fillPeriod(pvPeriod,
                 thicknessPeriod*(1./Gaudi::Units::cm),
                 dzglue,
                 tan_delta_phi_2,
                 5, period);

      if (m_verbose) checktransfunc(thicknessAbsorber,thicknessPeriod,1,0.0);

      // Place period in the absorber
      tfPeriod = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
      pvAbsorber->add(new GeoIdentifierTag(0));
      pvAbsorber->add(tfPeriod);
      pvAbsorber->add(pvPeriod);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" _fillSection: Absorber (case default) Ok "<< endmsg;
      break;
    }
  }

  // Place absorber in the module mother
  GeoTransform *tfAbsorber{nullptr}, *tfAbsorber1{nullptr}, *tfAbsorber3{nullptr};

  double dXAbsorber = (m_dbManager->TILBdzend1() - m_dbManager->TILBdzend2());
  double dZAbsorber = (m_dbManager->TILBrmax() - tile_rmax);

  if (sec_number==3)
    { 
      // ps specialModuleZShift
      tfAbsorber = new GeoTransform(GeoTrf::Translate3D( specialModuleZShift + dXAbsorber*Gaudi::Units::cm/2, 0., dZAbsorber*Gaudi::Units::cm/2));
      mother->add(tfAbsorber);
      mother->add(pvAbsorber);
    }
  else if (sec_number==2)
    { 
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << " _fillsection  Ex.barrel in "<< endmsg;
 
     tfAbsorber1 = new GeoTransform(GeoTrf::Translate3D(dXAbsorber*Gaudi::Units::cm/2 - PosAbsor1, 0.,
                                   (dZAbsorber + m_dbManager->TILBrmin() - rminb)*Gaudi::Units::cm/2)); 
     mother->add(tfAbsorber1);
     if (m_dbManager->BoolCuts() && ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62)) ) { 
       mother->add(pvTmp_Absorber1);
       pvAbsorber1->ref(); pvAbsorber1->unref();
     } else {
       mother->add(pvAbsorber1);
     }

     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG << " _fillsection  ext.barrel pvAbsorber1 Ok"<< endmsg;

     tfAbsorber  = new GeoTransform(GeoTrf::Translate3D(dXAbsorber*Gaudi::Units::cm/2 - PosAbsor2, 0.,
                                    (dZAbsorber + m_dbManager->TILBrmin() - rminb)*Gaudi::Units::cm/2));  
     mother->add(tfAbsorber);
     mother->add(pvAbsorber);

     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG << " _fillsection  ext.barrel pvAbsorber Ok"<< endmsg;

     tfAbsorber3 = new GeoTransform(GeoTrf::Translate3D(dXAbsorber*Gaudi::Units::cm/2 - PosAbsor3, 0.,  
                                    (dZAbsorber + m_dbManager->TILBrmin() - rminb)*Gaudi::Units::cm/2));                
     mother->add(tfAbsorber3);
     if (m_dbManager->BoolCuts() && ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62)) ) {
       mother->add(pvTmp_Absorber3);
       pvAbsorber3->ref(); pvAbsorber3->unref();
     } else {
       mother->add(pvAbsorber3);
     }

     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG << " _fillsection  ext.barrel pvAbsorber3 Ok"<< endmsg;
   }
  else                                           
   { 
     tfAbsorber = new GeoTransform(GeoTrf::Translate3D(dXAbsorber*Gaudi::Units::cm/2, 0.,
                                   (dZAbsorber + m_dbManager->TILBrmin() - rminb)*Gaudi::Units::cm/2));
     mother->add(tfAbsorber);
     mother->add(pvAbsorber);
     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG << " _fillsection other pvAbsorber Ok"<< endmsg;
   }

  if(CutA) {
    CutA->ref();
    CutA->unref();
  }
  if(CutB) {
    CutB->ref();
    CutB->unref();
  }

}


void TileGeoSectionBuilder::fillGirder(GeoPhysVol*&             mother,
				       double                   tile_rmax,
				       double                   tilb_rmax,
				       double                   tan_delta_phi_2,
				       double                   thickness)
{
  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  // Obtain required materials - Iron, Aluminium and electronic boards

  const GeoMaterial* matIron = m_theMaterialManager->getMaterial("std::Iron");
  const GeoMaterial* matAluminium = m_theMaterialManager->getMaterial("std::Aluminium");
  const GeoMaterial* matElBoard = m_theMaterialManager->getMaterial("tile::SiO2CondEpox");

  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

  GeoTrd* girderElement = 0;
  GeoLogVol* lvGirderElement = 0;
  GeoPhysVol* pvGirderElement = 0;
  GeoTransform* tfGirderElement = 0;

  int CurrentGird = 1;
  int j;

  //if (m_dbManager->TILBsection()==8)
  //  CurrentGird = 801;                  //Prototype module 
  // ps  commented out  old geometry
  
  for (j = CurrentGird; j < (CurrentGird + m_dbManager->TILBngirder()); j++)
  {
    m_dbManager->SetCurrentGird(j);
    double elementRC = m_dbManager->TIGRrc();
    double elementSizeInR = m_dbManager->TIGRdr();
    double elementSizeInY = m_dbManager->TIGRdw();
    double elementOffsetInY = m_dbManager->TIGRoff();

    double dy1GirderElement, dy2GirderElement;
    if (elementSizeInY >0)
    {
      dy1GirderElement = elementSizeInY/2;
      dy2GirderElement = elementSizeInY/2;
    }
    else
    {
      dy1GirderElement = (elementRC - elementSizeInR/2) * tan_delta_phi_2;
      dy2GirderElement = (elementRC + elementSizeInR/2) * tan_delta_phi_2;
    }

    girderElement = new GeoTrd(thickness/2*Gaudi::Units::cm,
			       thickness/2*Gaudi::Units::cm,
			       dy1GirderElement*Gaudi::Units::cm,
			       dy2GirderElement*Gaudi::Units::cm,
			       elementSizeInR/2*Gaudi::Units::cm);

    switch(m_dbManager->TIGRmaterial())
    {
    case 1:                    //Iron
      {
	lvGirderElement = new GeoLogVol("GirderIron",girderElement,matIron);
        break;
      }
    case 2:                    //Aluminium
      {
	lvGirderElement = new GeoLogVol("GirderAluminium",girderElement,matAluminium);
        break;
      }
    case 3:                    //Electronics
      {
	lvGirderElement = new GeoLogVol("GirderElectronics",girderElement,matElBoard);
        break;
      }
    default:
      {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::FillGirder: unknown material = "
                 << m_dbManager->TIGRmaterial() << endmsg;
	return;
      }
    }

    pvGirderElement = new GeoPhysVol(lvGirderElement);
    tfGirderElement = new GeoTransform(GeoTrf::Translate3D(0.,
						      elementOffsetInY*Gaudi::Units::cm,
						      (elementRC-(tilb_rmax + tile_rmax)/2)*Gaudi::Units::cm));
    mother->add(tfGirderElement);
    mother->add(pvGirderElement);
  }
}

void TileGeoSectionBuilder::fillFinger(GeoPhysVol*&             mother,
				       int                      sec_number,
				       double                   tile_rmax,
				       double                   tilb_rmax,
				       double                   /* delta_phi */,
				       bool                     testbeam,
				       int                      ModuleNcp,
				       double                   corrected_dz)
{
  bool boolNeg = false;

  if (ModuleNcp > 99)
   { ModuleNcp = ModuleNcp/100;
     boolNeg = true;
   }

  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  // Obtain required materials - Iron, Aluminium and electronic boards

  const GeoMaterial* matIron = m_theMaterialManager->getMaterial("std::Iron");
  const GeoMaterial* matAluminium = m_theMaterialManager->getMaterial("std::Aluminium");
  const GeoMaterial* matElBoard = m_theMaterialManager->getMaterial("tile::SiO2CondEpox");

  // Get required elements
  //const GeoElement* oxygen = m_theMaterialManager->getElement("Oxygen");
  //const GeoElement* hydrogen = m_theMaterialManager->getElement("Hydrogen");
  const GeoElement* copper = m_theMaterialManager->getElement("Copper");

  // Get some standard materials
  const GeoMaterial *air        = m_theMaterialManager->getMaterial("std::Air");
  const GeoMaterial *iron       = m_theMaterialManager->getMaterial("std::Iron");
  const GeoMaterial *shieldSteel = m_theMaterialManager->getMaterial("shield::ShieldSteel");
  const GeoMaterial *matRubber = m_theMaterialManager->getMaterial("sct::Rubber");

  // InDetServices
  if (m_matLArServices == 0)
   { m_matLArServices = new GeoMaterial("LArServices", 2.5*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
     m_matLArServices->add(shieldSteel, 0.20);
     m_matLArServices->add(copper, 0.60);
     m_matLArServices->add(matRubber, 0.10);
     m_matLArServices->add(air, 0.10);
     m_matLArServices->lock();
   }

  // m_matIronHalfDens
  if (m_matIronHalfDens == 0)
   { m_matIronHalfDens = new GeoMaterial("LArIronBox", 4.5*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
     m_matIronHalfDens->add(shieldSteel, 0.80);
     m_matIronHalfDens->add(matRubber, 0.10);
     m_matIronHalfDens->add(copper, 0.10);
     m_matIronHalfDens->lock();
   }
 
  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

  GeoTrd* fingerElementTrd = 0;
  GeoTrap* fingerElementTrap = 0;

  GeoBox *fingerCablesL{nullptr}, *fingerCablesR{nullptr};
  GeoPhysVol *pvFingerElement = 0, *pvFingerCablesL{nullptr}, *pvFingerCablesR{nullptr};
  GeoLogVol *lvFingerElement = 0, *lvFingerCablesL{nullptr}, *lvFingerCablesR{nullptr}; 
  GeoTransform* tfFingerElement = 0, *tfFingerCables{nullptr};

  GeoTransform* ZrotateMod = 0;
  GeoTransform* yrotateMod = 0;
  GeoTransform* zrotateMod = 0;

  const GeoMaterial *currentMaterial{nullptr}, *leftMaterial{nullptr}, *rightMaterial{nullptr};
  std::string currentName, leftName, rightName;

  int CurrentTicg = 100*sec_number + 1;
  int j;
  double AirVolumeShift = 0.0;
  double AirVolumeSize = corrected_dz;

  for (j = CurrentTicg; j < (CurrentTicg + m_dbManager->TIFGnelem()); j++)
  {
    m_dbManager->SetCurrentTicg(j);
    double elementRC = m_dbManager->TICGrc();
    double elementHeight = m_dbManager->TICGdr();
    double elementDy1 = m_dbManager->TICGdx1();
    double elementDy2 = m_dbManager->TICGdx2();
    double elementOffset = m_dbManager->TICGoff();
    double elementDz = m_dbManager->TICGdz();
    double elementZPozition = m_dbManager->TICGzc();

    if(corrected_dz > 0.) {
      if (j-CurrentTicg==0) { // take air volume size from first element
        AirVolumeShift = (elementDz-corrected_dz)/2 - elementZPozition;
        AirVolumeSize = elementDz;
	if(m_log->level()<=MSG::DEBUG) {
	  (*m_log) << MSG::DEBUG << "TileFinger: OldAirVolumeCenter ="<<elementZPozition << endmsg;
	  (*m_log) << MSG::DEBUG << "TileFinger: NewAirVolumeCenter ="<<elementZPozition+AirVolumeShift << endmsg;
	  (*m_log) << MSG::DEBUG << "TileFinger: AirVolumeShift ="<< AirVolumeShift << endmsg;
	  (*m_log) << MSG::DEBUG << "TileFinger: AirVolumeSize ="<< AirVolumeSize << endmsg;
	}
      }
      if (elementZPozition*2-AirVolumeSize<-0.01) { // compare with zero with 0.1 Gaudi::Units::mm precision
        elementZPozition += AirVolumeShift; // shift all volumes keeping size
      } else { // resize finger cover with shims attached to it
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << "TileFinger: OldCoverThickness ="<<elementDz << endmsg;
        elementDz = corrected_dz - AirVolumeSize;
	if(m_log->level()<=MSG::DEBUG) {
	  (*m_log) << MSG::DEBUG << "TileFinger: NewCoverThickness ="<<elementDz << endmsg;
	  (*m_log) << MSG::DEBUG << "TileFinger: OldCoverCenter ="<<elementZPozition << endmsg;
	}
        elementZPozition = (corrected_dz-elementDz)/2;
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << "TileFinger: NewCoverCenter ="<<elementZPozition << endmsg;
      }
    }
    
    switch(m_dbManager->TICGmaterial())
    {
    case 1:
      {
	currentMaterial = matIron;
	currentName = "FingerIron";
	break;
      }
    case 2:
      {
	currentMaterial = matAluminium;
	currentName = "FingerAluminum";
	break;
      }
    case 3:
      {
	currentMaterial = matElBoard;
	currentName = "FingerElectronics";
	break;
      }
    default:
      {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::fillFinger: unexpected material = "
                 << m_dbManager->TICGmaterial() << " for finger element" << endmsg;
	return;
      }
    }

    if(m_dbManager->TICGshape()==1)
    {
      fingerElementTrd = new GeoTrd(elementDz/2*Gaudi::Units::cm,
				    elementDz/2*Gaudi::Units::cm,
				    elementDy1/2*Gaudi::Units::cm,
				    elementDy2/2*Gaudi::Units::cm,
				    elementHeight/2*Gaudi::Units::cm);
      lvFingerElement = new GeoLogVol(currentName,fingerElementTrd,currentMaterial);
    }
    else if(m_dbManager->TICGshape()==2)
    {
 
      fingerElementTrap = new GeoTrap(elementDz/2*Gaudi::Units::cm,
				      0.,
				      0.,
				      elementHeight/2*Gaudi::Units::cm,
				      elementDy2/2*Gaudi::Units::cm,
				      elementDy1/2*Gaudi::Units::cm,
				      atan((elementDy1-elementDy2)/(2.*elementHeight)),
				      elementHeight/2*Gaudi::Units::cm,
				      elementDy2/2*Gaudi::Units::cm,
				      elementDy1/2*Gaudi::Units::cm,
				      atan((elementDy1-elementDy2)/(2.*elementHeight)));
 
      lvFingerElement = new GeoLogVol(currentName,fingerElementTrap,currentMaterial);
    }    
    else
    {
      (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::fillFinger: unexpected shape = "
               << m_dbManager->TICGshape() << " for finger element" << endmsg;
      return;
    }


    pvFingerElement = new GeoPhysVol(lvFingerElement);
    tfFingerElement = new GeoTransform(GeoTrf::Translate3D(elementZPozition*Gaudi::Units::cm,
						      elementOffset*Gaudi::Units::cm,
						      (elementRC-(tilb_rmax + tile_rmax)/2)*Gaudi::Units::cm));

    mother->add(tfFingerElement);
    if (m_dbManager->TICGshape()==2)
    {
      if(elementOffset<0)
      {
	ZrotateMod = new GeoTransform(GeoTrf::RotateZ3D(180*Gaudi::Units::deg));
	mother->add(ZrotateMod); 
      }

      zrotateMod = new GeoTransform(GeoTrf::RotateZ3D(90*Gaudi::Units::deg));
      yrotateMod = new GeoTransform(GeoTrf::RotateY3D(-90*Gaudi::Units::deg));
      mother->add(yrotateMod);
      mother->add(zrotateMod);
    }

    mother->add(new GeoIdentifierTag(ModuleNcp));
    mother->add(pvFingerElement);
  }

  if (testbeam) return; // no cables between fingers at the testbeam

  // Cables space

  int Isector =0, LRflag =0;

  if ((ModuleNcp/4 - (float)ModuleNcp/4) != 0) Isector = (int)ModuleNcp/4;
  else 
   {Isector = (int)ModuleNcp/4-1;
   }
  LRflag = ModuleNcp-4*Isector;
  
  m_dbManager->SetCurrentTicg(CurrentTicg);

  double elementRC = m_dbManager->TICGrc();
  double elementOffset = m_dbManager->TICGoff();
  double elementDz = m_dbManager->TICGdz();
  double elementZPozition = m_dbManager->TICGzc();

  double dXleft =35., dXright = 35.;
  double dZleft = elementDz/2, dZright = elementDz/2;
  double dZsaddleL = elementDz/4, dZsaddleR = elementDz/4;
  double dY = 7.5;

  rightMaterial = leftMaterial = m_matLArServices;
  rightName = leftName = "LArCables";

  if (LRflag == 1)
    { dZright = elementDz, dXright = 35;
      dZsaddleR = 0;
      rightMaterial = m_matIronHalfDens;
      rightName = "LArService";

    } else if (LRflag == 4) 
    { dZleft = elementDz, dXleft = 35;
      dZsaddleL = 0;
      leftMaterial = m_matIronHalfDens;
      leftName = "LArService";
    }

  if (sec_number == 1)
   { if ( (ModuleNcp >=40 && ModuleNcp <=41) || (ModuleNcp >=56 && ModuleNcp <=57) ) 
     { dZright = dZleft = 8.5;
       dZsaddleL = dZsaddleR = 13.7;
       rightMaterial = leftMaterial = iron;
       rightName = leftName = "BarrelSaddleSupport";
     }
   } else 
   { if ( (ModuleNcp >=39 && ModuleNcp <=42) || (ModuleNcp >=55 && ModuleNcp <=58) ) 
     { dZright = dZleft = 8.5;
       dZsaddleR = dZsaddleL = 13.7;
       rightMaterial = leftMaterial = iron;
       rightName = leftName = "ExtBarrelSaddleSupport";
     }
   } 

  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << " Finger : number= "<<sec_number<<" ModuleNcp = " <<ModuleNcp<< " Sector= "<< Isector
	     << " LRflag= " << LRflag <<" Neg "<< boolNeg 
	     << endmsg;

  GeoTransform *rotateY = new GeoTransform(GeoTrf::RotateY3D(90*Gaudi::Units::deg));
  GeoTransform *rotateZ = new GeoTransform(GeoTrf::RotateZ3D(3*Gaudi::Units::deg));
  GeoTransform *rotateZm = new GeoTransform(GeoTrf::RotateZ3D(-3*Gaudi::Units::deg));

  // Left (+phi)
  fingerCablesL = new GeoBox(dXleft/2*Gaudi::Units::cm, dY/2*Gaudi::Units::cm, dZleft/2*Gaudi::Units::cm);
  lvFingerCablesL = new GeoLogVol(leftName,fingerCablesL,leftMaterial);
  pvFingerCablesL = new GeoPhysVol(lvFingerCablesL);

  // Right (-phi)
  fingerCablesR = new GeoBox(dXright/2*Gaudi::Units::cm, dY/2*Gaudi::Units::cm, dZright/2*Gaudi::Units::cm);
  lvFingerCablesR = new GeoLogVol(rightName,fingerCablesR,rightMaterial);
  pvFingerCablesR = new GeoPhysVol(lvFingerCablesR);

  double YpoFinger =0;

  if (boolNeg)
    { YpoFinger = -elementOffset+5.4; 
    } else
    { YpoFinger = elementOffset-5.4;  
    }

  tfFingerCables = new GeoTransform(GeoTrf::Translate3D(elementZPozition*Gaudi::Units::cm +0.5*Gaudi::Units::cm -dZsaddleL*Gaudi::Units::cm,
	                                           YpoFinger*Gaudi::Units::cm,
				                   (elementRC-(tilb_rmax + tile_rmax)/2)*Gaudi::Units::cm));
  mother->add(tfFingerCables);

  // inversion for negativ fingers, Left
  if (boolNeg)
    { mother->add(rotateY);
      mother->add(rotateZm);
     } else
    { mother->add(rotateY);
      mother->add(rotateZ);
    }

  mother->add(new GeoIdentifierTag(ModuleNcp));
  mother->add(pvFingerCablesL); 

  if (boolNeg)
    { YpoFinger = elementOffset-5.4;
    } else
    { YpoFinger = -elementOffset+5.4;
    }

  tfFingerCables = new GeoTransform(GeoTrf::Translate3D(elementZPozition*Gaudi::Units::cm +0.5*Gaudi::Units::cm -dZsaddleR*Gaudi::Units::cm,
	                                           YpoFinger*Gaudi::Units::cm,
	                                           (elementRC-(tilb_rmax + tile_rmax)/2)*Gaudi::Units::cm));
  mother->add(tfFingerCables);

  // inversion for negativ fingers, Right
  if (boolNeg)
    { mother->add(rotateY);
      mother->add(rotateZ);
     } else
    { mother->add(rotateY);
      mother->add(rotateZm);
    }

  mother->add(new GeoIdentifierTag(ModuleNcp));
  mother->add(pvFingerCablesR);
  // m_matLArServices->unref();
  // m_matIronHalfDens->unref(); 
}

void TileGeoSectionBuilder::fillPeriod(GeoPhysVol*&              mother,
				       double                    thickness,
				       double                    dzglue,
				       double                    tan_delta_phi_2,
				       int                       period_type,
                                       GeoTrd *                  period)
{
  int j;
  int CurrentScin = 0;
  const GeoShape* glue = 0;
  GeoLogVol* lvGlue = 0;
  GeoPhysVol* pvGlue = 0;
  GeoTransform* tfGlue = 0;

  double scintiWrapInZ, scintiWrapInR, scintiThickness, scintiDeltaInPhi;
  double scintiHeight, scintiRC, scintiZPos, dy1Scintillator, dy2Scintillator;
  const GeoShape* scintillator = 0;
  GeoLogVol* lvScintillator = 0;
  GeoPhysVol* pvScintillator = 0;
  GeoTransform* tfScintillator = 0;

  double thicknessWrapper, heightWrapper, dy1Wrapper, dy2Wrapper;
  const GeoShape* wrapper = 0;
  GeoLogVol* lvWrapper = 0;
  GeoPhysVol* pvWrapper = 0;
  GeoTransform* tfWrapper = 0;

  GeoIdentifierTag* idTag = 0;

  (*m_log) << MSG::VERBOSE <<" TileGeoSectionBuilder::fillPeriod"<< endmsg;

  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

  // Determine the number of first scintillator depending on the section type
  if (m_dbManager->TILBsection() >= 7 && m_dbManager->TILBcurscint() == 0)
  {
    CurrentScin = 101;    //special modules for testbeam identical to normal barrel module
  }
  else 
  {
    if (m_dbManager->TILBcurscint()>0)
    {
      CurrentScin = m_dbManager->TILBcurscint();
    }
    else
    {
      CurrentScin = 100*m_dbManager->TILBsection()+1;
    }
  }

  // special EB modules with cut-outs
  if (m_dbManager->TILBsection()>=26 && m_dbManager->TILBsection()<=38 && m_dbManager->TILBcurscint() == 0) 
  { //ps shortcut. Should be read from DB later on.
    CurrentScin = 100*2 + 1;
  }

  int zpShift = 1;
  // special EB modules with small D4 cell
  if (m_dbManager->TILBsection()>=7 && m_dbManager->TILBsection()<=8 && m_dbManager->TILBcurscint() == 301) 
  { //ps fix. Should put curscint = 1301 in DB and zpShift will not be needed
    zpShift = -1;
  }
  
  (*m_log) << MSG::VERBOSE <<"     TILBsection= "<<m_dbManager->TILBsection()<<endmsg;
  (*m_log) << MSG::VERBOSE <<"     CurrentScin= "<<CurrentScin<<" TILBcurscint= "<<m_dbManager->TILBcurscint() 
                           << " - " << m_dbManager->TILBcurscint()+m_dbManager->TILBnscin()-1 <<endmsg;
  (*m_log) << MSG::VERBOSE <<"     period_type= "<<period_type <<endmsg;

  // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  // Obtain required materials - Air, Glue, Scintillator

  const GeoMaterial* matAir = m_theMaterialManager->getMaterial("std::Air");
  const GeoMaterial* matScin = m_theMaterialManager->getMaterial("tile::Scintillator");

  //Cs hole parameters
  double csHoleR       = 0.45 * Gaudi::Units::cm;
  double csTubeOuterR  = 0.4  * Gaudi::Units::cm;
  double csTubeInnerR  = 0.3  * Gaudi::Units::cm;
  double csTubeOffCorr = 1.35 * Gaudi::Units::cm;

  double thicknessMother2 = thickness/2.*Gaudi::Units::cm;
  double heightMother2    = (m_dbManager->TILBrmax() - m_dbManager->TILBrmin())*Gaudi::Units::cm/2.;

  const bool removeGlue = (m_glue == 0 || m_glue == 2);

  //Glue layer
  if (dzglue>0.0 && period_type<4 && !removeGlue) {
    const GeoMaterial* matGlue = m_theMaterialManager->getMaterial("tile::Glue");

    double dzglue2 = dzglue/2*Gaudi::Units::cm;
    dzglue2 = floor(dzglue2*1.0e+10)*1.0e-10;

    if (m_verbose) {
      printdouble(" period thickness/2 = ",thickness/2*Gaudi::Units::cm);
      printdouble("   glue thickness/2 = ",dzglue/2*Gaudi::Units::cm);
      printdouble("rounded thickness/2 = ",dzglue2);
    }

    double dy1Glue = (m_dbManager->TILBrmin() * tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
    double dy2Glue = (m_dbManager->TILBrmax() * tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
    double heightGlue2 = (m_dbManager->TILBrmax() - m_dbManager->TILBrmin())*Gaudi::Units::cm/2.;

    checking("Glue 0", false, 4, 
             dzglue2,dzglue2,dy1Glue,dy2Glue,heightGlue2);

    glue = new GeoTrd(dzglue2,dzglue2,dy1Glue,dy2Glue,heightGlue2);

  //Cs tubes in mother volume and holes in glue
  if (m_csTube) {
    for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
    {
      idTag = new GeoIdentifierTag(j-CurrentScin);
      m_dbManager->SetCurrentScin(j);

      double off0 = m_dbManager->SCNTrc()*Gaudi::Units::cm - heightMother2;
      double off  = m_dbManager->SCNTdr()/2.*Gaudi::Units::cm - csTubeOffCorr;

      GeoTrf::Transform3D tfHole1 = GeoTrf::Translate3D(0.,0.,(off0-off)) * GeoTrf::RotateY3D(-90*Gaudi::Units::deg);
      GeoTrf::Transform3D tfHole2 = GeoTrf::Translate3D(0.,0.,(off0+off)) * GeoTrf::RotateY3D(-90*Gaudi::Units::deg);

      // air around iron rod, around Cs tube and inside Cs tube
      GeoShape *air1 = new GeoTubs(csTubeOuterR, csHoleR, thicknessMother2, 0.,360.0 * Gaudi::Units::deg);
      GeoShape *air2 = new GeoTubs(csTubeOuterR, csHoleR, thicknessMother2, 0.,360.0 * Gaudi::Units::deg);
      GeoShape *air3 = new GeoTubs(0.,      csTubeInnerR, thicknessMother2, 0.,360.0 * Gaudi::Units::deg);

      GeoLogVol * lvAir1 = new GeoLogVol("CsTubeAir1",air1,matAir);
      GeoLogVol * lvAir2 = new GeoLogVol("CsTubeAir2",air2,matAir);
      GeoLogVol * lvAir3 = new GeoLogVol("CsTubeAir3",air3,matAir);
      GeoPhysVol * pvAir1 = new GeoPhysVol(lvAir1);
      GeoPhysVol * pvAir2 = new GeoPhysVol(lvAir2);
      GeoPhysVol * pvAir3 = new GeoPhysVol(lvAir3);

      GeoTransform* tftube1 = new GeoTransform(tfHole1);
      GeoTransform* tftube2 = new GeoTransform(tfHole2);
      GeoTransform* tftube3 = new GeoTransform(tfHole2);

      mother->add(tftube1);
      mother->add(pvAir1);
      mother->add(tftube2);
      mother->add(pvAir2);
      mother->add(tftube3);
      mother->add(pvAir3);

      //Holes in Glue
      if (glue) {
        glue = makeHoles(glue, csHoleR, dzglue2, off, off0);
      }
    }
  }
    lvGlue = new GeoLogVol("Glue",glue,matGlue);
    pvGlue = new GeoPhysVol(lvGlue);
  }
  
  switch(period_type)
  {
  case 1:
    {
      //Ordinary period in Barrel and Extended barrel - /spacer/glue/master/glue/spacer/glue/master/glue

      //Glue layer
      if (glue)
      {
        if (m_verbose)
            printdouble("      glue position = ",(-3.*dzglue/2-m_dbManager->TILBdzmast())*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((-3.*dzglue/2-m_dbManager->TILBdzmast())*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",-dzglue/2*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D(-dzglue/2*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",(dzglue/2+m_dbManager->TILBdzspac())*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((dzglue/2+m_dbManager->TILBdzspac())*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",(thickness-dzglue)/2*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((thickness-dzglue)/2*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);
      }

      for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	idTag = new GeoIdentifierTag(j-CurrentScin);
	m_dbManager->SetCurrentScin(j);

        scintiHeight = m_dbManager->SCNTdr();
        scintiRC = m_dbManager->SCNTrc();
        scintiZPos = zpShift * m_dbManager->SCNTzp(); // ps special module inversion
        scintiThickness = m_dbManager->SCNTdt();
        scintiWrapInZ = m_dbManager->SCNTdtw();
        scintiWrapInR = m_dbManager->SCNTdrw();
        scintiDeltaInPhi = (m_uShape > 0) ? 0.0 : m_dbManager->SCNTdphi();

        thicknessWrapper = (m_dbManager->TILBdzspac() <= (scintiThickness + 2*scintiWrapInZ)) ?
                           (scintiThickness + 2*scintiWrapInZ)*Gaudi::Units::cm: m_dbManager->TILBdzspac()*Gaudi::Units::cm;
        if (m_glue == 2) thicknessWrapper = std::max(thicknessWrapper - m_additionalIronLayer, scintiThickness);

        // create wrapper
        heightWrapper = (scintiHeight + 2*scintiWrapInR)*Gaudi::Units::cm;
        dy1Wrapper = ((scintiRC - scintiHeight/2 - scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
        dy2Wrapper = ((scintiRC + scintiHeight/2 + scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

        checking("Wrapper 0", true, 5, 
            thicknessWrapper/2,thicknessWrapper/2,dy1Wrapper,dy2Wrapper,heightWrapper/2);

	wrapper = new GeoTrd(thicknessWrapper/2,
			     thicknessWrapper/2,
			     dy1Wrapper,
			     dy2Wrapper,
			     heightWrapper/2);

	if (m_csTube) {
          wrapper = makeHoles(wrapper, csHoleR, thicknessWrapper/2, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
        }
	lvWrapper = new GeoLogVol("Wrapper",wrapper,matAir);
	pvWrapper = new GeoPhysVol(lvWrapper);

        // create scintillator
        dy1Scintillator = ((scintiRC - scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;
        dy2Scintillator = ((scintiRC + scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;

        checking("Scintillator 0", true, 6, 
                 scintiThickness/2*Gaudi::Units::cm,scintiThickness/2*Gaudi::Units::cm,dy1Scintillator,dy2Scintillator,scintiHeight/2*Gaudi::Units::cm);

	scintillator = new GeoTrd(scintiThickness/2*Gaudi::Units::cm,
				  scintiThickness/2*Gaudi::Units::cm,
				  dy1Scintillator,
				  dy2Scintillator,
				  scintiHeight/2*Gaudi::Units::cm);

	if (m_csTube) {
          scintillator = makeHolesScint(scintillator, csHoleR, scintiThickness/2 * Gaudi::Units::cm, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
	}
	lvScintillator = new GeoLogVol("Scintillator",scintillator,matScin);
	pvScintillator = new GeoPhysVol(lvScintillator);

        // place scintillator in wrapper
	tfScintillator = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	pvWrapper->add(idTag);
	pvWrapper->add(tfScintillator);
	pvWrapper->add(pvScintillator);

        //place wrapper in period
        if (m_verbose) {
          (*m_log) << MSG::VERBOSE <<" X scintiZPos= "<<scintiZPos;
          printdouble("  ==>  ",(scintiZPos*thickness+m_dbManager->TILBdzspac()/2)*Gaudi::Units::cm);
          (*m_log) << MSG::VERBOSE <<" Y scintiRC= "<<scintiRC <<endmsg;
        }
        
	tfWrapper = new GeoTransform(GeoTrf::Translate3D((scintiZPos*thickness+m_dbManager->TILBdzspac()/2)*Gaudi::Units::cm,
						    0.,
						    (scintiRC-(m_dbManager->TILBrmax()-m_dbManager->TILBrmin())/2)*Gaudi::Units::cm));
	mother->add(idTag);
	mother->add(tfWrapper);
	mother->add(pvWrapper);
      }
      break;
    }
  case 2:
    {
      //Special period in the barrel - /spacer/glue/master/glue/spacer

      //Glue layer
      if (glue)
      {
        if (m_verbose)
            printdouble("      glue position = ",(dzglue + m_dbManager->TILBdzmast())*Gaudi::Units::cm/2);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((dzglue + m_dbManager->TILBdzmast())*Gaudi::Units::cm/2,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",-(dzglue + m_dbManager->TILBdzmast())*Gaudi::Units::cm/2);
	tfGlue = new GeoTransform(GeoTrf::Translate3D(-(dzglue + m_dbManager->TILBdzmast())*Gaudi::Units::cm/2,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);
      }

      for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	idTag = new GeoIdentifierTag(j-CurrentScin);
	m_dbManager->SetCurrentScin(j);

        scintiHeight = m_dbManager->SCNTdr();
        scintiRC = m_dbManager->SCNTrc();
        scintiZPos = m_dbManager->SCNTzp();
        scintiThickness = m_dbManager->SCNTdt();
        scintiWrapInZ = m_dbManager->SCNTdtw();
        scintiWrapInR = m_dbManager->SCNTdrw();
        scintiDeltaInPhi = (m_uShape > 0) ? 0.0 : m_dbManager->SCNTdphi();

        thicknessWrapper = (m_dbManager->TILBdzspac() <= (scintiThickness + 2*scintiWrapInZ)) ?
                           (scintiThickness + 2*scintiWrapInZ)*Gaudi::Units::cm: m_dbManager->TILBdzspac()*Gaudi::Units::cm;
        if (m_glue == 2)   thicknessWrapper = std::max(thicknessWrapper - m_additionalIronLayer, scintiThickness);

        // create wrapper
        heightWrapper = (scintiHeight + 2*scintiWrapInR)*Gaudi::Units::cm;
        dy1Wrapper = ((scintiRC - scintiHeight/2 - scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
        dy2Wrapper = ((scintiRC + scintiHeight/2 + scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

        checking("Wrapper 1", true, 5, 
            thicknessWrapper/2,thicknessWrapper/2,dy1Wrapper,dy2Wrapper,heightWrapper/2);

	wrapper = new GeoTrd(thicknessWrapper/2,
			     thicknessWrapper/2,
			     dy1Wrapper,
			     dy2Wrapper,
			     heightWrapper/2);

	if (m_csTube) {
          wrapper = makeHoles(wrapper, csHoleR, thicknessWrapper/2, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
        }
	lvWrapper = new GeoLogVol("Wrapper",wrapper,matAir);
	pvWrapper = new GeoPhysVol(lvWrapper);

        // create scintillator
        dy1Scintillator = ((scintiRC - scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;
        dy2Scintillator = ((scintiRC + scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;

        checking("Scintillator 1", true, 6, 
                 scintiThickness/2*Gaudi::Units::cm,scintiThickness/2*Gaudi::Units::cm,dy1Scintillator,dy2Scintillator,scintiHeight/2*Gaudi::Units::cm);

	scintillator = new GeoTrd(scintiThickness/2*Gaudi::Units::cm,
				  scintiThickness/2*Gaudi::Units::cm,
				  dy1Scintillator,
				  dy2Scintillator,
				  scintiHeight/2*Gaudi::Units::cm);

	if (m_csTube) {
          scintillator = makeHolesScint(scintillator, csHoleR, scintiThickness/2 * Gaudi::Units::cm, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
	}
	lvScintillator = new GeoLogVol("Scintillator",scintillator,matScin);
	pvScintillator = new GeoPhysVol(lvScintillator);

        // place scintillator in wrapper
	tfScintillator = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	pvWrapper->add(idTag);
	pvWrapper->add(tfScintillator);
	pvWrapper->add(pvScintillator);

        //place wrapper in period
        if (m_verbose) {
          (*m_log) << MSG::VERBOSE <<" X scintiZPos= "<<scintiZPos; 
          printdouble("  ==>  ",(2*scintiZPos+0.5)*(thickness-m_dbManager->TILBdzspac())*Gaudi::Units::cm);
          (*m_log) << MSG::VERBOSE <<" Y scintiRC= "<<scintiRC <<endmsg;
        }
        
	tfWrapper = new GeoTransform(GeoTrf::Translate3D((2*scintiZPos+0.5)*(thickness-m_dbManager->TILBdzspac())*Gaudi::Units::cm,
						    0.,
						    (scintiRC-(m_dbManager->TILBrmax()-m_dbManager->TILBrmin())/2)*Gaudi::Units::cm));
	mother->add(idTag);
	mother->add(tfWrapper);
	mother->add(pvWrapper);
      }
      break;
    }
  case 3:
    {
      //Ordinary period in Plug Section 1 - /glue/master/glue/spacer/glue/master/glue/spacer
      
      //Glue layer
      if (glue)
      {
        if (m_verbose)
            printdouble("      glue position = ",(-thickness + dzglue)*Gaudi::Units::cm/2);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((-thickness + dzglue)*Gaudi::Units::cm/2,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",((-thickness + 3*dzglue)+m_dbManager->TILBdzmast())/2*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D(((-thickness + 3*dzglue)+m_dbManager->TILBdzmast())/2*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",dzglue/2*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D(dzglue/2*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);

        if (m_verbose)
            printdouble("      glue position = ",(3.*dzglue/2 + m_dbManager->TILBdzmast())*Gaudi::Units::cm);
	tfGlue = new GeoTransform(GeoTrf::Translate3D((3.*dzglue/2 + m_dbManager->TILBdzmast())*Gaudi::Units::cm,0.,0.));
	mother->add(tfGlue);
	mother->add(pvGlue);
      }

      for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	idTag = new GeoIdentifierTag(j-CurrentScin);
	m_dbManager->SetCurrentScin(j);

        scintiHeight = m_dbManager->SCNTdr();
        scintiRC = m_dbManager->SCNTrc();
        scintiZPos = m_dbManager->SCNTzp();
	scintiThickness = m_dbManager->SCNTdt();
        scintiWrapInZ = m_dbManager->SCNTdtw();
        scintiWrapInR = m_dbManager->SCNTdrw();
        scintiDeltaInPhi = (m_uShape > 0) ? 0. : m_dbManager->SCNTdphi();

        thicknessWrapper = (m_dbManager->TILBdzspac() <= (scintiThickness + 2*scintiWrapInZ)) ?
                           (scintiThickness + 2*scintiWrapInZ)*Gaudi::Units::cm: m_dbManager->TILBdzspac()*Gaudi::Units::cm;
        if (m_glue == 2)   thicknessWrapper = std::max(thicknessWrapper - m_additionalIronLayer, scintiThickness);

        // create wrapper
        heightWrapper = (scintiHeight + 2*scintiWrapInR)*Gaudi::Units::cm;

        dy1Wrapper = ((scintiRC - scintiHeight/2 - scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
        dy2Wrapper = ((scintiRC + scintiHeight/2 + scintiWrapInR + m_dbManager->TILBrmin()) *
                     tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;

        checking("Wrapper 2", true, 5, 
            thicknessWrapper/2,thicknessWrapper/2,dy1Wrapper,dy2Wrapper,heightWrapper/2);

	wrapper = new GeoTrd(thicknessWrapper/2,
			     thicknessWrapper/2,
			     dy1Wrapper,
			     dy2Wrapper,
			     heightWrapper/2);

	if (m_csTube) {
          wrapper = makeHoles(wrapper, csHoleR, thicknessWrapper/2, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
        }
	lvWrapper = new GeoLogVol("Wrapper",wrapper,matAir);
	pvWrapper = new GeoPhysVol(lvWrapper);

        // create scintillator
        dy1Scintillator = ((scintiRC - scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;
        dy2Scintillator = ((scintiRC + scintiHeight/2 + m_dbManager->TILBrmin()) *
                          tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;

        checking("Scintillator 2", true, 6, 
                 scintiThickness/2*Gaudi::Units::cm,scintiThickness/2*Gaudi::Units::cm,dy1Scintillator,dy2Scintillator,scintiHeight/2*Gaudi::Units::cm);

	scintillator = new GeoTrd(scintiThickness/2*Gaudi::Units::cm,
				  scintiThickness/2*Gaudi::Units::cm,
				  dy1Scintillator,
				  dy2Scintillator,
				  scintiHeight/2*Gaudi::Units::cm);

	if (m_csTube) {
          scintillator = makeHolesScint(scintillator, csHoleR, scintiThickness/2 * Gaudi::Units::cm, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
	}
	lvScintillator = new GeoLogVol("Scintillator",scintillator,matScin);
	pvScintillator = new GeoPhysVol(lvScintillator);

        // place scintillator in wrapper
	tfScintillator = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	pvWrapper->add(idTag);
	pvWrapper->add(tfScintillator);
	pvWrapper->add(pvScintillator);

        //place wrapper in period
        if (m_verbose) {
          (*m_log) << MSG::VERBOSE <<" X scintiZPos= "<<scintiZPos; 
          printdouble("  ==>  ",(scintiZPos*thickness-m_dbManager->TILBdzspac()/2)*Gaudi::Units::cm);
          (*m_log) << MSG::VERBOSE <<" Y scintiRC= "<<scintiRC <<endmsg;
        }
      
	tfWrapper = new GeoTransform(GeoTrf::Translate3D((scintiZPos*thickness-m_dbManager->TILBdzspac()/2)*Gaudi::Units::cm,
						    0.,
						    (scintiRC-(m_dbManager->TILBrmax()-m_dbManager->TILBrmin())/2)*Gaudi::Units::cm));
	mother->add(idTag);
	mother->add(tfWrapper);
	mother->add(pvWrapper);
      }
      break;
    }
  case 4:
    {
      //Special period in Plug Section  - /spacer

      for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	m_dbManager->SetCurrentScin(j);

        scintiHeight = m_dbManager->SCNTdr();
        scintiRC = m_dbManager->SCNTrc();
        scintiZPos = zpShift * m_dbManager->SCNTzp(); // ps special module inversion
        scintiThickness = m_dbManager->SCNTdt();
        scintiWrapInZ = m_dbManager->SCNTdtw();
        scintiWrapInR = m_dbManager->SCNTdrw();
        scintiDeltaInPhi = (m_uShape > 0) ? 0.0 : m_dbManager->SCNTdphi();

        thicknessWrapper = (m_dbManager->TILBdzspac() <= (scintiThickness + 2*scintiWrapInZ)) ?
                           (scintiThickness + 2*scintiWrapInZ)*Gaudi::Units::cm: m_dbManager->TILBdzspac()*Gaudi::Units::cm;
        if (m_glue == 2)   thicknessWrapper = std::max(thicknessWrapper - m_additionalIronLayer, scintiThickness);

	if(scintiZPos<0)
	{
	  idTag = new GeoIdentifierTag(j-CurrentScin);
	  // create wrapper
	  heightWrapper = (scintiHeight + 2*scintiWrapInR)*Gaudi::Units::cm;
	  dy1Wrapper = ((scintiRC - scintiHeight/2 - scintiWrapInR + m_dbManager->TILBrmin()) *
                       tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
	  dy2Wrapper = ((scintiRC + scintiHeight/2 + scintiWrapInR + m_dbManager->TILBrmin()) *
                       tan_delta_phi_2 - m_dbManager->TILBphigap()/2)*Gaudi::Units::cm;
	  
          checking("Wrapper 3", true, 5, 
              thicknessWrapper/2,thicknessWrapper/2,dy1Wrapper,dy2Wrapper,heightWrapper/2);

	  wrapper = new GeoTrd(thicknessWrapper/2,
			       thicknessWrapper/2,
			       dy1Wrapper,
			       dy2Wrapper,
			       heightWrapper/2);

          if (m_csTube) {
            wrapper = makeHoles(wrapper, csHoleR, thicknessWrapper/2, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
          }
	  lvWrapper = new GeoLogVol("Wrapper",wrapper,matAir);
	  pvWrapper = new GeoPhysVol(lvWrapper);

	  // create scintillator
	  dy1Scintillator = ((scintiRC - scintiHeight/2 + m_dbManager->TILBrmin()) *
                            tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;
	  dy2Scintillator = ((scintiRC + scintiHeight/2 + m_dbManager->TILBrmin()) *
                            tan_delta_phi_2 -  m_dbManager->TILBphigap()/2 - scintiDeltaInPhi)*Gaudi::Units::cm;
	  
          checking("Scintillator 3", true, 6, 
                   scintiThickness/2*Gaudi::Units::cm,scintiThickness/2*Gaudi::Units::cm,dy1Scintillator,dy2Scintillator,scintiHeight/2*Gaudi::Units::cm);

	  scintillator = new GeoTrd(scintiThickness/2*Gaudi::Units::cm,
				    scintiThickness/2*Gaudi::Units::cm,
				    dy1Scintillator,
				    dy2Scintillator,
				    scintiHeight/2*Gaudi::Units::cm);

          if (m_csTube) {
            scintillator = makeHolesScint(scintillator, csHoleR, scintiThickness/2 * Gaudi::Units::cm, scintiHeight/2.*Gaudi::Units::cm - csTubeOffCorr);
          }
	  lvScintillator = new GeoLogVol("Scintillator",scintillator,matScin);
	  pvScintillator = new GeoPhysVol(lvScintillator);
	  
	  // place scintillator in wrapper
	  tfScintillator = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	  pvWrapper->add(idTag);
	  pvWrapper->add(tfScintillator);
	  pvWrapper->add(pvScintillator);
	  
	  //place wrapper in period
          if (m_verbose) {
            (*m_log) << MSG::VERBOSE <<" X scintiZPos= "<<0.0 <<endmsg; 
            (*m_log) << MSG::VERBOSE <<" Y scintiRC= "<<scintiRC <<endmsg;
          }
          
	  tfWrapper = new GeoTransform(GeoTrf::Translate3D(0.,
						      0.,
						      (scintiRC-(m_dbManager->TILBrmax()-m_dbManager->TILBrmin())/2)*Gaudi::Units::cm));
	  mother->add(idTag);
	  mother->add(tfWrapper);
	  mother->add(pvWrapper);
	}
      }
      break;
    }
  default:
    {
      //Scintillator layers in Plug Sections 3 and 4      

      // checking shape of mother volume.
      double dy1Period = period->getYHalfLength1();
      double tanphi = (period->getYHalfLength2()-dy1Period)/period->getZHalfLength()/2.;
      if(m_log->level()<=MSG::DEBUG)
          if (fabs(tanphi-tan_delta_phi_2) > 1.e-5)
              (*m_log) << MSG::DEBUG <<"Different tan_delta_phi_2 " << tanphi << " " << tan_delta_phi_2  <<endmsg;
      
      for (j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	idTag = new GeoIdentifierTag(j-CurrentScin);
      	m_dbManager->SetCurrentScin(j);

        scintiHeight = m_dbManager->SCNTdr();
        scintiRC = m_dbManager->SCNTrc();
        scintiZPos = m_dbManager->SCNTzp();
        scintiThickness = m_dbManager->SCNTdt();
        scintiWrapInZ = m_dbManager->SCNTdtw();
        scintiWrapInR = m_dbManager->SCNTdrw();
        scintiDeltaInPhi = m_dbManager->SCNTdphi(); // don't need to check m_uShape for single scintillator

        // create wrapper
        heightWrapper = (scintiHeight + 2*scintiWrapInR)*Gaudi::Units::cm;
        thicknessWrapper = (scintiThickness + 2*scintiWrapInZ)*Gaudi::Units::cm;
        if (m_glue == 2)   thicknessWrapper = std::max(thicknessWrapper - m_additionalIronLayer, scintiThickness);

        double thicknessEnvelope = (m_dbManager->TILBdzmodul()*Gaudi::Units::cm - thicknessWrapper); // along phi thickness is twice bigger than along Z 
        dy1Wrapper = dy1Period - thicknessEnvelope + ((scintiRC - scintiHeight/2. - scintiWrapInR)*tanphi)*Gaudi::Units::cm;
        dy2Wrapper = dy1Period - thicknessEnvelope + ((scintiRC + scintiHeight/2. + scintiWrapInR)*tanphi)*Gaudi::Units::cm;

        if(m_log->level()<=MSG::DEBUG)
            (*m_log) << MSG::DEBUG <<"Envelope thickness is " << thicknessEnvelope <<endmsg;
        checking("Wrapper 4", true, 5, 
            thicknessWrapper/2,thicknessWrapper/2,dy1Wrapper,dy2Wrapper,heightWrapper/2);

	wrapper = new GeoTrd(thicknessWrapper/2,
			     thicknessWrapper/2,
			     dy1Wrapper,
			     dy2Wrapper,
			     heightWrapper/2);
	lvWrapper = new GeoLogVol("Wrapper",wrapper,matAir);
	pvWrapper = new GeoPhysVol(lvWrapper);

        // create scintillator
        dy1Scintillator = dy1Period - thicknessEnvelope + ((scintiRC - scintiHeight/2.)*tanphi - scintiDeltaInPhi)*Gaudi::Units::cm;
        dy2Scintillator = dy1Period - thicknessEnvelope + ((scintiRC + scintiHeight/2.)*tanphi - scintiDeltaInPhi)*Gaudi::Units::cm;

        checking("Scintillator 4", true, 6, 
            scintiThickness/2*Gaudi::Units::cm,scintiThickness/2*Gaudi::Units::cm,dy1Scintillator,dy2Scintillator,scintiHeight/2*Gaudi::Units::cm);

	scintillator = new GeoTrd(scintiThickness/2*Gaudi::Units::cm,
				  scintiThickness/2*Gaudi::Units::cm,
				  dy1Scintillator,
				  dy2Scintillator,
				  scintiHeight/2*Gaudi::Units::cm);
	lvScintillator = new GeoLogVol("Scintillator",scintillator,matScin);
	pvScintillator = new GeoPhysVol(lvScintillator);

        // place scintillator in wrapper
	tfScintillator = new GeoTransform(GeoTrf::Translate3D(0.,0.,0.));
	pvWrapper->add(idTag);
	pvWrapper->add(tfScintillator);
	pvWrapper->add(pvScintillator);

        //place wrapper in period
        if (m_verbose) {
          (*m_log) << MSG::VERBOSE <<" X scintiZPos= "<<0.0 <<endmsg; 
          (*m_log) << MSG::VERBOSE <<" Y scintiRC= "<<scintiRC <<endmsg;
        }
        
	tfWrapper = new GeoTransform(GeoTrf::Translate3D(0.,
						    0.,
						    (scintiRC-(m_dbManager->TILBrmax()-m_dbManager->TILBrmin())/2)*Gaudi::Units::cm));
	mother->add(idTag);
	mother->add(tfWrapper);
	mother->add(pvWrapper);
      }
      break;
    }
  }
}

// Checking geometry dimensions for all direction

void TileGeoSectionBuilder::checking(std::string Name, bool print, int level,
                                double X1, double X2, double Y1, double Y2, double Z) 
{
  double rless = .005; //5 [mkm]
  std::string Step[8] = {" ",
                         "  ",
                         "   ",
                         "    ",
                         "     ",
                         "      ",
                         "       ",
                         "        "};
  if (print)
   {
     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG <<Step[level]<<Name<<" -"<<level
                << std::setiosflags(std::ios::fixed) << std::setw(9) << std::setprecision(4)
		<<"  dX1,dX2= "<<X1<<" "<<X2
                <<"  dY1,dY2= "<<Y1<<" "<<Y2
                <<"  dZ= "<<Z
		<<endmsg;
   }
  if (X1 < rless && X2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, X1 or X2<0 "<<endmsg;
   }
  if (Y1 < rless && Y2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Y1 or Y2<0 "<<endmsg;
   }
  if (Z < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Z<0   "<<endmsg;
   }
}

// -- Readout

void TileGeoSectionBuilder::fillDescriptor(TileDetDescriptor*&   descriptor,
					   unsigned int          detector,
					   int                   side,
					   bool                  testbeam,
					   bool                  addPlates,
                                           unsigned int          nphi,
                                           float                 zshift)
{
  // auxillary variables
  unsigned int index;
  int currentSample, etaIndex, nsamp;
  std::vector<int> samples;    // samples of a given region

  // -- default values for all regions
  // TODO: Connect to DB !!!!!!!!!
  float phi_min, phi_max;
  float dphi = 4*acos(0.)/64;

  if(testbeam) // put modules symmetricaly 
  {
    phi_max = nphi*dphi/2;
    phi_min = -phi_max;
  }
  else // put edge of very first module on horizontal plane
  {
    phi_min = 0.;
    phi_max = nphi * dphi;
  }

  // Hardcoded values for R/dR
  float rBarrel[] = {2450., 3020., 3630.};
  float drBarrel[] = {300., 840., 380.};

  float rExtended[] = {2450., 2870., 3480.};
  float drExtended[] = {300., 540., 680.};

  float rGap[] = {3215., 3630., 2802.5, 2475.5, 2066., 1646.};
  float drGap[] = {450., 380., 313., 341., 478., 362.};

  if (addPlates) {
    rBarrel[0] -= 10*Gaudi::Units::mm/2;
    rBarrel[2] += 40*Gaudi::Units::mm/2;
    drBarrel[0] += 10*Gaudi::Units::mm;
    drBarrel[2] += 40*Gaudi::Units::mm;
    rExtended[0] -= 10*Gaudi::Units::mm/2;
    rExtended[2] += 40*Gaudi::Units::mm/2;
    drExtended[0] += 10*Gaudi::Units::mm;
    drExtended[2] += 40*Gaudi::Units::mm;
    rGap[1] += 40*Gaudi::Units::mm/2;
    drGap[1] += 40*Gaudi::Units::mm;
  }
    
  int indHardcoded = 0;
  
  // --------------------------------

  // -- initialize all necessary arrays with 0.
  std::vector<float> eta_min (MAX_N_SAMP_TILEDD, 0.);
  std::vector<float> eta_max (MAX_N_SAMP_TILEDD, 0.);
  std::vector<float> deta (MAX_N_SAMP_TILEDD, 0.);
  std::vector<unsigned int> neta (size_t(MAX_N_SAMP_TILEDD), 0U);
  std::vector<float> rcenter (MAX_N_SAMP_TILEDD, 0.);
  std::vector<float> dr (MAX_N_SAMP_TILEDD, 0.);
  std::vector<float> zcenter (MAX_N_SAMP_TILEDD, 0.);
  std::vector<float> dz (MAX_N_SAMP_TILEDD, 0.);

  // ---------------------------------

  // Set side
  int sign_eta = (side>0) ? 1 : -1;


  // iterate over all samples of the region and fill corresponding eta values.
  // this works for Central and Extended Barrel regions only!
  if (detector==TILE_REGION_CENTRAL || detector ==  TILE_REGION_EXTENDED)
  {
    // -- Calculate number of samples for the region
    // find first TICL for the given detector
    if(m_dbManager->SetFirstTiclInDet(detector))
    {
      // for the central barrel region take care of the side
      if(detector == TILE_REGION_CENTRAL)
      {
	while(sign_eta * m_dbManager->TICLncell() < 0)
	{
	  if (!(m_dbManager->SetNextTiclInDet()))
	  {
	    (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::fillDescriptor: No TICL structures for detector: "
                     << detector << " and side: " << sign_eta << endmsg;
	    return;
	  }
	}
      }

      samples.push_back((int)m_dbManager->TICLsample());

      // iterate over all other TICLs in the detector
      while(m_dbManager->SetNextTiclInDet())
      {
	if(!((detector == TILE_REGION_CENTRAL)&&(sign_eta * m_dbManager->TICLncell() < 0)))
	{
	  // find if the sample of the cell is already in the samples vector
	  // and if not then add it
	  currentSample = (int)m_dbManager->TICLsample();
	  for(index=0; index<samples.size(); index++)
	    if(currentSample==samples[index]) break;
	  if(index==samples.size()) samples.push_back(currentSample);
	}
      }
    }
    else
    {
      (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::fillDescriptor: No TICL structures for the detector: "
               << detector << endmsg;
      return;
    }

    nsamp = 0;
    for(index=0; index<samples.size(); index++, indHardcoded++)
    {
      etaIndex = samples[index];
      if(etaIndex > nsamp) nsamp = etaIndex;
      calculateEta(detector,
		   side,
		   etaIndex,
		   eta_min[etaIndex-1],      // In TICL structures samples are numbered from 1
		   eta_max[etaIndex-1],
		   deta[etaIndex-1],
		   neta[etaIndex-1]);
      calculateZ(detector,
                 side,
                 etaIndex,
                 zshift,
                 zcenter[etaIndex-1],
                 dz[etaIndex-1]);

      // Fill in the hardcoded values
      if(detector == TILE_REGION_CENTRAL)
      {
         rcenter[etaIndex-1] = rBarrel[indHardcoded];
         dr[etaIndex-1] = drBarrel[indHardcoded];
      }
      else
      {
         rcenter[etaIndex-1] = rExtended[indHardcoded];
         dr[etaIndex-1] = drExtended[indHardcoded];	      
      }
    }
  }
  else if(detector == TILE_REGION_GAP)
  {
    // We are in GAP detectors (detector = 3 & 4)
    // Loop over all TICL structures of these detectors
    // Determine eta_min, eta_max, deta
    // neta = 1 for all
    //
    // !!! For DETECTOR=4 INDEX is set by TOWER and NOT SAMPLE !!!
    //

    nsamp = 0;
    for(unsigned int det = detector; det < detector+2; det++)
      for(int status=m_dbManager->SetFirstTiclInDet(det); status>0; status=m_dbManager->SetNextTiclInDet())
      {
        if ((int)m_dbManager->TICLtower()>16) continue;
	if(det==detector)
	  etaIndex = (int)m_dbManager->TICLsample();
	else
	  etaIndex = (int)m_dbManager->TICLtower();
	if(etaIndex > nsamp) nsamp = etaIndex;
        // simple calculations of eta for one cell
	neta[etaIndex-1] = 1;
	deta[etaIndex-1] = m_dbManager->TICLdeta();
	eta_min[etaIndex-1] = m_dbManager->TICLeta() - m_dbManager->TICLdeta()/2;
	eta_max[etaIndex-1] = m_dbManager->TICLeta() + m_dbManager->TICLdeta()/2;
        calculateZ(detector,
                   side,
                   etaIndex,
                   zshift,
                   zcenter[etaIndex-1],
                   dz[etaIndex-1]);
 
	// Fill in the hardcoded values
	rcenter[etaIndex-1] = rGap[indHardcoded];
	dr[etaIndex-1] = drGap[indHardcoded];
	indHardcoded++;
    }
  }
  else // MBSCIN case
       // - should not arrive to this place !!!
       // TileDetDescrManager do not expect MBSCIN at all
       // it is kept here just in case we'll want to describe
       // them as a separate region
       //
  {
    assert (detector <= TILE_REGION_GAP);

    // diameter: 276 - 740 - 1964
    // radius:   138 - 370 -  982
    float rMB[]  = { 676., 254. };
    float drMB[] = { 306., 116. };
    float zMB = 3500;
    float dzMB = 30;

    for (int ind=0; ind<2; ++ind) {
      neta[ind] = 1;
      deta[ind] = 1.;
      eta_min[ind] = 2. + ind;
      eta_max[ind] = 3. + ind;
      zcenter[ind] = zMB;
      dz[ind] = dzMB;
      rcenter[ind] = rMB[ind];
      dr[ind] = drMB[ind];
    }
    nsamp = 2;
    nphi  = 8;
    dphi  = atan(1.);
    phi_min = -dphi/2;
    phi_max = nphi * dphi + phi_min;
  }
  
  descriptor->set(&*eta_min.begin(),
		  &*eta_max.begin(),
		  &*deta.begin(),
		  phi_min,
		  phi_max,
		  dphi,
		  zshift,
		  &*rcenter.begin(),
		  &*dr.begin(),
		  &*zcenter.begin(),
		  &*dz.begin(),
		  nsamp,
		  &*neta.begin(),
		  nphi,
		  sign_eta);


  // debug print
  if (m_verbose)
    descriptor->print();

}


/**
 Method creating one TileCellDim object per particular type of Tile Cell
 Thes objects can be accessed later through TileDetDescManager 
 providing Cell Identifier with module field = 0
*/

void TileGeoSectionBuilder::computeCellDim(TileDetDescrManager*& manager,
					   int                   detector,
                                           bool                  addPlates,
                                           float                 zShiftPos,
                                           float                 zShiftNeg)
{
  if (m_log->level()<=MSG::DEBUG) 
    (*m_log) << MSG::DEBUG << "TileGeoSectionBuilder::computeCellDim for detector="
             << detector << endmsg;

  boost::io::ios_all_saver ias (std::cout);

  /** Vectors holding rMin, rMax, zMin, zMax for each tilerow of the cell */
  std::vector<double> rmins;
  std::vector<double> rmaxs;
  std::vector<double> zmins;
  std::vector<double> zmaxs;

  /** Sample numbers for a section */
  int currentSample;
  std::vector<int> samples;

  /** -------- Auxiliary variables ---------- */
  unsigned int index;
  unsigned int CurrentScin;

  int nSide, nTower, nSample=0;
  unsigned int nFirstRow, nLastRow, nRows;

  double dzCell, dzPeriod;
  double rMin, rMax;
  double zEnd1 =0,zEnd2 =0,zEnd1Lim =0,zEnd2Lim =0;

  bool isSquare;
  float minTilesInRow;
  TileCellDim *cellDim =0, *cellDimNeg =0;
  // -------- Auxiliary variables ---------- 

  // Barrel/Extended barrel regions 
  if(detector != TILE_REGION_GAP ) 
  {
    if(detector == TILE_REGION_CENTRAL)
    {
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
      dzPeriod = m_barrelPeriodThickness;
    }
    else
    {
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
      dzPeriod = m_extendedPeriodThickness;
    }

    rMin = m_dbManager->TILBrmin() *Gaudi::Units::cm;
    if (addPlates) rMin -= m_dbManager->TILBdrfront() *Gaudi::Units::cm;
    CurrentScin = 100*m_dbManager->TILBsection() + 1;
    //dzMaster = m_dbManager->TILBdzmast()*Gaudi::Units::cm;

    /** Initialize rMin, rMax vectors  - once per region
        Initial values for zMin - leftmost edge
        Initial values for zMax = 0
    */
    for (unsigned int j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
    {
      m_dbManager->SetCurrentScin(j);

      if ( (j == CurrentScin + m_dbManager->TILBnscin() - 1) && addPlates ) {
        // use end of the master as end of last cell 
        rMax = m_dbManager->TILBrmax()*Gaudi::Units::cm; 
      } else {
        double tileSize=m_dbManager->SCNTdr();
        if (m_dbManager->SCNTdrw() > 0)
          // round to integer for all tiles except gap scin 
          tileSize=round(tileSize); 
        rMax = (m_dbManager->TILBrmin() + m_dbManager->SCNTrc() + tileSize/2)*Gaudi::Units::cm;
      }

      rmins.push_back(rMin);
      rmaxs.push_back(rMax);
      rMin = rMax;

      if(detector == TILE_REGION_CENTRAL) {
	zmins.push_back((-m_dbManager->TILBdzmast()/2 - m_barrelGlue*(1./Gaudi::Units::cm) 
                         -(m_dbManager->TILBdzmodul()/2-m_dbManager->TILBdzend1()))*Gaudi::Units::cm);
        zEnd1Lim = (-m_dbManager->TILBdzmodul()/2+m_dbManager->TILBdzend1())*Gaudi::Units::cm;
        zEnd2Lim = ( m_dbManager->TILBdzmodul()/2-m_dbManager->TILBdzend2())*Gaudi::Units::cm;
        zEnd1 = (-m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;
        zEnd2 = ( m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;
      }
      else {
	zmins.push_back((m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2 + m_dbManager->TILBdzend1())*Gaudi::Units::cm);
        zEnd1Lim = (m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2+m_dbManager->TILBdzend1()+0.001)*Gaudi::Units::cm;
        zEnd2Lim = (m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2-m_dbManager->TILBdzend2()-0.001)*Gaudi::Units::cm;
        zEnd1 = (m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;
        zEnd2 = (m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;
      }
      
      zmaxs.push_back(0.);
    }

    // Iterate through region cells and fill in the Samples vector 
    if(m_dbManager->SetFirstTiclInDet(detector))
    {
      samples.push_back((int)m_dbManager->TICLsample());

      // iterate over all other TICLs in the detector
      while(m_dbManager->SetNextTiclInDet())
      {
	/// find if the sample of the cell is already in the samples vector and if not then add it 
	currentSample = (int)m_dbManager->TICLsample();
	for(index=0; index<samples.size(); index++)
	  if(currentSample==samples[index]) break;
	if(index==samples.size()) samples.push_back(currentSample);
      }
    }
    else
    {
      (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::computeCellDim: No TICL structures for the detector: "
               << detector << endmsg;
      return;
    }

    /** Iterate through cells by samples */
    for(index=0; index<samples.size(); index++)
    {
      currentSample = samples[index];

      if(m_dbManager->SetFirstTiclInDetSamp(detector, currentSample))
      {
	nFirstRow = (int)m_dbManager->TICLfirstrow();
        // BC sample 
	if(m_dbManager->TICLsample()==2 && detector == TILE_REGION_CENTRAL) 
	  nLastRow = nFirstRow + 2;
	else
	  nLastRow = (int)m_dbManager->TICLlastrow();
	
	nRows = nLastRow - nFirstRow + 1;

	do
	{
	  isSquare = true;
	  minTilesInRow = m_dbManager->TICLntilesrow(0);
	  for(unsigned int jj=1; jj<nRows; jj++)
	  {
	    if(m_dbManager->TICLntilesrow(jj)!=minTilesInRow)
	      isSquare = false;
	    if(m_dbManager->TICLntilesrow(jj)<minTilesInRow)
	      minTilesInRow = m_dbManager->TICLntilesrow(jj);
	  }

	  if(isSquare)
	    dzCell = minTilesInRow*dzPeriod;
	  else
	    dzCell = (minTilesInRow + 0.5)*dzPeriod;

	  for(unsigned int jj=nFirstRow; jj<=nLastRow; jj++)
	    zmaxs[jj-1] = zmins[jj-1] + dzCell;


	  /** -------- BC ---------- */
	  if(m_dbManager->TICLsample()==2 && detector == TILE_REGION_CENTRAL)
	  {
	    isSquare = true;
	    int firstC = nLastRow + 1;
	    nLastRow +=3;

	    minTilesInRow = m_dbManager->TICLntilesrow(firstC-nFirstRow);

	    for(unsigned int jj=firstC-nFirstRow+1; jj<=nLastRow-nFirstRow; jj++)
	    {
	      if(m_dbManager->TICLntilesrow(jj)!=minTilesInRow)
		isSquare = false;
	      if(m_dbManager->TICLntilesrow(jj)<minTilesInRow)
		minTilesInRow = m_dbManager->TICLntilesrow(jj);
	    }

	    if(isSquare)
	      dzCell = minTilesInRow*dzPeriod;
	    else
	      dzCell = (minTilesInRow + 0.5)*dzPeriod;

	    for(unsigned int jj=firstC; jj<=nLastRow; jj++)
	      zmaxs[jj-1] = zmins[jj-1] + dzCell;
	  }

	  /** Create and fill in TileCellDim objects, one for Central Barrel two for extended (+/- side) */
	  if(m_dbManager->TICLsample()==2 && detector == TILE_REGION_CENTRAL)
	    cellDim = new TileCellDim(2*nRows);
	  else
	    cellDim = new TileCellDim(nRows);

	  if(detector != TILE_REGION_CENTRAL)
	    cellDimNeg = new TileCellDim(nRows);

	  for(unsigned int jj=nFirstRow; jj<=nLastRow; jj++)
	  {
            double Zmin=zmins[jj-1];
            double Zmax=zmaxs[jj-1];
            if (addPlates) {
              if (Zmin<zEnd1Lim) Zmin=zEnd1;
              else if (Zmin>zEnd2Lim) Zmin=zEnd2;
              if (Zmax<zEnd1Lim) Zmax=zEnd1;
              else if (Zmax>zEnd2Lim) Zmax=zEnd2;
            }
            
	    cellDim->addRMin(rmins[jj-1]);
	    cellDim->addRMax(rmaxs[jj-1]);
            cellDim->addZMin(Zmin+zShiftPos);
            cellDim->addZMax(Zmax+zShiftPos);
            
	    if(detector != TILE_REGION_CENTRAL)
	    {
	      cellDimNeg->addRMin(rmins[jj-1]);
	      cellDimNeg->addRMax(rmaxs[jj-1]);
              cellDimNeg->addZMin(-Zmax-zShiftNeg);
              cellDimNeg->addZMax(-Zmin-zShiftNeg);
	    }
	  }

	  for(unsigned int jj=nFirstRow; jj<=nLastRow; jj++)
	    zmins[jj-1] = zmaxs[jj-1];
	  
	  if(m_dbManager->TICLncell()<0)
	    nSide = -1;
	  else
	    nSide = 1;
	  
	  nTower = abs((int)m_dbManager->TICLtower())-1;
	  nSample = currentSample - 1;

	  if(detector == TILE_REGION_CENTRAL) {
            cellDim->computeVolume();
	    manager->add_cellDim(detector, nSide, nTower, nSample, cellDim);
	  } else
	  {
            cellDim->computeVolume();
            cellDimNeg->computeVolume();
	    manager->add_cellDim(detector, 1, nTower, nSample, cellDim);
	    manager->add_cellDim(detector, -1, nTower, nSample, cellDimNeg);
	  }

	  if(m_dbManager->TICLsample()==2 && detector == TILE_REGION_CENTRAL) nLastRow -=3;

	  /* -------- DEBUG printouts -------------- */

          if (m_verbose) {
            std::cout << std::setiosflags(std::ios::fixed)
                      << std::setw(9) << std::setprecision(2);
            std::cout << "\n **** Cell dimensions computed for : ";
            std::cout << "(" << detector << ", " 
                      << nSide << ", "
                      << nTower << ", "
                      << nSample << ")\n";
            std::cout << " >> CellDim contains " << cellDim->getNRows() << " rows\n";
            for(unsigned int jj=0; jj<cellDim->getNRows(); jj++)
              std::cout << cellDim->getRMin(jj) << " " 
                        << cellDim->getRMax(jj) << " "
                        << cellDim->getZMin(jj) << " "
                        << cellDim->getZMax(jj) << "\n";
            std::cout << " >> Cell Volume is " << cellDim->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";

            if(detector != TILE_REGION_CENTRAL)
            {
              std::cout << " >> CellDimNeg contains " << cellDimNeg->getNRows() << " rows\n";
              for(unsigned int jj=0; jj<cellDimNeg->getNRows(); jj++)
                std::cout << cellDimNeg->getRMin(jj) << " " 
                          << cellDimNeg->getRMax(jj) << " "
                          << cellDimNeg->getZMin(jj) << " "
                          << cellDimNeg->getZMax(jj) << "\n";
              std::cout << " >> CellNeg Volume is " << cellDimNeg->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";
            }
          }
	  /* ------------------------------------------------------------------------------------------------ */	  
	}
	while(m_dbManager->SetNextTiclInDetSamp());
      }
      else
      {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::computeCellDim: No TICL structures in detector: "
                 << detector << " for sample: " << nSample << endmsg;
	return;
      }
    }
  }
  else // ITC/GAP/CRACK
  {
    nSide = 1;
    // Deal with ITC
    for(unsigned int sec=0; sec<2; sec++)    // PLUG1 & PLUG2
    { 
      rmins.clear();
      rmaxs.clear();
      zmins.clear();
      zmaxs.clear();

      m_dbManager->SetFirstTiclInDet(detector);

      if(sec)
        m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
      else
      {
        m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
        m_dbManager->SetNextTiclInDet();
      }

      rMin = m_dbManager->TILBrmin()*Gaudi::Units::cm;
      if (addPlates) rMin -= m_dbManager->TILBdrfront() *Gaudi::Units::cm;
      CurrentScin = 100*m_dbManager->TILBsection() + 1;

      for (unsigned int j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
        m_dbManager->SetCurrentScin(j);

        if ( (j == CurrentScin + m_dbManager->TILBnscin() - 1)  && addPlates ) {
          /** use end of the master as end of last cell */
          rMax = m_dbManager->TILBrmax()*Gaudi::Units::cm; 
          /** subtract from C10 thickness of D4 front plate  */
          if (addPlates && sec)
            rMax -= m_dbManager->TILBdrfront()*Gaudi::Units::cm;
        } else {
          double tileSize=m_dbManager->SCNTdr();
          if (m_dbManager->SCNTdrw() > 0)
            /** round to integer for all tiles except gap scin */
            tileSize=round(tileSize); 
          rMax = (m_dbManager->TILBrmin() + m_dbManager->SCNTrc() + tileSize/2)*Gaudi::Units::cm;
        }

        rmins.push_back(rMin);
        rmaxs.push_back(rMax);
        rMin = rMax;
        
        zEnd1Lim = (m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2+m_dbManager->TILBdzend1()+0.001)*Gaudi::Units::cm;
        zEnd2Lim = (m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2-m_dbManager->TILBdzend2()-0.001)*Gaudi::Units::cm;
        zEnd1 = (m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;
        zEnd2 = (m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm;

        if ( addPlates ) {
          zmins.push_back((m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm);
          zmaxs.push_back((m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm);
        } else {
          zmins.push_back((m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2 + m_dbManager->TILBdzend1())*Gaudi::Units::cm);
          zmaxs.push_back((m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2 - m_dbManager->TILBdzend2())*Gaudi::Units::cm);
        }
      }

      // Create TileCellDim objects
      if (m_dbManager->TILBnscin() < 0) {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::computeCellDim: Bad return from TILBnscin" << endmsg;
	return;
      }
      unsigned int nscin = static_cast<unsigned int>(m_dbManager->TILBnscin());
      cellDim = new TileCellDim(nscin);
      cellDimNeg = new TileCellDim(nscin);

      for(unsigned int jj=0; jj<nscin; jj++)
      {
        cellDim->addRMin(rmins[jj]);
        cellDim->addRMax(rmaxs[jj]);
        cellDim->addZMin(zmins[jj]+zShiftPos);
        cellDim->addZMax(zmaxs[jj]+zShiftPos);

        cellDimNeg->addRMin(rmins[jj]);
        cellDimNeg->addRMax(rmaxs[jj]);
        cellDimNeg->addZMin(-zmaxs[jj]-zShiftNeg);
        cellDimNeg->addZMax(-zmins[jj]-zShiftNeg);
      }


      nTower = (int)m_dbManager->TICLtower()-1;
      nSample = (int)m_dbManager->TICLsample()-1;
	
      cellDim->computeVolume();
      cellDimNeg->computeVolume();
      manager->add_cellDim(detector, 1, nTower, nSample, cellDim);
      manager->add_cellDim(detector, -1, nTower, nSample, cellDimNeg);

/* -------- DEBUG printouts -------------- */
      if (m_verbose) {
        std::cout << std::setiosflags(std::ios::fixed)
                  << std::setw(9) << std::setprecision(2);
        std::cout << "\n **** Cell dimension computed for : ";
        std::cout << "(" << detector << ", " 
                  << nSide << ", "
                  << nTower << ", "
                  << nSample << ")\n";
        std::cout << " >> CellDim contains " << cellDim->getNRows() << " rows\n";
        for(unsigned int jj=0; jj<cellDim->getNRows(); jj++)
          std::cout << cellDim->getRMin(jj) << " " 
                    << cellDim->getRMax(jj) << " "
                    << cellDim->getZMin(jj) << " "
                    << cellDim->getZMax(jj) << "\n";
        std::cout<< " >> Cell Volume is " << cellDim->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";
    
        std::cout << " >> CellDimNeg contains " << cellDimNeg->getNRows() << " rows\n";
        for(unsigned int jj=0; jj<cellDimNeg->getNRows(); jj++)
          std::cout << cellDimNeg->getRMin(jj) << " " 
                    << cellDimNeg->getRMax(jj) << " "
                    << cellDimNeg->getZMin(jj) << " "
                    << cellDimNeg->getZMax(jj) << "\n";
        std::cout << " >> CellNeg Volume is " << cellDimNeg->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";
      }
/* -------------------------------------------- */
    }

    for(unsigned int sec=0; sec<2; sec++)    // GAP & CRACK
    {
      rmins.clear();
      rmaxs.clear();
      zmins.clear();
      zmaxs.clear();

      if(sec)
      {
	m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
	CurrentScin = 100*m_dbManager->TILBsection()+1;
      }
      else
      {
	m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
	CurrentScin = 100*m_dbManager->TILBsection()+1;
      }

      rMin = m_dbManager->TILBrmin()*Gaudi::Units::cm;

      // Initialize rMin, rMax, zMin, zMax vectors
      for (unsigned int j = CurrentScin; j < (CurrentScin + m_dbManager->TILBnscin()); j++)
      {
	m_dbManager->SetCurrentScin(j);
	rMax = rMin + m_dbManager->SCNTdr()*Gaudi::Units::cm;
	
	rmins.push_back(rMin);
	rmaxs.push_back(rMax);
	rMin = rMax;
	
	zmins.push_back((m_dbManager->TILBzoffset() - m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm);
	zmaxs.push_back((m_dbManager->TILBzoffset() + m_dbManager->TILBdzmodul()/2)*Gaudi::Units::cm);
      }

      // Iterate through scintillators and create corresponding TileCellDim objects (+/-)
      for (unsigned int jj = 0; jj < (unsigned int)m_dbManager->TILBnscin(); jj++)
      {
	if(sec==0 && jj==0)
	  m_dbManager->SetFirstTiclInDet(detector+1);
	else
	  m_dbManager->SetNextTiclInDet();
        if ((int)m_dbManager->TICLtower()>16) continue;

	cellDim = new TileCellDim(1);
	cellDimNeg = new TileCellDim(1);

	cellDim->addRMin(rmins[m_dbManager->TILBnscin() - jj - 1]);
	cellDim->addRMax(rmaxs[m_dbManager->TILBnscin() - jj - 1]);
	cellDim->addZMin(zmins[m_dbManager->TILBnscin() - jj - 1]+zShiftPos);
	cellDim->addZMax(zmaxs[m_dbManager->TILBnscin() - jj - 1]+zShiftPos);

	cellDimNeg->addRMin(rmins[m_dbManager->TILBnscin() - jj - 1]);
	cellDimNeg->addRMax(rmaxs[m_dbManager->TILBnscin() - jj - 1]);
	cellDimNeg->addZMin(-zmaxs[m_dbManager->TILBnscin() - jj - 1]-zShiftNeg);
	cellDimNeg->addZMax(-zmins[m_dbManager->TILBnscin() - jj - 1]-zShiftNeg);

	nTower = (int)m_dbManager->TICLtower()-1;
	nSample = (int)m_dbManager->TICLsample()-1;
	
        cellDim->computeVolume();
        cellDimNeg->computeVolume();
	manager->add_cellDim(detector, 1, nTower, nSample, cellDim);
	manager->add_cellDim(detector, -1, nTower, nSample, cellDimNeg);

/* -------- DEBUG printouts -------------- */
        if (m_verbose) {
          std::cout << std::setiosflags(std::ios::fixed)
                    << std::setw(9) << std::setprecision(2);
          std::cout << "\n **** Cell dimension computed for : ";
          std::cout << "(" << detector << ", " 
                    << nSide << ", "
                    << nTower << ", "
                    << nSample << ")\n";
          std::cout << " >> CellDim contains " << cellDim->getNRows() << " rows\n";
          for(unsigned int jj=0; jj<cellDim->getNRows(); jj++)
            std::cout << cellDim->getRMin(jj) << " " 
                      << cellDim->getRMax(jj) << " "
                      << cellDim->getZMin(jj) << " "
                      << cellDim->getZMax(jj) << "\n";
          std::cout << " >> Cell Volume is " << cellDim->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";
    
          std::cout << " >> CellDimNeg contains " << cellDimNeg->getNRows() << " rows\n";
          for(unsigned int jj=0; jj<cellDimNeg->getNRows(); jj++)
            std::cout << cellDimNeg->getRMin(jj) << " " 
                      << cellDimNeg->getRMax(jj) << " "
                      << cellDimNeg->getZMin(jj) << " "
                      << cellDimNeg->getZMax(jj) << "\n";
          std::cout << " >> CellNeg Volume is " << cellDimNeg->getVolume()*(1./Gaudi::Units::cm3) << " cm^3\n";
        }
/* -------------------------------------------- */
      }
    }
  } 
}

void TileGeoSectionBuilder::calculateZ(int detector,
                                       int side,
                                       int sample,
                                       float zshift,
                                       float& zcenter,
                                       float& dz)
{
  switch(detector) {
  case TILE_REGION_CENTRAL:
    m_dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
    break;
  case TILE_REGION_EXTENDED:
    m_dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
    break;
  case TILE_REGION_GAP:
    if (sample==3) { // D4
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
    } else if (sample==2) { // C10
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
    } else if (sample<13) { // E1-E2
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
    } else { // E3-E4
      m_dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
    }
    break;
  default:
    (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::calculateZ: Unexpected detector: "
             << detector << endmsg;
    return;
  }

  // first - find position in ideal world before Z-shift and misalignment
  if (detector == TILE_REGION_CENTRAL) {
    // need to split one cylinder in pos/neg halves
    float zmin=m_dbManager->TILBzoffset()/2 * Gaudi::Units::cm ;
    float zmax=zmin+m_dbManager->TILBdzmodul()/2 * Gaudi::Units::cm ;
    if (sample==3) { // fix for D0 cell 
      float D0size = 560.58/307*40 * Gaudi::Units::cm; // size of D0 along Z in Gaudi::Units::cm
                                  // FIXME:: should be taken from DB
      if (side>0) // positive
        zmin = - D0size/2;
      else
        zmin = D0size/2;
    }
    zcenter = (zmin+zmax)/2;
    dz = (zmax-zmin);
  } else if (detector == TILE_REGION_GAP && (sample > 9) ){
    zcenter=m_dbManager->TILBzoffset() * Gaudi::Units::cm ;
    m_dbManager->SetCurrentScin(100*m_dbManager->TILBsection() + 1 );
    dz =  m_dbManager->SCNTdt()*Gaudi::Units::cm;
  }
  else {
    zcenter=m_dbManager->TILBzoffset() * Gaudi::Units::cm ;
    dz=m_dbManager->TILBdzmodul() * Gaudi::Units::cm ;
  }

  // apply zshift from ideal pseudo-projective eta (which includes alignment also!)
  zcenter += zshift;
  // inversion for negative side
  if (side<1) zcenter *= -1;

  return;
}

void TileGeoSectionBuilder::calculateEta(int detector,
					 int side,
					 int sample,
					 float& etamin,
					 float& etamax,
					 float& deta,
					 unsigned int& neta)
{
  /** 
   The following actions need to be performed for Central/Extended Barrel
       A. Find first cell. D0 goes to the positive side
       B. Initialize etamax, etamin and neta. deta initialized but not changed later.
       C. Iterate over all remaining cells in the sample and fill etamin, etamax, neta.
  
   Any other detectors are not expected!
  */

  int sign_eta = 1;
  if ((detector == TILE_REGION_CENTRAL) && (side<1)) sign_eta = -1;

  switch(detector)
  {
  case TILE_REGION_CENTRAL:  // Central Barrel
  case TILE_REGION_EXTENDED:  // Extended Barrel
    {
      if(m_dbManager->SetFirstTiclInDetSamp(detector, sample))
      {
	// --- A ---
	while(sign_eta * m_dbManager->TICLncell() < 0)
	  if (!(m_dbManager->SetNextTiclInDetSamp()))
	  {
	    (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::calculateEta: No TICL structures of positive side "
                     << "in Central Barrel for sample: " << sample << endmsg;
	    return;
	  }

	// --- B ---
	neta = 1;
	deta = m_dbManager->TICLdeta();
	etamin = m_dbManager->TICLeta()*sign_eta - m_dbManager->TICLdeta()/2; 
	etamax = m_dbManager->TICLeta()*sign_eta + m_dbManager->TICLdeta()/2;

	// --- C ---
	while(m_dbManager->SetNextTiclInDetSamp())
	{
	  if(!((detector == TILE_REGION_CENTRAL)&&(sign_eta * m_dbManager->TICLncell() <= 0)))
	  {
	    neta++;
	    if (etamax < m_dbManager->TICLeta()*sign_eta + m_dbManager->TICLdeta()/2)
	      etamax = m_dbManager->TICLeta()*sign_eta + m_dbManager->TICLdeta()/2;
	    if (etamin > m_dbManager->TICLeta()*sign_eta - m_dbManager->TICLdeta()/2)
	      etamin = m_dbManager->TICLeta()*sign_eta - m_dbManager->TICLdeta()/2;
	  }
	}
      }
      else
      {
	(*m_log) << MSG::ERROR << "TileGeoSectionBuilder::calculateEta: No TICL structures in detector: "
                 << detector << " for sample: " << sample << endmsg;
	return;
      }
      break;
    }
  default:
    {
      (*m_log) << MSG::ERROR << "TileGeoSectionBuilder::calculateEta: Unexpected detector: "
               << detector << endmsg;
      return;
    }
  }
}

void TileGeoSectionBuilder::setBarrelPeriodThickness(double val)
{
  m_barrelPeriodThickness = val;
}

void TileGeoSectionBuilder::setBarrelGlue(double val)
{
  m_barrelGlue = val;
}

void TileGeoSectionBuilder::setExtendedPeriodThickness(double val)
{
  m_extendedPeriodThickness = val;
}

void TileGeoSectionBuilder::checktransfunc(double absorber, double period, int np, double center)
{
    boost::io::ios_all_saver ias (std::cout);
    std::cout << std::setprecision (std::numeric_limits<double>::digits10 + 1)
              << " Absorber center = " << center 
              << "          length = " << absorber 
              << "          period = " << period 
              << "        nperiods = " << np 
              << std::endl;
    double zC,dZ,z1,z2=-absorber/2.+center;
    for (int i=0; i<np; ++i) {
        zC = (period*(2*i+1)-absorber)/2. + center;
        z1 = zC - period/2.;
        dZ = z1 - z2;
        z2 = zC + period/2.;
        std::cout << std::setw(4)
                  << i << "  z1= " << z1 << "  z2= " << z2 
                  << "  dist from previous= " << dZ << std::endl;
    }
}

void TileGeoSectionBuilder::printdouble(const char * name, double val)
{
    (*m_log) << MSG::VERBOSE  << std::setprecision (std::numeric_limits<double>::digits10 + 1)
             << name << val << endmsg;
}

const GeoShape * TileGeoSectionBuilder::makeHolesScint(const GeoShape * mother, double R, double H2, double off, double off0) {
    GeoShape *hole = new GeoTubs(0., R, H2, 0., 360.0 * Gaudi::Units::deg);
    const  GeoShapeUnion& scintUnion = hole->add( *hole << GeoTrf::Translate3D((off0-off*2.0),0.,0.));
    GeoTrf::Transform3D tfHole = GeoTrf::Translate3D(0.,0.,(off0-off)) * GeoTrf::RotateY3D(90*Gaudi::Units::deg);
    const GeoShape & motherWithHoles = (mother->subtract(scintUnion<<tfHole));
    return &motherWithHoles;
}

const GeoShape * TileGeoSectionBuilder::makeHoles(const GeoShape * mother, double R, double H2, double off, double off0) {
  GeoShape *hole1 = new GeoTubs(0., R, H2, 0., 360.0 * Gaudi::Units::deg);
  GeoShape *hole2 = new GeoTubs(0., R, H2, 0., 360.0 * Gaudi::Units::deg);
  GeoTrf::Transform3D tfHole1 = GeoTrf::Translate3D(0.,0.,(off0-off)) * GeoTrf::RotateY3D(-90*Gaudi::Units::deg);
  GeoTrf::Transform3D tfHole2 = GeoTrf::Translate3D(0.,0.,(off0+off)) * GeoTrf::RotateY3D(-90*Gaudi::Units::deg);
  const GeoShape & motherWithHoles = (mother->subtract((*hole1)<<tfHole1).subtract((*hole2)<<tfHole2));
  return &motherWithHoles;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SupportRailFactory.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShape.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include <iostream>
#include <math.h>


SupportRailFactory::SupportRailFactory(const InDetDD::AthenaComps * athenaComps, 
				       InDetMaterialManager * matManager)
  : InDetDD::SubDetectorFactoryBase(athenaComps, matManager)
{  
}

SupportRailFactory::~SupportRailFactory()
{
}



//## Other Operations (implementation)
void SupportRailFactory::create(GeoPhysVol *mother)
{

  DecodeVersionKey atlasVersionKey(geoDbTagSvc(),"ATLAS");
  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  DecodeVersionKey larVersionKey(geoDbTagSvc(),"LAr");

  std::string railversion =  rdbAccessSvc()->getChildTag("IDDetailedRail",indetVersionKey.tag(),indetVersionKey.node());
  if(!railversion.empty()) {
    // ______________________________________________________________________________________________________________________
    // Geometry with ID rail
 
//  std::cout <<" Version!!="<<atlasVersionKey.tag()<<", "<<atlasVersionKey.node()<<'\n';
//  std::cout <<" Version!!="<<indetVersionKey.tag()<<", "<<indetVersionKey.node()<<'\n';
//---------------------------------------------------------------------------------
//     Long ribs (initial position is along X axis then move and rotate)
//
//      Th
//      ___
//     |  |
//     |  |  Wid
//     |  |
//     |__|
//
// Default(initial) values
//    
    double RMAX_ID      = 1150.0*CLHEP::mm -1.0*CLHEP::mm;   //Some safety margin
    double railLengthB  = 1600.0*CLHEP::mm;
    double railWidthB   = 5.5*CLHEP::mm;
    double railThickB   = 34.7*CLHEP::mm;
//
    double railLengthE  = 2600.0*CLHEP::mm;
//    double railWidthE   = 14.*CLHEP::mm;
//    double railThickE   = 14.7*CLHEP::mm;
//
    double suppLength   = 6800.0*CLHEP::mm;
    double suppWidth    = 54.*CLHEP::mm;
//    double suppThick    = 22.6*CLHEP::mm;
//
//   Database 
//
    IRDBRecordset_ptr atls      = rdbAccessSvc()->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
    IRDBRecordset_ptr railrec   = rdbAccessSvc()->getRecordsetPtr("IDRails", indetVersionKey.tag(), indetVersionKey.node());
    IRDBRecordset_ptr endplate  = rdbAccessSvc()->getRecordsetPtr("EPShell", indetVersionKey.tag(), indetVersionKey.node());
    IRDBRecordset_ptr cage      = rdbAccessSvc()->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());
    IRDBRecordset_ptr idSupportRails  = rdbAccessSvc()->getRecordsetPtr("IdSupportRail", larVersionKey.tag(), larVersionKey.node());

//    const GeoMaterial*  steel = materialManager()->getMaterial("std::Iron");
//    const GeoMaterial*  alum  = materialManager()->getMaterial("std::Aluminium");
    const GeoMaterial*  steel = materialManager()->getMaterial((*railrec)[0]->getString("MATERIAL"));
    const GeoMaterial*  alum  = materialManager()->getMaterial((*railrec)[0]->getString("MATSUP"));
    
//Radius of Squirrel cage
    double rminInt = (*cage)[0]->getDouble("RINGRMIN")*CLHEP::mm;
//Thick of U Shape Support
    IRDBQuery* queryUSP = rdbAccessSvc()->getQuery("IDDetRailUSP",indetVersionKey.tag(), indetVersionKey.node());
    if(!queryUSP)
      queryUSP = rdbAccessSvc()->getQuery("IDDetRailUSP","IDDetRailUSP-00");

    queryUSP->setOrder("USP_ID");
    queryUSP->execute();
 
    queryUSP->next();
    double yWidthUSP1 = queryUSP->data<double>("IDDETRAILUSP_DATA.YWIDTH");//SquirrelCageFactory.cxx - not in database
    queryUSP->next();
    double xDepthUSP2 = queryUSP->data<double>("IDDETRAILUSP_DATA.XDEPTH");
    queryUSP->finalize();
    delete queryUSP;

    double epsilon = 0.01;  // +Some safety margin
 
     RMAX_ID = (*atls)[0]->getDouble("IDETOR")*CLHEP::cm;  
     //railLengthB = (*railrec)[0]->getDouble("LENGTHB")*CLHEP::mm; At database there is 34.7 but it could cause crash.

     railWidthB  = (*railrec)[0]->getDouble("WIDTHB")*CLHEP::mm;
     railThickB  = (*railrec)[0]->getDouble("THICKB")*CLHEP::mm;
  //------------ Limited by EP ExternalShell
     railLengthE = (*endplate)[0]->getDouble("ZSTART")*CLHEP::mm  
                  +(*endplate)[0]->getDouble("ZSHIFT")*CLHEP::mm
                  +(*endplate)[0]->getDouble("ZGAP")*CLHEP::mm   - railLengthB/2.;
//     railWidthE  = (*railrec)[0]->getDouble("WIDTHE")*CLHEP::mm;
//     railThickE  = (*railrec)[0]->getDouble("THICKE")*CLHEP::mm;
 
     suppLength = railLengthB + 2.*railLengthE;
     suppWidth  = (*railrec)[0]->getDouble("WIDTHSUP")*CLHEP::mm;
//     suppThick  = (*railrec)[0]->getDouble("THICKSUP")*CLHEP::mm;

     double zLengthB   =  (*idSupportRails)[0]->getDouble("ZLENGTH");
     double yWidthB    =  (*idSupportRails)[0]->getDouble("YWIDTH");
     double zLengthE   =  (suppLength - zLengthB)/2.;

     IRDBRecordset_ptr commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon",indetVersionKey.tag(), indetVersionKey.node());
     if(commonParameters->size()==0)
       commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon","IDDetRailCommon-00");
     
     double railEBx = (*commonParameters)[0]->getDouble("RAILEBX");
     double railEBy = (*commonParameters)[0]->getDouble("RAILEBY");
 
     IRDBQuery* querySP = rdbAccessSvc()->getQuery("IDDetRailSP",indetVersionKey.tag(), indetVersionKey.node());
     if(!querySP)
       querySP = rdbAccessSvc()->getQuery("IDDetRailSP","IDDetRailSP-00");
     
     querySP->setOrder("SP_ID");
     querySP->execute();

     querySP->next();
     double railSP1x = querySP->data<double>("IDDETRAILSP_DATA.SP_X");
     double railSP1y = querySP->data<double>("IDDETRAILSP_DATA.SP_Y");
     querySP->next();
     double railSP2x = querySP->data<double>("IDDETRAILSP_DATA.SP_X");
     double railSP2y = querySP->data<double>("IDDETRAILSP_DATA.SP_Y");
     querySP->next();
     double railSP3x = querySP->data<double>("IDDETRAILSP_DATA.SP_X");
     double railSP3y = querySP->data<double>("IDDETRAILSP_DATA.SP_Y");

     querySP->finalize();
     delete querySP;

     double exactPl = (*commonParameters)[0]->getDouble("EXACTPL");
     double gapOfRSF = (*commonParameters)[0]->getDouble("GAPOFRSF");

//Parameters of Barrel Tracker Support Rail  
     IRDBQuery* querySRBP = rdbAccessSvc()->getQuery("IDDetRailSRBP",indetVersionKey.tag(), indetVersionKey.node());
     if(!querySRBP)
       querySRBP = rdbAccessSvc()->getQuery("IDDetRailSRBP","IDDetRailSRBP-00");
     
     querySRBP->setOrder("SRBP_ID");
     querySRBP->execute();

     querySRBP->next();
     double trackerSRBP1x = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_X");
     double trackerSRBP1y = yWidthB;
     querySRBP->next();
     double trackerSRBP2x = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_X");
     double trackerSRBP2y = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_Y");
     querySRBP->next();
     double trackerSRBP3x = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_X");
     double trackerSRBP3y = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_Y");
     querySRBP->next();
     double trackerSRBP4x = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_X");
     double trackerSRBP4y = querySRBP->data<double>("IDDETRAILSRBP_DATA.SRBP_Y");

     querySRBP->finalize();
     delete querySRBP;

     double distBottom = (*commonParameters)[0]->getDouble("DISTBOTTOM");
     double distTop    = (*commonParameters)[0]->getDouble("DISTTOP");

//Parameters of Endcap Tracker Support Rail
     IRDBQuery* querySREP = rdbAccessSvc()->getQuery("IDDetRailSREP",indetVersionKey.tag(), indetVersionKey.node());
     if(!querySREP)
       querySREP = rdbAccessSvc()->getQuery("IDDetRailSREP","IDDetRailSREP-00");
     
     querySREP->setOrder("SREP_ID");
     querySREP->execute();
     querySREP->next();
     double trackerSREP1x = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_X");
     double trackerSREP1y = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_Y");
     querySREP->next();
     double trackerSREP2x = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_X");
     double trackerSREP2y = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_Y");
     querySREP->next();
     double trackerSREP3x = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_X");
     double trackerSREP3y = querySREP->data<double>("IDDETRAILSREP_DATA.SREP_Y");
     querySREP->finalize();
     delete querySREP;

//
// To avoid rail corner outside ID envelope
     RMAX_ID = sqrt(RMAX_ID*RMAX_ID-suppWidth*suppWidth/4.)-epsilon;  

//--------------------------------------------------------------------------------
//Rail Barrel
    const GeoBox* railBarrel = new GeoBox(railThickB/2., railWidthB/2., railLengthB/2.);

    
//Rail Endcap
    const int numberOfPieces = 10;

    //GeoBox** railEndcap = new GeoBox*[numberOfPieces];
    GeoBox* railEndcap[numberOfPieces];
    //    CLHEP::Hep3Vector* vectorForTrans = new CLHEP::Hep3Vector[numberOfPieces];
    //    //CLHEP::Hep3Vector vectorForTrans[numberOfPieces];
    CLHEP::Hep3Vector vectorForTrans[numberOfPieces];

    GeoBox* railEndcapFirst = new GeoBox(railEBx/2., railEBy/2., railLengthE/2);
    CLHEP::Hep3Vector vectorForTransFirst (0., 0., 0.);

    double lastY = 0.;
    double startY = 0.;

    for (int i = 0; i < numberOfPieces; i++)
    {
     double xa = (railEBx/2.)/(numberOfPieces + 2) * (i+1);    
     double ya = xa - startY; //because of tangens of 45 degree
     startY += ya;

     //std::cerr << "xa: " << xa << ", ya:" << ya << std::endl;
     
     railEndcap[i] = new GeoBox(railEBx/2. - xa, ya/2, railLengthE/2);
     if (i == 0)
     {
       vectorForTrans[0] = CLHEP::Hep3Vector(0., railEBy/2. + ya/2, 0.) + vectorForTransFirst;
       lastY = ya/2;
     }
     else
     {
       vectorForTrans[i] = vectorForTrans[i-1] + CLHEP::Hep3Vector(0., lastY + ya/2, 0.);
       lastY = ya/2;
     }

    }
//RailSupport
    const GeoShape* elementOfRailSupport1 = new GeoBox(railSP1x/2., railSP1y/2., suppLength/2);
    const GeoShape* elementOfRailSupport2 = new GeoBox(railSP2x/2., railSP2y/2., suppLength/2);
    const GeoShape* elementOfRailSupport3 = new GeoBox(railSP3x/2., railSP3y/2., suppLength/2);

    CLHEP::Hep3Vector transRailSupport1(0., 0., 0.);
    CLHEP::Hep3Vector transRailSupport2Part1(-railSP1x/2. + railSP2x/2., railSP1y/2. + railSP2y/2. ,0.);
    CLHEP::Hep3Vector transRailSupport3Part1(-railSP1x/2. - railSP3x/2., railSP1y/2. + railSP2y - railSP3y/2. , 0.);
    
    CLHEP::Hep3Vector transRailSupport2Part2(railSP1x/2. - railSP2x/2., railSP1y/2. + railSP2y/2. ,0.);
    CLHEP::Hep3Vector transRailSupport3Part2(railSP1x/2. + railSP3x/2., railSP1y/2. + railSP2y - railSP3y/2. ,0.);


//Definition of Barrel Tracker Support Rail
    const GeoBox* trackerSRBP1 = new GeoBox(trackerSRBP1x/2., trackerSRBP1y/2., zLengthB/2);
    const GeoBox* trackerSRBP2 = new GeoBox(trackerSRBP2x/2., trackerSRBP2y/2., zLengthB/2);
    const GeoBox* trackerSRBP3 = new GeoBox(trackerSRBP3x/2., trackerSRBP3y/2., zLengthB/2);
    const GeoBox* trackerSRBP4 = new GeoBox(trackerSRBP4x/2., trackerSRBP4y/2., zLengthB/2);

    CLHEP::Hep3Vector trans1TSRBP1(0., 0.,0.);
    CLHEP::Hep3Vector trans2TSRBP1(0., 0.,0.);

    CLHEP::Hep3Vector trans1TSRBP2(trackerSRBP1x/2. + trackerSRBP2x/2., - (trackerSRBP1y/2. - trackerSRBP2y/2. - distBottom), 0.);
    CLHEP::Hep3Vector trans2TSRBP2(-trackerSRBP1x/2. - trackerSRBP2x/2., - (trackerSRBP1y/2. - trackerSRBP2y/2. - distBottom), 0.);
    
    CLHEP::Hep3Vector trans1TSRBP3(trackerSRBP1x/2. + trackerSRBP2x + trackerSRBP3x/2., - (trackerSRBP1y/2. - trackerSRBP3y/2. - distBottom), 0.);
    CLHEP::Hep3Vector trans2TSRBP3(- trackerSRBP1x/2. - trackerSRBP2x - trackerSRBP3x/2., - (trackerSRBP1y/2. - trackerSRBP3y/2. - distBottom), 0.);

    CLHEP::Hep3Vector trans1TSRBP4(trackerSRBP1x/2. + trackerSRBP2x + trackerSRBP4x/2., trackerSRBP1y/2. - trackerSRBP4y/2. - distTop, 0.);
    CLHEP::Hep3Vector trans2TSRBP4(- trackerSRBP1x/2. - trackerSRBP2x - trackerSRBP4x/2.,trackerSRBP1y/2. - trackerSRBP4y/2. - distTop, 0.);

//Definition Of Endcap Tracker Support Rail

    const GeoBox* trackerSREP1 = new GeoBox(trackerSREP1x/2., trackerSREP1y/2.,zLengthE/2.);
    const GeoBox* trackerSREP2 = new GeoBox(trackerSREP2x/2., trackerSREP2y/2.,zLengthE/2.); 
    const GeoBox* trackerSREP3 = new GeoBox(trackerSREP3x/2., trackerSREP3y/2.,zLengthE/2.);

    CLHEP::Hep3Vector trans1TSREP1(0. ,0.,0.);
    CLHEP::Hep3Vector trans2TSREP1(0. ,0.,0.);
  
    CLHEP::Hep3Vector trans1TSREP2(trackerSREP1x/2. + trackerSREP2x/2., -trackerSREP1y/2. + trackerSREP2y/2., 0.);
    CLHEP::Hep3Vector trans2TSREP2(- trackerSREP1x/2. - trackerSREP2x/2., -trackerSREP1y/2. + trackerSREP2y/2., 0.);

    CLHEP::Hep3Vector trans1TSREP3(trackerSREP1x/2. + trackerSREP3x/2., trackerSREP1y/2. - trackerSREP3y/2., 0.);
    CLHEP::Hep3Vector trans2TSREP3(- trackerSREP1x/2. - trackerSREP3x/2., trackerSREP1y/2. - trackerSREP3y/2., 0.);




//Logical  and Physical Volume
    const GeoLogVol* railBarrelLog = new GeoLogVol("railBarrel",railBarrel,steel);
    const GeoLogVol* railSupportLogPart1 = new GeoLogVol("RailSupportPart1", elementOfRailSupport1, alum);
    const GeoLogVol* railSupportLogPart2 = new GeoLogVol("RailSupportPart2", elementOfRailSupport2, alum);
    const GeoLogVol* railSupportLogPart3 = new GeoLogVol("RailSupportPart3", elementOfRailSupport3, alum);

    const GeoLogVol* railEndcapFirstLog = new GeoLogVol("railEndcap", railEndcapFirst, steel);
    
    GeoVPhysVol* railEndcapFirstPhys = new GeoPhysVol(railEndcapFirstLog); 

    //GeoLogVol** railEndcapLog = new GeoLogVol*[numberOfPieces];
    //GeoVPhysVol** railEndcapPhys = new GeoVPhysVol*[numberOfPieces];
    GeoLogVol* railEndcapLog[numberOfPieces];
    GeoVPhysVol* railEndcapPhys[numberOfPieces];
 
    for (int i = 0; i < numberOfPieces; i++)
    {
     railEndcapLog[i] = new GeoLogVol("railEndcap", railEndcap[i], steel);
     railEndcapPhys[i] = new GeoPhysVol(railEndcapLog[i]);
    }


    GeoVPhysVol* railBarrelPhys = new GeoPhysVol(railBarrelLog);

    GeoVPhysVol* railSupportPhysPart1 = new GeoPhysVol(railSupportLogPart1);
    GeoVPhysVol* railSupportPhysPart2 = new GeoPhysVol(railSupportLogPart2);
    GeoVPhysVol* railSupportPhysPart3 = new GeoPhysVol(railSupportLogPart3);


    const GeoLogVol* trackerSRBLP1 = new GeoLogVol("TrackerSupportRailBarrelPart1", trackerSRBP1, alum);
    const GeoLogVol* trackerSRBLP2 = new GeoLogVol("TrackerSupportRailBarrelPart2", trackerSRBP2, alum);
    const GeoLogVol* trackerSRBLP3 = new GeoLogVol("TrackerSupportRailBarrelPart3", trackerSRBP3, alum);
    const GeoLogVol* trackerSRBLP4 = new GeoLogVol("TrackerSupportRailBarrelPart3", trackerSRBP4, alum);

    const GeoLogVol* trackerSRELP1 = new GeoLogVol("TrackerSupportRailEndPart1", trackerSREP1, alum);
    const GeoLogVol* trackerSRELP2 = new GeoLogVol("TrackerSupportRailEndPart2", trackerSREP2, alum);
    const GeoLogVol* trackerSRELP3 = new GeoLogVol("TrackerSupportRailEndPart3", trackerSREP3, alum);

    GeoVPhysVol* trackerSRBPP1 = new GeoPhysVol(trackerSRBLP1);
    GeoVPhysVol* trackerSRBPP2 = new GeoPhysVol(trackerSRBLP2);
    GeoVPhysVol* trackerSRBPP3 = new GeoPhysVol(trackerSRBLP3);
    GeoVPhysVol* trackerSRBPP4 = new GeoPhysVol(trackerSRBLP4);
      
    GeoVPhysVol* trackerSREPP1 = new GeoPhysVol(trackerSRELP1);
    GeoVPhysVol* trackerSREPP2 = new GeoPhysVol(trackerSRELP2);
    GeoVPhysVol* trackerSREPP3 = new GeoPhysVol(trackerSRELP3);

//Positioning and addig to geometry

    GeoTransform* pos;
//It is need because we want to put Support Rail, Barrel Rail, etc into "U" shape support - SquirrelCageFactory.cxx    
    double coordY = yWidthUSP1/2. - railSP1y - railSP2y + (railSP3y - gapOfRSF/2.);

//Support rail - male part

    CLHEP::Hep3Vector suppPos1Part1(rminInt + xDepthUSP2 + railSP2x - railSP1x/2., - yWidthUSP1/2. + railSP1y/2. + coordY, 0.);
    CLHEP::Hep3Vector suppPos2Part1( - rminInt - xDepthUSP2 - railSP2x + railSP1x/2., - yWidthUSP1/2. + railSP1y/2. + coordY, 0.);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1Part1));
    mother->add(pos);
    mother->add(railSupportPhysPart1);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2Part1));
    mother->add(pos);
    mother->add(railSupportPhysPart1);
   
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1Part1 + transRailSupport2Part2));
    mother->add(pos);
    mother->add(railSupportPhysPart2);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2Part1 + transRailSupport2Part1));
    mother->add(pos);
    mother->add(railSupportPhysPart2);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1Part1+ transRailSupport3Part2));
    mother->add(pos);
    mother->add(railSupportPhysPart3);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2Part1+ transRailSupport3Part1));
    mother->add(pos);
    mother->add(railSupportPhysPart3);



//
//    Barrel steel rails
//
//    double safety=0.01; // to provide safety gap for G4


    CLHEP::Hep3Vector railBpos1(railSP1x/2. - railThickB/2. - railSP2x, railSP1y/2. + railWidthB/2., 0.);
    CLHEP::Hep3Vector railBpos2(- railSP1x/2. + railThickB/2. + railSP2x, railSP1y/2. + railWidthB/2., 0.);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos1 + suppPos1Part1));
    mother->add(pos);
    mother->add(railBarrelPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos2 + suppPos2Part1));
    mother->add(pos);
    mother->add(railBarrelPhys);
//
//    Endcap steel rails
//

    CLHEP::Hep3Vector railEpos1( railSP1x/2 + railSP3x - exactPl, railSP1y/2. + railEBy/2., (railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos2( -railSP1x/2 - railSP3x + exactPl, railSP1y/2. + railEBy/2., (railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos3( railSP1x/2 + railSP3x - exactPl, railSP1y/2. + railEBy/2.,-(railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos4( -railSP1x/2 - railSP3x + exactPl, railSP1y/2. + railEBy/2.,-(railLengthB+railLengthE)/2.);

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos1 + vectorForTransFirst + suppPos1Part1));
    mother->add(pos);
    mother->add(railEndcapFirstPhys);
   
    for (int i = 0; i < numberOfPieces; i++)
    {
     pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(), railEpos1 + vectorForTrans[i] + suppPos1Part1));
     mother->add(pos);
     mother->add(railEndcapPhys[i]);
    }

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos2 + vectorForTransFirst + suppPos2Part1));
    mother->add(pos);
    mother->add(railEndcapFirstPhys);

    for (int i = 0; i < numberOfPieces; i++)
    {
     pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(), railEpos2 + vectorForTrans[i] + suppPos2Part1));
     mother->add(pos);
     mother->add(railEndcapPhys[i]);
    }

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos3 + vectorForTransFirst + suppPos1Part1));
    mother->add(pos);
    mother->add(railEndcapFirstPhys);

    for (int i = 0; i < numberOfPieces; i++)
    {
     pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(), railEpos3 + vectorForTrans[i] + suppPos1Part1));
     mother->add(pos);
     mother->add(railEndcapPhys[i]);
    }

    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos4 + vectorForTransFirst + suppPos2Part1));
    mother->add(pos);
    mother->add(railEndcapFirstPhys);

    for (int i = 0; i < numberOfPieces; i++)
    {
     pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(), railEpos4 + vectorForTrans[i] + suppPos2Part1));
     mother->add(pos);
     mother->add(railEndcapPhys[i]);
    }

    CLHEP::Hep3Vector transToInnerWall(rminInt + xDepthUSP2 + railSP2x + railSP3x + trackerSRBP2x + trackerSRBP1x, 0, 0);
    CLHEP::Hep3Vector transMalePart(0., 0., 0.);
    GeoTransform *xfPos;
    GeoTransform *xfNeg;

//Barrel           
    xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),transToInnerWall - CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans2TSRBP1 + transMalePart));
    xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),- transToInnerWall + CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans1TSRBP1 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSRBPP1);
     mother->add(xfNeg);
     mother->add(trackerSRBPP1);

     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(),transToInnerWall - CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans2TSRBP2 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans1TSRBP2 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSRBPP2);
     mother->add(xfNeg);
     mother->add(trackerSRBPP2);
  
     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans2TSRBP3 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans1TSRBP3 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSRBPP3);
     mother->add(xfNeg);
     mother->add(trackerSRBPP3);

     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans2TSRBP4 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), -transToInnerWall + CLHEP::Hep3Vector(trackerSRBP1x/2., 0, 0) + trans1TSRBP4 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSRBPP4);
     mother->add(xfNeg);
     mother->add(trackerSRBPP4);
//Endcap positiv

     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, zLengthB/2 + zLengthE/2) + trans2TSREP1 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, zLengthB/2 + zLengthE/2) + trans1TSREP1 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP1);
     mother->add(xfNeg);
     mother->add(trackerSREPP1);

     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, zLengthB/2 + zLengthE/2) + trans2TSREP2 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, zLengthB/2 + zLengthE/2) + trans1TSREP2 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP2);
     mother->add(xfNeg);
     mother->add(trackerSREPP2);
  
     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, zLengthB/2 + zLengthE/2) + trans2TSREP3 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, zLengthB/2 + zLengthE/2) + trans1TSREP3 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP3);
     mother->add(xfNeg);
     mother->add(trackerSREPP3);       
//Endcap negativ
    
     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, - zLengthB/2 - zLengthE/2) + trans2TSREP1 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, - zLengthB/2 - zLengthE/2) + trans1TSREP1 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP1);
     mother->add(xfNeg);
     mother->add(trackerSREPP1);

     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, - zLengthB/2 - zLengthE/2) + trans2TSREP2 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, - zLengthB/2 - zLengthE/2) + trans1TSREP2 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP2);
     mother->add(xfNeg);
     mother->add(trackerSREPP2);
  
     xfPos = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), transToInnerWall - CLHEP::Hep3Vector(trackerSREP1x/2., 1.5, - zLengthB/2 - zLengthE/2) + trans2TSREP3 + transMalePart));
     xfNeg = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - transToInnerWall + CLHEP::Hep3Vector(trackerSREP1x/2., -1.5, - zLengthB/2 - zLengthE/2) + trans1TSREP3 + transMalePart));
     mother->add(xfPos);
     mother->add(trackerSREPP3);
     mother->add(xfNeg);
     mother->add(trackerSREPP3);      
   
  }
  else {
    // ______________________________________________________________________________________________________________________
    // Geometry without ID rail
//  std::cout <<" Version!!="<<atlasVersionKey.tag()<<", "<<atlasVersionKey.node()<<'\n';
//  std::cout <<" Version!!="<<indetVersionKey.tag()<<", "<<indetVersionKey.node()<<'\n';
//---------------------------------------------------------------------------------
//     Long ribs (initial position is along X axis then move and rotate)
//
//      Th
//      ___
//     |  |
//     |  |  Wid
//     |  |
//     |__|
//
// Default(initial) values
//    
    double RMAX_ID      = 1150.0*CLHEP::mm -1.0*CLHEP::mm;   //Some safety margin
    double railLengthB  = 1600.0*CLHEP::mm;
    double railWidthB   = 5.5*CLHEP::mm;
    double railThickB   = 34.7*CLHEP::mm;
//
    double railLengthE  = 2600.0*CLHEP::mm;
    double railWidthE   = 14.*CLHEP::mm;
    double railThickE   = 14.7*CLHEP::mm;
//
    double suppLength   = 6800.0*CLHEP::mm;
    double suppWidth    = 54.*CLHEP::mm;
    double suppThick    = 22.6*CLHEP::mm;
//
//   Database 
//
    IRDBRecordset_ptr atls      = rdbAccessSvc()->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
    IRDBRecordset_ptr railrec   = rdbAccessSvc()->getRecordsetPtr("IDRails", indetVersionKey.tag(), indetVersionKey.node());
    IRDBRecordset_ptr endplate  = rdbAccessSvc()->getRecordsetPtr("EPShell", indetVersionKey.tag(), indetVersionKey.node());


//    const GeoMaterial*  steel = materialManager()->getMaterial("std::Iron");
//    const GeoMaterial*  alum  = materialManager()->getMaterial("std::Aluminium");
    const GeoMaterial*  steel = materialManager()->getMaterial((*railrec)[0]->getString("MATERIAL"));
    const GeoMaterial*  alum  = materialManager()->getMaterial((*railrec)[0]->getString("MATSUP"));

//  std::cout << "Rail Steel density="<<steel->getDensity()<<" Rad.length="<<steel->getRadLength()<<'\n';
//  std::cout << "Rail Aluminium density="<<alum->getDensity()<<" Rad.length="<<alum->getRadLength()<<'\n';




    
    double epsilon = 0.01;  // +Some safety margin
 
     RMAX_ID = (*atls)[0]->getDouble("IDETOR")*CLHEP::cm;  
     railLengthB = (*railrec)[0]->getDouble("LENGTHB")*CLHEP::mm;
     railWidthB  = (*railrec)[0]->getDouble("WIDTHB")*CLHEP::mm;
     railThickB  = (*railrec)[0]->getDouble("THICKB")*CLHEP::mm;
  //------------ Limited by EP ExternalShell
     railLengthE = (*endplate)[0]->getDouble("ZSTART")*CLHEP::mm  
                  +(*endplate)[0]->getDouble("ZSHIFT")*CLHEP::mm
                  +(*endplate)[0]->getDouble("ZGAP")*CLHEP::mm   - railLengthB/2.;
     railWidthE  = (*railrec)[0]->getDouble("WIDTHE")*CLHEP::mm;
     railThickE  = (*railrec)[0]->getDouble("THICKE")*CLHEP::mm;
 
     suppLength = railLengthB + 2.*railLengthE;
     suppWidth  = (*railrec)[0]->getDouble("WIDTHSUP")*CLHEP::mm;
     suppThick  = (*railrec)[0]->getDouble("THICKSUP")*CLHEP::mm;
//
// To avoid rail corner outside ID envelope
     RMAX_ID = sqrt(RMAX_ID*RMAX_ID-suppWidth*suppWidth/4.)-epsilon;  

//--------------------------------------------------------------------------------

    const GeoBox* railBarrel = new GeoBox(railThickB/2., railWidthB/2., railLengthB/2.);
    const GeoBox* railEndcap = new GeoBox(railThickE/2., railWidthE/2., railLengthE/2.);
    const GeoBox* support    = new GeoBox(suppThick/2.,  suppWidth/2.,  suppLength/2.);

    const GeoLogVol* railBarrelLog = new GeoLogVol("railBarrel",railBarrel,steel);
    const GeoLogVol* railEndcapLog = new GeoLogVol("railEndcap",railEndcap,steel);
    const GeoLogVol* supportLog    = new GeoLogVol("RailSupport",support,alum);
    GeoVPhysVol* railBarrelPhys = new GeoPhysVol(railBarrelLog);
    GeoVPhysVol* railEndcapPhys = new GeoPhysVol(railEndcapLog);
    GeoVPhysVol* supportPhys = new GeoPhysVol(supportLog);


    GeoTransform* pos;
//
//    Barrel steel rails
//
    double safety=0.01; // to provide safety gap for G4

    CLHEP::Hep3Vector railBpos1( ( RMAX_ID-suppThick-railThickB/2.-safety), 0., 0.);
    CLHEP::Hep3Vector railBpos2( (-RMAX_ID+suppThick+railThickB/2.+safety), 0., 0.);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos1));
    mother->add(pos);
    mother->add(railBarrelPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railBpos2));
    mother->add(pos);
    mother->add(railBarrelPhys);
//
//    Endcap steel rails
//
    CLHEP::Hep3Vector railEpos1( ( RMAX_ID-suppThick-railThickE/2.-safety), 0., (railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos2( (-RMAX_ID+suppThick+railThickE/2.+safety), 0., (railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos3( ( RMAX_ID-suppThick-railThickE/2.-safety), 0.,-(railLengthB+railLengthE)/2.);
    CLHEP::Hep3Vector railEpos4( (-RMAX_ID+suppThick+railThickE/2.+safety), 0.,-(railLengthB+railLengthE)/2.);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos1));
    mother->add(pos);
    mother->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos2));
    mother->add(pos);
    mother->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos3));
    mother->add(pos);
    mother->add(railEndcapPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),railEpos4));
    mother->add(pos);
    mother->add(railEndcapPhys);
//
//    Rail supports
//
    
    CLHEP::Hep3Vector suppPos1( ( RMAX_ID-suppThick/2.), 0., 0.);
    CLHEP::Hep3Vector suppPos2( (-RMAX_ID+suppThick/2.), 0., 0.);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos1));
    mother->add(pos);
    mother->add(supportPhys);
    pos = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),suppPos2));
    mother->add(pos);
    mother->add(supportPhys);

  }
}



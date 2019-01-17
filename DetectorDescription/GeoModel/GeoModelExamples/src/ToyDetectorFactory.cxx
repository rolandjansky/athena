/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ToyDetectorFactory.h"
#include "CentralScrutinizer.h"  
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
using namespace GeoGenfun;
using namespace GeoXF;





ToyDetectorFactory::ToyDetectorFactory(StoreGateSvc *detStore)
  :m_detectorManager(NULL),
   m_detectorStore(detStore)
{
}


ToyDetectorFactory::~ToyDetectorFactory()
{

}



//## Other Operations (implementation)
void ToyDetectorFactory::create(GeoPhysVol *world)
{
  m_detectorManager=new ToyDetectorManager();

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return; 
  } 
  

  //-----------------------------------------------------------------------------------//  
  // Get the materials that we shall use.                                              //  
  // ----------------------------------------------------------------------------------//  
  
  const GeoMaterial *air        = materialManager->getMaterial("std::Air");
  const GeoMaterial *poly       = materialManager->getMaterial("std::Polystyrene");

  //-----------------------------------------------------------------------------------//  
  // Next make the box that describes the shape of the toy volume:                     //  
  //                                                                                   //  
  const GeoBox      *toyBox    = new GeoBox(800*Gaudi::Units::cm,800*Gaudi::Units::cm, 1000*Gaudi::Units::cm);                   //
  //                                                                                   //  
  // Bundle this with a material into a logical volume:                                //  
  //                                                                                   //  
  const GeoLogVol   *toyLog    = new GeoLogVol("ToyLog", toyBox, air);                 //  
  //                                                                                   //  
  // ..And create a physical volume:                                                   //  
  //                                                                                   //  
  GeoPhysVol        *toyPhys   = new GeoPhysVol(toyLog);                               //  
  //                                                                                   //  
  // Add this to the list of top level physical volumes:                               //  
  //                                                                                   //  
  m_detectorManager->addTreeTop(toyPhys);                                                //  
  //-----------------------------------------------------------------------------------//  
  
  //-----------------------------------------------------------------------------------//  
  // Daughters                                                                         //  
  //                                                                                   //  
  //                                                                                   //  
  const GeoTube     *ringTube  = new GeoTube(500*Gaudi::Units::cm, 1000*Gaudi::Units::cm, 5.0*Gaudi::Units::cm);                 //
  //                                                                                   //  
  // Bundle this with a material into a logical volume:                                //  
  //                                                                                   //  
  const GeoLogVol   *ringLog  = new  GeoLogVol("RingLog", ringTube, air);              //  
  //                                                                                   //  
  // Make 100 of these within the volume of the toy:                                   //  
  //                                                                                   //  
  GeoSerialDenominator       *ringName = new GeoSerialDenominator("RING");             //  
  toyPhys->add(ringName);                                                              //  
  for (int i=0;i<100;i++) {                                                            //  
    GeoFullPhysVol         *ringPhys = new GeoFullPhysVol(ringLog);                    //  
    GeoAlignableTransform  *xform    = new GeoAlignableTransform(GeoTrf::TranslateZ3D((i-50)*20*Gaudi::Units::cm));
    toyPhys->add(xform);                                                               //  
    toyPhys->add(ringPhys);                                                            //  
    m_detectorManager->addCentralScrutinizer(new CentralScrutinizer(ringPhys));          //  
  }                                                                                    //
  //-----------------------------------------------------------------------------------//


  //-----------------------------------------------------------------------------------//
  //  Now, in addition to active daughters add some passive material.  This is done    //
  //  here using the "SerialTransformer," our way of parameterizing volumes.  It       //
  //  does not need to be done this way.. but we want to provide an example of         //
  //  parametrizations in the Toy                                                      //
  //-----------------------------------------------------------------------------------//

  GeoBox       *sPass = new GeoBox(5.0*Gaudi::Units::cm, 30*Gaudi::Units::cm, 30*Gaudi::Units::cm);
  GeoLogVol    *lPass = new GeoLogVol("Passive", sPass, poly);
  GeoPhysVol   *pPass = new GeoPhysVol(lPass);

  GeoBox       *sIPass = new GeoBox(4*Gaudi::Units::cm, 25*Gaudi::Units::cm, 25*Gaudi::Units::cm);
  GeoLogVol    *lIPass = new GeoLogVol("InnerPassive", sIPass, air);
  GeoPhysVol   *pIPass = new GeoPhysVol(lIPass);
  
  pPass->add(pIPass);
  
  const unsigned int NPLATES=100;
  Variable       i;
  Sin            sin;
  GENFUNCTION    f = 360*Gaudi::Units::deg/NPLATES*i;
  GENFUNCTION    g = sin(4*f);
  GENFUNCTION    h = -g;
  TRANSFUNCTION t1 = Pow(GeoTrf::RotateZ3D(1.0),f)*GeoTrf::TranslateX3D(1100*Gaudi::Units::cm)*Pow(GeoTrf::TranslateZ3D(800*Gaudi::Units::cm),g);
  TRANSFUNCTION t2 = Pow(GeoTrf::RotateZ3D(1.0),f)*GeoTrf::TranslateX3D(1100*Gaudi::Units::cm)*Pow(GeoTrf::TranslateZ3D(800*Gaudi::Units::cm),h);

  //-----------------------------------------------------------------------------------//
  // Inside, by the way, the serial transformer will evaluate the functions:           //
  // HepTransform3D xf = t1(i), for i=1,NPLATES....                                    //
  //-----------------------------------------------------------------------------------//

  GeoSerialDenominator  *pass1Name = new GeoSerialDenominator("PASSIVE-1-");      
  GeoSerialTransformer *s1 = new GeoSerialTransformer(pPass,&t1, NPLATES);
  toyPhys->add(pass1Name);
  toyPhys->add(s1);

  GeoSerialDenominator *pass2Name = new GeoSerialDenominator("PASSIVE-2-");      
  GeoSerialTransformer *s2 = new GeoSerialTransformer(pPass,&t2, NPLATES);
  toyPhys->add(pass2Name);
  toyPhys->add(s2);


  //------------------------------------------------------------------------------------//
  // Now insert all of this into the world...                                           //
  GeoNameTag *tag = new GeoNameTag("Toy");                                              //
  world->add(tag);                                                                      //
  world->add(toyPhys);                                                                  //
  //------------------------------------------------------------------------------------//
}

const ToyDetectorManager * ToyDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}






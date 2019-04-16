/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelExamples/SimplestToyDetectorFactory.h"
#include "CentralScrutinizer.h"
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
#include "GeoModelKernel/Units.h"

#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"

using namespace GeoGenfun;
using namespace GeoXF;


SimplestToyDetectorFactory::SimplestToyDetectorFactory(StoreGateSvc *detStore) // TODO: remove StoreGate reference
:m_detectorManager(NULL), m_detectorStore(detStore) {}


SimplestToyDetectorFactory::~SimplestToyDetectorFactory() {}



//## Other Operations (implementation)
void SimplestToyDetectorFactory::create(GeoPhysVol *world)
{
	m_detectorManager=new ToyDetectorManager();

	//-----------------------------------------------------------------------------------//
	// Get the materials that we shall use.                                              //
	// ----------------------------------------------------------------------------------//

	// Bogus densities.  Later: read from database.
	double densityOfAir=0.1, densityOfPolystyrene=0.2;
	const GeoMaterial *air        = new GeoMaterial("Air Two",densityOfAir);
	const GeoMaterial *poly       = new GeoMaterial("std::Polystyrene",densityOfPolystyrene);


	//-----------------------------------------------------------------------------------//
	// Next make the box that describes the shape of the toy volume:                     //
	const GeoBox      *toyBox    = new GeoBox(800*GeoModelKernelUnits::cm, 800*GeoModelKernelUnits::cm, 1000*GeoModelKernelUnits::cm);                   //
	// Bundle this with a material into a logical volume:                                //
	const GeoLogVol   *toyLog    = new GeoLogVol("ToyLog", toyBox, air);                 //
	// ..And create a physical volume:                                                   //
	GeoPhysVol        *toyPhys   = new GeoPhysVol(toyLog);                               //
	// Add this to the list of top level physical volumes:                               //
	m_detectorManager->addTreeTop(toyPhys);                                                //



	GeoBox       *sPass = new GeoBox(5.0*GeoModelKernelUnits::cm, 30*GeoModelKernelUnits::cm, 30*GeoModelKernelUnits::cm);
	GeoLogVol    *lPass = new GeoLogVol("Passive", sPass, poly);
	GeoPhysVol   *pPass = new GeoPhysVol(lPass);

	GeoBox       *sIPass = new GeoBox(4*GeoModelKernelUnits::cm, 25*GeoModelKernelUnits::cm, 25*GeoModelKernelUnits::cm);
	GeoLogVol    *lIPass = new GeoLogVol("InnerPassive", sIPass, air);
	GeoPhysVol   *pIPass = new GeoPhysVol(lIPass);

	pPass->add(pIPass);
	toyPhys->add(pPass);



	//------------------------------------------------------------------------------------//
	// Now insert all of this into the world...                                           //
	GeoNameTag *tag = new GeoNameTag("Toy");                                              //
	world->add(tag);                                                                      //
	world->add(toyPhys);                                                                  //
	//------------------------------------------------------------------------------------//
}

const ToyDetectorManager * SimplestToyDetectorFactory::getDetectorManager() const
{
	return m_detectorManager;
}

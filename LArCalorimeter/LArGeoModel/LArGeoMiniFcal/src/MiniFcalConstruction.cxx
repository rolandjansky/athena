/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MiniFcalConstruction

// 19.Aug 2008 M.Fincke  -  First simple description of a possible MiniFcal 

#include "LArGeoMiniFcal/MiniFcalConstruction.h"

#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoBox.h"  

#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "CLHEP/GenericFunctions/Variable.hh"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>
#include <map>

typedef std::map<int,int> MapNumToIndex;

LArGeo::MiniFcalConstruction::MiniFcalConstruction(bool posZSide)
  : m_physiMiniFcal(0)
  , m_transform()
  , m_posZSide(posZSide)
{
}

LArGeo::MiniFcalConstruction::~MiniFcalConstruction()
{
}

GeoFullPhysVol* LArGeo::MiniFcalConstruction::GetEnvelope()
{
  // Get message service, GeoModelSvc and RDBAccessSvc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc(0);

  StatusCode sc = svcLocator->service("MessageSvc", msgSvc, true);
  if(sc!=StatusCode::SUCCESS) 
    throw std::runtime_error("Error in MiniFcalConstruction, cannot access MessageSvc");

  MsgStream log(msgSvc, "LArGeo::MiniFcalConstruction"); 
  log << MSG::DEBUG << "In MiniFcalConstruction GetEnvelope" << endreq;

  IRDBAccessSvc* pAccessSvc(0);
  sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::ERROR <<"Cannot locate RDBAccessSvc!!" << endreq;
    return 0;
  }

  IGeoModelSvc* geoModelSvc(0);
  sc = svcLocator->service ("GeoModelSvc",geoModelSvc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR <<"Cannot locate GeoModelSvc!!" << endreq;
    return 0;
  }
  
  StoreGateSvc* detStore(0);
  sc = svcLocator->service("DetectorStore", detStore, false);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR <<"Error in MiniFcalConstruction, cannot access DetectorStore" << endreq;
    return 0;
  }

  //_________ Get envelope parameters from the database ________________________
  std::string AtlasVersion = geoModelSvc->atlasVersion();
  std::string LArVersion = geoModelSvc->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

  IRDBRecordset_ptr recEnvelope = pAccessSvc->getRecordsetPtr("MiniFcalEnvelope",detectorKey,detectorNode);
  if(recEnvelope->size()==0) {
    log << MSG::ERROR << "Unable to get envelope parameters from the database" << endreq;
    return 0;
  }
  const IRDBRecord* envParameters = (*recEnvelope)[0];

  //_________ Get materials from the Material Manager ___________________________
  DataHandle<StoredMaterialManager> materialManager;
  sc = detStore->retrieve(materialManager, std::string("MATERIALS"));
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Unable to retrieve the Stored Material Manager" << endreq;
    return 0;
  }
  
  GeoMaterial* Copper  = materialManager->getMaterial(envParameters->getString("MATERIAL"));
  if(!Copper) {
    log << MSG::ERROR << "Error in MiniFcalConstruction, unable to find material for the envelope" << endreq;
    return 0;
  }

  GeoMaterial* Diamond  = materialManager->getMaterial("pix::Diamond");
  if(!Diamond) {
    log << MSG::ERROR << "Error in MiniFcalConstruction, unable to find Diamond material" << endreq;
    return 0;
  }

  GeoMaterial *Feldspar  = materialManager->getMaterial("std::Feldspar");
  if (!Feldspar) {
    log << MSG::ERROR << "Error in MiniFcalConstruction, unable to find material for the Ceramic Layers" << endreq;
    return 0;
  }

  //_________ Define geometry __________________________

  //__Copper envelope
  double halfLength = envParameters->getDouble("DZ")*CLHEP::mm; 
  double Router     = envParameters->getDouble("RMAX")*CLHEP::mm; 
  double Rinner     = envParameters->getDouble("RMIN")*CLHEP::mm;

  // Buld a Cu block and place layers into that...
  GeoTubs *solidMiniFcal = new GeoTubs(Rinner, Router, halfLength, 0., 2.*M_PI); // Big outer radius
  std::string moduleName = "MiniFCAL" ;
  GeoLogVol* logiMiniFcal = new GeoLogVol(moduleName, solidMiniFcal, Copper);
  m_physiMiniFcal = new GeoFullPhysVol(logiMiniFcal);


  //__Layers
  if(!pAccessSvc->getChildTag("MiniFcalLayers",detectorKey,detectorNode).empty()) {

    IRDBRecordset_ptr recCommon = pAccessSvc->getRecordsetPtr("MiniFcalCommon",detectorKey,detectorNode);
    if(recCommon->size()==0) {
      log << MSG::ERROR << "Unable to get MiniFcalCommon from the database" << endreq;
      return 0;
    }
    IRDBRecordset_ptr recLayers = pAccessSvc->getRecordsetPtr("MiniFcalLayers",detectorKey,detectorNode);
    if(recLayers->size()==0) {
      log << MSG::ERROR << "Unable to get MiniFcalLayers from the database" << endreq;
      return 0;
    }
    IRDBRecordset_ptr recRings = pAccessSvc->getRecordsetPtr("MiniFcalRings",detectorKey,detectorNode);
    if(recRings->size()==0) {
      log << MSG::ERROR << "Unable to get MiniFcalRings from the database" << endreq;
      return 0;
    }

    // Map layer and ring numbers to corresponding indexes in recordsets
    MapNumToIndex layerIndexes, ringIndexes;
    for(unsigned i=0; i<recLayers->size(); ++i)
      layerIndexes[(*recLayers)[i]->getInt("LAYERNUM")] = i;

    for(unsigned i=0; i<recRings->size(); ++i)
      ringIndexes[(*recRings)[i]->getInt("RINGNUM")] = i;

    double L1         =  (*recCommon)[0]->getDouble("ABSORBERTHICKNESS")*CLHEP::mm; // Cu plates of fixed thickness
    double LayerThick =  (*recCommon)[0]->getDouble("LAYERTHICKNESS")*CLHEP::mm;    // Layers between the Cu plates 
    double WaferThick =  (*recCommon)[0]->getDouble("WAFERTHICKNESS")*CLHEP::mm;   // Diamond wafers - thickness
    double WaferSize  =  (*recCommon)[0]->getDouble("WAFERSIZEX")*CLHEP::mm;    // Square Daimond wafers
    int    NLayers    =  (*recCommon)[0]->getInt("NLAYERS");      // Have 11 gaps and 12 Cu plates

    log << MSG::DEBUG << "=====> Build a Mini FCal of length  " << 2.*halfLength << " CLHEP::mm and " 
	<< NLayers << " layers of  " << LayerThick << " CLHEP::mm thickness each; place them every  "
	<< L1 << " CLHEP::mm " << endreq;

    // Make the Layers (all the same) - out of Feldspar (perhaps close to ceramics)
    std::string layerName = moduleName + "::Layer" ;
    GeoTubs *solidLayer = new GeoTubs(Rinner, Router, LayerThick/2., 0., 2.*M_PI); // Big outer radius
    GeoLogVol* logiLayer = new GeoLogVol(layerName, solidLayer, Feldspar);
    
    //-- Construct wafers and arrange them in rings inside the ceramic layers.
    std::string waferName = moduleName + "::Wafer" ;
    GeoBox* solidWafer = new GeoBox( (WaferSize/2.)*CLHEP::mm, (WaferSize/2.)*CLHEP::mm, (WaferThick/2.)*CLHEP::mm);
    GeoLogVol* logiWafer = new GeoLogVol(waferName,solidWafer,Diamond);
    GeoPhysVol* physiWafer = new GeoPhysVol(logiWafer);

    // use this to get the phi numbering right:
    double phisense = 1.;
    double activate = 0.;
    if (!m_posZSide) { phisense = -1.; activate = 1.; } 
    
    // Construct layers
    for(unsigned j=0; j<recLayers->size(); ++j) {
      // Check whether we have all layers available in the database
      if(layerIndexes.find(j)==layerIndexes.end()) {
	log << MSG::ERROR << "Wrong numbering of Layers. " << j << "  is missing" << endreq;
	logiLayer->ref(); logiLayer->unref();
	physiWafer->ref(); physiWafer->unref();
	m_physiMiniFcal->ref(); m_physiMiniFcal->unref();
	return 0;
      }

      GeoPhysVol* physiLayer = new GeoPhysVol(logiLayer);

      // Construct layer contents
      Genfun::Variable Index;
      double rwafer(0.);
      int nwafers(0);

      double phishift = (*recLayers)[layerIndexes[j]]->getDouble("PHISHIFT");
      double rshift = (*recLayers)[layerIndexes[j]]->getDouble("RSHIFT")*CLHEP::mm;

      for (unsigned int i=0; i<recRings->size(); i++){  // loop over the number of wafer rings
	if(ringIndexes.find(i)==ringIndexes.end()) {
	  log << MSG::ERROR << "Wrong numbering of Rings. " << i << "  is missing" << endreq;
	  physiLayer->ref(); physiLayer->unref();
	  physiWafer->ref(); physiWafer->unref();
	  m_physiMiniFcal->ref(); m_physiMiniFcal->unref();
	  return 0;
	}

	rwafer = (*recRings)[ringIndexes[i]]->getDouble("RINNER");
	nwafers = (*recRings)[ringIndexes[i]]->getInt("NWAFERS");

	GeoSerialIdentifier  *sIF = new GeoSerialIdentifier(i*100);
	log << MSG::DEBUG << "MiniFcal - Put " << nwafers << " wafers into Ring " << i << " now " << endreq;
	double wAngle = 2.*M_PI/nwafers;
      
	// for the negative z-side have to add pi to get things right:
	Genfun::GENFUNCTION RotationAngle = activate*(M_PI) + phisense * (phishift + wAngle/2. + wAngle*Index) ;
	GeoXF::TRANSFUNCTION t  = 
	  GeoXF::Pow(HepGeom::RotateZ3D(1.0),RotationAngle) * HepGeom::TranslateX3D(rshift+rwafer+5.*CLHEP::mm) * HepGeom::TranslateZ3D(-LayerThick/2.+ WaferThick/2.) ;
	GeoSerialTransformer *sTF = new GeoSerialTransformer (physiWafer,&t,nwafers);
	physiLayer->add(sIF);
	physiLayer->add(sTF);
      }

      log << MSG::DEBUG << "- Working on layer " << j << " now. Place it at " 
	  << ( -halfLength + L1 + double(j)*( L1 + LayerThick) + LayerThick/2. ) << " CLHEP::mm " << endreq;
      m_physiMiniFcal->add(new GeoIdentifierTag(j));        
      GeoTransform *xf = new GeoTransform(HepGeom::TranslateZ3D( -halfLength + L1 + double(j)*( L1 + LayerThick) + LayerThick/2. ));
      m_physiMiniFcal->add(xf);
      m_physiMiniFcal->add(physiLayer);
    }
  }


  //________ Construct top transform object _____________
  m_transform = HepGeom::TranslateZ3D(envParameters->getDouble("ZPOS")*CLHEP::mm);
  // Layers should be fully equipeed now. Put them into MiniFcal
 
  return m_physiMiniFcal;

}

const HepGeom::Transform3D&  LArGeo::MiniFcalConstruction::GetTopTransform()
{
  return m_transform;
}

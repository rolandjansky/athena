/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP12DGeometrySystems/StoreGateGeometryReader.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "TileDetDescr/TileDetDescrManager.h"

#include "LArReadoutGeometry/LArDetectorManager.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoTrd.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "GeoPrimitives/EulerAnglesHelpers.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"


StoreGateGeometryReader::StoreGateGeometryReader()
  : newDetector(0),
    m_pixel_manager(0),
    m_silicon_manager(0),
    m_trt_manager(0),
    m_calo_manager(0),
    m_tile_manager(0),
    m_lar_manager(0),
    m_detStore(0)
{}

const double StoreGateGeometryReader::m_smallAngle    = 0.05;
const double StoreGateGeometryReader::m_smallDistance = 100.;

//attempt to parse geometry for each system, return true if succes, false if failure
bool StoreGateGeometryReader::readGeometry()
{
  if (!m_detStore->retrieve(m_pixel_manager, "Pixel"))
    {
      std::cout << "Could not retrieve Pixel Manager!" << std::endl;
      return false;
    }
  else
    readPixelGeometry();

  if (!m_detStore->retrieve(m_silicon_manager, "SCT"))
    {
      std::cout << "Could not retrieve SCT Manager!" << std::endl;
      return false;
    }
  else
    readSCTGeometry();

  // check if SLHC geo is used (TRT not implemented)
  // if not SLHC, get the TRT Det Descr manager
  bool isSLHC = false;
  const IdDictManager* idDictMgr;
  if (m_detStore->retrieve(idDictMgr, "IdDict").isFailure()) {
    std::cout << "Could not get IdDictManager !" << std::endl;
    // return StatusCode::RECOVERABLE;
  } else {
    const IdDictDictionary* dict = idDictMgr->manager()->find_dictionary("InnerDetector"); 
    if(!dict) {
      std::cout << " Cannot access InnerDetector dictionary "<< std::endl;
      // return StatusCode::RECOVERABLE;
    }else{
      //	if (dict->file_name().find("SLHC")!=std::string::npos) isSLHC=true;
      if (dict->m_version.find("SLHC")!=std::string::npos) isSLHC=true;
    }
  }
 
  if(!isSLHC){
    if (!m_detStore->retrieve(m_trt_manager, "TRT"))
      {
	std::cout << "Could not retrieve TRT Manager!" << std::endl;
	return false;
      }
    else
      readTRTGeometry();
  }

  readSolenoidGeometry();

//   if (!m_detStore->retrieve(m_calo_manager, "CaloMgr"))
//     {
//       std::cout << "Could not retrieve Calo Manager!" << std::endl;
//       return false;
//     }
//   else
//     {
//       //lots of weirdo stuff here?
//       readLArGeometry();
//     }

  if (!m_detStore->retrieve(m_tile_manager, "Tile"))
    {
      std::cout << "Could not retrieve Tile Manager!" << std::endl;
      return false;
    }
  else
    readTILEGeometry();

  if (!m_detStore->retrieve(m_lar_manager, "LArMgr"))
    {
      std::cout << "Could not retreive LAr Manager!" << std::endl;
      return false;
    }
  else
    readMBTSGeometry();

  if (!m_detStore->retrieve(m_muon_manager))
    {
      std::cout << "Could not retrieve Muon Manager!" << std::endl;
      return false;
    }
  else
    readMuonGeometry();

  return true;

}


bool StoreGateGeometryReader::readPixelGeometry()
{
  const PixelID* pixelIdHelper = 0;
  if(m_detStore->retrieve(pixelIdHelper,"PixelID").isFailure() || !pixelIdHelper)
    return false;

    InDetDD::SiDetectorElementCollection::const_iterator it;

    for (it=m_pixel_manager->getDetectorElementBegin(); it<m_pixel_manager->getDetectorElementEnd(); it++)
      {

	const InDetDD::SiDetectorElement *element = *it;

	// Just an extra precaution to avoid crashes.
	if (!element) continue;

	if (element->isBarrel() && pixelIdHelper->phi_module(element->identify()) == 0
	    && pixelIdHelper->eta_module(element->identify()) == pixelIdHelper->eta_module_min(element->identify()))
	  {

	    double rMin = element->rMin();
	    double rMax = element->rMax();
	    double zMax = element->zMax();
	    const InDetDD::SiDetectorElement *next = element->nextInEta();
	    while (next)
	      {
		if (next->rMin() < rMin) rMin = next->rMin();
		if (next->rMax() > rMax) rMax = next->rMax();
		if (next->zMax() > zMax) zMax = next->zMax();
		next = next->nextInEta();
	      }
	    zMax += 5;

	    newDetector = new Detector;

	    newDetector->type      = Detector::ABarrelSiliconDetector;
	    newDetector->name      = "Pixel";
	    newDetector->color     = Qt::blue;
	    newDetector->layer     = pixelIdHelper->layer_disk(element->identify());
	    newDetector->length    = element->length()/10.;
	    newDetector->width     = element->width()/10.;
	    newDetector->thickness = (rMax - rMin - element->sinTilt()*element->width())/40.;
	    newDetector->tilt      = 180./M_PI * asin(element->sinTilt());
	    newDetector->nz        = pixelIdHelper->eta_module_max(element->identify()) - pixelIdHelper->eta_module_min(element->identify())+1;
	    newDetector->nphi      = pixelIdHelper->phi_module_max(element->identify())+1;
	    newDetector->r0        = (rMin+rMax)/20.;
	    newDetector->phi0      = 180./M_PI * (element->phiMin()+element->phiMax())/2.;
	    newDetector->zMin      = -zMax/10.;
	    newDetector->zMax      = zMax/10;

	    mDetectors.push_back(*newDetector);

// 	    out << "<ABarrelSiliconDetector c=\"" << "PIX" << "\" n=\"" << "Pixel" << "\""
// 		<< " layer=\"" << pixelIdHelper->layer_disk(element->identify()) << "\""
// 		<< " length=\"" << element->length()/10. << "\""
// 		<< " width=\"" << element->width()/10. << "\""
// 		<< " thickness=\"" << (rMax - rMin - element->sinTilt()*element->width())/40. << "\""
// 		<< " tilt=\"" << 180./M_PI * asin(element->sinTilt()) << "\""
// 		<< " nz=\"" << pixelIdHelper->eta_module_max(element->identify())
// 	      - pixelIdHelper->eta_module_min(element->identify())+1 << "\""
// 		<< " nphi=\"" << pixelIdHelper->phi_module_max(element->identify())+1 << "\""
// 		<< " r0=\"" << (rMin+rMax)/20. << "\""
// 		<< " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
// 		<< " zMin=\"" << -zMax/10. << "\""
// 		<< " zMax=\"" << zMax/10. << "\""
// 		<< " />" << std::endl;
	  }

	if (element->isEndcap() && element->zMin() > 0 && pixelIdHelper->phi_module(element->identify()) == 0
	    && pixelIdHelper->eta_module(element->identify()) == pixelIdHelper->eta_module_min(element->identify()))
	  {

	    double rMin = element->rMin();
	    double rMax = element->rMax();
	    double zMin = element->zMin();
	    double zMax = element->zMax();
	    const InDetDD::SiDetectorElement *next = element->nextInEta();
	    while (next)
	      {
		if (next->zMin() < 0) continue;
		if (next->rMin() < rMin) rMin = next->rMin();
		if (next->rMax() > rMax) rMax = next->rMax();
		if (next->zMin() < zMin) zMin = next->zMin();
		if (next->zMax() > zMax) zMax = next->zMax();
		next = next->nextInEta();
	      }
	    zMin -= 5;
	    zMax += 5;

	    newDetector = new Detector;

	    newDetector->type      = Detector::AEndcapSiliconDetector;
	    newDetector->name      = "Pixel";
	    newDetector->color     = Qt::blue;
	    newDetector->layer     = pixelIdHelper->layer_disk(element->identify());
	    newDetector->length    = element->length()/10.;
	    newDetector->width     = element->width()/10.;
	    newDetector->thickness = (rMax - rMin - element->sinTilt()*element->width())/40.;
	    //newDetector->tilt    = 180./M_PI * asin(element->sinTilt());
	    newDetector->nz        = pixelIdHelper->eta_module_max(element->identify()) - pixelIdHelper->eta_module_min(element->identify())+1;
	    newDetector->nphi      = pixelIdHelper->phi_module_max(element->identify())+1;
	    //newDetector->r0      = (rMin+rMax)/20.;
	    newDetector->rMin      = rMin/10.;
	    newDetector->rMax      = rMax/10.;
	    newDetector->phi0      = 180./M_PI * (element->phiMin()+element->phiMax())/2.;
	    newDetector->zMin      = -zMax/10.;
	    newDetector->zMax      = zMax/10;

	    mDetectors.push_back(*newDetector);

// 	    out << "<AEndcapSiliconDetector c=\"" << "PIX" << "\" n=\"" << "Pixel" << "\""
// 		<< " layer=\"" << pixelIdHelper->layer_disk(element->identify()) << "\""
// 		<< " length=\"" << element->length()/10. << "\""
// 		<< " width=\"" << element->width()/10. << "\""
// 		<< " thickness=\"" << (rMax - rMin - element->sinTilt()*element->width())/40. << "\""
// 		<< " nz=\"" << pixelIdHelper->eta_module_max(element->identify())
// 	      - pixelIdHelper->eta_module_min(element->identify())+1 << "\""
// 		<< " nphi=\"" << pixelIdHelper->phi_module_max(element->identify())+1 << "\""
// 		<< " rMin=\"" << rMin/10. << "\""
// 		<< " rMax=\"" << rMax/10. << "\""
// 		<< " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
// 		<< " zMin=\"" << zMin/10. << "\""
// 		<< " zMax=\"" << zMax/10. << "\""
// 		<< " />" << std::endl;
	  }
      }


    return true;
}

bool StoreGateGeometryReader::readSCTGeometry()
{
  const SCT_ID* siliconIdHelper = 0;
  if(m_detStore->retrieve(siliconIdHelper,"SCT_ID").isFailure() || !siliconIdHelper)
    return false;

  InDetDD::SiDetectorElementCollection::const_iterator it;

  for (it=m_silicon_manager->getDetectorElementBegin(); it<m_silicon_manager->getDetectorElementEnd(); it++)
    {

      const InDetDD::SiDetectorElement *element = *it;

      // Just an extra precaution to avoid crashes.
      if (!element) continue;

      if (element->isBarrel() && element->zMax() > 0 && siliconIdHelper->phi_module(element->identify()) == 0
	  && siliconIdHelper->eta_module(element->identify()) == siliconIdHelper->eta_module_min(element->identify())
	  && siliconIdHelper->side(element->identify()) > 0)
	{

	  double rMin = element->rMin();
	  double rMax = element->rMax();
	  double zMax = element->zMax();
	  const InDetDD::SiDetectorElement *next = element->nextInEta();
	  while (next)
	    {
	      if (next->rMin() < rMin) rMin = next->rMin();
	      if (next->rMax() > rMax) rMax = next->rMax();
	      if (next->zMax() > zMax) zMax = next->zMax();
	      next = next->nextInEta();
	    }
	  next = element->otherSide();
	  while (next)
	    {
	      if (next->rMin() < rMin) rMin = next->rMin();
	      if (next->rMax() > rMax) rMax = next->rMax();
	      if (next->zMax() > zMax) zMax = next->zMax();
	      next = next->nextInEta();
	    }
	  zMax += 5;

	  newDetector = new Detector;

	  newDetector->type      = Detector::ABarrelSiliconDetector;
	  newDetector->name      = "Silicon";
	  newDetector->color     = Qt::green;
	  newDetector->layer     = siliconIdHelper->layer_disk(element->identify());
	  newDetector->length    = element->length()/10.;
	  newDetector->width     = element->width()/10.;
	  newDetector->thickness = 2*(rMax-rMin-element->sinTilt()*element->width())/10.;
	  newDetector->tilt      = 180./M_PI * asin(element->sinTilt());
	  newDetector->nz        = siliconIdHelper->eta_module_max(element->identify()) - siliconIdHelper->eta_module_min(element->identify()) + 1;
	  newDetector->nphi      = siliconIdHelper->phi_module_max(element->identify()) + 1;
	  newDetector->r0        = (rMin+rMax)/20;
	  newDetector->phi0      = 180./M_PI + (element->phiMin()+element->phiMax())/2.;
	  newDetector->zMin      = -zMax/10.;
	  newDetector->zMax      = zMax/10.;

	  mDetectors.push_back(*newDetector);

// 	  out << "<ABarrelSiliconDetector c=\"" << "SIL" << "\" n=\"" << "Silicon" << "\""
// 	      << " layer=\"" << siliconIdHelper->layer_disk(element->identify()) << "\""
// 	      << " length=\"" << element->length()/10. << "\""
// 	      << " width=\"" << element->width()/10. << "\""
// 	      << " thickness=\"" << 2. * (rMax - rMin - element->sinTilt()*element->width())/10. << "\""
// 	      << " tilt=\"" << 180./M_PI * asin(element->sinTilt()) << "\""
// 	      << " nz=\"" << siliconIdHelper->eta_module_max(element->identify())
// 	    - siliconIdHelper->eta_module_min(element->identify())+1 << "\""
// 	      << " nphi=\"" << siliconIdHelper->phi_module_max(element->identify())+1 << "\""
// 	      << " r0=\"" << (rMin+rMax)/20. << "\""
// 	      << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
// 	      << " zMin=\"" << -zMax/10. << "\""
// 	      << " zMax=\"" << zMax/10. << "\""
// 	      << " />" << std::endl;
	}

      if (element->isEndcap() && element->zMin() > 0 && siliconIdHelper->phi_module(element->identify()) == 0
	  && siliconIdHelper->eta_module(element->identify()) == siliconIdHelper->eta_module_min(element->identify())
	  && siliconIdHelper->side(element->identify()) > 0)
	{

	  double rMin = element->rMin();
	  double rMax = element->rMax();
	  double zMin = element->zMin();
	  double zMax = element->zMax();
	  const InDetDD::SiDetectorElement *next = element->nextInEta();
	  while (next)
	    {
	      if (next->zMin() < 0) continue;
	      if (next->rMin() < rMin) rMin = next->rMin();
	      if (next->rMax() > rMax) rMax = next->rMax();
	      if (next->zMin() < zMin) zMin = next->zMin();
	      if (next->zMax() > zMax) zMax = next->zMax();
	      next = next->nextInEta();
	    }
	  next = element->otherSide();
	  while (next)
	    {
	      if (next->zMin() < 0) continue;
	      if (next->rMin() < rMin) rMin = next->rMin();
	      if (next->rMax() > rMax) rMax = next->rMax();
	      if (next->zMin() < zMin) zMin = next->zMin();
	      if (next->zMax() > zMax) zMax = next->zMax();
	      next = next->nextInEta();
	    }
	  zMin -= 5;
	  zMax += 5;

	  newDetector = new Detector;

	  newDetector->type      = Detector::AEndcapSiliconDetector;
	  newDetector->name      = "Silicon";
	  newDetector->color     = Qt::green;
	  newDetector->layer     = siliconIdHelper->layer_disk(element->identify());
	  newDetector->length    = element->length()/10.;
	  newDetector->width     = element->width()/10.;
	  newDetector->thickness = 2*(rMax-rMin-element->sinTilt()*element->width())/10.;
	  //newDetector->tilt    = 180./M_PI * asin(element->sinTilt());
	  newDetector->nz        = siliconIdHelper->eta_module_max(element->identify()) - siliconIdHelper->eta_module_min(element->identify()) + 1;
	  newDetector->nphi      = siliconIdHelper->phi_module_max(element->identify()) + 1;
	  //newDetector->r0      = (rMin+rMax)/20;
	  newDetector->rMin      = rMin/10.;
	  newDetector->rMax      = rMax/10.;
	  newDetector->phi0      = 180./M_PI + (element->phiMin()+element->phiMax())/2.;
	  newDetector->zMin      = -zMax/10.;
	  newDetector->zMax      = zMax/10.;

	  mDetectors.push_back(*newDetector);

// 	  out << "<AEndcapSiliconDetector c=\"" << "SIL" << "\" n=\"" << "Silicon" << "\""
// 	      << " layer=\"" << siliconIdHelper->layer_disk(element->identify()) << "\""
// 	      << " length=\"" << element->length()/10. << "\""
// 	      << " width=\"" << element->width()/10. << "\""
// 	      << " thickness=\"" << 2. * (rMax - rMin - element->sinTilt()*element->width())/10. << "\""
// 	      << " nz=\"" << siliconIdHelper->eta_module_max(element->identify())
// 	    - siliconIdHelper->eta_module_min(element->identify())+1 << "\""
// 	      << " nphi=\"" << siliconIdHelper->phi_module_max(element->identify())+1 << "\""
// 	      << " rMin=\"" << rMin/10. << "\""
// 	      << " rMax=\"" << rMax/10. << "\""
// 	      << " phi0=\"" << 180./M_PI * (element->phiMin()+element->phiMax())/2. << "\""
// 	      << " zMin=\"" << zMin/10. << "\""
// 	      << " zMax=\"" << zMax/10. << "\""
// 	      << " />" << std::endl;
	}
    }
  return true;
}

bool StoreGateGeometryReader::readTRTGeometry()
{

  InDetDD::TRT_DetElementCollection::const_iterator it;
  const TRT_ID *trtIdHelper = 0;
  if(m_detStore->retrieve(trtIdHelper,"").isFailure() || !trtIdHelper)
    return false;

  for (it=m_trt_manager->getDetectorElementBegin(); it<m_trt_manager->getDetectorElementEnd(); it++)
    {

      const InDetDD::TRT_BaseElement *elementIn = *it;

      // Just an extra precaution to avoid crashes.
      if (!elementIn) continue;

      Identifier id = elementIn->identify();


      if (trtIdHelper->phi_module(id) != 0) continue;
      if (trtIdHelper->straw_layer(id) != 0) continue;
      Amg::Vector3D posIn = elementIn->strawTransform(0) * Amg::Vector3D(0., 0., 0.);
      if (posIn.z() < 0) continue;

      const InDetDD::TRT_BaseElement *elementOut;

      if (trtIdHelper->is_barrel(id))
	{
	  elementOut = m_trt_manager->getBarrelElement(trtIdHelper->barrel_ec(id),
						       trtIdHelper->layer_or_wheel(id),
						       0,
						       trtIdHelper->straw_layer_max(id));

	  // Just an extra precaution to avoid crashes.
	  if (!elementOut) continue;

	  Amg::Vector3D posOut = elementOut->strawTransform(0) * Amg::Vector3D(0., 0., 0.);

	  const InDetDD::TRT_BarrelElement *element = dynamic_cast<const InDetDD::TRT_BarrelElement *>(elementIn);
	  if (!element) continue;
	  int nphi = (int) round(2.*M_PI / fabs(element->nextInPhi()->center().phi() - element->center().phi()));
	  double dphiIn = 2.*M_PI / (nphi * trtIdHelper->straw_max(elementIn->identify()));
	  double dphiOut = 2.*M_PI / (nphi * trtIdHelper->straw_max(elementOut->identify()));

	  newDetector = new Detector;

	  newDetector->type   = Detector::ABarrelTRTDetector;
	  newDetector->name   = "TRT";
	  newDetector->color  = Qt::red;
	  newDetector->layer  = trtIdHelper->layer_or_wheel(id);
	  newDetector->nphi   = nphi;
	  newDetector->rMin   = posIn.perp()/10. - 0.4;
	  newDetector->rMax   = posOut.perp()/10. + 0.4;
	  newDetector->phiIn  = 180./M_PI * (posIn.phi() - dphiIn - 2e-3);
	  newDetector->phiOut = 180./M_PI * (posOut.phi() - dphiOut - 2e-3);
	  newDetector->zMin   = -(posIn.z() + elementIn->strawLength()/2.)/10.;
	  newDetector->zMax   = (posIn.z() + elementIn->strawLength()/2.)/10.;

	  mDetectors.push_back(*newDetector);

// 	  out << "<ABarrelTRTDetector c=\"TRT\" n=\"TRT\""
// 	      << " layer=\"" << trtIdHelper->layer_or_wheel(id) << "\""
// 	      << " nphi=\"" << nphi << "\""
// 	      << " rMin=\"" << posIn.perp()/10. - .4 << "\""
// 	      << " rMax=\"" << posOut.perp()/10. + .4 << "\""
// 	      << " phiIn=\"" << 180./M_PI * (posIn.phi() - dphiIn - 2e-3) << "\""
// 	      << " phiOut=\"" << 180./M_PI * (posOut.phi() - dphiOut - 2e-3) << "\""
// 	      << " zMin=\"" << -(posIn.z() + elementIn->strawLength()/2.)/10. << "\""
// 	      << " zMax=\"" << (posIn.z() + elementIn->strawLength()/2.)/10. << "\""
// 	      << " />" << std::endl;
	}

      else
	{
	  elementOut = m_trt_manager->getEndcapElement(1,//trtIdHelper->barrel_ec(id),
						       trtIdHelper->layer_or_wheel(id),
						       trtIdHelper->straw_layer_max(id),
						       0);

	  // Just an extra precaution to avoid crashes.
	  if (!elementOut) continue;

	  Amg::Vector3D posOut = elementOut->strawTransform(trtIdHelper->straw_max(id)) * Amg::Vector3D(0., 0., 0.);

	  // floor() instead of round() becuase we are neglecting the space between two modules
	  int nphi = (int) floor(2.*M_PI / fabs(posOut.phi() - posIn.phi()));

	  newDetector = new Detector;

	  newDetector->type   = Detector::AEndcapTRTDetector;
	  newDetector->name   = "TRT";
	  newDetector->color  = Qt::red;
	  newDetector->layer  = trtIdHelper->layer_or_wheel(id);
	  newDetector->nphi   = nphi;
	  newDetector->rMin   = (posIn.perp() - elementIn->strawLength()/2.)/10.;
	  newDetector->rMax   = (posIn.perp() + elementIn->strawLength()/2.)/10.;
	  newDetector->phi0   = 180./M_PI * posIn.phi();
	  //newDetector->phiOut = 180./M_PI * (posOut.phi() - dphiOut - 2e-3);
	  newDetector->zMin   = posIn.z()/10.;
	  newDetector->zMax   = posOut.z()/10.;

	  mDetectors.push_back(*newDetector);


// 	  out << "<AEndcapTRTDetector c=\"TRT\" n=\"TRT\""
// 	      << " layer=\"" << trtIdHelper->layer_or_wheel(id) << "\""
// 	      << " nphi=\"" << nphi << "\""
// 	      << " rMin=\"" << (posIn.perp()-elementIn->strawLength()/2.)/10. << "\""
// 	      << " rMax=\"" << (posIn.perp()+elementIn->strawLength()/2.)/10. << "\""
// 	      << " phi0=\"" << 180./M_PI * posIn.phi() << "\""
// 	      << " zMin=\"" << posIn.z()/10. << "\""
// 	      << " zMax=\"" << posOut.z()/10. << "\""
// 	      << " />" << std::endl;
	}

    }
  return true;
}

bool StoreGateGeometryReader::readSolenoidGeometry()
{
  double zIn = -265.0;
  double zOut = 265.0;
  double rIn = 122.9;
  double rOut = 127.4;

  newDetector = new Detector;

  newDetector->type       = Detector::ADisc;
  newDetector->name       = "Solenoid";
  newDetector->color      = Qt::darkGray;
  newDetector->projection = "YX";
  newDetector->rInner     = rIn;
  newDetector->rOuter     = rOut;

  mDetectors.push_back(*newDetector);

  //   out << "<ADisc p=\"YX\" c=\"Sol\"  n=\"Solenoid\""
  //       << " rIn=\"" << rIn << "\""
  //       << " rOut=\"" << rOut << "\" />"
  //       << std::endl;

  newDetector = new Detector;

  newDetector->type       = Detector::ARectangle;
  newDetector->color      = Qt::darkGray;
  newDetector->name       = "Solenoid";
  newDetector->projection = "RZ";
  newDetector->xMin       = zIn;
  newDetector->xMax       = zOut;
  newDetector->yMin       = rIn;
  newDetector->yMax       = rOut;
  newDetector->xReflect   = false;//TK: was "NO" in the original code

  mDetectors.push_back(*newDetector);

//   out << "<ARectangle p=\"RZ\" c=\"Sol\" n=\"Solenoid\""
//       << " xMin=\"" << zIn << "\""
//       << " xMax=\"" << zOut << "\""
//       << " yMin=\"" << rIn << "\""
//       << " yMax=\"" << rOut << "\""
//       << " xR=\"NO\" />"
//       << std::endl;

  return true;

}

bool StoreGateGeometryReader::readTILEGeometry()
{
  const TileID *tileIdHelper = m_tile_manager->get_id();
  TileDetDescrManager::tile_descr_const_iterator tileIt;
  for (tileIt=m_tile_manager->tile_descriptors_begin(); tileIt!=m_tile_manager->tile_descriptors_end(); tileIt++)
    {
      const TileDetDescriptor *descriptor = *tileIt;
      if (!descriptor) continue;

      int i;
      for (i=0; i<descriptor->n_samp(); i++)
	{

	  if (descriptor->zcenter(i) <= 0) continue;

	  std::string name;
	  if (tileIdHelper->is_tile_barrel(descriptor->identify())) name = "TILE Barrel";
	  else if (tileIdHelper->is_tile_extbarrel(descriptor->identify())) name = "Extended TILE";
	  else if (tileIdHelper->is_tile_gap(descriptor->identify())) name = "ITC Gap";
	  else if (tileIdHelper->is_tile_gapscin(descriptor->identify())) name = "ITC Gap Scintillator";
	  else name = "TILE";

	  if (!name.compare(0, 3, "ITC"))
	    {
	      newDetector = new Detector;

	      newDetector->type     = Detector::AGapCalorimeter;
	      newDetector->name     = name;
	      newDetector->color    = Qt::cyan;
	      newDetector->sampling = ( i < 3 ? i : 3);
	      newDetector->region   = 0;
	      newDetector->rMin     = (descriptor->rcenter(i) - descriptor->dr(i)/2.)/10.;
	      newDetector->rMax     = (descriptor->rcenter(i) + descriptor->dr(i)/2.)/10.;
	      newDetector->zMin     = (descriptor->zcenter(i) - descriptor->dz(i))/10.;
	      newDetector->zMax     = (descriptor->zcenter(i) + descriptor->dz(i))/10.;
	      newDetector->eta      = (i < 3 ? tileIdHelper->eta_min(descriptor->identify())-i+2 : i);
	      newDetector->phi0     = descriptor->phi_min();
	      newDetector->nphi     = descriptor->n_phi();

	      mDetectors.push_back(*newDetector);

// 	      out << "<AGapCalorimeter c=\"" << "HCAL" << "\" n=\"" << name << "\""
// 		  << " sampling=\"" << (i < 3 ? i : 3)  << "\" region=\"" << 0 << "\""
// 		  << " rMin=\"" << (descriptor->rcenter(i)-descriptor->dr(i)/2.)/10. << "\""
// 		  << " rMax=\"" << (descriptor->rcenter(i)+descriptor->dr(i)/2.)/10. << "\""
// 		  << " zMin=\"" << (descriptor->zcenter(i)-descriptor->dz(i))/10. << "\""
// 		  << " zMax=\"" << (descriptor->zcenter(i)+descriptor->dz(i))/10. << "\""
// 		  << " eta=\"" << (i < 3 ? tileIdHelper->eta_min(descriptor->identify())-i+2 : i) << "\""
// 		  << " phi0=\"" << descriptor->phi_min() << "\""
// 		  << " nphi=\"" << descriptor->n_phi() << "\""
// 		  << " />" << std::endl;
	    }

	  else
	    {
	      int section = tileIdHelper->section(descriptor->identify());
	      int side = tileIdHelper->side(descriptor->identify());
	      int module = tileIdHelper->module(descriptor->identify());
	      int tower = tileIdHelper->tower(descriptor->identify());
	      Identifier id = tileIdHelper->cell_id(section, side, module, tower, i);

	      newDetector = new Detector;

	      newDetector->type     = Detector::ABarrelCalorimeter;
	      newDetector->name     = name;
	      newDetector->color    = Qt::cyan;
	      newDetector->sampling = i;
	      newDetector->region   = 0;
	      newDetector->rMin     = (descriptor->rcenter(i) - descriptor->dr(i)/2.)/10.;
	      newDetector->rMax     = (descriptor->rcenter(i) + descriptor->dr(i)/2.)/10.;
	      newDetector->zMin     = (descriptor->zcenter(i) - descriptor->dz(i))/10.;
	      newDetector->zMax     = (descriptor->zcenter(i) + descriptor->dz(i))/10.;
	      newDetector->eta0     = descriptor->eta_min(i);
	      newDetector->deta     = descriptor->deta(i);
	      newDetector->neta     = descriptor->n_eta(i);
	      newDetector->meta     = tileIdHelper->eta_min(id);
	      newDetector->phi0     = descriptor->phi_min();
	      newDetector->nphi     = descriptor->n_phi();

	      mDetectors.push_back(*newDetector);

// 	      out << "<ABarrelCalorimeter c=\"" << "HCAL" << "\" n=\"" << name << "\""
// 		  << " sampling=\"" << i << "\" region=\"" << 0 << "\""
// 		  << " rMin=\"" << (descriptor->rcenter(i)-descriptor->dr(i)/2.)/10. << "\""
// 		  << " rMax=\"" << (descriptor->rcenter(i)+descriptor->dr(i)/2.)/10. << "\""
// 		  << " zMin=\"" << (descriptor->zcenter(i)-descriptor->dz(i))/10. << "\""
// 		  << " zMax=\"" << (descriptor->zcenter(i)+descriptor->dz(i))/10. << "\""
// 		  << " eta0=\"" << descriptor->eta_min(i) << "\""
// 		  << " deta=\"" << descriptor->deta(i) << "\""
// 		  << " neta=\"" << descriptor->n_eta(i) << "\""
// 		  << " meta=\"" << tileIdHelper->eta_min(id) << "\""
// 		  << " phi0=\"" << descriptor->phi_min() << "\""
// 		  << " nphi=\"" << descriptor->n_phi() << "\""
// 		  << " />" << std::endl;
	    }
	}
    }

  return true;
}

bool StoreGateGeometryReader::readMBTSGeometry()
{
  PVConstLink myVol = m_lar_manager->getTreeTop(1U);
  for (unsigned int c=0; c< myVol->getNChildVols();c++)
    {
      PVConstLink child = myVol->getChildVol(c);
       if((child->getLogVol())->getName()=="MBTS_mother")
	 {
	   int sampling=0,numPhi=0;
	   std::string stringOfNames="";
	   HepGeom::Scale3D     scale;
	   HepGeom::Rotate3D    rotate;
	   HepGeom::Translate3D translate;
	   (child->getX()).getDecomposition(scale, rotate, translate);
	   double zlocation = translate.dz();
    	  if(zlocation<0)
	    zlocation=-zlocation;
    	  for (unsigned int cc=0; cc< child->getNChildVols();cc++)
	    {
	      if(((child->getChildVol(cc))->getLogVol())->getName()=="MBTS1")
    	        numPhi++;
	    }
    	  for (unsigned int cc=0; cc< child->getNChildVols();cc++)
	    {
	      PVConstLink childschild = child->getChildVol(cc);
	      if((childschild->getLogVol())->getShape()->typeID() == GeoTrd::getClassTypeID() )
		{
		  std::string currentName = (childschild->getLogVol())->getName();
		  if(stringOfNames.find(currentName,0) == std::string::npos)
		    {
		      stringOfNames+=" " + currentName;
		      HepGeom::Translate3D translateToChild;
		      (child->getXToChildVol(cc)).getDecomposition(scale, rotate, translateToChild);
		      const GeoTrd* theTrd = dynamic_cast<const GeoTrd*> ((childschild->getLogVol())->getShape());
		      double rho=pow(translateToChild.dx(),2.0) + pow(translateToChild.dy(),2.0);
		      rho=pow(rho,0.5);
		      double RMin=rho-theTrd->getZHalfLength();
		      double RMax=rho+theTrd->getZHalfLength();
		      double zmovement=translateToChild.dz();
		      double zthickness=theTrd->getXHalfLength1();

		      newDetector = new Detector;

		      newDetector->type = Detector::AEndcapCryostat;
		      newDetector->name = "Minimum Bias Trigger Scintillators";
		      newDetector->color= Qt::cyan;
		      newDetector->sampling = sampling;
		      newDetector->rMin = RMin/10.;
		      newDetector->rMax = RMax/10.;
		      newDetector->zMin = (zlocation + zmovement - zthickness)/10.;
		      newDetector->zMax = (zlocation + zmovement + zthickness)/10.;
		      newDetector->neta = 1;
		      newDetector->nphi = numPhi;

		      mDetectors.push_back(*newDetector);

// 		      out << "<AEndcapCryostat c=\"HCAL\" n=\"Minimum Bias Trigger Scintillators\""
// 			  << " sampling=\"" << sampling << "\" region=\"" << 0 << "\""
// 			  << " rMin=\"" << RMin/10. << "\""
// 			  << " rMax=\"" << RMax/10. << "\""
// 			  << " zMin=\"" << (zlocation+zmovement-zthickness)/10. << "\""
// 			  << " zMax=\"" << (zlocation+zmovement+zthickness)/10. << "\""
// 			  << " neta=\"" << "1" << "\""
// 			  << " nphi=\"" << numPhi << "\""
// 			  << " />" << std::endl;

		      sampling++;
		    }
		}
	    }
	 }
    }
  return true;
}

bool StoreGateGeometryReader::readLArGeometry()
{
  QColor lArColor;
  const CaloCell_ID *idHelper = m_calo_manager->getCaloCell_ID();
  CaloDetDescrManager::calo_element_const_iterator it;

  // This code is not very efficient in terms of speed. Since it will only be used
  // when the geometry has changed, the code is made to be easily readable instead.
  for (int type=0; type<m_numCaloTypes; type++)
    {

      for (int region=0; region<8; region++)
	{

	  for (int inner=0; inner<2; inner++)
	    {

	      int minEtaIndex = 0, maxEtaIndex = 0;
	      double minEta = 0.0;
	      const CaloDetDescrElement *oneSuchElement = 0;

	      for (it=m_calo_manager->element_begin(); it<m_calo_manager->element_end(); it++)
		{

		  const CaloDetDescrElement *element = *it;
		  int etaIndex, phiIndex;
		  std::string technology;
		  std::string name;

		  if (!element) continue;

		  Identifier id = element->identify();

		  // Select everything but TILE.
		  if (element->is_tile()) continue;

		  // Select only this sampling.
		  if (element->getSampling() != type) continue;

		  // Select only this region.
		  if (element->is_lar_fcal())
		    {
		      if (idHelper->eta(id) != idHelper->eta_min(id) || idHelper->side(id) < 0) continue;

		    }

		  else
		    {
		      if (idHelper->region(id) != region) continue;
		    }

		  // Differentiate between the LAr inner/outer endcap
		  if (inner > 0 && !idHelper->is_em_endcap_inner(id)) continue;

		  // Skip -z, we will mirror +z.
		  if (element->z() < 0) continue;

		  // Select only the first element in phi.
		  etaIndex = idHelper->eta(id);
		  phiIndex = idHelper->phi(id);
		  if (phiIndex > 0) continue;

		  if (!oneSuchElement)
		    {
		      oneSuchElement = element;
		      minEtaIndex = maxEtaIndex = etaIndex;
		      minEta = element->eta();
		    }
		  else
		    {
		      if (etaIndex < minEtaIndex)
			{
			  minEtaIndex = etaIndex;
			  minEta = element->eta();
			}
		      if (etaIndex > maxEtaIndex) {

			maxEtaIndex = etaIndex;
		      }
		    }
		}

	      if (oneSuchElement)
		{

		  std::string color;
		  std::string name;
		  int sampling = 0;
		  enum {BARREL, ENDCAP, FORWARD} part;

		  const CaloDetDescriptor *descriptor = oneSuchElement->descriptor();

		  switch(type)
		    {
		    case CaloCell_ID::PreSamplerB:
		      color = "ECAL";
		      lArColor = Qt::cyan;
		      name = "LAr Presampler";
		      sampling = 0;
		      part = BARREL;
		      break;
		    case CaloCell_ID::EMB1:
		    case CaloCell_ID::EMB2:
		    case CaloCell_ID::EMB3:
		      color = "ECAL";
		      lArColor = Qt::cyan;
		      name = "LAr";
		      sampling = type - CaloCell_ID::PreSamplerB;
		      part = BARREL;
		      break;
		    case CaloCell_ID::PreSamplerE:
		      color = "ECAL";
		      lArColor = Qt::cyan;
		      name = "LAr_EC_Presampler";
		      part = ENDCAP;
		      break;
		    case CaloCell_ID::EME1:
		    case CaloCell_ID::EME2:
		    case CaloCell_ID::EME3:
		      color = "ECAL";
		      lArColor = Qt::cyan;
		      name = inner ? "LAr Inner Endcap" : "LAr Outer Endcap";
		      sampling = type - CaloCell_ID::PreSamplerE;
		      part = ENDCAP;
		      break;
		    case CaloCell_ID::HEC0:
		    case CaloCell_ID::HEC1:
		    case CaloCell_ID::HEC2:
		    case CaloCell_ID::HEC3:
		      color = "HCAL";
		      lArColor = Qt::darkCyan;
		      name = "HEC";
		      sampling = type - CaloCell_ID::HEC0;
		      part = ENDCAP;
		      break;
		    case CaloCell_ID::FCAL0:
		      color = "ECAL";
		      lArColor = Qt::cyan;
		      name = "FCAL EM";
		      part = FORWARD;
		      break;
		    case CaloCell_ID::FCAL1:
		      color = "HCAL";
		      lArColor = Qt::darkCyan;
		      name = "FCAL HAD 1";
		      part = FORWARD;
		      break;
		    case CaloCell_ID::FCAL2:
		      color = "HCAL";
		      lArColor = Qt::darkCyan;
		      name = "FCAL HAD 2";
		      part = FORWARD;
		      break;
		    default:
		      assert(0&&"Unknown CaloCell_ID type");
		      part = BARREL;//To avoid compile warning - but should never happen.
		    }

		  double phi0 = 180./M_PI * (oneSuchElement->phi()-oneSuchElement->dphi()/2.);
		  int numPhi = (int) round(2.*M_PI / oneSuchElement->dphi());
		  int numEta = maxEtaIndex - minEtaIndex + 1;

		  switch (part)
		    {
		    case BARREL:

		      newDetector = new Detector;

		      newDetector->type = Detector::ABarrelCalorimeter;
		      newDetector->name = name;
		      newDetector->color= lArColor;
		      newDetector->sampling = sampling;
		      newDetector->region = region % 8;
		      newDetector->rMin = (oneSuchElement->r() - oneSuchElement->dr()/2.)/10.;
		      newDetector->rMax = (oneSuchElement->r() + oneSuchElement->dr()/2.)/10.;
		      newDetector->zMin = m_larMin[type];
		      newDetector->zMax = m_larMax[type];
		      newDetector->eta0 = minEta - oneSuchElement->deta()/2.;
		      newDetector->deta = oneSuchElement->deta();
		      newDetector->neta = numEta;
		      newDetector->meta = minEtaIndex;
		      newDetector->phi0 = phi0;
		      newDetector->nphi = numPhi;

		      mDetectors.push_back(*newDetector);

// 		      out << "<ABarrelCalorimeter c=\"" << color << "\" n=\"" << name << "\""
// 			  << " sampling=\"" << sampling << "\" region=\"" << region%8 << "\""
// 			  << " rMin=\"" << (oneSuchElement->r()-oneSuchElement->dr()/2.)/10. << "\""
// 			  << " rMax=\"" << (oneSuchElement->r()+oneSuchElement->dr()/2.)/10. << "\""
// 			  << " zMin=\"" << m_larMin[type] << "\""
// 			  << " zMax=\"" << m_larMax[type] << "\""
// 			  << " eta0=\"" << minEta-oneSuchElement->deta()/2. << "\""
// 			  << " deta=\"" << oneSuchElement->deta() << "\""
// 			  << " neta=\"" << numEta << "\""
// 			  << " meta=\"" << minEtaIndex << "\""
// 			  << " phi0=\"" << phi0 << "\""
// 			  << " nphi=\"" << numPhi << "\""
// 			  << " />" << std::endl;
		      break;
		    case ENDCAP:

		      newDetector = new Detector;

		      newDetector->type     = Detector::AEndcapCalorimeter;
		      newDetector->name     = name;
		      newDetector->color    = lArColor;
		      newDetector->sampling = sampling;
		      newDetector->region   = region % 8;
		      newDetector->rMin     = m_larMin[type];
		      newDetector->rMax     = m_larMax[type];
		      newDetector->zMin     = (oneSuchElement->z() - oneSuchElement->dz()/2.)/10.;
		      newDetector->zMax     = (oneSuchElement->z() + oneSuchElement->dz()/2.)/10.;
		      newDetector->eta0     = minEta - oneSuchElement->deta()/2.;
		      newDetector->deta     = oneSuchElement->deta();
		      newDetector->neta     = numEta;
		      newDetector->meta     = minEtaIndex;
		      newDetector->phi0     = phi0;
		      newDetector->nphi     = numPhi;

		      mDetectors.push_back(*newDetector);

// 		      out << "<AEndcapCalorimeter c=\"" << color << "\" n=\"" << name << "\""
// 			  << " sampling=\"" << sampling << "\" region=\"" << region%8 << "\""
// 			  << " rMin=\"" << m_larMin[type] << "\""
// 			  << " rMax=\"" << m_larMax[type] << "\""
// 			  << " zMin=\"" << (oneSuchElement->z()-oneSuchElement->dz()/2.)/10. << "\""
// 			  << " zMax=\"" << (oneSuchElement->z()+oneSuchElement->dz()/2.)/10. << "\""
// 			  << " eta0=\"" << minEta-oneSuchElement->deta()/2. << "\""
// 			  << " deta=\"" << oneSuchElement->deta() << "\""
// 			  << " neta=\"" << numEta << "\""
// 			  << " meta=\"" << minEtaIndex << "\""
// 			  << " phi0=\"" << phi0 << "\""
// 			  << " nphi=\"" << numPhi << "\""
// 			  << " />" << std::endl;
		      break;
		    case FORWARD:

		      newDetector = new Detector;

		      newDetector->type = Detector::ADisc;
		      newDetector->name = name;
		      newDetector->color= lArColor;
		      newDetector->projection = "YX";
		      newDetector->rInner = descriptor->calo_r_min()/10.;
		      newDetector->rOuter = descriptor->calo_r_max()/10.;

		      mDetectors.push_back(*newDetector);

// 		      out << "<ADisc p=\"YX\" c=\"" << color << "\" n=\"" << name << "\""
// 			  << " rIn=\"" << descriptor->calo_r_min()/10. << "\""
// 			  << " rOut=\"" << descriptor->calo_r_max()/10. << "\""
// 			  << " />" << std::endl;

		      newDetector = new Detector;

		      newDetector->type = Detector::ARectangle;
		      newDetector->name = name;
		      newDetector->color= lArColor;
		      newDetector->projection = "RZ";
		      newDetector->xMin = descriptor->calo_z_min()/10.;
		      newDetector->xMax = descriptor->calo_z_max()/10.;
		      newDetector->yMin = descriptor->calo_r_min()/10.;
		      newDetector->yMax = descriptor->calo_r_max()/10.;

		      mDetectors.push_back(*newDetector);

// 		      out << "<ARectangle p=\"RZ\" c=\"" << color << "\" n=\"" << name << "\""
// 			  << " xMin=\"" << descriptor->calo_z_min()/10. << "\""
// 			  << " xMax=\"" << descriptor->calo_z_max()/10. << "\""
// 			  << " yMin=\"" << descriptor->calo_r_min()/10. << "\""
// 			  << " yMax=\"" << descriptor->calo_r_max()/10. << "\""
// 			  << " />" << std::endl;
		    }
		}
	    }
	}
    }
  return true;
}

bool StoreGateGeometryReader::readToDiscElement()
{
  return true;
}

bool StoreGateGeometryReader::readToRectElement()
{
  return true;
}

bool StoreGateGeometryReader::readToRectanglishElement()
{
  return true;
}

bool StoreGateGeometryReader::readMuonGeometry()
{
  QColor muonColor;
  // While we retrieve the MdtIdHelper, we use the functionality provided by its parent class MuonIdHelper
  // this is not MDT specific and any of the other IdHelpers would have worked as well.
  const MdtIdHelper *mdtIdHelper = m_muon_manager->mdtIdHelper();
  int snMax = mdtIdHelper->nStationNames()-1;

  // Loop over all station types.
  for (int sn=0; sn<=snMax; sn++)
    {
      std::string stationName = mdtIdHelper->stationNameString(sn);

      // Determine the type of chamber from the stationName string.
      std::string stationTech;
      switch(stationName[0])
	{
	case 'B':
	case 'E':
	  stationTech = "MDT";
	  muonColor = Qt::white;
	  break;
	case 'C':
	  stationTech = "CSC";
	  muonColor = Qt::gray;
	  break;
	case 'T':
	  stationTech = "TGC";
	  muonColor = Qt::darkGray;
	  break;
	default:
	  stationTech = "???";
	  muonColor = Qt::lightGray;
	  break;
	}

      // The TGCs contain several stations in one sector. As this would create several stations with the same
      // identifier, people apparently chose to let the phi index of the stations continue up to 3 or even 6
      // times the "real" maximum value. We need to determine this maximum value in order to calculate the real
      // phi index.
      int maxPhi;
      if (stationTech == "TGC")
	{
	  if (stationName[2] == 'E' && stationName[1] != '4')
	    {
	      maxPhi = 48;
	    }
	  else
	    {
	      maxPhi = 24;
	    }
	}
      else
	{
	  maxPhi = 8;
	}

      // Loop over all eta values.
      for (int eta=0; eta<=16; eta++)
	{
	  std::vector<const MuonGM::MuonStation *> *stations = new std::vector<const MuonGM::MuonStation *>;

	  // And loop over all possible phi values.
	  for (int phi=maxPhi; phi>0; phi--)
	    {

	      // Try to retrieve the station, it might not exist, but this seems to be the most
	      // reliable way to find out.
	      const MuonGM::MuonStation *station = m_muon_manager->getMuonStation(stationName, eta, phi);

	      // If station != 0, the station exists and we add it to our vector.
	      if (station) stations->push_back(station);
	    }

	  // While there are stations that haven't been written to XML, stay in this loop.
	  while (stations->size() > 0)
	    {

	      // Take the parameters of a station and store them in the variables with the "1" suffix.
	      const MuonGM::MuonStation *station1 = *(stations->end()-1);

	      Amg::Vector3D pos1 = getPosition(station1, maxPhi);
	      int phi1 = station1->getPhiIndex();
	      double dphi1 = getDeltaPhi(pos1, maxPhi);
	      double shift1 = getShift(pos1, dphi1);
	      double alpha1 = getAlpha( Amg::CLHEPTransformToEigen(station1->getTransform()) );

	      // Now determine the dimensions of a station of this station.
	      double zi1 = pos1.z() - station1->Zsize()/2.;    // inner z
	      double zo1 = pos1.z() + station1->Zsize()/2.;    // outer z
	      double ri1 = pos1.perp() - station1->Rsize()/2.; // inner r
	      double ro1 = pos1.perp() + station1->Rsize()/2.; // outer r
	      double wi1 = station1->Ssize();                  // width at inner r
	      double wo1 = station1->LongSsize();              // width at outer r

	      // Create the string containing the phi sectors this station is in.
	      std::stringstream sstream1;
	      sstream1 << phi1;
	      std::string phiString = sstream1.str();

	      // Remove this station from the to-be-processed list.
	      stations->erase(stations->end()-1, stations->end());

	      // Now loop over the other stations and see if any of them have the same parameters. Do
	      // this in reverse order to allow the current station to be erased from the vector.
	      std::vector<const MuonGM::MuonStation *>::iterator it;
	      for (it=stations->end()-1; it>=stations->begin(); it--)
		{
		  Amg::Vector3D pos2 = getPosition(*it, maxPhi);
		  int phi2 = (*it)->getPhiIndex();
		  double dphi2 = getDeltaPhi(pos2, maxPhi);
		  double shift2 = getShift(pos2, dphi2);
		  double alpha2 = getAlpha( Amg::CLHEPTransformToEigen( (*it)->getTransform()) );

		  double zi2 = pos2.z() - (*it)->Zsize()/2.;    // inner z
		  double zo2 = pos2.z() + (*it)->Zsize()/2.;    // outer z
		  double ri2 = pos2.perp() - (*it)->Rsize()/2.; // inner r
		  double ro2 = pos2.perp() + (*it)->Rsize()/2.; // outer r
		  double wi2 = (*it)->Ssize();                  // width at inner r
		  double wo2 = (*it)->LongSsize();              // width at outer r

		  // If the parameters are indeed the same (within reasonable limits), then we
		  // can simply add this chamber to the sectors attribute of the first one.
		  if (Amg::distance(pos1, pos2) < m_smallDistance
		      && fabs(dphi1 - dphi2) < m_smallAngle
		      && fabs(shift1 - shift2) < m_smallDistance
		      && fabs(alpha1 - alpha2) < m_smallAngle
		      && fabs(zi1 - zi2) < m_smallDistance
		      && fabs(zo1 - zo2) < m_smallDistance
		      && fabs(ri1 - ri2) < m_smallDistance
		      && fabs(ro1 - ro2) < m_smallDistance
		      && fabs(wi1 - wi2) < m_smallDistance
		      && fabs(wo1 - wo2) < m_smallDistance)
		    {

		      // Add the station to the phi sector string and remove it from the
		      // to-be-processed list.
		      std::stringstream sstream2;
		      sstream2 << phi2;
		      phiString += " " + sstream2.str();

		      stations->erase(it, it+1);
		    }
		}

	      // From here onwards we need to treat barrel chambers and endcap chambers differently.
	      if (stationName[0] == 'B')
		{

		  // Barrel chambers can have inner and/or outer RPCs.
		  // Let's take a default of 0. (no RPCs).
		  double rpci = 0.;
		  double rpco = 0.;

		  // Determine the thickness of the RPC layer on this station.
		  if (stationName[1] == 'M')
		    {
		      // Middle (BM*) stations have RPCs on both sides.
		      rpci = rpco = 15.;
		    }
		  else if (stationName[1] == 'O')
		    {
		      // Outer (BO*) stations have RPCs on one side.
		      if (stationName[2] == 'S')
			{
			  // On the side facing the IP for small sectors (BOS).
			  rpci = 15.;
			}
		      else
			{
			  // On the outside for large sectors (BOL, BOF, BOG, BOH).
			  rpco = 15.;
			}
		    }

		  newDetector = new Detector;
		  newDetector->type   = Detector::ABox;
		  newDetector->name   = stationTech+"_"+stationName;
		  newDetector->color  = muonColor;
		  newDetector->zInner = zi1/10.;
		  newDetector->zOuter = zo1/10.;
		  newDetector->rInner = ri1/10.;
		  newDetector->rOuter = ro1/10.;
		  newDetector->w      = wi1/10.;
		  newDetector->eta    = eta;
		  newDetector->phi    = getPhiVectorFromString(phiString);

// 		  // Barrel chambers are written as <ABox> elements.
// 		  out << "<ABox n=\"" << stationTech << "_" << stationName << "\""
// 		      << " zi=\"" << zi1/10. << "\"" << " zo=\"" << zo1/10. << "\""
// 		      << " ri=\"" << ri1/10. << "\"" << " ro=\"" << ro1/10. << "\""
// 		      << " w=\"" << wi1/10. << "\""
// 		      << " eta=\"" << eta << "\""
// 		      << " phi=\"" << phiString << "\"";

		  // A rotation with respect to the large sector.
		  if (fabs(dphi1) > m_smallAngle)
		    newDetector->dphi = 180/M_PI * dphi1;
		  //out << " dphi=\"" << 180/M_PI * dphi1 << "\"";

		  // A shift perpendicular to r in the xy-plane.
		  if (fabs(shift1) > m_smallDistance)
		    newDetector->sh = shift1/10.;
		  //out << " sh=\"" << shift1/10. << "\"";

		  // RPCs.
		  if (rpci > 0.)
		    newDetector->RPCi = rpci;
		  //out << " RPCi=\"" << rpci << "\"";
		  if (rpco > 0.)
		    newDetector->RPCo = rpco;
		  //out << " RPCo=\"" << rpco << "\"";
		  //out << " />" << std::endl;

		  mDetectors.push_back(*newDetector);

		}
	      else
		{
		  std::stringstream etastream;
		  etastream << eta;
		  newDetector = new Detector;
		  newDetector->name   = stationTech+"_"+stationName+etastream.str();
		  newDetector->type   = Detector::ATrd;
		  newDetector->color  = muonColor;
		  newDetector->zInner = zi1/10.;
		  newDetector->zOuter = zo1/10.;
		  newDetector->rInner = ri1/10.;
		  newDetector->rOuter = ro1/10.;
		  newDetector->wInner = wi1/10.;
		  newDetector->wOuter = wo1/10.;
		  newDetector->eta    = eta;
		  newDetector->phi    = getPhiVectorFromString(phiString);

// 		  // Endcap chambers are written as <ATrd> elements, parameters are similar to <ABox>.
// 		  out << "<ATrd n=\"" << stationTech << "_" << stationName << eta << "\""
// 		      << " zi=\"" << zi1/10. << "\"" << " zo=\"" << zo1/10. << "\""
// 		      << " ri=\"" << ri1/10. << "\"" << " ro=\"" << ro1/10. << "\""
// 		      << " wi=\"" << wi1/10. << "\"" << " wo=\"" << wo1/10. << "\""
// 		      << " eta=\"" << eta << "\""
// 		      << " phi=\"" << phiString << "\"";

		  if (fabs(dphi1) > m_smallAngle)
		    newDetector->dphi = 180/M_PI * dphi1;
		  //out << " dphi=\"" << 180/M_PI * dphi1 << "\"";

		  if (fabs(shift1) > m_smallDistance)
		    newDetector->sh = shift1/10.;
		  //out << " sh=\"" << shift1/10. << "\"";

		  // A tilt in the rz-plane, for the CSCs.
		  if (fabs(alpha1) > m_smallAngle)
		    newDetector->alpha = 180/M_PI * alpha1;
		  //out << " a=\"" << 180/M_PI * alpha1 << "\"";
		  //out << " />" << std::endl;

		  mDetectors.push_back(*newDetector);
		}
	    }

	  delete stations;
	}
    }

  return true;
}

Amg::Vector3D StoreGateGeometryReader::getPosition(const MuonGM::MuonStation *station, int maxPhi) const
{
  // Take the position of the station.
  Amg::Vector3D pos = Amg::CLHEPTransformToEigen( station->getTransform() ) * Amg::Vector3D(0., 0., 0.);

  double phi = 2.*M_PI * ((double) station->getPhiIndex()-1.) / maxPhi;

  // Rotate it to sector 1.
  //return HepGeom::RotateZ3D((1-phi) * M_PI/4.) * pos;
  return Amg::getRotateZ3D(-phi) * pos;
}

double StoreGateGeometryReader::getDeltaPhi(const Amg::Vector3D &pos, int maxPhi) const
{
  if (maxPhi > 8)
    {
      // For TGCs there is no shift, so we can just return the angle.
      return pos.phi();
    }
  else if (fabs(pos.phi() - M_PI/8.) < m_smallAngle)
    {
      // For the others, we consider it to be the angle if it's reasonably close to M_PI/8.
      // Any further deviation will be put in as a shift.
      return M_PI/8.;
    }
  else
    {
      // No rotation at all.
      return 0.;
    }
}

double StoreGateGeometryReader::getShift(const Amg::Vector3D &pos, double dphi) const
{
  Amg::Vector3D rotpos;

  // First we remove the shift caused by the rotation over dphi.
  if (fabs(dphi) < m_smallAngle)
    {
      rotpos = pos;
    }
  else
    {
      rotpos = Amg::getRotateZ3D(-dphi) * pos;
    }

  // Then we return what is left as the shift.
  if (fabs(rotpos.y()) < m_smallDistance)
    {
      return 0.;
    }
  else
    {
      return rotpos.y();
    }
}

double StoreGateGeometryReader::getAlpha(const Amg::Transform3D &trans) const
{
  // Extract the rotation from the transformation.
  Amg::RotationMatrix3D rot = trans.rotation();

  // The theta component is what we're interested in.
//  return M_PI/2. - rot.getTheta();
  double theta = Amg::getPhiThetaPsi(rot)[1];
  return M_PI/2. - theta;
}

std::vector<int> StoreGateGeometryReader::getPhiVectorFromString(std::string phiString)
{
  std::vector<int> phis;
  QString lets_split = phiString.c_str();
  QStringList phiList = lets_split.split(" ");
  for (int i = 0; i < phiList.size(); i++)
    {
      QString phiString = phiList.at(i);
      phis.push_back(phiString.toInt());
    }
  return phis;
}

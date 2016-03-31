/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarModuleProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
#include <InDetTrackingGeometryXML/XMLReaderSvc.h>
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
#include "ISF_FatrasDetDescrModel/PlanarOverlapDescriptor.h"
#include "ISF_FatrasDetDescrModel/DiscOverlapDescriptor.h"
#include "ISF_FatrasDetDescrAlpine/PlanarModuleProvider.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometryAlpine/AlpineLayer.h"
#include "TMath.h"

iFatras::PlanarModuleProvider::PlanarModuleProvider(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","InDetXMLReaderSvc"),
  m_pixElementMapName("Pixel_IdHashDetElementMap"),
  m_sctElementMapName("SCT_IdHashDetElementMap")
{
  declareInterface<IModuleProvider>(this);

  declareProperty("InDetXMLReader", m_xmlReader);

  m_pixElementMap = 0;
  m_sctElementMap = 0;
}

iFatras::PlanarModuleProvider::~PlanarModuleProvider()
{
}

StatusCode iFatras::PlanarModuleProvider::initialize()
{
  ATH_MSG_INFO("iFatras::PlanarModuleProvider::initialize()");
  
  const Trk::LayerMaterialProperties* material = m_xmlReader->getHomogeneousMaterial("DefaultPixelModuleMaterial");
  ATH_MSG_INFO("Module Material Properties (DefaultPixelModuleMaterial) : " << *(material));

 // Retrieve helpers & tools from detector store
  ATH_MSG_INFO("Retrieve helpers & tools from detector store");
  if (detStore()->retrieve(m_atlasIdHelper, "AtlasID").isFailure())
    ATH_MSG_ERROR("Could not get Atlas ID helper");

  if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
    ATH_MSG_ERROR("Could not get Pixel ID helper");

  if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure())
    ATH_MSG_ERROR("Could not get SCT ID helper");

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  // Initialize IdHash to Pixel DetElement map for digitization
  m_pixElementMap = new iFatras::IdHashDetElementCollection;
  //Retrieve and/or store the map with IdHash to DetElement
  if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_pixElementMapName))){
    if((detStore()->retrieve(m_pixElementMap, m_pixElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_pixElementMapName);
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO("Found and Retrieved collection " << m_pixElementMapName );
  } else {
    if((detStore()->record(m_pixElementMap, m_pixElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_pixElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("Stored and Retrieved collection " << m_pixElementMapName);
  }

  // Initialize IdHash to SCT DetElement map for digitization
  m_sctElementMap = new iFatras::IdHashDetElementCollection;
  //Retrieve and/or store the map with IdHash to DetElement
  if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_sctElementMapName))){
    if((detStore()->retrieve(m_sctElementMap, m_sctElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_sctElementMapName);
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO("Found and Retrieved collection " << m_sctElementMapName );
  } else {
    if((detStore()->record(m_sctElementMap, m_sctElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_sctElementMapName);
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO("Stored and Retrieved collection " << m_sctElementMapName);
  }

  return StatusCode::SUCCESS;
}

StatusCode iFatras::PlanarModuleProvider::finalize()
{
  ATH_MSG_INFO("iFatras::PlanarModuleProvider::finalize()");

  return StatusCode::SUCCESS;
}

Trk::TrkDetElementBase* iFatras::PlanarModuleProvider::getDetElement(const Identifier &id, 
								const IdentifierHash & idHash, 
								InDet::ModuleTmp* moduleTmp, 
  							        const Amg::Vector3D transl,
								const Amg::Transform3D & transf,
								const bool isPixel,
								const bool isBarrel,
								const bool isOuterMost,
								const bool debug,
								bool   useDiscSurface,
								double rMin, 
								double rMax,
								double stereo)
{
  if(moduleTmp ==0) return 0;

  double thickness    = moduleTmp->thickness_sensor;
  double lengthY      = moduleTmp->length; 
  double lengthXmin   = moduleTmp->widthmin; 
  double lengthXmax   = moduleTmp->widthmax;
  
  // create the material
  const Trk::LayerMaterialProperties* material = m_xmlReader->getHomogeneousMaterial("DefaultPixelModuleMaterial");

  // create the planarDetElement
  // Warning : naming inconsistency between GeoModel database and PlanarDet interface
  const double pitchX = moduleTmp->pitchPhi;  
  const double pitchY = moduleTmp->pitchEta;  

  iFatras::PlanarDetElement* elem = new iFatras::PlanarDetElement(id, idHash, transl, transf, *material, thickness, 
								  lengthY, lengthXmin, lengthXmax, rMin, rMax, pitchX, pitchY,
								  stereo, isPixel, isBarrel, isOuterMost, !useDiscSurface, debug);

  ATH_MSG_DEBUG("PlanarDetElement Material Property Input: " << *(material));
  ATH_MSG_DEBUG("PlanarDetElement Material Property Output: " << *(elem->surface().materialLayer()->layerMaterialProperties()));

  return elem;
}


void iFatras::PlanarModuleProvider::setIdentifier(bool isPix, Identifier& id, IdentifierHash& idhash, 
						int brl_ec, unsigned int ilayer, int iphi, int ieta, int side)
{
  ATH_MSG_DEBUG("Setting identifier for: Pixels=" << isPix << " brl_ec=" << brl_ec << " layer=" << ilayer 
		<< " phi=" << iphi << " eta=" << ieta << " side=" << side);

  if(isPix) { // PIXEL
    id = m_pixIdHelper->wafer_id(brl_ec,ilayer,iphi,ieta);
    idhash = m_pixIdHelper->wafer_hash(m_pixIdHelper->wafer_id(id));
    ATH_MSG_DEBUG("PIX id=" << id  << " " <<  m_pixIdHelper->wafer_id(id) << " idhash=" << idhash);
  } else { // SCT
    id = m_sctIdHelper->wafer_id(brl_ec,ilayer,iphi,ieta,side);
    idhash = m_sctIdHelper->wafer_hash(m_sctIdHelper->wafer_id(id));
    ATH_MSG_DEBUG("SCT id=" << id << " " <<  m_sctIdHelper->wafer_id(id)<< " idhash=" << idhash);
  }

  bool isSCT = m_atlasIdHelper->is_sct(id);
  if(isSCT && isPix)
    ATH_MSG_INFO("*** Bad SCT flag for PIX: Is SCT=" << isSCT); 
  else if(!isSCT && !isPix)
    ATH_MSG_INFO("### Bad SCT flag for SCT: Is SCT=" << isSCT); 
  /*
  else
    ATH_MSG_INFO("   Is SCT=" << isSCT); 
  */
}

Amg::Transform3D iFatras::PlanarModuleProvider::getTransform(double R, double dR, double xshift, double zpos, 
							     double tilt, double stereo, double rot, double phi, 
							     bool useDiscSurface)  const {

  // Identity rotation
   Amg::RotationMatrix3D rotationId;     
  rotationId=
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());
  
  // ------ Initial position :  module in proper direction along Z for phi=0
  //                            + rotation + tilt
  Amg::RotationMatrix3D rotationInit;     
  rotationInit=
    Amg::AngleAxis3D(tilt, Amg::Vector3D::UnitZ())*             // last transform
    Amg::AngleAxis3D(stereo, Amg::Vector3D::UnitX())*           // ^
    Amg::AngleAxis3D(rot, Amg::Vector3D::UnitY())*              // |
    Amg::AngleAxis3D(TMath::Pi()*.5, Amg::Vector3D::UnitX())*   // |
    Amg::AngleAxis3D(TMath::Pi()*.5, Amg::Vector3D::UnitY())*   // |
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());               // first transform

  Amg::Transform3D transformInit(rotationInit,Amg::Vector3D(0.,0.,0.) );
  
  // -------- Radial and lateral shifts ( shifts are defined in the stave coordinates system )
  //                                        radial shift : perpendicular to the stave
  //                                        lateral shift : lateraly / stave
  double xRadialPos=dR*cos(tilt);
  double yRadialPos=dR*sin(tilt);
  Amg::Transform3D transformRadialShift(rotationId,Amg::Vector3D(xRadialPos,yRadialPos,0.) );

  double xLatPos=xshift*-sin(tilt);
  double yLatPos=xshift*cos(tilt);
  Amg::Transform3D transformLatShift(rotationId,Amg::Vector3D(xLatPos,yLatPos,0.) );

  Amg::Transform3D transformShift = transformLatShift*transformRadialShift;

  // ------ Radial position  ( vs layer radius )   at phi =0
  Amg::Vector3D radial_transl(R,0.,zpos);
  Amg::Transform3D transformRadPos(rotationId , radial_transl);

  // ------ Phi rotation
  Amg::RotationMatrix3D rotationPhi;     
  rotationPhi=
    Amg::AngleAxis3D(phi, Amg::Vector3D::UnitZ())* // you need TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())* // you need TMath::Pi()/2 to make the correct rotation
    Amg::AngleAxis3D(0., Amg::Vector3D::UnitY());
  Amg::Transform3D transformPhi(rotationPhi,Amg::Vector3D(0.,0.,0.) );

  // global Transform3D  
  Amg::Transform3D transform = transformPhi*transformRadPos*transformShift*transformInit;

  /*
  ATH_MSG_DEBUG("1) Translation along x-axis: " << Amg::toString(translX)); // only for transition modules
  ATH_MSG_DEBUG("2) Initial Rotation tilt+angle: " << Amg::toString(rotationInit)); // angle only for mountains
  ATH_MSG_DEBUG("3) Rotation along z-axis (phi):    " << Amg::toString(rotationPhi));  // everyone - stave position
  ATH_MSG_DEBUG("4) Translation in (R-z): " << Amg::toString(transl)); // module position in R-z

  ATH_MSG_DEBUG("Total:       " << Amg::toString(transform));
  */
  
  //  IMPORTANT FOR DISCTRAPEZOIDALBOUNDS
  // If you have trapezoidal shape elements and you want to have DiscTrapezoidalBounds
  // In this case you have a DiscSurface and you need to redefine the translation vector and the rotation matrix
  // --> For the centerOnModule use the medium point of the height --> transform.translation()
  // --> For the translation use the center of the disc surface
  // if you have no stereo angle it is (0., 0., z) for the disc
  // if you have a stereo angle != 0. then you need to shift it accordingly to the tilt angle
  // --> The rotation matrix will be the identity

  if (useDiscSurface) {
    Amg::Vector3D discTranslation = transform.translation();
    /*discTranslation.x() = -zpos/fabs(zpos)*2.*R*sin(tilt/2.)*sin(tilt/2.+ M_PI + phi); 
    discTranslation.y() = zpos/fabs(zpos)*2.*R*sin(tilt/2.)*cos(tilt/2.+ M_PI + phi);
    */ 
    
    discTranslation.x() = -2.*R*sin(tilt/2.)*sin(tilt/2.+ M_PI + phi); 
    discTranslation.y() = 2.*R*sin(tilt/2.)*cos(tilt/2.+ M_PI + phi);
    
    Amg::RotationMatrix3D discRotation;
    discRotation = Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
		   Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
		   Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());   
		   
    return Amg::Transform3D(discRotation, discTranslation);
  }
  
  return transform;
  
}


Trk::OverlapDescriptor* iFatras::PlanarModuleProvider::getDiscOverlapDescriptor(bool) const
{
  return new iFatras::DiscOverlapDescriptor();
}

Trk::OverlapDescriptor* iFatras::PlanarModuleProvider::getDiscOverlapDescriptor(bool, Trk::BinnedArray<Trk::Surface>* binnedArray, std::vector< Trk::BinUtility*>* subBinUtilities) const
{
  return new iFatras::DiscOverlapDescriptor(binnedArray,subBinUtilities);
}

Trk::OverlapDescriptor* iFatras::PlanarModuleProvider::getPlanarOverlapDescriptor(bool) const
{
  return new iFatras::PlanarOverlapDescriptor();
}

void iFatras::PlanarModuleProvider::setPhiNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* next) const
{
  iFatras::PlanarDetElement* elem1 = (iFatras::PlanarDetElement *) elem;
  iFatras::PlanarDetElement* elem2 = (iFatras::PlanarDetElement *) next;
  elem1->setPrevInPhi(elem2);
  elem2->setNextInPhi(elem1);

  // take care of stereo face
  if (elem1->otherSide() && elem2->otherSide()) {
    iFatras::PlanarDetElement *otherSide = (iFatras::PlanarDetElement *) elem1->otherSide();
    iFatras::PlanarDetElement *prevOtherSide = (iFatras::PlanarDetElement *) elem2->otherSide();
    otherSide->setPrevInPhi(prevOtherSide);
    prevOtherSide->setNextInPhi(otherSide);
  }
}

void iFatras::PlanarModuleProvider::setEtaNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* next) const
{
  iFatras::PlanarDetElement* elem1 = (iFatras::PlanarDetElement *) elem;
  iFatras::PlanarDetElement* elem2 = (iFatras::PlanarDetElement *) next;
  elem1->setPrevInEta(elem2);
  elem2->setNextInEta(elem1);

  // take care of stereo face
  if (elem1->otherSide() && elem2->otherSide()) {
    iFatras::PlanarDetElement *otherSide = (iFatras::PlanarDetElement *) elem1->otherSide();
    iFatras::PlanarDetElement *prevOtherSide = (iFatras::PlanarDetElement *) elem2->otherSide();
    otherSide->setPrevInEta(prevOtherSide);
    prevOtherSide->setNextInEta(otherSide);
  }
}

void iFatras::PlanarModuleProvider::setFrontAndBackSides(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* back)   const
{
  iFatras::PlanarDetElement* elem1 = (iFatras::PlanarDetElement *) elem;
  iFatras::PlanarDetElement* elem2 = (iFatras::PlanarDetElement *) back;
  
  elem1->setBackSide(elem2);  
  elem2->setFrontSide(elem1); 
}

void iFatras::PlanarModuleProvider::mapSurfaceDetElement(bool isPixel, const Trk::Surface* surf) const
{
  IdentifierHash idhash(0);

  const iFatras::PlanarDetElement* cpElement  = dynamic_cast<const iFatras::PlanarDetElement*>(surf->associatedDetectorElement());
  iFatras::PlanarDetElement* pElement  = const_cast<iFatras::PlanarDetElement*>(cpElement);
  if(pElement) {
    idhash = pElement->identifyHash();
    if(isPixel) m_pixElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, pElement)); 
    else m_sctElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, pElement)); 
    if (pElement->otherSide()) {
      idhash = pElement->otherSide()->identifyHash();
      if(isPixel) m_pixElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, pElement->otherSide())); 
      else m_sctElementMap->insert(std::pair<IdentifierHash, iFatras::PlanarDetElement*>(idhash, pElement->otherSide())); 
    }
  }
}

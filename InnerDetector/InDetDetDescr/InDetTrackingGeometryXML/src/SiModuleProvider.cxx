/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiModuleProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "InDetTrackingGeometryXML/SiModuleProvider.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementMap.h"
#include "GaudiKernel/IToolSvc.h"
#include "InDetTrackingGeometryUtils/DiscOverlapDescriptor.h"
#include "InDetTrackingGeometryUtils/PixelOverlapDescriptor.h"
#include "InDetTrackingGeometryUtils/SCT_OverlapDescriptor.h"
#include "TMath.h"

InDet::SiModuleProvider::SiModuleProvider(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  detElementMap(0)
{
  declareInterface<IModuleProvider>(this);
}

InDet::SiModuleProvider::~SiModuleProvider()
{
}

StatusCode InDet::SiModuleProvider::initialize()
{
  ATH_MSG_INFO("InDet::SiModuleProvider::initialize()");

  // Retrieve helpers & tools from detector store
  ATH_MSG_INFO("Retrieve helpers & tools from detector store");
  if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
    ATH_MSG_ERROR("Could not get Pixel ID helper");
  
  if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure())
    ATH_MSG_ERROR("Could not get SCT ID helper");

  // Retrieve DetElement maps
  if(detStore()->retrieve(detElementMap, "SiDetectorElementMap").isFailure())
    ATH_MSG_WARNING("Could not get SiDetectorElementMap");
  else
    ATH_MSG_INFO("SiDetectorElementMap retrieved from detstore");

  return StatusCode::SUCCESS;
}

StatusCode InDet::SiModuleProvider::finalize()
{
  ATH_MSG_INFO("InDet::SiModuleProvider::finalize()");

  return StatusCode::SUCCESS;
}

void InDet::SiModuleProvider::setIdentifier(bool isPix, Identifier& id, IdentifierHash& idhash, 
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
}

Trk::TrkDetElementBase *InDet::SiModuleProvider::getDetElement(const Identifier & /* id */, 
							       const IdentifierHash & idHash, 
							       InDet::ModuleTmp* /* moduleTmp */, 
							       const Amg::Vector3D /* transl */,
							       const Amg::Transform3D & /* transf */,
							       const bool /* isPixel */,
							       const bool /* isBarrel */,
							       const bool /* isOuterMost */,
							       const bool /* debug */,
							       bool   /*useDiscSurface*/,
							       double /*rmin*/,
							       double /*rmax*/,
							       double /*stereo*/)
{  
  // Get DetElement using FastGeoModel id/elem map
  const InDetDD::SiDetectorElement* elem = detElementMap->readEntry(idHash);
  return (InDetDD::SiDetectorElement *) elem;
}

Amg::Transform3D InDet::SiModuleProvider::getTransform(double R, double dR, double xshift, double zpos, 
						       double tilt, double stereo, double rot, double phi,
						       bool useDiscSurface) const 
{
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
    discTranslation.x() = -2.*dR*sin(tilt/2.)*sin(tilt/2.+ M_PI + phi); 
    discTranslation.y() = 2.*dR*sin(tilt/2.)*cos(tilt/2.+ M_PI + phi);
        
    Amg::RotationMatrix3D discRotation;
    discRotation = Amg::AngleAxis3D(0., Amg::Vector3D::UnitX())*
		   Amg::AngleAxis3D(0., Amg::Vector3D::UnitY())*
		   Amg::AngleAxis3D(0., Amg::Vector3D::UnitZ());   
		   
    return Amg::Transform3D(discRotation, discTranslation);
  }
    
  return transform;
  
}


Trk::OverlapDescriptor* InDet::SiModuleProvider::getDiscOverlapDescriptor(bool isPixel) const
{
  return new InDet::DiscOverlapDescriptor(isPixel);
}

Trk::OverlapDescriptor* InDet::SiModuleProvider::getDiscOverlapDescriptor(bool isPixel, Trk::BinnedArray<Trk::Surface>* binnedArray, std::vector< Trk::BinUtility*>* subBinUtilities) const
{
  return new InDet::DiscOverlapDescriptor(isPixel,binnedArray,subBinUtilities);
}

Trk::OverlapDescriptor* InDet::SiModuleProvider::getPlanarOverlapDescriptor(bool isPixel) const
{
  return isPixel ? (Trk::OverlapDescriptor*) new InDet::PixelOverlapDescriptor() : (Trk::OverlapDescriptor*) new InDet::SCT_OverlapDescriptor();
}

void InDet::SiModuleProvider::setPhiNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev) const
{
  InDetDD::SiDetectorElement* elem1 = (InDetDD::SiDetectorElement *) elem;
  InDetDD::SiDetectorElement* elem2 = (InDetDD::SiDetectorElement *) prev;
  elem1->setPrevInPhi(elem2);
  elem2->setNextInPhi(elem1);

  // take care of stereo face
  if (elem1->otherSide() && elem2->otherSide()) {
    InDetDD::SiDetectorElement *otherSide = (InDetDD::SiDetectorElement *) elem1->otherSide();
    InDetDD::SiDetectorElement *prevOtherSide = (InDetDD::SiDetectorElement *) elem2->otherSide();
    otherSide->setPrevInPhi(prevOtherSide);
    prevOtherSide->setNextInPhi(otherSide);
  }
}


void InDet::SiModuleProvider::setEtaNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev) const
{
  InDetDD::SiDetectorElement* elem1 = (InDetDD::SiDetectorElement *) elem;
  InDetDD::SiDetectorElement* elem2 = (InDetDD::SiDetectorElement *) prev;
  elem1->setPrevInEta(elem2);
  elem2->setNextInEta(elem1);

  // take care of stereo face
  if (elem1->otherSide() && elem2->otherSide()) {
    InDetDD::SiDetectorElement *otherSide = (InDetDD::SiDetectorElement *) elem1->otherSide();
    InDetDD::SiDetectorElement *prevOtherSide = (InDetDD::SiDetectorElement *) elem2->otherSide();
    otherSide->setPrevInEta(prevOtherSide);
    prevOtherSide->setNextInEta(otherSide);
  }
}

void InDet::SiModuleProvider::setFrontAndBackSides(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* back)   const
{
  InDetDD::SiDetectorElement* elem1 = (InDetDD::SiDetectorElement *) elem;
  InDetDD::SiDetectorElement* elem2 = (InDetDD::SiDetectorElement *) back;
  
  elem1->setOtherSide(elem2);
  elem2->setOtherSide(elem1);
}

void InDet::SiModuleProvider::mapSurfaceDetElement(bool /* isPixel */, const Trk::Surface* /* surf */) const
{
  return;
}

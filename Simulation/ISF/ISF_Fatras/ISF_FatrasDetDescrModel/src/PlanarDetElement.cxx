/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarDetElement.cxx
// Implementation file class PlanarDetElement
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Class header
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/DiscTrapezoidalBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkGeometry/MaterialLayer.h"

#include "ISF_FatrasDetDescrModel/PixelSegmentation.h"
#include "ISF_FatrasDetDescrModel/SCT_Segmentation.h" 

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "ReadoutGeometryBase/SiCellId.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

namespace iFatras{

  //Constructor with parameters:
  PlanarDetElement::PlanarDetElement( const Identifier &id, 
				      const IdentifierHash & idHash,
				      const Amg::Vector3D & centerOnModule,
				      const Amg::Transform3D & transf,
				      const Trk::LayerMaterialProperties & layerMaterial,
				      const double thickness,
				      const double lengthY,
				      const double lengthXmin,
				      const double lengthXmax,
				      const double rMin,
				      const double rMax,
				      const double pitchX, 
				      const double pitchY,
				      const double stereo,
				      const bool isPixel,
				      const bool isBarrel,
				      const bool isOuterMost,
				      const bool useTrapezoidal,
				      const bool debug) :
    TrkDetElementBase(0),
    m_id(id),
    m_idHash(idHash),
    m_transform(transf), // set the base class transform (tracking transform)
    m_center(m_transform.translation()),
    m_centerOnModule(centerOnModule),
    m_normal(m_transform.rotation().col(Trk::z)), // an expression of the i-th column of *this. Note that the numbering starts at 0.
    m_surface(0),
    m_lengthY(lengthY),
    m_lengthXmin(lengthXmin),
    m_lengthXmax(lengthXmax),
    m_pitchX(pitchX),
    m_pitchY(pitchY),
    m_thickness(thickness),
    m_stereo(stereo),
    m_isOuterMost(isOuterMost),
    m_debug(debug),
    m_backSide(0),
    m_frontSide(0),
    m_nextEta(0),
    m_prevEta(0),
    m_nextPhi(0),
    m_prevPhi(0),
    m_isPixel(isPixel),
    m_isBarrel(isBarrel)
  {
    double avephi = 0.;
    if (lengthXmax == 0.0 || lengthXmax == lengthXmin){
      m_surface = new Trk::PlaneSurface(*this);
      Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(lengthXmin/2., lengthY/2.);
      m_bounds = new Trk::SharedObject<const Trk::SurfaceBounds>(rbounds,  [](const Trk::SurfaceBounds*){});
      m_shape = InDetDD::Box;
      if (m_debug){
	std::cout << "DEBUG: Created Rectangle bounds " << std::endl;
      }
    } else {
      if (useTrapezoidal) {
	m_surface = new Trk::PlaneSurface(*this);
	Trk::TrapezoidBounds* tbounds = new Trk::TrapezoidBounds(lengthXmin/2., lengthXmax/2., lengthY/2.);
	m_bounds = new Trk::SharedObject<const Trk::SurfaceBounds>(tbounds, [](const Trk::SurfaceBounds*){});
	m_shape = InDetDD::Trapezoid;
	if (m_debug)
	  std::cout << "DEBUG: Created Trapezoid bounds " << std::endl;
      } else {
	m_surface = new Trk::DiscSurface(*this);

	avephi = stereo + m_centerOnModule.phi();

	Trk::DiscTrapezoidalBounds* dtbounds = new Trk::DiscTrapezoidalBounds(lengthXmin/2., lengthXmax/2., rMin, rMax, avephi, stereo);
	
	m_bounds = new Trk::SharedObject<const Trk::SurfaceBounds>(dtbounds, [](const Trk::SurfaceBounds*){});
	m_shape = InDetDD::Other;
	if (m_debug){
	  std::cout << "DEBUG: Using Parameters =  " << lengthXmin/2. << ", " << lengthXmax/2. << ", " << rMin << ", "<< rMax << ", "<< avephi << ", " << stereo << std::endl;
	  std::cout << "DEBUG: Created DiscTrapezoidal bounds " << std::endl;
	  std::cout << "Surface = " << *m_surface << std::endl;
	  std::cout << "Center = " << m_center << std::endl;
	  std::cout << "Bounds = " << *dtbounds << std::endl;
	}
      }
    }  
    
    // get the tracking rotation
    Amg::RotationMatrix3D trackingRotation(m_transform.rotation());
    
    // create the hit rotation
    Amg::RotationMatrix3D hitRotation;
    hitRotation.col(Trk::x) = trackingRotation.col(Trk::z);
    hitRotation.col(Trk::y) = trackingRotation.col(Trk::x);
    hitRotation.col(Trk::z) = trackingRotation.col(Trk::y);
    
    // and finally the transform
    m_hitTransform = Amg::Transform3D(hitRotation, m_transform.translation());
    m_hitTransformCLHEP = Amg::EigenTransformToCLHEP(m_hitTransform);
    
    if (m_debug){
      std::cout << "DEBUG: PlanarDetElement constructor:" << std::endl;
      std::cout << "DEBUG: Identifier = " << m_id << std::endl;
      std::cout << "DEBUG: IdentifierHash = " << m_idHash << std::endl;
      std::cout << "DEBUG: Transform3D = " << Amg::toString(m_transform) << std::endl;
      std::cout << "DEBUG: HitTransform3D = " << Amg::toString(m_hitTransform) << std::endl;
    }
    
    if (!m_id.is_valid()) throw std::runtime_error("PlanarDetElement: Invalid identifier");
    
    if (!m_idHash.is_valid()) throw std::runtime_error("PlanarDetElement: Unable to set IdentifierHash");
    
    const Trk::MaterialLayer* materialLayer = new Trk::MaterialLayer(Trk::SharedObject<Trk::Surface>(m_surface), layerMaterial);
    m_surface->setMaterialLayer(*materialLayer);
    
    if (isPixel)
      m_segmentation = dynamic_cast<PixelSegmentation*> (new PixelSegmentation(m_lengthXmin, m_lengthY, m_pitchX, m_pitchY));
    else {
      if (m_lengthXmax == 0.) m_segmentation = dynamic_cast<SCT_Segmentation*> (new SCT_Segmentation(m_lengthXmin, m_lengthXmax, m_lengthY, m_pitchX));
      else {
	m_segmentation = useTrapezoidal ? 
	  dynamic_cast<SCT_Segmentation*> (new SCT_Segmentation(m_lengthXmin, m_lengthXmax, m_lengthY, m_pitchX)) :
	  dynamic_cast<SCT_Segmentation*> (new SCT_Segmentation(m_lengthXmin, m_lengthXmax, rMin, rMax, m_pitchX, avephi));
      }      
    }    
  }
  
  // Destructor:
  PlanarDetElement::~PlanarDetElement()
  {
    delete m_surface;
  }

  const Trk::Surface& PlanarDetElement::surface() const
  {
    if (!m_surface) m_surface = new Trk::PlaneSurface(*this);
    return *m_surface;
  }

  const std::vector<const Trk::Surface*>& PlanarDetElement::surfaces() const  { 
    if (!m_surfaces.size()){ 
      // get this surface 
      m_surfaces.push_back(&surface()); 
      // get the other side surface 
      if (otherSide()){ 
	m_surfaces.push_back(&(otherSide()->surface())); 
      } 
    } 
    // return the surfaces 
    return m_surfaces; 
  }   
  
  bool PlanarDetElement::cellOfPosition(const Amg::Vector2D &localPos, std::pair<int, int>& entryXY) const {
    return m_segmentation->cellOfPosition(localPos, entryXY);
  }  

  Amg::Vector2D PlanarDetElement::localPositionOfCell(const Identifier & id) const {
    
    Amg::Vector2D local;
    if (id.is_valid()) {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      StoreGateSvc *detStore;
      StatusCode sc = svcLocator->service("DetectorStore", detStore);
      if (sc.isFailure()) {
	std::cout << "PlanarDetElement::localPositionOfCell --- ERROR: DetectorStore service not found!" << std::endl;
	return local;
      } 

      if (isPixel()){
	
	const PixelID * pixId = 0;
	sc = detStore->retrieve(pixId, "PixelID");
	if (sc.isFailure()) {
	  std::cout << "PlanarDetElement::localPositionOfCell --- ERROR: Could not get PixelID helper!" << std::endl;
	  return local;
	} 
	
	local = m_segmentation->localPositionOfCell(InDetDD::SiCellId(pixId->phi_index(id), pixId->eta_index(id)));
      
      } else {
	const SCT_ID * sctId = 0;
	sc = detStore->retrieve(sctId, "SCT_ID");
	if (sc.isFailure()) {
	  std::cout << "PlanarDetElement::localPositionOfCell --- ERROR: Could not get SCT_ID helper!" << std::endl;
	  return local;
	} 
	
	local = m_segmentation->localPositionOfCell(InDetDD::SiCellId(sctId->strip(id)));
      }
    }
    return local;
  }
}

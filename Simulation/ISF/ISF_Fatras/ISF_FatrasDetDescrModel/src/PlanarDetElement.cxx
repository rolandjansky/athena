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
#include "TrkGeometry/MaterialLayer.h"

#include "ISF_FatrasDetDescrModel/PixelSegmentation.h"

namespace iFatras{

  //Constructor with parameters:
  PlanarDetElement::PlanarDetElement( const Identifier &id, 
				      const IdentifierHash & idHash,
				      const Amg::Transform3D & transf,
				      const Trk::LayerMaterialProperties & layerMaterial,
				      const double thickness,
				      const double lengthY,
				      const double lengthXmin,
				      const double lengthXmax,
				      const double pitchX, 
				      const double pitchY,
				      const bool isPixel,
				      const bool isBarrel,
				      const bool isOuterMost,
				      const bool debug) :
    TrkDetElementBase(0),
    m_id(id),
    m_idHash(idHash),
    m_transform(transf), // set the base class transform (tracking transform)
    m_center(m_transform.translation()),
    m_normal(m_transform.rotation().col(Trk::z)), // an expression of the i-th column of *this. Note that the numbering starts at 0.
    m_surface(*this),
    m_lengthY(lengthY),
    m_lengthXmin(lengthXmin),
    m_lengthXmax(lengthXmax),
    m_pitchX(pitchX),
    m_pitchY(pitchY),
    m_thickness(thickness),
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
    
    if (lengthXmax == 0.0){
      
      Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(lengthXmin/2., lengthY/2.);
      m_bounds = new Trk::SharedObject<const Trk::SurfaceBounds>(rbounds, true);
      if (m_debug){
	std::cout << "DEBUG: Created Rectangle bounds " << std::endl;
      }
    } else {
    
      Trk::TrapezoidBounds* tbounds = new Trk::TrapezoidBounds(lengthXmin/2., lengthXmax/2., lengthY/2.);
      m_bounds = new Trk::SharedObject<const Trk::SurfaceBounds>(tbounds, true);
      if (m_debug){
	std::cout << "DEBUG: Created Trapezoid bounds " << std::endl;
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

    const Trk::MaterialLayer* materialLayer = new Trk::MaterialLayer(m_surface, layerMaterial);
    m_surface.setMaterialLayer(*materialLayer);

    m_segmentation = dynamic_cast<PixelSegmentation*> (new PixelSegmentation(m_lengthXmin, m_lengthY, m_pitchX, m_pitchY));
  }

  // Destructor:
  PlanarDetElement::~PlanarDetElement()
  {}

  std::pair<int, int> PlanarDetElement::cellsOfPosition(const Amg::Vector2D &localPos) const {
    return m_segmentation->cellsOfPosition(localPos);
  }   

  bool PlanarDetElement::isLastCellIdValid() const {
    
    return m_segmentation->isIdValid();

  }
  
}

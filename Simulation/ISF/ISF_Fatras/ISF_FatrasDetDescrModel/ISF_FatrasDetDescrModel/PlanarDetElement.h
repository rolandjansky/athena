/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarDetElement.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRMODEL_PLANARDETELEMENT_H
#define ISF_FATRASDETDESCRMODEL_PLANARDETELEMENT_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

// Data member classes
#include "Identifier/Identifier.h" 
#include "Identifier/IdentifierHash.h" 
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkGeometry/LayerMaterialProperties.h"

// Math & Geometry library
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "ISF_FatrasDetDescrInterfaces/ISegmentation.h"



namespace iFatras {

  /**
     @class PlanarDetElement
     Class to hold geometrical description of a planar silicon detector element. 

     @author Noemi Calace
     
  */
  
  class PlanarDetElement : public Trk::TrkDetElementBase {
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:

    /// Constructor:
    PlanarDetElement( const Identifier &id, 
		      const IdentifierHash & idHash,
		      const Amg::Transform3D & transf,
		      const Trk::LayerMaterialProperties & layerMaterial,
		      const double thickness,
		      const double lengthY,
		      const double lengthXmin,
		      const double lengthXmax = 0,
		      const double pitchX = 0,
		      const double pitchY = 0,
		      const bool isPixel = true,
		      const bool isBarrel = true,
		      const bool isOuterMost = false,
		      const bool debug = false);

    /// Copy-Constructor:
    PlanarDetElement( const PlanarDetElement& );
    
    /// Destructor:
    ~PlanarDetElement();

    /*---------------------------------------------------------------------
     *  From TrkDetElementBase
     *---------------------------------------------------------------------*/
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Identification
    /// Methods to identify the element and identifier manipulation.
    // 
    ///////////////////////////////////////////////////////////////////
  
    //@{
    
    // Identifier
    virtual Identifier identify() const;

    // Identifier hash
    virtual IdentifierHash identifyHash() const;

    //@}

    ///////////////////////////////////////////////////////////////////
    //
    /// @name Transformation/Orientation
    //
    ///////////////////////////////////////////////////////////////////
    
    //@{
    
    // Position 
    /// Local (reconstruction frame) to global transform
    const Amg::Transform3D & transform() const {return m_transform;};
    /** Return local to global transform associated with this identifier
	In the case of silicon it returns the same as transform()*/
    const Amg::Transform3D & transform(const Identifier&) const {return transform();};

    /// Local (simulation/hit frame) to global transform
    const Amg::Transform3D & HitTransform() const { return m_hitTransform;};
    const HepGeom::Transform3D & transformHit() const { return m_hitTransformCLHEP;};
    
    /// Center in global coordinates
    const Amg::Vector3D & center() const {return m_center;};

    /**Return the center of the surface associated with this identifier
       In the case of silicon it returns the same as center()*/  
    const Amg::Vector3D & center(const Identifier&) const {return center();};

    /// Get reconstruction local normal axes in global frame. Choosen to give right-handed coordinate frame.
    const Amg::Vector3D & normal() const {return m_normal;};
    
    /**Return the normal of the surface associated with this identifier
       In the case of silicon it returns the same as normal()*/  
    const Amg::Vector3D & normal(const Identifier&) const {return normal();};

    /// Element Surface
    const Trk::Surface & surface () const {return m_surface;};
    /**Return the surface associated with this identifier
       In the case of silicon it returns the same as surface()*/  
    const Trk::Surface & surface (const Identifier&) const {return surface();};

    // Return boundaries of the element
    const Trk::SurfaceBounds & bounds() const ;
    /**Return the boundaries of the surface associated with this identifier
       In the case of silicon it returns the same as bounds()*/  
    const Trk::SurfaceBounds & bounds(const Identifier&) const {return bounds();};

    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Design methods
    //
    ///////////////////////////////////////////////////////////////////
    //@{
    
    double thickness() const;
    double pitchX() const; 
    double pitchY() const;
    double lengthY() const;
    double lengthXmin() const;
    double lengthXmax() const;
    bool isOuterMost() const;
    bool isPixel() const;
    bool isBarrel() const;
    bool isSCT() const;
    std::pair<int, int> cellsOfPosition(const Amg::Vector2D &localPos) const;
    bool isLastCellIdValid() const;

    //@}
    
    ///////////////////////////////////////////////////////////////////
    //
    /// @name Navigation
    /// Methods to access neighbours. 
    //
    ///////////////////////////////////////////////////////////////////
    
    //@{
    PlanarDetElement * backSide() const; // Useful for SCT-like only
    void setBackSide(PlanarDetElement *); // For SCT-like only
    PlanarDetElement * frontSide() const; // Useful for SCT-like only
    void setFrontSide(PlanarDetElement *); // For SCT-like only
    PlanarDetElement * otherSide() const; // Useful for SCT-like only
        
    PlanarDetElement * nextInPhi() const; 
    void setNextInPhi(PlanarDetElement *);
    PlanarDetElement * nextInEta() const; 
    void setNextInEta(PlanarDetElement *);
    PlanarDetElement * prevInPhi() const; 
    void setPrevInPhi(PlanarDetElement *);
    PlanarDetElement * prevInEta() const; 
    void setPrevInEta(PlanarDetElement *);
    //@}

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
  protected:

    Identifier m_id; // identifier of this detector element
    IdentifierHash m_idHash; // hash id

    mutable Amg::Transform3D m_transform; 
    mutable Amg::Transform3D m_hitTransform;
    mutable HepGeom::Transform3D m_hitTransformCLHEP;
    
    mutable Amg::Vector3D    m_center;
    mutable Amg::Vector3D    m_normal;
    
    mutable Trk::PlaneSurface m_surface;
    Trk::SharedObject<const Trk::SurfaceBounds> * m_bounds;

    double m_lengthY;
    double m_lengthXmin;
    double m_lengthXmax;
    double m_pitchX;
    double m_pitchY;
    double m_thickness;

    bool m_isOuterMost;
    bool m_debug;
    
    PlanarDetElement * m_backSide;
    PlanarDetElement * m_frontSide;
    PlanarDetElement * m_nextEta;
    PlanarDetElement * m_prevEta;
    PlanarDetElement * m_nextPhi;
    PlanarDetElement * m_prevPhi;
    
    bool m_isPixel;
    bool m_isBarrel;
    
    ISegmentation * m_segmentation;

  };
  
  inline Identifier PlanarDetElement::identify() const
    {
      return m_id;
    }
  
  inline IdentifierHash PlanarDetElement::identifyHash() const
    {
      return m_idHash;
    }
  
  inline double PlanarDetElement::thickness() const
  {
    return m_thickness;
  }
  
  inline const Trk::SurfaceBounds & 
    PlanarDetElement::bounds() const
  {
    return m_bounds->getRef();
  }
  
  inline double PlanarDetElement::pitchX() const 
  {  
    return m_segmentation->pitchX();
  }  
  
  inline double PlanarDetElement::pitchY() const 
  { 
    return m_segmentation->pitchY();
  } 

  inline double PlanarDetElement::lengthY()const
  {
    return m_lengthY;
  }

    inline double PlanarDetElement::lengthXmin() const
  {
    return m_lengthXmin;
  }

  inline double PlanarDetElement::lengthXmax() const
  {
    
    return m_lengthXmax;
  }

  
  inline bool PlanarDetElement::isOuterMost() const
    {
      return m_isOuterMost;
    }
  
  inline PlanarDetElement * PlanarDetElement::backSide() const
  {
    return m_backSide;
  }
  
  inline void PlanarDetElement::setBackSide(PlanarDetElement * element) // For SCT-like
  {
    m_backSide = element;
  }

  inline PlanarDetElement * PlanarDetElement::frontSide() const
  {
    return m_frontSide;
  }
  
  inline void PlanarDetElement::setFrontSide(PlanarDetElement * element) // For SCT-like
  {
    m_frontSide = element;
  }

  inline PlanarDetElement * PlanarDetElement::otherSide() const
  {
    if (m_backSide) return m_backSide;
    else return m_frontSide;
  }
      
  inline PlanarDetElement * PlanarDetElement::nextInPhi() const
  {
    return m_nextPhi;
  }
  
  inline void PlanarDetElement::setNextInPhi(PlanarDetElement * element) 
  {
    m_nextPhi = element;
    if(m_backSide) m_backSide->setNextInPhi(element->backSide());
  }

  inline PlanarDetElement * PlanarDetElement::nextInEta() const
  {
    return m_nextEta;
  }
  
  inline void PlanarDetElement::setNextInEta(PlanarDetElement * element) 
  {
    m_nextEta = element;
    if(m_backSide) m_backSide->setNextInEta(element->backSide());
  }

  inline PlanarDetElement * PlanarDetElement::prevInPhi() const
  {
    return m_prevPhi;
  }
  
  inline void PlanarDetElement::setPrevInPhi(PlanarDetElement * element) 
  {
    m_prevPhi = element;
    if(m_backSide) m_backSide->setPrevInPhi(element->backSide());
  }

  inline PlanarDetElement * PlanarDetElement::prevInEta() const
  {
    return m_prevEta;
  }
  
  inline void PlanarDetElement::setPrevInEta(PlanarDetElement * element) 
  {
    m_prevEta = element;
    if(m_backSide) m_backSide->setPrevInEta(element->backSide());
  }
  
  inline bool PlanarDetElement::isPixel() const {
   return m_isPixel; 
  }

  inline bool PlanarDetElement::isBarrel() const {
    return m_isBarrel; 
  }
  
  inline bool PlanarDetElement::isSCT() const {
   return !m_isPixel; 
  }

}

#endif // ISF_FATRASDETDESCRMODEL_PLANARDETELEMENT_H

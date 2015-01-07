/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrismVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_PRISMVOLUMEBOUNDS_H
#define TRKVOLUMES_PRISMVOLUMEBOUNDS_H
    
// Trk
#include "TrkVolumes/VolumeBounds.h"
#include "TrkDetDescrUtils/EightObjectsAccessor.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

  class Surface;
  class PlaneSurface;
  class TriangleBounds;

   
  /**
   @class PrismVolumeBounds
    
   Bounds for the transcript of triangular prism
      
    BoundarySurfaceFace [index]:
        
        - negativeFaceXY     [0] : Triangular Trk::PlaneSurface, 
                                   parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY     [1] : Triangular Trk::PlaneSurface, 
                                   parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - face [2... n+1] : Rectangular  Trk::PlaneSurface
              
    @author Sarka.Todorova@cern.ch 
    */
    
 class PrismVolumeBounds : public VolumeBounds {
  
  public:
    /**Default Constructor*/
    PrismVolumeBounds();
                                                                                               
    /**Constructor - generic case (from float)*/
    PrismVolumeBounds(std::vector<std::pair<float,float> > xyvtx, float hlengthz);
    
    /**Constructor - generic case from (double)*/
    PrismVolumeBounds(std::vector<std::pair<double,double> > xyvtx, double hlengthz);
    
    /**Copy Constructor */
    PrismVolumeBounds(const PrismVolumeBounds& bobo);
    
    /**Destructor */
    virtual ~PrismVolumeBounds();
    
    /**Assignment operator*/
    PrismVolumeBounds& operator=(const PrismVolumeBounds& bobo);
    
    /**Virtual constructor */
    PrismVolumeBounds* clone() const override;
    
    /**This method checks if position in the 3D volume frame is inside the volume*/     
    bool inside(const Amg::Vector3D& , double tol=0.) const override;
         
    /** Method to decompose the Bounds into Surfaces */
    const std::vector<const Trk::Surface*>* decomposeToSurfaces(const Amg::Transform3D& transform) const override;
    
    /** Provide accessor for BoundarySurfaces */
    const ObjectAccessor& boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                                  const Amg::Vector3D& dir,
                                                  bool forceInside=false) const override;

#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif                                                                                              
    /**This method returns the set of xy generating vertices*/
    const std::vector<std::pair<double, double> >  xyVertices() const;
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif
     
    /**This method returns the halflength in local z*/
    double halflengthZ() const;           

    /** Output Method for MsgStream*/
    MsgStream& dump(MsgStream& sl) const override;
    
    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:   
    /** method to construct side boundary planes */
    Trk::PlaneSurface* sideSurf(Amg::Transform3D,unsigned int,unsigned int) const;
    
    /** mirror the input vertices for down-side boundary */   
    std::vector<std::pair<double,double> > mirror_xyVtx() const;   
    
    /** assess ordering of vertices */ 
    int ordering() const;
 
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif    
    mutable std::vector<std::pair<double,double> > m_xyVtx; //!< generating xy vertices
    double m_halfZ;                                         //!< halflength in z
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif    
    mutable Trk::TriangleBounds* m_baseBounds;              //!< base xy bounds
    mutable int m_ordering;                                 //!< cache vertex ordering 
        
   /** There's only one single object Acessor for the moment
       has to be implemented if Cuboids are used more widely */
    Trk::EightObjectsAccessor m_objectAccessor;             //!< this is a real relict,can we get rid of it ?
  
 };

 inline PrismVolumeBounds* PrismVolumeBounds::clone() const
 { return new PrismVolumeBounds(*this); }

#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif     
 inline const std::vector<std::pair<double,double> > PrismVolumeBounds::xyVertices() const { return m_xyVtx; }
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif    
 
 
 inline double PrismVolumeBounds::halflengthZ() const { return m_halfZ; }
 
 inline const ObjectAccessor& PrismVolumeBounds::boundarySurfaceAccessor(const Amg::Vector3D&,
                                                        const Amg::Vector3D&,
                                                        bool) const
 { return(m_objectAccessor); }

}


#endif // TRKVOLUMES_PRISMVOLUMEBOUNDS_H




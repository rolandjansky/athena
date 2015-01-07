/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimplePolygonBrepVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_SIMPLEPOLYGONBREPVOLUMEBOUNDS_H
#define TRKVOLUMES_SIMPLEPOLYGONBREPVOLUMEBOUNDS_H
    
// Trk
#include "TrkVolumes/VolumeBounds.h"
#include "TrkDetDescrUtils/EightObjectsAccessor.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
    
namespace Trk {

  class Surface;
  class PlaneSurface;
  class RectangleBounds;

   
  /**
   @class SimplePolygonBrepVolumeBounds
    
   Bounds for the exact transcript of the GeoSimplePolygonBrep; volume defined by combination of symm.trapezoids
      
    BoundarySurfaceFace [index]:
        
        - negativeFaceXY     [0] : Trk::SubtractedPlaneSurface, 
                                   parallel to \f$ xy \f$ plane at negative \f$ z \f$
        - positiveFaceXY     [1] : Trk::SubtractedPlaneSurface, 
                                   parallel to \f$ xy \f$ plane at positive \f$ z \f$
        - face [2... n+1] : Rectangular  Trk::PlaneSurface
              
    @author sarka.todorova@cern.ch , marcin.wolter@cern.ch
    */
    
 class SimplePolygonBrepVolumeBounds : public VolumeBounds {
  
  public:           
    /**Default Constructor*/
    SimplePolygonBrepVolumeBounds();
        
    /**Constructor - generic case (from float)*/
    SimplePolygonBrepVolumeBounds(std::vector<std::pair<float,float> > xyvtx, float hlengthz);
    
    /**Constructor - generic case (from double)*/
    SimplePolygonBrepVolumeBounds(std::vector<std::pair<double,double> > xyvtx, double hlengthz);
    
    /**Copy Constructor */
    SimplePolygonBrepVolumeBounds(const SimplePolygonBrepVolumeBounds& bobo);
    
    /**Destructor */
    virtual ~SimplePolygonBrepVolumeBounds();
    
    /**Assignment operator*/
    SimplePolygonBrepVolumeBounds& operator=(const SimplePolygonBrepVolumeBounds& bobo);
    
    /**Virtual constructor */
    SimplePolygonBrepVolumeBounds* clone() const override;
    
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
       
    /**This method returns the transcript into combined volume*/
    const Trk::Volume* combinedVolume() const;
    
    /**This method returns the volume envelope*/
    const Trk::Volume* envelope() const;
       
    /** Output Method for MsgStream*/
    MsgStream& dump(MsgStream& sl) const override;
    
    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:  
    void processSubVols() const;
    Trk::PlaneSurface* sideSurf(Amg::Transform3D,unsigned int,unsigned int) const;
    bool Xor(bool x, bool y) const;
    
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif      
    bool Left(std::pair<double,double> a, std::pair<double,double> b, std::pair<double,double> c) const;
    
    bool Intersect(std::pair<double,double> a, std::pair<double,double> b, 
		   std::pair<double,double> c, std::pair<double,double> d) const;
		   
    bool InCone(int i, int j, std::vector<std::pair<double,double> > inputVertices) const;
    
    bool Diagonalie(int  i , int j  ,std::vector<std::pair<double,double> > inputVertices) const;
    
    bool Diagonal(int i, int j, std::vector<std::pair<double,double> > inputVertices) const;
    

    std::vector<std::pair<double,double> > TriangulatePolygon(const std::vector<std::pair<double,double> >& Vertices ) const;
    
    std::vector<std::pair<double,double> > TriangulatePolygonCheck(const std::vector<std::pair<double,double> >& Vertices ) const;

    mutable std::vector<std::pair<double,double> > m_xyVtx; //!< generating xy vertices
    double m_halfX;                                         //!< halflength in x - to define enclosing rectangle 
    double m_halfY;                                         //!< halflength in y - to define enclosing rectangle
    double m_halfZ;                                         //!< halflength in z
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif    
    
    mutable int m_ordering;                                 //!< -1 not set/ 1 anticlockwise / 0  clockwise 
    mutable const Trk::Volume* m_combinedVolume;            //!< triangulated polygon   
    mutable const Trk::Volume* m_envelope;                  //!< simplified envelope
         
   /** There's only one single object Acessor for the moment
       has to be implemented if Cuboids are used more widely */
    Trk::EightObjectsAccessor m_objectAccessor;             //!< can we get rid of this ?
 
 };

 inline SimplePolygonBrepVolumeBounds* SimplePolygonBrepVolumeBounds::clone() const
 { return new SimplePolygonBrepVolumeBounds(*this); }
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif 
 inline const std::vector<std::pair<double,double> > SimplePolygonBrepVolumeBounds::xyVertices() const { return m_xyVtx; }
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif     

 inline double SimplePolygonBrepVolumeBounds::halflengthZ() const { return m_halfZ; }
 
 inline const Trk::Volume* SimplePolygonBrepVolumeBounds::combinedVolume() const { return m_combinedVolume; }
 
 inline const Trk::Volume* SimplePolygonBrepVolumeBounds::envelope() const { return m_envelope; }
 
 inline const ObjectAccessor& SimplePolygonBrepVolumeBounds::boundarySurfaceAccessor(const Amg::Vector3D&,
                                                                                     const Amg::Vector3D&,
                                                                                     bool) const
 { return(m_objectAccessor); }

}


#endif // TRKVOLUMES_SIMPLEPOLYGONBREPVOLUMEBOUNDS_H




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          VxTrackAtVertex.h  -  Description
                             -------------------
    begin   : 03-03-2004
    changes : 28-02-2006 by Kirill Prokofiev
    email   : Kirill.Prokofiev@cern.ch    
              Added pointer to the PerigeeLinearizedTrack required by
	      iterative vertex fitters
    changes : 28-02-2006 by Giacinto Piacquadio
    changes : added support for non integer ndf < 2 (2 remains default)
              (adaptive vertexing methods)
    changes : 20-03-2006 by Kirill Prokofiev
            : Equality operator added	      
    changes : 27-03-2006 by Kirill Prokofiev	      
              added support for storage of the refitted track state
	      for vertex fitters with track smoothing.
    changes:  06-04-2006 by Giacinto Piacquadio
              added support for ParametersBase At the ImpactPoint in 3d Plane
    changes:  08-06-2006 by Kirill Prokofiev
              set methods added. Allow to use the smoothed chi2 of tracks.  
    changes:  05-12-2006 by Kirill Prokofiev
              EDM cleanup: private ElementLink, modified FitQuality instead
	      of separate ndf and chi2.             
 ***************************************************************************/

#ifndef VXVERTEX_VXTRACKATVERTEX_H
#define VXVERTEX_VXTRACKATVERTEX_H

#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "VxVertex/LinearizedTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrackLink/ITrackLink.h"

class MsgStream;

namespace Trk
{

/**
 * The VxTrackAtVertex is a common class for all present TrkVertexFitters
 * The VxTrackAtVertex is designed to store the information about the trajectory 
 * fitted to the VxCandidate.  
 * The values of VxTrackAtVertex's datamembers are set in different ways,
 * depending on which Vertex Fitter was used in the reconstruction.
 * The general functionality is the following:
 *
 * -- const initialPerigee(void) const: always returns the initial (before vertex fit)
 * perigee state  of the trajectory
 *
 * -- const Trk::Track* origTrack(void) const: returns a pointer to the original
 * track if tracks were used for the vertex fit. In case the vertex was fitted
 * directly out of a vector of ParametersBase, this pointer will be zero.
 *
 * -- const Trk::ParametersBase* perigeeAtVertex(void) const: returns a
 * perigee parameters of the trajectory defined with respect to the fitted vertex.
 * In the case of Full Billoir Vertex Fit or any iterative vertex fit (Sequential, Kalman, etc)
 * with subsequent smoothing, the trajectory is defined _at_ the vertex position: d0 = 0.; z_0 = 0. 
 *
 * In the case no refit of trajectory with  the knowledge of the vertex was performed
 * (Billoir Fast fit, iterative vertex fits without smoothing), the trajectory is just
 * extrapolated to the point of the closest approach in the transverse plane
 * to the fitted vertex and  perigee parameters with  respect to that vertex are calculated.
 *
 * @author andreas.wildauer@cern.ch, Kirill.Prokofiev@cern.ch, Giacinto.Piacquadio@physik.uni-freiburg.de   
 */

  //class ParametersBase;
  
  class VxTrackAtVertex
  {
  
    public:
    
   /**
    * Default constructor
    */  
      VxTrackAtVertex();
   
   /**
    * Constructor taking the perigee parameters and corresponding 
    * Chi2 value. NOTE: using this constructor does not allow to store the
    * initial (before refit) Perigee parameters of the track. 
    */
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, NeutralParameters* neutralPerigeeAtVertex=NULL);
      
    /**
     * Constructor taking the  perigee parameters of the track, chi2,
     * and the initial TrackParameters of the track.
     */ 
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, NeutralParameters* neutralPerigeeAtVertex, const TrackParameters* initialPerigee, const NeutralParameters* initialNeutralPerigee);
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, const TrackParameters* initialPerigee);

    /**
     * Constructor required for iterative vertex fitters only (with and without link to original track).
     * In addition to the previous ones, it asks for a pointer to the LinearizedTrack
     */
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, NeutralParameters* neutralPerigeeAtVertex,
		      double ndfPerTrk,LinearizedTrack * linState=0);
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex,
		      double ndfPerTrk,LinearizedTrack * linState=0);
		     
    /**
     * Constructor required for iterative vertex fitters only (with and without link to original track).
     * In addition to the previous ones, it asks for a pointer to the LinearizedTrack, but also needs an
     * initial track parameters.
     */
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, NeutralParameters* neutralPerigeeAtVertex,  const TrackParameters* initialPerigee, const NeutralParameters* initialNeutralPerigee,
		      double ndfPerTrk,LinearizedTrack * linState=0); 
      VxTrackAtVertex(double chi2PerTrk, TrackParameters* perigeeAtVertex, const TrackParameters* initialPerigee,
		      double ndfPerTrk,LinearizedTrack * linState=0); 

      VxTrackAtVertex(ITrackLink* trackOrParticleLink);

      virtual ~VxTrackAtVertex();
      
    /**
     * Copy constructor
     */  
      VxTrackAtVertex(const VxTrackAtVertex& rhs);
    
    /**
     * Assignment operator
     */
      VxTrackAtVertex &operator= (const VxTrackAtVertex &);  
     
    /**
     * Move assignment operator
     */
      VxTrackAtVertex &operator= (VxTrackAtVertex &&);  
     
    /**
     * Clone method
     */
      virtual VxTrackAtVertex* clone() const;                
         
    /**
     * Output Method for MsgStream, to be overloaded by child classes
     */
      virtual MsgStream& dump(MsgStream& sl) const;
    
    /**
     * Output Method for std::ostream, to be overloaded by child classes
     */
      virtual std::ostream& dump(std::ostream& sl) const;
    
    /**
     * Returns a pointer to the original track if any
     */ 
//       const Track* origTrack(void) const;
      
    /**
     * Track Quality (Chi2 and ndf per track in a veretx fit)
     */    
      const FitQuality trackQuality(void) const;       
      
    /**
     * Perigee parameters with respect to fitted vertex
     * d_0 and z_0 are equal to 0 in case the trajectory was refitted
     * with the knowledge of the reconstructed vertex 
     */  
      const TrackParameters* perigeeAtVertex(void) const; 

    /**
     * Access to the initial perigee parameters of trajectory
     */
      virtual const TrackParameters* initialPerigee(void) const; 
 

    /**
     * Perigee parameters with respect to fitted vertex
     * d_0 and z_0 are equal to 0 in case the trajectory was refitted
     * with the knowledge of the reconstructed vertex 
     */  
      const NeutralParameters* neutralPerigeeAtVertex(void) const; 

    /**
     * Access to the initial perigee parameters of trajectory
     */
      virtual const NeutralParameters* initialNeutralPerigee(void) const; 
 
     
    /**
     * Set methods for various components
     */
      void setTrackQuality(const FitQuality& trkQuality);
 
     
    /**
     * Setting up the initial track.
     */ 
      void setOrigTrack(ITrackLink * trk);


    /**
     * Setting up the initial track, without caching parameters.
     */ 
      void setOrigTrackNoCache(ITrackLink * trk);


    /**
     * Setting up the linearized track.
     */  
      void setLinTrack(LinearizedTrack * myLinTrack);
      
    /**
     * Setting up parameters at vertex.
     */   
      void setPerigeeAtVertex(TrackParameters * perigee);
      void setPerigeeAtVertex(NeutralParameters * perigee);
     
    /**
     * This method changes a pointer to original perigee of the track.
     * If the object has EL to Track, the pointer
     * will not be changed. Use setOrigTrack() method for that.
     */ 
      void setInitialPerigee(const TrackParameters * perigee); 

    /**
     * This method changes a pointer to original perigee of the neutral.
     * If the object has EL to Neutral, the pointer
     * will not be changed. Use setOrigTrack() method for that.
     */ 
      void setInitialPerigee(const NeutralParameters * perigee); 
      
    /**
     * Equality operator required when working with vectors
     * Compares only the equality of the parameters of the 
     *  Perigee with which this object was created.
     */
      bool operator==(const VxTrackAtVertex & data) const;
      
    /**
     * Access method for the perigee linearized track
     */
      LinearizedTrack * linState(void); 
      
      const LinearizedTrack * linState(void) const;      

    /**
     * Information about fast compatibility estimation, to be given to the annealing
     */      
      double vtxCompatibility(void) const;
      
      void setVtxCompatibility(const double);
     
    /**
     * Information about the weight of track in fit (given back by annealing): weight=ndf/2
     */    
      double weight(void) const;
      
    /**
     * Set method for a weight
     */      
      void setWeight(const double);
     
    /**
     * Set method for ImpactPoint3dAtaPlane.
     */
      void setImpactPoint3dAtaPlane(AtaPlane* myIP3dAtaPlane);
      
    /**
     * Returns ImpactPoint3dAtaPlane. 
     */
      AtaPlane * ImpactPoint3dAtaPlane(void);
      
    /**
     * Returns ImpactPoint3dAtaPlane.(const access) 
     */
      const AtaPlane * ImpactPoint3dAtaPlane(void) const;

    /**
     * Set method for ImpactPoint3dNeutralAtaPlane.
     */
      void setImpactPoint3dNeutralAtaPlane(NeutralAtaPlane* myIP3dNeutralAtaPlane);
      
    /**
     * Returns ImpactPoint3dNeutralAtaPlane. 
     */
      NeutralAtaPlane * ImpactPoint3dNeutralAtaPlane(void);
      
    /**
     * Returns ImpactPoint3dNeutralAtaPlane.(const access) 
     */
      const NeutralAtaPlane * ImpactPoint3dNeutralAtaPlane(void) const;

      

//hack: access methods  to the new link:  
      const  ITrackLink * trackOrParticleLink(void) const;
      
      ITrackLink * trackOrParticleLink(void); 
      
      /**return number of objects currently created*/
      static unsigned int numberOfInstantiations() ;

  protected:
    
    /** 
     * The chi2 and number of degrees of freedom of the track
     */
      FitQuality  m_fitQuality;
      
    /**
     * Weight of the track with respect to vertex
     */  
      double m_trkWeight;
     
    /** 									     
     * Value of the compatibility of the track to the actual vertex, based	     
     * on the estimation on the distance between the track and the vertex through    
     * the IP3dAtaPlane or IP3dNeutralAtaPlane.							     
     */ 									     
      double m_VertexCompatibility;         
      
    /** 
     *  The perigee parameters of tracks used in the fit
     *  wrt. the fitted vertex. In case the vertex fitter used
     *  includes smoothing, this perigee state is set to 
     *  refitted track state after the fit.
     *  These values are written to the VxPrimary part of the CBNT. 
     */
      TrackParameters* m_perigeeAtVertex;

    /** 
     *  The perigee parameters of neutrals used in the fit
     *  wrt. the fitted vertex. In case the vertex fitter used
     *  includes smoothing, this perigee state is set to 
     *  refitted neutral state after the fit.
     *  These values are written to the VxPrimary part of the CBNT. 
     */
      NeutralParameters* m_neutralPerigeeAtVertex;
       
    /** 							     
     * Linearized track, required for the iterative vertex fitting    
     */ 							     
      LinearizedTrack * m_linState;
       
    /** 							     
     * Pointer to initial (before fit) perigee state of the track.   
     * It is stored separately for the back-compatibility issues:    
     * in some cases the vertex fit may be done directly on the      
     * vector of TrackParameterss, instead of vector of tracks.      
     */ 							     
      const TrackParameters * m_initialPerigee;
      const NeutralParameters * m_initialNeutralPerigee;
       
    /** 									       
     * Pointer to the TrackParameters defined on the Plane with center the actual      
     * vertex estimate and passing through the point of closest approach in 3d from    
     * this vertex to the track, with direction of track perpendicular to the plane.   
     * Needed by the IVertexTrackCompatibilityEstimator to estimate in a faster way    
     * the compatibility of a vertex with the track.				       
     */ 									       
      AtaPlane * m_ImpactPoint3dAtaPlane;
      NeutralAtaPlane * m_ImpactPoint3dNeutralAtaPlane;
  
//hacks related to the new link structure
      ITrackLink * m_trackOrParticleLink;  
    
    private:
      /** number of objects of this type in memory */
      static unsigned int s_numberOfInstantiations;
  
  };//end of class definitions
  
  inline const  ITrackLink * VxTrackAtVertex::trackOrParticleLink(void) const
  {
   return m_trackOrParticleLink; 
  }
  
  inline ITrackLink * VxTrackAtVertex::trackOrParticleLink(void)
  {
   return m_trackOrParticleLink;
  }
  
  inline const TrackParameters*  VxTrackAtVertex::initialPerigee(void) const
  {
    if (m_initialPerigee!=0) return m_initialPerigee;//as default always return the proper object (be consistent!)  
    if(m_trackOrParticleLink!=0) return m_trackOrParticleLink->parameters();
    return 0;													    
  }

  inline const NeutralParameters*  VxTrackAtVertex::initialNeutralPerigee(void) const
  {
    if (m_initialNeutralPerigee!=0) return m_initialNeutralPerigee;//as default always return the proper object (be consistent!)  
    if(m_trackOrParticleLink!=0) return m_trackOrParticleLink->neutralParameters();
    return 0;													    
  }

  inline VxTrackAtVertex* VxTrackAtVertex::clone() const
  {
    return new VxTrackAtVertex(*this);
  }

  inline void VxTrackAtVertex::setImpactPoint3dAtaPlane(AtaPlane * myIP3dAtaPlane)
  { 
    if (m_ImpactPoint3dAtaPlane != 0) delete m_ImpactPoint3dAtaPlane;
    m_ImpactPoint3dAtaPlane=myIP3dAtaPlane;
  }  

  inline AtaPlane * VxTrackAtVertex::ImpactPoint3dAtaPlane(void)
  {
    return m_ImpactPoint3dAtaPlane;
  }
 
  inline const AtaPlane * VxTrackAtVertex::ImpactPoint3dAtaPlane(void) const
  {
    return m_ImpactPoint3dAtaPlane;
  }

  inline void VxTrackAtVertex::setImpactPoint3dNeutralAtaPlane(NeutralAtaPlane * myIP3dNeutralAtaPlane)
  { 
    if (m_ImpactPoint3dNeutralAtaPlane != 0) delete m_ImpactPoint3dNeutralAtaPlane;
    m_ImpactPoint3dNeutralAtaPlane=myIP3dNeutralAtaPlane;
  }  

  inline NeutralAtaPlane * VxTrackAtVertex::ImpactPoint3dNeutralAtaPlane(void)
  {
    return m_ImpactPoint3dNeutralAtaPlane;
  }
 
  inline const NeutralAtaPlane * VxTrackAtVertex::ImpactPoint3dNeutralAtaPlane(void) const
  {
    return m_ImpactPoint3dNeutralAtaPlane;
  }

  inline void VxTrackAtVertex::setWeight(const double weight)
  {
    m_trkWeight = weight;
  }

  inline double VxTrackAtVertex::weight(void) const 
  {
    return m_trkWeight;
  }

  inline void VxTrackAtVertex::setVtxCompatibility(const double vtxCompatibility)
  {
    m_VertexCompatibility=vtxCompatibility;
  }

  inline double VxTrackAtVertex::vtxCompatibility(void) const 
  {
    return m_VertexCompatibility;
  }

  inline LinearizedTrack * VxTrackAtVertex::linState(void)
  {
   return m_linState;
  }

  inline const LinearizedTrack * VxTrackAtVertex::linState(void) const
  {
   return m_linState;
  }

  inline const FitQuality VxTrackAtVertex::trackQuality(void) const
  {
    return m_fitQuality;
  }
  
  inline const TrackParameters* VxTrackAtVertex::perigeeAtVertex(void) const
  {
    return m_perigeeAtVertex;
  }

  inline const NeutralParameters* VxTrackAtVertex::neutralPerigeeAtVertex(void) const
  {
    return m_neutralPerigeeAtVertex;
  }

  inline void VxTrackAtVertex::setTrackQuality(const FitQuality& trkQuality)
  {   
    m_fitQuality = trkQuality;
  } 

  inline void VxTrackAtVertex::setPerigeeAtVertex(TrackParameters * perigee)
  {
    if (m_perigeeAtVertex != 0) delete m_perigeeAtVertex;
    m_perigeeAtVertex = perigee;
  }

  inline void VxTrackAtVertex::setPerigeeAtVertex(NeutralParameters * perigee)
  {
    if (m_neutralPerigeeAtVertex != 0) delete m_neutralPerigeeAtVertex;
    m_neutralPerigeeAtVertex = perigee;
  }
  
  inline void VxTrackAtVertex::setLinTrack(LinearizedTrack * myLinTrack)
  {
    if (m_linState != 0) delete m_linState;
    m_linState=myLinTrack;
  }

  inline void VxTrackAtVertex::setOrigTrack(ITrackLink* trk)
  {
    delete m_trackOrParticleLink;
//setting first the actual pointer:
    m_trackOrParticleLink = trk;    

//setting the initial perigee to be consistent with this 
// new track    
    if(trk != 0)
    {
     // delete initialPerigee before, otherwise memleak!!
//      if (m_initialPerigee!=0) { delete m_initialPerigee; m_initialPerigee=0; }
//      const TrackParameters * tmpInPer = trk->parameters();
      m_initialPerigee = trk->parameters();
      m_initialNeutralPerigee = trk->neutralParameters();
    }else {
      m_initialPerigee = 0;
      m_initialNeutralPerigee = 0;
    }
  }//end of setOrigTrack method
  
  inline void VxTrackAtVertex::setOrigTrackNoCache(ITrackLink* trk)
  {
    delete m_trackOrParticleLink;
    m_trackOrParticleLink = trk;    

    m_initialPerigee = nullptr;
    m_initialNeutralPerigee = nullptr;
  }//end of setOrigTrackNocache method
  
  inline void VxTrackAtVertex::setInitialPerigee(const TrackParameters * perigee)
  {
   if(m_trackOrParticleLink == 0) m_initialPerigee = perigee;
  }

  inline void VxTrackAtVertex::setInitialPerigee(const NeutralParameters * perigee)
  {
   if(m_trackOrParticleLink == 0) m_initialNeutralPerigee = perigee;
  }
  
/**Overload of << operator for both, MsgStream and std::ostream for debug 
output; only needed in base class? */ 
MsgStream& operator << ( MsgStream& sl, const VxTrackAtVertex& sf);
std::ostream& operator << ( std::ostream& sl, const VxTrackAtVertex& sf); 
  
}//end of namespace definitions
#endif






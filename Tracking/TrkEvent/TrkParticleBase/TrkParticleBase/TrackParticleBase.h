/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          TrackParticleBase.h  -  Description
                             -------------------
    authors : Edward.Moyse@cern.ch (based on TrackParticleBase from andreas.wildauer@cern.ch, fredrik.akesson@cern.ch)
 
    Adding the TrackInfo to be stored at the AOD level: 2009-1-29
    author: Kirill.Prokofiev@cern.ch
    
 ***************************************************************************/
#ifndef TRKPARTICLEBASE_TRKTRACKPARTICLEBASE_H
#define TRKPARTICLEBASE_TRKTRACKPARTICLEBASE_H

// includes for datamembers
#include "AthLinks/ElementLink.h"
#include "TrkTrack/TrackCollection.h" // typedef
#include "VxVertex/VxContainer.h"     // typedef
#include "TrkEventPrimitives/VertexType.h"

#include "TrkTrack/Track.h"           // no forward declare for now because needed in cast
#include <vector>
#include "TrkParameters/TrackParameters.h"    // no forward declare for now because needed in cast
#include "TrkTrack/TrackInfo.h"       // no forward declaration: we are storing the value 


class MsgStream;
class TrackParticleBaseCnv_p1;

namespace Trk
{
  typedef VertexType TrackParticleOrigin;

  class VxCandidate;
  class TrackSummary;
  class FitQuality;
  
  class TrackParticleBase
  {
  public:

    /** Constructor 0
    standard constructor which sets everything to 0, needed for persistency */
    TrackParticleBase();
 
/** Main constructor. I have tried to merge the various TrackParticle constructors 
(though perhaps we should split this into one which takes RecVertex and one which takes VxCandidate, 
    since they seem to be exclusive)
    @param  trk Track used to create this object. Since an ElementLink is used to store the object, 
                setStorableObject(const TrackCollection* trackColl) must be called to set persistency.
    @param  vertexType See TrackParticleOrigin for definitions, but should be set to NoVtx if no VxCandidate pointer is passed.
    @param  vxCandidate Pointer to the VxCandidate used to create this TrackParticleBase.
    @param  trkSummary Summary object of this TrackParticleBase (should be created with TrackSummaryTool). Will be owned by this TrackParticleBase.
    @param  parameters std::vector of TrackParameters which describe this TrackParticleBase. Ownership of these TrackParameters objects passes to TrackParticleBase 
            (so if you copied them from a Track, make sure you didn't just copy the pointers but actually created new objects, for example, by using clone() )
    @param  definingParameter The parameter which defines the charge of this TrackParticleBase 
            and which in derived classes (such as Rec::TrackParticle), could be used to define e.g. the 4-momentum. 
            This will be appended to the vector of parameters passed before, so please make sure that it is *NOT* already contained in that!
    @param fitQuality FitQuality which describes this TrackParticleBase. Will be owned by this TrackParticleBase. 

    @warning You MUST pass in a valid 'definingParameter' and this 'definingParameter' must NOT be part of the std::vector<const TrackParameters*> passed.
*/                                                                              
    TrackParticleBase(  const Track*                            trk, 
                        const TrackParticleOrigin                    vertexType, 
                        const VxCandidate*                      vxCandidate, 
                        const TrackSummary*                     trkSummary, 
                        std::vector<const TrackParameters*>&     parameters,
                        const TrackParameters*                   definingParameter,
                        const FitQuality*                       fitQuality);
    
    TrackParticleBase (const ElementLink<TrackCollection>& trackLink,
                       const Trk::TrackParticleOrigin trkPrtOrigin, 
                       const ElementLink<VxContainer>& vxCandidate,
                       std::unique_ptr<Trk::TrackSummary> trkSummary,
                       // passes ownership of elements.
                       std::vector<const Trk::TrackParameters*>&&  parameters,
                       std::unique_ptr<Trk::FitQuality> fitQuality,
                       const TrackInfo& info);

    /** Copy constructor */
    TrackParticleBase(const TrackParticleBase &);

    /** Assignement operator */
    TrackParticleBase &operator= (const TrackParticleBase &);
    TrackParticleBase &operator= (TrackParticleBase &&);
    
    /** Destructor */
    virtual ~TrackParticleBase();
    
    /** Return charge of the particle.
    @return The charge of the particle is calculated via the TrackParameters::charge() method.
    It is calculated via the sign of the track parameters \f$ \frac{q_0}{p} \f$. 
    */
    double                                          charge() const; 

    /** Return pointer to associated track.
    @return a pointer to the Track with which the particle was created. Right now it is also possible to create
    a particle without a track (or possibly the Track is not accessible). In that case the method returns 0.*/
    const Track*                               originalTrack() const;

    /** Get a pointer to the primary vertex.
    @return Returns a pointer to the primary vertex.*/
    const VxCandidate*                         reconstructedVertex() const;

    const ElementLink<VxContainer>&            reconstructedVertexLink() const;

    /** Get the particle origin type.
    @return A TrackParticleOrigin enum (see TrkEventPrimitives/VertexType.h for definitions)
    */
    TrackParticleOrigin                             particleOriginType() const;
    
    /** Returns the 'defining' parameter of this TrackParticle
    @return The parameters (from the vector returned by trackParameters() ) which define the charge(), 
    and (in the inherited Rec::TrackParticle, the 4 momentum) */
    const TrackParameters&                      definingParameters() const;   

    /** Returns the track parameters.
    @return The TrackParameters of the TrackParticleBase, expressed at various points through out the detector.
    
    "Various" may seem vague, but it is up to the clients of this class to decide what is needed (Perigee, MuonSpectrometer entrance etc).
    However, the size of this vector must be at least 1, since ALL TrackParticleBases are required to have ONE defining parameter*/
    const std::vector<const TrackParameters*>&  trackParameters() const;   

    /** Return the ElementLink to the Track.
    @return ElementLink<TrackCollection> 

    This is necessary to force backnavigation from AOD to ESD */
    const ElementLink< TrackCollection >*           trackElementLink() const;   

    /** accessor function for TrackSummary. 
    @return A pointer to the TrackSummary owned by this object (could be 0)*/
    const TrackSummary*                        trackSummary() const;   

    /** accessor function for FitQuality. 
    @return The pointer to the FitQuality for this class. Can be 0 if object was created with no FitQuality.     
    @todo Is this correct? Is it valid to have a TPB with no FQ?*/
    const FitQuality *                         fitQuality() const;
    
    /** Attempts to cast the definingParameters() to Perigee. The result is not cached, so there will be a (small) time penalty 
    each time you call this.
    @return Perigee* if the cast was successful, zero if not*/
    const Perigee* perigee() const;

    void setStorableObject(const TrackCollection* trackColl);
    void setStorableObject(const VxContainer* vxColl);
    void setTrackLink ( const TrackCollection *  cont, const Track * track);
    
    /** Dump methods to be used by the overloaded stream operator (inheritance!) */
    virtual MsgStream&    dump( MsgStream& out ) const ;
    virtual std::ostream& dump( std::ostream& out ) const ;
    
   /**									            
    * returns the info of the track.           
    */									            
    const TrackInfo& info() const;	
    
  protected:
    friend class ::TrackParticleBaseCnv_p1;

    /** ElementLink to the Track used to create this TrackParticleBase object
    Before writing to disk, should be made valid using setStorableObject(const VxContainer* vxColl).*/
    ElementLink< TrackCollection >              m_originalTrack; 
    
    /** specifies the origin type of the particle (prim.vertex, ...)*/    
    TrackParticleOrigin                         m_trackParticleOrigin; 
    /** ElementLink to a VxCandidate. 
    Before writing to disk, should be made valid using setStorableObject(const VxContainer* vxColl)*/
    ElementLink< VxContainer >                  m_elVxCandidate; 
    
    /** DataVector of TrackParameters stored inside this TrackParticle. (This TrackParticleBase owns all the TrackParameters, and will delete them in the destructor)*/
    std::vector<const TrackParameters*> m_trackParameters; 
    
    /** the summary of this TrackParticle. Owned by this object.*/
    const TrackSummary*                    m_trackSummary; 

    /** the fit quality of this TrackParticle. Owned by this object.*/
    const FitQuality*                      m_fitQuality; 
    
    
   /**									   
    * This is a class which stores the identity of where the track 	   
    * was created, fitted, which properties the reconstruction had								   
    */									   
     TrackInfo  m_trackInfo;	
    
  };
    
  /**
  * Overload of << operator for MsgStream for debug output
  */ 
  MsgStream& operator << ( MsgStream& sl, const TrackParticleBase& trackParticleBase);

  /**
  * Overload of << operator for std::ostream for debug output
  */ 
  std::ostream& operator << ( std::ostream& sl, const TrackParticleBase& trackParticleBase);
    
  inline double TrackParticleBase::charge(void) const
  {
    return definingParameters().charge();
  }

  inline const Track* TrackParticleBase::originalTrack() const
  {
    if (m_originalTrack.isValid()){
      return *m_originalTrack;
    } 
      return nullptr;
    
  }

  inline
  const VxCandidate* TrackParticleBase::reconstructedVertex() const
  {
    if (m_elVxCandidate.isValid()) {
      return *m_elVxCandidate;
    } 
    
      return nullptr;
    
  }

  inline const ElementLink<VxContainer>& TrackParticleBase::reconstructedVertexLink() const
  {
    return m_elVxCandidate;
  }

 inline const ElementLink< TrackCollection >*  TrackParticleBase::trackElementLink() const
  {
     return &m_originalTrack;
  }

  inline TrackParticleOrigin TrackParticleBase::particleOriginType() const {
    return m_trackParticleOrigin;
  }
  
  inline const TrackParameters& TrackParticleBase::definingParameters() const {
    return *( m_trackParameters.back() );
  }

  inline const std::vector<const TrackParameters*>& TrackParticleBase::trackParameters() const {
    return m_trackParameters;
  }  

  inline const TrackSummary* TrackParticleBase::trackSummary() const {
    return m_trackSummary;
  }

  inline void TrackParticleBase::setStorableObject(const TrackCollection* trackColl) {
    m_originalTrack.setStorableObject(*trackColl);
  }
  
  inline void TrackParticleBase::setStorableObject(const VxContainer* vxColl) {
    m_elVxCandidate.setStorableObject(*vxColl);
  }
  
  inline const FitQuality* TrackParticleBase::fitQuality() const {
    return m_fitQuality;
  }
  
  inline const Perigee* TrackParticleBase::perigee() const
  {
  	return dynamic_cast<const Perigee*>( &(definingParameters()) );
  }
  
  inline 
  void TrackParticleBase::setTrackLink ( const TrackCollection *  cont, const Track * track) 
  {
     if ( m_originalTrack.isValid() ) { m_originalTrack.reset();
}
     m_originalTrack.toContainedElement( *cont, track);
  }
  
  inline const TrackInfo& TrackParticleBase::info() const
  {
    return m_trackInfo;
  }

}

#endif

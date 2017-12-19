/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          TrackParticle.h  -  Description
                             -------------------
    begin   : 27-02-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
    
 ***************************************************************************/
#ifndef RECPARTICLE_TRACKPARTICLE_H
#define RECPARTICLE_TRACKPARTICLE_H

// includes for inheritance
#include "FourMom/P4PxPyPzE.h"
#include "Navigation/Navigable.h"
#include "Navigation/AthenaBarCodeImpl.h"
#include "EventKernel/INavigable4Momentum.h"

// includes for datamembers
#include "AthLinks/ElementLink.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"

#include "VxVertex/RecVertex.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/FitQuality.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "AthenaKernel/BaseInfo.h"

class MsgStream;

namespace Rec
{
  class TrackParticle :
      virtual public INavigable4Momentum,
      public Trk::TrackParticleBase, 
      public P4PxPyPzE ,  
      public NavigableTerminalNode

  {
  public:

    /** Constructor 0
    standard constructor which sets everything to 0, needed for persistency */
    TrackParticle();
    
    /** 'Beautiful' TrackParticle Constructor. 
    @param  trk Track used to create this object. Since an ElementLink is used to store the object, 
                setStorableObject(const TrackCollection* trackColl) must be called to set persistency.
    @param  trkPrtOrigin See TrackParticleOrigin for definitions, but should be set to NoVtx if no Trk::VxCandidate pointer is passed.
    @param  vxCandidate Pointer to the Trk::VxCandidate used to create this TrackParticle.
    @param  trkSummary Summary object of this TrackParticle (should be created with Trk::TrackSummaryTool). Will be owned by this TrackParticle.
    @param  parameters std::vector of Trk::TrackParameters which describe this Trk::TrackParticle. Ownership of these Trk::TrackParameters objects passes to TrackParticle 
            (so if you copied them from a Trk::Track, make sure you didn't just copy the pointers but actually created new objects, for example, by using clone() )
    @param  definingParameter The Trk::TrackParameters which defines the charge and 4-momentum of this TrackParticle. 
            This will be appended to the vector of parameters passed before, so please make sure that it is *NOT* already contained in that!
    @param fitQuality Trk::FitQuality which describes this Trk::TrackParticle. Will be owned by this TrackParticle. 

@warning You MUST pass in a valid 'definingParameter' and this 'definingParameter' must NOT be part of the std::vector<const Trk::TrackParameters*> passed.
    */  
    TrackParticle(  const Trk::Track*                                       trk, 
                    const Trk::TrackParticleOrigin                          trkPrtOrigin, 
                    const Trk::VxCandidate*                                 vxCandidate, 
                    const Trk::TrackSummary*                                trkSummary, 
                    std::vector<const Trk::TrackParameters*>&                parameters,
                    const Trk::TrackParameters*                              definingParameter,
                    const Trk::FitQuality*                                  fitQuality);   


    /**
     * @brief Alternate constructor, to be used by TP converters.
     *        Passes in ElementLinks rather than pointers, and does
     *        not dereference those links.
     * @param  trackLink Link to track used to create this object.
     * @param  trkPrtOrigin See TrackParticleOrigin for definitions, but
     *         should be set to NoVtx if no Trk::VxCandidate pointer is passed.
     * @param  vxCandidate Link to the Trk::VxCandidate used to create
     *         this TrackParticle.
     * @param  trkSummary Summary object of this TrackParticle (should be
     *         created with Trk::TrackSummaryTool).
     * @param  parameters std::vector of Trk::TrackParameters which describe
     *         this Trk::TrackParticle. Ownership of these Trk::TrackParameters
     *         objects passes to TrackParticle  (so if you copied them from a
     *         Trk::Track, make sure you didn't just copy the pointers
     *         but actually created new objects, for example, by using clone()).
     *         In contrast to the previous constructor, this vector
     *         must constain the defining parameters.
     * @param fitQuality Trk::FitQuality which describes this TrackParticle.
     * @param info TrackInfo for this track.
     * @param mom Four-momentum for this particle.
     */
    TrackParticle (const ElementLink<TrackCollection>& trackLink,
                   const Trk::TrackParticleOrigin trkPrtOrigin, 
                   const ElementLink<VxContainer>& vxCandidate,
                   std::unique_ptr<Trk::TrackSummary> trkSummary,
                   // passes ownership of elements.
                   std::vector<const Trk::TrackParameters*>&&  parameters,
                   std::unique_ptr<Trk::FitQuality> fitQuality,
                   const Trk::TrackInfo& info,
                   const P4PxPyPzE& mom);
 
    /** Copy constructor */
    TrackParticle(const TrackParticle &);

    /** Assignment operator */
    TrackParticle &operator= (const TrackParticle &);
    TrackParticle &operator= (TrackParticle &&);
    
    /** Destructor */
    virtual ~TrackParticle();  
    
    /** Accessor method for Perigee.
    @return Perigee used to make this object
    @warning A TrackParticle is REQUIRED to have a Trk::Perigee and so this pointer should never be zero, however internally it 
    is cached using a dynamic_cast and so in principle this cast could fail if the object is constructed improperly. 
    There is an assert to test this in debug.
    @warning This method should properly return the Perigee by reference,
    but it is a bit late to make interface changes this close to 13.0.0*/
    const Trk::Perigee* measuredPerigee() const; 

    /** Slimming methods used to reduce the TrackParticle content and size that may be called 
       by users when making DPDs */

    /** removeSummary to delete and set summary to null */ 
    void removeSummary();

    /** removeFitQuality to delete the fit quality object */
    void removeFitQuality();

    /** removeErrorMatrix to delete the error matrix. This uses the cloneWithoutError() method of
       measuredPerigee() to create a Perigee() and replace the existing Perigee with a Perigee */
    void removeErrorMatrix();
    
    /** Dump methods to be used by the overloaded stream operator (inheritance!) */
    virtual MsgStream&    dump( MsgStream& out ) const ;
    virtual std::ostream& dump( std::ostream& out ) const ;
    
    /** set 4Momentum (will throw exception if cannot be implemented) */ 
    void set4Mom (const I4Momentum * const theI4Mom );

    /** set 4Momentum (will throw exception if cannot be implemented) */
    void set4Mom (const I4Momentum & theI4Mom);

    /** set 4Momentum (will throw exception if cannot be implemented) */
    void set4Mom (const CLHEP::HepLorentzVector & theHlv );

    /** @{ IAthenaBarCode interface forwarding --- copied from ParticleImpl */

    AthenaBarCode_t getAthenaBarCode() const;
    void setAthenaBarCode(AthenaBarCode_t /*id*/);
    
    // Comparing & Versioning Control
    // Should not be necessary here with a reasonable IAthenaBarCode implementation
    bool hasSameAthenaBarCode(const IAthenaBarCode &/*obj*/) const;
    bool hasSameAthenaBarCodeExceptVersion(const IAthenaBarCode &) const;
    AthenaBarCodeVersion_t getVersion() const;
    void newVersion();
    void setVersion(AthenaBarCodeVersion_t newversion);
    //    std::ostream& dump(std::ostream& out) const;

    /** @} */

private:
    /**Cached pointer to the Perigee (if one was used to create this class).
    It is required that Trk::TrackParticleBase::definingParameters() is actually a Perigee. If not then
    this call just returns a 0 pointer.*/
    mutable const Trk::Perigee*       m_cachedPerigee;

    AthenaBarCodeImpl m_abc;
    };
}


SG_BASES2 (Rec::TrackParticle,
           SG_VIRTUAL(INavigable4Momentum),
           Trk::TrackParticleBase);

inline const Trk::Perigee* Rec::TrackParticle::measuredPerigee() const
{
    if (0==m_cachedPerigee){
        m_cachedPerigee = dynamic_cast<const Trk::Perigee*>( &definingParameters() );
    }
    return m_cachedPerigee;
}
  
  /**
  * Overload of << operator for MsgStream for debug output
  */ 
  MsgStream& operator << ( MsgStream& sl, const Rec::TrackParticle& trackParticle);

  /**
  * Overload of << operator for std::ostream for debug output
  */ 
  std::ostream& operator << ( std::ostream& sl, const Rec::TrackParticle& trackParticle);

#endif

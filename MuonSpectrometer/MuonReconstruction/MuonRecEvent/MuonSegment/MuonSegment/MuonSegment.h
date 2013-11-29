/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegment.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONSEGMENT_MUONSEGMENT_H
#define MUONSEGMENT_MUONSEGMENT_H

// Trk
#include "TrkSegment/Segment.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSpaceTimePoint/SpaceTimePointBase.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

namespace Muon {

  /** @class MuonSegment

      This is the common class for 3D segments used in the muon spectrometer.

      The Surface type for MuonSegments is restricted to be a PlaneSurface. 
      The parameters of the MuonSegment are:
      - the Trk::LocalPosition of the segment in the surface reference frame 
      - the Trk::LocalDirection of the segment which consistes of the angles 
        @f$ \theta_{xz} @f$ and @f$\theta_{yz}@f$. 

      The MuonSegment stores a list of Trk::MeasurementBase objects allowing it to 
      contain measurements from all detector types in the muon spectrometer.

      @image html segment_example.gif 
      @image html MuonSegment.gif 

      @author Niels.van.Eldik@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
  */  
  class MuonSegment : public Trk::Segment, public Trk::SpaceTimePointBase {
  public:

    /** define invalid value, used when the segment has no fitted t0 */
    static const float kNoValue; 

    /** Default Constructor for POOL */   
    MuonSegment();
    /** Copy Constructor */
    MuonSegment(const MuonSegment& seg);
    /** Assignment operator */
    MuonSegment& operator=(const MuonSegment& seg);
    
    /** Constructor within standard track parameters frame taking a vector of MeasurementBase.
        @param locpars 4 dim or 5 dim standard track parameters representation 
        @param locerr 4 x 4 error or 5 x 5 error on standard track parameters repr. 
        @param psf plane surface 
        @param cmeas vector of contained measurements on track 
        @param fqual fit quality object 
	@param author enum to indicate author, see Segment.h for the possible authors
     */
    MuonSegment( const Trk::LocalParameters& locpars,         
         const Amg::MatrixX& locerr,              
         Trk::PlaneSurface* psf,                       
         DataVector<const Trk::MeasurementBase>* cmeas, 
         Trk::FitQuality* fqual,
         Segment::Author author=AuthorUnknown
         );                     

    /** Constructor within local parameters of the Segment taking a vector of MeasurementBase  
        @param segLocPos 2 local position coordinates
        @param segLocalErr 2 local direction coordinates
        @param segLocalErr 4 x 4 full local error
        @param psf plane surface 
        @param cmeas vector of contained measurements on track 
        @param fqual fit quality object 
	@param author enum to indicate author, see Segment.h for the possible authors
    */
    MuonSegment( const Amg::Vector2D& segLocPos,         // 2 local position coordinates
		 const Trk::LocalDirection& segLocDir,        // 
         const Amg::MatrixX& segLocalErr,         //  
         Trk::PlaneSurface* psf,                      // plane surface to define frame
         DataVector<const Trk::MeasurementBase>* cmeas, // vector of contained measurements on track
         Trk::FitQuality* fqual,                    // fit quality object
         Segment::Author author=AuthorUnknown);                  

    /** Destructor */
    virtual ~MuonSegment();
  
    /** needed to avoid excessive RTTI*/
    virtual MuonSegment* clone() const;

    /** global position */
    const Amg::Vector3D&  globalPosition() const;

    /** global direction */
    const Amg::Vector3D& globalDirection() const;

    /** local direction */ 
    const Trk::LocalDirection& localDirection() const;

    /** returns the surface for the local to global transformation 
         - interface from MeasurementBase */
    const Trk::PlaneSurface& associatedSurface() const;

    /** number of RIO_OnTracks */    
    unsigned int numberOfContainedROTs() const;

    /** returns the vector of Trk::RIO_OnTrack objects 
       - specific for this TrackSegment: Trk::RIO_OnTrack (generic)
       */
    const std::vector<const Trk::RIO_OnTrack*>& containedROTs() const;
    
    /** returns the RIO_OnTrack (also known as ROT) objects depending on the integer*/
    const Trk::RIO_OnTrack* rioOnTrack(unsigned int) const;

    /** set the fitted time and error on the time */
    void setT0Error(float t0, float t0Error);

    /** returns whether the segment has a fitted t0 */
    bool hasFittedT0() const;

    /** recalculate the cache */
    void recalculateCache();

  private:
    /** The global position the surface can be associated to. Cached (not persistified)*/
    Amg::Vector3D      m_globalPosition;
     
    /** cache global direction, not persistified */
    Amg::Vector3D     m_globalDirection;

    /** LocalDirection */
    Trk::LocalDirection      m_localDirection;
     
    /** The plane surface to which the segment parameters are expressed to */
    mutable const Trk::PlaneSurface*        m_associatedSurface;

    /** The cached vector of contained (generic) Trk::RIO_OnTrack objects. 
    The pointers are not deleted, since the actual objects belong to the base class */
    mutable std::vector<const Trk::RIO_OnTrack*>*   m_cachedRots;

    /** private method to clear the Trk::RIO_OnTrack vector */ 
    void                               clearRotVector();

    /** private method to clear the Trk::MeasurementBase vector */ 
    void                               clearMeasVector();

    /** private method to copy all Trk::RIO_OnTrack pointers in m_cachedRots to m_containedMeasurements */ 
    void                               copyMeasurementsToROTs() const;

  protected:
    /**returns some information about this RIO_OnTrack/TrackSegment. 
     It should be overloaded by any child classes*/
    MsgStream&    dump( MsgStream& out ) const;  
    /**returns some information about this RIO_OnTrack/TrackSegment.
     It should be overloaded by any child classes*/
    std::ostream& dump( std::ostream& out ) const;
    
  };

  inline const Amg::Vector3D&  MuonSegment::globalPosition() const
  {
    return m_globalPosition;
  }

  inline const Amg::Vector3D&  MuonSegment::globalDirection() const
  {
    return m_globalDirection;
  }

  inline const Trk::LocalDirection& MuonSegment::localDirection() const
  {
    return m_localDirection;
  }

  inline const Trk::PlaneSurface& MuonSegment::associatedSurface() const
  { return (*m_associatedSurface); }  

  inline MuonSegment* MuonSegment::clone() const
  {
    return new MuonSegment(*this); 
  }
  
  inline const std::vector<const Trk::RIO_OnTrack*>& MuonSegment::containedROTs() const
   { 
       if (0==m_cachedRots) copyMeasurementsToROTs();
       return (*m_cachedRots); 
   }
 
  inline const Trk::RIO_OnTrack* MuonSegment::rioOnTrack(unsigned int indx) const
    { 
      if (0==m_cachedRots) copyMeasurementsToROTs();
      if (indx<numberOfContainedROTs())
          return (*m_cachedRots)[indx];
      return 0;
   }

  inline unsigned int MuonSegment::numberOfContainedROTs() const {
      if (0==m_cachedRots) copyMeasurementsToROTs();
      return m_cachedRots->size();
  }

  inline void MuonSegment::setT0Error(float t0, float t0Error){
    m_time=t0; m_errorTime=t0Error;
  }

  inline bool MuonSegment::hasFittedT0() const {
    return m_time != MuonSegment::kNoValue;
  }

}

#endif // MUONSEGMENT_MUONSEGMENT_H


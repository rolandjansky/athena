/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSegment.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSEGMENT_TRACKSEGMENT_H
#define TRKSEGMENT_TRACKSEGMENT_H

// Trk
#include "TrkSegment/Segment.h"
#include "TrkMeasurementBase/MeasurementBase.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <ostream>
#include <vector>

#include "AthContainers/DataVector.h"
class MsgStream;
class TrackSegmentCnv_p1;

namespace Trk{

class Surface;
class PrepRawData;
class LocalParameters;
class TrkDetElementBase;
class FitQuality;

/** @class TrackSegment
    
    Class for a generic track segment
    that holdes polymorphic Trk::MeasurementBase objects, it
    extends the Trk::Segment base class.

    Trk::MeasurementBase objects of different type can be used to 
    describe this track segment. The fitted Trk::LocalParameters
    such as the associated ErrorMatrix are directly forwarded to be
    stored in the Trk::MeasurementBase base class.
    
    A Surface has to be provided to give the reference frame the \
    Trk::TrackSegment is expressed to.
    
    @author Andreas.Salzburger@cern.ch
    */

  class TrackSegment : public Segment {
    
    public:
      /** Default Constructor for POOL */
      TrackSegment();
      /** Copy Constructor */
      TrackSegment(const TrackSegment& seg);
      /** Move Constructor */
      TrackSegment(TrackSegment&&); 
      /** Assignment operator */
      TrackSegment& operator=(const TrackSegment& seg); 
      /** Move assignment operator*/
      TrackSegment& operator=(TrackSegment&&);
    
      /** Constructor with parameters */
      TrackSegment( const LocalParameters& locpars,
                    const Amg::MatrixX& locerr,
                    const Surface* sf,
                    DataVector<const MeasurementBase>* crots,
                    FitQuality* fqual,
		    Segment::Author author = Segment::AuthorUnknown
                    ); 
  
     /** Destructor */
     virtual ~TrackSegment();
  
     /** needed to avoid excessive RTTI*/
     TrackSegment* clone() const;
     
     /** returns the surface for the local to global transformation 
         - interface from MeasurementBase */
     const Surface& associatedSurface() const;
     
     /**Interface method to get the global Position
        - interface from MeasurementBase */
     const Amg::Vector3D&  globalPosition() const;
           
     /**returns some information about this MeasurementBase/TrackSegment. 
     It should be overloaded by any child classes*/
     MsgStream&    dump( MsgStream& out ) const;  
     /**returns some information about this MeasurementBase/TrackSegment.
     It should be overloaded by any child classes*/
     std::ostream& dump( std::ostream& out ) const;
    
  private:
    friend class ::TrackSegmentCnv_p1;

    /** The surface to which the segment parameters are expressed to */
    mutable const Surface*                  m_associatedSurface;
    /** The surface to which the segment parameters are expressed to */
    mutable const Amg::Vector3D*            m_globalPosition;

};

inline TrackSegment* TrackSegment::clone() const
  { return new TrackSegment(*this); }

inline const Surface& TrackSegment::associatedSurface() const
 { return (*m_associatedSurface); }  
   
}

#endif // TRKSEGMENT_TRACKSEGMENT_H


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Segment.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSEGMENT_SEGMENT_H
#define TRKSEGMENT_SEGMENT_H

// Trk
#include "TrkMeasurementBase/MeasurementBase.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <ostream>
#include <vector>

#include "AthContainers/DataVector.h"
#include "TrkMeasurementBase/MeasurementBase.h"

class MsgStream;
class SegmentCnv_p1;

namespace Trk{

class PrepRawData;
class RIO_OnTrack;
class TrkDetElementBase;
class FitQuality;

/** @class Segment
    
    Base class for all TrackSegment implementations,
    extends the common MeasurementBase.
        
    Trk::LocalParameters, an Trk::ErrorMatrix and a number of fitted 
    RIOs are the commonalities of all track segments, the derived classes
    can overwrite the base class definitions of Trk::RIO_OnTrack, Trk::PrepRawData
    and Trk::Surface with derived classes for interanal use and to avoid extensive RTTI.
    
    The Surface is chosen not to be a private member of
    the base class, such that dedicated Segments can save specific Surface types and overwrite
    the return type by a child class to avoid extensive RTTI.
    
    Access to any information of the contained ROTs, such as the underlying Trk::PrepRawData,
    the pointer to the Trk::DetectorElementBase such as identifiers have to be retrieved from
    the ROT itself.   
    
    @author Andreas.Salzburger@cern.ch
    */

  class Segment : public MeasurementBase {
    
    public:
      /** enum to identify who created the segment. 
	  If you update this don't forget to update the dump method.
      */
      enum Author {
	AuthorUnknown = 0,
	MooMdtSegmentMakerTool = 1,
	MooCscSegmentMakerTool = 2,
	Muonboy = 3,
	DCMathSegmentMaker = 4,
	MDT_DHoughSegmentMakerTool = 5,
	CSC_DHoughSegmentMakerTool = 6,
	Csc2dSegmentMaker = 7,
	Csc4dSegmentMaker = 8,
	TRT_SegmentMaker = 9,
	CTBTracking = 10,
	DCMathSegmentMakerCurved = 11,
	NumberOfAuthors = 12
      }; 


      /** Default Constructor for POOL */
      Segment();
      /** Copy Constructor */
      Segment(const Segment& seg);
      /** Move Constructor */   
      Segment(Segment&&);      
      /** Assignment operator */
      Segment& operator=(const Segment& seg);
      /** Move assignment operator*/
      Segment& operator=(Segment&&);

      /** Constructor with parameters */
      Segment( const LocalParameters& locpars,
               const Amg::MatrixX& locerr,
               DataVector<const MeasurementBase>* measurements,
               FitQuality* fitq=0,Author author=AuthorUnknown);
  
      /** Destructor */
      virtual ~Segment();
  
      /** Pseudo-constructor:  needed to avoid excessive RTTI*/
      virtual Segment* clone() const = 0;
     
      /** returns the vector of Trk::MeasurementBase objects 
        - specific for this TrackSegment: Trk::MeasurementBase (generic)
        */
      const std::vector<const Trk::MeasurementBase*>& containedMeasurements() const;

      const DataVector<const Trk::MeasurementBase>& containedMeasurementsDataVector() const;

      bool hasContainedMeasurements() const; 
      /** Return the number of contained Trk::MeasurementBase (s)*/
      unsigned int numberOfMeasurementBases() const ;

      /** returns the Trk::MeasurementBase objects depending on the integer*/
      const MeasurementBase* measurement(unsigned int) const;
           
      /** return the FitQuality object, returns NULL if no FitQuality is defined
        - extends Trk::MeasurementBase */
      const FitQuality* fitQuality() const;
    
      /**return number of Trk::Segments currently created (used in TrkEDM_Monitor)*/
      static unsigned int numberOfInstantiations() ;

      /** return segment author */
      Author author() const;

      /** returns human readble string version of author */
      std::string dumpAuthor() const;

    protected:
      friend class ::SegmentCnv_p1;
      
      /** The fit quality of the Segment */
      FitQuality* m_fitQuality;  
       
      /** The vector of contained (generic) Trk::MeasurementBase objects */
      DataVector<const MeasurementBase>* m_containedMeasBases;   
  
      /** number of objects of this type in memory */
      static unsigned int  s_numberOfInstantiations;

      /** segment author */
      Author m_author;
};

inline const FitQuality* Segment::fitQuality() const 
{ 
    return m_fitQuality;  
}

inline const std::vector<const MeasurementBase*>& Segment::containedMeasurements() const
{ 
     return m_containedMeasBases->stdcont(); 
}
 
inline const DataVector<const MeasurementBase>& Segment::containedMeasurementsDataVector() const
{ 
     return *m_containedMeasBases; 
}
 
inline bool Segment::hasContainedMeasurements() const
{ 
  return m_containedMeasBases != nullptr;
}
 
inline const MeasurementBase* Segment::measurement(unsigned int indx) const
 { if (m_containedMeasBases && indx<m_containedMeasBases->size())
      return m_containedMeasBases->operator[](indx);
   return 0;
 }
 
inline unsigned int Segment::numberOfMeasurementBases() const
{ 
     return m_containedMeasBases->size(); 
}  

inline Segment::Author Segment::author() const 
{
   return m_author;
}
}

#endif // TRKSEGMENT_SEGMENT_H


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RIO_OnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKRIO_ONTRACK_RIO_ONTRACK_H
#define TRKRIO_ONTRACK_RIO_ONTRACK_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/LocalParameters.h"
// Identifier
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <ostream>
class MsgStream;
class RIO_OnTrackCnv_p1;
class RIO_OnTrackCnv_p2;


namespace Trk {
  
  class PrepRawData;
  class TrkDetElementBase;
  class ITrkEventCnvTool;

/** @class RIO_OnTrack
    
  Class to handle RIO On Tracks  ROT) for InDet and Muons,
  it inherits from the common MeasurementBase. 
    
  The Track holds a vector of TrackStateOnSurface 
  that carry the MeasurmentBase class pointers.
    
  Need a multimap{RIO, ROT} to go from RIO to ROT
  and a map{ROT, Track} to go from ROT to Track 
        
  @author Veronique.Boisvert@cern.ch 
  @author Edward.Moyse@cern.ch
  @author Andreas.Salzburger@cern.ch
 */

  class RIO_OnTrack : public MeasurementBase {
    
    friend class ITrkEventCnvTool;
      
    public:
      /** Default Constructor for POOL */
      RIO_OnTrack();
      /** Copy Constructor */
      RIO_OnTrack(const RIO_OnTrack& rot);

      /** Assignment operator */
      RIO_OnTrack& operator=(const RIO_OnTrack& rot);

      /** Constructor with parameters and without externalPrediction: LocalParameters, 
      LocalAmg::MatrixX, id&
      This class owns the LocalParameters and the error matrix */
      RIO_OnTrack( const LocalParameters& locpars,
                   const Amg::MatrixX& loccov,
                   const Identifier& id);
    
      /** Destructor */
      virtual ~RIO_OnTrack();

      /** Pseudo-constructor, needed to avoid excessive RTTI*/
      virtual RIO_OnTrack* clone() const = 0;
                
     /** returns the surface for the local to global transformation 
      - interface from MeasurementBase */
      virtual const Surface& associatedSurface() const = 0;
     
     /**Interface method to get the global Position
      - interface from MeasurementBase */
      virtual const Amg::Vector3D& globalPosition() const = 0;

      /**returns the some information about this RIO_OnTrack. */
      virtual MsgStream&    dump( MsgStream& out ) const;  

      /**returns the some information about this RIO_OnTrack. */
      virtual std::ostream& dump( std::ostream& out ) const;
           
     /** returns the PrepRawData (also known as  RIO) object to which this RIO_OnTrack is associated.
      Can be null (in case where the Trk::PrepRawData is not persistified). 
      Use Detector Element if possible (this is always there).
      - extends MeasurementBase */
      virtual const Trk::PrepRawData* prepRawData() const = 0;

       /** returns the DE hashID 
      - extends MeasurementBase */
      virtual IdentifierHash idDE() const = 0;
    
       /** returns the detector element, assoicated with the PRD of this class
      - extends MeasurementBase */
      virtual const TrkDetElementBase* detectorElement() const = 0;
    
       /** return the identifier
      -extends MeasurementBase */
      virtual Identifier identify() const final;

      /**return number of parameters currently created*/
      static unsigned int numberOfInstantiations() ;
     
    protected:
      friend class ::RIO_OnTrackCnv_p1;
      friend class ::RIO_OnTrackCnv_p2;
      /** ONLY for use in custom convertor
      Allows the custom convertor to reset values when persistying/reading back RoTs*/
      virtual void setValues(
          const Trk::TrkDetElementBase* detEl, 
          const Trk::PrepRawData* prd)=0;
        
      /**Identifier of the RIO_OnTrack (comes from the associated Trk::PrepRawData)*/     
      Identifier m_identifier; 

      /** number of objects of this type in memory */
      static unsigned int s_numberOfInstantiations;
  };
  
  inline Identifier RIO_OnTrack::identify() const     
  { return m_identifier; }
}

#endif // TRKRIO_ONTRACK_RIO_ONTRACK_H


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftCircleOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRIO_ONTRACK_TRT_DRIFTCIRCLEONTRACK_H
#define INDETRIO_ONTRACK_TRT_DRIFTCIRCLEONTRACK_H

// Base classes
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "TrkEventPrimitives/DriftCircleStatus.h"
#include "TrkEventPrimitives/DriftCircleSide.h"

// InDet


// for ElementLink to IdentifiableContainer PixelClusterContainer
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "AthLinks/ElementLink.h"

#include "GaudiKernel/GaudiException.h"

#include <atomic>

namespace Trk {
  class Surface;
  class LocalParameters;
  class ITrkEventCnvTool;
}

namespace InDetDD {
  class TRT_BaseElement;
}

typedef ElementLink<InDet::TRT_DriftCircleContainer> ElementLinkToIDCTRT_DriftCircleContainer;

namespace InDet{
  class TRT_DriftCircle;

  /**@class TRT_DriftCircleOnTrack
  Represents 'corrected' measurements from the TRT (for example, corrected for wire sag). The 
  equivalent 'uncorrected' class is InDet::TRT_DriftCircle*
     
  @author Edward.Moyse@cern.ch
  @author Andreas.Salzburger@cern.ch
   */

  class TRT_DriftCircleOnTrack :   public Trk::RIO_OnTrack{
	
    public:
      friend class  Trk::ITrkEventCnvTool;
      /**Default constructor - needed for POOL*/
      TRT_DriftCircleOnTrack();
      /**Copy constructor */
      TRT_DriftCircleOnTrack(const TRT_DriftCircleOnTrack &);
      /**Assignment operator*/
      TRT_DriftCircleOnTrack &operator=(const TRT_DriftCircleOnTrack &);
      /**Move assignment operator*/
      TRT_DriftCircleOnTrack &operator=(TRT_DriftCircleOnTrack &&);
		
     /** Constructor with parameters and without externalPrediction:
      RIO pointer, LocalPosition*, LocalErrorMatrix*, 
      local position of center of drift circle*, global position of center of drift 
      circle, side on which the drift radius is wrt the track
      The base class owns local position, error matrix, 
      this class owns locCenterDriftCircle and globCenterDriftCircle */
      TRT_DriftCircleOnTrack( const InDet::TRT_DriftCircle* RIO, 
                              const Trk::LocalParameters& driftRadius,
                              const Amg::MatrixX& errDriftRadius, 
                              const IdentifierHash& idDE,
                              double predictedLocZ,
                              const Amg::Vector3D& predictedTrackDirection, 
                              const Trk::DriftCircleStatus status);

      TRT_DriftCircleOnTrack( const ElementLinkToIDCTRT_DriftCircleContainer& RIO,
                              const Trk::LocalParameters& driftRadius,
                              const Amg::MatrixX& errDriftRadius, 
                              IdentifierHash idDE,
                              const Identifier& id,
                              double predictedLocZ,
                              float localAngle,
                              const Trk::DriftCircleStatus status,
                              bool highLevel,
                              double timeOverThreshold);
	
      /**Destructor */
      virtual ~TRT_DriftCircleOnTrack();
		
      /**allows copying without losing the type information. Used in Trk::Track*/
      TRT_DriftCircleOnTrack* clone() const ;
	
     /** return the global position of this RIO_OnTrack
      @todo convention about z coordinate 
      - fullfills Trk::MeasurementBase interface
      */
      const Amg::Vector3D& globalPosition() const final;
     
     /** returns the surface for the local to global transformation
      - fullfills Trk::MeasurementBase interface 
      */
      const Trk::Surface& associatedSurface() const;
     
     
     /** returns the PrepRawData - is a TRT_DriftCircle in this scope
      - fullfills Trk::RIO_OnTrack interface
      */
      const TRT_DriftCircle* prepRawData() const;
      const ElementLinkToIDCTRT_DriftCircleContainer& prepRawDataLink() const;
    
     /** returns the DE hashID
      - fullfills Trk::RIO_OnTrack interface
      */
      IdentifierHash idDE() const;
  
     /** returns the detector element, assoicated with the PRD of this class
      - fullfills Trk::RIO_OnTrack interface
      */
      const InDetDD::TRT_BaseElement* detectorElement() const;

     /** returns the side on which the drift radius is. 
      (for more information see the definition of
      TrkEventPrimitives/DriftCirlceSide)
      - extends Trk::RIO_OnTrack interface*/
      Trk::DriftCircleSide side() const;

     /** returns the status of the  drift radius calibration. 
      (for more information see the definition of 
      TrkEventPrimitives/DriftCirlceStatus)
      - extends Trk::RIO_OnTrack interface*/
      Trk::DriftCircleStatus status() const;

         
     /** returns true if the high level threshold was passed
      - extends Trk::RIO_OnTrack interface*/
      bool highLevel() const ;

     /** returns time over threshold in ns for valid digits; zero otherwise
      - extends Trk::RIO_OnTrack interface*/
      double timeOverThreshold() const;

      /**returns some information about this RIO_OnTrack.*/
      virtual MsgStream&    dump( MsgStream& out ) const;	
      /**returns some information about this RIO_OnTrack.*/
      virtual std::ostream& dump( std::ostream& out ) const;

      float localAngle() const;
      float positionAlongWire() const;

    private:
    /** ONLY for use in custom convertor
      Allows the custom convertor to reset values when persistying/reading back RoTs*/
      virtual void setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prd);
 
    /** @brief Uses the passed loc3Dframe to calculate and set the global coord of this hit. 
       The detector element surface is used*/
      void setGlobalPosition(Amg::Vector3D& loc3Dframe) const;   
 
      /** global position to be cached */
      CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;
      
      /**local angle to be written out */     
      mutable std::atomic<float> m_localAngle;
      
      /**local position along wire to be written out*/
      mutable std::atomic<float> m_positionAlongWire;
      
      /** the contained RIO (PRD, PrepRawData) - TRT_DriftCircle in this case */
//       mutable const InDet::TRT_DriftCircle*  m_rio;

      ElementLinkToIDCTRT_DriftCircleContainer m_rio;
      
      /** the IdentifierHash - probably not used*/
      IdentifierHash                         m_idDE;
            
      /** the information how the (possibly signed) drift radius should
          be understood */
      Trk::DriftCircleStatus                 m_status;
      
      /** the high level flag */
      bool                                   m_highLevel;
      
      /** the time over threshold in nsec */
      double                                 m_timeOverThreshold;

      /** corresponding detector element*/
      const InDetDD::TRT_BaseElement*        m_detEl;

  };

  inline TRT_DriftCircleOnTrack* TRT_DriftCircleOnTrack::clone() const 
  { 
    return new TRT_DriftCircleOnTrack(*this); 
  }
  
  inline const TRT_DriftCircle* TRT_DriftCircleOnTrack::prepRawData() const
  { 
    // somehow one has to ask first if it is valid ... otherwise it always returns 0 ...
    if (m_rio.isValid()) return m_rio.cachedElement();
    else return 0;
  }

  inline
  const ElementLinkToIDCTRT_DriftCircleContainer&
  TRT_DriftCircleOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }
  
  inline IdentifierHash TRT_DriftCircleOnTrack::idDE() const
  { 
    return m_idDE; 
  }
      
  inline const InDetDD::TRT_BaseElement* TRT_DriftCircleOnTrack::detectorElement() const
  { 
    return m_detEl;
  }
      
  inline  Trk::DriftCircleStatus TRT_DriftCircleOnTrack::status() const
  { 
    return m_status; 
  }

  inline bool TRT_DriftCircleOnTrack::highLevel() const 
  { 
    return m_highLevel; 
  }

  inline double TRT_DriftCircleOnTrack::timeOverThreshold() const
  {
    return m_timeOverThreshold;
  }

  inline float TRT_DriftCircleOnTrack::localAngle() const
  {
    return m_localAngle;
  }

  inline float TRT_DriftCircleOnTrack::positionAlongWire() const
  {
    return m_positionAlongWire;
  }
}

#endif // TRKRIO_ONTRACK_TRT_DRIFTCIRCLEONTRACK_H


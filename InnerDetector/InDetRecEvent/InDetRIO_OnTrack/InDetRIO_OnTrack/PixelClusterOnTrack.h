/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusterOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRIO_ONTRACK_PIXELCLUSTERONTRACK_H
#define INDETRIO_ONTRACK_PIXELCLUSTERONTRACK_H

// Base classes
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkEventPrimitives/LocalParameters.h"

// for ElementLink to IdentifiableContainer PixelClusterContainer
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "AthLinks/ElementLink.h"

class FakeTrackBuilder;
namespace Trk {
  class Surface;
  class TrkDetElementBase;
  class ITrkEventCnvTool;
}

namespace InDetDD {
  class SiDetectorElement;
}

typedef ElementLink<InDet::PixelClusterContainer> ElementLinkToIDCPixelClusterContainer;

namespace InDet {

  class PixelDetectorElement;
  class LocalParameters;

  /** @class PixelClusterOnTrack
  Specific class to represent the pixel measurements.
  At the moment the only extension is whether there was an ambiguity with the assignment of this 
  cluster.
     
  @author Veronique.Boisvert@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
   */

  class PixelClusterOnTrack :   public SiClusterOnTrack {

    public:
      friend class  Trk::ITrkEventCnvTool;
      /**Default constructor - needed for POOL */
      PixelClusterOnTrack();
      /**Copy constructor */
      PixelClusterOnTrack(const PixelClusterOnTrack &);
      /**Assignment operator*/
      PixelClusterOnTrack &operator=(const PixelClusterOnTrack &);

      /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition&, LocalErrorMatrix&, idDE&
      Everything else is owned elsewhere. */
      PixelClusterOnTrack( const PixelCluster* RIO, 
                           const Trk::LocalParameters& locpars, 
                           const Amg::MatrixX& locerr, 
                           const IdentifierHash& idDE,
                           bool hasAmbiguity=false, 
			   // this parameter is ignored, 
			   // information taken from RIO
			   // Just kept not to break the interface to
			   // already existing code. 
                           bool isbroad=false
                         ); 

    
      /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition&, LocalErrorMatrix&, idDE&,
      Global Position.
      Everything else is owned elsewhere. */
      PixelClusterOnTrack( const PixelCluster* RIO, 
                           const Trk::LocalParameters& locpars, 
                           const Amg::MatrixX& locerr, 
                           const IdentifierHash& idDE,
                           const Amg::Vector3D& globalPosition,
                           bool hasAmbiguity=false, 
			   // this parameter is ignored, 
			   // information taken from RIO
			   // Just kept not to break the interface to
			   // already existing code. 
                           bool isbroad=false
                         ); 
	
      PixelClusterOnTrack( const ElementLinkToIDCPixelClusterContainer& RIO,
                           const Trk::LocalParameters& locpars, 
                           const Amg::MatrixX& locerr, 
                           const IdentifierHash& idDE,
                           const Identifier& id,
                           float energyLoss,
                           bool isFake,
                           bool hasClusterAmbiguity,
                           bool isbroad
                         ); 
	
      /** Destructor: */
      virtual ~PixelClusterOnTrack();
	
      /** Pseudo-constructor : needed to avoid excessive RTTI*/
      virtual PixelClusterOnTrack* clone() const;
    
    /** returns the surface for the local to global transformation 
      - fullfills Trk::MeasurementBase interface*/
      const Trk::Surface& associatedSurface() const;

    /** returns the PrepRawData - is a SiCluster in this scope
      - fullfills Trk::RIO_OnTrack interface*/
      const PixelCluster* prepRawData() const;

      const ElementLinkToIDCPixelClusterContainer& prepRawDataLink() const;
       
    /** returns the detector element, assoicated with the PRD of this class
      - fullfills Trk::RIO_OnTrack interface*/
      const InDetDD::SiDetectorElement* detectorElement() const;
   
      
	  /** returns whether there was an ambiguity associated with this pixel cluster.
      - extends the Trk::RIO_OnTrack interface*/
      bool hasClusterAmbiguity() const;
      /** returns whether this cluster is likely to be the fake mirror 
	  image of a ganged pixel.
	  Is it set if the cluster is a single hit cluster and the ganged
	  pixel instead is part of a bigger cluster.*/
      bool isFake() const;
      /** returns the energy loss in MeV associated to this cluster.
	  It is 0 if no calibration data is used in clusterization*/
      float energyLoss() const;
		
      /**returns some information about this RIO_OnTrack.*/
      MsgStream&  dump( MsgStream& out ) const;	
	
      /**returns some information about this RIO_OnTrack.*/
      std::ostream& dump( std::ostream& out ) const;

    private:
      friend class PixelClusterOnTrackCnv_p1;
      friend class ::FakeTrackBuilder;
      /** ONLY for use in custom convertor
      Allows the custom convertor to reset values when persistying/reading back RoTs*/
      virtual void setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prd);

      /** PixelCluster - the RIO (PRD, PrepRawData)*/
      ElementLinkToIDCPixelClusterContainer m_rio;
//       mutable const PixelCluster*   m_rio;
    
      /** records whether there is an ambiguity about this cluster*/
      bool                          m_hasClusterAmbiguity;   
      /** records whether this cluster would be removed by the internal 
          solving of ganged pixel ambiguities */
      bool                          m_isFake;   
      /** get energy deposited in the cluster, in MeV */
      float m_energyLoss; 
      /** corresponding detector element*/
      const InDetDD::SiDetectorElement* m_detEl;

  };

  inline PixelClusterOnTrack* PixelClusterOnTrack::clone() const 
  { 
    return new PixelClusterOnTrack(*this); 
  }

  inline const PixelCluster* PixelClusterOnTrack::prepRawData() const
  { 
    // somehow one has to ask first if it is valid ... otherwise it always returns 0 ...
      if (m_rio.isValid()) return m_rio.cachedElement(); 
      else return 0;

  }
    
  inline const ElementLinkToIDCPixelClusterContainer&
  PixelClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }
    
  inline const InDetDD::SiDetectorElement* PixelClusterOnTrack::detectorElement() const
  { 
    return m_detEl;
  }
   
  inline bool PixelClusterOnTrack::hasClusterAmbiguity() const 
  { 
    return m_hasClusterAmbiguity; 
  }
  
  inline bool PixelClusterOnTrack::isFake() const 
  { 
    return m_isFake; 
  }
  
  inline float  PixelClusterOnTrack::energyLoss() const 
  { 
    return m_energyLoss; 
  }

}

#endif // TRKRIO_ONTRACK_SICLUSTERONTRACK_H

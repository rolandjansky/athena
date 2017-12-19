/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_ClusterOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRIO_ONTRACK_SCTCLUSTERONTRACK_H
#define INDETRIO_ONTRACK_SCTCLUSTERONTRACK_H

// Base classes
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SCT_Cluster.h"

// for ElementLink to IdentifiableContainer SCT_ClusterContainer
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "AthLinks/ElementLink.h"

namespace Trk {
  class Surface;
  class LocalParameters;
  class TrkDetElementBase;
  class ITrkEventCnvTool;
}

namespace InDetDD {
   class SiDetectorElement;
}

typedef ElementLink<InDet::SCT_ClusterContainer> ElementLinkToIDCSCT_ClusterContainer;

namespace InDet{

  /**@class SCT_ClusterOnTrack
  Specific class to represent the SCT measurements.
  It does not currently extend the interface of InDet::SiClusterOnTrack.
     
  @author Veronique.Boisvert@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
   */
  class SCT_ClusterOnTrack :   public SiClusterOnTrack{

    public:
      friend class  Trk::ITrkEventCnvTool;
      /**For POOL only. Do not use*/
      SCT_ClusterOnTrack();
      /**Copy constructor*/
      SCT_ClusterOnTrack(const SCT_ClusterOnTrack &);

    /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      SCT_ClusterOnTrack( const InDet::SCT_Cluster* RIO, 
                          const Trk::LocalParameters* locpars, 
                          const Amg::MatrixX* locerr, 
                          const IdentifierHash& idDE,
                          bool isbroad=false); 
                        
	  /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&,
      Global Position
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      SCT_ClusterOnTrack( const InDet::SCT_Cluster* RIO, 
                          const Trk::LocalParameters* locpars, 
                          const Amg::MatrixX* locerr, 
                          const IdentifierHash& idDE,
                          const Amg::Vector3D& globalPosition,
                          bool isbroad=false); 
      
    /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      SCT_ClusterOnTrack( const InDet::SCT_Cluster* RIO, 
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          const IdentifierHash& idDE,
                          bool isbroad=false); 
                        
	  /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&,
      Global Position
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      SCT_ClusterOnTrack( const InDet::SCT_Cluster* RIO, 
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          const IdentifierHash& idDE,
                          const Amg::Vector3D& globalPosition,
                          bool isbroad=false); 
      
      SCT_ClusterOnTrack( const ElementLinkToIDCSCT_ClusterContainer& RIO,
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          IdentifierHash idDE,
                          const Identifier& id,
                          bool isbroad,
                          double positionAlongStrip);

      /**Assignment operator*/
      SCT_ClusterOnTrack &operator=(const SCT_ClusterOnTrack &);

      /** Destructor */
      virtual ~SCT_ClusterOnTrack();
     
      /** returns global position (gathered through Surface constraint)
      - fullfills Trk::MeasurementBase interface
        Overload of the method in parent class */
      virtual const Amg::Vector3D& globalPosition() const final;
     
      /** Pseudo-constructor */
      SCT_ClusterOnTrack* clone() const ;

    /** returns the surface for the local to global transformation
      - fullfills the Trk::MeasurementBase interface
     */
      const Trk::Surface& associatedSurface() const;

    	
    /** returns the PrepRawData - is a SCT_Cluster in this scope
      - fullfills the Trk::RIO_OnTrack interface
     */
      virtual const InDet::SCT_Cluster* prepRawData() const;

    const ElementLinkToIDCSCT_ClusterContainer& prepRawDataLink() const;
     
  
    /** returns the detector element, assoicated with the PRD of this class
      - fullfills the Trk::RIO_OnTrack interface
     */
      virtual const InDetDD::SiDetectorElement* detectorElement() const;
    
      /**returns some information about this RIO_OnTrack.*/
      virtual MsgStream&    dump( MsgStream& out ) const;	
	
      /**returns some information about this RIO_OnTrack.*/
      virtual std::ostream& dump( std::ostream& out ) const;

      double positionAlongStrip() const;

    private:
    /** ONLY for use in custom convertor
      Allows the custom convertor to reset values when persistying/reading back RoTs*/
      virtual void setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prd);
 
      /** SCT_Cluster - the RIO (PRD, PrepRawData)*/
//       mutable const SCT_Cluster*   m_rio;
      ElementLinkToIDCSCT_ClusterContainer m_rio; 
      /** corresponding detector element*/
      const InDetDD::SiDetectorElement* m_detEl;
      
      double m_positionAlongStrip;
      
  };

  inline SCT_ClusterOnTrack* SCT_ClusterOnTrack::clone() const 
  { 
    return new SCT_ClusterOnTrack(*this); 
  }
    
  inline const SCT_Cluster* SCT_ClusterOnTrack::prepRawData() const
  { 
    // somehow one has to ask first if it is valid ... otherwise it always returns 0 ...
    if (m_rio.isValid()) return m_rio.cachedElement();
    else return 0;
  }  
  
  inline const ElementLinkToIDCSCT_ClusterContainer&
  SCT_ClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }
    
  inline const InDetDD::SiDetectorElement* SCT_ClusterOnTrack::detectorElement() const
  { 
    return m_detEl; 
  }

  inline double SCT_ClusterOnTrack::positionAlongStrip() const
  {
    return m_positionAlongStrip;
  }
      

}//end of namespace definitions

#endif // TRKRIO_ONTRACK_SICLUSTERONTRACK_H

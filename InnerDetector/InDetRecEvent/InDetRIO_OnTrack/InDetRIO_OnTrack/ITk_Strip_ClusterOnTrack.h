/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITk_Strip_ClusterOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETRIO_ONTRACK_ITKSTRIPCLUSTERONTRACK_H
#define INDETRIO_ONTRACK_ITKSTRIPCLUSTERONTRACK_H

// Base classes
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/ITk_Strip_Cluster.h"

// for ElementLink to IdentifiableContainer ITk_Strip_ClusterContainer
#include "InDetPrepRawData/ITk_Strip_ClusterContainer.h"
#include "DataModel/ElementLink.h"

namespace Trk {
  class Surface;
  class LocalParameters;
  class TrkDetElementBase;
  class ITrkEventCnvTool;
}

namespace InDetDD {
   class SiDetectorElement;
}

typedef ElementLink<InDet::ITk_Strip_ClusterContainer> ElementLinkToIDCITk_Strip_ClusterContainer;

namespace InDet{

  /**@class ITk_Strip_ClusterOnTrack
  Specific class to represent the Strip measurements.
  It does not currently extend the interface of InDet::SiClusterOnTrack.
     
  @author Veronique.Boisvert@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
   */
  class ITk_Strip_ClusterOnTrack :   public SiClusterOnTrack{

    public:
      friend class  Trk::ITrkEventCnvTool;
      /**For POOL only. Do not use*/
      ITk_Strip_ClusterOnTrack();
      /**Copy constructor*/
      ITk_Strip_ClusterOnTrack(const ITk_Strip_ClusterOnTrack &);

    /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      ITk_Strip_ClusterOnTrack( const InDet::ITk_Strip_Cluster* RIO, 
                          const Trk::LocalParameters* locpars, 
                          const Amg::MatrixX* locerr, 
                          const IdentifierHash& idDE,
                          bool isbroad=false); 
                        
	  /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&,
      Global Position
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      ITk_Strip_ClusterOnTrack( const InDet::ITk_Strip_Cluster* RIO, 
                          const Trk::LocalParameters* locpars, 
                          const Amg::MatrixX* locerr, 
                          const IdentifierHash& idDE,
                          const Amg::Vector3D& globalPosition,
                          bool isbroad=false); 
      
    /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      ITk_Strip_ClusterOnTrack( const InDet::ITk_Strip_Cluster* RIO, 
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          const IdentifierHash& idDE,
                          bool isbroad=false); 
                        
	  /** Constructor with parameters :
      RIO/PrepRawData pointer, LocalPosition*, LocalErrorMatrix*, idDE&,
      Global Position
      The base class owns local position, error matrix, this class owns global pos. 
      Everything else is owned elsewhere. */
      ITk_Strip_ClusterOnTrack( const InDet::ITk_Strip_Cluster* RIO, 
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          const IdentifierHash& idDE,
                          const Amg::Vector3D& globalPosition,
                          bool isbroad=false); 
      
      ITk_Strip_ClusterOnTrack( const ElementLinkToIDCITk_Strip_ClusterContainer& RIO,
                          const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr, 
                          IdentifierHash idDE,
                          const Identifier& id,
                          bool isbroad,
                          double positionAlongStrip);

      /**Assignment operator*/
      ITk_Strip_ClusterOnTrack &operator=(const ITk_Strip_ClusterOnTrack &);

      /** Destructor */
      virtual ~ITk_Strip_ClusterOnTrack();
     
      /** returns global position (gathered through Surface constraint)
      - fullfills Trk::MeasurementBase interface
        Overload of the method in parent class */
      virtual const Amg::Vector3D& globalPosition() const final;
     
      /** Pseudo-constructor */
      ITk_Strip_ClusterOnTrack* clone() const ;

    /** returns the surface for the local to global transformation
      - fullfills the Trk::MeasurementBase interface
     */
      const Trk::Surface& associatedSurface() const;

    	
    /** returns the PrepRawData - is a ITk_Strip_Cluster in this scope
      - fullfills the Trk::RIO_OnTrack interface
     */
      virtual const InDet::ITk_Strip_Cluster* prepRawData() const;

    const ElementLinkToIDCITk_Strip_ClusterContainer& prepRawDataLink() const;
     
  
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
 
      /** ITk_Strip_Cluster - the RIO (PRD, PrepRawData)*/
//       mutable const ITk_Strip_Cluster*   m_rio;
      ElementLinkToIDCITk_Strip_ClusterContainer m_rio; 
      /** corresponding detector element*/
      const InDetDD::SiDetectorElement* m_detEl;
      
      double m_positionAlongStrip;
      
  };

  inline ITk_Strip_ClusterOnTrack* ITk_Strip_ClusterOnTrack::clone() const 
  { 
    return new ITk_Strip_ClusterOnTrack(*this); 
  }
    
  inline const ITk_Strip_Cluster* ITk_Strip_ClusterOnTrack::prepRawData() const
  { 
    // somehow one has to ask first if it is valid ... otherwise it always returns 0 ...
    if (m_rio.isValid()) return m_rio.cachedElement();
    else return 0;
  }  
  
  inline const ElementLinkToIDCITk_Strip_ClusterContainer&
  ITk_Strip_ClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }
    
  inline const InDetDD::SiDetectorElement* ITk_Strip_ClusterOnTrack::detectorElement() const
  { 
    return m_detEl; 
  }

  inline double ITk_Strip_ClusterOnTrack::positionAlongStrip() const
  {
    return m_positionAlongStrip;
  }
      

}//end of namespace definitions

#endif // TRKRIO_ONTRACK_SICLUSTERONTRACK_H

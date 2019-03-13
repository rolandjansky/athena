/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiClusterOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKRIO_ONTRACK_SICLUSTERONTRACK_H
#define TRKRIO_ONTRACK_SICLUSTERONTRACK_H

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "Identifier/IdentifierHash.h"

class SiClusterOnTrackCnv_p1;

namespace Trk 
{
  class LocalParameters;
}

namespace InDetDD
{
  class SiDetectorElement;
}

namespace InDet {

  class SiCluster;

  /**@class SiClusterOnTrack
  RIO_OnTrack base class for Silicon detector in the InnerDetector.
  It combines the common members / structures from SCT and 
  Pixel clusters on tracks.
  
  @author Veronique.Boisvert@cern.ch, Edward.Moyse@cern.ch, Andreas.Salzburger@cern.ch
   */
  class SiClusterOnTrack :   public Trk::RIO_OnTrack{


    public:
      /** Default Constructor - needed for POOL */
      SiClusterOnTrack();
      /** Copy constructor */
      SiClusterOnTrack(const SiClusterOnTrack &);
      /** Assignment operator */
      SiClusterOnTrack &operator=(const SiClusterOnTrack &);

    
      /** Constructor with parameters :
      LocalParameters&, 
      ErrorMatrix&, 
      idDE& */
      SiClusterOnTrack(const Trk::LocalParameters& locpos, 
                       const Amg::MatrixX& locerr, 
                       const IdentifierHash& idDE,
		       const Identifier& id,
                       bool isbroad=false); 
                        	
      /** Constructor with parameters :
      LocalParameters&, 
      ErrorMatrix&, 
      idDE&,
      GlobalPosition&,
      */
      SiClusterOnTrack( const Trk::LocalParameters& locpos, 
                        const Amg::MatrixX& locerr, 
                        const IdentifierHash& idDE,
			const Identifier& id,
                        const Amg::Vector3D& globalPosition,
                        bool isbroad=false); 
	
      /** Destructor:*/
      virtual ~SiClusterOnTrack();
	
      /** returns global position (gathered through Surface constraint)
      - fullfills Trk::MeasurementBase interface */
      virtual const Amg::Vector3D& globalPosition() const;
    	  
      /** returns the DE hashID* 
      - fullfills Trk::RIO_OnTrack interface*/
      IdentifierHash idDE() const;
        
      bool isBroadCluster() const;

	    /**returns some information about this RIO_OnTrack.
      - fullfills Trk::RIO_OnTrack interface*/
      virtual MsgStream&    dump( MsgStream& out ) const;	
	
	    /**returns some information about this RIO_OnTrack.
      - fullfills Trk::RIO_OnTrack interface*/
      virtual std::ostream& dump( std::ostream& out ) const;

    protected:
      friend class ::SiClusterOnTrackCnv_p1;

      /** ONLY for use in custom convertor
      Allows the custom convertor to reset values when persistying/reading back RoTs*/
      virtual void setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prd)=0;

      /** The IdentifierHash - probably not used*/
      IdentifierHash                      m_idDE;
      /** The global position */
      CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;
      bool m_isbroad;
  };

  inline  IdentifierHash SiClusterOnTrack::idDE() const
  { 
    return m_idDE; 
  }

  inline bool SiClusterOnTrack::isBroadCluster() const
  {
    return m_isbroad;
  }
}

#endif // TRKRIO_ONTRACK_SICLUSTERONTRACK_H

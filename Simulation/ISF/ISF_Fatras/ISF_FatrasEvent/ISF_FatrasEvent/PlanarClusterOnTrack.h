/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarClusterOnTrack.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASEVENT_PLANARCLUSTERONTRACK_H
#define ISF_FATRASEVENT_PLANARCLUSTERONTRACK_H

// Base class
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "CxxUtils/CachedUniquePtr.h"
#include "Identifier/IdentifierHash.h"

// for ElementLink to IdentifiableContainer PlanarClusterContainer
#include "ISF_FatrasEvent/PlanarClusterContainer.h"
#include "AthLinks/ElementLink.h"

namespace Trk  {
  class LocalParameters; }

typedef ElementLink<iFatras::PlanarClusterContainer> ElementLinkToIDCPlanarClusterContainer;

namespace iFatras {

  class PlanarDetElement;
  class PlanarCluster;

  /**
     @class PlanarClusterOnTrack
     RIO_OnTrack class for PlanarDetElement

     @author Noemi.Calace-at-cern.ch
  */
  
  class PlanarClusterOnTrack : public Trk::RIO_OnTrack {
    
  public:
    /** Default constructor */
    PlanarClusterOnTrack();
    
    /** Copy constructor */
    PlanarClusterOnTrack( const PlanarClusterOnTrack &);

    /** Assignment operator */
    PlanarClusterOnTrack &operator= ( const PlanarClusterOnTrack &);
    
    /** Constructor with parameters */
    PlanarClusterOnTrack( const PlanarCluster * RIO,
			  const Trk::LocalParameters& locpos,
			  const Amg::MatrixX& locerr,
			  const IdentifierHash& idDE);
			  
			  
    
    PlanarClusterOnTrack( const PlanarCluster * RIO,
			  const Trk::LocalParameters& locpos,
			  const Amg::MatrixX& locerr,
			  const IdentifierHash& idDE,
			  const Amg::Vector3D& globalPosition);
    
    /** Deconstructor */
    virtual ~PlanarClusterOnTrack();

    /** Pseudo-constructor : needed to avoid excessive RTTI*/
    virtual PlanarClusterOnTrack* clone() const override ;

    /** returns the surface for the local to global transformation 
	- fullfills Trk::MeasurementBase interface*/
    virtual const Trk::Surface& associatedSurface() const override;

    /** returns the PrepRawData 
	- fullfills Trk::RIO_OnTrack interface*/
    virtual const PlanarCluster* prepRawData() const override;

    /** returns the detector element, assoicated with the PRD of this class
	- fullfills Trk::RIO_OnTrack interface*/
    virtual const iFatras::PlanarDetElement* detectorElement() const override;
    
    /** returns global position (gathered through Surface constraint)
	- fullfills Trk::MeasurementBase interface */
    virtual const Amg::Vector3D& globalPosition() const override;

    /** returns global position (gathered through Surface constraint)
	- fullfills Trk::MeasurementBase interface */
    const Amg::Vector3D* globalPositionPtr() const;

    /** checks RIO Type
	- fullfills Trk::RIO_OnTrack interface */
    virtual bool rioType(Trk::RIO_OnTrackType::Type type) const override
    {
      return (type == Trk::RIO_OnTrackType::MdtDriftCircle);
    }

    /** returns the DE hashID* 
	- fullfills Trk::RIO_OnTrack interface */
    virtual IdentifierHash idDE() const override;
    
    /** returns some information about this RIO_OnTrack.
	- fullfills Trk::RIO_OnTrack interface */
    virtual MsgStream&    dump( MsgStream& out ) const override;   

    /** returns some information about this RIO_OnTrack.
	- fullfills Trk::RIO_OnTrack interface */
    virtual std::ostream& dump( std::ostream& out ) const override;
    
  protected:
    /** ONLY for use in custom convertor
	Allows the custom convertor to reset values when persistying/reading back RoTs*/
    virtual void setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prd) override;

    /** PlanarCluster - the RIO (PRD, PrepRawData)*/
    ElementLinkToIDCPlanarClusterContainer m_rio;
    
    /** The IdentifierHash - probably not used*/
    IdentifierHash                      m_idDE;

    /** corresponding detector element*/
    const iFatras::PlanarDetElement* m_detEl;

    /** The global position */
    CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;
    
  };

  inline const Trk::Surface& PlanarClusterOnTrack::associatedSurface() const
  { return ( detectorElement()->surface()); }

  inline const Amg::Vector3D* PlanarClusterOnTrack::globalPositionPtr() const {
    if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(associatedSurface().localToGlobal(localParameters())));
    return m_globalPosition.get(); }
  
  inline const Amg::Vector3D& PlanarClusterOnTrack::globalPosition() const {
    return *globalPositionPtr(); }
  
  inline PlanarClusterOnTrack* PlanarClusterOnTrack::clone() const {
    return new PlanarClusterOnTrack(*this); }

  inline const PlanarCluster* PlanarClusterOnTrack::prepRawData() const {
    if (m_rio.isValid()) return m_rio.cachedElement();
    else return 0; }

  inline const iFatras::PlanarDetElement * PlanarClusterOnTrack::detectorElement() const {
    return m_detEl; }

  inline  IdentifierHash PlanarClusterOnTrack::idDE() const { 
    return m_idDE; }
  
}

#endif // ISF_FATRASEVENT_PLANARCLUSTERONTRACK_H

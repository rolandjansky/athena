/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRIOONTRACK_STGCCLUSTERONTRACK_H
#define MUONRIOONTRACK_STGCCLUSTERONTRACK_H

// Base classes
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonPrepRawData/sTgcPrepData.h"
// needed classes
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

typedef ElementLink<Muon::sTgcPrepDataContainer> ElementLinkToIDC_STGC_Container;

class STGC_ClusterOnTrackCnv_p1;

namespace MuonGM
{
  class sTgcReadoutElement;
}

namespace Trk 
{
  class ITrkEventCnvTool;
}

namespace Muon
{

  /** @brief Class to represent calibrated clusters formed from TGC strips*/
  class sTgcClusterOnTrack :  public MuonClusterOnTrack
  {

  public:
    friend class  Trk::ITrkEventCnvTool;
    friend class ::STGC_ClusterOnTrackCnv_p1;

    sTgcClusterOnTrack();
    sTgcClusterOnTrack(const sTgcClusterOnTrack &);
    sTgcClusterOnTrack &operator=(const sTgcClusterOnTrack &);

    /** Constructor with parameters :
	The base class owns local position, error matrix.
	Everything else has ownership elsewhere.
	@param[in] RIO <b>Required</b> (i.e. must not be NULL). Ownership is not taken.
	@param[in] locpos <b>Required</b> (i.e. must not be NULL). Ownership is taken.
	@param[in] locerr <b>Required</b> (i.e. must not be NULL). Ownership is taken.
	@param[in] positionAlongStrip  <b>Required</b> Used to calculate global position.  
    */
    sTgcClusterOnTrack(
		       const sTgcPrepData* RIO,
		       const Trk::LocalParameters& locpos,
		       const Amg::MatrixX& locerr, 
		       double positionAlongStrip
		       );

    // Alternate constructor that doesn't dereference the RIO link.
    sTgcClusterOnTrack(
                     const ElementLinkToIDC_STGC_Container& RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     const Identifier& id,
                     const MuonGM::sTgcReadoutElement* detEl,
                     double positionAlongStrip);

    /** @brief Destructor*/
    virtual ~sTgcClusterOnTrack();

    /** @brief Clone this ROT */
    virtual sTgcClusterOnTrack* clone() const ;

    /** @brief Returns the sTgcPrepData - is a TRT_DriftCircle in this scope*/
    virtual const sTgcPrepData* prepRawData() const;
    virtual const ElementLinkToIDC_STGC_Container& prepRawDataLink() const;

    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::sTgcReadoutElement* detectorElement() const;

    /** @brief Returns the surface on which this measurement was taken. 
	(i.e. a surface of a detector element) */
    virtual const Trk::Surface& associatedSurface() const;

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;
    
  private:
    /**@brief Sets the DetElement and Trk::PrepRawData pointers after reading from disk.
       @warning Only intended for use by persistency convertors.
       @todo Throw exception if TrkDetElementBase isn't correct concrete type*/
    virtual void setValues(const Trk::TrkDetElementBase*, const Trk::PrepRawData*);

    /** PrepRawData object assoicated with this measurement*/
    ElementLinkToIDC_STGC_Container              m_rio;

    /** The detector element, assoicated with this measurement*/
    const MuonGM::sTgcReadoutElement*            m_detEl;

  };

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////


  inline sTgcClusterOnTrack* sTgcClusterOnTrack::clone() const
  {
    return new sTgcClusterOnTrack(*this);
  }

  inline const sTgcPrepData* sTgcClusterOnTrack::prepRawData() const
  {
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    else return 0;
  }

  inline const ElementLinkToIDC_STGC_Container& sTgcClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }

  inline const MuonGM::sTgcReadoutElement*  sTgcClusterOnTrack::detectorElement() const
  {
    return m_detEl;
  }

  inline const Trk::Surface& sTgcClusterOnTrack::associatedSurface() const
  {
    assert(0!=detectorElement());
    return detectorElement()->surface(identify());
  }

  inline void sTgcClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
					    const  Trk::PrepRawData* /*rio*/)
  {
    // m_rio = dynamic_cast<const sTgcPrepData*>(rio);
    //assert(0!=m_rio);
    m_detEl = dynamic_cast<const MuonGM::sTgcReadoutElement*>(detEl);
    assert(0!=m_detEl);
  }

}

#endif

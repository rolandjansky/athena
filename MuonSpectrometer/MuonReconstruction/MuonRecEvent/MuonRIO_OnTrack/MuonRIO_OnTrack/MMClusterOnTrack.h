/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRIOONTRACK_MMCLUSTERONTRACK_H
#define MUONRIOONTRACK_MMCLUSTERONTRACK_H

// Base classes
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonPrepRawData/MMPrepData.h"
// needed classes
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

typedef ElementLink<Muon::MMPrepDataContainer> ElementLinkToIDC_MM_Container;

namespace MuonGM
{
  class MMReadoutElement;
}

namespace Trk 
{
  class ITrkEventCnvTool;
}

namespace Muon
{

  /** @brief Class to represent calibrated clusters formed from TGC strips*/
  class MMClusterOnTrack :  public MuonClusterOnTrack
  {

  public:
    friend class  Trk::ITrkEventCnvTool;

    MMClusterOnTrack();
    MMClusterOnTrack(const MMClusterOnTrack &);
    MMClusterOnTrack &operator=(const MMClusterOnTrack &);

    /** Constructor with parameters :
	The base class owns local position, error matrix.
	Everything else has ownership elsewhere.
	@param[in] RIO <b>Required</b> (i.e. must not be NULL). Ownership is not taken.
	@param[in] locpos <b>Required</b> (i.e. must not be NULL). Ownership is taken.
	@param[in] locerr <b>Required</b> (i.e. must not be NULL). Ownership is taken.
	@param[in] positionAlongStrip  <b>Required</b> Used to calculate global position.  
    */
    MMClusterOnTrack(
		       const MMPrepData* RIO,
		       const Trk::LocalParameters& locpos,
		       const Amg::MatrixX& locerr, 
		       double positionAlongStrip
		       );

    // Alternate constructor that doesn't dereference the RIO link.
    MMClusterOnTrack(
                     const ElementLinkToIDC_MM_Container& RIO,
                     const Trk::LocalParameters& locpos,
                     const Amg::MatrixX& locerr,
                     const Identifier& id,
                     const MuonGM::MMReadoutElement* detEl,
                     double positionAlongStrip);

    /** @brief Destructor*/
    virtual ~MMClusterOnTrack();

    /** @brief Clone this ROT */
    virtual MMClusterOnTrack* clone() const ;

    /** @brief Returns the MMPrepData - is a TRT_DriftCircle in this scope*/
    virtual const MMPrepData* prepRawData() const;
    inline const ElementLinkToIDC_MM_Container& prepRawDataLink() const;

    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::MMReadoutElement* detectorElement() const;

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
    ElementLinkToIDC_MM_Container              m_rio;

    /** The detector element, assoicated with this measurement*/
    const MuonGM::MMReadoutElement*            m_detEl;

  };

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////


  inline MMClusterOnTrack* MMClusterOnTrack::clone() const
  {
    return new MMClusterOnTrack(*this);
  }

  inline const MMPrepData* MMClusterOnTrack::prepRawData() const
  {
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    else return 0;
  }

  inline const ElementLinkToIDC_MM_Container& MMClusterOnTrack::prepRawDataLink() const
  {
    return m_rio;
  }

  inline const MuonGM::MMReadoutElement*  MMClusterOnTrack::detectorElement() const
  {
    return m_detEl;
  }

  inline const Trk::Surface& MMClusterOnTrack::associatedSurface() const
  {
    assert(0!=detectorElement());
    return detectorElement()->surface(identify());
  }

  inline void MMClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
					    const  Trk::PrepRawData* /*rio*/)
  {
    // m_rio = dynamic_cast<const MMPrepData*>(rio);
    //assert(0!=m_rio);
    m_detEl = dynamic_cast<const MuonGM::MMReadoutElement*>(detEl);
    assert(0!=m_detEl);
  }

}

#endif

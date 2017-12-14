/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// CscClusterOnTrack.h
//   Header file for class CscClusterOnTrackOnTrack
//
// (c) ATLAS Detector software
//
// /** Class to implement Cluster On Track for Si */
//
// Version 1.0 06/07/2004 Ketevi A. Assamagan
// adapted from Veronique Boisvert
//

#ifndef MUONRIOONTRACK_CSCCLUSTERONTRACK_H
#define MUONRIOONTRACK_CSCCLUSTERONTRACK_H

// Base classes
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscTimeStatus.h"
// needed classes

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

typedef ElementLink<Muon::CscPrepDataContainer> ElementLinkToIDC_CSC_Container;

namespace MuonGM
{
class CscReadoutElement;
}

namespace Trk 
{
    class ITrkEventCnvTool;
}

namespace Muon
{

/** @brief Class to represent the calibrated clusters created from CSC strips*/
class CscClusterOnTrack :  public MuonClusterOnTrack
{

public:
    friend class  Trk::ITrkEventCnvTool;

    CscClusterOnTrack();
    CscClusterOnTrack(const CscClusterOnTrack &);
    CscClusterOnTrack &operator=(const CscClusterOnTrack &);


    /** Constructor with parameters :
    The base class owns local position, error matrix.
    Everything else has ownership elsewhere.
    @param[in] RIO <b>Required</b> (i.e. must not be NULL). Ownership is not taken.
    @param[in] locpos <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] locerr <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] positionAlongStrip  <b>Required</b> Used to calculate global position.  
    @param[in] status  <b>Required</b> Measure of 'quality' of cluster - see CscClusterStatus for more info.  
    @param[in] time  <b>Required</b>   
    */
    CscClusterOnTrack(
        const CscPrepData* RIO,
        const Trk::LocalParameters& locpos,
        const Amg::MatrixX& locerr,
        double positionAlongStrip,
        CscClusterStatus status,
        CscTimeStatus timeStatus=Muon::CscTimeStatusUndefined,
        float time=9999.0
    );

    // Alternate constructor that doesn't dereference the RIO link.
    CscClusterOnTrack(
        const ElementLinkToIDC_CSC_Container& RIO,
        const Trk::LocalParameters& locpos,
        const Amg::MatrixX& locerr,
        const Identifier& id,
        const MuonGM::CscReadoutElement*  detEL,
        double positionAlongStrip,
        CscClusterStatus status,
        CscTimeStatus timeStatus=Muon::CscTimeStatusUndefined,
        float time=9999.0
    );

    /** Destructor*/
    virtual ~CscClusterOnTrack();

    /** @brief Clone this ROT */
    virtual CscClusterOnTrack* clone() const ;

    /** @brief Returns the CscPrepData - is a CscPrepData in this scope*/
    virtual const CscPrepData* prepRawData() const;
    const ElementLinkToIDC_CSC_Container& prepRawDataLink() const;

    /** @brief Returns the detector element, associated with the PRD of this class*/
    virtual const MuonGM::CscReadoutElement* detectorElement() const;

    /** @brief Returns the surface on which this measurement was taken. 
    (i.e. a surface of a detector element) */
    virtual const Trk::Surface& associatedSurface() const;

    /** @brief Returns Csc position measurement  status flag */
    CscClusterStatus status() const;

    /** @brief Returns Csc time measurement status flag */
    CscTimeStatus timeStatus() const;
  
    /** Return the time(ns)*/
    float time() const;
    
    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;

private:
    /** this method is only used by the custom convertors. 
    @warning At the moment it only actually sets the CscPrepData, since that's all that's stored
    @todo Throw exception if TrkDetElementBase isn't correct concrete type*/
    virtual void setValues(const Trk::TrkDetElementBase*, const Trk::PrepRawData*);

    /** PrepRawData object assoicated with this measurement*/
    ElementLinkToIDC_CSC_Container      m_rio;

    /** The detector element, associated with this measurement*/
    const MuonGM::CscReadoutElement*    m_detEl;

    /** cluster position measurement status */
    CscClusterStatus                    m_status;

    /** cluster time measurement status */
    CscTimeStatus                    m_timeStatus;
  
    float                               m_time;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


inline CscClusterOnTrack* CscClusterOnTrack::clone() const
{
    return new CscClusterOnTrack(*this);
}

inline const CscPrepData* CscClusterOnTrack::prepRawData() const
{
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    //std::cout<<"CscClusterOnTrack::WARNING invalid PRD"<<std::endl;
    return 0;
}

inline const ElementLinkToIDC_CSC_Container&
CscClusterOnTrack::prepRawDataLink() const
{
  return m_rio;
}

inline const MuonGM::CscReadoutElement*  CscClusterOnTrack::detectorElement() const
{
    return m_detEl;
}

inline const Trk::Surface& CscClusterOnTrack::associatedSurface() const
{
    // FIXME! This will crash if detectorElement() returns 0! EJWM
    return detectorElement()->surface(identify());
}

inline CscClusterStatus CscClusterOnTrack::status() const
{
  return m_status;
}

inline CscTimeStatus CscClusterOnTrack::timeStatus() const
{
  return m_timeStatus;
}

inline float CscClusterOnTrack::time() const
{
    return m_time;
}

inline void CscClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
                     const  Trk::PrepRawData* /*rio*/)
  {
    // TODO should check this works EJWM
    //m_rio = dynamic_cast<const CscPrepData*>(rio);
    //assert(0!=m_rio);
    m_detEl = dynamic_cast<const MuonGM::CscReadoutElement*>(detEl);
    assert(0!=m_detEl);
  }
}

#endif // MUONRIOONTRACK_MUONCLUSTERONTRACK_H

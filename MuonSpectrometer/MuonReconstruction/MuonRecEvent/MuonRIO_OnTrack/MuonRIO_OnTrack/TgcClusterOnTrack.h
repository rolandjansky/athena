/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TgcClusterOnTrack.h
//   Header file for class TgcClusterOnTrackOnTrack
//
// (c) ATLAS Detector software
//
// /** Class to implement Cluster On Track for Si */
//
// Version 1.0 06/07/2004 Ketevi A. Assamagan
// adapted from Veronique Boisvert
//

#ifndef MUONRIOONTRACK_TGCCLUSTERONTRACK_H
#define MUONRIOONTRACK_TGCCLUSTERONTRACK_H

// Base classes
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonPrepRawData/TgcPrepData.h"
// needed classes
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

typedef ElementLink<Muon::TgcPrepDataContainer> ElementLinkToIDC_TGC_Container;

namespace MuonGM
{
    class TgcReadoutElement;
}

namespace Trk 
{
    class ITrkEventCnvTool;
}

namespace Muon
{

/** @brief Class to represent calibrated clusters formed from TGC strips*/
class TgcClusterOnTrack :  public MuonClusterOnTrack
{

public:
    friend class  Trk::ITrkEventCnvTool;

    TgcClusterOnTrack();
    TgcClusterOnTrack(const TgcClusterOnTrack &);
    TgcClusterOnTrack &operator=(const TgcClusterOnTrack &);


    /** Constructor with parameters :
    The base class owns local position, error matrix.
    Everything else has ownership elsewhere.
    @param[in] RIO <b>Required</b> (i.e. must not be NULL). Ownership is not taken.
    @param[in] locpos <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] locerr <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] positionAlongStrip  <b>Required</b> Used to calculate global position.  
    */
    TgcClusterOnTrack(
        const TgcPrepData* RIO,
        const Trk::LocalParameters& locpos,
        const Amg::MatrixX& locerr, 
        double positionAlongStrip
    );

    // Alternate constructor that doesn't dereference the RIO link.
    TgcClusterOnTrack(
                      const ElementLinkToIDC_TGC_Container& RIO,
                      const Trk::LocalParameters& locpos,
                      const Amg::MatrixX& locerr,
                      const Identifier& id,
                      const MuonGM::TgcReadoutElement* detEl,
                      double positionAlongStrip
                      );

    /** @brief Destructor*/
    virtual ~TgcClusterOnTrack();

    /** @brief Clone this ROT */
    virtual TgcClusterOnTrack* clone() const ;

    /** @brief Returns the TgcPrepData - is a TRT_DriftCircle in this scope*/
    virtual const TgcPrepData* prepRawData() const;
    const ElementLinkToIDC_TGC_Container& prepRawDataLink() const;

    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::TgcReadoutElement* detectorElement() const;

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
    ElementLinkToIDC_TGC_Container              m_rio;

    /** The detector element, assoicated with this measurement*/
    const MuonGM::TgcReadoutElement*            m_detEl;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


inline TgcClusterOnTrack* TgcClusterOnTrack::clone() const
{
    return new TgcClusterOnTrack(*this);
}

inline const TgcPrepData* TgcClusterOnTrack::prepRawData() const
{
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    else return 0;
}

inline const ElementLinkToIDC_TGC_Container& TgcClusterOnTrack::prepRawDataLink() const
{
  return m_rio;
}

inline const MuonGM::TgcReadoutElement*  TgcClusterOnTrack::detectorElement() const
{
  return m_detEl;
}

inline const Trk::Surface& TgcClusterOnTrack::associatedSurface() const
{
    assert(0!=detectorElement());
    return detectorElement()->surface(identify());
}

inline void TgcClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
                     const  Trk::PrepRawData* /*rio*/)
{
    // m_rio = dynamic_cast<const TgcPrepData*>(rio);
    //assert(0!=m_rio);
    m_detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(detEl);
    assert(0!=m_detEl);
}

}

#endif // MUONRIOONTRACK_MUONCLUSTERONTRACK_H

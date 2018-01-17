/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRIOONTRACK_RPCCLUSTERONTRACK_H
#define MUONRIOONTRACK_RPCCLUSTERONTRACK_H

// Base classes
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonPrepRawData/RpcPrepData.h"
// needed classes
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "AthLinks/ElementLink.h"

typedef ElementLink<Muon::RpcPrepDataContainer> ElementLinkToIDC_RPC_Container;

class RpcClusterOnTrackCnv_p1;


namespace MuonGM
{
    class RpcReadoutElement;
}

namespace Trk 
{
    class ITrkEventCnvTool;
}

namespace Muon
{

/** @brief Class to represent calibrated clusters formed from RPC strips*/
class RpcClusterOnTrack :  public MuonClusterOnTrack
{

public:

    friend class  Trk::ITrkEventCnvTool;
    friend class  ::RpcClusterOnTrackCnv_p1;

    RpcClusterOnTrack();
    RpcClusterOnTrack(const RpcClusterOnTrack &);
    RpcClusterOnTrack &operator=(const RpcClusterOnTrack &);


    /** Constructor with parameters :
    The base class owns local position, error matrix.
    Everything else has ownership elsewhere.
    @param[in] RIO <b>Required</b> (i.e. must not be NULL). Ownership is not taken.
    @param[in] locpos <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] locerr <b>Required</b> (i.e. must not be NULL). Ownership is taken.
    @param[in] positionAlongStrip  <b>Required</b> Used to calculate global position.  
    */
    RpcClusterOnTrack(
        const RpcPrepData* RIO,
        const Trk::LocalParameters& locpos,
        const Amg::MatrixX& locerr,
        double positionAlongStrip,
        float time=0.0
    );

    // Alternate constructor that doesn't dereference the RIO link.
    RpcClusterOnTrack(
                      const ElementLinkToIDC_RPC_Container& RIO,
                      const Trk::LocalParameters& locpos,
                      const Amg::MatrixX& locerr,
                      const Identifier& id,
                      const MuonGM::RpcReadoutElement* detEl,
                      double positionAlongStrip,
                      float time);

    /** @brief Destructor*/
    virtual ~RpcClusterOnTrack();

    /** @brief Clone this ROT */
    virtual RpcClusterOnTrack* clone() const ;

    /** @brief Returns the RpcPrepData - is a TRT_DriftCircle in this scope*/
    virtual const RpcPrepData* prepRawData() const;
    const ElementLinkToIDC_RPC_Container& prepRawDataLink() const;

    /** @brief Returns the detector element, assoicated with the PRD of this class*/
    virtual const MuonGM::RpcReadoutElement* detectorElement() const;

    /** @brief Returns the surface on which this measurement was taken. 
    (i.e. a surface of a detector element) */
    virtual const Trk::Surface& associatedSurface() const;
    
    /** Return the time (ns)*/
  float time() const;
    
    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;
    

private:
    /**@brief Sets the DetElement and Trk::PrepRawData pointers after reading from disk.
    @warning Only intended for use by persistency convertors    
    @todo Throw exception if TrkDetElementBase isn't correct concrete type*/
    virtual void setValues(const Trk::TrkDetElementBase*, const Trk::PrepRawData*);

   /** PrepRawData object assoicated with this measurement*/
    ElementLinkToIDC_RPC_Container              m_rio;

    /** The detector element, assoicated with this measurement*/
    const MuonGM::RpcReadoutElement*            m_detEl;
    
    /** The time in ns*/
    float m_time;
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////


inline RpcClusterOnTrack* RpcClusterOnTrack::clone() const
{
    return new RpcClusterOnTrack(*this);
}

inline const RpcPrepData* RpcClusterOnTrack::prepRawData() const
{
    if (m_rio.isValid()) return m_rio.cachedElement(); 
    else return 0;
}

inline const ElementLinkToIDC_RPC_Container& RpcClusterOnTrack::prepRawDataLink() const
{
  return m_rio;
}

inline const MuonGM::RpcReadoutElement*  RpcClusterOnTrack::detectorElement() const
{
  return m_detEl;
}

inline const Trk::Surface& RpcClusterOnTrack::associatedSurface() const
{
    // FIXME! This will crash if detectorElement() returns 0! EJWM
    return detectorElement()->surface(identify());
}

 inline void RpcClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl,
        const  Trk::PrepRawData* /*rio*/)
{
    // TODO should check this works EJWM
    //m_rio = dynamic_cast<const RpcPrepData*>(rio);
    //assert(0!=m_rio);
    m_detEl = dynamic_cast<const MuonGM::RpcReadoutElement*>(detEl);
    assert(0!=m_detEl);
}

inline float RpcClusterOnTrack::time() const
{
    return m_time;
}

}

#endif // MUONRIOONTRACK_MUONCLUSTERONTRACK_H

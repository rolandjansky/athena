/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpcPrepPdata.h
//   Header file for class RpcPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Muon - RPC
///////////////////////////////////////////////////////////////////
// Version 1.0 03/30/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_RPCPREPDATA_H
#define MUONPREPRAWDATA_RPCPREPDATA_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

class RpcPrepDataContainerCnv;

namespace Muon
{

    class RpcRdoToPrepDataToolCore;
    class RpcPrepDataContainerCnv_p1;
    class RpcPrepDataContainerCnv_p2;

/** @brief Class to represent RPC measurements. */
class RpcPrepData :   public MuonCluster
{

    friend class Muon::RpcRdoToPrepDataToolCore;

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    friend class ::RpcPrepDataContainerCnv;
    friend class Muon::RpcPrepDataContainerCnv_p1;
    friend class Muon::RpcPrepDataContainerCnv_p2;

    RpcPrepData();
    RpcPrepData(const RpcPrepData &);
    RpcPrepData(RpcPrepData &&) noexcept = default;
    RpcPrepData &operator=(const RpcPrepData &);
    RpcPrepData &operator=(RpcPrepData &&) noexcept = default;


    /** @brief Constructor.
    @param RDOId The identifier of the central strip of the cluster
    @param idDE  The IdenifierHash of the collection used to store this object (i.e. of Muon::CscPrepDataCollection)
    @param locpos The local coords of the measurement (this object will now own the LocalPostion)
    @param rdoList Vector of all the Identifiers of the strips used in this cluster
    @param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
    @param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
                (the pointer is assumed to belong to GeoModel and will not be deleted)
    @param time   The time measured by the RPC
    @param triggerInfo The trigger info flag - see m_triggerInfo for more definition.
    @param ambiguityFlag The ambiguity flag - see m_ambiguityFlag for more definition.
    */
    RpcPrepData( const Identifier& RDOId,
                 const IdentifierHash &idDE,
                 const Amg::Vector2D& locpos,
                 const std::vector<Identifier>& rdoList,
                 const Amg::MatrixX* locErrMat,
                 const MuonGM::RpcReadoutElement* detEl,
                 const float time,
                 const int triggerInfo,
                 const int ambiguityFlag);
    /** @brief Extra Constructor without TriggerInfo for a slimmed version of RpcPrepData to come. */
    RpcPrepData( const Identifier& RDOId,
                 const IdentifierHash &idDE,
                 const Amg::Vector2D& locpos,
                 const std::vector<Identifier>& rdoList,
                 const Amg::MatrixX* locErrMat,
                 const MuonGM::RpcReadoutElement* detEl,
                 const float time,
                 const int ambiguityFlag);


    /** @brief Destructor: */
    virtual ~RpcPrepData();

    /** @brief Returns the time. */
    float time() const;

    /** @brief Returns the global position*/
    virtual const Amg::Vector3D& globalPosition() const override;

    /** @brief Returns the detector element corresponding to this PRD.
    The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    virtual const MuonGM::RpcReadoutElement* detectorElement() const override final;

    /** Interface method checking the type*/
    virtual bool type(Trk::PrepRawDataType::Type type) const override final
    {
      return type == Trk::PrepRawDataType::RpcPrepData;
    }

    /** @brief Returns the trigger coincidence - usually false, unless ijk>5 or highpt&&ijk==0*/
    int triggerInfo() const;

    /** @brief Returns the number of ambiguities associated with this RpcPrepData.
        - 0 if the ambiguites have not been removed by choice;
        - 1 if the ambiguities are fully solved
        - i+1 if "i" other MuonPrepRawData are produced along with the current one from a single RDO hit*/
    int ambiguityFlag() const;

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const override;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const override;

private:

    /** Cached pointer to the detector element - should never be zero.*/
    const MuonGM::RpcReadoutElement* m_detEl;

    /** Float since PRD produced from RDO, and RDO should contain the time calculated from the bcid and the RPC clock
    ticks. (as bcid*25+ticks*3.125).*/
    float m_time;

    /** usually false, unless ijk>5 or highpt&&ijk==0 */

    int m_triggerInfo;
    /** Trigger ambiguities.
        - 0 if the ambiguites have not been removed by choice;
        - 1 if the ambiguities are fully solved
        - i+1 if "i" other preprawdata are produced along with the current one from a single RDO hit*/
    int m_ambiguityFlag;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline float RpcPrepData::time() const
{
    return m_time;
}

inline const MuonGM::RpcReadoutElement* RpcPrepData::detectorElement() const
{
    return m_detEl;
}

inline int RpcPrepData::triggerInfo() const
{
    return m_triggerInfo;
}

inline int RpcPrepData::ambiguityFlag() const
{
    return m_ambiguityFlag;
}
  // return globalPosition:
 inline const Amg::Vector3D& RpcPrepData::globalPosition() const
   {
      if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

      if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
      return *m_globalPosition;
    }
}

#endif // MUONPREPRAWDATA_RPCPREPDATA_H


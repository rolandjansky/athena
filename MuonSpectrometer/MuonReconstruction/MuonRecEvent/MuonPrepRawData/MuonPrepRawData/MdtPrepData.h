/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtPrepData.h
//   Header file for class MdtPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement DriftCircle for Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 03/30/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPDATA_MDTPREPDATA_H
#define MUONPREPDATA_MDTPREPDATA_H

// Base classes
#include "TrkPrepRawData/PrepRawData.h"
#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonPrepRawData/MdtDriftCircleStatus.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "CxxUtils/CachedUniquePtr.h"

class MdtPrepDataContainerCnv;

namespace Muon
{

  class MdtPrepDataContainerCnv_p1;

/** @brief Class to represent measurements from the Monitored Drift Tubes*/
class MdtPrepData :   public Trk::PrepRawData
{

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    friend class ::MdtPrepDataContainerCnv;
    friend class Muon::MdtPrepDataContainerCnv_p1;


    MdtPrepData();
    MdtPrepData(const MdtPrepData &);
    MdtPrepData(MdtPrepData &&) noexcept;
    MdtPrepData &operator=(const MdtPrepData &);
    MdtPrepData &operator=(MdtPrepData &&) noexcept;

    /** Constructor with parameters: this class owns the pointers passed (except the MuonDetectorElement)
    @param id Identifier of the tube which generated DriftCircle,
    @param collectionHash IdentifierHash of the PRD collection in which the MdtPrepData is stored
    @param driftRadius this local position object should contain the drift radius
    @param errDriftRadius the error on the driftRadius (i.e. 1d error matix)
    @param detEl pointer to the associated detectr element (will not be owned by MdtPrepData - i.e. will not be deleted)
    @param tdc TDC count
    @param adc ADC count
    @param status enum to indicate whether the measurement is masked, in time, etc (see MdtPrepDataStatus for details).
    */
    MdtPrepData(
        const Identifier &id,
        const IdentifierHash &collectionHash,
        const Amg::Vector2D& driftRadius,
        const Amg::MatrixX* errDriftRadius,
        const MuonGM::MdtReadoutElement* detEl,
        const int tdc,
        const int adc,
        const MdtDriftCircleStatus status
    );

    MdtPrepData(
        const Identifier &id,
        const IdentifierHash &collectionHash,
        const Amg::Vector2D& driftRadius,
        std::unique_ptr<const Amg::MatrixX> errDriftRadius,
        std::vector<Identifier>&& rdoList,
        const MuonGM::MdtReadoutElement* detEl,
        const int tdc,
        const int adc,
        const MdtDriftCircleStatus status
    );

    /** @brief Destructor*/
    virtual ~MdtPrepData();

    /** @brief Returns the global position of the CENTER of the drift tube (i.e. it is important to realise that this is NOT the true position of the measurement).
    The reason that this is done here, and not simply through the detector element is that here it can be cached for better performance.*/
    virtual const Amg::Vector3D& globalPosition() const;

    /** @brief Returns the detector element corresponding to this PRD.
    The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    virtual const MuonGM::MdtReadoutElement* detectorElement() const override;

    /** Interface method checking the type*/
    virtual bool type(Trk::PrepRawDataType::Type type) const override
    {
      return type == Trk::PrepRawDataType::MdtPrepData;
    }

    /** @brief Returns the TDC (typically range is 0 to 2500). */
    int tdc() const;

    /** @brief Returns the ADC (typically range is 0 to 250)*/
    int adc() const;

    /** @brief Returns the status of the measurement */
    MdtDriftCircleStatus status() const;

    /** @brief Returns the IdentifierHash corresponding to the Mdt tube which was hit. */
    virtual IdentifierHash collectionHash() const;

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const override;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const override;

private:

    /** @brief Cached pointer to detector element (not deleted in destructor, not written to disk) */
    const MuonGM::MdtReadoutElement* m_detEl;

    /** @brief TDC value - typical TDC spectra can go from 0 up to 2500.*/
    int m_tdc;

    /** @brief ADC value - typical ADC spectra can go from 0 up to 250.*/
    int m_adc;

    /** @brief enum to hold 'status' of measurement */
    MdtDriftCircleStatus m_status;

protected :

    /**@brief Global position of measurement.
    Calculated on demand and cached (not deleted in destructor, not written to disk)*/
    CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;

};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

// return globalPosition:
inline const Amg::Vector3D& MdtPrepData::globalPosition() const
{
    if (not m_globalPosition) m_globalPosition.set(std::make_unique<const Amg::Vector3D>(m_detEl->surface(identify()).center()));
    return *m_globalPosition;
}


inline const MuonGM::MdtReadoutElement* MdtPrepData::detectorElement() const
{
    return m_detEl;
}

inline int MdtPrepData::tdc() const
{
    return m_tdc;
}

inline int MdtPrepData::adc() const
{
    return m_adc;
}

inline MdtDriftCircleStatus MdtPrepData::status() const
{
    return m_status;
}

inline IdentifierHash MdtPrepData::collectionHash() const
{
    return getHashAndIndex().collHash();
}

}

#endif // MUONPREPDATA_MDTPREPDATA_H



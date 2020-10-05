/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcPrepPdata.h
//   Header file for class TgcPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Muon - TGC
///////////////////////////////////////////////////////////////////
// Version 1.0 03/30/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_TGCPREPDATA_H
#define MUONPREPRAWDATA_TGCPREPDATA_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

class TgcPrepDataContainerCnv;

namespace Muon
{
  class TgcPrepDataContainerCnv_p1;

/** @brief Class to represent TGC measurements*/
  class TgcPrepData :   public MuonCluster
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    friend class ::TgcPrepDataContainerCnv;
    friend class TgcPrepDataContainerCnv_p1;

    TgcPrepData();
    TgcPrepData(const TgcPrepData &);
    TgcPrepData(TgcPrepData &&) noexcept = default;
    TgcPrepData &operator=(const TgcPrepData &);
    TgcPrepData &operator=(TgcPrepData &&) noexcept = default;

    /** @brief Full constructor.
    @param RDOId The identifier of the central strip of the cluster
    @param idDE  The IdenifierHash of the collection used to store this object (i.e. of Muon::CscPrepDataCollection)
    @param locpos The local coords of the measurement (this object will now own the LocalPostion)
    @param rdoList Vector of all the Identifiers of the strips used in this cluster
    @param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
    @param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
                (the pointer is assumed to belong to GeoModel and will not be deleted)
    */
    TgcPrepData( const Identifier& RDOId,
                 const IdentifierHash &idDE,
                 const Amg::Vector2D& locpos,
                 const std::vector<Identifier>& rdoList,
                 const Amg::MatrixX* locErrMat,
                 const MuonGM::TgcReadoutElement* detEl,
                 const uint16_t bcBitMap=0);

    /// Destructor:
      virtual ~TgcPrepData();

      // /////////////////////////////////////////////////////////////////
      // Virtual methods
      // /////////////////////////////////////////////////////////////////

      /** @brief Returns the global position */
      virtual const Amg::Vector3D& globalPosition() const override;

      /** @brief Returns the detector element corresponding to this PRD
      The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
      virtual const MuonGM::TgcReadoutElement* detectorElement() const override final;

      /** Interface method checking the type*/
      virtual bool type(Trk::PrepRawDataType::Type type) const override final
      {
        return type == Trk::PrepRawDataType::TgcPrepData;
      }

      /** @brief Returns the bcBitMap of this PRD
      bit2 for Previous BC, bit1 for Current BC, bit0 for Next BC */
      uint16_t getBcBitMap() const;
      enum {BCBIT_UNDEFINED=0, BCBIT_NEXT=1, BCBIT_CURRENT=2, BCBIT_PREVIOUS=4};

      /** @brief set the bcBitMap for this PRD*/
      void setBcBitMap(const uint16_t);

      /** @brief Dumps information about the PRD*/
      virtual MsgStream&    dump( MsgStream&    stream) const override;

      /** @brief Dumps information about the PRD*/
      virtual std::ostream& dump( std::ostream& stream) const override;

  private:

      const MuonGM::TgcReadoutElement* m_detEl;
      uint16_t m_bcBitMap;

  };

  // /////////////////////////////////////////////////////////////////
  // Inline methods:
  // /////////////////////////////////////////////////////////////////


  inline const MuonGM::TgcReadoutElement* TgcPrepData::detectorElement() const
    {
      return m_detEl;
    }
  // return globalPosition:
  inline const Amg::Vector3D& TgcPrepData::globalPosition() const
    {
      if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

      if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
      return *m_globalPosition;
    }
}

#endif // MUONPREPRAWDATA_TGCPREPDATA_H


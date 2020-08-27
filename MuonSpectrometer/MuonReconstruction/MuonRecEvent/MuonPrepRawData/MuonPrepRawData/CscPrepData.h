/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscPrepPdata.h
//   Header file for class CscPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Muon - CSC
///////////////////////////////////////////////////////////////////
// Version 1.0 03/29/2005 Ketevi A. Assamagan
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_CSCPREPDATA_H
#define MUONPREPRAWDATA_CSCPREPDATA_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscTimeStatus.h"

#include<vector>

class CscPrepDataContainerCnv;

namespace Muon
{

  class CscPrepDataContainerCnv_p1;

    /**@brief Class representing clusters from the CSC.
    @author Edward.Moyse@cern.ch
    @author Ketevi A. Assamagan*/
class CscPrepData :   public MuonCluster
{

  // /////////////////////////////////////////////////////////////////
  // Public methods:
  // /////////////////////////////////////////////////////////////////
 public:

  friend class ::CscPrepDataContainerCnv;
  friend class Muon::CscPrepDataContainerCnv_p1;

  CscPrepData();
  CscPrepData(const CscPrepData &) = default;
  CscPrepData(CscPrepData &&) noexcept = default;
  CscPrepData &operator=(const CscPrepData &) = default;
  CscPrepData &operator=(CscPrepData &&) noexcept = default;


  /** @brief Full constructor.
      @param RDOId The identifier of the central strip of the cluster
      @param idDE  The IdenifierHash of the collection used to store this object (i.e. of Muon::CscPrepDataCollection)
      @param locpos The local coords of the measurement (this object will now own the LocalPostion)
      @param rdoList Vector of all the Identifiers of the strips used in this cluster
      @param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
      @param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
      (the pointer is assumed to belong to GeoModel and will not be deleted)
      @param charge The charge measured by the CSC
      @param time   The time measured by the CSC
      @param status The status of the position measurement. See Muon::CscClusterStatus for more information.
      @param timestatus The status of the time measurement (peak strip due to max charge). See Muon::CscTimeStatus for more information.
  */
  CscPrepData( const Identifier& RDOId,
               const IdentifierHash &idDE,
               const Amg::Vector2D& locpos,
               const std::vector<Identifier>& rdoList,
               const Amg::MatrixX* locErrMat,
               const MuonGM::CscReadoutElement* detEl,
               const int charge,
               const double time,
               const CscClusterStatus status,
               const CscTimeStatus timeStatus=Muon::CscTimeStatusUndefined
               );

  /** @brief Destructor */
  virtual ~CscPrepData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods
  ///////////////////////////////////////////////////////////////////

  /** @brief Returns the global position*/
  virtual const Amg::Vector3D& globalPosition() const override;

  /** @brief Return the detector element corresponding to this PRD.

  The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor) */
  virtual const MuonGM::CscReadoutElement* detectorElement() const override final;

  /** Interface method checking the type*/
  virtual bool type(Trk::PrepRawDataType::Type type) const override final
  {
    return type == Trk::PrepRawDataType::CscPrepData;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  /** Returns the charge. */
  int charge() const;

  /** Returns the time */
  double time() const;

  /** Returns the Csc status (position measurement) flag */
  CscClusterStatus status() const;

  /** Returns the Csc time status flag */
  CscTimeStatus timeStatus() const;

  /** Dumps information about the PRD*/
  virtual MsgStream&    dump( MsgStream&    stream) const override;

  /** Dumps information about the PRD*/
  virtual std::ostream& dump( std::ostream& stream) const override;

private:

  /** Pointer to the detector element. This is owned by this object and is therefore not deleted.*/
  const MuonGM::CscReadoutElement*  m_detEl;
  int                               m_charge;
  double                            m_time;
  CscClusterStatus                  m_status;
  CscTimeStatus                     m_timeStatus;
};

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////

  inline const MuonGM::CscReadoutElement* CscPrepData::detectorElement() const
  {
    return m_detEl;
  }

  inline int CscPrepData::charge() const
  {
    return m_charge;
  }

  inline double  CscPrepData::time() const
  {
    return m_time;
  }

  inline CscClusterStatus CscPrepData::status() const
  {
    return m_status;
  }

  inline CscTimeStatus CscPrepData::timeStatus() const
  {
    return m_timeStatus;
  }

  // return globalPosition:
  inline const Amg::Vector3D& CscPrepData::globalPosition() const
  {
    if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

    if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
    return *m_globalPosition;
  }

}

#endif // MUONPREPRAWDATA_CSCPREPDATA_H


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscStripPrepPdata.h
//   Header file for class CscPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Muon - CSC
///////////////////////////////////////////////////////////////////
// Version 1.0 03/12/2007 Niels van Eldik
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_CSCSTRIPPREPDATA_H
#define MUONPREPRAWDATA_CSCSTRIPPREPDATA_H

// Base classes
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/CachedUniquePtr.h"

#include<vector>

class CscStripPrepDataContainerCnv;

namespace Muon
{

  class CscStripPrepDataContainerCnv_p1;

  /**@brief Class representing the raw data of one CSC strip (for clusters look at Muon::CscPrepData).
     @author niels van Eldik <niels.van.eldik@cern.ch>
  */

  class CscStripPrepData :   public Trk::PrepRawData
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    friend class ::CscStripPrepDataContainerCnv;
    friend class Muon::CscStripPrepDataContainerCnv_p1;

    CscStripPrepData();
    CscStripPrepData(const CscStripPrepData &);
    CscStripPrepData &operator=(const CscStripPrepData &);


    /** Constructor
    @param RDOId The identifier of the strip
    @param collectionHash  The IdenifierHash of the collection used to store this object (i.e. of Muon::CscPrepDataCollection)
    @param locpos The local coords of the measurement (this object will now own the LocalPostion)
    @param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
    @param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
                (the pointer is assumed to belong to GeoModel and will not be deleted)
    @param sampleCharges The vector of charges measured by the strip. @todo More info.
    @param timeOfFirstSample   The time measured by the CSC @todo More info.
    @param samplingRate The sampling rate.
    */
    CscStripPrepData( const Identifier& RDOId,
                      const IdentifierHash &collectionHash,
                      const Amg::Vector2D& locpos,
                      const Amg::MatrixX* locErrMat,
                      const MuonGM::CscReadoutElement* detEl,
                      const std::vector<float>& sampleCharges,
                      float timeOfFirstSample,
                      unsigned short samplingRate
                      );

    /// Destructor:
      virtual ~CscStripPrepData();

     /** return global position reference */
     const Amg::Vector3D& globalPosition() const;

      /** return the detector element corresponding to this PRD
      The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor) */
      virtual const MuonGM::CscReadoutElement* detectorElement() const override;

      /** Interface method checking the type*/
      virtual bool type(Trk::PrepRawDataType::Type type) const override
      {
        return type == Trk::PrepRawDataType::MdtPrepData;
      }

      /** returns the IdentifierHash corresponding to the channel. */
      virtual const IdentifierHash collectionHash() const;

      /** Return the time samples. */
      const std::vector<float>& sampleCharges() const;

      /** return the time of the first sample */
      double timeOfFirstSample() const;

      /** return the sampling phase */
      bool samplingPhase() const;

      /** return the sampling time in ns: the time between samplings */
      unsigned short samplingTime() const;

      /** dump information about the PRD*/
      virtual MsgStream&    dump( MsgStream&    stream) const override;

      /** dump information about the PRD*/
      virtual std::ostream& dump( std::ostream& stream) const override;

      /** set the sampling phase
      @warning This does not conform to ATLAS Coding Conventions and will be renamed to setSamplingPhase() */
      void set_samplingPhase () ;

  private:
      /** The IdenifierHash of the collection used to store this PRD*/
      IdentifierHash m_collectionHash;
      /** The global position is calculated 'on the fly' and is not written to disk*/
      CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;
      const MuonGM::CscReadoutElement* m_detEl;
      std::vector<float> m_sampleCharges;
      float m_timeOfFirstSample;
      unsigned short m_samplingTime;
      bool m_samplingPhase;
  };

  // /////////////////////////////////////////////////////////////////
  // Inline methods:
  // /////////////////////////////////////////////////////////////////

   // return globalPosition:
  inline const Amg::Vector3D& CscStripPrepData::globalPosition() const
    {
       if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

       if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
       return *m_globalPosition;
     }

  inline const MuonGM::CscReadoutElement* CscStripPrepData::detectorElement() const
    {
      return m_detEl;
    }

  inline const IdentifierHash CscStripPrepData::collectionHash() const
    {
      return m_collectionHash;
    }

  inline const std::vector<float>& CscStripPrepData::sampleCharges() const
    {
      return m_sampleCharges;
    }

  inline double CscStripPrepData::timeOfFirstSample() const
    {
      return m_timeOfFirstSample;
    }

  inline unsigned short CscStripPrepData::samplingTime() const
    {
      return m_samplingTime;
    }

  inline  bool CscStripPrepData::samplingPhase() const
    {
      return m_samplingPhase;
    }

    inline void CscStripPrepData::set_samplingPhase()
    {
        m_samplingPhase = true;
    }
}

#endif // MUONPREPRAWDATA_CSCPREPDATA_H


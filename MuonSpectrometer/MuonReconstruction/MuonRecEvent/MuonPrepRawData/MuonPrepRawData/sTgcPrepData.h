/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPREPRAWDATA_STGCPREPDATA_H
#define MUONPREPRAWDATA_STGCPREPDATA_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

class sTgcPrepDataContainerCnv;

namespace Muon
{

  class sTgcRdoToPrepDataTool;

  /** @brief Class to represent sTgc measurements. */
  class sTgcPrepData :   public MuonCluster
  {

    friend class Muon::sTgcRdoToPrepDataTool;

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    friend class ::sTgcPrepDataContainerCnv;

    sTgcPrepData();
    sTgcPrepData(const sTgcPrepData &);
    sTgcPrepData(sTgcPrepData &&) noexcept = default;
    sTgcPrepData &operator=(const sTgcPrepData &);
    sTgcPrepData &operator=(sTgcPrepData &&) noexcept = default;


    /** @brief Constructor.
	@param RDOId The identifier of the central strip of the cluster
	@param locpos The local coords of the measurement (this object will now own the LocalPostion)
	@param rdoList Vector of all the Identifiers of the strips used in this cluster
	@param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
	@param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
	(the pointer is assumed to belong to GeoModel and will not be deleted)
	@param bcBitMap bitmap storing in which bunches the channel fired
    */
    sTgcPrepData( const Identifier& RDOId,
		  const IdentifierHash &idDE,
		  const Amg::Vector2D& locpos,
		  const std::vector<Identifier>& rdoList,
		  const Amg::MatrixX* locErrMat,
		  const MuonGM::sTgcReadoutElement* detEl,
		  const int charge,
		  const short int time,
		  const uint16_t bcBitMap,
		  const std::vector<uint16_t>& stripNumbers,
		  const std::vector<short int>& stripTimes,
		  const std::vector<int>& stripCharges );


    sTgcPrepData( const Identifier& RDOId,
		  const IdentifierHash &idDE,
		  const Amg::Vector2D& locpos,
		  const std::vector<Identifier>& rdoList,
		  const Amg::MatrixX* locErrMat,
		  const MuonGM::sTgcReadoutElement* detEl,
		  const int charge = 0,
		  const short int time   = 0,
		  const uint16_t bcBitMap=0);



    /** @brief Destructor: */
    virtual ~sTgcPrepData();

    /** @brief Returns the global position*/
    virtual const Amg::Vector3D& globalPosition() const override;

    /** @brief Returns the detector element corresponding to this PRD.
	The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    virtual const MuonGM::sTgcReadoutElement* detectorElement() const override final;

    /** Interface method checking the type*/
    virtual bool type(Trk::PrepRawDataType::Type type) const override final
    {
      return type == Trk::PrepRawDataType::sTgcPrepData;
    }

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const override;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const override;

    /** @brief Returns the bcBitMap of this PRD
	bit2 for Previous BC, bit1 for Current BC, bit0 for Next BC */
    int charge() const;
    short int time() const;
    uint16_t getBcBitMap() const;
    enum {BCBIT_UNDEFINED=0, BCBIT_NEXT=1, BCBIT_CURRENT=2, BCBIT_PREVIOUS=4};

    /** @brief returns the list of strip numbers */
    const std::vector<uint16_t>& stripNumbers() const;

    /** @brief returns the list of times */
    const std::vector<short int>& stripTimes() const;

     /** @brief returns the list of charges */
    const std::vector<int>& stripCharges() const;


  private:

    /** Cached pointer to the detector element - should never be zero.*/
    const MuonGM::sTgcReadoutElement* m_detEl;
    int m_charge;
    short int m_time;
    uint16_t m_bcBitMap;

    /** @list of strip numbers, time and charge, of the strips associated to the PRD */
    std::vector<uint16_t> m_stripNumbers;
     /**
     strip times for the sTGCs will be available for the commissioning of the NSW.
     Later the tdo (time) will not be in the datastream due to high occupancy.
     Then this an empty vector which does not take much space
     Patrick Scholer 3.12.2019
		 */
    std::vector<short int> m_stripTimes;
    std::vector<int> m_stripCharges;

  };

  inline const MuonGM::sTgcReadoutElement* sTgcPrepData::detectorElement() const
  {
    return m_detEl;
  }
  // return globalPosition:
  inline const Amg::Vector3D& sTgcPrepData::globalPosition() const
  {
    if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

    if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
    return *m_globalPosition;
  }

  inline int sTgcPrepData::charge() const
  {
    return m_charge;
  }

  inline short int sTgcPrepData::time() const
  {
    return m_time;
  }

  inline uint16_t sTgcPrepData::getBcBitMap() const
  {
    return m_bcBitMap;
  }

  inline const std::vector<uint16_t>& sTgcPrepData::stripNumbers() const
  {
    return m_stripNumbers;
  }

  inline const std::vector<short int>& sTgcPrepData::stripTimes() const
  {
    return m_stripTimes;
  }

  inline const std::vector<int>& sTgcPrepData::stripCharges() const
  {
    return m_stripCharges;
  }

}

#endif // MUONPREPRAWDATA_STGCPREPDATA_H


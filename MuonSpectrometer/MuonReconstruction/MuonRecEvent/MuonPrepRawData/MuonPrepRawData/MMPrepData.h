/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPREPRAWDATA_MMPREPDATA_H
#define MUONPREPRAWDATA_MMPREPDATA_H

// Base classes
#include "MuonPrepRawData/MuonCluster.h"
#include "TrkSurfaces/Surface.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include <vector>

class MMPrepDataContainerCnv;
class IdentifierHash;

namespace Muon
{

  class MMRdoToPrepDataTool;

  /** @brief Class to represent MM measurements. */
  class MMPrepData :   public MuonCluster
  {

    friend class Muon::MMRdoToPrepDataTool;

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    friend class ::MMPrepDataContainerCnv;

    MMPrepData();
    MMPrepData(const MMPrepData &);
    MMPrepData(MMPrepData &&) noexcept = default;
    MMPrepData &operator=(const MMPrepData &);
    MMPrepData &operator=(MMPrepData &&) noexcept = default;


    /** @brief Constructor.
	@param RDOId The identifier of the central strip of the cluster
	@param locpos The local coords of the measurement (this object will now own the LocalPostion)
	@param rdoList Vector of all the Identifiers of the strips used in this cluster
	@param locErrMat The error of the measurement (this object will now own the ErrorMatrix)
	@param detEl The pointer to the Detector Element on which this measurement was made (must NOT be zero). Ownership is NOT taken
	(the pointer is assumed to belong to GeoModel and will not be deleted)
    */

   /** @brief full constructor including time, charge and strip vectors */
    MMPrepData( const Identifier& RDOId,
		const IdentifierHash &idDE,
		const Amg::Vector2D& locpos,
		const std::vector<Identifier>& rdoList,
		const Amg::MatrixX* locErrMat,
		const MuonGM::MMReadoutElement* detEl,
		const short int time,
		const int charge,
		const float driftDist,
		const std::vector<uint16_t>& stripNumbers,
		const std::vector<short int>& stripTimes,
		const std::vector<int>& stripCharges );

    /** @brief constructor including time and charge and drift distance */
    MMPrepData( const Identifier& RDOId,
		const IdentifierHash &idDE,
		const Amg::Vector2D& locpos,
		const std::vector<Identifier>& rdoList,
		const Amg::MatrixX* locErrMat,
		const MuonGM::MMReadoutElement* detEl,
		const short int time,
		const int charge,
		const float driftDist );

    /** @brief constructor including time and charge */
    MMPrepData( const Identifier& RDOId,
		const IdentifierHash &idDE,
		const Amg::Vector2D& locpos,
		const std::vector<Identifier>& rdoList,
		const Amg::MatrixX* locErrMat,
		const MuonGM::MMReadoutElement* detEl,
		const short int time,
		const int charge );

    MMPrepData( const Identifier& RDOId,
		const IdentifierHash &idDE,
		const Amg::Vector2D& locpos,
		const std::vector<Identifier>& rdoList,
		const Amg::MatrixX* locErrMat,
		const MuonGM::MMReadoutElement* detEl);

    /** @brief Destructor: */
    virtual ~MMPrepData();

    /** @brief set microTPC parameters */
    void setMicroTPC(float angle, float chisqProb);

    /** @brief set drift distances and uncertainties */
    void setDriftDist(const std::vector<float>& driftDist, const std::vector<Amg::MatrixX>& driftDistErrors);

    // setter functions for the EventTPConverters
    void setDriftDist(const std::vector<float>& driftDist, const std::vector<float>& stripDriftErrors_0_0, const std::vector<float>& stripDriftErrors_1_1);

    /** @brief Returns the global position*/
    virtual const Amg::Vector3D& globalPosition() const override;

    /** @brief Returns the detector element corresponding to this PRD.
	The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    virtual const MuonGM::MMReadoutElement* detectorElement() const override final;

    /** Interface method checking the type*/
    virtual bool type(Trk::PrepRawDataType::Type type) const override final
    {
      return type == Trk::PrepRawDataType::MMPrepData;
    }

    /** @brief Returns the time (in ns) */
    short int time() const;

    /** @brief Returns the AD */
    int charge() const;

    /** @brief Returns the Drift Distance */
    float driftDist() const;

    /** @brief Returns the microTPC angle */
    float angle() const;

    /** @brief Returns the microTPC chisq Prob. */
    float chisqProb() const;

    /** @brief returns the list of strip numbers */
    const std::vector<uint16_t>& stripNumbers() const;

    /** @brief returns the list of times */
    const std::vector<short int>& stripTimes() const;

    /** @brief returns the list of charges */
    const std::vector<int>& stripCharges() const;

    /** @brief returns the list of drift distances */
    const std::vector<float>& stripDriftDist() const;

    /** @brief returns the list of drift distances */
    const std::vector<Amg::MatrixX>& stripDriftErrors() const;

    // getter functions for the EventTPConverters
    const std::vector<float> stripDriftErrors_0_0() const;
    const std::vector<float> stripDriftErrors_1_1() const;

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const override;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const override;


    enum Author{
      RDOTOPRDConverter = -1,
      SimpleClusterBuilder,
      ProjectionClusterBuilder,
      ClusterTimeProjectionClusterBuilder,
      ConstraintuTPCClusterBuilder,
      uTPCClusterBuilder,
    };

    Author author() const;
    void setAuthor(Author author);

  private:

    /** @brief Cached pointer to the detector element - should never be zero.*/
    const MuonGM::MMReadoutElement* m_detEl;

    /** @brief measured time */
    /** @brief the time is calibrated, i.e. it is in units of ns, after t0 subtraction **/
    short int m_time;

    /** @brief measured charge */
    /** @brief the charge is calibrated, i.e. it is in units of electrons, after pedestal subtraction **/
    int m_charge;

    /** @brief drift distance */
    float m_driftDist;

    /** @angle and chisquare from micro-TPC fit */
    float m_angle;
    float m_chisqProb;

    /** @list of strip numbers, time and charge, of the strips associated to the PRD */
    std::vector<uint16_t> m_stripNumbers;
    std::vector<short int> m_stripTimes;
    std::vector<int> m_stripCharges;
    std::vector<float> m_stripDriftDist;
    std::vector<Amg::MatrixX>  m_stripDriftErrors;

    Author m_author;

  };

  inline const MuonGM::MMReadoutElement* MMPrepData::detectorElement() const
  {
    return m_detEl;
  }
  // return globalPosition:
  inline const Amg::Vector3D& MMPrepData::globalPosition() const
  {
    if (not m_globalPosition) m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition())));

    if (not m_globalPosition) throw Trk::PrepRawDataUndefinedVariable();
    return *m_globalPosition;
  }

  inline short int MMPrepData::time() const
  {
    return m_time;
  }

  inline int MMPrepData::charge() const
  {
    return m_charge;
  }

  inline float MMPrepData::driftDist() const
  {
    return m_driftDist;
  }

  inline float MMPrepData::angle() const
  {
    return m_angle;
  }

  inline float MMPrepData::chisqProb() const
  {
    return m_chisqProb;
  }

  inline const std::vector<uint16_t>& MMPrepData::stripNumbers() const
  {
    return m_stripNumbers;
  }

  inline const std::vector<short int>& MMPrepData::stripTimes() const
  {
    return m_stripTimes;
  }

  inline const std::vector<int>& MMPrepData::stripCharges() const
  {
    return m_stripCharges;
  }

  inline const std::vector<float>& MMPrepData::stripDriftDist() const
  {
    return m_stripDriftDist;
  }

  inline const std::vector<Amg::MatrixX>& MMPrepData::stripDriftErrors() const
  {
    return m_stripDriftErrors;
  }

  inline MMPrepData::Author MMPrepData::author() const {
    return m_author;
  }

}

#endif // MUONPREPRAWDATA_MMREPDATA_H


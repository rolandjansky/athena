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
    sTgcPrepData(sTgcPrepData &&);
    sTgcPrepData &operator=(const sTgcPrepData &);
    sTgcPrepData &operator=(sTgcPrepData &&);


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
		  const int charge = 0,
		  const int time   = 0,
		  const uint16_t bcBitMap=0);



    /** @brief Destructor: */
    virtual ~sTgcPrepData();

    /** @brief Returns the global position*/
    const Amg::Vector3D& globalPosition() const;

    /** @brief Returns the detector element corresponding to this PRD.
	The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    const MuonGM::sTgcReadoutElement* detectorElement() const;

    /** @brief Dumps information about the PRD*/
    MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    std::ostream& dump( std::ostream& stream) const;

    /** @brief Returns the bcBitMap of this PRD
	bit2 for Previous BC, bit1 for Current BC, bit0 for Next BC */
    int charge() const;
    int time() const;
    uint16_t getBcBitMap() const;
    enum {BCBIT_UNDEFINED=0, BCBIT_NEXT=1, BCBIT_CURRENT=2, BCBIT_PREVIOUS=4};

  private:

    /** Cached pointer to the detector element - should never be zero.*/
    const MuonGM::sTgcReadoutElement* m_detEl;
    int m_charge;
    int m_time;
    uint16_t m_bcBitMap;

  };

  inline const MuonGM::sTgcReadoutElement* sTgcPrepData::detectorElement() const
  {
    return m_detEl;
  }
  // return globalPosition:
  inline const Amg::Vector3D& sTgcPrepData::globalPosition() const
  {
    if (m_globalPosition==0) m_globalPosition = m_detEl->surface(identify()).Trk::Surface::localToGlobal(localPosition());

    if (m_globalPosition==0) throw Trk::PrepRawDataUndefinedVariable();
    return *m_globalPosition;
  }

  inline int sTgcPrepData::charge() const
  {
    return m_charge;
  }

  inline int sTgcPrepData::time() const
  {
    return m_time;
  }

  inline uint16_t sTgcPrepData::getBcBitMap() const
  {
    return m_bcBitMap;
  }

}

#endif // MUONPREPRAWDATA_RPCPREPDATA_H


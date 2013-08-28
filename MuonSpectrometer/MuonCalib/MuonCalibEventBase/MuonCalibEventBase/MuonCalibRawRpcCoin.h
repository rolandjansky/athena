/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 21st june 2006                                  //
//*********************************************************//

#ifndef MuonCalibRawRpcCoin_h
#define MuonCalibRawRpcCoin_h

#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"

namespace MuonCalib {

  /**
     @class MuonCalibRawRpcCoin
     Calib-EDM version of RpcCoinData class
   */
  class MuonCalibRawRpcCoin : public MuonCalibRawRpcHit {
    public:
    MuonCalibRawRpcCoin(); //!< default constructor

    MuonCalibRawRpcCoin( const MuonCalibRawRpcHit& rpcHit, 
			    unsigned short ijk,
			    unsigned short threshold,
			    unsigned short overlap,
			    unsigned short parentCmId,
			    unsigned short parentPadId,
			    unsigned short parentSectorId,
			    bool lowPtCm ); //!< constructor setting all attributes

    ~MuonCalibRawRpcCoin(); //!< destructor
    
    std::ostream& dump( std::ostream& stream) const; //!< dump to be used for operator<<() to dump the MuonCalibRawRpcCoin

    unsigned short ijk() const { return m_ijk; }
    unsigned short threshold() const { return m_threshold; }
    unsigned short overlap() const { return m_overlap; }
    unsigned short parentCmId() const { return m_parentCmId; }
    unsigned short parentPadId() const { return m_parentPadId; }
    unsigned short parentSectorId() const { return m_parentSectorId; }
    bool           lowPtCm() const { return m_lowPtCm; }

  
    private:
    
    unsigned short m_ijk;
    unsigned short m_threshold;
    unsigned short m_overlap;
    unsigned short m_parentCmId;
    unsigned short m_parentPadId;
    unsigned short m_parentSectorId;
    bool m_lowPtCm;

  };
  
} // namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibRawRpcCoin& rawHit);

#endif // MuonCalibRawRpcCoin_h

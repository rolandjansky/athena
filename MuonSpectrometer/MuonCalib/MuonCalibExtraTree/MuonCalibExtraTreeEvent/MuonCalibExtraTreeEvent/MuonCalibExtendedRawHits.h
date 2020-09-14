/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTENDEDRAWHITS_H
#define MUONCALIBEXTENDEDRAWHITS_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonCalibLayerMapping.h"
#include "CxxUtils/checker_macros.h"

namespace MuonCalib {
  
  class MuonCalibRawMdtHit;
  class MuonCalibRawRpcHit;
  class MuonCalibRawTgcHit;
  class MuonCalibRawCscHit;
  class MuonCalibExtendedSegment;
  class MuonCalibPattern;
  class MuonCalibExtendedTrack;
  class MuonCalibRawHitCollection;

  /**
     @class MuonCalibExtendedRawHits

     Access to all raw hits with layer/station granularity.
     Also information on whether the hits are associated with segments or tracks

     @author niels.van.eldik@cern.ch
  */
  class ATLAS_NOT_THREAD_SAFE MuonCalibExtendedRawHits {
  public:
    struct ExtendedChannelInfo {
      std::vector<const MuonCalibRawMdtHit*>       hits;
      std::vector<const MuonCalibExtendedSegment*> associatedSegments;
      std::vector<const MuonCalibExtendedTrack*>   associatedTracks;
    };

    typedef std::map<MuonFixedId,ExtendedChannelInfo>                    MdtHitInfoMap;
    typedef MdtHitInfoMap::iterator                                      MdtHitInfoIt;
    typedef MdtHitInfoMap::const_iterator                                MdtHitInfoCit;
    
    struct MdtRegionInfo {
      MdtRegionInfo() : nhitsAboveAdcThreshold(0),nhitsOnSegment(0),nhitsOnTrack(0) {}
      unsigned int  nhitsAboveAdcThreshold;
      unsigned int  nhitsOnSegment;
      unsigned int  nhitsOnTrack;
      MdtHitInfoMap hits;
    };
    
    typedef std::map<MuonFixedId,MdtRegionInfo>                          MdtHitChamberIdMap;
    typedef std::map<MuonFixedIdManipulator::StationIndex,MdtRegionInfo> MdtHitStationIndexMap;

    struct ExtendedInfo {
      std::vector<const MuonCalibExtendedSegment*> associatedSegments;
      std::vector<const MuonCalibExtendedTrack*>   associatedTracks;
    };
    
    typedef std::pair<const MuonCalibRawRpcHit*,ExtendedInfo>            RpcHitInfo;
    typedef std::vector<RpcHitInfo>                                      RpcHitInfoVec;
    typedef RpcHitInfoVec::iterator                                      RpcHitInfoIt;
    typedef RpcHitInfoVec::const_iterator                                RpcHitInfoCit;
    typedef std::map<MuonFixedId,RpcHitInfo>                             RpcHitChamberIdMap;
    typedef std::map<MuonFixedIdManipulator::StationIndex,RpcHitInfoVec> RpcHitStationIndexMap;

    typedef std::pair<const MuonCalibRawTgcHit*,ExtendedInfo>            TgcHitInfo;
    typedef std::vector<TgcHitInfo>                                      TgcHitInfoVec;
    typedef TgcHitInfoVec::iterator                                      TgcHitInfoIt;
    typedef TgcHitInfoVec::const_iterator                                TgcHitInfoCit;
    typedef std::map<MuonFixedId,TgcHitInfo>                             TgcHitChamberIdMap;
    typedef std::map<MuonFixedIdManipulator::StationIndex,TgcHitInfoVec> TgcHitStationIndexMap;

    typedef std::pair<const MuonCalibRawCscHit*,ExtendedInfo>            CscHitInfo;
    typedef std::vector<CscHitInfo>                                      CscHitInfoVec;
    typedef CscHitInfoVec::iterator                                      CscHitInfoIt;
    typedef CscHitInfoVec::const_iterator                                CscHitInfoCit;
    typedef std::map<MuonFixedId,CscHitInfo>                             CscHitChamberIdMap;
    typedef std::map<MuonFixedIdManipulator::StationIndex,CscHitInfoVec> CscHitStationIndexMap;



  public:
    /**  Default constructor . */
    MuonCalibExtendedRawHits();

    MuonCalibExtendedRawHits( const MuonCalibRawHitCollection& rawHits,
			      const std::vector<const MuonCalibExtendedSegment*>& segments,
			      const std::vector<const MuonCalibExtendedTrack*>& tracks );

    std::string dumpMdtHits() const ;

    /** access to the mdt hits per chamber and station */
    const MdtHitChamberIdMap& mdtHitChamberIdMap() const { return m_mdtChamberIdMap; }
    const MdtHitStationIndexMap&  mdtChamberIndexMap() const { return m_mdtChamberIndexMap; }

    /** access to the rpc hits per chamber and station */
    const RpcHitChamberIdMap& rpcHitChamberIdMap() const { return m_rpcChamberIdMap; }
    const RpcHitStationIndexMap&  rpcChamberIndexMap() const { return m_rpcChamberIndexMap; }

    /** access to the tgc hits per chamber and station */
    const TgcHitChamberIdMap& tgcHitChamberIdMap() const { return m_tgcChamberIdMap; }
    const TgcHitStationIndexMap&  tgcChamberIndexMap() const { return m_tgcChamberIndexMap; }

    /** access to the csc hits per chamber and station */
    const CscHitChamberIdMap& cscHitChamberIdMap() const { return m_cscChamberIdMap; }
    const CscHitStationIndexMap&  cscChamberIndexMap() const { return m_cscChamberIndexMap; }


  private:
    double m_adcThreshold;
    
    MdtHitChamberIdMap    m_mdtChamberIdMap;
    MdtHitStationIndexMap m_mdtChamberIndexMap;

    RpcHitChamberIdMap    m_rpcChamberIdMap;
    RpcHitStationIndexMap m_rpcChamberIndexMap;

    TgcHitChamberIdMap    m_tgcChamberIdMap;
    TgcHitStationIndexMap m_tgcChamberIndexMap;

    CscHitChamberIdMap    m_cscChamberIdMap;
    CscHitStationIndexMap m_cscChamberIndexMap;
    
    
  };

}


#endif

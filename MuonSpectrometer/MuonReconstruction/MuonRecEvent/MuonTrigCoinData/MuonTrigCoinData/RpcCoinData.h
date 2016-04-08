/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpcCoinData.h
//   Header file for class RpcCoinData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRIGCOINDATA_RPCCOINDATA_H
#define MUONTRIGCOINDATA_RPCCOINDATA_H

#include "MuonPrepRawData/RpcPrepData.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

class RpcCoinDataContainerCnv;

namespace Muon
{

class RpcCoinData : public RpcPrepData 
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    friend class ::RpcCoinDataContainerCnv;
    
    /** Public, Copy, operator=, constructor*/
    RpcCoinData();
    RpcCoinData(const RpcCoinData &);
    RpcCoinData(RpcCoinData &&);
    RpcCoinData &operator=(const RpcCoinData &);
    RpcCoinData &operator=(RpcCoinData &&);

    /** @brief Dumps information about the RpcCoinData*/
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const;

    /// Constructor: 	
    RpcCoinData( const Identifier& stripId,
                 const IdentifierHash &idDE,
                 const Amg::Vector2D& locpos,
                 const std::vector<Identifier>& stripList,
                 const Amg::MatrixX* locErrMat,
                 const MuonGM::RpcReadoutElement* detEl,
                 const float time,
                 const unsigned short ambiguityFlag,
                 const unsigned short ijk,
                 const unsigned short threshold,
                 const unsigned short overlap,
                 const unsigned short parent_cmId,
                 const unsigned short parent_padId,
                 const unsigned short parent_sectorId,
                 const bool lowPtCm);

      /// Destructor:
      virtual ~RpcCoinData();
      
      /** return ijk of the trigger hit (can be 6, 7 or 0) */
      inline unsigned short ijk() const;

      /** return highest threshold for the trigger pattern this hit is contributing */
      inline unsigned short threshold() const;

      /** return the overlap flag */      
      inline unsigned short overlap() const;

      /** parent ID (online-style) of CM, PAD and Sector */      
      inline unsigned short parentCmId() const;
      inline unsigned short parentPadId() const;
      inline unsigned short parentSectorId() const;

      /** Aside or Cside */
      bool isAside() const;
      /** Low Pt, High Pt or Low Pt input to High Pt CM */
      bool isLowPtCoin() const;
      bool isHighPtCoin() const;
      bool isLowPtInputToHighPtCm() const;
      bool lowPtCm() const;

  private:

      unsigned short m_ijk;
      unsigned short m_threshold;
      unsigned short m_overlap;
      unsigned short m_parentCmId;
      unsigned short m_parentPadId;
      unsigned short m_parentSectorId;
      bool m_lowPtCm;

  };
  ////////////////////////////////////////////////
  // inline methods:
  ////////////////////////////////////////////////

  /** return ijk of the trigger hit (can be 6, 7 or 0) */
  inline unsigned short RpcCoinData::ijk() const {return m_ijk;}

  /** return highest threshold for the trigger pattern this hit is contributing */
  inline unsigned short RpcCoinData::threshold() const {return m_threshold;}
  
  /** return the overlap flag */      
  inline unsigned short RpcCoinData::overlap() const {return m_overlap;}

  /** parent ID (online-style) of CM, PAD and Sector */      
  inline unsigned short RpcCoinData::parentCmId() const {return m_parentCmId;}
  inline unsigned short RpcCoinData::parentPadId() const {return m_parentPadId;}
  inline unsigned short RpcCoinData::parentSectorId() const {return m_parentSectorId;}
  inline bool RpcCoinData::lowPtCm() const {return m_lowPtCm;}

}
  
#endif // MUONTRIGCOINDATA_RPCCOINDATA_H

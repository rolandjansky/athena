/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigiStoreAth_H
#define MuonRecDigiStoreAth_H

namespace Muon {
  class  CscPrepData;
  class  MdtPrepData;
  class  RpcPrepData;
  class  TgcPrepData;
  class  CscStripPrepData;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyCoreEvt/MuonRecDigiStore.h"

  /**
   @class MuonRecDigiStoreAth

   This class is managing the digits data. It derives from MuonRecDigiStore
   to which are added connections to PrepData.
   
   A digit is added using the AddDigit method.
   
  @author samusog@cern.ch
  
  */

class MuonRecDigiStoreAth: public MuonRecDigiStore{
public:
  MuonRecDigiStoreAth();
  virtual ~MuonRecDigiStoreAth();

public:
  ///////////////////////////////////

  /**  Reset the Store */
  void ResetMuonRecDigiStoreAth();

  /**  Add a Typed Digi */
  void AddDigit(
                MuonRecDigitMDT& ToBeAdded,
                const Muon::MdtPrepData*& pPrepData
                );

  /**  Add a Typed Digi */
  void AddDigit(
                MuonRecDigitRPC& ToBeAdded,
                const Muon::RpcPrepData*& pPrepData
                );

  /**  Add a Typed Digi */
  void AddDigit(
                MuonRecDigitTGC& ToBeAdded,
                const Muon::TgcPrepData*& pPrepData
                );

  /**  Add a Typed Digi */
  void AddDigit(
                MuonRecDigitCSC& ToBeAdded,
                const Muon::CscStripPrepData*& pPrepData
                );

  /**  Add a Typed Digi */
  void AddDigit(
                MuonRecDigitCSCClu& ToBeAdded,
                const Muon::CscPrepData*& pPrepData
                );

  /** Get  MdtPrepData */
  const Muon::MdtPrepData*      LiIdGetMdtPrepData             (int LinearDigiNber) const;

  /** Get  RpcPrepData */
  const Muon::RpcPrepData*      LiIdGetRpcPrepData             (int LinearDigiNber) const;

  /** Get  TgcPrepData */
  const Muon::TgcPrepData*      LiIdGetTgcPrepData             (int LinearDigiNber) const;

  /** Get  CscStripPrepData */
  const Muon::CscStripPrepData* LiIdGetCscStripPrepData        (int LinearDigiNber) const;

  /** Get  CscPrepData */
  const Muon::CscPrepData*      LiIdGetCscClusterPrepData      (int LinearDigiNber) const;

  /** Find Rank */
  int FindRank( const Muon::MdtPrepData*       pPrepData ) const;
  int FindRank( const Muon::RpcPrepData*       pPrepData ) const;
  int FindRank( const Muon::TgcPrepData*       pPrepData ) const;
  int FindRank( const Muon::CscStripPrepData*  pPrepData ) const;
  int FindRank( const Muon::CscPrepData*       pPrepData ) const;
  
  /** Reserve  */
  void ReserveMuonRecDigiStoreAth(
                                  const int& DoReserve,
                                  const int& SpaceForMDT   ,
                                  const int& SpaceForRPC   ,
                                  const int& SpaceForTGC   ,
                                  const int& SpaceForCSC   ,
                                  const int& SpaceForCSCClu
                                  );

private:
///////////////////////////////////
//Data

  std::vector<const Muon::MdtPrepData*>       MdtPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::RpcPrepData*>       RpcPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::TgcPrepData*>       TgcPrepDataVector         ; //!<  Storage of associated PrepData
  std::vector<const Muon::CscStripPrepData*>  CscStripPrepDataVector    ; //!<  Storage of associated PrepData
  std::vector<const Muon::CscPrepData*>       CscCluPrepDataVector      ; //!<  Storage of associated PrepData

};


#endif

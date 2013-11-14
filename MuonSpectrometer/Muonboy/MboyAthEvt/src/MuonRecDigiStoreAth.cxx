/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------//
#include "TrkPrepRawData/PrepRawData.h"

//----------------------------------------------------------------//
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include "MuonPrepRawData/CscStripPrepData.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

MuonRecDigiStoreAth::MuonRecDigiStoreAth():MuonRecDigiStore(){}

MuonRecDigiStoreAth::~MuonRecDigiStoreAth(){}

// Reset the Store
void MuonRecDigiStoreAth::ResetMuonRecDigiStoreAth(){

  ResetMuonRecDigiStore() ;

  MdtPrepDataVector.clear();
  RpcPrepDataVector.clear();
  TgcPrepDataVector.clear();
  CscStripPrepDataVector.clear();
  CscCluPrepDataVector.clear();

}

/**  Add a Typed Digi */
void MuonRecDigiStoreAth::AddDigit(
                                   MuonRecDigitMDT& ToBeAdded,
                                   const Muon::MdtPrepData*& pPrepData
                                   ){

  Add(ToBeAdded);

  MdtPrepDataVector.push_back(pPrepData);

}

void MuonRecDigiStoreAth::AddDigit(
                                   MuonRecDigitRPC& ToBeAdded,
                                   const Muon::RpcPrepData*& pPrepData
                                   ){

  Add(ToBeAdded);

  RpcPrepDataVector.push_back(pPrepData);

}

void MuonRecDigiStoreAth::AddDigit(
                                   MuonRecDigitTGC& ToBeAdded,
                                   const Muon::TgcPrepData*& pPrepData
                                   ){

  Add(ToBeAdded);

  TgcPrepDataVector.push_back(pPrepData);

}

void MuonRecDigiStoreAth::AddDigit(
                                   MuonRecDigitCSC& ToBeAdded,
                                   const Muon::CscStripPrepData*& pPrepData
                                   ){

  Add(ToBeAdded);

  CscStripPrepDataVector.push_back(pPrepData);

}


void MuonRecDigiStoreAth::AddDigit(
                                   MuonRecDigitCSCClu& ToBeAdded,
				   const Muon::CscPrepData*& pPrepData
                                   ){

  Add(ToBeAdded);
  
  CscCluPrepDataVector.push_back(pPrepData);

}

/** For a Linear Nber LinearDigiNber get a typed Pointer */
const Muon::MdtPrepData* MuonRecDigiStoreAth::LiIdGetMdtPrepData(int LinearDigiNber) const{
 if (TyIdMDT(LinearDigiNber)    >= 0) return MdtPrepDataVector[TyIdMDT(LinearDigiNber)   ] ;
 return 0;
}
const Muon::RpcPrepData* MuonRecDigiStoreAth::LiIdGetRpcPrepData(int LinearDigiNber) const{
 if (TyIdRPC(LinearDigiNber)    >= 0) return RpcPrepDataVector[TyIdRPC(LinearDigiNber)   ] ;
 return 0;
}
const Muon::TgcPrepData* MuonRecDigiStoreAth::LiIdGetTgcPrepData(int LinearDigiNber) const{
 if (TyIdTGC(LinearDigiNber)    >= 0) return TgcPrepDataVector[TyIdTGC(LinearDigiNber)   ] ;
 return 0;
}
const Muon::CscStripPrepData* MuonRecDigiStoreAth::LiIdGetCscStripPrepData(int LinearDigiNber) const{
 if (TyIdCSC(LinearDigiNber) >= 0) return CscStripPrepDataVector[TyIdCSC(LinearDigiNber)] ;
 return 0;
}
const Muon::CscPrepData* MuonRecDigiStoreAth::LiIdGetCscClusterPrepData(int LinearDigiNber) const{
 if (TyIdCSCClu(LinearDigiNber) >= 0) return CscCluPrepDataVector[TyIdCSCClu(LinearDigiNber)] ;
 return 0;
}

// Find Rank 
int MuonRecDigiStoreAth::FindRank( const Muon::MdtPrepData*       pPrepData ) const { 
 std::vector<const Muon::MdtPrepData*>::const_iterator it = std::find(MdtPrepDataVector.begin(), MdtPrepDataVector.end(), pPrepData); 
 if ( it != MdtPrepDataVector.end() ){ size_t index = std::distance(MdtPrepDataVector.begin(), it); return LinearIndexMDTSet[ int (index) ] ; }
 return -1;
}
int MuonRecDigiStoreAth::FindRank( const Muon::RpcPrepData*       pPrepData ) const { 
 std::vector<const Muon::RpcPrepData*>::const_iterator it = std::find(RpcPrepDataVector.begin(), RpcPrepDataVector.end(), pPrepData); 
 if ( it != RpcPrepDataVector.end() ){ size_t index = std::distance(RpcPrepDataVector.begin(), it); return LinearIndexRPCSet[ int (index) ] ; }
 return -1;
}
int MuonRecDigiStoreAth::FindRank( const Muon::TgcPrepData*       pPrepData ) const { 
 std::vector<const Muon::TgcPrepData*>::const_iterator it = std::find(TgcPrepDataVector.begin(), TgcPrepDataVector.end(), pPrepData); 
 if ( it != TgcPrepDataVector.end() ){ size_t index = std::distance(TgcPrepDataVector.begin(), it); return LinearIndexTGCSet[ int (index) ] ; }
 return -1;
}
int MuonRecDigiStoreAth::FindRank( const Muon::CscStripPrepData*  pPrepData ) const { 
 std::vector<const Muon::CscStripPrepData*>::const_iterator it = std::find(CscStripPrepDataVector.begin(), CscStripPrepDataVector.end(), pPrepData); 
 if ( it != CscStripPrepDataVector.end() ){ size_t index = std::distance(CscStripPrepDataVector.begin(), it); return LinearIndexCSCSet[ int (index) ] ; }
 return -1;
}
int MuonRecDigiStoreAth::FindRank( const Muon::CscPrepData*       pPrepData ) const { 
 std::vector<const Muon::CscPrepData*>::const_iterator it = std::find(CscCluPrepDataVector.begin(), CscCluPrepDataVector.end(), pPrepData); 
 if ( it != CscCluPrepDataVector.end() ){ size_t index = std::distance(CscCluPrepDataVector.begin(), it); return LinearIndexCSCCluSet[ int (index) ] ; }
 return -1;
}


// Reserve 
void MuonRecDigiStoreAth::ReserveMuonRecDigiStoreAth(
                                                     const int& DoReserve,
                                                     const int& SpaceForMDT   ,
                                                     const int& SpaceForRPC   ,
                                                     const int& SpaceForTGC   ,
                                                     const int& SpaceForCSC   ,
                                                     const int& SpaceForCSCClu
                                                     ){

  ReserveMuonRecDigiStore(
                          DoReserve,
                          SpaceForMDT   ,
                          SpaceForRPC   ,
                          SpaceForTGC   ,
                          SpaceForCSC   ,
                          SpaceForCSCClu
                          );

//   int SpaceAll =
//     + SpaceForMDT   
//     + SpaceForRPC   
//     + SpaceForTGC   
//     + SpaceForCSC
//     + SpaceForCSCClu;

  int CapacityAnteMdtPrepDataVector  	   = MdtPrepDataVector.capacity() ;
  int CapacityAnteRpcPrepDataVector  	   = RpcPrepDataVector.capacity() ;
  int CapacityAnteTgcPrepDataVector  	   = TgcPrepDataVector.capacity() ;
  int CapacityAnteCscStripPrepDataVector   = CscStripPrepDataVector.capacity() ;
  int CapacityAnteCscCluPrepDataVector     = CscCluPrepDataVector.capacity() ;

  MdtPrepDataVector.reserve(SpaceForMDT);
  RpcPrepDataVector.reserve(SpaceForRPC);
  TgcPrepDataVector.reserve(SpaceForTGC);
  CscStripPrepDataVector.reserve(SpaceForCSC);
  CscCluPrepDataVector.reserve(SpaceForCSCClu);

  int CapacityPostMdtPrepDataVector  	   = MdtPrepDataVector.capacity() ;
  int CapacityPostRpcPrepDataVector  	   = RpcPrepDataVector.capacity() ;
  int CapacityPostTgcPrepDataVector  	   = TgcPrepDataVector.capacity() ;
  int CapacityPostCscStripPrepDataVector   = CscStripPrepDataVector.capacity() ;
  int CapacityPostCscCluPrepDataVector     = CscCluPrepDataVector.capacity() ;

  if ( DoReserve == 2 ){
    std::cout << " MdtPrepDataVector       " << CapacityAnteMdtPrepDataVector      << "  " << CapacityPostMdtPrepDataVector       << std::endl;
    std::cout << " RpcPrepDataVector       " << CapacityAnteRpcPrepDataVector      << "  " << CapacityPostRpcPrepDataVector       << std::endl;
    std::cout << " TgcPrepDataVector       " << CapacityAnteTgcPrepDataVector      << "  " << CapacityPostTgcPrepDataVector       << std::endl;
    std::cout << " CscStripPrepDataVector  " << CapacityAnteCscStripPrepDataVector << "  " << CapacityPostCscStripPrepDataVector       << std::endl;
    std::cout << " CscCluPrepDataVector    " << CapacityAnteCscCluPrepDataVector   << "  " << CapacityPostCscCluPrepDataVector    << std::endl;
  }

}



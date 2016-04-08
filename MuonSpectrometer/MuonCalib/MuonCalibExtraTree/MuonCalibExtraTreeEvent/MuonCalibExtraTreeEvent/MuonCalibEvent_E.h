/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibEvent_E_h
#define MuonCalibEvent_E_h

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include <vector>

namespace MuonCalib {
  class MuonCalibTrack_E;
  class MuonCalibCaloHit;
  class MuonCalibTriggerInfo;
  class MuonCalibMUCTPI;
  class MuonCalibRpcSectorLogic;
  class MuonCalibRawRpcCoin;
  class MuonCalibRawRpcTriggerHit;
  
  /**
     @class MuonCalibEvent_E
     Extension to the 'original' MuonCalib::MuonCalibEvent class. This class makes it possible to store the additional, 'extended' MuonCalib_E classes in the same format. 

     The Extended MuonCalibEvent stores the following (additional) sets:
     
     - PhiPatVec, a collection of PhiPatterns (of type MuonCalib::MuonCalibTrack) with its associated PhiHits (of type MuonCalib::MuonCalibHit).
     - TrackVec, a collection of Tracks (of type MuonCalib::MuonCalibTrack) with its associated TrackHits (of type MuonCalib::MuonCalibHit).
     - MBTSVec, a collection of MuonCalibCaloHits that stores the MBTS trigger information.
     - L1CaloVec, a collection of MuonCalibCaloHits that stores the L1Calo tower trigger information.
     - MUCTPIVec, a collection of MuonCalibMUCTPI entries that stores the L1Muon trigger information.
     - RpcSLVec, a collection of MuonCalibRpcSectorLogic entries that stores the Rpc readout information.
     @author Zdenko.Van.Kesteren@cern.ch
  */

  class MuonCalibEvent_E : public MuonCalibEvent {
  public:
    typedef std::vector<MuonCalibTrack_E*> PhiPatVec; //!< typedef for a set of MuonCalibTrack_E s describing PhiPatterns
    typedef std::vector<MuonCalibTrack_E*> TrackVec;  //!< typedef for a set of MuonCalibTrack_E s describing Tracks
    typedef std::vector<MuonCalibTriggerInfo*> CtpVec;  //!< typedef for a set of TriggerInfo bits 
    typedef std::vector<MuonCalibMUCTPI*> MUCTPIVec;  //!< typedef for a set of MUCTPI entries
    typedef std::vector<MuonCalibRpcSectorLogic*> RpcSLVec;  //!< typedef for a set of MuonCalibRpcSectorLogic entries
    typedef std::vector<MuonCalibRawRpcCoin*> RpcCoinVec;  //!< typedef for a set of MuonCalibRpcCoin entries
    typedef std::vector<MuonCalibRawRpcTriggerHit*> RawRpcTrigVec;  //!< typedef for a set of MuonCalibRawRpcTriggerHit entries
    typedef std::vector<MuonCalibCaloHit*> MBTSVec;  //!< typedef for a set of MuonCalibCalo Hits for the MBTS 
    typedef std::vector<MuonCalibCaloHit*> L1CaloVec;  //!< typedef for a set of MuonCalibCalo Hits for L1Calo

    MuonCalibEvent_E(){};                             //!< default constructor
    MuonCalibEvent_E(const MuonCalibEvent& event);    //!< constructor starting from a MuonCalib::MuonCalibEvent
    MuonCalibEvent_E(const MuonCalibEvent_E& event);  //!< copyconstructor
    MuonCalibEvent_E& operator=(const MuonCalibEvent_E &right); //!< assign operator
    ~MuonCalibEvent_E () ;                            //!< destructor

    void addPattern( MuonCalibTrack_E* pat) ;             //!< Adds a PhiPattern to PhiPatVec
    void addTrack( MuonCalibTrack_E* track) ;             //!< Adds a Track to TrackVec
    void addCtp( MuonCalibTriggerInfo* trig) ;           //!< Adds a TriggerInfo to CtpVec
    void addMUCTPI( MuonCalibMUCTPI* mctp) ;            //!< Adds a MUCTPI obj to MUCTPIVec
    void addRpcSL( MuonCalibRpcSectorLogic* rpcsl) ;      //!< Adds an RpcSectorLogic obj to RpcSLVec
    void addRpcCoin( MuonCalibRawRpcCoin* rpcco) ;      //!< Adds an RpcCoin obj to RpcCoinVec
    void addRawRpcTrigHit( MuonCalibRawRpcTriggerHit* rpctr) ;      //!< Adds an RpcCoin obj to RpcCoinVec
    void addMBTS( MuonCalibCaloHit* hit) ;               //!< Adds a MBTS hit to MBTSVec 
    void addL1Calo( MuonCalibCaloHit* hit) ;            //!< Adds a L1Calo hit/tower to L1CaloVec

    unsigned int nrPhiPat() const { return m_phiPats.size();}; //!< returns number of PhiPatterns of the MuonCalibEvent_E
    unsigned int nrTracks() const { return m_tracks.size();};  //!< returns number of Tracks of the MuonCalibEvent_E
    unsigned int nrCtp() const { return m_ctp.size();};  //!< returns number of CTP hits /bits in MuonCalibEvent_E
    unsigned int nrMUCTPI() const { return m_mctpi.size();};  //!< returns number of MUCTPI objects in MuonCalibEvent_E
    unsigned int nrRpcSL() const { return m_rpcsl.size();};  //!< returns number of RpcSectorLogic objects in MuonCalibEvent_E
    unsigned int nrRpcCoin() const { return m_rpcco.size();};  //!< returns number of RpcCoinVec objects in MuonCalibEvent_E
    unsigned int nrRawRpcTrigHit() const { return m_rpctr.size();};  //!< returns number of RpcCoinVec objects in MuonCalibEvent_E
    unsigned int nrMBTS() const { return m_mbts.size();};  //!< returns number of MBTS hits in MuonCalibEvent_E
    unsigned int nrL1Calo() const { return m_l1calo.size();};  //!< returns number of L1Calo towers in MuonCalibEvent_E
    
    PhiPatVec::iterator beginPhiPat() { return m_phiPats.begin(); }; //!< retrieve an iterator of the first PhiPattern of the set
    PhiPatVec::iterator endPhiPat() { return m_phiPats.end(); };     //!< retrieve an iterator of the last PhiPattern of the set
    
    PhiPatVec::const_iterator beginPhiPat() const { return m_phiPats.begin(); }; //!< retrieve a const_iterator of the first PhiPattern of the set
    PhiPatVec::const_iterator endPhiPat() const { return m_phiPats.end(); };     //!< retrieve a const_iterator of the last PhiPattern of the set

    TrackVec::iterator beginTrack() { return m_tracks.begin(); };            //!< retrieve an iterator of the first Track of the set
    TrackVec::iterator endTrack() { return m_tracks.end(); };                //!< retrieve an iterator of the last Track of the set
    
    TrackVec::const_iterator beginTrack() const { return m_tracks.begin(); };//!< retrieve a const_iterator of the first Track of the set
    TrackVec::const_iterator endTrack() const { return m_tracks.end(); };    //!< retrieve a const_iterator of the last Track of the set

    CtpVec::iterator beginCtp() { return m_ctp.begin(); };                //!< retrieve an iterator of the first Ctp triggerinfo bit  
    CtpVec::iterator endCtp() { return m_ctp.end(); };                //!< retrieve an iterator of the last Ctp triggerinfo bit  
    CtpVec::const_iterator beginCtp() const { return m_ctp.begin(); };                //!< retrieve a const iterator of the first Ctp triggerinfo bit  
    CtpVec::const_iterator endCtp() const { return m_ctp.end(); };                //!< retrieve a const iterator of the first Ctp triggerinfo bit  

    MUCTPIVec::iterator beginMUCTPI() { return m_mctpi.begin(); };                //!< retrieve an iterator of the first MUCTPI triggerinfo bit  
    MUCTPIVec::iterator endMUCTPI() { return m_mctpi.end(); };                //!< retrieve an iterator of the last MUCTPI triggerinfo bit  
    MUCTPIVec::const_iterator beginMUCTPI() const { return m_mctpi.begin(); };                //!< retrieve a const iterator of the first MUCTPI triggerinfo bit  
    MUCTPIVec::const_iterator endMUCTPI() const { return m_mctpi.end(); };                //!< retrieve a const iterator of the first MUCTPI triggerinfo bit  

    RpcSLVec::iterator beginRpcSL() { return m_rpcsl.begin(); };                //!< retrieve an iterator of the first RpcSL triggerinfo bit  
    RpcSLVec::iterator endRpcSL() { return m_rpcsl.end(); };                //!< retrieve an iterator of the last RpcSL triggerinfo bit  
    RpcSLVec::const_iterator beginRpcSL() const { return m_rpcsl.begin(); };                //!< retrieve a const iterator of the first RpcSL triggerinfo bit  
    RpcSLVec::const_iterator endRpcSL() const { return m_rpcsl.end(); };                //!< retrieve a const iterator of the first RpcSL triggerinfo bit  

    RpcCoinVec::iterator beginRpcCoin() { return m_rpcco.begin(); };                //!< retrieve an iterator of the first RpcCoin
    RpcCoinVec::iterator endRpcCoin() { return m_rpcco.end(); };                //!< retrieve an iterator of the last RpcCoin
    RpcCoinVec::const_iterator beginRpcCoin() const { return m_rpcco.begin(); };                //!< retrieve a const iterator of the first RpcCoin
    RpcCoinVec::const_iterator endRpcCoin() const { return m_rpcco.end(); };                //!< retrieve a const iterator of the first RpcCoin

    RawRpcTrigVec::iterator beginRawRpcTrig() { return m_rpctr.begin(); };                //!< retrieve an iterator of the first RawRpcTrigHit
    RawRpcTrigVec::iterator endRawRpcTrig() { return m_rpctr.end(); };                //!< retrieve an iterator of the last RawRpcTrigHit
    RawRpcTrigVec::const_iterator beginRawRpcTrig() const { return m_rpctr.begin(); };                //!< retrieve a const iterator of the first RawRpcTrigHit
    RawRpcTrigVec::const_iterator endRawRpcTrig() const { return m_rpctr.end(); };                //!< retrieve a const iterator of the first RawRpcTrigHit

    MBTSVec::iterator beginMBTS() { return m_mbts.begin(); };                //!< retrieve an iterator of the first MBTS calo hit 
    MBTSVec::iterator endMBTS() { return m_mbts.end(); };                    //!< retrieve an iterator of the last MBTS calo hit 
    MBTSVec::const_iterator beginMBTS() const { return m_mbts.begin(); };    //!< retrieve a const_iterator of the first MBTS calo hit 
    MBTSVec::const_iterator endMBTS() const { return m_mbts.end(); };        //!< retrieve a const_iterator of the last MBTS calo hit 

    L1CaloVec::iterator beginL1Calo() { return m_l1calo.begin(); };                //!< retrieve an iterator of the first MBTS calo hit 
    L1CaloVec::iterator endL1Calo() { return m_l1calo.end(); };                    //!< retrieve an iterator of the last MBTS calo hit 
    L1CaloVec::const_iterator beginL1Calo() const { return m_l1calo.begin(); };    //!< retrieve a const_iterator of the first MBTS calo hit 
    L1CaloVec::const_iterator endL1Calo() const { return m_l1calo.end(); };        //!< retrieve a const_iterator of the last MBTS calo hit 

  private:
    PhiPatVec m_phiPats; //!< set of PhiPatterns belonging to the MuonCalibEvent_E
    TrackVec  m_tracks;  //!< set of Tracks belonging to the MuonCalibEvent_E
    CtpVec    m_ctp;     //!< set of CTP trigger bits in MuonCalibEvent_E
    MUCTPIVec m_mctpi;   //!< set of MUCTPI objects in MuonCalibEvent_E
    RpcSLVec  m_rpcsl;   //!< set of RpcSL objects in MuonCalibEvent_E
    RpcCoinVec m_rpcco;  //!< set of RpcCoin objects in MuonCalibEvent_E
    RawRpcTrigVec m_rpctr;  //!< set of RawRpcTrig objects in MuonCalibEvent_E
    MBTSVec   m_mbts;    //!< set of MBTS Calo Hits in MuonCalibEvent_E
    L1CaloVec m_l1calo;  //!< set of L1Calo Hits/towers in MuonCalibEvent_E
  };
} //namespace MuonCalib


#endif//MuonCalibEvent_E_h

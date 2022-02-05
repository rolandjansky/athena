/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibEvent_E_h
#define MuonCalibEvent_E_h

#include <vector>

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibExtraTreeEvent/MuonCalibCaloHit.h"
#include "MuonCalibExtraTreeEvent/MuonCalibMUCTPI.h"
#include "MuonCalibExtraTreeEvent/MuonCalibRpcSectorLogic.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTriggerInfo.h"
namespace MuonCalib {

    /**
       @class MuonCalibEvent_E
       Extension to the 'original' MuonCalib::MuonCalibEvent class. This class makes it possible to store the additional, 'extended'
       MuonCalib_E classes in the same format.

       The Extended MuonCalibEvent stores the following (additional) sets:

       - PhiPatVec, a collection of PhiPatterns (of type MuonCalib::MuonCalibTrack) with its associated PhiHits (of type
       MuonCalib::MuonCalibHit).
       - TrackVec, a collection of Tracks (of type MuonCalib::MuonCalibTrack) with its associated TrackHits (of type
       MuonCalib::MuonCalibHit).
       - MBTSVec, a collection of MuonCalibCaloHits that stores the MBTS trigger information.
       - L1CaloVec, a collection of MuonCalibCaloHits that stores the L1Calo tower trigger information.
       - MUCTPIVec, a collection of MuonCalibMUCTPI entries that stores the L1Muon trigger information.
       - RpcSLVec, a collection of MuonCalibRpcSectorLogic entries that stores the Rpc readout information.
    */

    class MuonCalibEvent_E : public MuonCalibEvent {
    public:
        using MuonCalibTrkPtr = std::shared_ptr<const MuonCalibTrack_E>;
        using MuCTPiPtr = std::shared_ptr<const MuonCalibMUCTPI>;
        using TrigInfoPtr = std::shared_ptr<const MuonCalibTriggerInfo>;
        using RpcSectorLogicPtr = std::shared_ptr<const MuonCalibRpcSectorLogic>;
        using RpcCoinPtr = std::shared_ptr<const MuonCalibRawRpcCoin>;
        using RpcTrigHitPtr = std::shared_ptr<const MuonCalibRawRpcTriggerHit>;
        using CaloHitPtr = std::shared_ptr<const MuonCalibCaloHit>;

        //!< typedef for a set of MuonCalibTrack_E s describing PhiPatterns
        typedef std::vector<MuonCalibTrkPtr> PhiPatVec;
        //!< typedef for a set of MuonCalibTrack_E s describing Tracks

        typedef std::vector<MuonCalibTrkPtr> TrackVec;
        //!< typedef for a set of TriggerInfo bits

        typedef std::vector<TrigInfoPtr> CtpVec;
        //!< typedef for a set of MUCTPI entries

        typedef std::vector<MuCTPiPtr> MUCTPIVec;
        //!< typedef for a set of MuonCalibRpcSectorLogic entries
        typedef std::vector<RpcSectorLogicPtr> RpcSLVec;
        //!< typedef for a set of MuonCalibRpcCoin entries

        typedef std::vector<RpcCoinPtr> RpcCoinVec;
        //!< typedef for a set of MuonCalibRawRpcTriggerHit entries
        typedef std::vector<RpcTrigHitPtr> RawRpcTrigVec;
        //!< typedef for a set of MuonCalibCalo Hits for the MBTS
        typedef std::vector<CaloHitPtr> MBTSVec;
        //!< typedef for a set of MuonCalibCalo Hits for L1Calo
        typedef std::vector<CaloHitPtr> L1CaloVec;

        MuonCalibEvent_E() = default;                                //!< default constructor
        MuonCalibEvent_E(const MuonCalibEvent& event);               //!< constructor starting from a MuonCalib::MuonCalibEvent
        MuonCalibEvent_E(const MuonCalibEvent_E& event);             //!< copyconstructor
        MuonCalibEvent_E& operator=(const MuonCalibEvent_E& right);  //!< assign operator
        ~MuonCalibEvent_E() = default;                               //!< destructor

        void addPattern(MuonCalibTrack_E* pat);                   //!< Adds a PhiPattern to PhiPatVec
        void addTrack(MuonCalibTrack_E* track);                   //!< Adds a Track to TrackVec
        void addCtp(MuonCalibTriggerInfo* trig);                  //!< Adds a TriggerInfo to CtpVec
        void addMUCTPI(MuonCalibMUCTPI* mctp);                    //!< Adds a MUCTPI obj to MUCTPIVec
        void addRpcSL(MuonCalibRpcSectorLogic* rpcsl);            //!< Adds an RpcSectorLogic obj to RpcSLVec
        void addRpcCoin(MuonCalibRawRpcCoin* rpcco);              //!< Adds an RpcCoin obj to RpcCoinVec
        void addRawRpcTrigHit(MuonCalibRawRpcTriggerHit* rpctr);  //!< Adds an RpcCoin obj to RpcCoinVec
        void addMBTS(MuonCalibCaloHit* hit);                      //!< Adds a MBTS hit to MBTSVec
        void addL1Calo(MuonCalibCaloHit* hit);                    //!< Adds a L1Calo hit/tower to L1CaloVec

        void addPattern(const MuonCalibTrkPtr& pat);        //!< Adds a PhiPattern to PhiPatVec
        void addTrack(const MuonCalibTrkPtr& track);        //!< Adds a Track to TrackVec
        void addCtp(const TrigInfoPtr& trig);               //!< Adds a TriggerInfo to CtpVec
        void addMUCTPI(const MuCTPiPtr& mctp);              //!< Adds a MUCTPI obj to MUCTPIVec
        void addRpcSL(const RpcSectorLogicPtr& rpcsl);      //!< Adds an RpcSectorLogic obj to RpcSLVec
        void addRpcCoin(const RpcCoinPtr& rpcco);           //!< Adds an RpcCoin obj to RpcCoinVec
        void addRawRpcTrigHit(const RpcTrigHitPtr& rpctr);  //!< Adds an RpcCoin obj to RpcCoinVec
        void addMBTS(const CaloHitPtr& hit);                //!< Adds a MBTS hit to MBTSVec
        void addL1Calo(const CaloHitPtr& hit);              //!< Adds a L1Calo hit/tower to L1CaloVec

        unsigned int nrPhiPat() const;         //!< returns number of PhiPatterns of the MuonCalibEvent_E
        unsigned int nrTracks() const;         //!< returns number of Tracks of the MuonCalibEvent_E
        unsigned int nrCtp() const;            //!< returns number of CTP hits /bits in MuonCalibEvent_E
        unsigned int nrMUCTPI() const;         //!< returns number of MUCTPI objects in MuonCalibEvent_E
        unsigned int nrRpcSL() const;          //!< returns number of RpcSectorLogic objects in MuonCalibEvent_E
        unsigned int nrRpcCoin() const;        //!< returns number of RpcCoinVec objects in MuonCalibEvent_E
        unsigned int nrRawRpcTrigHit() const;  //!< returns number of RpcCoinVec objects in MuonCalibEvent_E
        unsigned int nrMBTS() const;           //!< returns number of MBTS hits in MuonCalibEvent_E
        unsigned int nrL1Calo() const;         //!< returns number of L1Calo towers in MuonCalibEvent_E

        /// Accesses the Phi ptterns of the event directly
        const PhiPatVec& phiPat() const;
        PhiPatVec& phiPat();

        /// Accesses the tracks of the event directly
        const TrackVec& tracks() const;
        TrackVec& tracks();

        /// Access to the Ctp triggerinfo hits / bits
        const CtpVec& ctp() const;
        CtpVec& ctp();

        //!< retrieve an iterator of the first MUCTPI triggerinfo bit
        const MUCTPIVec& MUCTPI() const;
        MUCTPIVec& MUCTPI();

        const RpcSLVec& RpcSL() const;
        RpcSLVec& RpcSL();

        const RpcCoinVec& RpcCoin() const;
        RpcCoinVec& RpcCoin();

        const RawRpcTrigVec& rawRpcTrig() const;
        RawRpcTrigVec& rawRpcTrig();

        const MBTSVec& MBTS() const;
        MBTSVec& MBTS();
        const L1CaloVec& L1Calo() const;
        L1CaloVec& L1Calo();

    private:
        void copy(const MuonCalibEvent_E& other);

        PhiPatVec m_phiPats;    //!< set of PhiPatterns belonging to the MuonCalibEvent_E
        TrackVec m_tracks;      //!< set of Tracks belonging to the MuonCalibEvent_E
        CtpVec m_ctp;           //!< set of CTP trigger bits in MuonCalibEvent_E
        MUCTPIVec m_mctpi;      //!< set of MUCTPI objects in MuonCalibEvent_E
        RpcSLVec m_rpcsl;       //!< set of RpcSL objects in MuonCalibEvent_E
        RpcCoinVec m_rpcco;     //!< set of RpcCoin objects in MuonCalibEvent_E
        RawRpcTrigVec m_rpctr;  //!< set of RawRpcTrig objects in MuonCalibEvent_E
        MBTSVec m_mbts;         //!< set of MBTS Calo Hits in MuonCalibEvent_E
        L1CaloVec m_l1calo;     //!< set of L1Calo Hits/towers in MuonCalibEvent_E
    };
}  // namespace MuonCalib

#endif  // MuonCalibEvent_E_h

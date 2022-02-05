/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibEventBase_MuonCalibRawHitCollection_h
#define MuonCalibEventBase_MuonCalibRawHitCollection_h

#include <iostream>
#include <memory>
#include <vector>

#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"

namespace MuonCalib {

    class MuonCalibRawTgcCoin;

    /**
       @class MuonCalibRawHitCollection
       A MuonCalibRawHitcollections is a container class within the
       Calibration EDM holding the MuonCalibXxxRawHits from the
       different technologies:
       - a set of MuonCalibRawMdtHit s
       - a set of MuonCalibRawRpcHit s
       - a set of MuonCalibRawTgcHit s
       - a set of MuonCalibRawCscHit s

       A MuonCalibXxxRawHit is constructed from PrepRawData hits,
       collected from stations which have a MuonCalibSegment. These raw
       hits are uncalibrated. In order to know whether the raw hit is NOT
       assigned to a MuonCalibSegment, check the occupancy variable. It
       yields the number of times that the raw hit is used on a
       MuonCalibSegment (one hit may be used multiple times on
       different segments). A raw hit not used on any segment yields an
       occupancy of -1.

    */

    class MuonCalibRawHitCollection {
    public:
        //!< typedef of the basic pointer holding the CalibrationRawMdtHit
        using MdtCalibRawHitPtr = std::shared_ptr<MuonCalibRawMdtHit>;
        //!< typedef of a set of MuonCalibRawMdtHit s
        using MuonCalibRawMdtHitVec = std::vector<MdtCalibRawHitPtr>;

        //!< typedef of the smart pointer owning the CaliorationRawRpcHit
        using RpcCalibRawHitPtr = std::shared_ptr<const MuonCalibRawRpcHit>;
        //!< typedef of a set of MuonCalibRawRpcHit s
        using MuonCalibRawRpcHitVec = std::vector<RpcCalibRawHitPtr>;

        using TgcCalibRawHitPtr = std::shared_ptr<const MuonCalibRawTgcHit>;
        //!< typedef of a set of MuonCalibRawTgcHit s
        using MuonCalibRawTgcHitVec = std::vector<TgcCalibRawHitPtr>;

        using TgcCoinRawPtr = std::shared_ptr<const MuonCalibRawTgcCoin>;
        //!< typedef of a set of MuonCalibRawTgcHit s
        using MuonCalibRawTgcCoinVec = std::vector<TgcCoinRawPtr>;

        using CscCalibRawHitPtr = std::shared_ptr<const MuonCalibRawCscHit>;
        //!< typedef of a set of MuonCalibRawCscHit s
        using MuonCalibRawCscHitVec = std::vector<CscCalibRawHitPtr>;

        MuonCalibRawHitCollection() = default;                                                  //<! default constructor
        ~MuonCalibRawHitCollection() = default;                                                 //!< destructor
        MuonCalibRawHitCollection(const MuonCalibRawHitCollection& MuonCalibRawHitCollection);  //!< copy constructor
        MuonCalibRawHitCollection& operator=(const MuonCalibRawHitCollection& rhs);             //!< assignment operator

        //!< constructor initializing all raw hit collections
        MuonCalibRawHitCollection(MuonCalibRawMdtHitVec rawMdtHitVec, MuonCalibRawRpcHitVec rawRpcHitVec,
                                  MuonCalibRawTgcHitVec rawTgcHitVec, MuonCalibRawTgcCoinVec rawTgcCoinVec,
                                  MuonCalibRawCscHitVec rawCscHitVec);

        /// Retrieve references to the container themselves
        // Mdt containers
        const MuonCalibRawMdtHitVec& MdtContainer() const;
        MuonCalibRawMdtHitVec& MdtContainer();

        const MuonCalibRawRpcHitVec& RpcContainer() const;
        MuonCalibRawRpcHitVec& RpcContainer();

        const MuonCalibRawTgcHitVec& TgcContainer() const;
        MuonCalibRawTgcHitVec& TgcContainer();

        const MuonCalibRawCscHitVec& CscContainer() const;
        MuonCalibRawCscHitVec& CscContainer();

        const MuonCalibRawTgcCoinVec& TgcCoinContainer() const;
        MuonCalibRawTgcCoinVec& TgcCoinContainer();

        //!< retrieve the number of MuonCalibRawMdtHits in the collection
        int numberOfMuonCalibRawMdtHits() const;
        //!< retrieve the number of MuonCalibRawRpcHits in the collection
        int numberOfMuonCalibRawRpcHits() const;
        //!< retrieve the number of MuonCalibRawTgcHits in the collection
        int numberOfMuonCalibRawTgcHits() const;
        //!< retrieve the number of MuonCalibRawTgcHits in the collection
        int numberOfMuonCalibRawTgcCoins() const;
        //!< retrieve the number of MuonCalibRawCscHits in the collection
        int numberOfMuonCalibRawCscHits() const;

        //!< add a MuonCalibRawMdtHit to the collection
        void addMuonCalibRawHit(MuonCalibRawMdtHit* rawHit);
        void addMuonCalibRawHit(const MdtCalibRawHitPtr& rawHit);

        //!< add a MuonCalibRawRpcHit to the collection
        void addMuonCalibRawHit(MuonCalibRawRpcHit* rawHit);
        void addMuonCalibRawHit(const RpcCalibRawHitPtr& rawHit);

        //!< add a MuonCalibRawTgcHit to the collection
        void addMuonCalibRawHit(MuonCalibRawTgcHit* rawHit);
        void addMuonCalibRawHit(const TgcCalibRawHitPtr& rawHit);

        //!< add a MuonCalibRawTgcCoin to the collection
        void addMuonCalibRawHit(MuonCalibRawTgcCoin* rawHit);
        void addMuonCalibRawHit(const TgcCoinRawPtr& rawHit);

        //!< add a MuonCalibRawCscHit to the collection
        void addMuonCalibRawHit(MuonCalibRawCscHit* rawHit);
        void addMuonCalibRawHit(const CscCalibRawHitPtr& rawHit);

    private:
        void copy(const MuonCalibRawHitCollection& other);

        MuonCalibRawMdtHitVec m_rawMdtHitVec;    //!< set of MuonCalibRawMdtHit s
        MuonCalibRawRpcHitVec m_rawRpcHitVec;    //!< set of MuonCalibRawRpcHit s
        MuonCalibRawTgcHitVec m_rawTgcHitVec;    //!< set of MuonCalibRawTgcHit s
        MuonCalibRawTgcCoinVec m_rawTgcCoinVec;  //!< set of MuonCalibRawTgcCoin s
        MuonCalibRawCscHitVec m_rawCscHitVec;    //!< set of MuonCalibRawCscHit s
    };

}  // namespace MuonCalib

#endif

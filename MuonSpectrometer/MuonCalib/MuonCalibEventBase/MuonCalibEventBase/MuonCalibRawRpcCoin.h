/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc information in                   //
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
        MuonCalibRawRpcCoin() = default;                                           //!< default constructor
        MuonCalibRawRpcCoin(const MuonCalibRawRpcCoin& hit) = default;             //!< copyconstructor
        MuonCalibRawRpcCoin& operator=(const MuonCalibRawRpcCoin& rhs) = default;  //!< assignment operator
        ~MuonCalibRawRpcCoin() = default;                                          //!< destructor

        struct coinPars {
            unsigned short ijk{0};
            unsigned short threshold{0};
            unsigned short overlap{0};
            unsigned short parentCmId{0};
            unsigned short parentPadId{0};
            unsigned short parentSectorId{0};
            bool lowPtCm{false};
        };

        struct defPars : public MuonCalibRawRpcHit::defineParams, public coinPars {
            defPars() = default;
            defPars(const defPars&) = default;
            defPars(defPars&&) = default;
        };

        MuonCalibRawRpcCoin(const MuonCalibRawRpcCoin::defPars& pars);  //!< constructor setting all attributes

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawRpcCoin

        unsigned short ijk() const;
        unsigned short threshold() const;
        unsigned short overlap() const;
        unsigned short parentCmId() const;
        unsigned short parentPadId() const;
        unsigned short parentSectorId() const;
        bool lowPtCm() const;

    private:
        coinPars m_pars{};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawRpcCoin& rawHit);

#endif  // MuonCalibRawRpcCoin_h

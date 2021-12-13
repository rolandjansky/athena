/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Rpc TRIGGER hits                     //
//*********************************************************//

#ifndef MUONCALIBRAWRPCTRIGGERHIT_H
#define MUONCALIBRAWRPCTRIGGERHIT_H
// std
#include <iomanip>
#include <iostream>
#include <string>

namespace MuonCalib {

    /**
       @class MuonCalibRawRpcTriggerHit
       Calib-EDM version of RpcRawData class
     */

    class MuonCalibRawRpcTriggerHit {
    public:
        struct defineParams {
            int sector{0};
            int padId{0};
            int status{0};
            int errorCode{0};
            int cmaId{0};
            int fel1Id{0};
            int febcId{0};
            int crc{0};
            int bcId{0};
            int time{0};
            int ijk{0};
            int channel{0};
            int overlap{0};
            int threshold{0};
        };
        MuonCalibRawRpcTriggerHit() = default;                                                 //!< default constructor
        MuonCalibRawRpcTriggerHit(const MuonCalibRawRpcTriggerHit& hit) = default;             //!< copy constructor
        MuonCalibRawRpcTriggerHit& operator=(const MuonCalibRawRpcTriggerHit& rhs) = default;  //!< assignment operator
        ~MuonCalibRawRpcTriggerHit() = default;                                                //!< destructor

        MuonCalibRawRpcTriggerHit(const defineParams& pars);  //!< constructor setting all attributes

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRawRpcTriggerHit

        int sector() const;     // pad sector Id
        int padId() const;      // pad Id
        int status() const;     // status
        int errorCode() const;  // error code
        int cmaId() const;      // CMA Id
        int fel1Id() const;     // fel1Id
        int febcId() const;     // febcId
        int crc() const;        // crc
        int bcId() const;       // bc Id
        int time() const;       // time
        int ijk() const;        // ijk code
        int channel() const;    // fired channel
        int overlap() const;    // overlap
        int threshold() const;

    private:
        defineParams m_pars{};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRawRpcTriggerHit& rawHit);

#endif  // MuonCalibRawRpcTriggerHit_h

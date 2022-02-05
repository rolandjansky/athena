/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store truth information in                     //
//*********************************************************//

#ifndef MuonCalibRpcTruthHit_h
#define MuonCalibRpcTruthHit_h

#include <iostream>

#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibRpcTruthHit
       Class with truth information of a RPC hit, containing:
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - barcode
       - time
    */
    class MuonCalibRpcTruthHit {
    public:
        MuonCalibRpcTruthHit() = default;                                            //!< default constructor
        MuonCalibRpcTruthHit(const MuonCalibRpcTruthHit& truth) = default;           //!< copyconstructor
        ~MuonCalibRpcTruthHit() = default;                                           //!< destructor
        MuonCalibRpcTruthHit& operator=(const MuonCalibRpcTruthHit& rhs) = default;  //!< assignment operator

        MuonCalibRpcTruthHit(MuonFixedId id, int barCode, double time);  //!< constructor initializing all attributes of the truth hit

        std::ostream& dump(std::ostream& stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibRpcTruthHit

        const MuonFixedId& identify() const;  //!< retrieve the MuonFixedId
        int barCode() const;                  //!< retrieve the barcode
        double time() const;                  //!< retrieve the time

        void setIdentifier(MuonFixedId id);  //!< sets the MuonFixedId
        void setBarCode(int barCode);        //!< sets the barcode
        void setTime(double time);           //!< sets the time

    private:
        MuonFixedId m_ID{0};  //!< identifier of the truth hit, (is a MuonFixedId, not an Identifier)
        int m_barCode{0};     //!< barcode
        double m_time{0.};    //!< time
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibRpcTruthHit& truth);

#endif  // MuonCalibRpcTruth_h

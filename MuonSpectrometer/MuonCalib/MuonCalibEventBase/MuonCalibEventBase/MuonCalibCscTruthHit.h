/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store truth information in                     //
//*********************************************************//

#ifndef MuonCalibCscTruthHit_h
#define MuonCalibCscTruthHit_h

#include <iostream>

#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
    /**
       @class MuonCalibCscTruthHit
       Class with truth information of a RPC hit, containing:
       - an identifier, stored in the MuonFixedId format (not an Identifier)
       - barcode
       - time
    */
    class MuonCalibCscTruthHit {
    public:
        MuonCalibCscTruthHit() = default;                                            //!< default constructor
        MuonCalibCscTruthHit(const MuonCalibCscTruthHit &truth) = default;           //!< copy constructor
        MuonCalibCscTruthHit &operator=(const MuonCalibCscTruthHit &rhs) = default;  //!< assignment operator
        ~MuonCalibCscTruthHit() = default;                                           //!< destructor

        MuonCalibCscTruthHit(MuonFixedId id, int barCode, double time);  //!< constructor initializing all attributes of the truth hit

        std::ostream &dump(std::ostream &stream) const;  //!< dump to be used for operator<<() to dump the MuonCalibCscTruthHit

        MuonFixedId identify() const;  //!< retrieve the MuonFixedId
        int barCode() const;           //!< retrieve the barcode
        double time() const;           //!< retrieve the time

        void setIdentifier(MuonFixedId id);  //!< sets the MuonFixedId
        void setBarCode(int barCode);        //!< sets the barcode
        void setTime(double time);           //!< sets the time

    private:
        MuonFixedId m_ID{0};  //!< identifier of the truth hit, (a MuonFixedId, not an Identifier)
        int m_barCode{0};     //!< barcode
        double m_time{0.};    //!< time
    };

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibCscTruthHit &truth);

#endif  // MuonCalibCscTruth_h

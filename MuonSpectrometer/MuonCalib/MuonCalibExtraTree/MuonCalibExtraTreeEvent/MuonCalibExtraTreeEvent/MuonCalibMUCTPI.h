/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBMUCTPI_H
#define MUONCALIBMUCTPI_H

#include <string>
#include <vector>

namespace MuonCalib {

    /**
       Simplified class designed to store information from MuCTPi processor. It has :
    */
    class MuonCalibMUCTPI {
    public:
        MuonCalibMUCTPI() = default;  //!< default constructor
        struct defineParams {
            int roiWord{0};
            int bcID{0};
            int sysID{0};
            int subsysID{0};
            int sectorID{0};
            int thresholdNumber{0};
            int thresholdValue{0};
            int roINumber{0};
            int overlap{0};
            int firstCandidate{0};
            int sectorOverflow{0};
            int padOverflow{0};
            float phi{0.};
            float eta{0.};
        };

        MuonCalibMUCTPI(defineParams pars);  //!< constructor setting all trackparameters and position
        ~MuonCalibMUCTPI() = default;        //!< destructor

        int roiWord() const;
        int bcID() const;
        int sysID() const;
        int subsysID() const;
        int sectorID() const;
        int thresholdNumber() const;
        int thresholdValue() const;
        int roINumber() const;
        int overlap() const;
        int firstCandidate() const;
        int sectorOverflow() const;
        int padOverflow() const;
        float phi() const;
        float eta() const;

    private:
        defineParams m_params{};
    };

}  // namespace MuonCalib

#endif

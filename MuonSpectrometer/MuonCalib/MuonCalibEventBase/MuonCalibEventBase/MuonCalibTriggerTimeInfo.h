/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEVTBASE_MuonCalibTriggerTimeInfo_h
#define MUONCALIBEVTBASE_MuonCalibTriggerTimeInfo_h

#include <iostream>
#include <string>
#include <vector>

namespace MuonCalib {

    class MuonCalibTriggerTimeInfo {
    public:
        MuonCalibTriggerTimeInfo() = default;                                                      //!< Default constructor
        MuonCalibTriggerTimeInfo(const MuonCalibTriggerTimeInfo& dTimeInfo) = default;             //!< copyconstructor
        MuonCalibTriggerTimeInfo& operator=(const MuonCalibTriggerTimeInfo& dTimeInfo) = default;  //!< assignment operator
        ~MuonCalibTriggerTimeInfo() = default;                                                     //!< destructor

        MuonCalibTriggerTimeInfo(float dMbtsTime, float dLarTime);  //!< constructor with Mbts Time and LAr time

        std::ostream& dump(std::ostream& stream) const;

        float dMbtsTime() const;
        float dLarTime() const;

        void setdMbtsTime(float dMbtsTime);  //!< sets mbts
        void setdLarTime(float dLarTime);    //!< sets lar

    private:
        float m_dMbtsTime{-999.};
        float m_dLarTime{-999.};
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibTriggerTimeInfo& info);

#endif  // MuonCalibTriggerTimeInfo_h

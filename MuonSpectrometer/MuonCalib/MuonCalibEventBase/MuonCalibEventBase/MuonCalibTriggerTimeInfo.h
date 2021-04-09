/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibTriggerTimeInfo_h
#define MuonCalibTriggerTimeInfo_h

#include <iostream>
#include <string>
#include <vector>

namespace MuonCalib {

    class MuonCalibTriggerTimeInfo {
    public:
        MuonCalibTriggerTimeInfo();                                                      //!< Default constructor
        MuonCalibTriggerTimeInfo(float dMbtsTime, float dLarTime);                       //!< constructor with Mbts Time and LAr time
        MuonCalibTriggerTimeInfo(const MuonCalibTriggerTimeInfo& dTimeInfo);             //!< copyconstructor
        MuonCalibTriggerTimeInfo& operator=(const MuonCalibTriggerTimeInfo& dTimeInfo);  //!< assignment operator
        ~MuonCalibTriggerTimeInfo();                                                     //!< destructor

        std::ostream& dump(std::ostream& stream) const;

        float dMbtsTime() const { return m_dMbtsTime; }
        float dLarTime() const { return m_dLarTime; }

        void setdMbtsTime(float dMbtsTime) { m_dMbtsTime = dMbtsTime; }  //!< sets mbts
        void setdLarTime(float dLarTime) { m_dLarTime = dLarTime; }      //!< sets lar

    private:
        float m_dMbtsTime;
        float m_dLarTime;
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibTriggerTimeInfo& info);

#endif  // MuonCalibTriggerTimeInfo_h

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"

namespace MuonCalib {
    float MuonCalibTriggerTimeInfo::dMbtsTime() const { return m_dMbtsTime; }
    float MuonCalibTriggerTimeInfo::dLarTime() const { return m_dLarTime; }
    void MuonCalibTriggerTimeInfo::setdMbtsTime(float dMbtsTime) { m_dMbtsTime = dMbtsTime; }
    void MuonCalibTriggerTimeInfo::setdLarTime(float dLarTime) { m_dLarTime = dLarTime; }
    MuonCalibTriggerTimeInfo::MuonCalibTriggerTimeInfo(float dMbtsTime, float dLarTime) : m_dMbtsTime{dMbtsTime}, m_dLarTime{dLarTime} {}
    std::ostream &MuonCalibTriggerTimeInfo::dump(std::ostream &stream) const {
        stream << "MuonCalibTriggerTimeInfo : " << std::endl;
        stream << "         dMbtsTime = " << dMbtsTime() << std::endl;
        stream << "         dLarTime  = " << dLarTime() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibTriggerTimeInfo &info) { return info.dump(stream); }

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"

namespace MuonCalib {

    MuonCalibTriggerTimeInfo::MuonCalibTriggerTimeInfo() : m_dMbtsTime(-999.), m_dLarTime(-999.) {}

    MuonCalibTriggerTimeInfo::MuonCalibTriggerTimeInfo(float dMbtsTime, float dLarTime) : m_dMbtsTime(dMbtsTime), m_dLarTime(dLarTime) {}

    MuonCalibTriggerTimeInfo::MuonCalibTriggerTimeInfo(const MuonCalibTriggerTimeInfo &eventInfo) {
        m_dMbtsTime = eventInfo.dMbtsTime();
        m_dLarTime = eventInfo.dLarTime();
    }

    MuonCalibTriggerTimeInfo &MuonCalibTriggerTimeInfo::operator=(const MuonCalibTriggerTimeInfo &eventInfo) {
        if (this != &eventInfo) {
            m_dMbtsTime = eventInfo.dMbtsTime();
            m_dLarTime = eventInfo.dLarTime();
        }
        return *this;
    }

    MuonCalibTriggerTimeInfo::~MuonCalibTriggerTimeInfo() {}

    std::ostream &MuonCalibTriggerTimeInfo::dump(std::ostream &stream) const {
        stream << "MuonCalibTriggerTimeInfo : " << std::endl;
        stream << "         dMbtsTime = " << dMbtsTime() << std::endl;
        stream << "         dLarTime  = " << dLarTime() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibTriggerTimeInfo &info) { return info.dump(stream); }

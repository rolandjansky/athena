/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibEventInfo.h"

namespace MuonCalib {

    MuonCalibEventInfo::MuonCalibEventInfo() : m_runNumber(0), m_eventNumber(0), m_timeStamp(0), m_lumiBlock(0), m_bcId(0), m_tag("none") {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(0), m_lumiBlock(0), m_bcId(0), m_tag("none") {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(time_stamp), m_lumiBlock(0), m_bcId(0), m_tag("none") {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp,
                                           unsigned int lumi_block, unsigned int bc_id) :
        m_runNumber(run_number),
        m_eventNumber(event_number),
        m_timeStamp(time_stamp),
        m_lumiBlock(lumi_block),
        m_bcId(bc_id),
        m_tag("none") {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp, std::string tag) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(time_stamp), m_lumiBlock(0), m_bcId(0), m_tag(tag) {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp,
                                           unsigned int lumi_block, unsigned int bc_id, std::string tag) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(time_stamp), m_lumiBlock(lumi_block), m_bcId(bc_id), m_tag(tag) {}

    MuonCalibEventInfo::MuonCalibEventInfo(const MuonCalibEventInfo &eventInfo) {
        m_runNumber = eventInfo.runNumber();
        m_eventNumber = eventInfo.eventNumber();
        m_timeStamp = eventInfo.timeStamp();
        m_lumiBlock = eventInfo.lumiBlock();
        m_bcId = eventInfo.bcId();
        m_tag = eventInfo.tag();
        m_trigger_bits = eventInfo.m_trigger_bits;
    }

    MuonCalibEventInfo &MuonCalibEventInfo::operator=(const MuonCalibEventInfo &eventInfo) {
        if (this != &eventInfo) {
            m_runNumber = eventInfo.runNumber();
            m_eventNumber = eventInfo.eventNumber();
            m_timeStamp = eventInfo.timeStamp();
            m_lumiBlock = eventInfo.lumiBlock();
            m_bcId = eventInfo.bcId();
            m_tag = eventInfo.tag();
            m_trigger_bits = eventInfo.m_trigger_bits;
        }
        return *this;
    }

    MuonCalibEventInfo::~MuonCalibEventInfo() {}

    std::ostream &MuonCalibEventInfo::dump(std::ostream &stream) const {
        stream << "MuonCalibEventInfo : " << std::endl;
        stream << "         runnumber = " << runNumber() << std::endl;
        stream << "       eventnumber = " << eventNumber() << std::endl;
        stream << "         timestamp = " << timeStamp() << std::endl;
        stream << "         lumiblock = " << lumiBlock() << std::endl;
        stream << "         bcid      = " << bcId() << std::endl;
        stream << "               tag = " << tag() << std::endl;
        return stream;
    }

}  // namespace MuonCalib

std::ostream &operator<<(std::ostream &stream, const MuonCalib::MuonCalibEventInfo &info) { return info.dump(stream); }

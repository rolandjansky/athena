/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonCalibEventBase/MuonCalibEventInfo.h"

namespace MuonCalib {

    unsigned int MuonCalibEventInfo::runNumber() const { return m_runNumber; }
    unsigned int MuonCalibEventInfo::eventNumber() const { return m_eventNumber; }
    unsigned int MuonCalibEventInfo::timeStamp() const { return m_timeStamp; }
    unsigned int MuonCalibEventInfo::lumiBlock() const { return m_lumiBlock; }
    unsigned int MuonCalibEventInfo::bcId() const { return m_bcId; }
    std::string MuonCalibEventInfo::tag() const { return m_tag; }
    const std::vector<bool> &MuonCalibEventInfo::triggerBits() const { return m_trigger_bits; }

    void MuonCalibEventInfo::setRunNumber(const unsigned int run_number) { m_runNumber = run_number; }
    void MuonCalibEventInfo::setEventNumber(const unsigned int event_number) { m_eventNumber = event_number; }
    void MuonCalibEventInfo::setTimeStamp(const unsigned int time_stamp) { m_timeStamp = time_stamp; }
    void MuonCalibEventInfo::setLumiBlock(const unsigned int lumi_block) { m_lumiBlock = lumi_block; }
    void MuonCalibEventInfo::setBcId(const unsigned int bc_id) { m_bcId = bc_id; }
    void MuonCalibEventInfo::setTag(const std::string &tag) { m_tag = tag; }
    void MuonCalibEventInfo::setNumberOfTriggerBits(unsigned int n) { m_trigger_bits.resize(n); }
    void MuonCalibEventInfo::setTriggerBit(unsigned int n, bool bit) { m_trigger_bits[n] = bit; }

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number) :
        m_runNumber{run_number}, m_eventNumber{event_number} {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp) :
        m_runNumber{run_number}, m_eventNumber{event_number}, m_timeStamp{time_stamp} {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp,
                                           unsigned int lumi_block, unsigned int bc_id) :
        m_runNumber{run_number}, m_eventNumber{event_number}, m_timeStamp{time_stamp}, m_lumiBlock{lumi_block}, m_bcId{bc_id} {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp, std::string tag) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(time_stamp), m_lumiBlock(0), m_bcId(0), m_tag(std::move(tag)) {}

    MuonCalibEventInfo::MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp,
                                           unsigned int lumi_block, unsigned int bc_id, std::string tag) :
        m_runNumber(run_number), m_eventNumber(event_number), m_timeStamp(time_stamp), m_lumiBlock(lumi_block), m_bcId(bc_id), m_tag(std::move(tag)) {}

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

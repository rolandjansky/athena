/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store event information in for MuonCalibEvent  //
//*********************************************************//

#ifndef MUONCALIBEVTBASE_MuonCalibEventInfo_h
#define MUONCALIBEVTBASE_MuonCalibEventInfo_h

#include <iostream>
#include <string>
#include <vector>

namespace MuonCalib {
    /**
       @class MuonCalibEventInfo
       MuonCalib::MuonCalibEventInfo: a class which stores the following information of an event:
       - run number
       - event number
       - time stamp
       - a tag, used to distinguish which reconstruction package is used to reconstruct the
    MuonCalibSegment. At the moment, three tags are used:

    Moore : Muon reconstruction with the Moore package. One segment per pattern is be stored.

    Muonboy : Muon reconstruction with the Muonboy package. One segment per pattern is stored.

    MoMu : Modular muon reconstruction is applied. A pattern may contain multiple segments.
    */
    class MuonCalibEventInfo {
    public:
        MuonCalibEventInfo() = default;                                                //!< Default constructor
        MuonCalibEventInfo(const MuonCalibEventInfo& eventInfo) = default;             //!< copyconstructor
        MuonCalibEventInfo& operator=(const MuonCalibEventInfo& eventInfo) = default;  // !< assignment operator
        ~MuonCalibEventInfo() = default;                                               //!< destructor

        MuonCalibEventInfo(unsigned int run_number, unsigned int event_number);  //!< constructor with runnumber/eventnumber (for sim data)
        MuonCalibEventInfo(unsigned int run_number, unsigned int event_number,
                           unsigned int time_stamp);  //!< constructor run/event number and time stamp arguments
        MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp, unsigned int lumi_block,
                           unsigned int bc_id);
        MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp,
                           std::string tag);  //!< constructor from older times
        MuonCalibEventInfo(unsigned int run_number, unsigned int event_number, unsigned int time_stamp, unsigned int lumi_block,
                           unsigned int bc_id, std::string tag);  //!< constructor initializing all members

        std::ostream& dump(std::ostream& stream) const;

        unsigned int runNumber() const;    //!< retrieving runnumber of MuonCalibEvent
        unsigned int eventNumber() const;  //!< retrieving eventnumber of MuonCalibevent
        unsigned int timeStamp() const;    //!< retrieving timestamp of MuonCalibEvent
        unsigned int lumiBlock() const;    //!< retrieving lumiblock of MuonCalibEvent
        unsigned int bcId() const;         //!< retrieving bcid of MuonCalibEvent
        std::string tag() const;           //!< retrieving reconstruction tag of MuonCalibEvent
        const std::vector<bool>& triggerBits() const;

        void setRunNumber(const unsigned int run_number);      //!< sets runnumber
        void setEventNumber(const unsigned int event_number);  //!< sets eventnumber
        void setTimeStamp(const unsigned int time_stamp);      //!< sets timestamp
        void setLumiBlock(const unsigned int lumi_block);      //!< sets lumiblock
        void setBcId(const unsigned int bc_id);                //!< sets bcid
        void setTag(const std::string& tag);                   //!< sets reconstruction tag
        void setNumberOfTriggerBits(unsigned int n);
        void setTriggerBit(unsigned int n, bool bit);

    private:
        unsigned int m_runNumber{0};       //!< runnumber member
        unsigned int m_eventNumber{0};     //!< eventnumber member
        unsigned int m_timeStamp{0};       //!< timestamp member
        unsigned int m_lumiBlock{0};       //!< lumiBlock member
        unsigned int m_bcId{0};            //!< bcId member
        std::string m_tag{"none"};         //!< reconstruction tag member
        std::vector<bool> m_trigger_bits;  //!< Trigger bits
    };

}  // namespace MuonCalib

std::ostream& operator<<(std::ostream& stream, const MuonCalib::MuonCalibEventInfo& info);

#endif  // MuonCalibEventInfo_h

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibPatterns in of the same event  //
// Author: Zdenko van Kesteren                             //
// Date  : 18 October 2005                                 //
//*********************************************************//

#ifndef MuonCalibEvent_h
#define MuonCalibEvent_h

#include <iostream>
#include <string>
#include <vector>

#include "MuonCalibEventBase/MuonCalibEventInfo.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

namespace MuonCalib {

    class MuonCalibPattern;
    class MuonCalibRawHitCollection;
    class MuonCalibRawTriggerHitCollection;
    class MuonCalibTruthCollection;
    class MuonCalibTriggerTimeInfo;

    /**
       @class MuonCalibEvent
       Container class storing all entries of the CalibrationNtuple.
       It contains the following:
       - MuonCalibEventInfo: run/event number, a tag holding information
       about type of reconstruction performed
       - MuonCalibTruthCollection: This truth collection is the
       CalibrationEDM version of a Truth Track and SDO's from the
       several technologies
       - MuonCalibRawHitCollection: CalibrationEDM hits collected from
       PrepRawData objects. These can be assigned to a segment (see the
       'occupancy' variable).
       - MuonCalibPattern: Class to store the information belonging to a
       global pattern in the muon system.  It contains all information
       needed to calibrate the different technologies in the muon
       system.

        @author Zdenko.van.Kesteren@cern.ch
  */

    class MuonCalibEvent {
    public:
        typedef std::vector<MuonCalibPattern *> MCPVec;  //!< typedef to std::vector<MuonCalibPattern*>
        typedef MCPVec::iterator MCPVecIt;               //!< typedef to its iterator
        typedef MCPVec::const_iterator MCPVecCit;        //!< typedef to its const_iterator

        MuonCalibEvent();               //!< Default constructor
        MuonCalibEvent(MCPVec patvec);  //<! Constructor initializing its MuonCalibPatterns.
        MuonCalibEvent(MCPVec patvec,
                       const MuonCalibRawHitCollection *RHColl);  //!< Constructor initializing its MuonCalibPatterns and raw hits.
        MuonCalibEvent(
            MCPVec patvec, const MuonCalibEventInfo eventInfo,
            const MuonCalibRawHitCollection *Coll);   //!< Constructor initializing its MuonCalibPattern, raw hits and event information.
        MuonCalibEvent(const MuonCalibEvent &event);  //!< Copy constructor
        virtual ~MuonCalibEvent();                    //!< Destructor

        MuonCalibEvent &operator=(const MuonCalibEvent &rhs);  //!< Assignment operator

        MCPVecIt patternBegin();         //!< retrieving the iterator pointing to the first MuonCalibPattern
        MCPVecIt patternEnd();           //!< retrieving the iterator pointing to the last MuonCalibPattern
        MCPVecCit patternBegin() const;  //!< retrieving the const_iterator pointing to) the first MuonCalibPattern
        MCPVecCit patternEnd() const;    //!< retrieving the const_iterator pointing to) the last MuonCalibPattern

        int numberOfPatterns() const;  //!< retrieve the number of MuonCalibPatterns in the event
        int numberOfSegments() const;  //!< retrieve the number of MuonCalibSegments in the event
        int numberOfHits() const;      //!< retrieve the number of CalibHits in the event (sum of all technologies).

        const MuonCalibRawHitCollection *rawHitCollection() const {
            return m_rawColl;
        };  //!< retrieve  pointer to MuonCalibRawHitCollection
        const MuonCalibRawTriggerHitCollection *rawTriggerHitCollection() const {
            return m_rawTriggerColl;
        };  //!< retrieve  pointer to MuonCalibRawHitCollection
        const MuonCalibTruthCollection *calibTruthCollection() const {
            return m_truthColl;
        };                                                                    //!< retrieve the pointer to the MuonCalibTruthCollection.
        const MuonCalibEventInfo &eventInfo() const { return m_eventInfo; };  //!< retrieve the MuonCalibEventInfo.
        const RpcSectorLogicContainer *rpcSectorLogicContainer() const { return m_rpcSlLogicContainer; }
        const MuonCalibTriggerTimeInfo *triggerTimeInfo() const { return m_triggerTimeInfo; }

        void setMuonCalibTriggerTimeInfo(const MuonCalibTriggerTimeInfo *triggerTimeInfo);  //!< set the MuonCalibTriggerTimeInfo
        void setMuonCalibPatternVec(const MuonCalibEvent::MCPVec &mcpvec);                  //!< set the MuonCalibPatternVec
        void setMuonCalibRawHitCollection(const MuonCalibRawHitCollection *RHColl);         //!< set the MuonCalibRawHitCollection.
        void setMuonCalibRawTriggerHitCollection(
            const MuonCalibRawTriggerHitCollection *THColl);                        //!< set the MuonCalibRawTriggerHitCollection.
        void setMuonCalibTruthCollection(const MuonCalibTruthCollection *MCTColl);  //!< set the MuonCalibTruthCollection
        void setMuonCalibEventInfo(const MuonCalibEventInfo &eventInfo) { m_eventInfo = eventInfo; };  //!< set the MuonCalibEventInfo
        void setRpcSectorLogicContainer(const RpcSectorLogicContainer *rpcSlContainer) {
            m_rpcSlLogicContainer = rpcSlContainer;
        };  //!< set the RpcSectorLogicContainer

    private:
        MCPVec m_patternVec;             //!< A set of MuonCalibPattern s owned by the MuonCalibEvent
        MuonCalibEventInfo m_eventInfo;  //!< MuonCalibEventInfo of the MuonCalibEvent
        const MuonCalibTriggerTimeInfo *m_triggerTimeInfo;
        const MuonCalibRawHitCollection *m_rawColl;                //!< raw hits of the MuonCalibEvent
        const MuonCalibRawTriggerHitCollection *m_rawTriggerColl;  //!< raw hits of the MuonCalibEvent
        const MuonCalibTruthCollection *m_truthColl;               //!< truth collection of the MuonCalibEvent
        const RpcSectorLogicContainer *m_rpcSlLogicContainer;      //!< RpcSectorLogicContainer
    };

    inline void MuonCalibEvent::setMuonCalibTriggerTimeInfo(const MuonCalibTriggerTimeInfo *triggerTimeInfo) {
        m_triggerTimeInfo = triggerTimeInfo;
    }
    inline void MuonCalibEvent::setMuonCalibTruthCollection(const MuonCalibTruthCollection *MCTColl) { m_truthColl = MCTColl; }
    inline void MuonCalibEvent::setMuonCalibRawHitCollection(const MuonCalibRawHitCollection *RHColl) { m_rawColl = RHColl; }
    inline void MuonCalibEvent::setMuonCalibRawTriggerHitCollection(const MuonCalibRawTriggerHitCollection *THColl) {
        m_rawTriggerColl = THColl;
    }
    inline void MuonCalibEvent::setMuonCalibPatternVec(const MuonCalibEvent::MCPVec &mcpvec) { m_patternVec = mcpvec; }
    inline MuonCalibEvent::MCPVecIt MuonCalibEvent::patternBegin() { return m_patternVec.begin(); }
    inline MuonCalibEvent::MCPVecCit MuonCalibEvent::patternBegin() const { return m_patternVec.begin(); }
    inline MuonCalibEvent::MCPVecIt MuonCalibEvent::patternEnd() { return m_patternVec.end(); }
    inline MuonCalibEvent::MCPVecCit MuonCalibEvent::patternEnd() const { return m_patternVec.end(); }

}  // namespace MuonCalib

#endif  // MuonCalibEvent_h

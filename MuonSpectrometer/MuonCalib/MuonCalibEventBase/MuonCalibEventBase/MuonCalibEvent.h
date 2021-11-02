/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibPatterns in of the same event  //
//*********************************************************//

#ifndef MUONCALIBEVTBASE_MuonCalibEvent_h
#define MUONCALIBEVTBASE_MuonCalibEvent_h

#include <iostream>
#include <string>
#include <vector>

#include "MuonCalibEventBase/MuonCalibEventInfo.h"
#include "MuonCalibEventBase/MuonCalibRawRpcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

namespace MuonCalib {

    class MuonCalibPattern;
    class MuonCalibRawHitCollection;
    class MuonCalibRawTriggerHitCollection;
    class MuonCalibTruthCollection;

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

  */

    class MuonCalibEvent {
    public:
        using PatternPtr = std::shared_ptr<MuonCalibPattern>;
        typedef std::vector<PatternPtr> MCPVec;  //!< typedef to std::vector<MuonCalibPattern*>

        MuonCalibEvent() = default;                                      //!< Default constructor
        MuonCalibEvent(const MuonCalibEvent &event) = default;           //!< Copy constructor
        MuonCalibEvent &operator=(const MuonCalibEvent &rhs) = default;  //!< Assignment operator
        virtual ~MuonCalibEvent();                                       //!< Destructor

        MuonCalibEvent(MCPVec patvec);  //<! Constructor initializing its MuonCalibPatterns.
        MuonCalibEvent(
            MCPVec patvec,
            std::shared_ptr<const MuonCalibRawHitCollection> RHColl);  //!< Constructor initializing its MuonCalibPatterns and raw hits.
        MuonCalibEvent(MCPVec patvec, std::shared_ptr<MuonCalibEventInfo> eventInfo,
                       std::shared_ptr<const MuonCalibRawHitCollection>
                           Coll);  //!< Constructor initializing its MuonCalibPattern, raw hits and event information.

        /// Retrieving the MuonCalibPattern container
        MCPVec &pattern();
        const MCPVec &pattern() const;

        int numberOfPatterns() const;  //!< retrieve the number of MuonCalibPatterns in the event
        int numberOfSegments() const;  //!< retrieve the number of MuonCalibSegments in the event
        int numberOfHits() const;      //!< retrieve the number of CalibHits in the event (sum of all technologies).
                                       //!< retrieve  pointer to MuonCalibRawHitCollection
        const MuonCalibRawHitCollection *rawHitCollection() const;
        //!< retrieve  pointer to MuonCalibRawHitCollection
        const MuonCalibRawTriggerHitCollection *rawTriggerHitCollection() const;
        //!< retrieve the pointer to the MuonCalibTruthCollection.
        const MuonCalibTruthCollection *calibTruthCollection() const;
        //!< retrieve the MuonCalibEventInfo.
        const MuonCalibEventInfo &eventInfo() const;
        const RpcSectorLogicContainer *rpcSectorLogicContainer() const;
        const MuonCalibTriggerTimeInfo *triggerTimeInfo() const;

        void setMuonCalibTriggerTimeInfo(
            std::shared_ptr<const MuonCalibTriggerTimeInfo> triggerTimeInfo);                        //!< set the MuonCalibTriggerTimeInfo
        void setMuonCalibPatternVec(MuonCalibEvent::MCPVec mcpvec);                                  //!< set the MuonCalibPatternVec
        void setMuonCalibRawHitCollection(std::shared_ptr<const MuonCalibRawHitCollection> RHColl);  //!< set the MuonCalibRawHitCollection.
        void setMuonCalibRawTriggerHitCollection(
            std::shared_ptr<const MuonCalibRawTriggerHitCollection> THColl);  //!< set the MuonCalibRawTriggerHitCollection.
        void setMuonCalibTruthCollection(std::shared_ptr<const MuonCalibTruthCollection> MCTColl);  //!< set the MuonCalibTruthCollection
        void setMuonCalibEventInfo(std::shared_ptr<MuonCalibEventInfo> eventInfo);                  //!< set the MuonCalibEventInfo
        void setRpcSectorLogicContainer(
            std::shared_ptr<const RpcSectorLogicContainer> rpcSlContainer);  //!< set the RpcSectorLogicContainer

    private:
        void copy(const MuonCalibEvent &other);

        MCPVec m_patternVec;                              //!< A set of MuonCalibPattern s owned by the MuonCalibEvent
        std::shared_ptr<MuonCalibEventInfo> m_eventInfo;  //!< MuonCalibEventInfo of the MuonCalibEvent
        std::shared_ptr<const MuonCalibTriggerTimeInfo> m_triggerTimeInfo;
        std::shared_ptr<const MuonCalibRawHitCollection> m_rawColl;                //!< raw hits of the MuonCalibEvent
        std::shared_ptr<const MuonCalibRawTriggerHitCollection> m_rawTriggerColl;  //!< raw hits of the MuonCalibEvent
        std::shared_ptr<const MuonCalibTruthCollection> m_truthColl;               //!< truth collection of the MuonCalibEvent
        std::shared_ptr<const RpcSectorLogicContainer> m_rpcSlLogicContainer;      //!< RpcSectorLogicContainer
    };

}  // namespace MuonCalib

#endif  // MuonCalibEvent_h

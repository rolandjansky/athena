/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibEventLoop/MdtCalibEventLoop.h"

#include <algorithm>
#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

namespace MuonCalib {

    MdtCalibEventLoop::MdtCalibEventLoop(const std::string& regionKey) :
        m_calibrationImp(nullptr), m_calibrationResult(nullptr), m_regionKey(regionKey) {
        m_printLevel = 1;
    }

    MdtCalibEventLoop::~MdtCalibEventLoop() = default;

    void MdtCalibEventLoop::performAnalysis() {
        if (m_printLevel >= 1) {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::VERBOSE << "MdtCalibEventLoop::performAnalysis for region " << m_regionKey << endmsg;
        }

        // perform rt-calibration
        m_calibrationResult = m_calibrationImp->analyseSegments(m_segments);

        if (m_printLevel >= 2) {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::DEBUG << "new rtOutput " << m_calibrationResult << endmsg;
        }
    }

    bool MdtCalibEventLoop::handleSegment(const MuonCalibSegment* cseg) {
        if (m_printLevel >= 4) {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::WARNING << "MdtCalibEventLoop::handleSegment " << cseg << endmsg;
        }

        if (cseg) {
            // eventloop owns the segments so it should make a copy
            MuonCalibSegment* seg = new MuonCalibSegment(*cseg);

            // add segment to vector
            m_segments.emplace_back(seg);

        } else {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::WARNING << "MdtCalibEventLoop::handleSegment: got nullptr " << endmsg;
            return false;
        }
        return true;
    }

    void MdtCalibEventLoop::setCalibImp(IMdtCalibration* calibImp) {
        if (m_printLevel >= 2) {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::DEBUG << "MdtCalibEventLoop::setRtCalibTool >> new tool " << calibImp << endmsg;
        }
        if (calibImp) {
            m_calibrationImp = calibImp;
        } else {
            MsgStream log(Athena::getMessageSvc(), "MdtCalibEventLoop");
            log << MSG::WARNING << "MdtCalibEventLoop::setCalibImp: got nullptr " << endmsg;
            m_calibrationImp = nullptr;
        }
    }

}  // namespace MuonCalib

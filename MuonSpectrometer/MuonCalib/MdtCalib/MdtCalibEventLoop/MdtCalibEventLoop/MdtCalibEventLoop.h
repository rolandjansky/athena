/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MdtCalibEventLoop.h
//   Header file for class MdtCalibEventLoop
#ifndef MUONCALIB_MDTCALIBEVENTLOOP_H
#define MUONCALIB_MDTCALIBEVENTLOOP_H

#include <iostream>
#include <string>
#include <vector>

#include "MdtCalibInterfaces/IMdtCalibration.h"

namespace MuonCalib {
    /**
       @class MdtCalibEventLoop
       Class collects segments for a given chamber and performs several loops
       over the sample until the calibration algorithm thinks it converged

    */

    class IMdtCalibrationOutput;
    class MuonCalibSegment;

    class MdtCalibEventLoop {
    public:
        using MuonSegVec = IMdtCalibration::MuonSegVec;
        using MuonSegIt = MuonSegVec::iterator;
        using MuonSegCit = MuonSegVec::const_iterator;

    public:
        /** constructor */
        MdtCalibEventLoop(const std::string& regionKey);

        /** destructor */
        ~MdtCalibEventLoop();

        /** handle segment (for now store) */
        bool handleSegment(const MuonCalibSegment* seg);

        /** analyse segments using IRtCalibration */
        void performAnalysis();

        /** set pointer to RtCalibration */
        void setCalibImp(IMdtCalibration* calibImp);

        /** return results rt calibration */
        IMdtCalibration::MdtCalibOutputPtr calibrationResults() const;

        /** set maximum number of iteration (default = 10) */
        void setMaxIterations(unsigned int max);

        /** set print level */
        void setPrintLevel(int level) { m_printLevel = level; }

    private:
        IMdtCalibration* m_calibrationImp;  //!< pointer to calibration implementation
        MuonSegVec m_segments;              //!< segment collection

        IMdtCalibration::MdtCalibOutputPtr m_calibrationResult;  //!< results calibration

        /** print level */
        int m_printLevel;

        /** region key */
        std::string m_regionKey;
    };

    inline IMdtCalibration::MdtCalibOutputPtr MdtCalibEventLoop::calibrationResults() const { return m_calibrationResult; }

}  // namespace MuonCalib

#endif

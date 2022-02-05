/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalibStandAloneBase_INtupleCalibrationTool_H
#define MuonCalibStandAloneBase_INtupleCalibrationTool_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// c - c++
#include <memory>
#include <string>
#include <vector>

namespace MuonCalib {

    // calib

    class MuonCalibEvent;
    class MuonCalibSegment;
    class IMdtCalibrationOutput;
    class NtupleStationId;
    class IRtResolution;
    /** @class NtupleCalibrationTool
            Interface class for an interrface trool for calibration algorithms
    */

    class INtupleCalibrationTool : virtual public IAlgTool {
    public:
        /** destructor */
        virtual ~INtupleCalibrationTool() = default;
        /** interface method */
        static const InterfaceID &interfaceID() {
            static const InterfaceID IID_NtupleCalibrationTool("MuonCalib::NtupleCalibrationTool", 1, 0);
            return IID_NtupleCalibrationTool;
        }
        /** is called one per event, when the event is read*/
        virtual StatusCode handleEvent(std::shared_ptr<const MuonCalibEvent> & /*event*/, int /*evnt_nr*/,
                                       const std::vector<std::shared_ptr<MuonCalibSegment>> & /*segments*/, unsigned int /*position*/) = 0;
        /** is called at the end of the analysis */
        virtual StatusCode analyseSegments(const std::vector<std::shared_ptr<MuonCalibSegment>> & /*segemnts*/) = 0;
        /** get calibration results */
        virtual std::shared_ptr<IMdtCalibrationOutput> getResults() const = 0;
        /** get resolution */
        virtual std::shared_ptr<IRtResolution> getResolution() const = 0;
        /** return true if ther segment vector in analyseSegments is needed. If
        this function returns false, the segments are not stored in the segment
        vector */
        virtual bool KeepSegments() const { return true; }
    };

}  // namespace MuonCalib

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonCalib_ICalibSegmentPreparationTool_H
#define MuonCalib_ICalibSegmentPreparationTool_H

//:::::::::::::::::::::::::::::::::::::::
//:: CLASS CalibSegmentPreparationTool ::
//:::::::::::::::::::::::::::::::::::::::

/// \class CalibSegmentPreparationTool
///
/// This is the base class for algorithms preparing segments for the MDT
/// calibration.

// STL //
#include <map>

// Gaudi //
#include "GaudiKernel/IAlgTool.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

    class ICalibSegmentPreparationTool : virtual public IAlgTool {
    public:
        // Destructor //
        virtual ~ICalibSegmentPreparationTool() = default;
        ///< Destructor

        // Methods //
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_CalibSegmentPreparationTool("MuonCalib::CalibSegmentPreparationTool", 1, 0);
            return IID_CalibSegmentPreparationTool;
        }
        ///< get the interface ID

        virtual bool prepareSegments(std::shared_ptr<const MuonCalibEvent>& event,
                                     std::map<NtupleStationId, std::shared_ptr<MuonCalibSegment>>& /*segments*/) = 0;
        ///< Method to prepare the segments. The methods
        ///< updates segments and remove segments from the
        ///< map in case of failure.
        ///< \param event Current event (contains raw hits
        ///<              and other useful objects).
        ///> \param segments Vector of segments to be
        ///>                 prepared.
    };

}  // namespace MuonCalib

#endif

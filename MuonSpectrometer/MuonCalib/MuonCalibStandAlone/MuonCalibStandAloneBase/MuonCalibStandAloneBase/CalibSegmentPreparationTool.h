/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.01.2009, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_CalibSegmentPreparationToolH
#define MuonCalib_CalibSegmentPreparationToolH

//:::::::::::::::::::::::::::::::::::::::
//:: CLASS CalibSegmentPreparationTool ::
//:::::::::::::::::::::::::::::::::::::::

/// \class CalibSegmentPreparationTool
///
/// This is the base class for algorithms preparing segments for the MDT
/// calibration.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 23.01.2009

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <map>

// Gaudi //
#include "GaudiKernel/IAlgTool.h"

// MuonCalib //
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

    static const InterfaceID IID_CalibSegmentPreparationTool("MuonCalib::CalibSegmentPreparationTool", 1, 0);

    class CalibSegmentPreparationTool : virtual public IAlgTool {
    public:
        // Destructor //
        inline virtual ~CalibSegmentPreparationTool(void){};
        ///< Destructor

        // Methods //
        static const InterfaceID &interfaceID(void) { return IID_CalibSegmentPreparationTool; };
        ///< get the interface ID

        inline virtual void prepareSegments(const MuonCalibEvent *& /*event*/,
                                            std::map<NtupleStationId, MuonCalibSegment *> & /*segments*/) {
            return;
        };
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

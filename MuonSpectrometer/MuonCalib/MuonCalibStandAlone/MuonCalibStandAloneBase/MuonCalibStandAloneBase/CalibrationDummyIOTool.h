/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibrationDummyIOTool_H
#define CalibrationDummyIOTool_H

// MuonCalibStandAloneBase
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCalibStandAloneBase/CalibrationIOTool.h"

namespace MuonCalib {

    class CalibrationDummyIOTool : public AthAlgTool, virtual public CalibrationIOTool {
    public:
        /** constructor*/
        inline CalibrationDummyIOTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p) {
            declareInterface<CalibrationIOTool>(this);
        }
    };

}  // namespace MuonCalib
#endif

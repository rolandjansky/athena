/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCPCALIBCONTAINER_H
#define MCPCALIBCONTAINER_H

// STL //
#include <string>
#include <memory>

// ROOT //
#include "TH1.h"
#include "TH2.h"

#include "MuonMomentumCorrections/MuonObj.h"

namespace MCP {
    class CalibContainer {
    public:
        // Constructor //
        CalibContainer(const std::string& fileName, const std::string& histName);

        // To retrieve the calib constant
        double getCalibConstant(const TrackCalibObj& trk) const;

    protected:
        std::unique_ptr<const TH1> m_calibConstantHist;       
    };

} 

#endif

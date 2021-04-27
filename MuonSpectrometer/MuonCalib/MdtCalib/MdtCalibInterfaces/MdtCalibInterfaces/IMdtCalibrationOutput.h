/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMdtCalibrationOutput.h
//   Header file for class IMdtCalibrationOutput
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_IMDTCALIBRATIONOUTPUT_H
#define MUONCALIB_IMDTCALIBRATIONOUTPUT_H

#include <string>

namespace MuonCalib {

    /**
       @class IMdtCalibrationOutput
       Interface to pass calibration output during calibration.
       To access any members from the implementations a dynamic_cast is required.

       @author Niels.Van.Eldik@cern.ch
    */

    class IMdtCalibrationOutput {
    public:
        /** constructor, string used to identify the instance */
        IMdtCalibrationOutput(std::string name) : m_name(name) {}

        /** destructor */
        virtual ~IMdtCalibrationOutput() {}

        virtual const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };

}  // namespace MuonCalib

#endif

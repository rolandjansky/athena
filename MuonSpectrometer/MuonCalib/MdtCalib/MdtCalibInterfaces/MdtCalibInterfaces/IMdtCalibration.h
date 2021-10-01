/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// IMdtCalibration.h
//   Header file for class IMdtCalibration
#ifndef MUONCALIB_IMDTCALIBRATION_H
#define MUONCALIB_IMDTCALIBRATION_H

#include <string>
#include <vector>
#include <memory>

namespace MuonCalib {

    class IMdtCalibrationOutput;
    class MuonCalibSegment;

    /**
       @class IMdtCalibration
       Interface to Mdt calibration algortihms.

       @author Niels.Van.Eldik@cern.ch
    */

    class IMdtCalibration {
    public:
        using MuonSegVec = std::vector<std::shared_ptr<MuonCalibSegment>> ;
        using MuonSegIt = MuonSegVec::iterator ;
        using MuonSegCit = MuonSegVec::const_iterator ;
        using MdtCalibOutputPtr = std::shared_ptr<IMdtCalibrationOutput>;

    public:
        /** constructor, string used to identify the instance */
        IMdtCalibration(const std::string& name) : m_name(name) {}

        /** destructor */
        virtual ~IMdtCalibration()= default;

        /** initialize algoritm */
        virtual void setInput(const IMdtCalibrationOutput* input) = 0;

        /** final analyse results */
        virtual MdtCalibOutputPtr analyseSegments(const MuonSegVec& segs) = 0;

        /** retrieve results from calibration */
        virtual MdtCalibOutputPtr getResults() const = 0;

        /** returns name (region) of instance */
        virtual std::string name() const { return m_name; }

    private:
        std::string m_name;
    };

}  // namespace MuonCalib

#endif

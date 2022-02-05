/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTFULLCALIBDATA_H
#define MUONCALIB_MDTFULLCALIBDATA_H

namespace MuonCalib {

    class MdtCorFuncSet;
    class MdtRtRelation;
    class MdtTubeCalibContainer;

    /** class which holds the full set of calibration constants for a given tube */
    struct MdtFullCalibData {
        const MdtCorFuncSet* corrections;
        const MdtRtRelation* rtRelation;
        const MdtTubeCalibContainer* tubeCalib;
        MdtFullCalibData() : corrections(0), rtRelation(0), tubeCalib(0) {}
        MdtFullCalibData(const MdtCorFuncSet* cor, const MdtRtRelation* rt, const MdtTubeCalibContainer* tub) :
            corrections(cor), rtRelation(rt), tubeCalib(tub) {}
    };

}  // namespace MuonCalib

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _CalibrationIOTool_H
#define _CalibrationIOTool_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// c - c++
#include "map"

// this
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

    class T0CalibrationOutput;
    class RtCalibrationOutput;
    class IRtResolution;
    class MdtStationT0Container;
    class IRtRelation;
    class MdtTubeFitContainer;

    /** @class CalibrationIOTool
    Interface for a toool writing out calibration
    */

    class ICalibrationIOTool : virtual public IAlgTool {
    public:
        /** destructor */
        virtual ~ICalibrationIOTool() = default;
        /** interface method */
        static const InterfaceID &interfaceID() {
            static const InterfaceID IID_CalibrationIOTool("MuonCalib::ICalibrationIOTool", 1, 0);
            return IID_CalibrationIOTool;
        }
        /** write out t0
            @param t0_output t0 data
            @param station_id The station/multilayer identifier
            @param iov_start start of the interval of validity
            @param iov_end end of the interval of validity
        */
        virtual StatusCode WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int iov_start, int iov_end) = 0;
        /** write out rt relation
            @param rt_relation the rt relation
            @param resolution the rt_resolution
            @param station_id THe station identifier
            @param iov_start start of the interval of validity
            @param iov_end end of the interval of validity
         */
        virtual StatusCode WriteRt(const RtCalibrationOutput *rt_relation, std::shared_ptr<const IRtResolution> resolution,
                                   const NtupleStationId &station_id, int iov_start, int iov_end, bool real_rt, bool real_resolution) = 0;
        /** load t0s for iov
            @param t0s map to be filled with the t0 data
            @param iov_id the iov identifier, e.g. the run number. Note: This will, at the moment alwas be -1 (invalid). So the tool will
           have to determine the run number by itself (job option)
        */
        virtual StatusCode LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int iov_id) = 0;

        /** load rts for iov
            @param rts map to be filled with rts
            @param res map to be filled with resolution
            @param iov_id the iov identifier, e.g. the run number. Note: This will, at the moment alwas be -1 (invalid). So the tool will
           have to determine the run number by itself (job option)
        */
        virtual StatusCode LoadRt(std::map<NtupleStationId, IRtRelation *> &rts, std::map<NtupleStationId, IRtResolution *> &res,
                                  int iov_id) = 0;
    };

}  // namespace MuonCalib

#endif

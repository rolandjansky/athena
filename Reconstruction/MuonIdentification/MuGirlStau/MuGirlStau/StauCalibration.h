/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STAUCALIBRATION_H
#define STAUCALIBRATION_H

#include "GaudiKernel/MsgStream.h"
#include <map>
#include "TFile.h"

using namespace std;

namespace MuGirlNS
{
    struct StauCalibrationParameters
    {
        float timeShift;
        float error;
        StauCalibrationParameters(float in_timeShift = 0, float in_error = 0) :
                timeShift(in_timeShift), error(in_error)
        {
        }
    };

    enum StauRpcCalibRange
    {
        RPC_RANGE_00_ETA,
        RPC_RANGE_00_PHI,
        RPC_RANGE_01_ETA,
        RPC_RANGE_01_PHI,
        RPC_RANGE_02_ETA,
        RPC_RANGE_02_PHI,
        RPC_RANGE_03_ETA,
        RPC_RANGE_03_PHI,
        RPC_RANGE_04_ETA,
        RPC_RANGE_04_PHI,
        RPC_RANGE_05_ETA,
        RPC_RANGE_05_PHI
    };
    inline StauRpcCalibRange rpcCalibRange(double r, bool isEta)
    {
        if (r < 7400)
            return (isEta ? RPC_RANGE_00_ETA : RPC_RANGE_00_PHI);
        else if (r < 7800)
            return (isEta ? RPC_RANGE_01_ETA : RPC_RANGE_01_PHI);
        else if (r < 8300)
            return (isEta ? RPC_RANGE_02_ETA : RPC_RANGE_02_PHI);
        else if (r < 9700)
            return (isEta ? RPC_RANGE_03_ETA : RPC_RANGE_03_PHI);
        else if (r < 10200)
            return (isEta ? RPC_RANGE_04_ETA : RPC_RANGE_04_PHI);
        else
            return (isEta ? RPC_RANGE_05_ETA : RPC_RANGE_05_PHI);
    }

    class StauCalibration
    {
    public:
        StauCalibration(MsgStream& log) : m_log(log) {}
      void initialize(std::string &mdtCalibFile, std::string &rpcCalibFile, std::string &caloCalibFile);
      virtual ~StauCalibration();

        typedef std::map<int, StauCalibrationParameters> id_calibration_map;
        typedef std::map<std::pair<int, int>, StauCalibrationParameters> layer_bin_calibration_map;

        id_calibration_map* getMdtCalibration()
        {
            return &m_calibrationMdt;
        }
        id_calibration_map* getRpcCalibration()
        {
            return &m_calibrationRpc;
        }
        layer_bin_calibration_map* getTileCalibration()
        {
            return &m_calibrationTile;
        }

    private:
        MsgStream& m_log;
        id_calibration_map m_calibrationMdt;
        layer_bin_calibration_map m_calibrationTile;
        id_calibration_map m_calibrationRpc;

	void loadMdtStauCalibration(std::string &calibFile);
	void loadRpcStauCalibration(std::string &calibFile);
	void loadTileStauCalibration(std::string &calibFile);

    };
} //end of namespace
#endif //STAUCALIBRATION_H

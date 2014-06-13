/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STAUCALIBRATION_H
#define STAUCALIBRATION_H

#include <map>

using namespace std;

namespace MuGirlNS
{

struct StauCalibrationParameters 
{
	double timeShift;
	double error;
	StauCalibrationParameters(double in_timeShift = 0, double in_error = 0):
		timeShift(in_timeShift),
		error(in_error)
	{}
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
inline StauRpcCalibRange rpcCalibRange(double r,bool isEta)
{
	if(r<7400) return ( (isEta) ? RPC_RANGE_00_ETA :  RPC_RANGE_00_PHI );
	else if(r<7800) return ( (isEta) ? RPC_RANGE_01_ETA : RPC_RANGE_01_PHI );
	else if(r<8300) return ( (isEta) ? RPC_RANGE_02_ETA : RPC_RANGE_02_PHI );
	else if(r<9700) return ( (isEta) ? RPC_RANGE_03_ETA : RPC_RANGE_03_PHI );
	else if(r<10200) return ( (isEta) ? RPC_RANGE_04_ETA : RPC_RANGE_04_PHI );
	else  return ( (isEta) ? RPC_RANGE_05_ETA : RPC_RANGE_05_PHI );
}

class StauCalibration
{
public:
        StauCalibration() {} 
	StauCalibration(const char* mdtCalibFile, const char* rpcCalibFile, const char* rpcNewCalibFile, const char* tileCalibFile, const char* phaseShiftFile);
	virtual ~StauCalibration();

	std::map<int, StauCalibrationParameters >* getMdtCalibration() { return &m_calibrationMdt;} 
	std::map<int, StauCalibrationParameters >* getRpcCalibration() { return &m_calibrationRpc;} 
	std::map<int, StauCalibrationParameters >* getNewRpcCalibration() { return &m_calibrationRpcNew;} 
	std::map<int, StauCalibrationParameters >* getTileCalibration() { return &m_calibrationTile;} 
	std::map<int, double >* getPhaseShiftMap() { return &m_phaseShiftMap;} 

private:
	std::map<int, StauCalibrationParameters > m_calibrationMdt;
	std::map<int, StauCalibrationParameters > m_calibrationRpc;
	std::map<int, StauCalibrationParameters > m_calibrationTile;
	std::map<int, double > m_phaseShiftMap;
	std::map<int, StauCalibrationParameters > m_calibrationRpcNew;
	
	void loadMdtStauCalibration(const char* calibFile);
	void loadRpcStauCalibration(const char* calibFile);
	void loadNewRpcStauCalibration(const char* calibFile);
	void loadTileStauCalibration(const char* calibFile);
	void loadPhaseShiftMap(const char* phaseShiftFile);
};
} //end of namespace
#endif //STAUCALIBRATION_H

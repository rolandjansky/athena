/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcHitIdHelper_H
#define RpcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class RpcHitIdHelper: public HitIdHelper {
public:
	
	static RpcHitIdHelper* GetHelper();
  static RpcHitIdHelper* GetHelper(unsigned int nGasGaps);
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetPhiSector(const int& hid);
	int GetZSector(const int& hid);
	int GetDoubletR(const int& hid);
	int GetGasGapLayer(const int& hid);
	int GetDoubletPhi(const int& hid);
	int GetDoubletZ(const int& hid);
	int GetMeasuresPhi(const int& hid);
	
	int BuildRpcHitId (const std::string, const int, const int, const int,
		const int, const int, const int, const int);	
	
private:
	RpcHitIdHelper();
  RpcHitIdHelper(unsigned int nGasGaps);
	void Initialize(unsigned int nGasGaps=2); // all non-BIS RPCs (Run1+2) have 2 gas gaps, only BIS RPCs have 3 gas gaps
	void InitializeStationName();
	static RpcHitIdHelper* m_help;

};

#endif

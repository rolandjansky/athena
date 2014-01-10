/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscHitIdHelper_H
#define CscHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class CscHitIdHelper: public HitIdHelper {
public:
	
	static CscHitIdHelper* GetHelper();
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetPhiSector(const int& hid);
	int GetZSector(const int& hid);
	int GetChamberLayer(const int& hid);
	int GetWireLayer (const int& hid);
	
	int BuildCscHitId (const std::string, const int, const int, const int, 
	const int); 

private:
	CscHitIdHelper();
	void Initialize();
	void InitializeStationName();
	static CscHitIdHelper* m_help;

};

#endif

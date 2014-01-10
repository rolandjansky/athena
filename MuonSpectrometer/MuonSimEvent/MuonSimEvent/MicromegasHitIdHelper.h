/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MicromegasHitIdHelper_H
#define MicromegasHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class MicromegasHitIdHelper: public HitIdHelper {
public:
	
	static MicromegasHitIdHelper* GetHelper();
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetPhiSector(const int& hid);
	int GetZSector(const int& hid);
	int GetMultiLayer(const int& hid);
	int GetLayer(const int& hid);
	int GetSide(const int& hid);
	
	int BuildMicromegasHitId (const std::string, const int, const int, const int,
		const int, const int); 

private:
	MicromegasHitIdHelper();
	void Initialize();
	void InitializeStationName();
	static MicromegasHitIdHelper* m_help;

};

#endif

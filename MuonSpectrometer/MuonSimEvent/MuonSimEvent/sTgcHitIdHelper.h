/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcHitIdHelper_H
#define sTgcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class sTgcHitIdHelper: public HitIdHelper {
public:
	
	static sTgcHitIdHelper* GetHelper();
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetPhiSector(const int& hid);
	int GetZSector(const int& hid);
	int GetMultiLayer(const int& hid);
	int GetLayer(const int& hid);
	int GetSide(const int& hid);
	
	int BuildsTgcHitId (const std::string, const int, const int, const int,
		const int, const int); 

private:
	sTgcHitIdHelper();
	void Initialize();
	void InitializeStationName();
	static sTgcHitIdHelper* m_help;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MdtHitIdHelper_H
#define MdtHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class MdtHitIdHelper: public HitIdHelper {
public:
	
	static MdtHitIdHelper* GetHelper();
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetPhiSector(const int& hid);
	int GetZSector(const int& hid);
	int GetMultiLayer(const int& hid);
	int GetLayer(const int& hid);
	int GetTube(const int& hid);
	
	int BuildMdtHitId (const std::string, const int, const int, const int,
		const int, const int); 

private:
	MdtHitIdHelper();
	void Initialize();
	void InitializeStationName();
	static MdtHitIdHelper* m_help;

};

#endif

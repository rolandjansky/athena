/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcHitIdHelper_H
#define TgcHitIdHelper_H

#include <string>

//base class
#include "HitManagement/HitIdHelper.h" 

class TgcHitIdHelper: public HitIdHelper {
public:
	
	static TgcHitIdHelper* GetHelper();
	std::string GetStationName(const int& hid);
	void SetStationName(std::string name);
	int GetStationPhi(const int& hid);
	int GetStationEta(const int& hid);
	int GetGasGap(const int& hid);
	
	int BuildTgcHitId (const std::string, const int, const int, const int);

private:
	TgcHitIdHelper();
	void Initialize();
	void InitializeStationName();
	static TgcHitIdHelper* m_help;

};

#endif

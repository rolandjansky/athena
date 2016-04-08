/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEvent_h
#define AFP_TDLocRecoEvent_h

#include <vector>

class AFP_TDLocRecoEvent {

public:

	AFP_TDLocRecoEvent();
	~AFP_TDLocRecoEvent();


	AFP_TDLocRecoEvent(int iAlgoNum, int nStationID, int nDetectorID, int nTrainID, float fTrainTime, int nTrainSize, int nSaturation);

	inline int getAlgoNum() const {return m_iAlgoNum;}
	
	inline int getStationID() const {return m_nStationID;}
	inline int getDetectorID() const {return m_nDetectorID;}
	inline int getTrainID() const {return m_nTrainID;}
	
	inline float getTrainTime() const {return m_fTrainTime;}
	inline int   getTrainSize() const {return m_nTrainSize;}
	inline int   getSaturation() const {return m_nSaturation;}
	


private:

	int m_iAlgoNum;
	
	int m_nStationID;
	int m_nDetectorID;
	int m_nTrainID;
	
	float m_fTrainTime;
	int m_nTrainSize;
	int m_nSaturation;

};

#endif

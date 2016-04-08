/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEvent_h
#define AFP_SIDLocRecoEvent_h

#include <vector>

class AFP_SIDLocRecoEvent {
  
public:
  
	AFP_SIDLocRecoEvent();
	~AFP_SIDLocRecoEvent();
  
  
	AFP_SIDLocRecoEvent(int iAlgoNum, int nStationID, float x_pos, float y_pos, float z_pos, float x_slope, float y_slope, float z_slope, int nHits, int nHoles, float fChi2);

	inline int getAlgoNum() const {return m_iAlgoNum;}
	inline int getStationID() const {return m_nStationID;}
	
	inline float getXposition() const { return m_x; }
	inline float getYposition() const { return m_y; }
	inline float getZposition() const { return m_z; }
	inline float getXslope() const {return m_x_slope;}
	inline float getYslope() const {return m_y_slope;}
	inline float getZslope() const {return m_z_slope;}
	
	inline int getNHits()  const {return m_nHits;}
	inline int getNHoles() const {return m_nHoles;}
	inline float getChi2() const {return m_fChi2;}


private:
  
	int m_iAlgoNum;
	int m_nStationID;
	
	float m_x;
	float m_y;
	float m_z;
	float m_x_slope;
	float m_y_slope;
	float m_z_slope;
	
	int   m_nHits;
	int   m_nHoles;
	float m_fChi2;	
};

#endif

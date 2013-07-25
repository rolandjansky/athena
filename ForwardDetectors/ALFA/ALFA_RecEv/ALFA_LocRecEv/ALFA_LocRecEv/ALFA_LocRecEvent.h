/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEvent_h
#define ALFA_LocRecEvent_h

#include <vector>

class ALFA_LocRecEvent {
  
public:
  
	ALFA_LocRecEvent();
	~ALFA_LocRecEvent();
  
  
	ALFA_LocRecEvent(int iAlgoNum, int pot_num, float x, float y, float fOverU, float fOverV, int iNumU, int iNumV, std::vector<int> iFibSel);

	inline int getAlgoNum() const {return m_iAlgoNum;}
	inline int getPotNum() const {return m_pot_num;}
	inline float getXposition() const { return m_x; }
	inline float getYposition() const { return m_y; }

	inline float getOverU() const {return m_fOverU;}
	inline float getOverV() const {return m_fOverV;}
	inline int getNumU() const {return m_iNumU;}
	inline int getNumV() const {return m_iNumV;}
	inline std::vector<int> getFibSel() const {return m_iFibSel;}

private:
  
	int m_iAlgoNum;
	int m_pot_num;
	float m_x;
	float m_y;

	float m_fOverU;
	float m_fOverV;
	int m_iNumU;
	int m_iNumV;

	std::vector<int> m_iFibSel;
};

#endif

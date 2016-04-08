/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecODEvent_h
#define ALFA_LocRecODEvent_h

#include <vector>

class ALFA_LocRecODEvent {

public:

	ALFA_LocRecODEvent();
	~ALFA_LocRecODEvent();


	ALFA_LocRecODEvent(int iAlgoNum, int pot_num, int side, float y, float fOverY, int iNumY, std::vector<int> iFibSel);

	inline int getAlgoNum() const {return m_iAlgoNum;}
	inline int getPotNum() const {return m_pot_num;}
	inline int getSide() const {return m_side;}
	inline float getYposition() const { return m_y; }

	inline float getOverY() const {return m_fOverY;}
	inline int getNumY() const {return m_iNumY;}
	inline std::vector<int> getFibSel() const {return m_iFibSel;}

	private:

	int   m_iAlgoNum;
	int   m_pot_num;
	int   m_side;
	float m_y;

	float m_fOverY;
	int m_iNumY;

	std::vector<int> m_iFibSel;
};

#endif

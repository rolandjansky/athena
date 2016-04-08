/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEvent_h
#define ALFA_LocRecCorrEvent_h

class ALFA_LocRecCorrEvent {

public:
	ALFA_LocRecCorrEvent();
	~ALFA_LocRecCorrEvent();


	ALFA_LocRecCorrEvent(int iAlgoNum, int pot_num, float xLHC, float yLHC, float zLHC, float xPot, float yPot, float xStat, float yStat, float xBeam, float yBeam);

	inline int   getAlgoNum() const {return m_iAlgoNum;}
	inline int   getPotNum() const {return m_pot_num;}
	inline float getXpositionLHC() const {return m_xLHC;}
	inline float getYpositionLHC() const {return m_yLHC;}
	inline float getZpositionLHC() const {return m_zLHC;}
	inline float getXpositionPot() const {return m_xPot;}
	inline float getYpositionPot() const {return m_yPot;}
	inline float getXpositionStat() const {return m_xStat;}
	inline float getYpositionStat() const {return m_yStat;}
	inline float getXpositionBeam() const {return m_xBeam;}
	inline float getYpositionBeam() const {return m_yBeam;}

private:
	int m_iAlgoNum;
	int m_pot_num;
	float m_xLHC;
	float m_yLHC;
	float m_zLHC;
	float m_xPot;
	float m_yPot;
	float m_xStat;
	float m_yStat;
	float m_xBeam;
	float m_yBeam;
};

#endif

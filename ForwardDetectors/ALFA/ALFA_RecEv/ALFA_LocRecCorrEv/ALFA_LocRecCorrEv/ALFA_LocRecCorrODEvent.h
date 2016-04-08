/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrODEvent_h
#define ALFA_LocRecCorrODEvent_h

class ALFA_LocRecCorrODEvent {

public:
	ALFA_LocRecCorrODEvent();
	~ALFA_LocRecCorrODEvent();


	ALFA_LocRecCorrODEvent(int iAlgoNum, int pot_num, int side, float yLHC, float zLHC, float yPot, float yStat, float yBeam);

	inline int   getAlgoNum() const {return m_iAlgoNum;}
	inline int   getPotNum() const {return m_pot_num;}
	inline int   getSide() const {return m_side;}
	inline float getYpositionLHC() const {return m_yLHC;}
	inline float getZpositionLHC() const {return m_zLHC;}
	inline float getYpositionPot() const {return m_yPot;}
	inline float getYpositionStat() const {return m_yStat;}
	inline float getYpositionBeam() const {return m_yBeam;}

private:
	int   m_iAlgoNum;
	int   m_pot_num;
	int   m_side;
	float m_yLHC;
	float m_zLHC;
	float m_yPot;
	float m_yStat;
	float m_yBeam;
};

#endif

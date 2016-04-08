/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEvent_p1_h
#define ALFA_LocRecCorrEvent_p1_h


class ALFA_LocRecCorrEvent_p1
{
public:
	// needed by athenaRoot
	ALFA_LocRecCorrEvent_p1()
	{
		m_iAlgoNum=-1;
		m_pot_num=-1;
		m_xPot=m_yPot=-9999.0;
		m_xStat=m_yStat=-9999.0;
		m_xLHC=m_yLHC=m_zLHC=-9999.0;
		m_xBeam=m_yBeam=-9999.0;
	};
	friend class ALFA_LocRecCorrEventCnv_p1;

private:
	int m_iAlgoNum;
	int m_pot_num;
	float m_xPot, m_yPot;
	float m_xStat, m_yStat;
	float m_xLHC, m_yLHC, m_zLHC;
	float m_xBeam, m_yBeam;
};

     
#endif
     

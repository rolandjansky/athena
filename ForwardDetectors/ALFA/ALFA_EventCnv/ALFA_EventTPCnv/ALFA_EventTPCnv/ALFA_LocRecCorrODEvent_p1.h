/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrODEvent_p1_h
#define ALFA_LocRecCorrODEvent_p1_h


class ALFA_LocRecCorrODEvent_p1
{
public:
	// needed by athenaRoot
	ALFA_LocRecCorrODEvent_p1()
	{
		m_iAlgoNum=-1;
		m_pot_num=-1;
		m_side=-1;
		m_yPot=-9999.0;
		m_yStat=-9999.0;
		m_yLHC=m_zLHC=-9999.0;
		m_yBeam=-9999.0;
	};
	friend class ALFA_LocRecCorrODEventCnv_p1;

private:
	int m_iAlgoNum;
	int m_pot_num;
	int m_side;
	float m_yPot;
	float m_yStat;
	float m_yLHC, m_zLHC;
	float m_yBeam;
};

     
#endif
     

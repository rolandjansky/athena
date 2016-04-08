/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigi_p1_h
#define AFP_TDDigi_p1_h

class AFP_TDDigi_p1
{
 public:
	// needed by athenaRoot 
	AFP_TDDigi_p1(){};
	friend class AFP_TDDigiCnv_p1;

private:
	float m_fADC;                      // amplitude of the signal
	float m_fTDC;                      // Constant fraction discriminator time

	int m_nStationID;
	int m_nDetectorID;
	int m_nSensitiveElementID;
};


#endif //AFP_TDDigi_p1_h


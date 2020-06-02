/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

	float m_fADC = 0;                      // amplitude of the signal
	float m_fTDC = 0;                      // Constant fraction discriminator time

	int m_nStationID = 0;
	int m_nDetectorID = 0;
        int m_nSensitiveElementID = 0;


};


#endif //AFP_TDDigi_p1_h


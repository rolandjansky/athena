/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SiDigi_p1_h
#define AFP_SiDigi_p1_h

class AFP_SiDigi_p1
{
 public:
	// needed by athenaRoot 
	AFP_SiDigi_p1(){};
	friend class AFP_SiDigiCnv_p1;

private:

        float m_fADC = 0;                      // amplitude of the signal for given pixel
	float m_fTDC = 0;                      // Constant fraction discriminator time for given pixel

	int m_nStationID = 0;
	int m_nDetectorID = 0;
	int m_nPixelRow = 0;
	int m_nPixelCol = 0;

};


#endif //AFP_SiDigi_p1_h


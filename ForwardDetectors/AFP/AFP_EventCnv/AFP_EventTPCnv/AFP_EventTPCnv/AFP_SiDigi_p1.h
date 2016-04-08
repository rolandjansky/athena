/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
	float m_fADC;                      // amplitude of the signal for given pixel
	float m_fTDC;                      // Constant fraction discriminator time for given pixel

	int m_nStationID;
	int m_nDetectorID;
	int m_nPixelRow;
	int m_nPixelCol;
};


#endif //AFP_SiDigi_p1_h


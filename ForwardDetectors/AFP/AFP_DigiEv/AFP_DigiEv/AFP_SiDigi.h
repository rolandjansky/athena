/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SiDigi_h
#define AFP_SiDigi_h

class AFP_SiDigi
{
 public:
	AFP_SiDigi();
	virtual ~AFP_SiDigi();

public:
	float m_fADC;                          // amplitude of the signal for given pixel
	float m_fTDC;                          // Constant fraction discriminator time for given pixel

	int m_nStationID;
	int m_nDetectorID;
	int m_nPixelRow;
	int m_nPixelCol;
};


inline float getTDC(const AFP_SiDigi& digi)
{
	return digi.m_fTDC;
}
inline float getADC(const AFP_SiDigi& digi)
{
        return digi.m_fADC;    
}


#endif //AFP_SiDigi_h

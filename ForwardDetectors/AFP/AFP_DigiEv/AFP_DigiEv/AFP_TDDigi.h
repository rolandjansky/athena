/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigi_h
#define AFP_TDDigi_h

class AFP_TDDigi
{
 public:
	AFP_TDDigi();
	virtual ~AFP_TDDigi();

public:
	float m_fADC;                          // amplitude of the signal
	float m_fTDC;                          // Constant fraction discriminator time

	int m_nStationID;
	int m_nDetectorID;
	int m_nSensitiveElementID;
};


inline float getTDC(const AFP_TDDigi& digi)
{
	return digi.m_fTDC;
}
inline float getADC(const AFP_TDDigi& digi)
{
        return digi.m_fADC;    
}


#endif //AFP_TDDigi_h

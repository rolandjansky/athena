/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_DigiEv/AFP_SiDigi.h"

AFP_SiDigi::AFP_SiDigi( )
{
	m_fADC=0.0;
	m_fTDC=0.0;
	m_nStationID=-1;
	m_nDetectorID=-1;
	m_nPixelRow = -1;
	m_nPixelCol = -1;
}

AFP_SiDigi::~AFP_SiDigi()
{

}

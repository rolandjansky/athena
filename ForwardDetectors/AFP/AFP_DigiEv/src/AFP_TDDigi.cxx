/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_DigiEv/AFP_TDDigi.h"

AFP_TDDigi::AFP_TDDigi( )
{
	m_fADC=0.0;
	m_fTDC=0.0;
	m_nStationID=-1;
	m_nDetectorID=-1;
	m_nSensitiveElementID=0;
}

AFP_TDDigi::~AFP_TDDigi()
{

}


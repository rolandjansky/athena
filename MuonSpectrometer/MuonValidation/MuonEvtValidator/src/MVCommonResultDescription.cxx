/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Name    : MVCommonResultDescription.h
// Package : MuonValidation
// Author: M.Schott (MuonValidation-Team)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class is for describing the mean-values over a larger amount of chambers
///////////////////////////////////////////////////////////////////////////////////////////////////////
  

#include "MuonEvtValidator/MVCommonResultDescription.h"
#include <cmath>

/// Constructor
MVCommonResultDescription::MVCommonResultDescription(	int start,	/// Start-Value of Interval
							int end)	/// End-Value of Interval
{
	m_start=start;
	m_end=end;
	/// Clearing all Variables
	for (int i=m_start; i<=m_end; i++)
	{
		m_map_values[i]=0.0;
		m_map_entries[i]=0;
		m_map_pass[i]=0;
		m_map_total[i]=0;
	}
}

void  MVCommonResultDescription::clear()
{
	for (int i=m_start; i<=m_end; i++)
	{
		m_map_values[i]=0.0;
		m_map_entries[i]=0;
		m_map_pass[i]=0;
		m_map_total[i]=0;
	}
}

/// Adding Mean-Information to id-bin 
/*void MVCommonResultDescription::add(long id, float mean)
{
	if ((id>=m_start) && (id<=m_end))
	{
		m_map_values[id]+=mean;
		m_map_entries[id]++;
	}
}*/

/// Adding Number of Pass and Number of Total Events to id-bin
void MVCommonResultDescription::add(long id, int pass, int total)
{
	if ((id>=m_start) && (id<=m_end))
	{
		m_map_pass[id]+=pass;
		m_map_total[id]+=total;
	}
}

/// Gives back the Ratio between pass-Events and Total-Events for given id
float MVCommonResultDescription::getMean(int id)
{
	/*if ((id<m_start) || (id>m_end)) return 0.0;
	if (m_map_entries[id]==0.0) return 0.0;
	return m_map_values[id]/((float)(m_map_entries[id]));*/
	if ((id<m_start) || (id>m_end)) return 0.0;
	if (m_map_total[id]==0.0) return 0.0;
	return m_map_pass[id]/((float)(m_map_total[id]));
}

/// Gives back the Ratio and the Error of the Ratio between pass-Events and Total-Events for given id
void MVCommonResultDescription::getMean(int id, float &mean, float &error)
{
	mean=0.0;
	error=0.0;
	if ((id<m_start) || (id>m_end)) return;
	if (m_map_total[id]==0) return;
	mean	= ((float)(m_map_pass[id]))/((float)(m_map_total[id]));
	error	= 	std::pow(sqrt((float)(m_map_pass[id]))/((float)(m_map_total[id])),2) + 
			std::pow((float)(m_map_pass[id]),2)/std::pow((float)(m_map_total[id]),3);
	error	= sqrt(error);
}

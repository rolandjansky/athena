/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
// Name    : MVCommonResultDescription.h
// Package : MuonValidation
// Author  : M.Schott (LMU)
// CERN, MAY 1, 2006
//
// DESCRIPTION:
//
// This class is for describing the mean-values over a larger amount of chambers
//
/////////////////////////////////////////////////////////////////////
   

#include <TFile.h>
#include <TF1.h>
#include <TH2.h>
#include <TChain.h>
#include <map>

class MVCommonResultDescription
{
	public:
		/// Constructor: Defining start and end of interval
		MVCommonResultDescription(int begin, int end);

		/// Adding Mean-Information to id-bin 
//		void add(long id, float mean);

		/// Adding Number of Pass and Number of Total Events to id-bin
		void add(long id, int pass, int total);

		/// Gives back the Ratio between pass-Events and Total-Events for given id
		float getMean(int id);

		/// Gives back the Ratio and the Error of the Ratio between pass-Events and Total-Events for given id
		void  getMean(int id, float &mean, float &error);

		void  clear();

	private:
		long m_start, m_end;
		std::map<long, float>	m_map_values;
		std::map<long, long>	m_map_entries;
		std::map<long, long>	m_map_pass;
		std::map<long, long>	m_map_total;
};

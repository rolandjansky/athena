/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/AveragePrint.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TH1.h>
#include <TProfile.h>
#include <TClass.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/Result.h"

static dqm_algorithms::AveragePrint staticInstance;

namespace dqm_algorithms {
	
	// *********************************************************************
	// Public Methods
	// *********************************************************************
	
	void
	AveragePrint::
	printDescription(std::ostream& out)
	{
		std::string message;
		message += "\n";
		message += "Algorithm: \"" + m_name + "\"\n";
		message += "Description: Prints out the average of the histogram or profile bins\n";
		message += "             In the case of a TProfile each bin is weighted by its fraction of the entries\n";
		message += "             Overflow (and Underflow) bins are not included\n";
		message += "\n";
		
		out << message;
	}
	
	AveragePrint::
	AveragePrint()
    : m_name("AveragePrint")
	{
		dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
	}
	
	
	AveragePrint::
	~AveragePrint()
	{
	}
	
	
	dqm_core::Algorithm*
	AveragePrint::
	clone()
	{
		return new AveragePrint(*this);
	}
	
	
	dqm_core::Result*
	AveragePrint::
	execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& /*config*/)
	{
		//No status flags are set
		dqm_core::Result* result = new dqm_core::Result();
		result->status_ = dqm_core::Result::Undefined;
		
		if (!data.IsA()->InheritsFrom("TH1")) {
			throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
		}
		TH1* h = (TH1*)&data; // h cannot be null
		if (h->GetDimension() > 2) { 
			throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 2 ");
		}
		
		//**********
		// Profile case
		//**********
		if ( data.IsA()->InheritsFrom("TProfile") ) {
			TProfile* hp = (TProfile*)&data;
			//ASSUME: dimension = 1
			double Average_value = 0.;
			double Total_entries = 0;
			int NbinsX = h->GetNbinsX();
			for(int binX = 0; binX < NbinsX; binX++) {
				double Bin_entries = hp->GetBinEntries(binX + 1);
				double Bin_value = Bin_entries * (hp->GetBinContent(binX + 1));
				Total_entries += Bin_entries;
				Average_value += Bin_value;
			}
			Average_value = Average_value / Total_entries;
			std::string Average_name = Form("%s_Average", name.c_str());
			result->tags_[Average_name.c_str()] = Average_value;
		}
		
		//**********
		// 1D Histogram case
		//**********
		if((! data.IsA()->InheritsFrom("TProfile")) && h->GetDimension() == 1) {
			double Average_value = 0.;
			int NbinsX = h->GetNbinsX();
			for(int binX = 0; binX < NbinsX; binX++) {
				Average_value += h->GetBinContent(binX + 1);
			}
			std::string Average_name = Form("%s_Average", name.c_str());
			result->tags_[Average_name.c_str()] = Average_value;
		}
		
		//**********
		// 2D Histogram case
		//**********
		if((! data.IsA()->InheritsFrom("TProfile")) && h->GetDimension() == 2) {
			double Average_value = 0.;
			int NbinsX = h->GetNbinsX();
			int NbinsY = h->GetNbinsY();
			for(int binX = 0; binX < NbinsX; binX++) {
				for(int binY = 0; binY < NbinsY; binY++) {
					Average_value += h->GetBinContent(binY + 1, binY + 1);
				}
			}
			std::string Average_name = Form("%s_Average", name.c_str());
			result->tags_[Average_name.c_str()] = Average_value;
		}
		
		return result;
	}
	
}

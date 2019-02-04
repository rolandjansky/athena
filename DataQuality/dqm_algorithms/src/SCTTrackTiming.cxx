/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_algorithms/SCTTrackTiming.h"

#include <cmath>
#include <iostream>
#include <map>

#include <TH1.h>
#include <TClass.h>

#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/Result.h"

static dqm_algorithms::SCTTrackTiming staticInstance;

namespace dqm_algorithms {
	
	// *********************************************************************
	// Public Methods
	// *********************************************************************
	
	void
	SCTTrackTiming::
	printDescription(std::ostream& out)
	{
		std::string message;
		message += "\n";
		message += "Algorithm: \"" + m_name + "\"\n";
		message += "Description: For use with SCT Track Time Bins ONLY!\n";
		message += "             Prints the ratio of 010 : 01X hits.\n";
		message += "             Prints the mean hit time, with 100 as -1 BunchCrossing (early) and 001 as +1 BunchCrossing (late)\n";
		message += "             where 011 would contribute half a hit to 0 BC (timed in) and half a hit to +1 BC (late)\n";
		message += "             and the hit patterns 000, 111, and 101 will be ignored.\n";
		message += "\n";
		
		out << message;
	}
	
	SCTTrackTiming::
	SCTTrackTiming()
    : m_name("SCTTrackTiming")
    , m_NbinsX(8)
	{
		dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name, this );
	}
	
	
	dqm_core::Algorithm*
	SCTTrackTiming::
	clone()
	{
		return new SCTTrackTiming(*this);
	}
	
	
	dqm_core::Result*
	SCTTrackTiming::
	execute( const std::string& name, const TObject& data, const dqm_core::AlgorithmConfig& /* config */)
	{
		//No status flags are set
		dqm_core::Result* result = new dqm_core::Result();
		result->status_ = dqm_core::Result::Undefined;
		
		TH1* h;
		if(data.IsA()->InheritsFrom( "TH1" )) {
			h = (TH1*)&data;
			if (h->GetDimension() > 1 ) { 
				throw dqm_core::BadConfig( ERS_HERE, name, "Not SCT Time Bins: Hist. Dimension > 1 " );
			}
			if (h->GetNbinsX() != 8) {
				throw dqm_core::BadConfig( ERS_HERE, name, "Not SCT Time Bins: Hist. NBins != 8 " );
			}
		} else {
			throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1");
		}
		
		//Histogram is assumed to be SCT Time Bins.
		m_NbinsX = 8;
		
		//Get the ratio & average for hit times
		double Hits010 = 0.;
		double Hits011 = 0.;
		double EarlyHits = 0.;
		double InTimeHits = 0.;
		double LateHits = 0.;
		double AllEntries = 0.;
		
		double TBin = 0.;
		
		TBin = h->GetBinContent(0+1); //000
		AllEntries += TBin;
		
		TBin = h->GetBinContent(1+1); //001
		LateHits += TBin;
		AllEntries += TBin;
		
		TBin = h->GetBinContent(2+1); //010
		Hits010 += TBin;
		InTimeHits += TBin;
		AllEntries += TBin;
		
		TBin = h->GetBinContent(3+1); //011
		Hits011 += TBin;
		InTimeHits += TBin/2;
		LateHits += TBin/2;
		AllEntries += TBin;
		
		TBin = h->GetBinContent(4+1); //100
		EarlyHits += TBin;
		AllEntries += TBin;
		
		TBin = h->GetBinContent(5+1); //101
		AllEntries += TBin;
		
		TBin = h->GetBinContent(6+1); //110
		EarlyHits += TBin/2;
		InTimeHits += TBin/2;
		AllEntries += TBin;
		
		TBin = h->GetBinContent(7+1); //111
		AllEntries += TBin;
		
		//NOTE: If some quantity cannot be calculated it should not be printed at all.
		//Printing a default value will simply skew the scale of history plots.
		
		if (Hits010 + Hits011 > 0.) { //sufficient entries
			double HitBin_ratio = Hits010 / (Hits010 + Hits011);
			std::string HitBin_ratio_name = Form("%s_Ratio_010_over_01X", name.c_str());
			result->tags_[HitBin_ratio_name.c_str()] = HitBin_ratio;
		}
		
		if (AllEntries > 0.) { //sufficient entries
			double Edge0_ratio = Hits010 / AllEntries;
			std::string Edge0_name = Form("%s_Ratio_010_over_XXX", name.c_str());
			result->tags_[Edge0_name.c_str()] = Edge0_ratio;
		}
		
		if (AllEntries > 0.) { //sufficient entries
			double Edge1_ratio = Hits011 / AllEntries;
			std::string Edge1_name = Form("%s_Ratio_011_over_XXX", name.c_str());
			result->tags_[Edge1_name.c_str()] = Edge1_ratio;
		}
		
		if (AllEntries > 0.) { //sufficient entries
			double EdgeX_ratio = (Hits010 + Hits011) / AllEntries;
			std::string EdgeX_name = Form("%s_Ratio_01X_over_XXX", name.c_str());
			result->tags_[EdgeX_name.c_str()] = EdgeX_ratio;
		}
		
		if (LateHits + InTimeHits + EarlyHits > 0.) { //sufficient entries
			double HitMean_time = (LateHits - EarlyHits) / (LateHits + InTimeHits + EarlyHits);
			std::string HitMean_name = Form("%s_MeanHitTimeBC", name.c_str());
			result->tags_[HitMean_name.c_str()] = HitMean_time;
			
			double HitWidth_time = (LateHits + EarlyHits) / (LateHits + InTimeHits + EarlyHits);
			HitWidth_time = std::sqrt(HitWidth_time - HitMean_time*HitMean_time);
			std::string HitWidth_name = Form("%s_VarianceHitTimeBC", name.c_str());
			result->tags_[HitWidth_name.c_str()] = HitWidth_time;
		}
		
		return result;
	}
	
}

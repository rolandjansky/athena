/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Author:   M.Schott(LMU)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: This class does an overall Muon-Reconstruction Analysis.
// It provides Plots for Resolution, Efficiency, Fake-Rate, Pull-Distribution etc
// Moreover it is possible to compare different Algorithms directly with each other
// Two Modes are possible:
// - VersionComparingMode: In this mode one Reconstruction Algorithm is compared for two different
//   input-files
// - Normal-Mode: Each Reconstruction Algorithms is compared to each other
///////////////////////////////////////////////////////////////////////////


#ifndef MSMUONANALYSIS_H
#define MSMUONANALYSIS_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"
 
using namespace std;
// Defining Cuts for Single Mu-Analysis

struct cutMuonAnalDefinitons
 {
         double  TrigMu_Pt;
	 double  muonRec_Pt;
 };

class MSMuonAnalysis
{
	public :
		// Default Constructor - Empty
		MSMuonAnalysis();
		~MSMuonAnalysis();

		// Setting up the object for Version-Comparing Mode
		void	setup(	string outputfile,		// Output-Root-File
		                cutMuonAnalDefinitons cuts,     // Which Cuts for singleMuAnal should be used?
				double maxConeDistance,		// Maximal-Cone-Distance for assoziating a Track
				double minimalMuonPt,		// Minimal PT of Muons which should be considered
				string comparingTrackContainerName,	// Which Track-Container should be compared?
				string comparingTriggerContainerName,	// Which Trigger-Container should be compared?
				vector<float>	vEfficiencyRanges,	// Efficiency Ranges, e.g Eta from 2.5 to 2.5 with 10 Bins
				vector<float>	vResolutionRanges,	// Ranges of Resolution determination
				string versionName1,			// How should we call the first version
				string versionName2);			// How should we call the second version

		void	setup(	string outputfile,		// Output-Root-File
		                cutMuonAnalDefinitons cuts,     // Which Cuts for singleMuAnal should be used?
				double maxConeDistance,		// Maximal-Cone-Distance for assoziating a Track
				double minimalMuonPt,		// Minimal PT of Muons which should be considered
				vector<string>	vTrackContainerNames,	// Names of the TrackContainer which should be compared
				vector<string>	vTriggerContainerNames,	// Name of the Triggers which should be analysed
				vector<float>	vEfficiencyRanges,	// Efficiency Ranges, e.g Eta from 2.5 to 2.5 with 10 Bins
				vector<float>	vResolutionRanges,	// Ranges of Resolution determination
				bool            triggerAware);

		// These two methods do the whole comparing thing
		void	analyse(MSEvent &msEvent);	
		void	analyse_TwoVersions(MSEvent &msEvent, int version);

		void	writeResults();		// Writing root-results out

		MSEfficiency getEfficiencyDescription(std::string name)	
		{
			return mapEfficiency[name];
		}
		MSResolution getResolutionDescription(std::string name)	
		{
			return mapResolution[name];
		}

//		TH1F *Hist1;

	private:

		void	analyse_Truth(MSEvent &msEvent);
		void	analyse_EfficiencyAndResolution(MSEvent &msEvent, int n);
		void	analyse_FakeRate(MSEvent &msEvent, int n);
		void	analyse_ParticleProperties(MSEvent &msEvent, int n);

		void	analyse_Trigger(MSEvent &msEvent, int n);

		void	set_Track_Histograms(MSEfficiency &msEfficiency, vector<float>	vResolutionRanges, vector<float> vEfficiencyRanges);
		void	set_Trigger_Histograms(MSEfficiency &msEfficiency, vector<float> vEfficiencyRanges);

		int	getSector(double phi);

		int                     m_total, m_pass;
		double			m_PhiOffset;
		double			m_MaxConeDistance;
		double			m_MinimalMuonPt;
 		
		std::string		m_versionName1;
		std::string		m_versionName2;
		std::string		m_comparingTrackContainerName;
		std::string		m_comparingTriggerContainerName;
		std::string		m_Outputfile;		
		bool 	                m_TriggerFired;
		bool                    m_TriggerAware;
				
		MSRootInterface			msRootInterface;
		map<string,MSEfficiency>	mapEfficiency;
		map<string,MSResolution>	mapResolution;
                
		cutMuonAnalDefinitons           cutMu;
		
		vector<string>			vecTrackContainerNames;
		vector<string>			vecTriggerContainerNames;


};
#endif
 
 

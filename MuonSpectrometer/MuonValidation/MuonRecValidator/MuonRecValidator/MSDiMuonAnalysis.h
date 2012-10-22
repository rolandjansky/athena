/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
//
// DESCRIPTION:
// Subject: 	Class for doing the DiMuonResolution analysis.
///////////////////////////////////////////////////////////////////////////

#ifndef MSDiMuonAnalysis_H
#define MSDiMuonAnalysis_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
//#include "MuonInSituPerformance/MSPhysicsAnalysis.h"

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"

using namespace std;

class MSDiMuonAnalysis
{
	public :
		// Default Constructor - Empty
		MSDiMuonAnalysis();
		~MSDiMuonAnalysis();

		void	setup	(string outputfile,
				double maxConeDistance,
				double minimalMuonPt,
				string comparingTrackContainerName,
				string versionName1,
				string versionName2,
				vector<float>	vResolutionRangesPt,
				vector<float>	vResolutionRangesEta);

		void	setup	(string outputfile,
				double maxConeDistance,
				double minimalMuonPt,
				vector<string>	vTrackContainerNames,
				vector<float>	vResolutionRangesPt,
				vector<float>	vResolutionRangesEta);


		void	analyse(MSEvent &msEvent);
		void	analyse_TwoVersions(MSEvent &msEvent, int version);
		
// Calculating the invariant Mass from two muons
		double	getDiMuonMass(MSPhysicsObject &muon1, MSPhysicsObject &muon2);
				
		void	writeResults();

	private:
		void	analyse_truth(MSEvent &msEvent);
		void	analyse_dimuons(MSEvent &msEvent, int n);
		void	set_Track_Histograms(vector<float> vResolutionRangesPt, vector<float> vResolutionRangesEta);

		std::string		m_Outputfile;
		double			m_MaxConeDistance;
		double			m_MinimalMuonPt;
		std::string		m_versionName1;
		std::string		m_versionName2;
		std::string		m_comparingTrackContainerName;
		int			m_PDG;


		MSTrackObject		True_Z_Boson;
		MSTrackObject		True_Z_Muon1;
		MSTrackObject		True_Z_Muon2;
		
		MSRootInterface			msRootInterface;
		vector<string>			vecTrackContainerNames;
		map<string,MSResolution>	mapResolution;
};
#endif
 
 

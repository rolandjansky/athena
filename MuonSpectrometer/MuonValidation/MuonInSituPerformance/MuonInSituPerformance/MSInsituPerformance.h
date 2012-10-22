/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MSPhysicsAnalysis
///////////////////////////////////////////////////////////////////////////


#ifndef MSINSITUPERFORMANCE_H
#define MSINSITUPERFORMANCE_H

#include "MuonInSituPerformance/MSPhysicsAnalysis.h"

#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"
#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSTrackObject.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>
#include <TF1.h>
#include <TH2.h>
#include <TFile.h>
#include <TChain.h>
#include <TGraph.h>
#include <TGraphErrors.h>


class MSInsituPerformance : public MSPhysicsAnalysis
{
	public :
		// Default Constructor - Empty
		MSInsituPerformance();
		~MSInsituPerformance();

		void setup(	std::string outputname,
				std::string RootDirName,
				cutDefinitons cuts,
				std::string InnerTrackContainerName,	        // Which Inner Track Container should be used
				std::string MuonSpectrometerTrackContainerName,	// Which MuonSpec Container should be used
				std::string MuonContainerName,                  // Which Muon Container should be used
				std::vector<string> TriggerContainerName);	// Which Trigger Container should be used

		bool	analyseMuonSpectrometerEfficiencies(MSEvent &msEvent,double Luminosity); // Which integrated Luminosity has been reached with this event
		bool	analyseInnerTrackerEfficiency(MSEvent &msEvent);
		bool    analyseMuonSpectrometerTriggerEfficienciesDO(MSEvent &msEvent,
								     double Luminosity,       // Which integrated Luminosity has been reached with this event);
								     std::string trigger_item,   // trigger item under test (e.g. "EF_mu20i")
								     unsigned int trigger_level);  // trigger level under test (e.g. TriggerDecision::EF)	

		void	writeResults();

	private:
		void	getAveragePrecisionForSections(double &MeanEfficiency, double &MeanPrecision, double &LimitPrecision);

		std::string		m_InnerTrackContainer;
		std::string		m_MuonSpectrometerTrackContainer;
                std::string             m_MuonContainer;

		vector<string>			vecEfficiencyNames;
		std::vector<string>		m_TriggerContainer;
		
		map<string, MSEfficiency>	mapEfficiency;
		
		MSTrackObject		Tag_Muon;
		MSTrackObject		Probe_Muon;
		MSTrackObject		Track1;
		MSTrackObject		Track2;
		MSTrackObject		Track;

};

#endif
 

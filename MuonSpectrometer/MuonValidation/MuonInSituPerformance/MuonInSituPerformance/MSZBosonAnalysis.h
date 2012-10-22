/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on MSEvent.
//		It is derivated from MSPhysicsAnalysis
///////////////////////////////////////////////////////////////////////////

#ifndef MSZBOSONANALYSIS_H
#define MSZBOSONANALYSIS_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>

#include "MuonInSituPerformance/MSPhysicsAnalysis.h"

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSTrackObject.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"

class MSZBosonAnalysis : public MSPhysicsAnalysis
{
	public :
		// Default Constructor - Empty
		MSZBosonAnalysis();
		~MSZBosonAnalysis();

		void setup(	std::string outputname,
				std::string RootDirName,
				cutDefinitons cuts,
				std::string MuonContainerName);	   // Which Muon Container should be used
		void	analyse(MSEvent &msEv);
		void	writeResults();

	private:
		void	analyseMCTruth(MSEvent &msEv);
		void	setupHistograms();

		std::string m_MuonContainer; 

		MSTrackObject		Rec_Muon1;
		MSTrackObject		Rec_Muon2;
		MSTrackObject		Rec_ZMuon1;
		MSTrackObject		Rec_ZMuon2;
		MSTrackObject		True_ZMuon1;
		MSTrackObject		True_ZMuon2;
		MSTrackObject		True_ZBoson;
};

#endif
 

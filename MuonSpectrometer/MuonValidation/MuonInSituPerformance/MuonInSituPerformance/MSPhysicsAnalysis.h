/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
// Subject: 	This class is the basis for all Physics-Analysis-Classes
//		which are based on MSEvent.
//		It contains functions like diMuonMass-calculation,...
///////////////////////////////////////////////////////////////////////////

#ifndef MSPhysicsAnalysis_H
#define MSPhysicsAnalysis_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>

#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSTrackObject.h"
#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"

// Defining Cuts for Zmumu-Analysis
struct cutDefinitons
{
	double	Pt_loose;
	double	Pt_tight;
	double  TrigMu_Pt;
	double	diMuonMass;
	double  ZInvmMassWnd;
	double	deltaPhi;
	double	Jet_InHalo_Loose;
	double	NTracks_InHalo_Loose;
	double	PTTracks_InHalo_Loose;
	double	ET_InHalo_Loose;
	double	Jet_InHalo_Tight;
	double	NTracks_InHalo_Tight;
	double	PTTracks_InHalo_Tight;
	double	ET_InHalo_Tight;
};

class MSPhysicsAnalysis
{
	public :
		// Default Constructor - Empty
		MSPhysicsAnalysis();
		~MSPhysicsAnalysis();

		void	setup(	std::string outputname,		// Name of the Root-File
				std::string RootDirName,	// Directory within the root-File where results should be written
				cutDefinitons cuts		// Which Cuts should be used?
				);

	protected:
		// Getting the Isolation-Value for a muon. It can be:	0:Not isolated, 1: Loose Isolated, 2:Tight Isolated, 3: Very Tight isolated
		int	getIsolation(MSPhysicsObject &muon);
		
		// Calculating the invariant Mass from two muons
		double	getDiMuonMass(MSPhysicsObject &muon1, MSPhysicsObject &muon2);
		
		// Calculation the invariant PT
		double	getDiMuonPt(MSPhysicsObject &muon1, MSPhysicsObject &muon2);
		
		// Calculation the DeltaPhi of two muons.
		double	getDeltaPhi(MSPhysicsObject &muon1, MSPhysicsObject &muon2);

		double	dRand(double min, double max);

		std::string		m_Outputfile;
		std::string		m_RootDirName;

		cutDefinitons		cutD;
		MSEfficiency		msEfficiency;
		MSRootInterface		msRootInterface;
};

#endif
 

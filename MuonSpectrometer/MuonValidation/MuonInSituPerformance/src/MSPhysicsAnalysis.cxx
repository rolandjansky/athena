/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class is the basis for all Physics-Analysis-Classes
//		which are based on MSEvent.
//		It contains functions like diMuonMass-calculation,...
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonInSituPerformance/MSPhysicsAnalysis.h"


// Default Constructor - Empty
MSPhysicsAnalysis::MSPhysicsAnalysis()
{
  cutD.Pt_loose	= 15000.;  // in MeV
  cutD.Pt_tight	= 20000.;  // in MeV
  cutD.TrigMu_Pt = 20000.; // in MeV

  cutD.diMuonMass	= 30000.; // in MeV
  cutD.ZInvmMassWnd     = 12000.; // in MeV
  cutD.deltaPhi	        = 0.1;    // the distance in phi space between two items
  
// Jet(loose identified) Energy, NTrack, PTTracks, ET in Halo around Muon [GeV]
  cutD.Jet_InHalo_Loose		= 25000.;
  cutD.NTracks_InHalo_Loose	= 6.;
  cutD.PTTracks_InHalo_Loose	= 15000.;
  cutD.ET_InHalo_Loose		= 20000.;
  
// Jet(tight identified) Energy, NTrack, PTTracks, ET in Halo around Muon [GeV]
  cutD.Jet_InHalo_Tight		= 15000.;
  cutD.NTracks_InHalo_Tight	= 4.;
  cutD.PTTracks_InHalo_Tight	= 8000.;
  cutD.ET_InHalo_Tight		= 6000.;
}


MSPhysicsAnalysis::~MSPhysicsAnalysis()
{
}

void	MSPhysicsAnalysis::setup(std::string outputname, std::string dirname, cutDefinitons cuts)
{
  cutD		= cuts;
  m_Outputfile	= outputname;
  m_RootDirName	= dirname;
  msRootInterface.createDirectory(m_Outputfile, m_RootDirName);
}


double	MSPhysicsAnalysis::getDiMuonMass(MSPhysicsObject &muon1, MSPhysicsObject &muon2)
{
  double tpx=0.0;
  double tpy=0.0;
  double tpz=0.0;
  double tee=0.0;
  double En1, En2;
  En1 = sqrt(pow((double)fabs(muon1.Pt) * cosh((double)muon1.Eta),2.) + pow((double)muon1.AnalysisParameters.dMass,2.));
  En2 = sqrt(pow((double)fabs(muon2.Pt) * cosh((double)muon2.Eta),2.) + pow((double)muon2.AnalysisParameters.dMass,2.));
  tpx = muon1.Px() + muon2.Px();
  tpy = muon1.Py() + muon2.Py();
  tpz = muon1.Pz() + muon2.Pz();
  tee = En1+En2;
  return sqrt(tee*tee - tpx*tpx - tpy*tpy - tpz*tpz);
}

double	MSPhysicsAnalysis::getDiMuonPt(MSPhysicsObject &muon1, MSPhysicsObject &muon2)
{
  double tpx = muon1.Px() + muon2.Px();
  double tpy = muon1.Py() + muon2.Py();
  return sqrt(tpx*tpx + tpy*tpy);
}


double	MSPhysicsAnalysis::getDeltaPhi(MSPhysicsObject &muon1, MSPhysicsObject &muon2)
{
  double p1, p2, dp;
  p1=muon1.Phi;
  p2=muon2.Phi;

  dp=fabs(p2-p1);
  if (dp>3.1415) return fabs(2.0*3.1415-dp); else return dp;
}

int	MSPhysicsAnalysis::getIsolation(MSPhysicsObject &muon)
{
  int iso=0;
  int loose=0;
  int tight=0;
  if (muon.AnalysisParameters.dJetIsolation < cutD.Jet_InHalo_Loose) 		loose++;
  if (muon.AnalysisParameters.dNTrackConeIsolation < cutD.NTracks_InHalo_Loose)	loose++;
  if (muon.AnalysisParameters.dTrackPtConeIsolation < cutD.PTTracks_InHalo_Loose)	loose++;
  if (muon.AnalysisParameters.dETIsolation < cutD.ET_InHalo_Loose)			loose++;

  if (loose>=3)
    {
      iso=1;
      if (muon.AnalysisParameters.dJetIsolation < cutD.Jet_InHalo_Tight) 		tight++;
      if (muon.AnalysisParameters.dNTrackConeIsolation < cutD.NTracks_InHalo_Tight)	tight++;
      if (muon.AnalysisParameters.dTrackPtConeIsolation < cutD.PTTracks_InHalo_Tight)	tight++;
      if (tight==3) iso=2;
      if (muon.AnalysisParameters.dETIsolation < cutD.ET_InHalo_Loose)			tight++;
      if (tight==4) iso=3;
    }
  return iso;
}

double	MSPhysicsAnalysis::dRand(double min, double max)
{
  return (((max-min)*((double)(rand()))/((double)(RAND_MAX)))+min);
}

 

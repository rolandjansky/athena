/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI) - M. Bellomo (INFN)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MSPhysicsAnalysis
///////////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>
#include <TF1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>

#include "MuonInSituPerformance/MSPhysicsAnalysis.h"
#include "MuonInSituPerformance/MSInsituPerformance.h"

#include "MuonValUtils/MSEfficiency.h"
#include "MuonValUtils/MSResolution.h"
#include "MuonValUtils/MSRootInterface.h"
#include "MuonValUtils/MSEvent.h"
#include "MuonValUtils/MSTrackObject.h"


// Default Constructor - Empty
MSInsituPerformance::MSInsituPerformance()
{
  m_InnerTrackContainer		   = "";
  m_MuonSpectrometerTrackContainer = "";
  //m_TriggerContainer		   = "";
  m_MuonContainer                  = "";
}

MSInsituPerformance::~MSInsituPerformance()
{
}

void MSInsituPerformance::setup(std::string outputname,
				std::string RootDirName,
				cutDefinitons cuts,
				std::string InnerTrackContainerName,
				std::string MuonSpectrometerTrackContainerName,
                                std::string MuonContainerName,
				std::vector<string> TriggerContainerName)
{
  MSPhysicsAnalysis::setup(outputname, RootDirName, cuts);

  vector<double> bins1, bins2, bins3;

  bins1.push_back(15000.);
  bins1.push_back(20000.);
  bins1.push_back(25000.);
  bins1.push_back(30000.);
  bins1.push_back(35000.);
  bins1.push_back(40000.);
  bins1.push_back(45000.);
  bins1.push_back(50000.);
  bins1.push_back(55000.);
  bins1.push_back(60000.);
  bins1.push_back(65000.);
  bins1.push_back(70000.);
  bins1.push_back(75000.);

  bins2.push_back(-2.50);
  bins2.push_back(-2.00);
  bins2.push_back(-1.60);
  bins2.push_back(-1.30);
  bins2.push_back(-1.05);
  bins2.push_back(-0.90);
  bins2.push_back(-0.75);
  bins2.push_back(-0.60);
  bins2.push_back(-0.45);
  bins2.push_back(-0.25);
  bins2.push_back(0.0);
  bins2.push_back(0.25);
  bins2.push_back(0.45);
  bins2.push_back(0.60);
  bins2.push_back(0.75);
  bins2.push_back(0.90);
  bins2.push_back(1.30);
  bins2.push_back(1.60);
  bins2.push_back(2.00);
  bins2.push_back(2.50);

  bins3.push_back(0.00  * 0.0174533);
  bins3.push_back(30.0  * 0.0174533);
  bins3.push_back(45.0  * 0.0174533);
  bins3.push_back(75.0  * 0.0174533);
  bins3.push_back(90.0  * 0.0174533);
  bins3.push_back(120.0 * 0.0174533);
  bins3.push_back(135.0 * 0.0174533);
  bins3.push_back(165.0 * 0.0174533);
  bins3.push_back(180.0 * 0.0174533);
  bins3.push_back(210.0 * 0.0174533);
  bins3.push_back(225.0 * 0.0174533);
  bins3.push_back(255.0 * 0.0174533);
  bins3.push_back(270.0 * 0.0174533);
  bins3.push_back(300.0 * 0.0174533);
  bins3.push_back(315.0 * 0.0174533);
  bins3.push_back(345.0 * 0.0174533);
  bins3.push_back(360.0 * 0.0174533);

  MSEfficiency msEffDummy;
  msEffDummy.setupDim(1, bins1);
  msEffDummy.setupDim(2, bins2);
  msEffDummy.setupDim(3, bins3);

  m_InnerTrackContainer		   = InnerTrackContainerName;
  m_MuonSpectrometerTrackContainer = MuonSpectrometerTrackContainerName;
  m_TriggerContainer		   = TriggerContainerName;
  m_MuonContainer                  = MuonContainerName;
  
  msRootInterface.createDirectory(m_Outputfile,m_RootDirName,"TagAndProbe_Cut_Histograms");
  msRootInterface.createDirectory(m_Outputfile,m_RootDirName,"DO_Histograms");
  msRootInterface.createDirectory(m_Outputfile,m_RootDirName,"Detailed_Information");
  msRootInterface.setDirectory(m_Outputfile,"m_RootDirName");

  vecEfficiencyNames.clear();
  vecEfficiencyNames.push_back("MuonReconstruction");

  for(int i=0; i<(int)m_TriggerContainer.size(); i++)
    vecEfficiencyNames.push_back(m_TriggerContainer[i]);

  vecEfficiencyNames.push_back("InnerTracking");

  for (int i=0; i<(int)vecEfficiencyNames.size(); i++)
    {

      mapEfficiency[vecEfficiencyNames[i]] = msEffDummy;

      msRootInterface.setNewHistogram("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Overall",
				      vecEfficiencyNames[i]+"_Efficiency_Overall_TagProbe",
				      vecEfficiencyNames[i]+" TagAndProbe",
				      "Efficiency",
				      3,
				      -1.
				      ,1.);
		
      msRootInterface.setNewHistogram("TagAndProbe_"+vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall",
				      vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall_TagProbe",
				      vecEfficiencyNames[i]+" TagAndProbe",
				      "Efficiency Uncertainty",
				      3,
				      -1.
				      ,1.);

      // ROOT Graphs
      // msRootInterface.setNewGraph(id, name, Xaxis, Yaxis);
      msRootInterface.setNewGraph("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Eta",vecEfficiencyNames[i]+"_Efficiency_eta_TagProbe","#eta","Efficiency");				   
      msRootInterface.setNewGraph("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Phi",vecEfficiencyNames[i]+"_Efficiency_phi_TagProbe","#phi","Efficiency");
      msRootInterface.setNewGraph("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Pt",vecEfficiencyNames[i]+"_Efficiency_Pt_TagProbe","P_{T}","Efficiency");	
      msRootInterface.setNewGraph("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_vs_Lumi",vecEfficiencyNames[i]+"_Efficiency_vs_Luminosity","Luminosity","Efficiency");
      msRootInterface.setNewGraph("TagAndProbe_"+vecEfficiencyNames[i]+"_Precision_vs_Lumi",vecEfficiencyNames[i]+"_Precision_vs_Luminosity","Luminosity","Precision");

      ////////////////////////////////
      // Histogram version of graphs
      ////////////////////////////////
      msRootInterface.setNewHistogram("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Eta_h","TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Eta_h","#eta","Efficiency",bins2);
      msRootInterface.setNewHistogram("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Phi_h","TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Phi_h","#phi","Efficiency",bins3);
      msRootInterface.setNewHistogram("TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Pt_h","TagAndProbe_"+vecEfficiencyNames[i]+"_Efficiency_Pt_h","P_{T}","Efficiency",bins1);
	
    } //for vecEfficiencyNames loop
  
  msRootInterface.setNewGraph("TagAndProbe_Overall_Efficiency_vs_Lumi","MuonReconstruction_Overall_Efficiency_vs_Luminosity","Luminosity","Efficiency");
  msRootInterface.setNewGraph("TagAndProbe_Overall_Precision_vs_Lumi","MuonReconstruction_Overall_Precision_vs_Luminosity","Luminosity","Precision");
  msRootInterface.setNewGraph("TagAndProbe_MuonReconstruction_LimitPrecision_vs_Lumi","MuonReconstruction_80_Precision_vs_Luminosity","Luminosity","80% Limit Precision");
 
  //setNewHistrogram(string id, string name, string xAxis, string yAxis, int bins, float start, float end);
  msRootInterface.setNewHistogram(	"TagAndProbe_Cut_Flow",
					"TagProbe_Cut_Flow",
					"CutFlow (1:Charge, 2:Mass, 3:Kinematic Cuts, 4:iso, 5:No Electron, 6:MS Track)",
					"Number of Entries",7,0,7);

  for(int i=0; i<(int)m_TriggerContainer.size(); i++) 
    {
    
      msRootInterface.setNewHistogram(	"DO_Cut_Flow_"+m_TriggerContainer[i],
					"DO_Cut_Flow_"+m_TriggerContainer[i],
					"CutFlow (1:Charge, 2:Mass, 3:Kinematic Cuts, 4:iso, 5:No Electron, 6:Single Trigger, 7:Double Trigger)",
					"Number of Entries",8,0,8);
    } // for m_TriggerContainer loop

  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_DiMuonMass","TagAndProbe_MuonSpec_Inner_DiMuonMass", "DiMuonMass [GeV]","Number of Entries", 100, 0.0, 120000.);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_DeltaPhi","TagAndProbe_MuonSpec_Inner_DeltaPhi", "#Delta #phi","Number of Entries", 32, 0.0, 3.2);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_JetEnergy","TagAndProbe_MuonSpec_Inner_JetEnergy", "Jet En ergy in Halo [GeV]", "Number of Entries", 100, 0.0, 100000.);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_TrackPt","TagAndProbe_MuonSpec_Inner_TrackPt", "Sum of Track Pts in Halo [GeV]", "Number of Entries", 100, 0.0, 100000.);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_NumberOfTracks","TagAndProbe_MuonSpec_Inner_NumberOfTracks", "Number of Inner Tracks in Halo [GeV]", "Number of Entries", 40, 0, 20);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_Inner_Pts", "TagAndProbe_MuonSpec_Inner_Pts", "Track Pt [GeV]", "Number of Entries", 100, 0.0, 100000.);
  msRootInterface.setNewHistogram("TagAndProbe_MuonSpec_NCandidates", "TagAndProbe_MuonSpec_NCandidates", "TagAndProbe Candidate Pairs", "Number of Entries", 20, 0, 5);
  msRootInterface.setNewHistogram("TagAndProbe_Section_Efficiencies", "TagAndProbe_Section_Efficiencies", "Efficiency for each Section seperatly", "Number of Entries", 101, 0.0, 1.01);
  msRootInterface.setNewHistogram("TagAndProbe_Section_EffErr", "TagAndProbe_Section_EffErr", "Error on Efficiency for each Section seperatly", "Number of Entries", 101, 0.0, 1.01);
 
  for(int i=0; i<(int)m_TriggerContainer.size(); i++) 
    {
    
      msRootInterface.setNewHistogram("DO_MuonSpec_DiMuonMass_"+m_TriggerContainer[i], 
				      "DO_MuonSpec_DiMuonMass_"+m_TriggerContainer[i], 
				      "DiMuonMass ("+m_TriggerContainer[i]+") [GeV]","Number of Entries", 100, 0.0, 120000.);
    
      msRootInterface.setNewHistogram("DO_MuonSpec_DiMuonMassN1_"+m_TriggerContainer[i], 
				      "DO_MuonSpec_DiMuonMassN1_"+m_TriggerContainer[i], 
				      "DiMuonMass (c.s. "+m_TriggerContainer[i]+") [GeV]","Number of Entries", 100, 0.0, 120000.);

      msRootInterface.setNewHistogram("DO_MuonSpec_DiMuonMassN2_"+m_TriggerContainer[i], 
				      "DO_MuonSpec_DiMuonMassN2_"+m_TriggerContainer[i], 
				      "DiMuonMass (d. s. "+m_TriggerContainer[i]+") [GeV]","Number of Entries", 100, 0.0, 120000.);
    }

  for (int i=0; i<(int)vecEfficiencyNames.size(); i++)
    {

      // Set Histogramm for Users
      // Setting Up a 2d-Histogram
      // setNew2DHistogram(string id, string name, string xAxis, string yAxis,int bins, float start, float end, int bins1, float start2, float end2);

      msRootInterface.setNew2DHistogram(vecEfficiencyNames[i]+"_Efficiency_Eta_Phi",vecEfficiencyNames[i]+"_Efficiency_Eta_Phi","Eta", "Phi",50, -2.5, 2.5, 63, 0.0, 6.3);
      msRootInterface.setNew2DHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Eta_Phi",vecEfficiencyNames[i]+"_EfficiencyUncertainty_Eta_Phi","Eta", "Phi",50, -2.5, 2.5,63, 0.0, 6.3);

      msRootInterface.setNewHistogram(vecEfficiencyNames[i]+"_Efficiency_Pt",vecEfficiencyNames[i]+"_Efficiency_Pt","Pt","Efficiency",30, 5000., 80000.);
      msRootInterface.setNewHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Pt",vecEfficiencyNames[i]+"_EfficiencyUncertainty_Pt","Pt",	"Efficiency",30, 5000., 80000.);

      msRootInterface.setNewHistogram(vecEfficiencyNames[i]+"_Efficiency_Overall",vecEfficiencyNames[i]+"_Efficiency_Overall","Overall","Efficiency",1, -1.0, 1.0);
      msRootInterface.setNewHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall",vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall","OverLL","Efficiency",1, -1.0, 1.0);
    }
}


bool MSInsituPerformance::analyseMuonSpectrometerEfficiencies(MSEvent &msEvent, double Luminosity)
{
  double MeanEfficiency;
  double MeanPrecision;
  double LimitPrecision;

  // Just Informational-Output
  getAveragePrecisionForSections(MeanEfficiency, MeanPrecision, LimitPrecision);
  msRootInterface.setGraphPoint("TagAndProbe_MuonReconstruction_Efficiency_vs_Lumi", Luminosity, fabs(MeanEfficiency),0.0001,MeanPrecision);
  msRootInterface.setGraphPoint("TagAndProbe_MuonReconstruction_Precision_vs_Lumi", Luminosity, fabs(MeanPrecision),0.0001,0.0001);
  msRootInterface.setGraphPoint("TagAndProbe_MuonReconstruction_LimitPrecision_vs_Lumi", Luminosity, fabs(LimitPrecision),0.0001,0.0001);
  mapEfficiency["MuonReconstruction"].getOverallEfficiency(MeanEfficiency, MeanPrecision);
  msRootInterface.setGraphPoint("TagAndProbe_Overall_Efficiency_vs_Lumi", Luminosity, MeanEfficiency, 0.001, MeanPrecision);
  msRootInterface.setGraphPoint("TagAndProbe_Overall_Precision_vs_Lumi", Luminosity, MeanPrecision, 0.001, 0.001);

  int	nCandidates		= 0;
//  int	nNumberOfHaloTracks	= 0;
  Track1.clear();
  Track2.clear();

  msEvent.setDefaultTrackReconstruction(m_InnerTrackContainer);
  if (msEvent.getNumberOfTracks()<2) return false;
  msEvent.setupTrackCombination();

//  double distance;
  MSPhysicsObject	dummy;
  MSTrackObject	Rec_Muon1;
  MSTrackObject	Rec_Muon2;

  while (msEvent.getNextTrackCombination(Track1, Track2)==true)
    {
      double	dInvMass= getDiMuonMass(Track1, Track2);
//      double	dInvPt	= getDiMuonPt(Track1, Track2);
      double	dDeltaPhi= getDeltaPhi(Track1, Track2);
      double	dMinPt	= Track1.Pt < Track2.Pt ? Track1.Pt : Track2.Pt;
      double	dMaxPt	= Track1.Pt < Track2.Pt ? Track2.Pt : Track1.Pt;
      int	iso1	= getIsolation(Track1);
      int	iso2	= getIsolation(Track2);
      
      msRootInterface.fill("TagAndProbe_Cut_Flow",0.5);  
      if (Track1.Charge*Track2.Charge<0.0) // charge cut
	{
	  msRootInterface.fill("TagAndProbe_Cut_Flow",1.5);
	  if (fabs(dInvMass-91000.)<cutD.diMuonMass) // mass cut
	    {
	      msRootInterface.fill("TagAndProbe_Cut_Flow",2.5);
	      if ((dMaxPt>cutD.Pt_tight) && (dMinPt>cutD.Pt_loose) && (dDeltaPhi>cutD.deltaPhi)) //kinematic cuts
		{
		  msRootInterface.fill("TagAndProbe_Cut_Flow",3.5);
		  if ((iso1==3) && (iso2==3)) //isolation cuts
		    {
		      msRootInterface.fill("TagAndProbe_Cut_Flow",4.5);
		      if 	((Track1.AnalysisParameters.isElectronCandidate==false) &&
				 (Track2.AnalysisParameters.isElectronCandidate==false)) //no electron cut
			{
			  msRootInterface.fill("TagAndProbe_Cut_Flow",5.5);
			  Rec_Muon1=Track1;
			  Rec_Muon2=Track2;
			  nCandidates++;
			}
		    }
		}
	    }
	  // The next lines are just for informational porposes
	  msRootInterface.fill("TagAndProbe_MuonSpec_Inner_DiMuonMass",dInvMass);
	} // cut flow
    } //track combination

  // The next lines are just for informational porposes
  msEvent.setupTrackCounter();
  while (msEvent.getNextTrack(Track)==true)
    {
      msRootInterface.fill("TagAndProbe_MuonSpec_Inner_JetEnergy",Track.AnalysisParameters.dJetIsolation);
      msRootInterface.fill("TagAndProbe_MuonSpec_Inner_TrackPt",Track.AnalysisParameters.dTrackPtConeIsolation);
      msRootInterface.fill("TagAndProbe_MuonSpec_Inner_NumberOfTracks",Track.AnalysisParameters.dNTrackConeIsolation);
      msRootInterface.fill("TagAndProbe_MuonSpec_Inner_Pts",Track.Pt);
    }

  if ((nCandidates==0) || (msEvent.getNumberOfTriggerInfos()==0))
    {
      msRootInterface.fill("TagAndProbe_MuonSpec_NCandidates", nCandidates);
      return false;
    }

  if (	(Rec_Muon1.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true) ||
	(Rec_Muon2.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true))
    msRootInterface.fill("TagAndProbe_Cut_Flow",6.5);

  if (dRand(0.0,100.0)<50.0)
    {
      Track=Rec_Muon1;
      Rec_Muon1=Rec_Muon2;
      Rec_Muon2=Track;
    }

  if (	(Rec_Muon2.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true)
	//		&& (Rec_Muon2.getAssoziatedTriggerInformation(m_TriggerContainer, dummy)==true)
	)
    {
      if (Rec_Muon1.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true)
	{
  	  // MSEfficiency :: void setEfficiencyForEvent(bool pass, float dim1, float dim2, float dim3);
	  mapEfficiency["MuonReconstruction"].setEfficiencyForEvent(true, Rec_Muon1.Pt,	Rec_Muon1.Eta, Rec_Muon1.PhiPlane());  
	}
      else
	mapEfficiency["MuonReconstruction"].setEfficiencyForEvent(false, Rec_Muon1.Pt, Rec_Muon1.Eta, Rec_Muon1.PhiPlane());
      for(int i=0; i<(int)m_TriggerContainer.size(); i++){
			
	if (Rec_Muon1.getAssoziatedTriggerInformation(m_TriggerContainer[i], dummy)==true)
	  {
	    mapEfficiency[m_TriggerContainer[i]].setEfficiencyForEvent(true, Rec_Muon1.Pt, Rec_Muon1.Eta, Rec_Muon1.PhiPlane());
	  }
	else
	  mapEfficiency[m_TriggerContainer[i]].setEfficiencyForEvent(false, Rec_Muon1.Pt, Rec_Muon1.Eta, Rec_Muon1.PhiPlane());
	
      }
    }
  if (	(Rec_Muon1.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true)
	//		&& (Rec_Muon1.getAssoziatedTriggerInformation(m_TriggerContainer, dummy)==true)
	)
    {
      if (Rec_Muon2.getAssoziatedTrackInformation(m_MuonSpectrometerTrackContainer, dummy)==true)
	{
	  mapEfficiency["MuonReconstruction"].setEfficiencyForEvent(true, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
	}
      else
	mapEfficiency["MuonReconstruction"].setEfficiencyForEvent(false, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
      for(int i=0; i<(int)m_TriggerContainer.size(); i++){
			
	if (Rec_Muon2.getAssoziatedTriggerInformation(m_TriggerContainer[i], dummy)==true)
	  {
	    mapEfficiency[m_TriggerContainer[i]].setEfficiencyForEvent(true, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
	  }
	else
	  mapEfficiency[m_TriggerContainer[i]].setEfficiencyForEvent(false, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
      }
    }
  msRootInterface.fill("TagAndProbe_MuonSpec_NCandidates", nCandidates);

  return true;
}


bool	 MSInsituPerformance::analyseInnerTrackerEfficiency(MSEvent &msEvent)
{

  int	nCandidates		= 0;
//  int	nNumberOfHaloTracks	= 0;
  Track1.clear();
  Track2.clear();

  msEvent.setDefaultTrackReconstruction(m_MuonSpectrometerTrackContainer);

  if (msEvent.getNumberOfTracks()<2) return false;
  msEvent.setupTrackCombination();

  MSPhysicsObject	dummy;
  MSTrackObject	Rec_Muon1;
  MSTrackObject	Rec_Muon2;

  while (msEvent.getNextTrackCombination(Track1, Track2)==true)
    {
      double	dInvMass= getDiMuonMass(Track1, Track2);
//      double	dInvPt	= getDiMuonPt(Track1, Track2);
      double	dDeltaEta= fabs(Track1.Eta-Track2.Eta);
      double	dMinPt	= Track1.Pt < Track2.Pt ? Track1.Pt : Track2.Pt;
      double	dMaxPt	= Track1.Pt < Track2.Pt ? Track2.Pt : Track1.Pt;
      int	iso1	= getIsolation(Rec_Muon1);
      int	iso2	= getIsolation(Rec_Muon2);

      if (	(Track1.Charge*Track2.Charge < 0.0) &&
		(fabs(dInvMass-91000.) < cutD.ZInvmMassWnd /* 12000.*/ ) &&
		(dMaxPt>cutD.Pt_tight) &&
		(dMinPt>cutD.Pt_loose) &&
		(dDeltaEta > 0.5) &&
		(iso1>=2) && (iso2>=2))
	{
	  Rec_Muon1 = Track1;
	  Rec_Muon2 = Track2;
	  nCandidates++;
	}
    }

  if ((Rec_Muon1.Chi2oDOF < 30.) || (Rec_Muon1.Chi2oDOF < 30.)) return false;

  if (nCandidates==0) return false;

  if (dRand(0.0,100.0) < 50.0)
    {
      Track=Rec_Muon1;
      Rec_Muon1=Rec_Muon2;
      Rec_Muon2=Track;
    }

  if (Rec_Muon2.getAssoziatedTrackInformation(m_InnerTrackContainer, dummy)==true)
    {
      if (Rec_Muon1.getAssoziatedTrackInformation(m_InnerTrackContainer, dummy)==true)
	{
	  mapEfficiency["InnerTracking"].setEfficiencyForEvent(true, Rec_Muon1.Pt, Rec_Muon1.Eta, Rec_Muon1.PhiPlane());
	}
      else
	mapEfficiency["InnerTracking"].setEfficiencyForEvent(false, Rec_Muon1.Pt, Rec_Muon1.Eta, Rec_Muon1.PhiPlane());
    }

  if (Rec_Muon1.getAssoziatedTrackInformation(m_InnerTrackContainer, dummy)==true)
    {
      if (Rec_Muon2.getAssoziatedTrackInformation(m_InnerTrackContainer, dummy)==true)
	{
	  mapEfficiency["InnerTracking"].setEfficiencyForEvent(true, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
	}
      else
	mapEfficiency["InnerTracking"].setEfficiencyForEvent(false, Rec_Muon2.Pt, Rec_Muon2.Eta, Rec_Muon2.PhiPlane());
    }

  return true;
}

bool MSInsituPerformance::analyseMuonSpectrometerTriggerEfficienciesDO(MSEvent &msEvent, double /*Luminosity*/, std::string /*trigger_item*/, unsigned int /*trigger_level*/)
{
  
  //if(msEvent.isTriggered(trigger_level,trigger_item)) {
  // in order to be able to test also MuonStandalone and Muid reconstruction
  // all events are used

  int	nCandidates		= 0;
//  int	nNumberOfHaloTracks	= 0;
  Track1.clear();
  Track2.clear();
      
  msEvent.setDefaultTrackReconstruction(m_MuonContainer);
  if (msEvent.getNumberOfTracks()<2) return false;
  
  MSTrackObject	Rec_Muon1;
  MSTrackObject	Rec_Muon2;
  
  MSTrackObject	Trigger_Muon;
  

  for(int i=0; i<(int)m_TriggerContainer.size(); i++){
    
    int Nmu20 = 0;
      
    // get number of EF trigger muon with pt > 20 GeV
    msEvent.setDefaultTrigger(m_TriggerContainer[i]);
    msEvent.setupTrackCombination();
    msEvent.setupTriggerCounter();

    while(msEvent.getNextTrigger(Trigger_Muon)) {
     if(Trigger_Muon.Pt > cutD.TrigMu_Pt /*20000.*/) Nmu20++;
    }    
 
    // if at least 1 trigger muon with pt > GeV 
    // should be trivial: we are triggering with EF_mu20i menu ...
    if(Nmu20) {
      while (msEvent.getNextTrackCombination(Track1, Track2)==true) {
	double	dInvMass= getDiMuonMass(Track1, Track2);
//	double	dInvPt	= getDiMuonPt(Track1, Track2);
//	double	dDeltaPhi= getDeltaPhi(Track1, Track2);
//	double	dMinPt	= Track1.Pt < Track2.Pt ? Track1.Pt : Track2.Pt;
//	double	dMaxPt	= Track1.Pt < Track2.Pt ? Track2.Pt : Track1.Pt;
//	int	iso1	= getIsolation(Track1);
//	int	iso2	= getIsolation(Track2);
		
	msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],0.5);
	if (Track1.Charge*Track2.Charge<0.0) {
	  msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],1.5);
	  if (fabs(dInvMass-91000.)<cutD.diMuonMass) {
	    msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],2.5);
	    //if ((dMaxPt>cutD.Pt_tight) && (dMinPt>cutD.Pt_loose) && (dDeltaPhi>cutD.deltaPhi))
	    //	{
	    //	  msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],3.5);
	    //  if ((iso1==3) && (iso2==3))
	    //    {
	    //      msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],4.5);
	    //      if 	((Track1.AnalysisParameters.isElectronCandidate==false) &&
	    //		 (Track2.AnalysisParameters.isElectronCandidate==false))
	    //	{
	    //	  msRootInterface.fill("DO_Cut_Flow_"+m_TriggerContainer[i],5.5);
	    Rec_Muon1=Track1;
	    Rec_Muon2=Track2;
	    nCandidates++;
		    
	    msRootInterface.fill("DO_MuonSpec_DiMuonMass_"+m_TriggerContainer[i],dInvMass);
		    
//	    double TrigPt1 = 0;
//	    double TrigPt2 = 0;
		    
	    msRootInterface.fill("DO_MuonSpec_DiMuonMassN1_"+m_TriggerContainer[i],dInvMass);
		    
	    if(Nmu20 > 1)
	      msRootInterface.fill("DO_MuonSpec_DiMuonMassN2_"+m_TriggerContainer[i],dInvMass);
		    
	    //}
	    //}
	    //}
	  }
	  // The next lines are just for informational porposes
	}
      }
    }
  } // end of loop on TriggerContainer
  //}
  return true;
}



void	MSInsituPerformance::getAveragePrecisionForSections(double &MeanEfficiency, double &MeanPrecision, double &LimitPrecision)
{
  double Eff;
  double EffError;
//  double meanOfBin;
  double widthOfBin;

  MeanEfficiency	= 0.0;
  MeanPrecision	        = 0.0;
  LimitPrecision	= 0.0;

  msRootInterface.clearHistogram("TagAndProbe_Section_Efficiencies");
  msRootInterface.clearHistogram("TagAndProbe_Section_EffErr");

  for (int d2=0; d2 < (int)mapEfficiency["MuonReconstruction"].getNumberOfBins(2); d2++)
    for (int d3=0; d3 < (int)mapEfficiency["MuonReconstruction"].getNumberOfBins(3); d3++)
      {
	double x2,x3;
	mapEfficiency["MuonReconstruction"].getDimBinInfo(2, d2, x2, widthOfBin);
	mapEfficiency["MuonReconstruction"].getDimBinInfo(3, d3, x3, widthOfBin);
	mapEfficiency["MuonReconstruction"].getEfficiency23(x2, x3, Eff, EffError);
	msRootInterface.fill("TagAndProbe_Section_Efficiencies",Eff);
	msRootInterface.fill("TagAndProbe_Section_EffErr",EffError);
      }

  double lower;
  double higher;

  MeanEfficiency = msRootInterface.getMeanOfHistogram("TagAndProbe_Section_Efficiencies");
  MeanPrecision	 = msRootInterface.getMeanOfHistogram("TagAndProbe_Section_EffErr");
  msRootInterface.getRangeLimitsOfHistogram("TagAndProbe_Section_EffErr", .8, lower, higher);
  LimitPrecision = lower;

  return;
}



void MSInsituPerformance::writeResults()
{
  double Eff;
  double EffError;
//  double meanOfBin;
//  double widthOfBin;
  vector<string> combNames;


  msRootInterface.setDirectory(m_Outputfile,m_RootDirName);
  for (int i=0; i<(int)vecEfficiencyNames.size(); i++)
    {
      for (double x=-2.501; x<2.5; x=x+0.1)
	{
	  for (double y=0.01; y<6.2; y=y+0.1)
	    {
	      mapEfficiency[vecEfficiencyNames[i]].getEfficiency23(x, y, Eff, EffError);

	      if (Eff>=0.0)
		msRootInterface.fill2D(vecEfficiencyNames[i]+"_Efficiency_Eta_Phi",x, y, Eff);

	      if (EffError>=0.0)
		msRootInterface.fill2D(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Eta_Phi",x, y, EffError);
	    }
	}
      msRootInterface.write2DHistogram(vecEfficiencyNames[i]+"_Efficiency_Eta_Phi");
      msRootInterface.write2DHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Eta_Phi");

      for (double y=5000.1; y<80000.; y=y+5000.)
	{
	  mapEfficiency[vecEfficiencyNames[i]].getEfficiency1(y, Eff, EffError);

	  if (Eff>=0.0)
	    msRootInterface.fill(vecEfficiencyNames[i]+"_Efficiency_Pt", y, Eff);

	  if (EffError>=0.0)
	    msRootInterface.fill(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Pt",y, EffError);
	}
      msRootInterface.writeHistogram(vecEfficiencyNames[i]+"_Efficiency_Pt");
      msRootInterface.writeHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Pt");

      // Overall Efficiency
      mapEfficiency[vecEfficiencyNames[i]].getOverallEfficiency(Eff, EffError);
      msRootInterface.fill(vecEfficiencyNames[i]+"_Efficiency_Overall", 1., Eff);
      msRootInterface.fill(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall", 1., EffError);
      msRootInterface.writeHistogram(vecEfficiencyNames[i]+"_Efficiency_Overall");
      msRootInterface.writeHistogram(vecEfficiencyNames[i]+"_EfficiencyUncertainty_Overall");
    }


  msRootInterface.setDirectory(m_Outputfile,m_RootDirName+"/Detailed_Information");

  for (int n=0; n<(int)vecEfficiencyNames.size(); n++)
    {
      mapEfficiency[vecEfficiencyNames[n]].getOverallEfficiency(Eff, EffError);

      msRootInterface.fill("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Overall", 0., Eff);
      msRootInterface.fill("TagAndProbe_"+vecEfficiencyNames[n]+"_EfficiencyUncertainty_Overall", 0., EffError);
      msRootInterface.writeHistogram("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Overall");
      msRootInterface.writeHistogram("TagAndProbe_"+vecEfficiencyNames[n]+"_EfficiencyUncertainty_Overall");

      //msRootInterface.setGraphPoint(	"TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Overall", 
      //				0.0,Eff,2.5,EffError);
      //msRootInterface.writeGraph("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Overall");

      msRootInterface.setGraphPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Pt",mapEfficiency[vecEfficiencyNames[n]], 1);
      msRootInterface.writeGraph("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Pt");

      msRootInterface.setGraphPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Eta",mapEfficiency[vecEfficiencyNames[n]], 2);
      msRootInterface.writeGraph("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Eta");

      msRootInterface.setGraphPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Phi",mapEfficiency[vecEfficiencyNames[n]], 3);
      msRootInterface.writeGraph("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Phi");
    
      ////////////////////////////////////
      // histogram version of graphs
      ////////////////////////////////////

      msRootInterface.setHistogramPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Pt_h", mapEfficiency[vecEfficiencyNames[n]], 1);
      msRootInterface.writeHistogram("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Pt_h");

      msRootInterface.setHistogramPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Eta_h",mapEfficiency[vecEfficiencyNames[n]], 2);
      msRootInterface.writeHistogram("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Eta_h");

      msRootInterface.setHistogramPoints("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Phi_h",mapEfficiency[vecEfficiencyNames[n]], 3);
      msRootInterface.writeHistogram("TagAndProbe_"+vecEfficiencyNames[n]+"_Efficiency_Phi_h");
    }

  combNames.clear();
  combNames.push_back("TagAndProbe_MuonReconstruction_IfTriggered_Efficiency_Eta");
  combNames.push_back("TagAndProbe_MuonReconstruction_IfNotTriggered_Efficiency_Eta");
  msRootInterface.writeComparisionGraph(combNames, "MuonReconstruction_Trigger_NotTrigger_Eta");

  msRootInterface.writeGraph("TagAndProbe_MuonReconstruction_Efficiency_vs_Lumi");
  msRootInterface.writeGraph("TagAndProbe_MuonReconstruction_Precision_vs_Lumi");
  msRootInterface.writeGraph("TagAndProbe_MuonReconstuction_LimitPrecision_vs_Lumi");
  msRootInterface.writeGraph("TagAndProbe_Resolution_Eta");

  msRootInterface.writeGraph("TagAndProbe_Overall_Efficiency_vs_Lumi");
  msRootInterface.writeGraph("TagAndProbe_Overall_Precision_vs_Lumi");

  msRootInterface.setDirectory(m_Outputfile,m_RootDirName+"/TagAndProbe_Cut_Histograms");

  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_DiMuonMass");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_DeltaPhi");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_JetEnergy");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_TrackPt");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_NumberOfTracks");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_Inner_Pts");
  msRootInterface.writeHistogram("TagAndProbe_MuonSpec_NCandidates");
  msRootInterface.writeHistogram("TagAndProbe_Section_Efficiencies");
  msRootInterface.writeHistogram("TagAndProbe_Section_EffErr");

  msRootInterface.writeHistogram("TagAndProbe_Cut_Flow");

  msRootInterface.setDirectory(m_Outputfile,m_RootDirName+"/DO_Histograms");

  for(int i=0; i<(int)m_TriggerContainer.size(); i++) {
	
    msRootInterface.writeHistogram("DO_MuonSpec_DiMuonMass_"+m_TriggerContainer[i]);
    msRootInterface.writeHistogram("DO_MuonSpec_DiMuonMassN1_"+m_TriggerContainer[i]);
    msRootInterface.writeHistogram("DO_MuonSpec_DiMuonMassN2_"+m_TriggerContainer[i]);
	
    msRootInterface.writeHistogram("DO_Cut_Flow_"+m_TriggerContainer[i]);
    
  }
	
}
 

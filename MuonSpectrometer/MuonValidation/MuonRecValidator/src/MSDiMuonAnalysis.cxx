/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	Class for doing the DiMuonResolution analysis.
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "MuonRecValidator/MSDiMuonAnalysis.h"


MSDiMuonAnalysis::MSDiMuonAnalysis()
{
  m_PDG=23;
}


MSDiMuonAnalysis::~MSDiMuonAnalysis()
{

}


void MSDiMuonAnalysis::setup(string outputfile,
			     double maxConeDistance,
			     double minimalMuonPt,
			     string comparingTrackContainerName,
			     string versionName1,
			     string versionName2,
			     vector<float>	vResolutionRangesPt,
			     vector<float>	vResolutionRangesEta)
{
  m_versionName1	= versionName1;
  m_versionName2	= versionName2;
  m_comparingTrackContainerName	= comparingTrackContainerName;

  vecTrackContainerNames.clear();
  vecTrackContainerNames.push_back(m_versionName1+comparingTrackContainerName);
  vecTrackContainerNames.push_back(m_versionName2+comparingTrackContainerName);

  setup(outputfile, maxConeDistance, minimalMuonPt, vecTrackContainerNames, vResolutionRangesPt, vResolutionRangesEta);
}


void MSDiMuonAnalysis::setup(string outputfile,
			     double maxConeDistance,
			     double minimalMuonPt,
			     vector<string>	vTrackContainerNames,
			     vector<float>	vResolutionRangesPt,
			     vector<float>	vResolutionRangesEta)
{
  m_Outputfile		= outputfile;
  m_MaxConeDistance	= maxConeDistance;
  m_MinimalMuonPt		= minimalMuonPt;

  vecTrackContainerNames	= vTrackContainerNames;

  set_Track_Histograms(vResolutionRangesPt, vResolutionRangesEta);

  msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance");
  msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance","TruthInfo");

  msRootInterface.setNewHistogram("TruthInfo_DiMuonMass",  "TrueDiMuonMass",  "True Invariant Mass",  "Number of Entries", 120, 0., 120000.);
  msRootInterface.setNewHistogram("TruthInfo_ZBosonMass",  "TrueZBosonMass",  "True Z Boson Mass",  "Number of Entries", 120, 0., 120000.);
	
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance",vecTrackContainerNames[n]);	
  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance");
}



void MSDiMuonAnalysis::set_Track_Histograms(vector<float>	vResolutionRangesPt, vector<float>	vResolutionRangesEta)
{
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      mapResolution[vecTrackContainerNames[n]].setupDim("Pt", vResolutionRangesPt[0],	vResolutionRangesPt[1],(int)vResolutionRangesPt[2]);
      mapResolution[vecTrackContainerNames[n]].setupDim("Eta",vResolutionRangesEta[0],vResolutionRangesEta[1],(int)vResolutionRangesEta[2]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonMass_Overall",
				      vecTrackContainerNames[n]+"_DiMuonMass_Overall",
				      "DiMuonMass_Overall",
				      "Number of Entries",  120, 0.0, 120000.);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonMass_EndCap",
				      vecTrackContainerNames[n]+"_DiMuonMass_EndCap",
				      "DiMuonMass_EndCap",
				      "Number of Entries",  120, 0.0, 120000.);
		
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonMass_Barrel",
				      vecTrackContainerNames[n]+"_DiMuonMass_Barrel",
				      "DiMuonMass_EndCap",
				      "Number of Entries",  120, 0.0, 120000.);

      // msRootInterface.setNewGraph(id, name, Xaxis, Yaxis);							
		
      // ROOT GRaph  _DiMuonMean_Eta
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_DiMuonMean_Eta", "DiMuonMean #eta ("+vecTrackContainerNames[n]+")", "#eta","DiMuonMean");
      // histogram version of graph _DiMuonMean_Eta
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonMean_Eta_h",
				      vecTrackContainerNames[n]+"_DiMuonMean_Eta_h",
				      "#eta",
				      "DiMuonMean",
				      (int)vResolutionRangesEta[0],
				      vResolutionRangesEta[1],
				      vResolutionRangesEta[2]);

      // ROOT GRaph  _DiMuonMean_Pt
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_DiMuonMean_Pt","DiMuonMean #pt ("+vecTrackContainerNames[n]+")","#pt","DiMuonMean");
      // histogram version of graph _DiMuonMean_Pt
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonMean_Pt_h",
				      vecTrackContainerNames[n]+"_DiMuonMean_Pt_h",
				      "#pt",
				      "DiMuonMean",
				      (int)vResolutionRangesPt[0],
				      vResolutionRangesPt[1],
				      vResolutionRangesPt[2]);	       
	      
      // ROOT GRaph  _DiMuonWidth_Eta
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_DiMuonWidth_Eta","DiMuonWidth #eta ("+vecTrackContainerNames[n]+")","#eta","DiMuonWidth");
      // histogram version of graph _DiMuonWidth_Eta              
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonWidth_Eta_h",
				      vecTrackContainerNames[n]+"_DiMuonWidth_Eta_h",
				      "#eta",
				      "DiMuonWidth",
				      (int)vResolutionRangesEta[0],
				      vResolutionRangesEta[1],
				      vResolutionRangesEta[2]);	       
	      		
      // ROOT GRaph  _DiMuonWidth_Pt		
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_DiMuonWidth_Pt","DiMuonWidth #pt ("+vecTrackContainerNames[n]+")","#pt","DiMuonWidth");
      // histogram version of graph _DiMuonWidth_Pt
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_DiMuonWidth_Pt_h",
				      vecTrackContainerNames[n]+"_DiMuonWidth_Pt_h",
				      "#pt",
				      "DiMuonWidth",
				      (int)vResolutionRangesPt[0],
				      vResolutionRangesPt[1],
				      vResolutionRangesPt[2]);	      
    }
}


void MSDiMuonAnalysis::analyse(MSEvent &msEvent)
{
  analyse_truth(msEvent);

  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      msEvent.setDefaultTrackReconstruction(vecTrackContainerNames[n]);
      analyse_dimuons(msEvent,n);
    }
}


void	MSDiMuonAnalysis::analyse_TwoVersions(MSEvent &msEvent, int nVersionID)
{
  if ((nVersionID!=0) && (nVersionID!=1)) return;

  msEvent.setDefaultTrackReconstruction(m_comparingTrackContainerName);
  analyse_truth(msEvent);
  analyse_dimuons(msEvent,nVersionID);
}




void MSDiMuonAnalysis::analyse_truth(MSEvent &msEvent)
{
  MSTrackObject		truthParticle1;
  MSTrackObject		truthParticle2;
  bool			tag;

	
  tag=false;
  msEvent.setupTrackCounter();
  while (msEvent.getNextParticle(truthParticle1)==true)
    {
      if ((abs(truthParticle1.PdgID)==23) && (!tag))
	{
	  tag=true;
	  msRootInterface.fill("TruthInfo_ZBosonMass",truthParticle1.AnalysisParameters.dMass);
	  True_Z_Boson = truthParticle1;
	}
    }
	
  tag=false;
  msEvent.setupParticleCombination();
  while (msEvent.getNextParticleCombination(truthParticle1, truthParticle2)==true)
    {
      if (	(abs(truthParticle1.PdgID)==13) && (truthParticle1.Mother==23) && (abs(truthParticle2.PdgID)==13) && (truthParticle2.Mother==23) &&
		(!tag) && (truthParticle1.Charge!=truthParticle2.Charge))
	{
	  msRootInterface.fill("TruthInfo_DiMuonMass",getDiMuonMass(truthParticle1, truthParticle2));
	  True_Z_Muon1 = truthParticle1;
	  True_Z_Muon2 = truthParticle2;
	  tag=true;
	}
    }	
  return;
}


void MSDiMuonAnalysis::analyse_dimuons(MSEvent &msEvent, int n)
{
  double dimass;
//  double dStep;
//  double d_begin;
//  double d_end;
  MSTrackObject	Rec_Z_Muon1;
  MSTrackObject	Rec_Z_Muon2;
  int n1;
  int n2;
	
  if	((True_Z_Muon1.getAssoziatedTrackInformation(msEvent.getDefaultTrackReconstruction(), Rec_Z_Muon1)==true) && 
	 (True_Z_Muon2.getAssoziatedTrackInformation(msEvent.getDefaultTrackReconstruction(), Rec_Z_Muon2)==true))
    {
      dimass = getDiMuonMass(Rec_Z_Muon1, Rec_Z_Muon2);

      msRootInterface.fill(vecTrackContainerNames[n]+"_DiMuonMass_Overall",dimass);
      if ((fabs(Rec_Z_Muon1.Eta)>1.0) && (fabs(Rec_Z_Muon2.Eta)>1.0))
	msRootInterface.fill(vecTrackContainerNames[n]+"_DiMuonMass_EndCap",dimass);
      if ((fabs(Rec_Z_Muon1.Eta)<1.0) && (fabs(Rec_Z_Muon2.Eta)<1.0))
	msRootInterface.fill(vecTrackContainerNames[n]+"_DiMuonMass_Barrel",dimass);
		
      n1 = mapResolution[vecTrackContainerNames[n]].getBinInfo("Pt", Rec_Z_Muon1.Pt);
      n2 = mapResolution[vecTrackContainerNames[n]].getBinInfo("Pt", Rec_Z_Muon2.Pt);
      if ((n1==n2) && (n1!=-1))	mapResolution[vecTrackContainerNames[n]].add("Pt", Rec_Z_Muon1.Pt, dimass);

      n1 = mapResolution[vecTrackContainerNames[n]].getBinInfo("Eta", Rec_Z_Muon1.Eta);
      n2 = mapResolution[vecTrackContainerNames[n]].getBinInfo("Eta", Rec_Z_Muon2.Eta);
      if ((n1==n2) && (n1!=-1))	mapResolution[vecTrackContainerNames[n]].add("Eta", Rec_Z_Muon1.Eta, dimass);
    }
}


void MSDiMuonAnalysis::writeResults()
{
  vector<string> combNames;
  string name;

  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance");

  // Writing Graphs and histogram version of graphs
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerDiMuonPerformance/"+vecTrackContainerNames[n]);
                
      // msRootInterface.setNewGraph(id, name, Xaxis, Yaxis);
      // ROOT Graph  _DiMuonWidth_Pt
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_DiMuonWidth_Pt",mapResolution[vecTrackContainerNames[n]], "Pt", "ZBoson_Width");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_DiMuonWidth_Pt");	 
      // histogram version of graph _DiMuonWidth_Pt
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_DiMuonWidth_Pt",mapResolution[vecTrackContainerNames[n]], "Pt", "ZBoson_Width");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonWidth_Pt");		

      // ROOT Graph  _DiMuonWidth_Eta
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_DiMuonWidth_Eta",mapResolution[vecTrackContainerNames[n]], "Eta", "ZBoson_Width");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_DiMuonWidth_Eta");
      // histogram version of graph _DiMuonWidth_Eta
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_DiMuonWidth_Eta",mapResolution[vecTrackContainerNames[n]], "Eta", "ZBoson_Width");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonWidth_Eta");		
                
      // ROOT Graph  _DiMuonMean_Pt
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_DiMuonMean_Pt", mapResolution[vecTrackContainerNames[n]], "Pt", "ZBoson_Mean");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_DiMuonMean_Pt");
      // histogram version of graph _DiMuonMean_Pt
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_DiMuonMean_Pt", mapResolution[vecTrackContainerNames[n]], "Pt", "ZBoson_Mean");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonMean_Pt");		

      // ROOT Graph  _DiMuonMean_Eta
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_DiMuonMean_Eta",mapResolution[vecTrackContainerNames[n]], "Eta", "ZBoson_Mean");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_DiMuonMean_Eta");
      // histogram version of graph _DiMuonMean_Eta
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_DiMuonMean_Eta",mapResolution[vecTrackContainerNames[n]], "Eta", "ZBoson_Mean");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonMean_Eta");		 

    }

  // Writing Combined Graphs
  msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerDiMuonPerformance");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_DiMuonWidth_Eta");
  msRootInterface.writeComparisionGraph(combNames, "DiMuonWidth_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_DiMuonWidth_Pt");
  msRootInterface.writeComparisionGraph(combNames, "DiMuonWidth_Pt");
	
  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_DiMuonMean_Eta");
  msRootInterface.writeComparisionGraph(combNames, "DiMuonMean_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_DiMuonMean_Pt");
  msRootInterface.writeComparisionGraph(combNames, "DiMuonMean_Pt");

  // Writing Histograms
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      if (vecTrackContainerNames[n]!="TrackParticleCandidate")
	{
	  msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerDiMuonPerformance/"+vecTrackContainerNames[n]);	
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonMass_Overall");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonMass_EndCap");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_DiMuonMass_Barrel");
	}
    }

  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerDiMuonPerformance/TruthInfo");
  msRootInterface.writeHistogram("TruthInfo_DiMuonMass");
  msRootInterface.writeHistogram("TruthInfo_ZBosonMass");
}

double	MSDiMuonAnalysis::getDiMuonMass(MSPhysicsObject &muon1, MSPhysicsObject &muon2)
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
 

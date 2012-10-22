/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
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


#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "MuonRecValidator/MSMuonAnalysis.h"
#include "CLHEP/Units/SystemOfUnits.h"

MSMuonAnalysis::MSMuonAnalysis()
{

  cutMu.muonRec_Pt  = 20.*CLHEP::GeV; // in MeV

}


MSMuonAnalysis::~MSMuonAnalysis()
{

}


void MSMuonAnalysis::setup(	string outputfile,
                                cutMuonAnalDefinitons cuts, 
				double maxConeDistance,
				double minimalMuonPt,
				string comparingTrackContainerName,
				string comparingTriggerContainerName,
				vector<float>	vEfficiencyRanges,
				vector<float>	vResolutionRanges,
				string versionName1,
				string versionName2)
{
  cutMu                 = cuts;
  m_versionName1	= versionName1;
  m_versionName2	= versionName2;
  m_comparingTrackContainerName	= comparingTrackContainerName;
  m_comparingTriggerContainerName	= comparingTriggerContainerName;

  vecTrackContainerNames.clear();
  vecTrackContainerNames.push_back(m_versionName1+comparingTrackContainerName);
  vecTrackContainerNames.push_back(m_versionName2+comparingTrackContainerName);

  vecTriggerContainerNames.clear();
  vecTriggerContainerNames.push_back(m_versionName1+comparingTriggerContainerName);
  vecTriggerContainerNames.push_back(m_versionName2+comparingTriggerContainerName);

  setup(outputfile, cutMu, maxConeDistance, minimalMuonPt, vecTrackContainerNames, vecTriggerContainerNames, vEfficiencyRanges, vResolutionRanges, false);
}


void MSMuonAnalysis::setup(	string outputfile,
                                cutMuonAnalDefinitons cuts, 
				double maxConeDistance,
				double minimalMuonPt,
				vector<string>	vTrackContainerNames,
				vector<string>	vTriggerContainerNames,
				vector<float>	vEfficiencyRanges,
				vector<float>	vResolutionRanges,
				bool triggerAware)
{
  cutMu                 = cuts;
  m_TriggerAware        = triggerAware;
  m_Outputfile		= outputfile;
  m_MaxConeDistance	= maxConeDistance;
  m_MinimalMuonPt	= minimalMuonPt;

  vecTrackContainerNames	= vTrackContainerNames;
  vecTriggerContainerNames= vTriggerContainerNames;

  std::cout << "vEfficiencyRanges size = " << vEfficiencyRanges.size() << std::endl;
  std::cout << "vResolutionRanges size = " << vResolutionRanges.size() << std::endl;
	
  MSEfficiency msEffDummy;
  if ((int)vEfficiencyRanges.size()==9)
    {
      m_PhiOffset=0.0;
      msEffDummy.setupDim(1, vEfficiencyRanges[0], vEfficiencyRanges[1], (int)vEfficiencyRanges[2]);
      msEffDummy.setupDim(2, vEfficiencyRanges[3], vEfficiencyRanges[4], (int)vEfficiencyRanges[5]);
      msEffDummy.setupDim(3, vEfficiencyRanges[6], vEfficiencyRanges[7], (int)vEfficiencyRanges[8]);
    } else
      {
	vector <double> bins1, bins2, bins3;

	m_PhiOffset=3.4034;
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
		
	bins2.push_back(-2.40);
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
	bins2.push_back(2.40);
		
	bins3.push_back(0.00  * 0.0174533);//large sector
	bins3.push_back(30.0  * 0.0174533);//small sector
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
		
	msEffDummy.setupDim(1, bins1);
	msEffDummy.setupDim(2, bins2);
	msEffDummy.setupDim(3, bins3);
      }

  set_Track_Histograms(msEffDummy, vResolutionRanges, vEfficiencyRanges);
  set_Trigger_Histograms(msEffDummy, vEfficiencyRanges);

  m_total=0;
  m_pass=0;

  msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerPerformance");
  msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerPerformance","TruthInfo");

  msRootInterface.setNewHistogram("TruthInfo_TrueMuon_Pt",   // Identifier of Histogram inside msRootInterface
				  "TrueMuon_Pt",   // Histogramm-Title on the Plot
				  "True Muon Pt",  // X-Axis title oin the plot
				  "Number of Entries", // 
				  100,            // Number of bins
				  0.,	           // low value
				  100000.);       // high value
  msRootInterface.setNewHistogram("TruthInfo_TrueMuon_Eta", "TrueMuon_Eta", "True Muon Eta", "Number of Entries", 60, -3.0, 3.0);
  msRootInterface.setNewHistogram("TruthInfo_TrueMuon_Phi", "TrueMuon_Phi", "True Muon Phi", "Number of Entries", 60, -3.2, 3.2);

  //create the ROOT subdirectories for selected TrackContainerNames
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerPerformance",vecTrackContainerNames[n]);
    
  //create the ROOT subdirectories for selected TriggerContainerNames	
  for (int n=0; n<(int)vecTriggerContainerNames.size(); n++)
    msRootInterface.createDirectory(m_Outputfile,"MuonSpectrometerPerformance",vecTriggerContainerNames[n]);

  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerPerformance");
}



void MSMuonAnalysis::set_Track_Histograms(MSEfficiency &msEfficiency, vector<float>	vResolutionRanges, vector<float> vEfficiencyRanges)
{
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      mapResolution[vecTrackContainerNames[n]].setupDim("Pt", vResolutionRanges[0], 
							vResolutionRanges[1], 
							(int)vResolutionRanges[2]);
      mapResolution[vecTrackContainerNames[n]].setupDim("Eta",vResolutionRanges[3], 
							vResolutionRanges[4],
							(int)vResolutionRanges[5]);
      mapResolution[vecTrackContainerNames[n]].setupDim("Phi",vResolutionRanges[6], 
							vResolutionRanges[7], 
							(int)vResolutionRanges[8]);

      mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"]=msEfficiency;
      mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"]=msEfficiency;
      mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"]=msEfficiency;

      //  set ROOT 2DHistogram
      msRootInterface.setNew2DHistogram(vecTrackContainerNames[n]+"_Eta_Phi_Efficiency", 
					"Efficiency Eta vs. Phi", "#eta", "phi", 
					52, -2.6, 2.6, 62, -3.1, 3.1);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_pT_Resolution",
                                      "Overall p_T Resolution ("+vecTrackContainerNames[n]+")",
				      "(1/p_{T}^{True}-1/p_{T}^{Rec})/(1/p_{T}^{True})",	
				      "Number of Entries",
				      100,
				      -0.25,
				      0.25);
      
      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_pT_Resolution_Eta_EndCap",
					"Overall p_T Resolution ("+vecTrackContainerNames[n]+",EndCap)",
					"(1/p_{T}^{True}-1/p_{T}^{Rec})/(1/p_{T}^{True})",
					"Number of Entries",
					100,
					-0.25,
					0.25);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_pT_Resolution_Eta_Barrel",
					"Overall p_T Resolution ("+vecTrackContainerNames[n]+",Barrel)",
					"(1/p_{T}^{True}-1/p_{T}^{Rec})/(1/p_{T}^{True})",
					"Number of Entries",
					100,
					-0.25,
					0.25);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_NumberOfMDTHits",
					"Number of MDT Hits ("+vecTrackContainerNames[n]+")",
					"Number of MDT Hits",
					"Number of Entries",
					20,
					0.,
					20.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Charge",
					"Charge ("+vecTrackContainerNames[n]+")",
					"Charge",
					"Charge",
					10,
					-2.,
					2.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_chi2oDOF",
					"ChiSQ over Degress of Freedom ("+vecTrackContainerNames[n]+")",
					"ChiSQ over Degress of Freedom",
					"Number of Entries",
					60,
					0.,
					60.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_ETCone45",
					"ET Cone ("+vecTrackContainerNames[n]+")",
					"ET Cone [GeV]",
					"Number of Entries",
					100,
					0.,
					100000.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_chi2",
					"chi2 ("+vecTrackContainerNames[n]+")",
					"chi2 ",
					"Number of Entries",
					60,
					0.,
					60.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_EnergyLoss",
					"Energy Loss ("+vecTrackContainerNames[n]+")",
					"Energy Loss [GeV]",
					"Number of Entries",
					100,
					0.,
					10000.);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Status",
					"Track Status ("+vecTrackContainerNames[n]+")",
					"Track Status (1:Combinded, 2:MuonSpectrometer Only, 3:LowPt)",
					"Number of Entries",
					10,
					0,
					5);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_FakeDOF",
					"FakeDOF ("+vecTrackContainerNames[n]+")",
					"FakeDOF",
					"Number of Entries",
					100,
					0,
					100);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_RealDOF",
					"RealDOF ("+vecTrackContainerNames[n]+")",
					"RealDOF",
					"Number of Entries",
					100,
					0,
					100);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_MetricDistance_EndCap",
					vecTrackContainerNames[n]+"_MetricDistance_EndCap",
					"Metric Distance to Truth Track",
					"Number of Entries", 100, 0.0, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_MetricDistance_Barrel",
					vecTrackContainerNames[n]+"_MetricDistance_Barrel",
					"Metric Distance to Truth Track",
					"Number of Entries",  100, 0.0, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_ImpactParameter_Z0_Resolution",
					vecTrackContainerNames[n]+"_ImpactParameter_Z0_Resolution",
					"Z0_Truth - Z0_Rec",
					"Number of Entries",  100, -1000.0, 1000.0);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_ImpactParameter_D0_Resolution",
					vecTrackContainerNames[n]+"_ImpactParameter_D0_Resolution",
					"D0_Truth - D0_Rec",
					"Number of Entries",  100, -0.1, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Eta_Resolution",
					vecTrackContainerNames[n]+"_Eta_Resolution",
					"Eta_Truth - Eta_Rec/Eta_Truth",
					"Number of Entries",  100, -0.1, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Phi_Resolution",
					vecTrackContainerNames[n]+"_Phi_Resolution",
					"Phi_Truth - Phi_Rec",
					"Number of Entries",  100, -0.1, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Pull_Phi_Resolution",
					vecTrackContainerNames[n]+"_Pull_Phi_Resolution",
					"Pull_Phi",
					"Number of Entries",  100, -0.1, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Pull_Eta_Resolution",
					vecTrackContainerNames[n]+"_Pull_Eta_Resolution",
					"Pull_Eta",
					"Number of Entries",  100, -0.1, 0.1);

      msRootInterface.setNewHistogram(	vecTrackContainerNames[n]+"_Pull_Pt_Resolution",
					vecTrackContainerNames[n]+"_Pull_Pt_Resolution",
					"Pull_1/Pt",
					"Number of Entries",  100, -0.1, 0.1);

      // set ROOT Graph

      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Efficiency_Eta", "Efficiency #eta ("+vecTrackContainerNames[n]+")", "#eta", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Efficiency_Phi", "Efficiency #phi ("+vecTrackContainerNames[n]+")", "#phi", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Efficiency_Pt", "Efficiency #Pt ("+vecTrackContainerNames[n]+")", "P_{T}", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta", "Triggered Track Reco Efficiency #eta ("+vecTrackContainerNames[n]+")", "#eta", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi", "Triggered Track Reco Efficiency #phi ("+vecTrackContainerNames[n]+")", "#phi", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt", "Triggered Track Reco Efficiency #Pt ("+vecTrackContainerNames[n]+")", "P_{T}", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta", "Not Triggered Track Reco Efficiency #eta ("+vecTrackContainerNames[n]+")", "#eta", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi", "Not Triggered Track Reco #phi ("+vecTrackContainerNames[n]+")", "#phi", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt", "Not Triggered Track Reco #Pt ("+vecTrackContainerNames[n]+")", "P_{T}", "Efficiency");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Width_Pt","Resolution Width #Pt ("+vecTrackContainerNames[n]+")", "P_{T}", "Resolution");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Width_Eta", "Resolution Width #Eta ("+vecTrackContainerNames[n]+")", "#eta", "Resolution");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Width_Phi", "Resolution Width #Phi ("+vecTrackContainerNames[n]+")", "#phi", "Resolution");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Pt", "Resolution Mean #Pt ("+vecTrackContainerNames[n]+")", "P_{T}", "Resolution");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Eta", "Resolution Mean #Eta ("+vecTrackContainerNames[n]+")", "#eta", "Resolution");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Phi", "Resolution Mean #Phi ("+vecTrackContainerNames[n]+")", "#phi", "Resolution");
						
      mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"]=msEfficiency;
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_FakeRate_Eta", "FakeRate #eta ("+vecTrackContainerNames[n]+")", "#eta","FakeRate");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_FakeRate_Phi","FakeRate #phi ("+vecTrackContainerNames[n]+")", "#phi", "FakeRate");
      msRootInterface.setNewGraph(vecTrackContainerNames[n]+"_FakeRate_Pt", "FakeRate #Pt ("+vecTrackContainerNames[n]+")", "P_T", "FakeRate");
		
      //////////////////////////////////////////////////////////
      // histogram version of graphs 
      /////////////////////////////////////////////////////////
		
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Efficiency_Overall",
				      "Efficiency Overall ("+vecTrackContainerNames[n]+")", 
				      vecTrackContainerNames[n]+"Efficiency Overall", "Efficiency Overall", 
				      3, 0., 2.);
      
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_EfficiencyUncertainty_Overall",
				      "Efficiency Uncertainty Overall ("+vecTrackContainerNames[n]+")",
				      vecTrackContainerNames[n]+" Efficiency Uncertainty Overall",
				      "Efficiency Uncertainty Overall",
				      3, 0., 2.);
      
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Efficiency_Eta_h",
				      vecTrackContainerNames[n]+"_Efficiency_Eta_h",
				      "#eta",
				      "Efficiency",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Efficiency_Phi_h",
				      vecTrackContainerNames[n]+"_Efficiency_Phi_h",
				      "#phi",
				      "Efficiency",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Efficiency_Pt_h",
				      vecTrackContainerNames[n]+"_Efficiency_Pt_h",
				      "P_{T}",
				      "Efficiency",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta_h",
				      vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta_h",
				      "#eta",
				      "Efficiency",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi_h",
				      vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi_h",
				      "#phi",
				      "Efficiency",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt_h",
				      vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt_h",
				      "P_{T}",
				      "Efficiency",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta_h",
				      vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta_h",
				      "#eta",
				      "Efficiency",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi_h",
				      vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi_h",
				      "#phi",
				      "Efficiency",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt_h",
				      vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt_h",
				      "P_{T}",
				      "Efficiency",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Pt_h",
				      vecTrackContainerNames[n]+"_Resolution_Width_Pt_h",
				      "P_{T}",
				      "Resolution",
				      (int)vResolutionRanges[2],
				      vResolutionRanges[0],
				      vResolutionRanges[1]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Eta_h",
				      vecTrackContainerNames[n]+"_Resolution_Width_Eta_h",
				      "#eta",
				      "Resolution",
				      (int)vResolutionRanges[5],
				      vResolutionRanges[3],
				      vResolutionRanges[4]);
	
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Phi_h",
				      vecTrackContainerNames[n]+"_Resolution_Width_Phi_h",
				      "#phi",
				      "Resolution",
				      (int)vResolutionRanges[8],
				      vResolutionRanges[6],
				      vResolutionRanges[7]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Pt_h",
				      vecTrackContainerNames[n]+"_Resolution_Mean_Pt_h",
				      "P_{T}",
				      "Resolution",
				      (int)vResolutionRanges[2],
				      vResolutionRanges[0],
				      vResolutionRanges[1]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Eta_h",
				      vecTrackContainerNames[n]+"_Resolution_Mean_Eta_h",
				      "#eta",
				      "Resolution",
				      (int)vResolutionRanges[5],
				      vResolutionRanges[3],
				      vResolutionRanges[4]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Phi_h",
				      vecTrackContainerNames[n]+"_Resolution_Mean_Phi_h",
				      "#phi",
				      "Resolution",
				      (int)vResolutionRanges[8],
				      vResolutionRanges[6],
				      vResolutionRanges[7]);
						
      mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"]=msEfficiency;
      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_FakeRate_Eta_h",
				      vecTrackContainerNames[n]+"_FakeRate_Eta_h",
				      "#eta",
				      "FakeRate",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_FakeRate_Phi_h", 
				      vecTrackContainerNames[n]+"_FakeRate_Phi_h",
				      "#phi",
				      "FakeRate",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTrackContainerNames[n]+"_FakeRate_Pt_h",
				      vecTrackContainerNames[n]+"_FakeRate_Pt_h",
				      "P_T",
				      "FakeRate",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);
    }
}


void	MSMuonAnalysis::set_Trigger_Histograms(MSEfficiency &msEfficiency, vector<float> vEfficiencyRanges)
{
  for (int n=0; n<(int)vecTriggerContainerNames.size(); n++)
    {

      mapEfficiency[vecTriggerContainerNames[n]+"_FakeRate"]=msEfficiency;
      mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"]=msEfficiency;
		
      msRootInterface.setNew2DHistogram(vecTriggerContainerNames[n]+"_Eta_Phi_Efficiency","Efficiency Eta vs. Phi", 
					"#eta", "phi",
					52, -2.6, 2.6, 62, -3.1, 3.1);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_MetricDistance",
				      "MetricDistance ("+vecTriggerContainerNames[n]+")",
				      "Metric Distance","Number of Entries",
				      100, 0., 1.);
		
      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_Efficiency_Overall",
				      "Efficiency Overall ("+vecTriggerContainerNames[n]+")", 
				      vecTriggerContainerNames[n]+"Efficiency Overall", "Efficiency Overall", 
				      3, 0., 2.);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_EfficiencyUncertainty_Overall",
				      "Efficiency Uncertainty Overall ("+vecTriggerContainerNames[n]+")",
				      vecTriggerContainerNames[n]+" Efficiency Uncertainty Overall",
				      "Efficiency Uncertainty Overall",
				      3, 0., 2.);
		
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_Efficiency_Eta", "Efficiency #eta ("+vecTriggerContainerNames[n]+")", "#eta", "Efficiency");
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_Efficiency_Phi", "Efficiency #phi ("+vecTriggerContainerNames[n]+")", "#phi", "Efficiency");
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_Efficiency_Pt",  "Efficiency #Pt ("+vecTriggerContainerNames[n]+")", "P_{T}", "Efficiency");
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_FakeRate_Eta", "FakeRate #eta ("+vecTriggerContainerNames[n]+")", "#eta", "FakeRate");
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_FakeRate_Phi", "FakeRate #phi ("+vecTriggerContainerNames[n]+")", "#phi","FakeRate");
      msRootInterface.setNewGraph(vecTriggerContainerNames[n]+"_FakeRate_Pt", "FakeRate #Pt ("+vecTriggerContainerNames[n]+")", "P_T", "FakeRate");

      ///////////////////////////////////////////////
      // histogram version of graphs 
      ///////////////////////////////////////////////

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_Efficiency_Eta_h",
				      vecTriggerContainerNames[n]+"_Efficiency_Eta_h",
				      "#eta",
				      "Efficiency",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_Efficiency_Phi_h",
				      vecTriggerContainerNames[n]+"_Efficiency_Phi_h",
				      "#phi",
				      "Efficiency",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_Efficiency_Pt_h",
				      vecTriggerContainerNames[n]+"_Efficiency_Pt_h",
				      "P_{T}",
				      "Efficiency",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_FakeRate_Eta_h",
				      vecTriggerContainerNames[n]+"_FakeRate_Eta_h",
				      "#eta",
				      "FakeRate",
				      (int)vEfficiencyRanges[5],
				      vEfficiencyRanges[3],
				      vEfficiencyRanges[4]);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_FakeRate_Phi_h",
				      vecTriggerContainerNames[n]+"_FakeRate_Phi_h",
				      "#phi",
				      "FakeRate",
				      (int)vEfficiencyRanges[8],
				      vEfficiencyRanges[6],
				      vEfficiencyRanges[7]);

      msRootInterface.setNewHistogram(vecTriggerContainerNames[n]+"_FakeRate_Pt_h",
				      vecTriggerContainerNames[n]+"_FakeRate_Pt_h",
				      "P_T",
				      "FakeRate",
				      (int)vEfficiencyRanges[2],
				      vEfficiencyRanges[0],
				      vEfficiencyRanges[1]);

    }
}


void MSMuonAnalysis::analyse(MSEvent &msEvent)
{
  analyse_Truth(msEvent);

  // for (int n=0; n<(int)vecTriggerContainerNames.size(); n++)
  //  {
  //    msEvent.setDefaultTrigger(vecTriggerContainerNames[n]);
  analyse_Trigger(msEvent, 0);
  //  }
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      msEvent.setDefaultTrackReconstruction(vecTrackContainerNames[n]);
      analyse_EfficiencyAndResolution(msEvent,n);
      analyse_FakeRate(msEvent,n);
      analyse_ParticleProperties(msEvent,n);
    }
}


void MSMuonAnalysis::analyse_TwoVersions(MSEvent &msEvent, int nVersionID)
{
  if ((nVersionID!=0) && (nVersionID!=1)) return;

  msEvent.setDefaultTrackReconstruction(m_comparingTrackContainerName);
  analyse_EfficiencyAndResolution(msEvent,nVersionID);
  analyse_FakeRate(msEvent,nVersionID);
  analyse_ParticleProperties(msEvent,nVersionID);

  msEvent.setDefaultTrackReconstruction(m_comparingTriggerContainerName);
  analyse_Trigger(msEvent, nVersionID);
}



void MSMuonAnalysis::analyse_Trigger(MSEvent &msEvent, int /*n*/)
{

  // TriggerContainerNames logic
  //////////////////////////////
  // 0 : Level 1
  // 1 : Level 2 MS
  // 2 : Level 2 CB
  // 3 : Level 3 MS
  // 4 : Level 3 CB
  // 5 : OverAll MS = L1 x L2MS x EFMS
  // 6 : OverAll CB = L1 x L2CB x EFCB
  ////////////////////////////////////

  std::cout << "Analyze trigger with level by level method" << std::endl;

  MSPhysicsObject muonTrigger; //member of MSPhysicsObject
  MSTrackObject	muonTruth; //member of MSTrackObject

  bool	bAsso;
  m_TriggerFired = false;

  msEvent.setupParticleCounter();

  // Determination of Trigger efficiency

  while (msEvent.getNextParticle(muonTruth)==true)
    {
      if ((abs(muonTruth.PdgID)==13) && (fabs(muonTruth.Eta)<2.4) && (muonTruth.Pt>m_MinimalMuonPt))
	{
	  // test Level 1 trigger
	  bAsso = muonTruth.getAssoziatedTriggerInformation("Trigger_Level_1", muonTrigger);
	  msRootInterface.fill(vecTriggerContainerNames[0]+"_MetricDistance",muonTrigger.metricDistance);
	  if (bAsso==true && (muonTrigger.Pt > 19900.) ) {
	    mapEfficiency[vecTriggerContainerNames[0]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);

	    // test HLT MuonSpectrometer standalone trigger efficiency

	    // test Level 2 MS
	    bAsso = muonTruth.getAssoziatedTriggerInformation("Trigger_Level_2_MS", muonTrigger);
	    msRootInterface.fill(vecTriggerContainerNames[1]+"_MetricDistance",muonTrigger.metricDistance);
	    if (bAsso==true && (muonTrigger.Pt > cutMu.TrigMu_Pt /*20000.*/ ) ) {
	      mapEfficiency[vecTriggerContainerNames[1]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	    
	      // test Level 3 MS
	      bAsso = muonTruth.getAssoziatedTriggerInformation("Trigger_Level_3_MS", muonTrigger);
	      msRootInterface.fill(vecTriggerContainerNames[3]+"_MetricDistance",muonTrigger.metricDistance);
	      if (bAsso==true && (muonTrigger.Pt > cutMu.TrigMu_Pt /*20000.*/ ) ) {
		mapEfficiency[vecTriggerContainerNames[3]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		mapEfficiency[vecTriggerContainerNames[5]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      }else{
		// level 3 
		mapEfficiency[vecTriggerContainerNames[3]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		mapEfficiency[vecTriggerContainerNames[5]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      }
	    }else{
	      mapEfficiency[vecTriggerContainerNames[1]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      mapEfficiency[vecTriggerContainerNames[5]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	    }
 
	    // test HLT MuonSpectrometer ID combined trigger efficiency

	    // test Level 2 CB
	    bAsso = muonTruth.getAssoziatedTriggerInformation("Trigger_Level_2_CB", muonTrigger);
	    msRootInterface.fill(vecTriggerContainerNames[2]+"_MetricDistance",muonTrigger.metricDistance);
	    if (bAsso==true && (muonTrigger.Pt > cutMu.TrigMu_Pt /*20000.*/ ) ) {
	      mapEfficiency[vecTriggerContainerNames[2]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      
	      // test Level 3 CB
	      bAsso = muonTruth.getAssoziatedTriggerInformation("Trigger_Level_3_CB", muonTrigger);
	      msRootInterface.fill(vecTriggerContainerNames[4]+"_MetricDistance",muonTrigger.metricDistance);
	      if (bAsso==true && (muonTrigger.Pt > cutMu.TrigMu_Pt /*20000.*/ ) ) {
		mapEfficiency[vecTriggerContainerNames[4]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		mapEfficiency[vecTriggerContainerNames[6]+"_Efficiency"].setEfficiencyForEvent(true,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		m_TriggerFired = true;
	      }else{
		mapEfficiency[vecTriggerContainerNames[4]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		mapEfficiency[vecTriggerContainerNames[6]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      }
	    }else{
	      mapEfficiency[vecTriggerContainerNames[2]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	      mapEfficiency[vecTriggerContainerNames[6]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	    }
	    
	  }else{
	    mapEfficiency[vecTriggerContainerNames[0]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	    mapEfficiency[vecTriggerContainerNames[5]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	    mapEfficiency[vecTriggerContainerNames[6]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
	  }
	}// ((abs(muonTruth.PdgID)==13) && (fabs(muonTruth.Eta)<2.4) && (muonTruth.Pt>m_MinimalMuonPt))
    }
}


void MSMuonAnalysis::analyse_Truth(MSEvent &msEvent)
{
  MSTrackObject	muonTruth; //member of MSTrackObject

  msEvent.setupParticleCounter(); // alwsays for Truth container

  while (msEvent.getNextParticle(muonTruth)==true)
    {
      if (abs(muonTruth.PdgID)==13) //for muons
	{
	  msRootInterface.fill("TruthInfo_TrueMuon_Pt",muonTruth.Pt); // Public Member Variables of MSPhysicsObject
	  msRootInterface.fill("TruthInfo_TrueMuon_Eta",muonTruth.Eta);
	  msRootInterface.fill("TruthInfo_TrueMuon_Phi",muonTruth.Phi);
	}
    }
  return;
}


void MSMuonAnalysis::analyse_ParticleProperties(MSEvent &msEvent, int n)
{
  MSTrackObject	muonTrack; //member of MSTrackObject

  msEvent.setupTrackCounter();

  while (msEvent.getNextTrack(muonTrack)==true)
    {
      msRootInterface.fill(vecTrackContainerNames[n]+"_NumberOfMDTHits",muonTrack.Hits);
      msRootInterface.fill(vecTrackContainerNames[n]+"_chi2oDOF",muonTrack.Chi2oDOF);
      msRootInterface.fill(vecTrackContainerNames[n]+"_chi2",muonTrack.Chi2);
      msRootInterface.fill(vecTrackContainerNames[n]+"_EnergyLoss",muonTrack.EnergyLoss);
      msRootInterface.fill(vecTrackContainerNames[n]+"_Status",muonTrack.Type);
      msRootInterface.fill(vecTrackContainerNames[n]+"_ETCone45",muonTrack.AnalysisParameters.dETIsolation);
    }
}


void MSMuonAnalysis::analyse_EfficiencyAndResolution(MSEvent &msEvent, int n)
{
  MSPhysicsObject muonRecTrack;
  MSPhysicsObject muonTrigger;
  MSTrackObject	  muonTruth;
  double	  dPTSigma;
  bool	          bAsso, bAssoTriggered, bEventTriggered;

  msEvent.setupParticleCounter();

  if(m_TriggerAware)
    bEventTriggered = m_TriggerFired;
  else
    bEventTriggered = true;

  // Determination of Efficiency and Resolution
  while (msEvent.getNextParticle(muonTruth)==true)
    {
      if ((abs(muonTruth.PdgID) == 13 /* for muons*/ ) && (fabs(muonTruth.Eta) < 2.7 ) && (muonTruth.Pt>m_MinimalMuonPt)) 
	{
	  bAsso = muonTruth.getAssoziatedTrackInformation(msEvent.getDefaultTrackReconstruction(), muonRecTrack);

	  m_total++;
	  if (bAsso==true && bEventTriggered && (muonRecTrack.Pt > cutMu.muonRec_Pt /* 20000.*/) )
	    {
	      if (fabs(muonTruth.Eta)>1.0) msRootInterface.fill(vecTrackContainerNames[n]+"_MetricDistance_EndCap",muonRecTrack.metricDistance);
	      if (fabs(muonTruth.Eta)<1.0) msRootInterface.fill(vecTrackContainerNames[n]+"_MetricDistance_Barrel",muonRecTrack.metricDistance);
			
	      m_pass++;
	      dPTSigma = ((1.0/muonTruth.Pt)-(1.0/muonRecTrack.Pt)) / (1.0/muonTruth.Pt);
	      mapResolution[vecTrackContainerNames[n]].add("Pt", muonTruth.Pt, dPTSigma);
	      mapResolution[vecTrackContainerNames[n]].add("Eta", muonTruth.Eta, dPTSigma);
	      mapResolution[vecTrackContainerNames[n]].add("Phi", muonTruth.Phi, dPTSigma);
			      
	      msRootInterface.fill(vecTrackContainerNames[n]+"_pT_Resolution",dPTSigma);
			      
	      if ((muonTruth.Eta>-1.0) && (muonTruth.Eta<1.0))
		{
		  msRootInterface.fill(vecTrackContainerNames[n]+"_pT_Resolution_Eta_Barrel",dPTSigma);
		} else
		  {
		    msRootInterface.fill(vecTrackContainerNames[n]+"_pT_Resolution_Eta_EndCap",dPTSigma);
		  }
	      if (muonTruth.Charge==muonRecTrack.Charge)
		{
		  msRootInterface.fill(vecTrackContainerNames[n]+"_Charge",1.0);
		} else {
		  msRootInterface.fill(vecTrackContainerNames[n]+"_Charge",-1.0);
		}	      
	      msRootInterface.fill(vecTrackContainerNames[n]+"_ImpactParameter_Z0_Resolution",(muonRecTrack.Z0-muonTruth.Z0));
	      msRootInterface.fill(vecTrackContainerNames[n]+"_ImpactParameter_D0_Resolution",(muonRecTrack.R0-muonTruth.R0));
	      msRootInterface.fill(vecTrackContainerNames[n]+"_Eta_Resolution",(muonRecTrack.Eta-muonTruth.Eta));
	      msRootInterface.fill(vecTrackContainerNames[n]+"_Phi_Resolution",(muonRecTrack.Phi-muonTruth.Phi));
	      msRootInterface.fill(vecTrackContainerNames[n]+"_Pull_Phi_Resolution",(muonRecTrack.Phi-muonTruth.Phi)/muonRecTrack.Delta_Phi);
	      msRootInterface.fill(vecTrackContainerNames[n]+"_Pull_Eta_Resolution",(muonRecTrack.Eta-muonTruth.Eta)/muonRecTrack.Delta_Eta);
	      msRootInterface.fill(vecTrackContainerNames[n]+"_Pull_Pt_Resolution",(muonRecTrack.Pt-muonTruth.Pt)/(muonRecTrack.Delta_Pt));			      
	      
	      mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt, muonTruth.Eta, muonTruth.Phi);
	    }// (bAsso==true && bEventTriggered && (muonRecTrack.Pt > 20000.)
	  else
	    {
	      mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta, muonTruth.Phi);
	    }
	  bAssoTriggered = muonTruth.getAssoziatedTriggerInformation(msEvent.getDefaultTrigger(), muonTrigger);
	  if (bAssoTriggered==true)
	    {
	      if (bAsso==true)
		{
		  mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt, muonTruth.Eta, muonTruth.Phi);
		} 
	      else
		{
		  mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt, muonTruth.Eta, muonTruth.Phi);
		}
	    } // if (bAssoTriggered==true)
	  else
	    {
	      if (bAsso==true)
		{
		  mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"].setEfficiencyForEvent(true, muonTruth.Pt, muonTruth.Eta, muonTruth.Phi);
		} 
	      else
		{
		  mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"].setEfficiencyForEvent(false,muonTruth.Pt,muonTruth.Eta,muonTruth.Phi);
		}
	    }
	} //((abs(muonTruth.PdgID)==13) && (fabs(muonTruth.Eta)<2.7) && (muonTruth.Pt>m_MinimalMuonPt))
    } //while (msEvent.getNextParticle(muonTruth)==true)
} // end of class definition


void MSMuonAnalysis::analyse_FakeRate(MSEvent &msEvent, int n)
{
  MSTrackObject	   muonRecTrack;
  MSPhysicsObject  muonTruth;
  bool	bAsso;

  msEvent.setupTrackCounter();

  // Determination of Fake-Rate
  while (msEvent.getNextTrack(muonRecTrack)==true)
    {
      bAsso = muonRecTrack.getAssoziatedTruthInformation(muonTruth);
      if (bAsso==false)
	{
	  mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"].setEfficiencyForEvent(true, muonRecTrack.Pt, muonRecTrack.Eta, muonRecTrack.Phi);
	  msRootInterface.fill(vecTrackContainerNames[n]+"_FakeDOF",muonRecTrack.Chi2oDOF);
	} 
      else
	{
	  mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"].setEfficiencyForEvent(false, muonRecTrack.Pt,muonRecTrack.Eta,muonRecTrack.Phi);
	}
    }
}


void MSMuonAnalysis::writeResults()
{
  vector<string> combNames;
  string name;
  double Eff;
  double EffError;

  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerPerformance");

  // Writing Graphs
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerPerformance/"+vecTrackContainerNames[n]);

      ///////////// Efficiencies
      //   Graph for vecTrackContainerNames[n]_Efficiency_Pt 
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Efficiency_Pt",mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 1);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Efficiency_Pt");

      //   Graph for vecTrackContainerNames[n]_Efficiency_Eta 
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Efficiency_Eta",mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 2);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Efficiency_Eta");

      //   Graph for vecTrackContainerNames[n]_Efficiency_Phi 
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Efficiency_Phi",mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 3);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Efficiency_Phi");

      // histogram version of graphs (added by Mass. Bellomo)
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Efficiency_Pt_h", mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 1);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Efficiency_Pt_h");
      
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Efficiency_Eta_h",mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 2);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Efficiency_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Efficiency_Phi_h", mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"], 3);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Efficiency_Phi_h");
      /////////////////////////////////////// 

      ///////////// FakeRates
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_FakeRate_Pt", mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 1);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_FakeRate_Pt");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_FakeRate_Eta", mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 2);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_FakeRate_Eta");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_FakeRate_Phi",mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 3);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_FakeRate_Phi");

      // histogram version of graphs
      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_FakeRate_Pt_h",mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 1);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_FakeRate_Pt_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_FakeRate_Eta_h",mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 2);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_FakeRate_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_FakeRate_Phi_h",mapEfficiency[vecTrackContainerNames[n]+"_FakeRate"], 3);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_FakeRate_Phi_h");
      /////////////////////////////////////// 

      ///////////// Triggered_Efficiencies and _NotTriggered_Efficiency
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 1);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 2);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 3);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 1);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 2);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 3);
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi");

      // histogram version of graphs

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt_h",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 1);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta_h",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 2);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi_h",mapEfficiency[vecTrackContainerNames[n]+"_Triggered_Efficiency"], 3);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt_h",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 1);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta_h",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 2);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi_h",mapEfficiency[vecTrackContainerNames[n]+"_NotTriggered_Efficiency"], 3);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi_h");
      ///////////////////////////////////////

      for (double dn=-2.55; dn<=2.6; dn+=0.1)
	for (double dp=-3.05; dp<=3.1; dp+=0.1)
	  {
	    mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"].getEfficiency23(dn, dp, Eff, EffError);
	    msRootInterface.fill2D(vecTrackContainerNames[n]+"_Eta_Phi_Efficiency", dn, dp, Eff);
	  }

      msRootInterface.write2DHistogram(vecTrackContainerNames[n]+"_Eta_Phi_Efficiency");

      ///////////// Resolutions Widths ROOT Graphs
      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Width_Pt",mapResolution[vecTrackContainerNames[n]], "Pt", "GausSigma");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Width_Pt");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Width_Eta",	mapResolution[vecTrackContainerNames[n]], "Eta", "GausSigma");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Width_Eta");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Width_Phi",mapResolution[vecTrackContainerNames[n]], "Phi", "GausSigma");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Width_Phi");
 
      // histogram version of graphs

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Width_Pt_h",mapResolution[vecTrackContainerNames[n]], "Pt", "GausSigma");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Pt_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Width_Eta_h",mapResolution[vecTrackContainerNames[n]], "Eta", "GausSigma");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Width_Phi_h",mapResolution[vecTrackContainerNames[n]], "Phi", "GausSigma");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Width_Phi_h");
      ///////////////////////////////////////

      ///////////// Resolutions Mean ROOT Graphs

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Pt",mapResolution[vecTrackContainerNames[n]], "Pt", "MeanValue");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Pt");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Eta",mapResolution[vecTrackContainerNames[n]], "Eta", "MeanValue");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Eta");

      msRootInterface.setGraphPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Phi",mapResolution[vecTrackContainerNames[n]], "Phi", "MeanValue");
      msRootInterface.writeGraph(vecTrackContainerNames[n]+"_Resolution_Mean_Phi");
		
      // histogram version of graph

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Pt_h",mapResolution[vecTrackContainerNames[n]], "Pt", "MeanValue");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Pt_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Eta_h",mapResolution[vecTrackContainerNames[n]], "Eta", "MeanValue");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Eta_h");

      msRootInterface.setHistogramPoints(vecTrackContainerNames[n]+"_Resolution_Mean_Phi_h", mapResolution[vecTrackContainerNames[n]], "Phi", "MeanValue");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Resolution_Mean_Phi_h");

      // OverAll track efficiency
      double Eff;
      double EffError;
      mapEfficiency[vecTrackContainerNames[n]+"_Efficiency"].getOverallEfficiency(Eff, EffError);
      msRootInterface.fill(vecTrackContainerNames[n]+"_Efficiency_Overall", 1., Eff);
      msRootInterface.fill(vecTrackContainerNames[n]+"_EfficiencyUncertainty_Overall", 1., EffError);
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Efficiency_Overall");
      msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_EfficiencyUncertainty_Overall");

      ///////////////////////////////////////
		
      combNames.clear();
      combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Eta");
      combNames.push_back(vecTrackContainerNames[n]+"_Triggered_Efficiency_Eta");
      combNames.push_back(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Eta");
      msRootInterface.writeComparisionGraph(combNames, "Trigger Aware Comparison "+vecTrackContainerNames[n]+" (Efficiency vs. Eta)");

      combNames.clear();
      combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Phi");
      combNames.push_back(vecTrackContainerNames[n]+"_Triggered_Efficiency_Phi");
      combNames.push_back(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Phi");
      msRootInterface.writeComparisionGraph(combNames, "Trigger Aware Comparison "+vecTrackContainerNames[n]+" (Efficiency vs. Phi)");
	
      combNames.clear();
      combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Pt");
      combNames.push_back(vecTrackContainerNames[n]+"_Triggered_Efficiency_Pt");
      combNames.push_back(vecTrackContainerNames[n]+"_NotTriggered_Efficiency_Pt");
      msRootInterface.writeComparisionGraph(combNames, "Trigger Aware Comparison "+vecTrackContainerNames[n]+" (Efficiency vs. Pt)");
    } // for loop Writing Graphs

  // Trigger Information
  for (int n=0; n<(int)vecTriggerContainerNames.size(); n++)
    {
      ///////////// TriggerContainerNames_Efficiencies ROOT Graphs
      msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerPerformance/"+vecTriggerContainerNames[n]);
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_MetricDistance");

      msRootInterface.setGraphPoints(vecTriggerContainerNames[n]+"_Efficiency_Pt",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 1);
      msRootInterface.writeGraph(vecTriggerContainerNames[n]+"_Efficiency_Pt");

      msRootInterface.setGraphPoints(vecTriggerContainerNames[n]+"_Efficiency_Eta",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 2);
      msRootInterface.writeGraph(vecTriggerContainerNames[n]+"_Efficiency_Eta");

      msRootInterface.setGraphPoints(vecTriggerContainerNames[n]+"_Efficiency_Phi",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 3);
      msRootInterface.writeGraph(vecTriggerContainerNames[n]+"_Efficiency_Phi");

      // histogram version of graphs
		
      msRootInterface.setHistogramPoints(vecTriggerContainerNames[n]+"_Efficiency_Pt_h",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 1);
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_Efficiency_Pt_h");

      msRootInterface.setHistogramPoints(vecTriggerContainerNames[n]+"_Efficiency_Eta_h",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 2);
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_Efficiency_Eta_h");

      msRootInterface.setHistogramPoints(vecTriggerContainerNames[n]+"_Efficiency_Phi_h",mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"], 3);
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_Efficiency_Phi_h");

      for (double dn=-2.55; dn<=2.6; dn+=0.1)
	for (double dp=-3.05; dp<=3.1; dp+=0.1)
	  {
	    mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"].getEfficiency23(dn,dp,Eff,EffError);
	    msRootInterface.fill2D(vecTriggerContainerNames[n]+"_Eta_Phi_Efficiency", dn, dp, Eff);
	  }
      msRootInterface.write2DHistogram(vecTriggerContainerNames[n]+"_Eta_Phi_Efficiency");
		
      // OverAll trigger efficiency
      double Eff;
      double EffError;
      mapEfficiency[vecTriggerContainerNames[n]+"_Efficiency"].getOverallEfficiency(Eff, EffError);
      msRootInterface.fill(vecTriggerContainerNames[n]+"_Efficiency_Overall", 1., Eff);
      msRootInterface.fill(vecTriggerContainerNames[n]+"_EfficiencyUncertainty_Overall", 1., EffError);
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_Efficiency_Overall");
      msRootInterface.writeHistogram(vecTriggerContainerNames[n]+"_EfficiencyUncertainty_Overall");
    }

  // Writing Combined Graphs
  msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerPerformance");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Resolution_Width_Eta");
  msRootInterface.writeComparisionGraph(combNames, "Resolution_Width_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Resolution_Width_Pt");
  msRootInterface.writeComparisionGraph(combNames, "Resolution_Width_Pt");
	
  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Resolution_Mean_Eta");
  msRootInterface.writeComparisionGraph(combNames, "Resolution_Mean_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Resolution_Mean_Pt");
  msRootInterface.writeComparisionGraph(combNames, "Resolution_Mean_Pt");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Eta");
  msRootInterface.writeComparisionGraph(combNames, "Efficiency_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Pt");
  msRootInterface.writeComparisionGraph(combNames, "Efficiency_Pt");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_Efficiency_Phi");
  msRootInterface.writeComparisionGraph(combNames, "Efficiency_Phi");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_FakeRate_Eta");
  msRootInterface.writeComparisionGraph(combNames, "FakeRate_Eta");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_FakeRate_Pt");
  msRootInterface.writeComparisionGraph(combNames, "FakeRate_Pt");

  combNames.clear();
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++) combNames.push_back(vecTrackContainerNames[n]+"_FakeRate_Phi");
  msRootInterface.writeComparisionGraph(combNames, "FakeRate_Phi");

  // Writing Histograms
  for (int n=0; n<(int)vecTrackContainerNames.size(); n++)
    {
      if (vecTrackContainerNames[n]!="TrackParticleCandidate")
	{
	  msRootInterface.setDirectory(m_Outputfile, "MuonSpectrometerPerformance/"+vecTrackContainerNames[n]);

	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_MetricDistance_EndCap");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_MetricDistance_Barrel");

	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_pT_Resolution");

	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_pT_Resolution_Eta_EndCap");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_pT_Resolution_Eta_Barrel");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Eta_Resolution");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Phi_Resolution");

	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_NumberOfMDTHits");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_chi2oDOF");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Charge");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_ETCone45");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_chi2");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_EnergyLoss");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Status");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_RealDOF");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_FakeDOF");

	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_ImpactParameter_Z0_Resolution");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_ImpactParameter_D0_Resolution");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Pull_Phi_Resolution");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Pull_Eta_Resolution");
	  msRootInterface.writeHistogram(vecTrackContainerNames[n]+"_Pull_Pt_Resolution");

	} // if TrackParticleCandidate
    } // for loop vecTrackContainerNames

  msRootInterface.setDirectory(m_Outputfile,"MuonSpectrometerPerformance/TruthInfo");
  msRootInterface.writeHistogram("TruthInfo_TrueMuon_Pt");
  msRootInterface.writeHistogram("TruthInfo_TrueMuon_Eta");
  msRootInterface.writeHistogram("TruthInfo_TrueMuon_Phi");
}// end of writeResults class


int MSMuonAnalysis::getSector(double phi)
{
  phi=(phi*360.)/(2.0*3.14159265);
  if ((phi>-15.0) && (phi<+15.0)) return 1;
  if ((phi>+15.0) && (phi<+30.0)) return 2;
  if ((phi>+30.0) && (phi<+60.0)) return 3;
  if ((phi>+60.0) && (phi<+75.0)) return 4;
  if ((phi>+75.0) && (phi<+105.0)) return 5;
  if ((phi>+105.0) && (phi<+120.0)) return 6;
  if ((phi>+120.0) && (phi<+150.0)) return 7;
  if ((phi>+150.0) && (phi<+165.0)) return 8;
  if ((phi>+165.0) || (phi<-165.0)) return 9;
  if ((phi<-15.0) && (phi>-30.0)) return 10;
  if ((phi<-30.0) && (phi>-60.0)) return 11;
  if ((phi<-60.0) && (phi>-75.0)) return 12;
  if ((phi<-75.0) && (phi>-105.0)) return 13;
  if ((phi<-105.0) && (phi>-120.0)) return 14;
  if ((phi<-120.0) && (phi>-150.0)) return 15;
  if ((phi<-150.0) && (phi>-165.0)) return 16;
  return -1;
}

 

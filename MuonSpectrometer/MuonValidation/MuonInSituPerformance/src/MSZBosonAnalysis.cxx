/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI) - M.Bellomo (INFN) 
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on MSEvent.
//		It is derivated from MSPhysicsAnalysis
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonInSituPerformance/MSZBosonAnalysis.h"

// Default Constructor - Empty
MSZBosonAnalysis::MSZBosonAnalysis()
{
}

void	MSZBosonAnalysis::setup(std::string outputname, std::string RootDirName, cutDefinitons cuts, std::string MuonContainerName)
{
  MSPhysicsAnalysis::setup(outputname, RootDirName, cuts);
  m_MuonContainer = MuonContainerName;
  setupHistograms();
}

void	MSZBosonAnalysis::setupHistograms()
{
  msRootInterface.setNewHistogram("True_NumberOfMuons","True_NumberOfMuons","Number of Muons >6GeV","Number of Entries",100,0,100);
  msRootInterface.setNewHistogram("True_ZBosonMass","True_ZBosonMass","Z Boson Mass [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("True_DiMuonMass","True_DiMuonMass","Z Boson Mass [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("True_ZBosonPt","True_ZBosonPt","Z Boson Pt [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("True_MuonPt","True_Muon_Pt","Z Muon Pt [GeV]","Number of Entries",130,0,100000);
  msRootInterface.setNewHistogram("Reco_NumberOfTracks","Reco_NumberOfTracks","Number of Tracks","Number of Entries",100,0,100);
  msRootInterface.setNewHistogram("Reco_DiMuonMass","Reconstructed_DiMuonMass","DiMuon Mass [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("Reco_DiMuonPt","Reconstructed_DiMuonPt","DiMuon Pt [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("Reco_PtMin","Reconstructed_Minimal_Pt","Minimal Muon Pt [GeV]","Number of Entries",130,0,100000);
  msRootInterface.setNewHistogram("Reco_PtMax","Reconstructed_Maxomal_Pt","Maximal Muon Pt [GeV]","Number of Entries",130,0,100000);
  msRootInterface.setNewHistogram("Reco_DeltaPhi","Reconstructed_Delta_Phi","#Delta #phi","Number of Entries",32,0.0,3.2);
  msRootInterface.setNewHistogram("Reco_DeltaEta","Reconstructed_Delta_Eta","#Delta #eta","Number of Entries",40,0.0,4.0);
  msRootInterface.setNewHistogram("Reco_Charge","Reconstructed_Charge","Charge_{1}*Charge_{2}","Number of Entries",8,-2,2);
  msRootInterface.setNewHistogram("Selected_DiMuonMass","Selected_DiMuonMass","DiMuon Mass [GeV]","Number of Entries",130,0,130000);
  msRootInterface.setNewHistogram("Reco_NumberOfTracks_InHalo","Reconstructed_Number_Of_Tracks_InHalo","Number Of Tracks In Halo","Number of Entries",20,0,20);
  msRootInterface.setNewHistogram("Reco_JetEnergy_InHalo","Reco_JetEnergy_InHalo","Jet Energy in Halo around Muon [GeV]","Number of Entries",100,0,50000);
  msRootInterface.setNewHistogram("Reco_TrackPt_InHalo","Reco_TrackPt_InHalo","Sum of Track p_{T}'s in Halo around Muon [GeV]","Number of Entries",100,0,50000);
  msRootInterface.setNewHistogram("Reco_ET_InHalo","Reco_ET_InHalo","ET in Halo around Muon [GeV]","Number of Entries",100,0,50000);
  msRootInterface.setNewHistogram("Cut_Efficiency","Cut_Efficiency","Number of Events with (0:AllEvents, 1:DetectableZs,2:IdentifiedZ's,3:FakeZ's,4:TwoPossibleZ's)","Number of Entries",10,0,10);
  msRootInterface.setNewHistogram("Cut_Flow","Cut_Flow","CutFlow (0:TwoTracks,1:Charge,2:Mass,3:Ptmax,4:ptmin,5:phi,6:iso)","Number of Entries",10,0,10);
}


MSZBosonAnalysis::~MSZBosonAnalysis()
{
}

void MSZBosonAnalysis::writeResults()
{
  std::string name;

  msRootInterface.setDirectory(m_Outputfile, m_RootDirName);
  msRootInterface.writeHistogram("True_NumberOfMuons");
  msRootInterface.writeHistogram("True_ZBosonMass");
  msRootInterface.writeHistogram("True_DiMuonMass");
  msRootInterface.writeHistogram("True_ZBosonPt");
  msRootInterface.writeHistogram("True_MuonPt");
  msRootInterface.writeHistogram("Reco_NumberOfTracks");
  msRootInterface.writeHistogram("Reco_NumberOfJets");
  msRootInterface.writeHistogram("Reco_DiMuonMass");
  msRootInterface.writeHistogram("Reco_DiMuonPt");
  msRootInterface.writeHistogram("Reco_PtMin");
  msRootInterface.writeHistogram("Reco_PtMax");
  msRootInterface.writeHistogram("Reco_DeltaPhi");
  msRootInterface.writeHistogram("Reco_DeltaEta");
  msRootInterface.writeHistogram("Reco_NumberOfTracks_InHalo");
  msRootInterface.writeHistogram("Reco_JetEnergy_InHalo");
  msRootInterface.writeHistogram("Reco_TrackPt_InHalo");
  msRootInterface.writeHistogram("Reco_ET_InHalo");
  msRootInterface.writeHistogram("Cut_Efficiency");
  msRootInterface.writeHistogram("Cut_Flow");
  msRootInterface.writeHistogram("Selected_DiMuonMass");
}


void MSZBosonAnalysis::analyseMCTruth(MSEvent &msEvent)
{
  double		distance;
  long 			NumberOfMuons;
  MSTrackObject		msParticle;

  True_ZMuon1.clear();
  True_ZMuon2.clear();

  NumberOfMuons=0;
  msEvent.setupParticleCounter();
  while (msEvent.getNextParticle(msParticle)==true)
    {
      if ((abs(msParticle.PdgID)==23) && (True_ZBoson.isInitialized()==false)) // isInitialized() method in MSPhysicsObject class
	{
	  True_ZBoson=msParticle;
	  True_ZBoson.initialise();
	}
      if (abs(msParticle.PdgID)==13)	NumberOfMuons++;
      if ((abs(msParticle.PdgID)==13) && (msParticle.Pt>10000.)) //&& (msParticle.Mother()==1))
	{
	  if (True_ZMuon1.isInitialized()==false)
	    {
	      True_ZMuon1=msParticle;
	      True_ZMuon1.initialise();
	    } else
	      {
		if (True_ZMuon1.match(msParticle,0.05,distance)==false)
		  {
		    True_ZMuon2=msParticle;
		    True_ZMuon2.initialise();
		  }
	      }
	}
    }

  msRootInterface.fill("True_DiMuonMass",getDiMuonMass(True_ZMuon1, True_ZMuon2));
  msRootInterface.fill("True_NumberOfMuons",NumberOfMuons);
  if (True_ZBoson.isInitialized()==true)
    {
      msRootInterface.fill("True_ZBosonMass",True_ZBoson.AnalysisParameters.dMass);
      msRootInterface.fill("True_ZBosonPt",True_ZBoson.Pt);
    }
  if (True_ZMuon1.isInitialized()==true)	msRootInterface.fill("True_MuonPt",True_ZMuon1.Pt);
  if (True_ZMuon2.isInitialized()==true)	msRootInterface.fill("True_MuonPt",True_ZMuon2.Pt);
}



void MSZBosonAnalysis::analyse(MSEvent &msEvent)
{
  analyseMCTruth(msEvent);

  double	distance	= 0.0;
  int	nNumberOfZs	= 0;

  Rec_Muon1.clear();
  Rec_Muon2.clear();

  msRootInterface.fill("Cut_Efficiency",0.5);

  msEvent.setDefaultTrackReconstruction("StacoMuonCollection");
  if (msEvent.getNumberOfTracks()<2) return;
  msRootInterface.fill("Reco_NumberOfTracks",msEvent.getNumberOfTracks());

  msEvent.setupTrackCombination();

  while (msEvent.getNextTrackCombination(Rec_Muon1, Rec_Muon2)==true)
    {
      double	dInvMass= getDiMuonMass(Rec_Muon1, Rec_Muon2);
      double	dInvPt	= getDiMuonPt(Rec_Muon1, Rec_Muon2);
      double	dDeltaPhi= getDeltaPhi(Rec_Muon1, Rec_Muon2);
      double	dMinPt	= Rec_Muon1.Pt < Rec_Muon2.Pt ? Rec_Muon1.Pt : Rec_Muon2.Pt;
      double	dMaxPt	= Rec_Muon1.Pt < Rec_Muon2.Pt ? Rec_Muon2.Pt : Rec_Muon1.Pt;
      int	iso1	= getIsolation(Rec_Muon1);
      int	iso2	= getIsolation(Rec_Muon2);
      msRootInterface.fill("Reco_DiMuonMass",dInvMass);
      msRootInterface.fill("Reco_DiMuonPt",dInvPt);
      msRootInterface.fill("Reco_PtMin",dMinPt);
      msRootInterface.fill("Reco_PtMax",dMaxPt);
      msRootInterface.fill("Reco_DeltaPhi",dDeltaPhi);
      msRootInterface.fill("Reco_Charge",Rec_Muon1.Charge*Rec_Muon2.Charge);
      msRootInterface.fill("Reco_JetEnergy_InHalo",		Rec_Muon1.AnalysisParameters.dJetIsolation);
      msRootInterface.fill("Reco_NumberOfTracks_InHalo",	Rec_Muon1.AnalysisParameters.dNTrackConeIsolation);
      msRootInterface.fill("Reco_TrackPt_InHalo",		Rec_Muon1.AnalysisParameters.dTrackPtConeIsolation);
      msRootInterface.fill("Reco_ET_InHalo",			Rec_Muon1.AnalysisParameters.dETIsolation);
      msRootInterface.fill("Reco_JetEnergy_InHalo",		Rec_Muon2.AnalysisParameters.dJetIsolation);
      msRootInterface.fill("Reco_NumberOfTracks_InHalo",	Rec_Muon2.AnalysisParameters.dNTrackConeIsolation);
      msRootInterface.fill("Reco_TrackPt_InHalo",		Rec_Muon2.AnalysisParameters.dTrackPtConeIsolation);
      msRootInterface.fill("Reco_ET_InHalo",			Rec_Muon2.AnalysisParameters.dETIsolation);

      /*		if 	((True_ZMuon1.match(Rec_Muon1,0.5,distance) ||
			(True_ZMuon1.match(Rec_Muon2,0.5,distance))) &&
			(True_ZMuon2.match(Rec_Muon1,0.5,distance) ||
			(True_ZMuon2.match(Rec_Muon2,0.5,distance))))
			{
			msRootInterface.fill("Cut_Efficiency",1.5);
			}*/

      msRootInterface.fill("Cut_Flow",0.5);

      if (Rec_Muon1.Charge*Rec_Muon2.Charge<0.0)
	{
	  msRootInterface.fill("Cut_Flow",1.5);
	  if (fabs(dInvMass-91000.)<cutD.diMuonMass)
	    {
	      msRootInterface.fill("Cut_Flow",2.5);
	      if (dMaxPt>cutD.Pt_tight)
		{
		  msRootInterface.fill("Cut_Flow",3.5);
		  if (dMinPt>cutD.Pt_loose)
		    {
		      msRootInterface.fill("Cut_Flow",4.5);
		      if (dDeltaPhi>cutD.deltaPhi)
			{
			  msRootInterface.fill("Cut_Flow",5.5);
			  if ((iso1>=1) && (iso2>=1))
			    {
			      msRootInterface.fill("Cut_Flow",6.5);
			      if ((iso1>=2) || (iso2>=2))
				{
				  if (nNumberOfZs==0)
				    {
				      Rec_ZMuon1=Rec_Muon1;
				      Rec_ZMuon2=Rec_Muon2;
				      nNumberOfZs++;
				      msRootInterface.fill("Selected_DiMuonMass", dInvMass);
				    }
				  else
				    {
				      if (	((Rec_Muon1.match(Rec_ZMuon1,0.5,distance)==false) &&
						 (Rec_Muon1.match(Rec_ZMuon2,0.5,distance)==false)) ||
						((Rec_Muon2.match(Rec_ZMuon1,0.5,distance)==false) &&
						 (Rec_Muon2.match(Rec_ZMuon2,0.5,distance)==false)))
					{
					  nNumberOfZs++;
					  double	dInvMass_Old= getDiMuonMass(Rec_ZMuon1, Rec_ZMuon2);
					  if (fabs(91000.-dInvMass) < fabs(91000.-dInvMass_Old))
					    {
					      Rec_ZMuon1=Rec_Muon1;
					      Rec_ZMuon2=Rec_Muon2;
					    }
					}

				    }
				}
			    }
			}
		    }
		}
	    } //if (fabs(dInvMass-91000.)<cutD.diMuonMass)
	}// if (Rec_Muon1.Charge*Rec_Muon2.Charge<0.0)
    }

  if (nNumberOfZs>1) 	msRootInterface.fill("Cut_Efficiency",4.5);
  if (nNumberOfZs!=0)
    {
      if 	((True_ZMuon1.match(Rec_ZMuon1,0.5,distance) ||
		  (True_ZMuon1.match(Rec_ZMuon2,0.5,distance))) &&
		 (True_ZMuon2.match(Rec_ZMuon1,0.5,distance) ||
		  (True_ZMuon2.match(Rec_ZMuon2,0.5,distance))))
	{
	  msRootInterface.fill("Cut_Efficiency",2.5);
	} else
	  {
	    msRootInterface.fill("Cut_Efficiency",3.5);
	  }
    }
}
 

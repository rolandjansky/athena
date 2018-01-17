/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Apr 12 18:26:46 2008 by ROOT version 5.18/00a
// from TTree PixelNtupleTracks/Track Validation
// found on file: castor:///castor/cern.ch/user/s/smontesa/FDR2/SErun3076_PixelCalibration.root
//////////////////////////////////////////////////////////

#ifndef PixelNtupleTracks_h
#define PixelNtupleTracks_h

#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <TROOT.h>
#include <TChain.h>

class PixelNtupleTracks {
	
	public :
		TTree          *fChain;   //!pointer to the analyzed TTree or TChain
		Int_t           fCurrent; //!current Tree number in a TChain

		// Declaration of leaf types
		//Int_t           RunNumber;
		//Int_t           EventNumber;
		//Int_t           TrackID;
		//Int_t           IterationIndex;
		//Int_t           nHits;
		//Int_t           TrackStatesUnbiased;
		//Float_t         RecD0;
		//Float_t         RecZ0;
		//Float_t         RecPhi0;
		Float_t         RecTheta;
		//Float_t         RecEta;
		Float_t         RecQoverP;
		//Float_t         RecErrD0;
		//Float_t         RecErrZ0;
		//Float_t         RecErrPhi0;
		//Float_t         RecErrTheta;
		//Float_t         RecErrQoverP;
		//Float_t         Chi2overNdof;
		//Int_t           Ndof;
		//Int_t           nHoles;
		//std::vector<int>     *HoleDetectorType;
		//std::vector<float>   *pullLocX;
		//std::vector<float>   *pullLocY;
		//std::vector<float>   *residualLocX;
		//std::vector<float>   *residualLocY;
		//std::vector<int>     *DetectorType;
		//std::vector<int>     *outlierFlag;
		//Int_t           FitterStatusCode;
		Int_t           nPixelHits;
		Int_t           nSCTHits;
		Int_t           nTRTHits;
		//Int_t           nMDTHits;
		//Int_t           nCSCHits;
		//Int_t           nRPCHits;
		//Int_t           nTGCHits;
		std::vector<float>   *PixPullPhi;
		std::vector<float>   *PixPullEta;
		std::vector<float>   *PixResPhi;
		std::vector<float>   *PixResEta;
		std::vector<int>     *PixDetType;
		//std::vector<int>     *PixHasGanged;
		//std::vector<int>     *PixIsFake;
		//std::vector<int>     *PixToT;
		std::vector<float>   *PixCharge;
		std::vector<float>   *PixTLorPhi;
		//std::vector<float>   *PixTLorEta;
		std::vector<float>   *PixBiasVolt;
		//std::vector<float>   *PixDepVolt;
		std::vector<float>   *PixDigResPhi;
		std::vector<float>   *PixDigResEta;
		std::vector<float>   *PixErrPhi;
		std::vector<float>   *PixErrEta;
		//std::vector<float>   *PixLocX;
		//std::vector<float>   *PixLocY;
		std::vector<int>     *PixEtaModule;
		std::vector<int>     *PixPhiModule;
		//std::vector<float>   *PixGloX;
		//std::vector<float>   *PixGloY;
		//std::vector<float>   *PixGloZ;
		//std::vector<float>   *PixEta;
		//std::vector<float>   *PixFirstRow;
		//std::vector<float>   *PixFirstCol;
		std::vector<int>     *PixDeltaRow;
		std::vector<int>     *PixDeltaCol;
		//std::vector<float>   *PixDeltaPhi;
		//std::vector<float>   *PixDeltaEta;
		std::vector<float>   *PixOmegaPhi;
		std::vector<float>   *PixOmegaEta;
		std::vector<float>   *PixTrkAngle;
		std::vector<float>   *PixTrkEta;
		std::vector<float>   *PixTrkPt;
		//std::vector<float>   *PixTrkQ;
		//std::vector<float>   *PixHoleGloX;
		//std::vector<float>   *PixHoleGloY;
		//std::vector<float>   *PixHoleGloZ;
		//std::vector<float>   *PixHoleLocX;
		//std::vector<float>   *PixHoleLocY;
		//std::vector<int>     *PixHoleEtaModule;
		//std::vector<int>     *PixHolePhiModule;
		//std::vector<float>   *PixHoleLayer;
		//std::vector<float>   *PixHoleECBarrel;
		//std::vector<int>     *PixHoleClNearstSize;
		//std::vector<float>   *PixHoleLocXClNearst;
		//std::vector<float>   *PixHoleLocYClNearst;
		//std::vector<float>   *PixHoleClRowClNearst;
		//std::vector<float>   *PixHoleClColClNearst;
		//std::vector<float>   *PixHoleGloZNearst;
		//std::vector<float>   *PixHoleDeltaRowNearst;
		//std::vector<float>   *PixHoleDeltaColNearst;
		//std::vector<float>   *SCTPull;
		//std::vector<float>   *SCTRes;
		//std::vector<int>     *SctDetType;
		//std::vector<float>   *SctTLorPhi;
		//std::vector<float>   *SctTLorEta;
		//std::vector<float>   *SctEta;
		//std::vector<float>   *SctDeltaPhi;
		//std::vector<float>   *SctTrkAngle;
		//std::vector<float>   *SctTrkQ;
		//std::vector<float>   *TRTPull;
		//std::vector<float>   *TRTRes;
		//std::vector<float>   *HitX;
		//std::vector<float>   *HitY;
		//std::vector<float>   *HitZ;
		//std::vector<float>   *HitRadius;
		//std::vector<float>   *HitPhi;
		//std::vector<float>   *HitLocal1;
		//std::vector<float>   *HitLocal2;
		//std::vector<float>   *HitSurfaceX;
		//std::vector<float>   *HitSurfaceY;
		//std::vector<float>   *HitSurfaceZ;
		//std::vector<float>   *HitSurfaceRadius;
		//std::vector<float>   *HitSurfacePhi;
		//std::vector<float>   *HitSurfaceTheta;
		//std::vector<float>   *TrackX;
		//std::vector<float>   *TrackY;
		//std::vector<float>   *TrackZ;
		//std::vector<float>   *TrackMomentumX;
		//std::vector<float>   *TrackMomentumY;
		//std::vector<float>   *TrackMomentumZ;
		//std::vector<float>   *TrackLocal1;
		//std::vector<float>   *TrackLocal2;

		// List of branches
		//TBranch        *b_run_number;   //!
		//TBranch        *b_event_number;   //!
		//TBranch        *b_ID_of_track_in_event;   //!
		//TBranch        *b_Iteration_of_a_track;   //!
		//TBranch        *b_Number_of_measurements_on_track;   //!
		//TBranch        *b_are_track_states_unbiased;   //!
		//TBranch        *b_Reconstructed_d0;   //!
		//TBranch        *b_Reconstructed_z0;   //!
		//TBranch        *b_Reconstructed_phi0;   //!
		TBranch        *b_Reconstructed_theta;   //!
		//TBranch        *b_Reconstructed_eta;   //!
		TBranch        *b_Reconstructed_Q_over_p;   //!
		//TBranch        *b_err_d0;   //!
		//TBranch        *b_err_z0;   //!
		//TBranch        *b_err_phi0;   //!
		//TBranch        *b_err_theta;   //!
		//TBranch        *b_err_Q_over_p;   //!
		//TBranch        *b_Reco_Chi2_over_Ndof;   //!
		//TBranch        *b_Reco_Ndof;   //!
		//TBranch        *b_Number_of_holes_on_track;   //!
		//TBranch        *b_HoleDetectorType;   //!
		//TBranch        *b_pullLocX;   //!
		//TBranch        *b_pullLocY;   //!
		//TBranch        *b_residualLocX;   //!
		//TBranch        *b_residualLocY;   //!
		//TBranch        *b_DetectorType;   //!
		//TBranch        *b_outlierFlag;   //!
		//TBranch        *b_FitterStatusCode;   //!
		TBranch        *b_number_Pixel_measurements;   //!
		TBranch        *b_number_SCT_measurements;   //!
		TBranch        *b_number_TRT_measurements;   //!
		//TBranch        *b_number_MDT_measurements;   //!
		//TBranch        *b_number_CSC_measurements;   //!
		//TBranch        *b_number_RPC_measurements;   //!
		//TBranch        *b_number_TGC_measurements;   //!
		TBranch        *b_PixPullPhi;   //!
		TBranch        *b_PixPullEta;   //!
		TBranch        *b_PixResPhi;   //!
		TBranch        *b_PixResEta;   //!
		TBranch        *b_PixDetType;   //!
		//TBranch        *b_PixHasGanged;   //!
		//TBranch        *b_PixIsFake;   //!
		//TBranch        *b_PixToT;   //!
		TBranch        *b_PixCharge;   //!
		TBranch        *b_PixTLorPhi;   //!
		//TBranch        *b_PixTLorEta;   //!
		TBranch        *b_PixBiasVolt;   //!
		//TBranch        *b_PixDepVolt;   //!
		TBranch        *b_PixDigResPhi;   //!
		TBranch        *b_PixDigResEta;   //!
		TBranch        *b_PixErrPhi;   //!
		TBranch        *b_PixErrEta;   //!
		//TBranch        *b_PixLocX;   //!
		//TBranch        *b_PixLocY;   //!
		TBranch        *b_PixEtaModule;   //!
		TBranch        *b_PixPhiModule;   //!
		//TBranch        *b_PixGloX;   //!
		//TBranch        *b_PixGloY;   //!
		//TBranch        *b_PixGloZ;   //!
		//TBranch        *b_PixEta;   //!
		//TBranch        *b_PixFirstRow;   //!
		//TBranch        *b_PixFirstCol;   //!
		TBranch        *b_PixDeltaRow;   //!
		TBranch        *b_PixDeltaCol;   //!
		//TBranch        *b_PixDeltaPhi;   //!
		//TBranch        *b_PixDeltaEta;   //!
		TBranch        *b_PixOmegaPhi;   //!
		TBranch        *b_PixOmegaEta;   //!
		TBranch        *b_PixTrkAngle;   //!
		TBranch        *b_PixTrkEta;   //!
		TBranch        *b_PixTrkPt;   //!
		//TBranch        *b_PixTrkQ;   //!
		//TBranch        *b_PixHoleGloX;   //!
		//TBranch        *b_PixHoleGloY;   //!
		//TBranch        *b_PixHoleGloZ;   //!
		//TBranch        *b_PixHoleLocX;   //!
		//TBranch        *b_PixHoleLocY;   //!
		//TBranch        *b_PixHoleEtaModule;   //!
		//TBranch        *b_PixHolePhiModule;   //!
		//TBranch        *b_PixHoleLayer;   //!
		//TBranch        *b_PixHoleECBarrel;   //!
		//TBranch        *b_PixHoleClNearstSize;   //!
		//TBranch        *b_PixHoleLocXClNearst;   //!
		//TBranch        *b_PixHoleLocYClNearst;   //!
		//TBranch        *b_PixHoleClRowClNearst;   //!
		//TBranch        *b_PixHoleClColClNearst;   //!
		//TBranch        *b_PixHoleGloZNearst;   //!
		//TBranch        *b_PixHoleDeltaRowNearst;   //!
		//TBranch        *b_PixHoleDeltaColNearst;   //!
		//TBranch        *b_SCTPull;   //!
		//TBranch        *b_SCTRes;   //!
		//TBranch        *b_SctDetType;   //!
		//TBranch        *b_SctTLorPhi;   //!
		//TBranch        *b_SctTLorEta;   //!
		//TBranch        *b_SctEta;   //!
		//TBranch        *b_SctDeltaPhi;   //!
		//TBranch        *b_SctTrkAngle;   //!
		//TBranch        *b_SctTrkQ;   //!
		//TBranch        *b_TRTPull;   //!
		//TBranch        *b_TRTRes;   //!
		//TBranch        *b_HitX;   //!
		//TBranch        *b_HitY;   //!
		//TBranch        *b_HitZ;   //!
		//TBranch        *b_HitRadius;   //!
		//TBranch        *b_HitPhi;   //!
		//TBranch        *b_HitLocal1;   //!
		//TBranch        *b_HitLocal2;   //!
		//TBranch        *b_HitSurfaceX;   //!
		//TBranch        *b_HitSurfaceY;   //!
		//TBranch        *b_HitSurfaceZ;   //!
		//TBranch        *b_HitSurfaceRadius;   //!
		//TBranch        *b_HitSurfacePhi;   //!
		//TBranch        *b_HitSurfaceTheta;   //!
		//TBranch        *b_TrackX;   //!
		//TBranch        *b_TrackY;   //!
		//TBranch        *b_TrackZ;   //!
		//TBranch        *b_TrackMomentumX;   //!
		//TBranch        *b_TrackMomentumY;   //!
		//TBranch        *b_TrackMomentumZ;   //!
		//TBranch        *b_TrackLocal1;   //!
		//TBranch        *b_TrackLocal2;   //!

		PixelNtupleTracks(std::string input,std::string collection);
		virtual ~PixelNtupleTracks();
		virtual Int_t    GetEntry(Long64_t entry);
		virtual Long64_t LoadTree(Long64_t entry);
		virtual void     Init(TTree *tree);
		virtual void     Loop();
		virtual Bool_t   Notify();
		virtual void     Show(Long64_t entry = -1);

	// inline Functions to access variables: Maybe should be moved to PixelNtupleTracks??
	inline double GetGeVpT(int ihit){
		return (*PixTrkPt)[ihit] / 1000.;
	}

	inline double GetGeVGlobalpT(){ return fabs(sin(RecTheta)/RecQoverP)/1000;}
	
	inline double GetLorAlpha(int ihit){
		return 180. * atan((*PixTLorPhi)[ihit]) / M_PI;
		/*double LorAngle = 0;
		if( GetDetType(ihit)  < 3) LorAngle = -180. * 0.224/ M_PI;
		return LorAngle;*/
	}
	
	inline double GetRealAlpha(int ihit){
		//return GetAlphaMeas(ihit) - GetLorAlpha(ihit);
		double tan_angle = tan((*PixTrkAngle)[ihit]);
		double tan_angle_corr = tan_angle - (*PixTLorPhi)[ihit];
		return 180. * atan(tan_angle_corr) / M_PI;
	}
	
	inline double GetAlphaMeas(int ihit){
		return 180. * (*PixTrkAngle)[ihit] / M_PI;
	}
	
	inline double GetTilt(){
		return 180.*20.0 / M_PI;
	}
	
	inline int GetDetType(int ihit){
		if( abs( (*PixDetType)[ihit]) < 100) return (*PixDetType)[ihit];
		else return  abs( (*PixDetType)[ihit] ) - 96;
	}
	
	inline double GetTrkEta(int ihit){ return (*PixTrkEta)[ihit]; }
	inline double GetMicronDigResEta(int ihit){ return (*PixDigResEta)[ihit]*1000; }
	inline double GetMicronDigResPhi(int ihit){ return (*PixDigResPhi)[ihit]*1000; }
	inline double GetMicronResEta(int ihit){ return (*PixResEta)[ihit]*1000; }
	inline double GetMicronResPhi(int ihit){ return (*PixResPhi)[ihit]*1000; }
	inline double GetMicronErrEta(int ihit){ return (*PixErrEta)[ihit]*1000; }
	inline double GetMicronErrPhi(int ihit){ return (*PixErrPhi)[ihit]*1000; }
	inline double GetDeltaRow(int ihit){ return (*PixDeltaRow)[ihit]; }
	inline double GetDeltaCol(int ihit){ return (*PixDeltaCol)[ihit]; }
	inline double GetOmegaEta(int ihit){ return (*PixOmegaEta)[ihit]; }
	inline double GetOmegaPhi(int ihit){ return (*PixOmegaPhi)[ihit]; }
	inline double GetBiasVolt(int ihit){ return (*PixBiasVolt)[ihit]; }
	inline int GetEtaModule(int ihit){ return (*PixEtaModule)[ihit]; }
	inline int GetPhiModule(int ihit){ return (*PixPhiModule)[ihit]; }
	inline Double_t GetPullPhi(int ihit){ return (*PixPullPhi)[ihit]; }
	inline Double_t GetPullEta(int ihit){ return (*PixPullEta)[ihit]; }
	inline float GetCharge(int ihit){ return (*PixCharge)[ihit]; }
};

#endif

#ifdef PixelNtupleTracks_cxx
PixelNtupleTracks::PixelNtupleTracks(std::string input,std::string collection){
	TChain *mychain = 0;
	if(input != "" ){
		mychain = new TChain(collection.c_str());
		if( input.find(".root") != std::string::npos ){
			if(mychain->AddFile(input.c_str()) == 0){
				std::cout << "Failing to add " << input <<
				" to the chain..." << std::endl;
				abort();
			}
		}else{
                        std::ifstream myfile(input.c_str());
			if (myfile.is_open()){
				for(;;){
					getline(myfile,input);
					if(myfile.eof()) break;
					mychain->AddFile(input.c_str());
				}
				myfile.close();
			}else{
				std::cout << "Failing to open " << input << "..." << std::endl;
				abort();	
			}
		}
	}

	Init(mychain);
}

PixelNtupleTracks::~PixelNtupleTracks(){
}


Int_t PixelNtupleTracks::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t PixelNtupleTracks::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void PixelNtupleTracks::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   //HoleDetectorType = 0;
   //pullLocX = 0;
   //pullLocY = 0;
   //residualLocX = 0;
   //residualLocY = 0;
   //DetectorType = 0;
   //outlierFlag = 0;
   PixPullPhi = 0;
   PixPullEta = 0;
   PixResPhi = 0;
   PixResEta = 0;
   PixDetType = 0;
   //PixHasGanged = 0;
   //PixIsFake = 0;
   //PixToT = 0;
   PixCharge = 0;
   PixTLorPhi = 0;
   //PixTLorEta = 0;
   PixBiasVolt = 0;
   //PixDepVolt = 0;
   PixDigResPhi = 0;
   PixDigResEta = 0;
   PixErrPhi = 0;
   PixErrEta = 0;
   //PixLocX = 0;
   //PixLocY = 0;
   PixEtaModule = 0;
   PixPhiModule = 0;
   //PixGloX = 0;
   //PixGloY = 0;
   //PixGloZ = 0;
   //PixEta = 0;
   //PixFirstRow = 0;
   //PixFirstCol = 0;
   PixDeltaRow = 0;
   PixDeltaCol = 0;
   //PixDeltaPhi = 0;
   //PixDeltaEta = 0;
   PixOmegaPhi = 0;
   PixOmegaEta = 0;
   PixTrkAngle = 0;
   PixTrkEta = 0;
   PixTrkPt = 0;
   //PixTrkQ = 0;
   //PixHoleGloX = 0;
   //PixHoleGloY = 0;
   //PixHoleGloZ = 0;
   //PixHoleLocX = 0;
   //PixHoleLocY = 0;
   //PixHoleEtaModule = 0;
   //PixHolePhiModule = 0;
   //PixHoleLayer = 0;
   //PixHoleECBarrel = 0;
   //PixHoleClNearstSize = 0;
   //PixHoleLocXClNearst = 0;
   //PixHoleLocYClNearst = 0;
   //PixHoleClRowClNearst = 0;
   //PixHoleClColClNearst = 0;
   //PixHoleGloZNearst = 0;
   //PixHoleDeltaRowNearst = 0;
   //PixHoleDeltaColNearst = 0;
   //SCTPull = 0;
   //SCTRes = 0;
   //SctDetType = 0;
   //SctTLorPhi = 0;
   //SctTLorEta = 0;
   //SctEta = 0;
   //SctDeltaPhi = 0;
   //SctTrkAngle = 0;
   //SctTrkQ = 0;
   //TRTPull = 0;
   //TRTRes = 0;
   //HitX = 0;
   //HitY = 0;
   //HitZ = 0;
   //HitRadius = 0;
   //HitPhi = 0;
   //HitLocal1 = 0;
   //HitLocal2 = 0;
   //HitSurfaceX = 0;
   //HitSurfaceY = 0;
   //HitSurfaceZ = 0;
   //HitSurfaceRadius = 0;
   //HitSurfacePhi = 0;
   //HitSurfaceTheta = 0;
   //TrackX = 0;
   //TrackY = 0;
   //TrackZ = 0;
   //TrackMomentumX = 0;
   //TrackMomentumY = 0;
   //TrackMomentumZ = 0;
   //TrackLocal1 = 0;
   //TrackLocal2 = 0;
   
   b_Reconstructed_theta = nullptr;
   b_Reconstructed_Q_over_p = nullptr;
   b_number_Pixel_measurements = nullptr;
   b_number_SCT_measurements = nullptr;
   b_number_TRT_measurements = nullptr;
   b_PixPullPhi = nullptr;
   b_PixPullEta = nullptr;
   b_PixResPhi = nullptr;
   b_PixResEta = nullptr;
   b_PixDetType = nullptr;
   b_PixCharge = nullptr;
   b_PixTLorPhi = nullptr;
   b_PixBiasVolt = nullptr;
   b_PixDigResPhi = nullptr;
   b_PixDigResEta = nullptr;
   b_PixErrPhi = nullptr;
   b_PixErrEta = nullptr;
   b_PixEtaModule = nullptr;
   b_PixPhiModule = nullptr;
   b_PixDeltaRow = nullptr;
   b_PixDeltaCol = nullptr;
   b_PixOmegaPhi = nullptr;
   b_PixOmegaEta = nullptr;
   b_PixTrkAngle = nullptr;
   b_PixTrkEta = nullptr;
   b_PixTrkPt = nullptr;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

//   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_run_number);
//   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_event_number);
//   fChain->SetBranchAddress("TrackID", &TrackID, &b_ID_of_track_in_event);
//   fChain->SetBranchAddress("IterationIndex", &IterationIndex, &b_Iteration_of_a_track);
//   fChain->SetBranchAddress("nHits", &nHits, &b_Number_of_measurements_on_track);
//   fChain->SetBranchAddress("TrackStatesUnbiased", &TrackStatesUnbiased, &b_are_track_states_unbiased);
//   fChain->SetBranchAddress("RecD0", &RecD0, &b_Reconstructed_d0);
//   fChain->SetBranchAddress("RecZ0", &RecZ0, &b_Reconstructed_z0);
//   fChain->SetBranchAddress("RecPhi0", &RecPhi0, &b_Reconstructed_phi0);
   fChain->SetBranchAddress("RecTheta", &RecTheta, &b_Reconstructed_theta);
//   fChain->SetBranchAddress("RecEta", &RecEta, &b_Reconstructed_eta);
   fChain->SetBranchAddress("RecQoverP", &RecQoverP, &b_Reconstructed_Q_over_p);
//   fChain->SetBranchAddress("RecErrD0", &RecErrD0, &b_err_d0);
//   fChain->SetBranchAddress("RecErrZ0", &RecErrZ0, &b_err_z0);
//   fChain->SetBranchAddress("RecErrPhi0", &RecErrPhi0, &b_err_phi0);
//   fChain->SetBranchAddress("RecErrTheta", &RecErrTheta, &b_err_theta);
//   fChain->SetBranchAddress("RecErrQoverP", &RecErrQoverP, &b_err_Q_over_p);
//   fChain->SetBranchAddress("Chi2overNdof", &Chi2overNdof, &b_Reco_Chi2_over_Ndof);
//   fChain->SetBranchAddress("Ndof", &Ndof, &b_Reco_Ndof);
//   fChain->SetBranchAddress("nHoles", &nHoles, &b_Number_of_holes_on_track);
//   fChain->SetBranchAddress("HoleDetectorType", &HoleDetectorType, &b_HoleDetectorType);
//   fChain->SetBranchAddress("pullLocX", &pullLocX, &b_pullLocX);
//   fChain->SetBranchAddress("pullLocY", &pullLocY, &b_pullLocY);
//   fChain->SetBranchAddress("residualLocX", &residualLocX, &b_residualLocX);
//   fChain->SetBranchAddress("residualLocY", &residualLocY, &b_residualLocY);
//   fChain->SetBranchAddress("DetectorType", &DetectorType, &b_DetectorType);
//   fChain->SetBranchAddress("outlierFlag", &outlierFlag, &b_outlierFlag);
//   fChain->SetBranchAddress("FitterStatusCode", &FitterStatusCode, &b_FitterStatusCode);
   fChain->SetBranchAddress("nPixelHits", &nPixelHits, &b_number_Pixel_measurements);
   fChain->SetBranchAddress("nSCTHits", &nSCTHits, &b_number_SCT_measurements);
   fChain->SetBranchAddress("nTRTHits", &nTRTHits, &b_number_TRT_measurements);
//   fChain->SetBranchAddress("nMDTHits", &nMDTHits, &b_number_MDT_measurements);
//   fChain->SetBranchAddress("nCSCHits", &nCSCHits, &b_number_CSC_measurements);
//   fChain->SetBranchAddress("nRPCHits", &nRPCHits, &b_number_RPC_measurements);
//   fChain->SetBranchAddress("nTGCHits", &nTGCHits, &b_number_TGC_measurements);
   fChain->SetBranchAddress("PixPullPhi", &PixPullPhi, &b_PixPullPhi);
   fChain->SetBranchAddress("PixPullEta", &PixPullEta, &b_PixPullEta);
   fChain->SetBranchAddress("PixResPhi", &PixResPhi, &b_PixResPhi);
   fChain->SetBranchAddress("PixResEta", &PixResEta, &b_PixResEta);
   fChain->SetBranchAddress("PixDetType", &PixDetType, &b_PixDetType);
//   fChain->SetBranchAddress("PixHasGanged", &PixHasGanged, &b_PixHasGanged);
//   fChain->SetBranchAddress("PixIsFake", &PixIsFake, &b_PixIsFake);
//   fChain->SetBranchAddress("PixToT", &PixToT, &b_PixToT);
		fChain->SetBranchAddress("PixCharge", &PixCharge, &b_PixCharge);
   fChain->SetBranchAddress("PixTLorPhi", &PixTLorPhi, &b_PixTLorPhi);
//   fChain->SetBranchAddress("PixTLorEta", &PixTLorEta, &b_PixTLorEta);
   fChain->SetBranchAddress("PixBiasVolt", &PixBiasVolt, &b_PixBiasVolt);
//   fChain->SetBranchAddress("PixDepVolt", &PixDepVolt, &b_PixDepVolt);
   fChain->SetBranchAddress("PixDigResPhi", &PixDigResPhi, &b_PixDigResPhi);
   fChain->SetBranchAddress("PixDigResEta", &PixDigResEta, &b_PixDigResEta);
   fChain->SetBranchAddress("PixErrPhi", &PixErrPhi, &b_PixErrPhi);
   fChain->SetBranchAddress("PixErrEta", &PixErrEta, &b_PixErrEta);
//   fChain->SetBranchAddress("PixLocX", &PixLocX, &b_PixLocX);
//   fChain->SetBranchAddress("PixLocY", &PixLocY, &b_PixLocY);
   fChain->SetBranchAddress("PixEtaModule", &PixEtaModule, &b_PixEtaModule);
   fChain->SetBranchAddress("PixPhiModule", &PixPhiModule, &b_PixPhiModule);
//   fChain->SetBranchAddress("PixGloX", &PixGloX, &b_PixGloX);
//   fChain->SetBranchAddress("PixGloY", &PixGloY, &b_PixGloY);
//   fChain->SetBranchAddress("PixGloZ", &PixGloZ, &b_PixGloZ);
//   fChain->SetBranchAddress("PixEta", &PixEta, &b_PixEta);
//   fChain->SetBranchAddress("PixFirstRow", &PixFirstRow, &b_PixFirstRow);
//   fChain->SetBranchAddress("PixFirstCol", &PixFirstCol, &b_PixFirstCol);
   fChain->SetBranchAddress("PixDeltaRow", &PixDeltaRow, &b_PixDeltaRow);
   fChain->SetBranchAddress("PixDeltaCol", &PixDeltaCol, &b_PixDeltaCol);
//   fChain->SetBranchAddress("PixDeltaPhi", &PixDeltaPhi, &b_PixDeltaPhi);
//   fChain->SetBranchAddress("PixDeltaEta", &PixDeltaEta, &b_PixDeltaEta);
   fChain->SetBranchAddress("PixOmegaPhi", &PixOmegaPhi, &b_PixOmegaPhi);
   fChain->SetBranchAddress("PixOmegaEta", &PixOmegaEta, &b_PixOmegaEta);
   fChain->SetBranchAddress("PixTrkAngle", &PixTrkAngle, &b_PixTrkAngle);
   fChain->SetBranchAddress("PixTrkEta", &PixTrkEta, &b_PixTrkEta);
   fChain->SetBranchAddress("PixTrkPt", &PixTrkPt, &b_PixTrkPt);
//   fChain->SetBranchAddress("PixTrkQ", &PixTrkQ, &b_PixTrkQ);
//   fChain->SetBranchAddress("PixHoleGloX", &PixHoleGloX, &b_PixHoleGloX);
//   fChain->SetBranchAddress("PixHoleGloY", &PixHoleGloY, &b_PixHoleGloY);
//   fChain->SetBranchAddress("PixHoleGloZ", &PixHoleGloZ, &b_PixHoleGloZ);
//   fChain->SetBranchAddress("PixHoleLocX", &PixHoleLocX, &b_PixHoleLocX);
//   fChain->SetBranchAddress("PixHoleLocY", &PixHoleLocY, &b_PixHoleLocY);
//   fChain->SetBranchAddress("PixHoleEtaModule", &PixHoleEtaModule, &b_PixHoleEtaModule);
//   fChain->SetBranchAddress("PixHolePhiModule", &PixHolePhiModule, &b_PixHolePhiModule);
//   fChain->SetBranchAddress("PixHoleLayer", &PixHoleLayer, &b_PixHoleLayer);
//   fChain->SetBranchAddress("PixHoleECBarrel", &PixHoleECBarrel, &b_PixHoleECBarrel);
//   fChain->SetBranchAddress("PixHoleClNearstSize", &PixHoleClNearstSize, &b_PixHoleClNearstSize);
//   fChain->SetBranchAddress("PixHoleLocXClNearst", &PixHoleLocXClNearst, &b_PixHoleLocXClNearst);
//   fChain->SetBranchAddress("PixHoleLocYClNearst", &PixHoleLocYClNearst, &b_PixHoleLocYClNearst);
//   fChain->SetBranchAddress("PixHoleClRowClNearst", &PixHoleClRowClNearst, &b_PixHoleClRowClNearst);
//   fChain->SetBranchAddress("PixHoleClColClNearst", &PixHoleClColClNearst, &b_PixHoleClColClNearst);
//   fChain->SetBranchAddress("PixHoleGloZNearst", &PixHoleGloZNearst, &b_PixHoleGloZNearst);
//   fChain->SetBranchAddress("PixHoleDeltaRowNearst", &PixHoleDeltaRowNearst, &b_PixHoleDeltaRowNearst);
//   fChain->SetBranchAddress("PixHoleDeltaColNearst", &PixHoleDeltaColNearst, &b_PixHoleDeltaColNearst);
//   fChain->SetBranchAddress("SCTPull", &SCTPull, &b_SCTPull);
//   fChain->SetBranchAddress("SCTRes", &SCTRes, &b_SCTRes);
//   fChain->SetBranchAddress("SctDetType", &SctDetType, &b_SctDetType);
//   fChain->SetBranchAddress("SctTLorPhi", &SctTLorPhi, &b_SctTLorPhi);
//   fChain->SetBranchAddress("SctTLorEta", &SctTLorEta, &b_SctTLorEta);
//   fChain->SetBranchAddress("SctEta", &SctEta, &b_SctEta);
//   fChain->SetBranchAddress("SctDeltaPhi", &SctDeltaPhi, &b_SctDeltaPhi);
//   fChain->SetBranchAddress("SctTrkAngle", &SctTrkAngle, &b_SctTrkAngle);
//   fChain->SetBranchAddress("SctTrkQ", &SctTrkQ, &b_SctTrkQ);
//   fChain->SetBranchAddress("TRTPull", &TRTPull, &b_TRTPull);
//   fChain->SetBranchAddress("TRTRes", &TRTRes, &b_TRTRes);
//   fChain->SetBranchAddress("HitX", &HitX, &b_HitX);
//   fChain->SetBranchAddress("HitY", &HitY, &b_HitY);
//   fChain->SetBranchAddress("HitZ", &HitZ, &b_HitZ);
//   fChain->SetBranchAddress("HitRadius", &HitRadius, &b_HitRadius);
//   fChain->SetBranchAddress("HitPhi", &HitPhi, &b_HitPhi);
//   fChain->SetBranchAddress("HitLocal1", &HitLocal1, &b_HitLocal1);
//   fChain->SetBranchAddress("HitLocal2", &HitLocal2, &b_HitLocal2);
//   fChain->SetBranchAddress("HitSurfaceX", &HitSurfaceX, &b_HitSurfaceX);
//   fChain->SetBranchAddress("HitSurfaceY", &HitSurfaceY, &b_HitSurfaceY);
//   fChain->SetBranchAddress("HitSurfaceZ", &HitSurfaceZ, &b_HitSurfaceZ);
//   fChain->SetBranchAddress("HitSurfaceRadius", &HitSurfaceRadius, &b_HitSurfaceRadius);
//   fChain->SetBranchAddress("HitSurfacePhi", &HitSurfacePhi, &b_HitSurfacePhi);
//   fChain->SetBranchAddress("HitSurfaceTheta", &HitSurfaceTheta, &b_HitSurfaceTheta);
//   fChain->SetBranchAddress("TrackX", &TrackX, &b_TrackX);
//   fChain->SetBranchAddress("TrackY", &TrackY, &b_TrackY);
//   fChain->SetBranchAddress("TrackZ", &TrackZ, &b_TrackZ);
//   fChain->SetBranchAddress("TrackMomentumX", &TrackMomentumX, &b_TrackMomentumX);
//   fChain->SetBranchAddress("TrackMomentumY", &TrackMomentumY, &b_TrackMomentumY);
//   fChain->SetBranchAddress("TrackMomentumZ", &TrackMomentumZ, &b_TrackMomentumZ);
//   fChain->SetBranchAddress("TrackLocal1", &TrackLocal1, &b_TrackLocal1);
//   fChain->SetBranchAddress("TrackLocal2", &TrackLocal2, &b_TrackLocal2);
   Notify();
}

Bool_t PixelNtupleTracks::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void PixelNtupleTracks::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

void PixelNtupleTracks::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L PixelNtupleTracks.C
//      Root > PixelNtupleTracks t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}


#endif // #ifdef PixelNtupleTracks_cxx

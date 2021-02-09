/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//mmcTest
//root [0] .L mmcTest.C++
//root [1] mmcTest()
//setvalgrind='export PATH=/afs/cern.ch/sw/lcg/external/valgrind/3.5.0/x86_64-slc5-gcc43-opt/bin:$PATH ;export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/valgrind/3.5.0/x86_64-slc5-gcc43-opt/lib:$LD_LIBRARY_PATH; export PATH=${PATH}:/afs/cern.ch/sw/lcg/external/graphviz/1.9/slc3_ia32_gcc344/bin/'
//#valgrind --tool=callgrind --trace-children=yes ./mmcTest
//#kcachegrind callgrind.out.*
//if save histogram. 
//#define HISTITERATION
//if only check lep-had with high statistics for cpu optimisation
//#define HIGHSTAT
//to debug a particular event (one has to put by hand the input parameters)
//#define DEBUGEVENT
// if tau energy scanning
//DR #define TAUESCAN
// to dump the new mass values to be able to cut and paste them in the code
#include "DiTauMassTools/MissingMassCalculator.h"
//#include "JERProvider.h" remove
#include "TFile.h"
#include "TTree.h"
#include <TStopwatch.h>
//FIXME set one jet

void ut_mmcTest() 
{
  int algversionmin=3;
  int algversionmax=3;
  const int nCS=MMCCalibrationSet::MAXMMCCALIBRATIONSET;
  const int nCase=4;
  const int nJet=2;
  const int nAlg=3;

const std::string caseName[nCase]={ "ll", "hl", "lh", "hh"};

#ifdef HIGHSTAT
  int icalibmin=1;
  int icalibmax=1;
  int icasemin=2;
  int icasemax=2;
  int nrepeat=1;
  int injetmin=1;
  int injetmax=1;
  int niter=-1;
#else
  int icalibmin=0;
  int icalibmax=nCS-1;
  int icasemin=0;
  int icasemax=nCase-1;
  int injetmin=0;
  int injetmax=nJet-1;
  int nrepeat=1;
  int niter=-1;
  
  // int icalibmin=2;
  // int icalibmax=2;
  // int icasemin=3;
  // int icasemax=3;
  // int injetmin=1;
  // int injetmax=1;
  // int nrepeat=1;
#endif
#ifdef DEBUGEVENT
  icasemin=3;
  icasemax=3;
  algversionmin=3;
  algversionmax=3;
#endif


  //#ifdef HISTITERATION
  TFile *afile   = new TFile("mmcTest_output.root","RECREATE");
  TTree *treeOut = new TTree("MMC", "MMC scanning studies");
  gDirectory->pwd();
  //  gDirectory->cd("mmcTest_output");
  // gDirectory->pwd();  
  //  afile->cd();
  MissingMassCalculator MMC;
  
  //MissingMassCalculator::OutputInfoStuff ois;


  //std::cout << "Print before running calculator" << std::endl;
  //MMC.PrintResults(ois);
  //reference values : CalibrationSet, case ll lh etc, njet25, algversion -1
  //  double fitNewSolV[2][4][2][3];
  //double fitSolV[2][4][2][3]={

  double fitNewSolV[nCS][nCase][nJet][nAlg]={{{{0. }}}} ;
  double fitSolV[nCS][nCase][nJet][nAlg]={
#ifndef TAUESCAN
    // without tau scanning 
                    {{{0 , 0 , 113.442}, // CS MMC2011 case ll njet 0
                    {0 , 0 , 113.442}}, // CS MMC2011 case ll njet 1
                    {{0 , 0 , 104.943}, // CS MMC2011 case hl njet 0
                    {0 , 0 , 100.58}}, // CS MMC2011 case hl njet 1
                    {{0 , 0 , 104.943}, // CS MMC2011 case lh njet 0
                    {0 , 0 , 100.58}}, // CS MMC2011 case lh njet 1
                    {{0 , 0 , 86.7538}, // CS MMC2011 case hh njet 0
                    {0 , 0 , 90.5832}}}, // CS MMC2011 case hh njet 1
                    {{{0 , 0 , 117.643}, // CS MMC2012 case ll njet 0
                    {0 , 0 , 116.771}}, // CS MMC2012 case ll njet 1
                    {{0 , 0 , 100.596}, // CS MMC2012 case hl njet 0
                    {0 , 0 , 105.698}}, // CS MMC2012 case hl njet 1
                    {{0 , 0 , 100.596}, // CS MMC2012 case lh njet 0
                    {0 , 0 , 105.698}}, // CS MMC2012 case lh njet 1
                    {{0 , 0 , 85.6864}, // CS MMC2012 case hh njet 0
                    {0 , 0 , 88.9267}}}, // CS MMC2012 case hh njet 1
                    {{{0 , 0 , 120.936}, // CS MMC2015 case ll njet 0
                    {0 , 0 , 114.274}}, // CS MMC2015 case ll njet 1
                    {{0 , 0 , 100.596}, // CS MMC2015 case hl njet 0
                    {0 , 0 , 105.698}}, // CS MMC2015 case hl njet 1
                    {{0 , 0 , 100.596}, // CS MMC2015 case lh njet 0
                    {0 , 0 , 105.698}}, // CS MMC2015 case lh njet 1
                    {{0 , 0 , 85.6864}, // CS MMC2015 case hh njet 0
                    {0 , 0 , 88.9267}}}, // CS MMC2015 case hh njet 1
                    {{{0 , 0 , 120.936}, // CS MMC2015HIGHMASS case ll njet 0
                    {0 , 0 , 114.274}}, // CS MMC2015HIGHMASS case ll njet 1
                    {{0 , 0 , 100.818}, // CS MMC2015HIGHMASS case hl njet 0
                    {0 , 0 , 105.698}}, // CS MMC2015HIGHMASS case hl njet 1
                    {{0 , 0 , 100.818}, // CS MMC2015HIGHMASS case lh njet 0
                    {0 , 0 , 105.698}}, // CS MMC2015HIGHMASS case lh njet 1
                    {{0 , 0 , 88.9267}, // CS MMC2015HIGHMASS case hh njet 0
                    {0 , 0 , 88.9267}}}, // CS MMC2015HIGHMASS case hh njet 1
                    {{{0 , 0 , 111.713}, // CS UPGRADE case ll njet 0
                    {0 , 0 , 111.713}}, // CS UPGRADE case ll njet 1
                    {{0 , 0 , 98.9544}, // CS UPGRADE case hl njet 0
                    {0 , 0 , 98.9544}}, // CS UPGRADE case hl njet 1
                    {{0 , 0 , 98.9544}, // CS UPGRADE case lh njet 0
                    {0 , 0 , 98.9544}}, // CS UPGRADE case lh njet 1
                    {{0 , 0 , 97.1077}, // CS UPGRADE case hh njet 0
                    {0 , 0 , 97.1077}}}, // CS UPGRADE case hh njet 1
                    {{{0 , 0 , 158.953}, // CS LFVMMC2012 case ll njet 0
                    {0 , 0 , 159.219}}, // CS LFVMMC2012 case ll njet 1
                    {{0 , 0 , 102.183}, // CS LFVMMC2012 case hl njet 0
                    {0 , 0 , 103.508}}, // CS LFVMMC2012 case hl njet 1
                    {{0 , 0 , 102.183}, // CS LFVMMC2012 case lh njet 0
                    {0 , 0 , 103.508}}, // CS LFVMMC2012 case lh njet 1
                    {{0 , 0 , 89.0465}, // CS LFVMMC2012 case hh njet 0
                    {0 , 0 , 89.418}}} // CS LFVMMC2012 case hh njet 1


#else

    // with tau scanning OBSOLETE
                    {{{0 , 0 , 113.442}, // CS 0 case 0 njet 0
                    {0 , 0 , 113.442}}, // CS 0 case 0 njet 1
                    {{0 , 0 , 101.543}, // CS 0 case 1 njet 0
                    {0 , 0 , 103.003}}, // CS 0 case 1 njet 1
                    {{0 , 0 , 101.543}, // CS 0 case 2 njet 0
                    {0 , 0 , 103.003}}, // CS 0 case 2 njet 1
                    {{0 , 0 , 85.2869}, // CS 0 case 3 njet 0
                    {0 , 0 , 88.9654}}}, // CS 0 case 3 njet 1
                    {{{0 , 0 , 113.442}, // CS 1 case 0 njet 0
                    {0 , 0 , 113.442}}, // CS 1 case 0 njet 1
                    {{0 , 0 , 102.097}, // CS 1 case 1 njet 0
                    {0 , 0 , 101.387}}, // CS 1 case 1 njet 1
                    {{0 , 0 , 102.097}, // CS 1 case 2 njet 0
                    {0 , 0 , 101.387}}, // CS 1 case 2 njet 1
                    {{0 , 0 , 86.3365}, // CS 1 case 3 njet 0
                    {0 , 0 , 90.1199}}} // CS 1 case 3 njet 1
 
#endif  
  }  ;



  //  double fitSolV1[4]={101.575,100.888,100.888,94.0078};   
  // double fitSolV3[4]={113.442,100.58,100.58,91.3291};

  std::vector<TLorentzVector> jetvec;
  TLorentzVector jetP4;

  jetP4.SetPtEtaPhiM(30.,1.,0.1,1.);
  jetvec.push_back(jetP4);
  double mmu=0.105658;
  double mtauvis=0.8; // 0.8 GeV : 1 prong 1.2 : 3 prong
  int icase;
  int algversion;
  treeOut->Branch("icase",&icase);      
  treeOut->Branch("ialg",&algversion);      
 
  std::vector<double> pprob;
  std::vector<double> pmtautau;
  std::vector<double> pPhi1;
  std::vector<double> pPhi2;
  std::vector<double> pMEtP;
  std::vector<double> pMEtL;
  std::vector<double> pMEtX;
  std::vector<double> pMEtY;
  std::vector<double> pMnu1;
  std::vector<double> pMnu2;
  std::vector<double> peTau1;
  std::vector<double> peTau2;
  std::vector<int> ppCode;
  std::vector<int> pindex;
  treeOut->Branch("pprob",&pprob);      
  treeOut->Branch("pmtautau",&pmtautau);      
  treeOut->Branch("pphi1",&pPhi1);      
  treeOut->Branch("pphi2",&pPhi2);      
  treeOut->Branch("pmetp",&pMEtP);      
  treeOut->Branch("pmetl",&pMEtL);      
  treeOut->Branch("pmetx",&pMEtX);      
  treeOut->Branch("pmety",&pMEtY);      
  treeOut->Branch("pmnu1",&pMnu1);      
  treeOut->Branch("pmnu2",&pMnu2);      
  treeOut->Branch("petau1",&peTau1);      
  treeOut->Branch("petau2",&peTau2);      
  treeOut->Branch("ppcode",&ppCode);      
  treeOut->Branch("pindex",&pindex);      

  bool newMasses=false;

  for (int icalib=icalibmin;icalib<=icalibmax;++icalib){

     
    for (icase=icasemin;icase<=icasemax;++icase){
      // if (icase==1) continue ; // skip had-lep case
	
    
      for (int injet=injetmin;injet<=injetmax;++injet){


    
	// loop on different version. Only one remaining nowadays
	// (forget algversion 2 v10fast for time being)

	for (algversion=algversionmin;algversion<=algversionmax;algversion+=1){
	  // repetition
	  double cpuSum=0;
	  double cpuSum2=0;
	  for (int irepeat=0;irepeat<nrepeat;++irepeat) {
	    MMCCalibrationSet::e theCS=static_cast<MMCCalibrationSet::e>(icalib);
	    MMC.SetCalibrationSet(theCS);
	    // repeat with the same seed (to maximise cpu stability)
	    MMC.SetRndmSeedAltering(0); // random (default is 1e6)
	    if (algversion==10 || algversion==2) continue; // skip v10fast and v9
       

	    //MMC.SetMeanbinStop(0.02);
      
	    //    MissingMassCalculator MMC;

	    if (niter>0) MMC.SetNiterRandom(niter);



	    MMC.SetSpeedStudy(true);
	    // AlgVersion 2 -> 10
	    MMC.SetAlgorithmVersion(algversion);

	    //uncomment for complete dump
	    // MMC.SetUseVerbose(1); 

#ifdef DEBUGEVENT
	    int njet25=4;
	    MMC.SetUseVerbose(1);
#else
	    int njet25=injet;
#endif




	    MMC.SetNjet25(njet25);

#ifdef HIGHSTAT
	    MMC.SetUseVerbose(1);
#endif
    

	    TLorentzVector tau0;
	    TLorentzVector tau1;
	    // Mev now (no, buggy)
	    //tau0.SetPtEtaPhiM(23421.9,TMath::ATanH(cos(2.3094)),2.58926,104.189);
	    //tau1.SetPtEtaPhiM(22355.4,TMath::ATanH(cos(2.06735)),0.613857,552.88);




	    // MMC.SetNiterFit1(5);
	    // MMC.SetNiterFit2(5);
	    // MMC.SetNiterFit3(3);

	    // MMC.SetNiterFit1(10);
	    // MMC.SetNiterFit2(15);
	    // MMC.SetNiterFit3(5);

  
	    double tau0pt=40.3735;
	    double tau0eta=-1.40963;
	    double tau0phi=2.8783;
	    double tau1pt=32.2002;
	    double tau1eta=-1.35506;
	    double tau1phi=-0.26076;
    
#ifdef DEBUGEVENT
	    TVector2 MetVec(-125.515,-64.1046);
	    MMC.SetSumEt(847.81);
#else
	    TVector2 MetVec(4.28514,-6.21378);
	    MMC.SetSumEt(340.616);
#endif
	    //TVector2 MetVec(-6750.43,49263.8);
	    MMC.SetMetVec(MetVec); 
	    if (icalib==MMCCalibrationSet::UPGRADE) 
	      {
		MMC.SetMetScanParams(0.0,14.,14.); // METresolution should be obtained from getMETandRes() in METSmearing package	    
	      }
	    
	    // MMC.SetMetCovariance(std::pow(10.,2),std::pow(30.,2),150.);  std::cout << " custom MET covariance matrix " << std::endl;



#ifndef DEBUGEVENT	
	    MMC.SetMetScanParamsJets(jetvec);  
#endif    

	    switch (icase)
	      {
	
	      case 0:
		//std::cout << "********************* ll case ************************" << std::endl;
		MMC.SetVisTauType(0,0);
		MMC.SetVisTauType(1,0);
		tau0.SetPtEtaPhiM(tau0pt,tau0eta,tau0phi,mmu);
		tau1.SetPtEtaPhiM(tau1pt,tau1eta,tau1phi,mmu);
		break;
      
	      case 1:
		//std::cout << "********************* hl case ************************" << std::endl;
		MMC.SetVisTauType(0,10);
		MMC.SetVisTauType(1,0);
		tau0.SetPtEtaPhiM(tau0pt,tau0eta,tau0phi,mtauvis);
		tau1.SetPtEtaPhiM(tau1pt,tau1eta,tau1phi,mmu);
		break;
      
	      case 2:
		//std::cout << "********************* lh case ************************" << std::endl;
		//
		// flip tau0 and tau1, so that results should be identical to hl case
		// a non trivial check
		MMC.SetVisTauType(0,0);
		MMC.SetVisTauType(1,10);
		tau0.SetPtEtaPhiM(tau1pt,tau1eta,tau1phi,mmu);
		tau1.SetPtEtaPhiM(tau0pt,tau0eta,tau0phi,mtauvis);

		break;
      

	      case 3:
		//std::cout << "********************* hh case ************************" << std::endl;
#ifdef DEBUGEVENT
		MMC.SetVisTauType(0,30);
		MMC.SetVisTauType(1,10);
		tau0.SetPxPyPzE(-60.1543,-55.413,-39.9587,sqrt(std::pow(-60.1543,2)+std::pow(-55.413,2)+std::pow(-39.9587,2)+std::pow(1.2,2)));
		tau1.SetPxPyPzE(-75.536,6.15277,-73.7405,sqrt(std::pow(-75.536,2)+std::pow(6.15277,2)+std::pow(-73.7405,2)+std::pow(0.8,2)));

#else
		MMC.SetVisTauType(0,10);
		MMC.SetVisTauType(1,10);
		tau0.SetPtEtaPhiM(tau0pt,tau0eta,tau0phi,mtauvis);
		tau1.SetPtEtaPhiM(tau1pt,tau1eta,tau1phi,mtauvis);
#endif
		break;

	      default:
		std::cout << "ERROR unforeseen icase " << std::endl;
		break;
	
	      }

	    pprob.clear();
	    pmtautau.clear();
	    pPhi1.clear();
	    pPhi2.clear();
	    pMEtP.clear();
	    pMEtL.clear();
	    pMEtX.clear();
	    pMEtY.clear();
	    pMnu1.clear();
	    pMnu2.clear();
	    peTau1.clear();
	    peTau2.clear();
	    ppCode.clear();
	    pindex.clear();

	    MMC.SetVisTauVec(0,tau0);
	    MMC.SetVisTauVec(1,tau1);

	    afile->cd();
	    TStopwatch timer;
	    timer.Start();
	

	    
	    // print configuration in one line
	    std::cout << std::endl;
	    std::cout << "***************** CalibrationSet ";
	    std::cout << MMCCalibrationSet::name[icalib] ;

	    std::cout << " " << caseName[icase] << " " ;
	    

	    std::cout << " njet25=" << injet ;
	    std::cout << " algversion=" << algversion << " **********************" << std::endl;



	    // MMC.SetAllowUseHT(false); std::cout << "WARNING disallow useHT ! " << std::endl;



	    MMC.RunMissingMassCalculator();  
	    timer.Stop();
	    // ignore the first iteration
	    if (irepeat!=0){
	      double cpuTime=timer.CpuTime();
	      cpuSum+=cpuTime;
	      cpuSum2+=cpuTime*cpuTime;
	    }    
    
	    const MissingMassCalculator::AllPointsOutputInfo &  aPOI =MMC.GetAllPointsOutputInfo();
	    const int nAllPoints=aPOI.prob.size();
	    for (int i=0 ; i<nAllPoints ; ++i){
	      pprob.push_back(aPOI.prob.at(i)) ;
	      pmtautau.push_back(aPOI.mtautau.at(i)) ;
	      pPhi1.push_back(aPOI.Phi1.at(i)) ;
	      pPhi2.push_back(aPOI.Phi2.at(i)) ;
	      pMEtP.push_back(aPOI.MEtP.at(i)) ;
	      pMEtL.push_back(aPOI.MEtL.at(i)) ;
	      pMEtX.push_back(aPOI.MEtX.at(i)) ;
	      pMEtY.push_back(aPOI.MEtY.at(i)) ;
	      pMnu1.push_back(aPOI.Mnu1.at(i)) ;
	      pMnu2.push_back(aPOI.Mnu2.at(i)) ;
	      peTau1.push_back(aPOI.eTau1.at(i)) ;
	      peTau2.push_back(aPOI.eTau2.at(i)) ;
	      ppCode.push_back(aPOI.pCode.at(i)) ;
	      pindex.push_back(aPOI.index.at(i)) ;
	    }    

	    treeOut->Fill();

	    double mExpected;
	    mExpected=fitSolV[icalib][icase][std::min(1,injet)][algversion-1];

	    if (MMC.GetFitStatus()!=1) {
		  std::cout << "WARNING fit failed unlike reference for CS " << icalib << 
		    " case " << icase << 
		    " njet " << injet << 
		    " alg " << algversion << 
		    " expected:" << mExpected << " got:" << MMC.GetFittedMass(MMCFitMethod::MLM) << std::endl ;

	      newMasses=true;

	    } else
	      {
		const double delta=MMC.GetFittedMass(MMCFitMethod::MLM)-mExpected;
		fitNewSolV[icalib][icase][std::min(1,injet)][algversion-1]=MMC.GetFittedMass(MMCFitMethod::MLM);	
		if (std::abs(delta)>1e-3){
		  std::cout << "WARNING results differ from reference for CS " << icalib << 
		    " case " << icase << 
		    " njet " << injet << 
		    " alg " << algversion << 
		    " expected:" << mExpected << " got:" << MMC.GetFittedMass(MMCFitMethod::MLM) << " diff:" << delta << std::endl ;
		  newMasses=true;
		} else
		  {
		  std::cout << "results agree from reference for CS " << icalib << 
		    " case " << icase << 
		    " njet " << injet << 
		    " alg " << algversion << 
		    " expected:" << mExpected << " got:" << MMC.GetFittedMass(MMCFitMethod::MLM) << " diff:" << delta << std::endl ;
		  }
		double integralError;
		const double integral= MMC.GetMassHistogram()->IntegralAndError(0,1000000,integralError);//exceedingly large bin to get full integral
		std::cout << "Fitted mass from MAXW " << MMC.GetFittedMass(MMCFitMethod::MAXW) << " from MLM " << MMC.GetFittedMass(MMCFitMethod::MLM) << " from MLNU3P " << MMC.GetFittedMass(MMCFitMethod::MLNU3P)  
			  << " total W:" << integral << " and Error:" << integralError << std::endl;

	      }
    
	    //copy histogram to output
	    afile->cd();
	    // used to label histogram
	    int reiteration=100*algversion+10*irepeat+icase;
	    TString cloneName("clone_re");
	    TString cloneNoWName("clone_noweight_re");
	    cloneName+=reiteration;
	    cloneNoWName+=reiteration;


	    TH1F* theClone = (TH1F*) MMC.GetMassHistogram().get()->Clone(cloneName);
	    
        // redo the fit just to have the plot
	    std::vector<double> histInfo(HistInfo::MAXHISTINFO);
	    MMC.maxFromHist(theClone,histInfo,MaxHistStrategy::FIT,2,true);
	    theClone->SetAxisRange(50.,200.);
	    theClone=0;
	    TH1F* theCloneNoW = (TH1F*) MMC.GetMassHistogramNoWeight().get()->Clone(cloneNoWName);
	    theCloneNoW->SetAxisRange(50.,200.);
	    theCloneNoW=0;


	  } // repeat
	  if (nrepeat>1){
	    const double cpuAv= cpuSum/(nrepeat-1) ;
	    const double cpuRMS=sqrt(cpuSum2/(nrepeat-1)-std::pow(cpuSum/(nrepeat-1),2));
	    std::cout << "average CPU (first excluded) " << cpuAv << "+/-" << cpuRMS/sqrt(nrepeat-1) << std::endl;
	  }
	}// alg
      }// injet
    }// case
  }// icalib
  

  
  afile->Write();
  //  afile->Close();
  std::cout << "before new masses " << std::endl;
  
  if (newMasses){
    std::cout << " some mass have changed, these are the updated reference values " << std::endl;
    for (int iCS=0 ; iCS<nCS ; ++iCS){
      for (int iCase=0 ; iCase<nCase ; ++iCase){
	for (int iJet=0 ; iJet<nJet ; ++iJet){
	  std::cout << "                    {" ;
	  if (iCase==0 && iJet==0) std::cout << "{";
	  if (iJet==0) std::cout << "{";
	  for (int iAlg=0 ; iAlg<nAlg ; ++iAlg){
	    std::cout << fitNewSolV[iCS][iCase][iJet][iAlg]  ;
	    if (iAlg==nAlg-1) {
	      std::cout << "}";
	      if (iJet==nJet-1) std::cout << "}";
	      if (iCase==nCase-1 && iJet==nJet-1) std::cout << "}";
	    } else  {
	      std::cout << " , ";	      
	    }
	  }  
	  if ( !(iCS==nCS-1 && iCase ==nCase-1 && iJet == nJet-1)) std::cout << "," ;
	  std::cout << " // CS " << MMCCalibrationSet::name[iCS]  << " case " << caseName[iCase] << " njet " << iJet << std::endl; 
	}  
      }  

    }  
  }  else   {
    std::cout << " no change in output mass, perfect!" << std::endl;
  }
  std::cout << " end of tests" << std::endl;
  
  // MMC.PrintResults(ois);
}


int main()
{
  ut_mmcTest();
  return 0;
}


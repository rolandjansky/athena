WARNING 20150616 : TO GET NEUTRINO OR RESONANCE 4MOMENTUM FROM METHOD
FORMERLY KNOWN AS METHOD 2, ONE NOW SHOULD USE ENUM MMCFitMethod::MLNU3P 
(job will print many ERROR if not)


  
WARNING THIS README SHOULD BE UPDATED AFTER MIGRATION TO DITAUMASSTOOLS

See also 
https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MissingMassCalculator


Compilation
===========
(root should be set up first, we recommend Root 5.32 64 bit) (Root 5.34 should work also but not regularly tested)

MissingMassCalculator now uses optionally RootCore, see 
https://twiki.cern.ch/twiki/bin/view/Atlas/RootCore

So either one does a standalone compilation with:
make -f Makefile.StandAlone

either one uses RootCore:
In a nutshell, RootCore should be checked out next to MissingMassCalculator, and then for MissingMassCalculator standalone compilation:
cd MissingMassCalculator
source ../RootCore/scripts/setup.sh # to set up RootCore environment
make -f Makefile.RootCore

 .x $ROOTCOREDIR/scripts/load_packages.C+


Unit test
=========
make -f Makefile.Standalone test  
runs MMC for one specific event for multiple configuration. 
If you run you should get at the end "no change in output mass, perfect!". If different mass are obtained wrt to the reference, a WARNING is printed in each case. This might point to a wrong installation.
A sample output is available in share/mmcTest.log. The CPU time printed can also be used to compare (10% difference should not be taken seriously). Note though that the cpu time for this particular event does not necessarily reflects the average CPU time for real events.


Usage in CINT
=============

Example of How to compile and use MMC in a ROOT script :
(there should be a soft link to MissingMassCalculator/MissingMassCalculator directory 
and one to MissingMassCalculator/Root/MissingMassCalculator.cxx in the run directory )


> root
[0] .L MissingMassCalculator.cxx++
[1] .L yourAnalysisScript.C++

a WARNING printed out indicates a bug!


Usage in Python
===============
(to be udpated)
See mmcTest.py for an example. Most importantly you must first:

    import ROOT
    ROOT.gSystem.Load("libMissingMassCalculator.so")


How to use the MissingMassCalculator in your own code
=====================================================

There are four vesrions of MMC currently supported in this tag:
* tune for 2011 data: to be set during initialization by MMC.SetCalibrationSet(MMCCalibrationSet::MMC2011); 
* tune for 2012 data: to be set during initialization by
MMC.SetCalibrationSet(MMCCalibrationSet::MMC2012);
* tune for 2015 data: to be set during initialization by MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015);
* tune for 2015 high mass H/A/h search: to be set during initialization by MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015HIGHMASS);
* tune for Upgrade studies: to be set during initialization by MMC.SetCalibrationSet(MMCCalibrationSet::UPGRADE);
  ** Please see instructions for running MMC with upgrade setup below.  
* tune for LFV X(Z,H,Z')->lep+tau studies: to be set during initialization by MMC.SetCalibrationSet(MMCCalibrationSet::LFVMMC2012);
  and by MMC.SetAlgorithmVersion(4);
  ** Please see instructions for running MMC with LFV setup below.  


After all objects id selection, overlap removal, etc. MMC takes as an input:

* two TLorentzVectors corresponding to the 2 visible taus (el, mu, hadronic tau)
* a TVector2 containing the x,y components of MET
* an integer describing the tau decay mode: 0=electron, 1=muon, 10=1-prong, 30=3-prongs.
* SumEt (RefFinal for di-lepton events, or RefFinal_BDT_medium if hadronic tau is present in event) of the event
* A vector of TLorentzVectors containing all jets of the event (if any)

Here's a simplistic example with explanations:


    //**MMC set inputs**//

    //***ALL UNITS NEED TO BE EXPRESSED IN GeV (not MeV as ATLAS default)***


    TVector2 met_vect(MetX, MetY);//Define MET TVector2; it has to be RefFinal for di-lepton events or RefFinal_BDT_medium for
                                  // events with at least one hadronic tau   
    TLorentzVector VisTau0; //can be either el, mu or tau according to the channel
    TLorentzVector VisTau1; //can be either el, mu or tau according to the channel

    // it is not needed anymore to set the mass of the tau hadron, it
    is reset internally 
    (but electron and mu mass should be set by the user)
    //Those are the average hadronic tau visible mass values as described in truth level.
    // HadHad analysis only. If you are using 2-prong and 4-prong tau's for QCD background templates, please follow these rules:
    //                       0.8 GeV for 2-prong; 1.2 GeV for 4-prong.


    int tau0_decay_type, tau1_decay_type;
    //--> those integers should be filled following the rule: 0 for electron , 1 for muon , 10 for 1-prong, 30 for 3-prong)
    // HadHad analysis only. If you are using 2-prong and 4-prong tau's for QCD background templates, please follow these rules:
    //                       10 for 2-prong; 30 for 4-prong; use only 2p+2p, 2p+4p and 4p+4p events.     

    int Njet25; // number of jets with Et>25 and |eta|<4.5 // for hh 2013 analysis, this should be ET>30
                // Njet25-- is a number of jets passing kinematic cuts above and satisfying all over analysis citeria (JVF, etc.) 
                 

Specific instructions for lep-had channel: analysis of 2011, 2012, or 2015 data  
=======================================================================

    //call function, initialization
    // To save time, initialize MMC outside the event loop
    MissingMassCalculator MMC; // declaring & initializing new object
    MMC->SetUseEfficiencyRecovery(1);      // to recover efficiency loss 

    NEW : now the same code can be used for 2011, 2012 or 2015 tuning. One has to specify one of the following line: 
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015HIGHMASS); // 2015 high mass tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015); // 2015 tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2012); // 2012 tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2011); // 2011 tuning
    IMPORTANT : if SetCalibrationSet has not been called, the MMC will crash immediately when RunMissingMassCalculator is rn

    //give inputs
    MMC->SetMetVec(met_vec); // passing MET vector (in form of TVector2 object) 

    MMC->SetVisTauVec(0,VisTau0;); // passing TLorentzVec for visible tau-0 (first visible tau in the event) 
    MMC->SetVisTauVec(1,VisTau1;); // passing TLorentzVec for visible tau-1 (second visible tau in the event)

    MMC->SetVisTauType(0,tau0_decay_type); // passing decay type of tau-0 
    MMC->SetVisTauType(1,tau1_decay_type); // passing decay type of tau-1
    MMC->SetSumEt(SumEt);                  // passing event sumEt (RefFinal_BDT_medium, see comments above)
    MMC->SetNjet25(Njet25);                // For Lep-Had analysis only

    //run
    int misMassTest=MMC->RunMissingMassCalculator(); // run MMC
    int output_fitstatus=MMC->GetFitStatus(); // MMC output: 1=found solution; 0= no slution
    if(output_fitstatus==1) MMC_mass = MMC->GetFittedMass(MMCFitMethod::MLM); // returns mass according to default method


Specific instructions for had-had channel: analysis of 2011, 2012 OR 2015 data only 
========================================================================

    // To save time, initialize MMC outside the event loop
    MissingMassCalculator* MMC; // declaring & initializing new object
    MMC->SetUseEfficiencyRecovery(1);      // to recover efficiency loss 
    // now integrated in the code
     //    MMC.SetUseTailCleanup(0); // turns off "tail clean=up" feature (should be OFF for default analysis)	   

    The same code can be used for 2011, 2012 OR 2015 tuning. One has to specify one of the following line: 
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015HIGHMASS); // 2015 tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2015); // 2015 tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2012); // 2012 tuning
    OR
    MMC.SetCalibrationSet(MMCCalibrationSet::MMC2011); // 2011 tuning
    IMPORTANT : if SetCalibrationSet has not been called, the MMC will crash immediately when RunMissingMassCalculator is run


    //give inputs
    MMC->SetMetVec(met_vec); // passing MET vector (in form of TVector2 object) 

    MMC->SetVisTauVec(0,VisTau0;); // passing TLorentzVec for visible tau-0 (first visible tau in the event) 
    MMC->SetVisTauVec(1,VisTau1;); // passing TLorentzVec for visible tau-1 (second visible tau in the event)

    MMC->SetVisTauType(0,tau0_decay_type); // passing decay type of tau-0 
    MMC->SetVisTauType(1,tau1_decay_type); // passing decay type of tau-1
    MMC->SetSumEt(SumEt);                  // passing event sumEt (RefFinal_BDT_medium)
    MMC->SetNjet25(Njet30);                // Number of jets with Et>30 & |eta|<4.5


    //run
    int misMassTest=MMC->RunMissingMassCalculator(); // run MMC
    int output_fitstatus=MMC->GetFitStatus(); // MMC output: 1=found solution; 0= no slution
    if(output_fitstatus==1) 
      {
	MMC_mass = MMC->GetFittedMass(MMCFitMethod::MLM); // returns mass according to default method hh2013 now using meth 1 instead of 0
	MMC_rec4vec=MMC->GetResonanceVec(MMCFitMethod::MAXW); // Optional: returns resonance 4-vec, if you need it
	nu0=MMC->GetNeutrino4vec(MMCFitMethod::MAXW,0); // Optional: returns 4-vec for neutrino-1
	nu1=MMC->GetNeutrino4vec(MMCFitMethod::MAXW,1); // Optional:
    returns 4-vec for neutrino-2
       two possible method can be used to compute neutrino (or
    resonance) 4-vector : MAXW : point in parameter space with the
    maximum weight, MLNU3P : most likely neutrinos 3-momentum (6
    components in total). Both method can be used to get an MMC mass
    but MMCFitMethod::MLM is the preferred one. 
      }




Specific instructions for UPGRADE studies
===========================================================================================

* These instructions are the same for lep-had and had-had channels; lep-lep channel is not suppoorted yet. 
* This MMC tag requires METSmearing-00-00-05 tag, which allows to access MET resolution via getMETandRes() function.
// Initializing MMC (to save time, do initialization only once outside the event loop)
   MissingMassCalculator fMMC;
   fMMC.SetCalibrationSet(MMCCalibrationSet::UPGRADE);
// Running MMC (inside event loop)
   fMMC.SetMetVec(smeared_met); // input: TVector2 for MET smeared by METSmearing package
   fMMC.SetMetScanParams(0.0,METresolution,METresolution); // METresolution should be obtained from getMETandRes() in METSmearing package
// if lep-had channel
   fMMC.SetVisTauVec(0,smeared_lep_vec); // smeared lepton 4-vector; please read instruction above to properly set mass of leptons  
   fMMC.SetVisTauVec(1,smeared_tau_vec); // smeared tau 4 vector; please read instructions above to properly set tau mass 
   fMMC.SetVisTauType(0,tau0Type); // tau0Type=0 for electron and tau0Type=1 for muon 
   fMMC.SetVisTauType(1,tau1Type); // tau1Type=10 for 1-prongs and tau1Type=30 for 3-prongs
// if had-had channel
   fMMC.SetVisTauVec(0,smeared_tau0_vec); // smeared tau 4-vector; please read instructions above to properly set tau mass  
   fMMC.SetVisTauVec(1,smeared_tau1_vec); // smeared tau 4-vector; please read instructions above to properly set tau mass 
   fMMC.SetVisTauType(0,tau0Type); // tau*Type=10 for 1-prongs and tau*Type=30 for 3-prongs  
   fMMC.SetVisTauType(1,tau1Type);
// accessing results
   int mmc_stat=fMMC.RunMissingMassCalculator(); // run MMC 
   mmc_stat=fMMC.GetFitStatus();  
   if(mmc_stat==1)
     {
       double mmc_mass1=fMMC.GetFittedMass(MMCFitMethod::MLM); // recommended way to access MMC mass 
     }


Specific instructions for LFV studies
===========================================================================================
WARNING LFV HAS NOT BEEN REIMPLEMENTED IN THIS VERSION
* There are two sets of instructions: for X->lep+tau->e+mu+2nu (lep-lep) and X->lep+tau->lep+had+nu (lep-had) final states
//---------------
** Lep-lep specific instructions
// Initializing MMC (to save time, do initialization only once outside the event loop)
  MissingMassCalculator fMMC;
  fMMC.SetCalibrationSet(MMCCalibrationSet::LFVMMC2012);
  fMMC.SetAlgorithmVersion(4);
  fMMC.SetNiterFit2(40);
  fMMC.SetNiterFit3(10);
  fMMC.SetNsigmaMETscan(4.0);
  fMMC.SetUseTauProbability(1);
// Running MMC (inside event loop)
  fMMC.SetMetVec(met_vect); // passing MET vector (in form of TVector2 object), see instructions above, MMC is tuned for RefFinal_STVF	      
  fMMC.SetVisTauVec(0,lep0); // passing TLorentzVec for leading lepton (lepton coming directly from X(Z or H) LFV decay)
  fMMC.SetVisTauVec(1,lep1); // passing TLorentzVec for sub-leading lepton (lepton from tau decay)	      
  fMMC.SetVisTauType(0,lep0Type); // passing type of lepton-0: 0=electron, 1=muon
  fMMC.SetVisTauType(1,lep1Type); // passing type of lepton-1: 0=electron, 1=muon
  fMMC.SetSumEt(evnt_met_sumet);  // passing event sumEt (RefFinal_STVF)
  fMMC.SetNjet25(good_jets_pt30.size()); // Number of jets with Et>30 & |eta|<4.5 (after JVF cuts are applied)
  fMMC.SetLFVmode(lep1Type);
// accessing results (inside event loop)
  int misMassTest=fMMC.RunMissingMassCalculator(); // run MMC
  int output_fitstatus=fMMC.GetFitStatus(); // MMC output: 1=found solution; 0= no slution
  if(output_fitstatus==1)
    {
      double MMC_mass = fMMC.GetFittedMass(0); 
    }
//---------------
** Lep-had specific instructions
// Initializing MMC (to save time, do initialization only once outside the event loop)
  MissingMassCalculator fMMC;
  fMMC.SetCalibrationSet(MMCCalibrationSet::LFVMMC2012);
  fMMC.SetAlgorithmVersion(4);
  fMMC.SetNiterFit2(40);
  fMMC.SetUseTauProbability(1);
// Running MMC (inside event loop)
  fMMC.SetMetVec(met_vect); // passing MET vector (in form of TVector2 object), MMC is tuned for RefFinal_STVF	      
  fMMC.SetVisTauVec(0,lep); // passing TLorentzVec for lepton
  fMMC.SetVisTauVec(1,tau); // passing TLorentzVec for visible tau
  // note that mass of visible tau 4-vec has to be set to 1.1 GeV for 1-prongs and 1.35 GeV for 3-prongs	      
  fMMC.SetVisTauType(0,lepType); // passing type of lepton: 0=electron; 1=muon
  fMMC.SetVisTauType(1,tauType); // passing decay type of tau: 10=1-prong; 30=3-prong
  fMMC.SetSumEt(evnt_met_sumet); // passing event sumEt (RefFinal_STVF)
  fMMC.SetNjet25(good_jets_pt30.size()); // Number of jets with Et>30 & |eta|<4.5 (after JVF cuts)
// accessing results (inside event loop)
  int misMassTest=fMMC.RunMissingMassCalculator(); // run MMC
  int output_fitstatus=fMMC.GetFitStatus(); // MMC output: 1=found solution; 0= no slution
  if(output_fitstatus==1)



Optional features: for MVA analysis development, all features are validated and safe to use 
====================================================================================
MMC provides not only reconstructed mass by also a full access to kinematics of 
the tau-tau system. These features can be used Higgs CP/spin studies or in MVA 
analysis.

// to turn OFF preset initial values, one need to do the following right after initialization 
        MMC->SetUseDefaults(0);      // turns OFF pre-set values and allows to invoke various switches

//---------- event kinematics according to method-0
	MMC_rec4vec=MMC->GetResonanceVec(MMCFitMethod::MAXW); // Optional: returns resonance 4-vec, if you need it
	nu0=MMC->GetNeutrino4vec(MMCFitMethod::MAXW,0); // Optional: returns 4-vec for neutrino-1
	nu1=MMC->GetNeutrino4vec(MMCFitMethod::MAXW,1); // Optional: returns 4-vec for neutrino-2
	tau0=MMC->GetTau4vec(MMCFitMethod::MAXW,0); // Optional: returns 4-vec for tau-1
	tau1=MMC->GetTau4vec(MMCFitMethod::MAXW,1); // Optional: returns 4-vec for tau-2
//---------- event kinematics according to method-2
	MMC_rec4vec=MMC->GetResonanceVec(MMCFitMethod::MLNU3P); // Optional: returns resonance 4-vec, if you need it
	nu0=MMC->GetNeutrino4vec(MMCFitMethod::MLNU3P,0); // Optional: returns 4-vec for neutrino-1
	nu1=MMC->GetNeutrino4vec(MMCFitMethod::MLNU3P,1); // Optional: returns 4-vec for neutrino-2
	tau0=MMC->GetTau4vec(MMCFitMethod::MLNU3P,0); // Optional: returns 4-vec for tau-1
	tau1=MMC->GetTau4vec(MMCFitMethod::MLNU3P,1); // Optional: returns 4-vec for tau-2

Method-MMCFitMethod::MAXW and Method-MMCFitMethod::MLNU3P are two different methods to obtain the best MMC solution.


A few technical points
======================

For studies, MMC optimization and code development, there is a number of nobs, which can be chnaged.
If you want to explore these options please contact one of the MMC experts and code developers:  
Dimitris Varouchas, Sasha Pranko or David Rousseau.

Currently, running on a SM Higgs sample using a 64bit ROOT version, this the CPU time MMC (default version) needs per channel:

HadHad    ~ 0.02 s/event
LepHad     ~ 0.08  s/event 
LepLep      ~ 0.8 s/event



Older Tutorials
=========

Find tutorial slides here:

* Dimitris Varouchas, how to run MMC : https://indico.cern.ch/getFile.py/access?contribId=3&resId=5&materialId=slides&confId=143074
* Sasha Pranko, Ideas on how to tune MMC performance : https://indico.cern.ch/getFile.py/access?contribId=5&resId=0&materialId=slides&confId=143074  
 

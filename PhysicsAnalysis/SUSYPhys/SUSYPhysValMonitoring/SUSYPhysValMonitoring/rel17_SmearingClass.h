/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////
// SmearingClass.h -- ATLAS Experiment Software //
//////////////////////////////////////////////////
///
/// class providing corrections to the simulated muon pT
/// to match pT in data.
/// Resolution smearing and scale shift numbers are preliminary
/// 
/// Version for simulation and data from
/// RELEASE 16 (autumn reprocessing)
///
/// responsible: atlas-perf-muons-conveners (at) cern.ch
/// 
#ifndef SUSYTOOLS_SMEARINGCLASS_H
#define SUSYTOOLS_SMEARINGCLASS_H

#include <cstring>
#include <iostream>
#include <stdio.h>
#include <TROOT.h>
#include <math.h>
#include "TRandom3.h"

class SmearingClass{
 public:
  /*Constructors*/
  SmearingClass() {Initialize();}
  
  
  /************/
  /* Methods */    
  /**********/
  
  /** internal use only **/
  void Initialize() {    
    detType="";
    GeV=1000;
    gRand = new TRandom3();
    gRand->SetSeed(0);
    useTan2=true;
    useScale=false;
    ptms=0;
    ptid=0;
    ptcb=0;
    eta=0;
  }
  
  /** configure smearing **/
  
  void SetSeed(int seed) {gRand->SetSeed(seed);}
  void SetSeed(int evtnum, int muon_index, int offset=680049) {gRand->SetSeed(offset + evtnum + muon_index*100);}
  void UseTan2(bool U) {useTan2=U;}/* use pT/tan(theta)^2 parameterization for CSC region */  
  void UseGeV() {GeV=1;}  /* if input momenta are in GeV */
  void UseScale(bool applyScale) {useScale=applyScale;}  /* also use momentum scale, set as default */

  
  /* MS or ID smearing only. DetType="MS","ID"*/
  void Event(double Pt, double Eta,std::string DetType) 
  {        
    ptms=0;
    ptid=0;
    ptcb=0;
    
    if (DetType=="MS")      ptms = Pt;
    else if (DetType=="ID") ptid = Pt;
    else std::cout<<"SmearingClass ERROR: wrong DetType in input "<<DetType<<std::endl; 
    eta=Eta;    
    detType=DetType;
    Event();
  }
  
  /* For full MS ID and CB smearing */
  void Event(double PtMS, double PtID, double PtCB, double Eta) 
  {
    detType="All";
    ptms=PtMS;
    ptid=PtID;
    ptcb=PtCB;
    eta=Eta;
    Event();
  }  
  
  void  Event()  {
    smearMS=0;
    smearID=0;
    smearCB=0;
    
    /* Detector Region */
    if (fabs(eta)<1.05)     detRegion=0;
    else if (fabs(eta)<1.7) detRegion=1;
    else if (fabs(eta)<2.0) detRegion=2;
    else if (fabs(eta)<2.5) detRegion=3;
    else detRegion=-1;
    
    g1 = gRand->Gaus(0,1);  
    g2 = gRand->Gaus(0,1); 
    g3 = gRand->Gaus(0,1); 
    g4 = gRand->Gaus(0,1); 
    
    /** Definition: pT'=pT(1+smear) **/    
    smearMS=Smearing("MS");
    smearID=Smearing("ID");
    if (detType=="All") smearCB=Combine(smearMS,smearID);
    ErrorMatrix();    
  }
  

  double Smearing(std::string DetType) {
    
    if (detRegion<0||detRegion>3) return 0;
    
    if (DetType=="MS") {return (MS_MS[detRegion]*g1+MS_AL[detRegion]*g2*ptms/GeV);}
    else if(DetType=="ID") 
      {
	if (useTan2&&detRegion==3) {return (ID_MS[detRegion]*g3+ID_ALTAN[detRegion]*g4*ptid/GeV*sinh(eta)*sinh(eta));}
	else {return (ID_MS[detRegion]*g3+ID_AL[detRegion]*g4*ptid/GeV);}
      } 
    else std::cout<<"Error:: DetType not defined "<<DetType<<std::endl;
    return 0;
  }
  
  double Combine(double SmearMS,double SmearID) {
    /** do the average with the EXPECTED resolutions **/
    
    if (detRegion<0||detRegion>3) return 0;
    if (ptcb==0) {std::cout<<"Warning: ptcb==0"<<std::endl;return 0;}
    
    double SigmaMS=pow(
		       pow(MC_MS_CALO[detRegion]/ptcb*GeV,2)+
		       pow(MC_MS_MS[detRegion],2)+
		       pow(MC_MS_AL[detRegion]*ptcb/GeV,2),0.5);
    
    double SigmaID=pow(
		       pow(MC_ID_MS[detRegion],2)+
		       pow(MC_ID_AL[detRegion]*ptcb/GeV,2),0.5);
    
    if (detRegion==3&&useTan2) {
      SigmaID=pow(
		  pow(MC_ID_MS[detRegion],2)+
		  pow(MC_ID_ALTAN[detRegion]*ptcb/GeV*sinh(eta)*sinh(eta),2),0.5);
    }
    
    
    wMS=1./SigmaMS/SigmaMS;
    wID=1./SigmaID/SigmaID;
    return (SmearMS*wMS+SmearID*wID)/(wMS+wID);
  }
  
  void ErrorMatrix() {
    
    vms=0;
    vid=0;
    corr=0;
    
    if (detRegion<0||detRegion>3) return;
    
    
    if (!useTan2||detRegion!=3){
      double s1=pow(E_MS_MS[detRegion]*E_MS_MS[detRegion]+S_MS_MS[detRegion]*S_MS_MS[detRegion],0.5);
      double s2=pow(E_MS_AL[detRegion]*E_MS_AL[detRegion]+S_MS_AL[detRegion]*S_MS_AL[detRegion],0.5);
      double s3=pow(E_ID_MS[detRegion]*E_ID_MS[detRegion]+S_ID_MS[detRegion]*S_ID_MS[detRegion],0.5);
      double s4=pow(E_ID_AL[detRegion]*E_ID_AL[detRegion]+S_ID_AL[detRegion]*S_ID_AL[detRegion],0.5);
      
      vms=g1*g1*s1*s1+
	g2*g2*s2*s2*ptms/GeV*ptms/GeV+
	2.0*g1*g2*ptms/GeV*s1*s2*CorrMat[detRegion][0];
      vms=pow(vms,0.5);
      
      vid=g3*g3*s3*s3+g4*g4*s4*s4*ptid/GeV*ptid/GeV+2.0*g3*g4*s3*s4*ptid/GeV*CorrMat[detRegion][5];
      vid=pow(vid,0.5);
      
      if (vms*vid!=0)
	corr=(g1*s1*(g3*s3*CorrMat[detRegion][1]+g4*s4*ptid/GeV*CorrMat[detRegion][2])+
	      g2*s2*ptms/GeV*(g3*s3*CorrMat[detRegion][3]+g4*s4*ptid/GeV*CorrMat[detRegion][4]))/vms/vid;
      
    } else {
      double s1=pow(E_MS_MS[detRegion]*E_MS_MS[detRegion]+S_MS_MS[detRegion]*S_MS_MS[detRegion],0.5);
      double s2=pow(E_MS_AL[detRegion]*E_MS_AL[detRegion]+S_MS_AL[detRegion]*S_MS_AL[detRegion],0.5);
      double s3=pow(E_ID_MS[detRegion]*E_ID_MS[detRegion]+S_ID_MS[detRegion]*S_ID_MS[detRegion],0.5);
      double s4=pow(E_ID_ALTAN[detRegion]*E_ID_ALTAN[detRegion]+S_ID_ALTAN[detRegion]*S_ID_ALTAN[detRegion],0.5);
      
      vms=g1*g1*s1*s1+
	g2*g2*s2*s2*ptms/GeV*ptms/GeV+
	2.0*g1*g2*ptms/GeV*s1*s2*CorrMatTan[detRegion][0];
      vms=pow(vms,0.5);
      
      vid=g3*g3*s3*s3+g4*g4*s4*s4*ptid/GeV*ptid/GeV*sinh(eta)*sinh(eta)*sinh(eta)*sinh(eta)
	+2.0*g3*g4*s3*s4*ptid/GeV*sinh(eta)*sinh(eta)*CorrMatTan[detRegion][5];
      vid=pow(vid,0.5);
      
      if (vms*vid!=0)
	corr=(g1*s1*(g3*s3*CorrMatTan[detRegion][1]+g4*s4*ptid/GeV*sinh(eta)*sinh(eta)*CorrMatTan[detRegion][2])+
	      g2*s2*ptms/GeV*(g3*s3*CorrMatTan[detRegion][3]+g4*s4*ptid/GeV*sinh(eta)*sinh(eta)*CorrMatTan[detRegion][4]))/vms/vid;
      
    }
    
  }

  
  double pTMS() {return ptms*(1+smearMS);} /* smeared muon spectrometer pT */
  double pTMS(double SmearMS) {return ptms*(1+SmearMS);} /* smeared muon spectrometer pT */
  
  double pTID() {return ptid*(1+smearID);}  /* smeared inner detector pT */
  double pTID(double SmearID) {return ptid*(1+SmearID);}  /* smeared inner detector pT */
  
  double pTCB() {
    if (useScale && detRegion>-1 && detRegion<4) return scale_CB[detRegion]*ptcb*(1+smearCB);
    else return ptcb*(1+smearCB);
  }  /* smeared combined pT*/

  double pTCB(double SmearCB) {
    if (useScale && detRegion>-1 && detRegion<4) return scale_CB[detRegion]*ptcb*(1+SmearCB);
    else return ptcb*(1+SmearCB);
  }  /* smeared combined pT*/
  
  
  double SMS() {return smearMS;}
  double SID() {return smearID;}
  double SCB() {return smearCB;}
  
  double VMS()  {return vms;}  /* error smearMS */
  double VID()  {return vid;}  /* error smearID */
  double Corr() {return corr;} /* correlation between smearMS and smearID */
  
  
  void MSUP(double &PtMS) {
    double SmearMS=smearMS+Sign(smearMS)*vms;
    PtMS=pTMS(SmearMS);
  }
  
  void MSUP(double &PtMS,double &PtID,double &PtCB) {
    double SmearMS=smearMS+Sign(smearMS)*vms;
    double SmearID=smearID+Sign(smearMS)*vid*corr;
    
    PtMS=pTMS(SmearMS);
    PtID=pTID(SmearID);
    if (detType=="All") {
      double SmearCB=Combine(SmearMS,SmearID);
      PtCB=pTCB(SmearCB);
    }
  }
  
  void MSLOW(double &PtMS) {
    double SmearMS=smearMS-Sign(smearMS)*vms;
    PtMS=pTMS(SmearMS);
  }
  
  void MSLOW(double &PtMS,double &PtID,double &PtCB) {
    double SmearMS=smearMS-Sign(smearMS)*vms;
    double SmearID=smearID-Sign(smearMS)*vid*corr;
    
    PtMS=pTMS(SmearMS);
    PtID=pTID(SmearID);
    if (detType=="All") {
      double SmearCB=Combine(SmearMS,SmearID);
      PtCB=pTCB(SmearCB);
    }
  }


  void IDUP(double &PtID) {
    double SmearID=smearID+Sign(smearID)*vid;
    PtID=pTID(SmearID);
  }
  
  void IDUP(double &PtMS,double &PtID,double &PtCB) {
    double SmearMS=smearMS+Sign(smearID)*vms*corr;
    double SmearID=smearID+Sign(smearID)*vid;
    
    PtMS=pTMS(SmearMS);
    PtID=pTID(SmearID);
    if (detType=="All") {
      double SmearCB=Combine(SmearMS,SmearID);
      PtCB=pTCB(SmearCB);
    }
  }

  void IDLOW(double &PtID) {
    double SmearID=smearID-Sign(smearID)*vid;
    PtID=pTID(SmearID);
  }
  
  void IDLOW(double &PtMS,double &PtID,double &PtCB) {
    double SmearMS=smearMS-Sign(smearID)*vms*corr;
    double SmearID=smearID-Sign(smearID)*vid;
    
    PtMS=pTMS(SmearMS);
    PtID=pTID(SmearID);
    if (detType=="All") {
      double SmearCB=Combine(SmearMS,SmearID);
      PtCB=pTCB(SmearCB);
    }
  }
  


  void PTVar(double &Pt,std::string fun) {
    if (fun=="IDUP") IDUP(Pt);
    else if (fun=="IDLOW") IDLOW(Pt);
    else if (fun=="MSUP") MSUP(Pt);
    else if (fun=="MSLOW") MSLOW(Pt);
  }
  
  void PTVar(double &PtMS,double &PtID,double &PtCB,std::string fun) {
    if (fun=="IDUP") IDUP(PtMS,PtID,PtCB);
    else if (fun=="IDLOW") IDLOW(PtMS,PtID,PtCB);
    else if (fun=="MSUP") MSUP(PtMS,PtID,PtCB);
    else if (fun=="MSLOW") MSLOW(PtMS,PtID,PtCB);
  }
  

  double Sign(double x){return (x<0? -1 : 1);}


  /** simple methods for retrieving input values (not needed since all members are public) **/
      
  double ptms_orig() {return ptms;}
  double ptid_orig() {return ptid;}
  double ptcb_orig() {return ptcb;}
  double Eta() {return eta;}  
  int DetRegion() {return detRegion;}


  /*members*/
  TRandom3* gRand;
  double ptms,ptid,ptcb,eta;
  double vms,vid,corr;
  double smearMS,smearID,smearCB;
  bool   useTan2;
  std::string detType;
  int detRegion;
  double GeV;
  double g1,g2,g3,g4;
  double wMS,wID;
  bool useScale;

  static const double scale_CB[4];   
  static const double scaleSyst_CB[4];  

  static const double ID_MS[4];   
  static const double ID_AL[4];   
  static const double ID_ALTAN[4];
  static const double MS_MS[4];   
  static const double MS_AL[4];   

  static const double E_ID_MS[4];   
  static const double E_ID_AL[4];   
  static const double E_ID_ALTAN[4];
  static const double E_MS_MS[4];   
  static const double E_MS_AL[4];   
  
  static const double S_ID_MS[4];   
  static const double S_ID_AL[4];   
  static const double S_ID_ALTAN[4];
  static const double S_MS_MS[4];   
  static const double S_MS_AL[4];   

  static const double MC_ID_MS[4];   
  static const double MC_ID_AL[4];   
  static const double MC_ID_ALTAN[4];
  static const double MC_MS_CALO[4]; 
  static const double MC_MS_MS[4];   
  static const double MC_MS_AL[4];   
  
  static const double CorrMat[4][6];
  static const double CorrMatTan[4][6];
  
};

#endif // not SUSYTOOLS_SMEARINGCLASS_H

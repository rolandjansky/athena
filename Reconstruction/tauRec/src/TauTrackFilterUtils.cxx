/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauTrackFilterUtils.cxx
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-13
//
//
//-----------------------------------------------------------------------------

#include "TF1.h"
#include "TLorentzVector.h"
#include "tauRec/TauTrackFilterUtils.h"
#include <vector>
#include <iostream>
#include <string>

using namespace TauTrackFilterUtils;

bool TauTrackFilterUtils::pass3prong(std::vector<TauTrackFilterUtils::TrackInfo> combination,TLorentzVector tau){

   //Step 1: calculate angles
   TauTrackFilterUtils::TrackPair lp, lm, sp, ls, ms, mp;
   
   lm.angle = fabs(combination[0].p4.Angle(combination[1].p4.Vect()));
   lm.mass = (combination[0].p4+combination[1].p4).M();
   lm.charge = combination[0].charge*combination[1].charge;
   ls.angle = fabs(combination[0].p4.Angle(combination[2].p4.Vect()));
   ls.mass = (combination[0].p4+combination[2].p4).M();
   ls.charge = combination[0].charge*combination[2].charge;
   ms.angle = fabs(combination[1].p4.Angle(combination[2].p4.Vect()));
   ms.mass = (combination[1].p4+combination[2].p4).M();
   ms.charge = combination[1].charge*combination[2].charge;

	lp = lm;
	if(ls.angle > lp.angle){
		mp = lp;
		lp = ls;
	}
	else mp = ls;
	if(ms.angle > lp.angle){
		sp = mp;
		mp = lp;
		lp = ms;
	}   
	else if(ms.angle > mp.angle){
		sp = mp;
		mp = ms;
	}
	else sp = ms;

   //if (lp.angle < mp.angle) ATH_MSG_WARNING("Largest angle is smaller than medium angle!");
   //if (lp.angle < sp.angle) ATH_MSG_WARNING("Largest angle is smaller than smallest angle!");
   //if (mp.angle < sp.angle) ATH_MSG_WARNING("Medium angle is smaller than smallest angle!");
   
   //Step 3: calculate 99% angles
   float lp99 = 0, sp99 = 0, lm99 = 0, ls99 = 0;
   float p = tau.P(), eta = fabs(tau.Eta());
   
   float a[9][4];
   int npar = 4, npol = 3;

a[0][0] = 0.179041; a[1][0] = -0.0531058; a[2][0] = 0; 
a[0][1] = -0.0146875; a[1][1] = 0.00414247; a[2][1] = -0.000612045; 
a[0][2] = 0.0188939; a[1][2] = -0.00452375; a[2][2] = 0.00120015; 
a[0][3] = 58.3066; a[1][3] = -48.2594; a[2][3] = 26.8864; 
   lp99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.0741985; a[1][0] = -0.0181941; a[2][0] = 0; 
a[0][1] = -0.0149252; a[1][1] = 0.00512965; a[2][1] = -0.00125462; 
a[0][2] = 0.00802004; a[1][2] = -0.00252272; a[2][2] = 0.000761022; 
a[0][3] = 25.0145; a[1][3] = 0; a[2][3] = 0; 
   sp99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.102084; a[1][0] = -0.0256446; a[2][0] = 0; 
a[0][1] = -0.014259; a[1][1] = 0.00465467; a[2][1] = -0.00122856; 
a[0][2] = 0.010552; a[1][2] = -0.00176856; a[2][2] = 0.000446776; 
a[0][3] = 36.0848; a[1][3] = -16.1489; a[2][3] = 10.2994; 
   lm99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.152783; a[1][0] = -0.0390978; a[2][0] = 0; 
a[0][1] = -0.0139914; a[1][1] = 0.00352551; a[2][1] = -0.000624039; 
a[0][2] = 0.0159925; a[1][2] = -0.00332104; a[2][2] = 0.00100568; 
a[0][3] = 43.5804; a[1][3] = -18.681; a[2][3] = 6.29988; 
   ls99 = ComputeAngle(p,eta,a,npar,npol);
	   
   //Step 4: compare angles and track masses, pass if all pass, fail otherwise
   if((lp.angle > lp99)||(sp.angle > sp99)||(lm.angle > lm99)||(ls.angle > ls99)) return false; //One or more of the angles has failed - not a three prong tau
   return true; //Track combination is a 3prong candidate!
} //End pass3prong (efficiency studies)

bool TauTrackFilterUtils::pass2prong(std::vector<TauTrackFilterUtils::TrackInfo> pair,TLorentzVector tau){
   float angle = fabs(pair[0].p4.Angle(pair[1].p4.Vect()));
   int  charge = pair[0].charge*pair[1].charge;

   // Used to have more vars, but some were unused.
   //float lt99 = 0, mt99 = 0, st99 = 0, ct99 = 0, lp99 = 0, mp99 = 0, sp99 = 0, los99 = 0, sos99 = 0, ss99 = 0, lm99 = 0, ls99 = 0, ms99 = 0;
   float lp99 = 0, mp99 = 0, sp99 = 0, los99 = 0, sos99 = 0, ss99 = 0, lm99 = 0;
   float p = tau.P(), eta = fabs(tau.Eta());
   
   float a[9][4];
   int npar = 4, npol = 9;

   a[0][0] = 0.0584232; a[1][0] = -0.0177642; a[2][0] = 0; a[3][0] = 0; a[4][0] = 0; a[5][0] = 0; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
   a[0][1] = 0.0447435; a[1][1] = -0.659295; a[2][1] = 2.99202; a[3][1] = -6.10742; a[4][1] = 6.34017; a[5][1] = -3.49095; a[6][1] = 0.972228; a[7][1] = -0.107807; a[8][1] = 0; 
   a[0][2] = -0.249078; a[1][2] = 3.75779; a[2][2] = -18.9563; a[3][2] = 45.4474; a[4][2] = -59.333; a[5][2] = 44.722; a[6][2] = -19.4586; a[7][2] = 4.54039; a[8][2] = -0.4399; 
   a[0][3] = 124.481; a[1][3] = -1129.76; a[2][3] = 5198.92; a[3][3] = -10538.1; a[4][3] = 10741.4; a[5][3] = -5757; a[6][3] = 1548.86; a[7][3] = -164.644; a[8][3] = 0; 
   //lt99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.057286; a[1][0] = -0.0168061; a[2][0] = 0; a[3][0] = 0; a[4][0] = 0; a[5][0] = 0; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
a[0][1] = 0.0640448; a[1][1] = -0.922493; a[2][1] = 4.10239; a[3][1] = -8.19704; a[4][1] = 8.35619; a[5][1] = -4.52961; a[6][1] = 1.24415; a[7][1] = -0.136244; a[8][1] = 0; 
a[0][2] = -0.222389; a[1][2] = 3.34829; a[2][2] = -16.8256; a[3][2] = 40.1156; a[4][2] = -52.0129; a[5][2] = 38.9152; a[6][2] = -16.8076; a[7][2] = 3.89426; a[8][2] = -0.374831; 
a[0][3] = 97.8443; a[1][3] = -804.025; a[2][3] = 3412.76; a[3][3] = -6058.05; a[4][3] = 5028.88; a[5][3] = -1940.87; a[6][3] = 281.19; a[7][3] = 0; a[8][3] = 0; 
   //ct99 = ComputeAngle(p,eta,a,npar,npol);
   npol = 3;
a[0][0] = 0.0665222; a[1][0] = 0; a[2][0] = 0; 
a[0][1] = -0.018755; a[1][1] = 0.00258183; a[2][1] = 0; 
a[0][2] = 0.045607; a[1][2] = -0.0234824; a[2][2] = 0.00375319; 
a[0][3] = 43.8011; a[1][3] = -10.0462; a[2][3] = 0; 
   //mt99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.156972; a[1][0] = -0.0333305; a[2][0] = 0; 
a[0][1] = -0.0231364; a[1][1] = 0.0120482; a[2][1] = -0.00289192; 
a[0][2] = 0.0490898; a[1][2] = -0.0273084; a[2][2] = 0.00547379; 
a[0][3] = 33.1651; a[1][3] = 0; a[2][3] = 0; 
   //st99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.179041; a[1][0] = -0.0531058; a[2][0] = 0; 
a[0][1] = -0.0146875; a[1][1] = 0.00414247; a[2][1] = -0.000612045; 
a[0][2] = 0.0188939; a[1][2] = -0.00452375; a[2][2] = 0.00120015; 
a[0][3] = 58.3066; a[1][3] = -48.2594; a[2][3] = 26.8864; 
   lp99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.142962; a[1][0] = -0.0397119; a[2][0] = 0; 
a[0][1] = -0.014084; a[1][1] = 0.00437622; a[2][1] = -0.000992845; 
a[0][2] = 0.0145659; a[1][2] = -0.00270987; a[2][2] = 0.00079432; 
a[0][3] = 42.4831; a[1][3] = -25.893; a[2][3] = 13.6075; 
   mp99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.0741985; a[1][0] = -0.0181941; a[2][0] = 0; 
a[0][1] = -0.0149252; a[1][1] = 0.00512965; a[2][1] = -0.00125462; 
a[0][2] = 0.00802004; a[1][2] = -0.00252272; a[2][2] = 0.000761022; 
a[0][3] = 25.0145; a[1][3] = 0; a[2][3] = 0; 
   sp99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.177021; a[1][0] = -0.0800858; a[2][0] = 0.017266; 
a[0][1] = -0.0145132; a[1][1] = 0.00508756; a[2][1] = -0.00133994; 
a[0][2] = 0.0174059; a[1][2] = -0.00407948; a[2][2] = 0.00130897; 
a[0][3] = 59.5959; a[1][3] = -51.819; a[2][3] = 28.742; 
   los99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.126153; a[1][0] = -0.0504026; a[2][0] = 0.0100601; 
a[0][1] = -0.01373; a[1][1] = 0.0040825; a[2][1] = -0.00103933; 
a[0][2] = 0.0121626; a[1][2] = -0.00239224; a[2][2] = 0.000832398; 
a[0][3] = 43.6455; a[1][3] = -34.4061; a[2][3] = 17.558; 
   sos99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.159394; a[1][0] = -0.0461081; a[2][0] = 0; 
a[0][1] = -0.0148102; a[1][1] = 0.00429109; a[2][1] = -0.000670516; 
a[0][2] = 0.0167114; a[1][2] = -0.00539364; a[2][2] = 0.00175181; 
a[0][3] = 48.371; a[1][3] = -35.9336; a[2][3] = 19.3991; 
   ss99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.102084; a[1][0] = -0.0256446; a[2][0] = 0; 
a[0][1] = -0.014259; a[1][1] = 0.00465467; a[2][1] = -0.00122856; 
a[0][2] = 0.010552; a[1][2] = -0.00176856; a[2][2] = 0.000446776; 
a[0][3] = 36.0848; a[1][3] = -16.1489; a[2][3] = 10.2994; 
   lm99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.152783; a[1][0] = -0.0390978; a[2][0] = 0; 
a[0][1] = -0.0139914; a[1][1] = 0.00352551; a[2][1] = -0.000624039; 
a[0][2] = 0.0159925; a[1][2] = -0.00332104; a[2][2] = 0.00100568; 
a[0][3] = 43.5804; a[1][3] = -18.681; a[2][3] = 6.29988; 
   //ls99 = ComputeAngle(p,eta,a,npar,npol);
a[0][0] = 0.160615; a[1][0] = -0.0284831; a[2][0] = -0.00879631; 
a[0][1] = -0.0140811; a[1][1] = 0.00344844; a[2][1] = -0.000421752; 
a[0][2] = 0.0173056; a[1][2] = -0.00371573; a[2][2] = 0.00112158; 
a[0][3] = 59.28; a[1][3] = -48.2821; a[2][3] = 26.3103; 
   //ms99 = ComputeAngle(p,eta,a,npar,npol);
   
   if ((angle < lm99)&&((angle < lp99)||(angle < mp99)||(angle < sp99))){   
   	if((charge == -1)&&((angle < los99)||(angle < sos99))) return true;
   	else if((charge == 1)&&(angle < ss99)) return true;
   	else return false;
   }
   else return false;
} //End pass2prong

bool TauTrackFilterUtils::pass1prong(TLorentzVector track,TLorentzVector tau){
   //Step 1: Compute Angle Between Track and Tau
   float angle = fabs(track.Angle(tau.Vect()));
   //Step 2: Compute 99% angle
   float p = tau.P(), eta = fabs(tau.Eta());
   
   int npar = 4, npol = 3;
   float a[3][4];
   
   a[0][0] = 0.120777; a[1][0] = -0.0261681; a[2][0] = 0;
   a[0][1] = -0.0307174; a[1][1] = 0.0170112; a[2][1] = -0.00381298;
   a[0][2] = 0.0662689; a[1][2] = -0.0402811; a[2][2] = 0.00760013;
   a[0][3] = 24.512; a[1][3] = 0; a[2][3] = 0;
   float angle99 = ComputeAngle(p,eta,a,npar,npol);

   //Step 3: compare angles and return decision
   if(angle > angle99) return false; //Track angle exceeds kinematic boundary
   else return true;
} //End pass1prong

float TauTrackFilterUtils::ComputePi0Cone(int recProngs,TLorentzVector tau){
   float angle = -1;
   float atrue = 0, arec = 0;
   float p = tau.P(), eta = fabs(tau.Eta());   
   int npar = 4, npol = 9;
   float a[9][4];
   switch(recProngs){
      case 3:  //3 Prong Case
      	npol = 7;
         a[0][0] = 0.0457602; a[1][0] = 1.80062; a[2][0] = -6.82921; a[3][0] = 10.8605; a[4][0] = -8.52901; a[5][0] = 3.24106; a[6][0] = -0.473647;
         a[0][1] = -0.017874; a[1][1] = -0.0502181; a[2][1] = 0.162668; a[3][1] = -0.172266; a[4][1] = 0.0783324; a[5][1] = -0.013098; a[6][1] = 0;
         a[0][2] = 0.0266511; a[1][2] = -0.013319; a[2][2] = 0.00289217; a[3][2] = 0; a[4][2] = 0; a[5][2] = 0; a[6][2] = 0;
         a[0][3] = 237.828; a[1][3] = -2836.67; a[2][3] = 11074.1; a[3][3] = -18578.5; a[4][3] = 15086.9; a[5][3] = -5828.12; a[6][3] = 856.684;
         atrue = ComputeAngle(p,eta,a,npar,npol);
         npar = 3;
         npol = 5;
         a[0][0] = 0.178252; a[1][0] = 0.057474; a[2][0] = -0.256742; a[3][0] = 0.156772; a[4][0] = -0.0283407;
         a[0][1] = -0.00950538; a[1][1] = 0.00363589; a[2][1] = -0.00157984; a[3][1] = 0; a[4][1] = 0;
         a[0][2] = 0.0227538; a[1][2] = -0.0642722; a[2][2] = 0.121818; a[3][2] = -0.0679845; a[4][2] = 0.0115837;
         arec = ComputeAngle(p,eta,a,npar,npol,(char*)"[0]*exp([1]*x)+[2]");
         break;
      case 1: //1 Prong Case
         npol = 6;
         a[0][0] = 0.203158; a[1][0] = 0.269746; a[2][0] = -1.22961; a[3][0] = 1.41234; a[4][0] = -0.670384; a[5][0] = 0.114524;
         a[0][1] = -0.0300622; a[1][1] = -0.0115786; a[2][1] = 0.07541; a[3][1] = -0.0782728; a[4][1] = 0.0334031; a[5][1] = -0.0052381;
         a[0][2] = 0.0423083; a[1][2] = -0.0284378; a[2][2] = 0.0237394; a[3][2] = -0.0168315; a[4][2] = 0.0040657; a[5][2] = 0;
         a[0][3] = 45.0612; a[1][3] = -458.353; a[2][3] = 1521.8; a[3][3] = -1895.88; a[4][3] = 1000.72; a[5][3] = -187.091;
         atrue = ComputeAngle(p,eta,a,npar,npol);
         a[0][0] = 0.168639; a[1][0] = -0.325194; a[2][0] = 1.4594; a[3][0] = -3.20592; a[4][0] = 3.50676; a[5][0] = -2.0571; a[6][0] = 0.621729; a[7][0] = -0.0758951; a[8][0] = 0;
         a[0][1] = -0.0103477; a[1][1] = 0; a[2][1] = 0; a[3][1] = 0; a[4][1] = 0; a[5][1] = 0; a[6][1] = 0; a[7][1] = 0; a[8][1] = 0;
         a[0][2] = 0.0325721; a[1][2] = -0.0496515; a[2][2] = 0.0773747; a[3][2] = -0.0396844; a[4][2] = 0.00615314; a[5][2] = 0; a[6][2] = 0; a[7][2] = 0; a[8][2] = 0;
         arec = ComputeAngle(p,eta,a,npar,npol,(char*)"[0]*exp([1]*x)+[2]");
         break;
      default:
         //ATH_MSG_WARNING("Incorrect number of prongs!");
         return angle;
   }
   if (atrue > arec) angle = atrue;
   else angle = arec;
   return angle;
} //End ComputePi0Cone

float TauTrackFilterUtils::ComputeAngle(float p, float eta, float a[9][4], int npar, int npol, char eqn[]){
   char name[10];
   char poleqn[10];
   
   //TF1* etacoeff[npar]; //FIXME variable length array, use something like: = new TF1[npar];
   std::vector<TF1> etacoeff;
   TF1* pcone = new TF1("pcone",eqn); 
   for(int i=0;i<npar;i++){
      sprintf(name,"p%i",i);
      sprintf(poleqn,"pol%i",npol);
      etacoeff.push_back(TF1(name,poleqn));
      for(int j=0;j<npol;j++) etacoeff.at(i).SetParameter(j,a[j][i]);
		  pcone->SetParameter(i,etacoeff.at(i).Eval(eta));
   }
   float angle = pcone->Eval(p);
   delete pcone;
   return angle;
}

float TauTrackFilterUtils::Compute1dim(float p, float a[10], int npar, char eqn[]){
   TF1* pcone = new TF1("pcone",eqn); 
   for(int i=0;i<npar;i++) pcone->SetParameter(i,a[i]);
   float angle = pcone->Eval(p);
   delete pcone;
   return angle;
}

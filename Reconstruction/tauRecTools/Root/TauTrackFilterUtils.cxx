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
#include "tauRecTools/TauTrackFilterUtils.h"
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
         a[0][0] = 0.0419208; a[1][0] = 0.0481497; a[2][0] = -0.0225545; a[3][0] = 0; a[4][0] = 0; a[5][0] = 0; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
         a[0][1] = -0.00568661; a[1][1] = -0.00336825; a[2][1] = 0.00172832; a[3][1] = 0; a[4][1] = 0; a[5][1] = 0; a[6][1] = 0; a[7][1] = 0; a[8][1] = 0; 
         a[0][2] = 0.00177496; a[1][2] = 0.00296773; a[2][2] = -0.00123081; a[3][2] = 0; a[4][2] = 0; a[5][2] = 0; a[6][2] = 0; a[7][2] = 0; a[8][2] = 0; 
         a[0][3] = 90.9262; a[1][3] = -89.7105; a[2][3] = 49.0447; a[3][3] = 0; a[4][3] = 0; a[5][3] = 0; a[6][3] = 0; a[7][3] = 0; a[8][3] = 0; 
         atrue = ComputeAngle(p,eta,a,npar,npol);
         a[0][0] = 0.187427; a[1][0] = -0.0816934; a[2][0] = 0.0116366; a[3][0] = 0; a[4][0] = 0; a[5][0] = 0; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
         a[0][1] = -0.00961135; a[1][1] = 0.0188071; a[2][1] = -0.0279819; a[3][1] = 0.0175981; a[4][1] = -0.00374356; a[5][1] = 0; a[6][1] = 0; a[7][1] = 0; a[8][1] = 0; 
         a[0][2] = 0.0139015; a[1][2] = -0.0572689; a[2][2] = 0.0897538; a[3][2] = -0.0543513; a[4][2] = 0.0110609; a[5][2] = 0; a[6][2] = 0; a[7][2] = 0; a[8][2] = 0; 
         a[0][3] = -57.066; a[1][3] = 731.569; a[2][3] = -2351.02; a[3][3] = 3576.75; a[4][3] = -2802.87; a[5][3] = 1081.43; a[6][3] = -161.098; a[7][3] = 0; a[8][3] = 0; 
         arec = ComputeAngle(p,eta,a,npar,npol);
         break;
      case 1: //1 Prong Case
         a[0][0] = 0.077467; a[1][0] = -0.0648352; a[2][0] = 0.15807; a[3][0] = -0.111211; a[4][0] = 0.0223358; a[5][0] = 0; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
         a[0][1] = -0.0212485; a[1][1] = 0.213133; a[2][1] = -1.10606; a[3][1] = 2.81065; a[4][1] = -3.95237; a[5][1] = 3.21507; a[6][1] = -1.50167; a[7][1] = 0.373201; a[8][1] = -0.0381986; 
         a[0][2] = 0.0180949; a[1][2] = -0.215859; a[2][2] = 1.06949; a[3][2] = -2.61577; a[4][2] = 3.56621; a[5][2] = -2.82425; a[6][2] = 1.28799; a[7][2] = -0.313272; a[8][2] = 0.0314451; 
         a[0][3] = 55.3658; a[1][3] = 83.3644; a[2][3] = -243.958; a[3][3] = 303.823; a[4][3] = -257.709; a[5][3] = 125.826; a[6][3] = -23.0882; a[7][3] = 0; a[8][3] = 0; 
         atrue = ComputeAngle(p,eta,a,npar,npol);
         a[0][0] = 0.0887773; a[1][0] = 0.183147; a[2][0] = -0.53342; a[3][0] = 0.511497; a[4][0] = -0.207361; a[5][0] = 0.0299467; a[6][0] = 0; a[7][0] = 0; a[8][0] = 0; 
         a[0][1] = 0.00529589; a[1][1] = -0.0931825; a[2][1] = 0.331541; a[3][1] = -0.501175; a[4][1] = 0.356803; a[5][1] = -0.118988; a[6][1] = 0.0150108; a[7][1] = 0; a[8][1] = 0; 
         a[0][2] = -0.0152482; a[1][2] = 0.203442; a[2][2] = -0.799957; a[3][2] = 1.29237; a[4][2] = -0.943621; a[5][2] = 0.315001; a[6][2] = -0.0392101; a[7][2] = 0; a[8][2] = 0; 
         a[0][3] = 46.0655; a[1][3] = -61.8671; a[2][3] = 278.08; a[3][3] = -385.329; a[4][3] = 199.816; a[5][3] = -34.0016; a[6][3] = 0; a[7][3] = 0; a[8][3] = 0; 
         arec = ComputeAngle(p,eta,a,npar,npol);
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
   char name[13];
   char poleqn[13];
   
   //TF1* etacoeff[npar]; //FIXME variable length array, use something like: = new TF1[npar];
   std::vector<TF1> etacoeff;
   TF1* pcone = new TF1("pcone",eqn); 
   for(int i=0;i<npar;i++){
      snprintf(name,sizeof(name),"p%i",i);
      snprintf(poleqn,sizeof(poleqn),"pol%i",npol);
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

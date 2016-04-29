//
// Macro to calculate the B' width for the fourth generation vector quark
// search. 
//
// Set the coupling (gL, gR, kg) and then the code loops over B' masses
// and prints them to the screen and into a text file and shell script
// that is convenient to use for event generation.
//
// Author: Reinhard Schwienhorst, Huaqiao Zhang
// the equations are copied from the Mathematica notebook from Jianghao Yu
//
// Date: July 2012
// run with root -x -q -l -b bprimeWidth.C
// Then run masswidth.sh to create the run and param card files for madgraph.
//
#include "TMath.h"
#include <iostream>
ofstream fout, fshell;
// 
// coupling parameters. You could change these if you wanted to
double gL = 1.0;    // default: 1
double gR = 0;     // default: 0
double kg = 0.5;    // default: 1

// SM parameters
double MT  = 172.5;
double MW  = 80.267;
double MB  = 4.2;
double MH  = 125.0;
double k_z = 1.;
double gs = 1.2177158;
double MZ = 91.545;

double gwrs=0.65189214;
double cwrs=0.876812409;
double vrs=246.259632;

oneMBP (const double& MBP=700, const double& rgL=1, const double& rgR=0, const double& rkg=1);

bprimeWidth(){

  fout.open(Form("Bprime_masswidth_%2.1f_%2.1f_%2.1f.lst",gL,gR,kg));
  fshell.open(Form("masswidth_%2.1f_%2.1f_%2.1f.sh",gL,gR,kg));
  fout << " gL = " << gL <<"; gR = " << gR <<"; kg = "<<kg<<endl;
  fout << " M_W = " << MW <<"; M_T = " << MT << endl;
  fout << "Bprime Mass [GeV]  Width [GeV] " << endl;
  for(int i=0; i<40; i++){
      double inMBP = 300 + 50 * i ;
      cout << inMBP<<" gL = " << gL <<"; gR = " << gR <<"; kg = "<<kg<<endl;
      oneMBP(inMBP,gL,gR,kg);
  }
}

// helper functions for the width calculation
double lambda(const double& m0,const double& m1,const double& m2) {
  return (pow(m0,4) + pow(m1,4) + pow(m2,4) 
	  - 2.*pow(m0,2)*pow(m1,2)
	  - 2.*pow(m0,2)*pow(m2,2)
	  - 2.*pow(m1,2)*pow(m2,2));
}
double lambda2(const double& xf,const double& xv) { return lambda(xf,xv,1.); }
double FFV1(const double& g,const double& fL, const double& fR, const double& mB,const double& mv, const double& xf, const double& xv) {
  return (pow(g,2)/(32.*TMath::Pi())
	  * pow(mB,3)/pow(mv,2)
	  * TMath::Sqrt(lambda2(xf,xv))
	  * ((pow(fL,2)+pow(fR,2))*(1.+pow(xv,2)-2.*pow(xf,2)-2.*pow(xv,4)
				    +pow(xv,2)*pow(xf,2)+pow(xf,4))
	     - 12.*fL*fR*pow(xv,2)*xf) );
}
double FFH(const double& g,const double& yL,const double& yR,const double& mB, const double& mH, const double& xf, const double& xh) {
  return ( pow(g,2)/(32.*TMath::Pi())*mB
	   * TMath::Sqrt(lambda2(xf,xh))
	   * ((pow(yL,2)+pow(yR,2))*(1.+pow(xf,2)-pow(xh,2))
	      -4.*yL*yR*xf) );
}
double FFV2(const double& g,const double& fL, const double& fR, const double& mB,const double& mv, const double& xf, const double& xv) {
  return pow(g,2)/(32.*TMath::Pi())*mB
    * TMath::Sqrt(lambda2(xf,xv))
    * ((pow(fL,2)+pow(fR,2))*(2.-pow(xv,2)-4.*pow(xf,2)-pow(xv,4)
			      -pow(xv,2)*pow(xf,2)+2.*pow(xf,4))
       - 12.*fL*fR*pow(xv,2)*xf);
}


//
// function to compute the partial and total widths at one mass
//
oneMBP (const double& MBP, const double& rgL, const double& rgR, const double& rkg){
  //
  // RS implementing what Jianghao has:
  
  //
  // now check wt and bZ
  double wt = FFV1((gwrs/TMath::Sqrt(2.)),rgL*vrs/MBP,rgR*vrs/MBP,MBP,MW,MT/MBP,MW/MBP);
  
  double bz=FFV1(gwrs/(2.*cwrs),0,k_z*vrs/MBP,MBP,MZ,MB/MBP,MZ/MBP);
  
  //
  // now check bH
  double bh=FFH(gwrs/2.*MBP/MW,0,k_z*vrs/MBP,MBP,MH,MB/MBP,MH/MBP);
  
  //
  // now check bg
  double bg=4./3.*FFV2(gs,0,rkg/2.,MBP,0.,MB/MBP,0.);

  // total width
  double WBP=bg+bh+bz+wt;
  
  // output to text file and script that updates the param_card.dat-bak file
  fout << MBP << " \t\t " << WBP << endl;
  fshell << "cat Cards/param_card.dat-bak  | sed 's/1005 MBPSAVE # MBP/1005 "<<MBP<<" # MBP/g' | sed 's/DECAY 1005 WBPSAVE # WBP/DECAY 1005 "<<WBP<<"  #WBP/g' > mycards/param_card.dat-"<<(int)(MBP)<<Form("_%2.1f_%2.1f_%2.1f",gL,gR,kg)<<endl;
  fshell << Form("rpl FCOUPLL %2.1f mycards/param_card.dat-%d_%2.1f_%2.1f_%2.1f",gL,(int)MBP,gL,gR,kg)<<endl;
  fshell << Form("rpl FCOUPLR %2.1f mycards/param_card.dat-%d_%2.1f_%2.1f_%2.1f",gR,(int)MBP,gL,gR,kg)<<endl;
  fshell << Form("rpl FCOUPLG %2.1f mycards/param_card.dat-%d_%2.1f_%2.1f_%2.1f",kg,(int)MBP,gL,gR,kg)<<endl;

  fshell << "cat Cards/run_card.dat-bak | sed -e 's/MBPSAVE/"<<MBP<<"/g' > mycards/run_card.dat-"<<(int)(MBP)<<endl;
  
  cout <<" DEBUG: wt " << wt << " bh " << bh << " bg " << bg << " bz " << bz << endl;
  cout << "Width of " << MBP << " GeV Bprime is : " << WBP << endl;
  
}


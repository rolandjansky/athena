/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-*-c++-*-
//-*-CrossSection.cpp-*-
//   Written by James Monk and Andrew Pilkington
/////////////////////////////////////////////////////////////////////////////

#include "Exhume_i/CrossSection.h"

// External Fortran routines to link to:

extern "C" {
  double dsimps_(double*, double*, double*, int*);
  double my_pdfset_(double&);
  void   structm_(double *,double *,double *,double *,double *,double *,
		double *,double *,double *,double *,double *);
  void   my_pythia_init_();
  
}
/////////////////////////////////////////////////////////////////////////////

Exhume::CrossSection::CrossSection(std::string _ExhumeControlParams){


  std::cout<<std::endl<<
   " ........................................................................."
	    <<std::endl;
  std::cout<<std::endl<<"  = Initialising CrossSection ="<<std::endl;

  //Put data types into a map and pair with a string
  //for formating in/output.
  TypeMap.insert(PCharPair(typeid(double*).name(),"%f"));
  TypeMap.insert(PCharPair(typeid(float*).name(),"%f"));
  TypeMap.insert(PCharPair(typeid(int*).name(),"%d"));

  //Associate each variable with a string

  insert("AlphaEw",&AlphaEw);
  insert("WMass",&WMass);
  insert("ZMass",&ZMass);
  insert("HiggsMass",&HiggsMass);
  insert("HiggsVev",&HiggsVev);
  insert("MinQt2", &MinQt2);

  insert("TopMass",&TopMass);
  insert("BottomMass",&BottomMass);
  insert("CharmMass",&CharmMass);
  insert("StrangeMass",&StrangeMass);
  insert("TauMass",&TauMass);
  insert("MuonMass",&MuonMass);
  insert("LambdaQCD",&LambdaQCD);
  insert("Freeze",&Freeze);
  insert("B",&B);
  insert("gw",&gw);
  insert("LambdaW",&LambdaW);
  insert("FNAL_or_LHC",&FNAL_or_LHC);
  insert("s",&s);
  insert("root_s",&root_s);
  insert("Rg",&Rg);
  insert("Survive",&Survive);
  insert("PDF",&PDF);


  InvSqrtsHat = 0.;
  InvV1MinusV2  = 0.;
  InvsHat2 = 0.;
  InvsHat = 0.;
  Invsx1x2 = 0.;
  PPhi = 0.;
  Phi1 = 0.;
  Phi2  = 0.;
  Pt1DotPt2 = 0.;
  Pt1 = 0.;
  Pt2 = 0.;
  RgBegin = false;
  RgInterpolate[0] = false; 
  RgInterpolate[1] = false; 
  SqrtsHat  = 0.;
  TBegin = false;
  TInterpolate = false;
  ey = 0.;
  sHat2  = 0.;
  sHat = 0.;
  t1  = 0.;
  t2 = 0.;
  x1p = 0.;
  x1x2 = 0.;
  x1 = 0.;
  x2p = 0.;
  x2 = 0.;
  y = 0.;

  //Assign default values

  WMass = 80.33;
  ZMass = 91.187;
  HiggsMass = 120;
  HiggsVev = 246.0;
  AlphaEw = 1.0/137.0359895;
  TopMass = 175.0;
  BottomMass = 4.6;
  CharmMass = 1.42;
  StrangeMass = 0.19;
  TauMass = 1.777;
  MuonMass = 0.1057;
  LambdaQCD = 160;//MeV
  MinQt2 = 0.64;
  Freeze = sqrt(MinQt2);
  B = 4.0;
  FNAL_or_LHC = 1;//Defaults to LHC
  Survive = 0.03;
  Rg = 1.2;
  root_s = 14000.0;
  PDF    = 20250;

  //Read in any card file passed as argument (optional)
  if(_ExhumeControlParams.size()==0){
    std::cout<<"\n  No card file specified - using defaults\n"<<
      std::endl;
  }
  else{
    ReadCard(_ExhumeControlParams);
  }

  /*

  if(argc ==1 ){
    std::cout<<"\n  No card file specified - using defaults\n"<<
      std::endl;
  }else if(argc == 2){
    ReadCard(argv[1]);
  }else if(argc>2){
    std::cout<<"  Please pass only the card file name as argument"<<
      std::endl<<std::endl<<
      "  ...Exiting"<<std::endl;
    exit(0);
  }
  */

  if(FNAL_or_LHC == 0){
    Survive = 0.045;
    Rg = 1.4;
    root_s = 1960;
  }else if(FNAL_or_LHC==1){
    Survive = 0.03;
    Rg = 1.2;
    root_s = 14000;
  }

  s = root_s * root_s;
  Invs = 1.0/s;

  gw = sqrt(4.0*PI*AlphaEw/
	     (1.0 - WMass*WMass/
	      (ZMass*ZMass)));

  LambdaW = 0.5 * gw / WMass;

  Gev2fb = 3.88 * pow(10.0,11);

  //Now write out all the parameters:

  for(std::map<std::string,PConstVoidPair>::iterator ii = PMap.begin();
      ii!=PMap.end();ii++){

    if(typeid(double*).name()==(ii->second).first){
      if(strlen((ii->first).c_str())<6){
	printf("  %s\t\t\t%17g\n",
	       (ii->first).c_str(),*(double*)((ii->second).second));
      }else{
	printf("  %s\t\t%17g\n",
	       (ii->first).c_str(),*(double*)((ii->second).second));
      }
    }
  }

  //...........................................................................
  //Initialise the PDFs

  std::cout<<std::endl<<
   " ........................................................................."
	   <<std::endl<<std::endl<<"  = Initialising PDFs ="<<std::endl;
  my_pdfset_(PDF);
  std::cout<<std::endl<<
   " ........................................................................."
	     <<std::endl;
  //...........................................................................

  //  double smass=0.0;//for now
  // my_pythia_init__();
  //pyinre_();
  Proton1Id = 2212;
  Proton2Id = (FNAL_or_LHC==0)?-2212:2212;
  /*
  pydat2_.pmas[0][24] = HiggsMass;
  pydat2_.pmas[0][22] = ZMass;
  pydat2_.pmas[0][23] = WMass;
  pydat2_.pmas[0][5]  = TopMass;
  pydat2_.pmas[0][12] = MuonMass;
  pydat2_.pmas[0][14] = TauMass;
  pydat2_.pmas[0][4]  = BottomMass;
  pydat2_.pmas[0][3]  = CharmMass;
  pydat2_.pmas[0][2]  = StrangeMass;
  pydat2_.pmas[0][1]  = 0.0;
  pydat2_.pmas[0][0]  = 0.0;
  */
  AlphaSInit();
  LumiInit();

}
/////////////////////////////////////////////////////////////////////////////
Exhume::CrossSection::~CrossSection(){

  free(LumSimpsFunc);
  free(_Qt2);
  free(_Qt);
  free(_KtLow);
  free(_KtHigh);
  free(_AlphaS);
  free(_CfAs_2PIRg);
  free(_NcAs_2PI);
  free(TFunc);

}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::AlphaS(const double &scale_){
  if(scale_>Freeze){
    return(ASConst / log(scale_/LambdaQCD));
  }else{
    return(ASFreeze);
  }
  //  return(0.0);
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Rg1Rg2(const double &scale_){

  double Rg1, Rg2;

  if(RgBegin){

    RgBegin = false;

    RgHigh[0] = RgMap2d.upper_bound(x1);
    RgHigh[1] = RgMap2d.upper_bound(x2);

    RgLow[0] = RgHigh[0];
    RgLow[0]--;
    RgLow[1] = RgHigh[1];
    RgLow[1]--;

    //std::cout<<"   Beginning Rg calculation"<<std::endl;

    if(fabs(RgLow[0]->first - RgHigh[0]->first) > 0.05 * x1 ||
       RgHigh[0]==RgMap2d.end()||
       RgHigh[0]==RgMap2d.begin()){
      std::map<double, double> Rg1Temp;
      RgMap2d.insert(std::pair<double, std::map<double, double> >
		     (x1, Rg1Temp) );
      RgHigh[0] = RgMap2d.upper_bound(0.9999*x1);
      RgInterpolate[0] = false;
    }

    if(fabs(RgLow[1]->first - RgHigh[1]->first) > 0.05 * x2 ||
       RgHigh[1]==RgMap2d.end() ||
       RgHigh[1]==RgMap2d.begin()){
      std::map<double, double> Rg2Temp;
      RgMap2d.insert(std::pair<double, std::map<double, double> >
		     (x2, Rg2Temp) );
      RgHigh[1] = RgMap2d.upper_bound(0.9999*x2);
      RgInterpolate[1] = false;
    }
    /*
    for(int i=0;i<2;i++){
      if(RgInterpolate[i])std::cout<<"   Interpolating Rg"<<std::endl;
    }
    */
  }

  if(!RgInterpolate[0]){
    Rg1 = Rg_(x1, scale_);
    (RgHigh[0]->second).insert(std::pair<double, double> (scale_, Rg1) );
  } else {
    double RgxHigh, RgxLow;

    std::map<double, double>::iterator high_, low_;
    high_ = (RgHigh[0]->second).upper_bound(scale_);
    bool CalculateRg = true;
    if(high_!=(RgHigh[0]->second).end()&&high_!=(RgHigh[0]->second).begin()){
      low_ = high_;
      low_--;
      if(high_->first - low_->first <0.05 * scale_) CalculateRg = false;
    }

    if(CalculateRg){
      RgxHigh = Rg_(RgHigh[0]->first, scale_);
      (RgHigh[0]->second).insert(std::pair<double, double> (scale_, RgxHigh) );
    }else{
      RgxHigh = low_->second +
	(scale_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    CalculateRg = true;

    high_ = (RgLow[0]->second).upper_bound(scale_);
    if(high_!=(RgLow[0]->second).end()&&high_!=(RgLow[0]->second).begin()){
      low_ = high_;
      low_--;
      if(high_->first - low_->first <0.05 * scale_) CalculateRg = false;
    }

    if(CalculateRg){
      RgxLow = Rg_(RgLow[0]->first, scale_);
      (RgLow[0]->second).insert(std::pair<double, double> (scale_, RgxLow) );
    }else{
      RgxLow = low_->second +
	(scale_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    Rg1 = RgxLow +
      (x1 - RgLow[0]->first) * (RgxHigh - RgxLow) /
      (RgHigh[0]->first - RgLow[0]->first);
  }


  if(!RgInterpolate[1]){
    Rg2 = Rg_(x2, scale_);
    (RgHigh[1]->second).insert(std::pair<double, double> (scale_, Rg2) );
  } else {
    double RgxHigh, RgxLow;

    std::map<double, double>::iterator high_, low_;
    high_ = (RgHigh[1]->second).upper_bound(scale_);

    bool CalculateRg = true;
    if(high_!=(RgHigh[1]->second).end()&&high_!=(RgHigh[1]->second).begin()){
      low_ = high_;
      low_--;
      if(high_->first - low_->first <0.05 * scale_) CalculateRg = false;
    }

    if(CalculateRg){
      RgxHigh = Rg_(RgHigh[1]->first, scale_);
      (RgHigh[1]->second).insert(std::pair<double, double> (scale_, RgxHigh) );
    }else{
      RgxHigh = low_->second +
	(scale_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    CalculateRg = true;
    high_ = (RgLow[1]->second).upper_bound(scale_);

    if(high_!=(RgLow[1]->second).end()&&high_!=(RgLow[1]->second).begin()){
      low_ = high_;
      low_--;
      if(high_->first - low_->first <0.05 * scale_) CalculateRg = false;
    }

    if(CalculateRg){
      RgxLow = Rg_(RgLow[1]->first, scale_);
      (RgLow[1]->second).insert(std::pair<double, double> (scale_, RgxLow) );
    }else{
      RgxLow = low_->second +
	(scale_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    Rg2 = RgxLow +
      (x2 - RgLow[1]->first) * (RgxHigh - RgxLow) /
      (RgHigh[1]->first - RgLow[1]->first);
  }

  return(Rg1 * Rg2);
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Rg_(const double &x_, double scale_){

  if(x_<0.002 && scale_ <1.5625) return(1.0);

  double upv, dnv, usea, dsea,
    str, chm, bot, top, gl1, gl2;

  double x1_, x2_;

  x1_ = 0.995 * x_;
  x2_ = 1.005 * x_;

  if(x2_>1.0){
    x1_ = 0.99 * x_;
    x2_ = x_;
  }

  structm_
    (&x1_, &scale_, &upv, &dnv, &usea, &dsea, &str, &chm, &bot, &top, &gl1);
  structm_
    (&x2_, &scale_, &upv, &dnv, &usea, &dsea, &str, &chm, &bot, &top, &gl2);

  double dlam = (gl1 - gl2) * 100.0 / gl1;
  if(dlam > 1.0) dlam = 1.0;
  //if(dlam>1.0) return(1.0);
  if(dlam < 0.0) dlam = 0.0;

  return(1.0 + dlam * (0.82 + 0.56 * dlam));
}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::
SetKinematics(const double &SqrtsHat_, const double &y_,
	      const double &t1_, const double &t2_,
	      const double &Phi1_, const double &Phi2_){

  RgBegin = true;
  RgInterpolate[0] = true;
  RgInterpolate[1] = true;

  TBegin = true;
  TInterpolate = true;

  SqrtsHat = SqrtsHat_;
  sHat = SqrtsHat * SqrtsHat;
  sHat2 = sHat * sHat;
  InvSqrtsHat = 1.0/SqrtsHat;
  InvsHat = InvSqrtsHat * InvSqrtsHat;
  InvsHat2 = InvsHat * InvsHat;
  t1 = t1_;
  t2 = t2_;
  Phi1 = Phi1_;
  Phi2 = Phi2_;
  PPhi = Phi1 - Phi2;
  y = y_;

  //Mju = 0.5 * SqrtsHat;
  Mju = 0.618 * SqrtsHat;
  Mju2 = Mju * Mju;
  LnMju2 = log(Mju2);

  Pt1 = sqrt(-t1);
  Pt2 = sqrt(-t2);

  Pt1DotPt2 = Pt1 * Pt2 * cos(PPhi);//This is the 3 vector between the protons

  x1x2 = sqrt((sHat - t1 - t2 + 2.0 * Pt1DotPt2) * Invs);
  Invsx1x2 = Invs / x1x2;

  ey = exp(y_);

  x1 = x1x2 * ey;
  x2 = x1x2 / ey;

  CLHEP::HepLorentzVector Glu1, Glu2;

  Glu1.setE(x1 * P1In.e());
  Glu2.setE(x2 * P2In.e());
  Glu1.setPz(x1 * P1In.pz());
  Glu2.setPz(x2 * P2In.pz());
  Glu1.setPx(-Pt1 * cos(Phi1));
  Glu2.setPx(-Pt2 * cos(Phi2));
  Glu1.setPy(-Pt1 * sin(Phi1));
  Glu2.setPy(-Pt2 * sin(Phi2));

  Proton1 = P1In - Glu1;
  Proton2 = P2In - Glu2;

  CentralVector = Glu1 + Glu2;

  double V12 = 1.0 - 4.0 * t1 * Invs / (x1 * x1);
  double V22 = 1.0 - 4.0 * t2 * Invs / (x2 * x2);

  double V1MinusV2 =
    sqrt(V12 + V22 + 2.0 - 8.0 * Pt1DotPt2 * Invsx1x2);

  InvV1MinusV2 = 1.0 / V1MinusV2;

  return;
}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::Hadronise(){
  /*
  int one = 1;
  int njoin = Partons.size();

  int *ijoin = (int*) calloc(njoin, sizeof(int) );

  double e_, theta_, phi_;
  int id, nn, nnc;

  double ps_scale;

  e_ = Proton1.e();
  theta_ = Proton1.theta();
  phi_ = Proton1.phi();
  nn =1;
  py1ent_(nn, Proton1Id, e_, theta_, phi_);



  e_ = Proton2.e();
  theta_ = Proton2.theta();
  phi_ = Proton2.phi();
  nn = 2;
  py1ent_(nn, Proton2Id, e_, theta_, phi_);

  double px_, py_, pz_, mm_;

  for(int i=0; i<njoin; i++){
    e_ = Partons[i].p.e();
    px_ = Partons[i].p.px();
    py_ = Partons[i].p.py();
    pz_ = Partons[i].p.pz();
    mm_ = Partons[i].p.m();

    nn = i+3;//Fortran indexing starts at 1 and have already
             //Inserted the 2 protons.
    nnc = nn - 1;//C++ array indexing;
    id = Partons[i].id;

    for( int j=0;j<5;j++){
      pyjets_.k[j][nnc]=0;
      pyjets_.p[j][nnc]=0.0;
      pyjets_.v[j][nnc]=0.0;
    }

    pyjets_.k[0][nnc]=1; //states there is an undecayed particle/parton
    pyjets_.k[1][nnc]=id;//sets the PDG type

    pyjets_.p[0][nnc]=px_;
    pyjets_.p[1][nnc]=py_;
    pyjets_.p[2][nnc]=pz_;
    pyjets_.p[3][nnc]=e_;
    pyjets_.p[4][nnc]=mm_;

    pyjets_.n = nn;

    ijoin[i] = nn;
  }
  int three=3;
  int four=4;
  //pyresd_(three);
  //pyresd_(four);
  //pyexec_();
  if(njoin > 1){
    if(Partons[0].id!=22) pyjoin_(njoin, ijoin);
    //pyresd_(three);//needed instead of pyshow for double resonance production
    //pyresd_(four);
    int ip1 =3;
    int ip2 =4;
    //this works because no 3-part final state...will be improved
    HepLorentzVector b_COM = Partons[0].p.boost(CentralVector.findBoostToCM());

    ps_scale = 2*b_COM.et(); //Q^2 = 4pt^2 for final state parton showering
    if(Partons[0].id!=22) pyshow_(ip1,ip2,ps_scale);
  }

  pyexec_();
  pyhepc_(one);
  */
  return;
}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::AlphaSInit(){

  ASConst = 12.0 * PI/(11*3 -2*5)/2.0;
  LambdaQCD = 0.001 * LambdaQCD;
  ASFreeze = ASConst / (log(Freeze/LambdaQCD));

  return;
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Fg1Fg2(const double &Qt2_,
				    const double &x1_, const double &x2_){

  double Kt = sqrt(Qt2_);
  double grad1, grad2;
  double KtLow, KtHigh, upv1, dnv1, upv2, dnv2, usea, dsea,
    str, chm, bot, top, glLow, glHigh, gl1, gl2;
  double AlphaS_ = AlphaS(Kt);

  KtLow = 0.9 * Kt;
  KtHigh = 1.1 * Kt;
  double x_ = x1_;
  //structm returns gl*x:

  structm_
    (&x_, &KtLow, &upv1,&dnv1,&usea,&dsea,&str,&chm,&bot,&top,&glLow);
  structm_
    (&x_, &KtHigh, &upv1,&dnv1,&usea,&dsea,&str,&chm,&bot,&top,&glHigh);

  grad1 = 2.5 * (glHigh - glLow);

  structm_
    (&x_, &Kt, &upv1, &dnv1, &usea, &dsea, &str, &chm, &bot, &top, &gl1);

  x_ = x2_;

  structm_
    (&x_, &KtLow, &upv2,&dnv2,&usea,&dsea,&str,&chm,&bot,&top,&glLow);
  structm_
    (&x_, &KtHigh, &upv2,&dnv2,&usea,&dsea,&str,&chm,&bot,&top,&glHigh);

  grad2 = 2.5 * (glHigh - glLow);

  structm_
    (&x_, &Kt, &upv2, &dnv2, &usea, &dsea, &str, &chm, &bot, &top, &gl2);

  //double SqrtT = T(Qt2_);
  double SqrtT = TFast(Qt2_);
  double dT =  Nc_2PI * AlphaS_ * log(Inv3 * Kt / (Kt + Mju) );
  //double CfAs_2PIRg = 0.0;//Cf_2PIRg * AlphaS_;

  double Rg1Rg2_ = Rg1Rg2(Kt);
  //double Rg1Rg2_ = Rg_(x1, Kt) * Rg_(x2, Kt);
  //double Rg1Rg2_ = 1.2*1.2;

  //dT = 0.0;

  //grad1 = 0.0;

  return(Rg1Rg2_ *
	 (SqrtT * (grad1 - dT * gl1) ) * // - CfAs_2PIRg * (upv1 + dnv1) ) *
	 (SqrtT * (grad2 - dT * gl2) ) );//- CfAs_2PIRg * (upv2 + dnv2) ) );
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Fg1Fg2(const int &ii_,
				    const double &x1_, const double &x2_){

  double grad1, grad2;
  double upv1, dnv1, upv2, dnv2, usea, dsea,
    str, chm, bot, top, glLow, glHigh, gl1, gl2;

  double x_ = x1_;
  //structm returns gl*x:

  structm_
    (&x_, &_KtLow[ii_], &upv1,&dnv1,&usea,&dsea,&str,&chm,&bot,&top,&glLow);
  structm_
    (&x_, &_KtHigh[ii_], &upv1,&dnv1,&usea,&dsea,&str,&chm,&bot,&top,&glHigh);

  grad1 = 2.5 * (glHigh - glLow);

  structm_
    (&x_, &_Qt[ii_], &upv1, &dnv1, &usea, &dsea, &str, &chm, &bot, &top, &gl1);

  x_ = x2_;

  structm_
    (&x_, &_KtLow[ii_], &upv2,&dnv2,&usea,&dsea,&str,&chm,&bot,&top,&glLow);
  structm_
    (&x_, &_KtHigh[ii_], &upv2,&dnv2,&usea,&dsea,&str,&chm,&bot,&top,&glHigh);

  grad2 = 2.5 * (glHigh - glLow);

  structm_
    (&x_, &_Qt[ii_], &upv2, &dnv2, &usea, &dsea, &str, &chm, &bot, &top, &gl2);

  //double SqrtT = T(_Qt2[ii_]);
  double SqrtT = TFast(_Qt2[ii_]);
  double dT = _NcAs_2PI[ii_] * log(Inv3 * _Qt[ii_] / (_Qt[ii_] + Mju) );

  double Rg1Rg2_ = Rg1Rg2(_Qt[ii_]);
  //double Rg1Rg2_ = Rg_(x1, _Qt[ii_]) * Rg_(x2, _Qt[ii_]);
  //double Rg1Rg2_ = 1.2*1.2;

  //dT = 0.0;

  //  grad1 = 0.0;

  return(Rg1Rg2_ *
	 (SqrtT * (grad1 - dT * gl1)) *// - _CfAs_2PIRg[ii_] * (upv1 + dnv1) ) *
	 (SqrtT * (grad2 - dT * gl2)));//- _CfAs_2PIRg[ii_] * (upv2 + dnv2)));
}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::LumiInit(){

  MidQt2 = 4.0;
  InvMidQt2 = 1.0/MidQt2;
  InvMidQt4 = InvMidQt2 * InvMidQt2;
  InvMaxQt2 = 0.0;
  LumNStart = 10;
  LumAccuracy = 100;
  LumNSimps = 51;
  LumNSimps_1 = LumNSimps - 1;
  TConst = -0.25/PI;

  Inv2PI = 0.5 / PI;
  Nc_2PI = 3.0 * Inv2PI;

  Inv3 = 1.0/3.0;

  Cf_2PIRg = 4.0 * Inv3 * Inv2PI / Rg;

  LumSimpsIncr = (InvMidQt2 - InvMaxQt2)/((double)LumNSimps);
  LumSimpsFunc = (double*) calloc(LumNSimps, sizeof(double));
  _Qt2 = (double*) calloc(LumNSimps, sizeof(double));
  _Qt = (double*) calloc(LumNSimps, sizeof(double));
  _KtLow = (double*) calloc(LumNSimps, sizeof(double));
  _KtHigh = (double*) calloc(LumNSimps, sizeof(double));
  _AlphaS = (double*) calloc(LumNSimps, sizeof(double));
  _CfAs_2PIRg = (double*) calloc(LumNSimps, sizeof(double));
  _NcAs_2PI = (double*) calloc(LumNSimps, sizeof(double));

  if(LumSimpsFunc==NULL||
     _Qt==NULL||
     _Qt2==NULL||
     _KtLow==NULL||
     _KtHigh==NULL||
     _AlphaS==NULL||
     _CfAs_2PIRg==NULL||
     _NcAs_2PI==NULL){
    NoMem();
  }

  //Can't calculate Qt at InvQt = 0.0
  //but function converges to 0 there:
  LumSimpsFunc[0] = 0.0;
  double InvQt2 = InvMaxQt2 + LumSimpsIncr;
  for(int i=1; i < LumNSimps; i++){
    _Qt2[i] = 1.0/InvQt2;
    _Qt[i] = sqrt(_Qt2[i]);
    _KtLow[i] = 0.9 * _Qt[i];
    _KtHigh[i] = 1.1 * _Qt[i];
    _AlphaS[i] = AlphaS(_Qt[i]);
    _CfAs_2PIRg[i] = Cf_2PIRg * _AlphaS[i];
    _NcAs_2PI[i] = Nc_2PI * _AlphaS[i];
    InvQt2 = InvQt2 + LumSimpsIncr;
  }

  //LumConst = 0.015625 * Survive * Rg * Rg * Rg * Rg * PI * PI;
  LumConst = 0.015625 * Survive * PI * PI;

  Tn = 81;
  Tn_1 = Tn - 1;
  TFunc = (double*) calloc(Tn, sizeof(double));

  if(TFunc == NULL){
    NoMem();
  }

  P1In.setE(0.5 * root_s);
  P2In.setE(0.5 * root_s);
  P1In.setPz(0.5 * root_s);
  P2In.setPz(-0.5 * root_s);
  P1In.setPx(0.0);
  P2In.setPx(0.0);
  P1In.setPy(0.0);
  P2In.setPy(0.0);

  std::map<double, double> TempMap;
  double QtMin = sqrt(MinQt2);
  TempMap.insert(std::pair<double, double> (QtMin, 1.0) );
  TempMap.insert(std::pair<double, double> (100.0, 1.0) );

  RgMap2d.insert(std::pair<double, std::map<double, double> > (0.0, TempMap) );

  TempMap.clear();
  TempMap.insert(std::pair<double, double>(QtMin, Rg_(0.999, QtMin) ) );
  TempMap.insert(std::pair<double, double>(100.0, Rg_(0.999, 100.0) ) );

  RgMap2d.insert(std::pair<double, std::map<double, double> >
		 (0.999, TempMap) );

  TempMap.clear();

  Mju = 10.0;
  Mju2 = Mju * Mju;
  LnMju2 = log(Mju2);

  TempMap.insert(std::pair<double, double> (MinQt2, T(MinQt2) ) );
  TempMap.insert(std::pair<double, double> (10000.0, T(10000.0) ) );

  TMap2d.insert(std::pair<double, std::map<double, double> >
		(Mju, TempMap) );

  TempMap.clear();

  Mju = 1000.0;
  Mju2 = Mju * Mju;
  LnMju2 = log(Mju2);

  TempMap.insert(std::pair<double, double> (MinQt2, T(MinQt2) ) );
  TempMap.insert(std::pair<double, double> (10000.0, T(10000.0) ) );

  TMap2d.insert(std::pair<double, std::map<double, double> >
		(Mju, TempMap) );

  TempMap.clear();

  return;

}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Lumi_(){

  double lum;
  fg2Map.clear();
  std::map<double, double> NextM, M;
  std::map<double, double>::iterator high, low, q1, q2;
  double LumIncr;

  double fg2_Qt4;
  double Qt2;

  //.....................................................................
  //Do the integral between MinQt and MidQt:

  for(Qt2 = MinQt2; Qt2 < 0.0000004; Qt2 += 0.0000001){
    fg2_Qt4 = Fg1Fg2(Qt2, x1, x2) / (Qt2 * Qt2);

    if(fg2_Qt4!=fg2_Qt4) fg2_Qt4 = 0.0;

    fg2Map.insert(fEntry(Qt2, fg2_Qt4) );

  }

  fg2_Qt4 = Fg1Fg2(MidQt2, x1, x2) * InvMidQt4;

  fg2Map.insert(fEntry(MidQt2, fg2_Qt4) );

  lum = 1.0;

  NextM.insert(fEntry(0.0, MinQt2));
  NextM.insert(fEntry(lum, MidQt2));

  unsigned int N = LumNStart;
  unsigned int NTot = LumNStart;

  while(NTot<LumAccuracy){

    N = N * 2;
    NTot = NTot + N;
    M = NextM;
    NextM.clear();
    NextM.insert(fEntry(0.0,MinQt2));
    LumIncr = lum/(N - 1.0);

    for(double q_ = LumIncr; q_ < lum; q_ += LumIncr){
      high = M.upper_bound(q_);
      if( high==M.end() ) high--;
      low = high;
      low--;

      Qt2 = low->second +
	(high->second - low->second) *
	(q_ - low->first)/(high->first - low->first);

      fg2_Qt4 = Fg1Fg2(Qt2, x1, x2) / (Qt2 * Qt2);
      fg2Map.insert(fEntry(Qt2, fg2_Qt4) );
    }

    q1 = fg2Map.begin();
    q2 = q1;

    for(q2++;q2 != fg2Map.end(); q2++){
      NextM.insert
	(fEntry
	 (NextM.rbegin()->first +
	  0.5 * (q2->first - q1->first)*(q2->second + q1->second),
	  q2->first)
	 );
      q1 = q2;

    }
    lum = NextM.rbegin()->first;
  }
  //......................................................................

  //**********************************************************************
  //Now do the rest of the integral between MidQt and MaxQt = 1.0/InvMaxQt
  //Transform the integration variable to 1/Qt so \inf -> 0
  //Have already entered the first element - it is always 0.

  for(int i=1; i < LumNSimps; i++){
    //LumSimpsFunc[i] = Fg1Fg2(_Qt2[i], x1, x2);
    LumSimpsFunc[i] = Fg1Fg2(i, x1, x2);

  }
  lum = lum + dsimps_(LumSimpsFunc, &InvMaxQt2, &InvMidQt2,&LumNSimps_1);

  //**********************************************************************

  return(LumConst * lum * lum);

}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Lumi(){
  //Will modify this to create lookup table for Lumi on the fly - later
  //For now just return Lumi_

  return(Lumi_());
}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::NoMem(){
  std::cout<<"   Your computer cannot allocate enough memory."<<std::endl
<<"   Try replacing it with one that wouldn't be more useful as a doorstop."
	     <<std::endl
	     <<"   ...Exiting"<<std::endl;

    exit(0);

    return;

}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Splitting(const double &Kt){

  double D = Mju/(Mju + Kt);
  double D2 = D * D;
  double D3 = D2 * D;

  unsigned int Nf;
  if(Kt < CharmMass){
    Nf = 3;
  }else if(Kt < BottomMass){
    Nf = 4;
  }else{
    Nf = 5;
  }

  return(
	 6.0*(-D2 + Inv3 * D3 - 0.25 *D3 * D - log(1 - D)) +
	 Nf * (0.5 * (D - D2) + Inv3 * D3)
	 );

}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::TFast(const double &Qt2_){

  if(TBegin){
    TBegin = false;

    TMjuHigh = TMap2d.upper_bound(Mju);
    TMjuLow = TMjuHigh;
    TMjuLow--;

    if(TMjuHigh->first - TMjuLow->first > 0.01 * Mju ||
       TMjuHigh==TMap2d.end() ||
       TMjuHigh==TMap2d.begin()){
      std::map<double, double> TTemp;

      TMap2d.insert(std::pair<double, std::map<double, double> >
		    (Mju, TTemp) );
      TMjuHigh = TMap2d.upper_bound(0.9999*Mju);
      TInterpolate = false;
      //std::cout<<"Got here"<<std::endl;
    }
  }

  if(!TInterpolate){
    //std::cout<<"Not interpolating T"<<std::endl;
    double Tee = T(Qt2_);
    (TMjuHigh->second).insert(std::pair<double, double> (Qt2_, Tee) );
    return(Tee);
  }

  if(TInterpolate){

    //std::cout<<"Interpolating T"<<std::endl;

    double THigh, TLow;
    std::map<double, double>::iterator high_, low_;
    high_ = (TMjuHigh->second).upper_bound(Qt2_);
    bool CalculateT = true;

    if(high_!=(TMjuHigh->second).end() &&
       high_!=(TMjuHigh->second).begin()){

      low_ = high_;
      low_--;
      if(high_->first - low_->first < 0.01 * Qt2_) CalculateT = false;
    }

    if(CalculateT){
      double MjuOld = Mju;
      double Mju2Old = Mju2;
      double LnMju2Old = LnMju2;
      //This is a bit dirty, don't let anyone else screw with this
      //'cause I know what I'm doing:
      Mju = TMjuHigh->first;
      Mju2 = Mju * Mju;
      LnMju2 = log(Mju2);
      THigh = T(Qt2_);
      Mju = MjuOld;
      Mju2 = Mju2Old;
      LnMju2 = LnMju2Old;
      (TMjuHigh->second).insert(std::pair<double, double> (Qt2_, THigh) );
    }else{
      THigh = low_->second +
	(Qt2_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    CalculateT = true;
    high_ = (TMjuLow->second).upper_bound(Qt2_);

    if(high_!=(TMjuLow->second).end() &&
       high_!=(TMjuLow->second).begin()){
      low_ = high_;
      low_--;
      if(high_->first - low_->first < 0.01 * Qt2_) CalculateT = false;
    }

    if(CalculateT){
      double MjuOld = Mju;
      double Mju2Old = Mju2;
      double LnMju2Old = LnMju2;
      //This is a bit dirty, don't let anyone else screw with this
      //'cause I know what I'm doing:
      Mju = TMjuLow->first;
      Mju2 = Mju * Mju;
      LnMju2 = log(Mju2);
      TLow = T(Qt2_);
      Mju = MjuOld;
      Mju2 = Mju2Old;
      LnMju2 = LnMju2Old;
      (TMjuLow->second).insert(std::pair<double, double> (Qt2_, TLow) );
    }else{
      TLow = low_->second +
	(Qt2_ - low_->first) * (high_->second - low_->second) /
	(high_->first - low_->first);
    }

    return(TLow + (Mju - TMjuLow->first) * (THigh - TLow) /
	   (TMjuHigh->first - TMjuLow->first) );
  }

  return(0.0);
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::T(const double &Qt2_){

  double Kt;
  double LnQt2  = log(Qt2_);
  double TIncr  = (LnMju2 - LnQt2)/(Tn - 1.0);
  double TLnKt2 = LnQt2;

  for(int i=0; i<Tn; i++){
    Kt = sqrt(exp(TLnKt2));
    TFunc[i] =  Splitting(Kt)*AlphaS(Kt);
    TLnKt2 = TLnKt2 + TIncr;
  }
  //N.B. This is really the square root of T.

  if(Qt2_ > Mju2){
    return(exp(-TConst * dsimps_(TFunc, &LnQt2, &LnMju2, &Tn_1)));
  }
  return(exp(TConst * dsimps_(TFunc, &LnQt2, &LnMju2, &Tn_1)));
}
/////////////////////////////////////////////////////////////////////////////
double Exhume::CrossSection::Txg(const double &Qt2_, const double &x_){

  double x__ = x_;
  double Qt = sqrt(Qt2_);
  double upv, dnv, usea, dsea, str, chm, bot, top, gl;
  structm_(&x__, &Qt, &upv,&dnv,&usea,&dsea,&str,&chm,&bot,&top,&gl);

  //gl is really gl * x so don't need to multiply by x
  return(T(Qt2_) * gl);

}
/////////////////////////////////////////////////////////////////////////////
void Exhume::CrossSection::ReadCard(const std::string &filename){

  FILE * card;
  card = fopen(filename.c_str(),"r");
  if(card==NULL){
    std::cout<<std::endl<<"  File "<<filename<<" does not exist"<<
      std::endl<<std::endl<<"\t...Exiting"<<std::endl<<std::endl;
    exit(0);
  }else{
    std::cout<<std::endl<<"  Opening file "<<filename<<"..."<<
      std::endl<<std::endl;
  }

  char text[50];
  int read, read2;
  std::pair<const char*,const void*> value;
  std::map<std::string,std::pair<const char*,const void*> >
    ::iterator val_;
  int LinesRead = 0;


  while(!feof(card)){
    read = fscanf(card,"%s",text);
    if(read==1){
      val_ = PMap.find(text);

      if(val_!=PMap.end()){
	value = val_->second;
	LinesRead++;
	if(LinesRead==1){
	  std::cout<<"  Reading values for:";
	  printf("%20s\n",text);
	}else{
	  printf("\t\t\t%17s\n",text);
	}
	read2 = fscanf(card,TypeMap.find(value.first)->second,value.second);
        if (read2!=1) std::cout <<"RadCard error " << std::endl;
      }
    }
  }

  if(LinesRead==0) std::cout<<"  WARNING: No correct parameters found in "<<
    filename<<std::endl<<std::endl<<
    "  No values read in   --   using defaults"<<std::endl;

  std::cout<<std::endl<<"  ...Closing file "<<filename<<std::endl<<std::endl;

  fclose(card);

}
/////////////////////////////////////////////////////////////////////////////
std::complex<double> Exhume::CrossSection::Fsf(const double &Tau_){
    double InvTau = 1.0/Tau_;
    return(InvTau*(1.0+(1.0-InvTau)*f(Tau_)));
}
//////////////////////////////////////////////////////////////////////////////
std::complex<double> Exhume::CrossSection::F0(const double &Tau_){
    double InvTau = 1.0/Tau_;
    return(InvTau*(-1.0 + InvTau*f(Tau_)));
}
//////////////////////////////////////////////////////////////////////////////
std::complex<double> Exhume::CrossSection::f(const double &Tau_){
  std::complex<double> Sqrtf;
  std::complex<double> f;

    if(Tau_<=1.0){
        Sqrtf = asin(sqrt(Tau_));
        f = Sqrtf*Sqrtf;
    }
    if(Tau_>1.0){
      std::complex<double> SqrtTau = sqrt(Tau_),
            SqrtTau_1 = sqrt(Tau_-1.0);
        Sqrtf = log((SqrtTau + SqrtTau_1)/
                    (SqrtTau - SqrtTau_1)) - I*PI;
        f = -0.25 * Sqrtf * Sqrtf;
    }
    return(f);
}
//////////////////////////////////////////////////////////////////////////////

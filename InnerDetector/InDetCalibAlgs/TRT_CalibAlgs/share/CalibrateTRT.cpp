/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include <TNtuple.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TROOT.h>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

int Simple1dHist(float min, float max, int nbins, float value)
{
  if ( (value<min) or (value>max) ) return -1;
  int binno=(nbins*((value-min)/(max-min)));
  return binno;
}

int Simple2dHist(float minx, float maxx, int nbinsx, float miny, float maxy, int nbinsy, float valuex, float valuey)
{
  if ( (valuex<minx) or (valuex>maxx) or  (valuey<miny) or (valuey>maxy) ) return -1;
  int binnox=(nbinsx*((valuex-minx)/(maxx-minx)));
  int binnoy=(nbinsy*((valuey-miny)/(maxy-miny)));
  return binnoy*nbinsx+binnox;
}
//cppcheck-suppress ctuOneDefinitionRuleViolation
struct databundle{
  int det{}, lay{}, mod{}, brd{}, chp{}, stl{}, stw{}, sid{}, ievt{};
  float tres{}, weight{}, res{}, t{}, r{}, t0{}, x{}, y{}, z{}, rt0{};
};
//cppcheck-suppress ctuOneDefinitionRuleViolation
struct caldata{
  int det{}, lay{}, mod{}, brd{}, chp{}, stl{}, stw{};
  int sid{}, nres{}, minntres{}, maxntres{}, minnres{}, maxnres{}, minnrt{}, maxnrt{}, t0fittype{};
  float ntres{},nrt{},res{},reserr{},tres{},t0{},oldt0{},reft0{},t0err{},t0off{},rtt0{};
  float rtpar[4]{},nhits{},treshist[100]{},reshist[100]{},rthist[640]{},x{},y{},z{};
  bool calflag{},rtflag{},t0flag{};
};

struct epdata{
  float run{},event{},track{},nhits{},ephase1{},ephase2{},t0{},traw{};
};

class Calibrator{
public:
  Calibrator(int,string,int,int,string);
  ~Calibrator();
  int AddHit(string,databundle);
  TDirectory* Calibrate(TDirectory*, string, string, caldata);
  float FitRt(string,string,TH2F*);
  float FitTimeResidual(string,TH1F*);
  float FitResidual(string,TH1F*);
  void WriteStat(TDirectory*);
  map<string,caldata> data;
private:
  TH1F* resHist{};
  string name{};
  int level{},minrtstat{},mint0stat{},nbinsr{20},nbinst{32},nbinstres{},nbinsres{100};
  float minr{0.},maxr{2.},mint{-10},maxt{80},mintres{-25},maxtres{25},minres{-1},maxres{1};
  bool isdines{};
};


Calibrator::Calibrator(int lev, const string & nme, int mint0, int minrt, const string & rtr):
    name(nme), level(lev), minrtstat(minrt), mint0stat(mint0){
  isdines = rtr.find("dines")!=string::npos;

}

Calibrator::~Calibrator(){

}

float Calibrator::FitRt(const string & key, const string & opt, TH2F* rtHist){
  
  float mintime=999.0;
  float mindistance = 0.0; //0.1 
  int npoints=rtHist->GetNbinsY();
  char chname[100]{}, chtit[100]{};
  float tbinsize=3.125, width=2.5*tbinsize, mean;
  int maxbin{};
  float maxval{};
  float rtpars[4]{};

  TGraphErrors rtgr(npoints);
  TGraphErrors trgr(npoints);
  TF1 dtfitfunc("dtfitfunc","gaus(0)");

  //create an r-t graph by fitting gaussians to r-slizes for all r-bins

  for (int i=0;i<npoints;i++) {    
    
    sprintf(chname,"r-slize_%i",i); 
    sprintf(chtit,"bin %i : %4.2f < d < %4.2f  [mm]",i,rtHist->GetYaxis()->GetBinLowEdge(i+1),rtHist->GetYaxis()->GetBinUpEdge(i+1)); 
    TH1D* hslize = rtHist->ProjectionX(chname,i+1,i+1);
    hslize->SetTitle(chtit);
    hslize->SetDirectory(0); //do not write it to the file
    
    maxbin=1; maxval=0;
    for (int j=1;j<=hslize->GetNbinsX()+1;j++) {
      float val=hslize->GetBinContent(j);
      if (val>maxval){
 	      maxval=val;
 	      maxbin=j;
      }
    }
    
    mean=hslize->GetBinCenter(maxbin);
    dtfitfunc.SetRange(mean-width,mean+width);
    dtfitfunc.SetParameter(0,hslize->GetMaximum()); 
    dtfitfunc.SetParameter(1,mean); 
    dtfitfunc.SetParameter(2,5);
    
    hslize->Fit(&dtfitfunc,"QRI");
        
    float d = rtHist->GetYaxis()->GetBinCenter(i+1);
    float t = dtfitfunc.GetParameter(1);
    rtgr.SetPoint(i,t,d);
    rtgr.SetPointError(i,dtfitfunc.GetParError(1),rtHist->GetYaxis()->GetBinWidth(1)/sqrt(12.0)); 
    trgr.SetPoint(i,d,t);
    trgr.SetPointError(i,0,dtfitfunc.GetParError(1));
    if(d>=mindistance && t<mintime) mintime = t;
  }	
  rtgr.SetMarkerStyle(1) ;
  rtgr.SetMarkerColor(2) ;
  rtgr.SetLineColor(2) ;
  rtgr.SetName("rtgr") ;
  trgr.SetMarkerStyle(1) ;
  trgr.SetMarkerColor(2) ;
  trgr.SetLineColor(2) ;
  trgr.SetName("trgraph") ;
  

  gStyle->SetOptFit(1111);
  gStyle->SetPalette(1);

  string trrelation, rtrelation;
  if (isdines){
    trrelation = "[1] - abs([2]*[0]) + [2] * sqrt([0]*[0] + x*x)";
    rtrelation = "sqrt( ( (x-([1]-abs([2]*[0])))*(x-([1]-abs([2]*[0])))/([2]*[2]) ) - [0]*[0])";
  }
  else {
    trrelation="[0]+x*([1]+(x*([2]+x*[3])))";
    rtrelation="[0]+x*([1]+(x*([2]+x*[3])))";
  }

  // t-r relation
  TF1 trfunc2("trfunc2",trrelation.data(),0,3);
  if (isdines) trfunc2.SetParameters(0.21, 3.57, 17.0);
  else trfunc2.SetParameters(0.0, 0.0, 0.0);
  trfunc2.SetRange(0,2);
  if (opt.find('3')==string::npos) trfunc2.FixParameter(3,0);
  trfunc2.SetLineColor(4) ;
  int result3 = trgr.Fit(&trfunc2,"QR"); // always fit the t-r relation
  trgr.SetTitle(trrelation.data());
  if (opt.find('Q')==string::npos) trgr.Write();
  rtpars[0] = trfunc2.GetParameter(0);
  rtpars[1] = trfunc2.GetParameter(1);
  rtpars[2] = trfunc2.GetParameter(2);
  rtpars[3] = trfunc2.GetParameter(3);

  // r-t relation
  TF1 rtfunc2("rtfunc2",rtrelation.data(),mintime,200);
  rtfunc2.SetRange(-0.1,60);
  if (opt.find('3')==string::npos) rtfunc2.FixParameter(3,0);
  rtfunc2.SetLineColor(4) ;
  if (isdines) rtfunc2.SetParameters(trfunc2.GetParameter(0),trfunc2.GetParameter(1),trfunc2.GetParameter(2),trfunc2.GetParameter(3));
  else {
    rtfunc2.SetParameters(0.0, 0.1, -0.002, 3e-05);
    int result2 = rtgr.Fit(&rtfunc2,"QR"); // only fit the r-t relation if not Dines'
    rtpars[0] = rtfunc2.GetParameter(0);
    rtpars[1] = rtfunc2.GetParameter(1);
    rtpars[2] = rtfunc2.GetParameter(2);
    rtpars[3] = rtfunc2.GetParameter(3);
  }

  if (opt.find('0')==string::npos) {
    if (isdines) rtpars[1]=0;
    else rtpars[0]=0;
  }

  //ǵet the t0 from the tr relation
  float tdrift = 20;
  if (isdines) tdrift = trfunc2.GetParameter(1); 
  else         tdrift = trfunc2.GetParameter(0); 
    
 
  data[key].rtpar[0]=rtpars[0];
  data[key].rtpar[1]=rtpars[1];
  data[key].rtpar[2]=rtpars[2];
  data[key].rtpar[3]=rtpars[3];

  rtHist->GetListOfFunctions()->Add(rtfunc2.Clone());
  if (!isdines) rtHist->GetListOfFunctions()->Add(rtgr.Clone());

  return tdrift;
}


float Calibrator::FitTimeResidual(string key, TH1F* tresHist){
  
  float mean = tresHist->GetMean();
  float rms = tresHist->GetRMS();
  float val,maxval=0;
  int maxbin=1;
  for (int j=1; j<=tresHist->GetNbinsX()+1;j++){
    val=tresHist->GetBinContent(j);
    if (val>maxval){
      maxval=val;
      maxbin=j;
    }
  }  

  float fitmean=tresHist->GetBinCenter(maxbin);  
  if (fabs(fitmean)>5.0){ 
    data[key].t0    = mean;
    data[key].t0err = rms;
    data[key].t0fittype = 1;
    return mean;
  }

  float fitrms=rms;
  if (fitrms>5.0) fitrms=5.0;
  
  tresHist->Fit("gaus","QRI","",fitmean-fitrms,fitmean+fitrms);

  data[key].tres = tresHist->GetFunction("gaus")->GetParameter(2);
  data[key].t0err = tresHist->GetFunction("gaus")->GetParError(1);
  data[key].t0fittype = 2;

  gStyle->SetOptFit(1111);

  return tresHist->GetFunction("gaus")->GetParameter(1);
}


float Calibrator::FitResidual(string key, TH1F* resHist){

  float mean = resHist->GetMean();
  float rms = resHist->GetRMS();

  resHist->Fit("gaus","QRI","",mean-0.3,mean+0.3);

  gStyle->SetOptFit(1111);

  data[key].res=resHist->GetFunction("gaus")->GetParameter(2);
  data[key].reserr=resHist->GetFunction("gaus")->GetParError(2);

  return resHist->GetFunction("gaus")->GetParameter(2);

}


TDirectory* Calibrator::Calibrate(TDirectory* dir, string key, string opt, caldata caldata_above){

  //set some bool flags
  bool calrt=opt.find('R')!=string::npos;
  bool calt0=opt.find('T')!=string::npos;
  bool donothing=opt.find('N')!=string::npos;
  bool godeeper=opt.find('D')!=string::npos;
  bool isquiet=opt.find('Q')!=string::npos;
  bool prnt=opt.find('P')!=string::npos;
  bool useref=opt.find('B')!=string::npos;

  if (donothing) return dir;

  data[key].calflag=true; 

  bool enough_t0 = (int)data[key].ntres>=mint0stat;
  bool enough_rt = (int)data[key].nrt>=minrtstat;

  TDirectory* newdir;
  if ((enough_rt && calrt) | (enough_t0 && calt0)) {   
    newdir = dir->mkdir(Form("%s%s",name.data(),key.data()));
    newdir->cd();
  }
  else newdir=dir;
      
  //Fit also the residual if an rt or t0 calibration is made
  if ((int)data[key].nres>50 && ((enough_rt && calrt) | (enough_t0 && calt0))) {    

    resHist = new TH1F("residual","residual",nbinsres,minres,maxres);
    for (int i=0;i<100;i++) {
      resHist->SetBinContent(i+1,data[key].reshist[i]);
    }
    resHist->SetEntries((int)data[key].nres);
    if (isquiet) resHist->SetDirectory(0);                                 
    
    FitResidual(key,resHist);

  }
  
  if (prnt) printf("%8s %-13s: ",name.data(),key.data()); 

  //Calibrate r-t
  if (enough_rt && calrt) {

    data[key].rtflag=true;
    //make a root histogram
    TH2F* rtHist = new TH2F("rt-relation","rt-relation",nbinst,mint,maxt,nbinsr,minr,maxr);
    for (int i=0;i<nbinst;i++) {
      for (int j=0;j<nbinsr;j++) {
	rtHist->SetBinContent(i+1,j+1,data[key].rthist[i+nbinst*j]);
      }
    }
    rtHist->SetEntries((int)data[key].nrt);
    if (isquiet) {
      rtHist->SetDirectory(0);
    }

    data[key].rtt0=FitRt(key,opt,rtHist); //do the fit
    if (prnt) printf("RT    %7i %8.1e %8.1e %8.1e %8.1e  : ", (int)data[key].nrt, data[key].rtpar[0], data[key].rtpar[1], data[key].rtpar[2], data[key]. rtpar[3]); 

  }
  else{
    //use data from level above
    data[key].rtt0=caldata_above.rtt0;
    for (int i=0;i<4;i++) data[key].rtpar[i]=caldata_above.rtpar[i];
    if (prnt) printf("RT /\\ %7i %8.1e %8.1e %8.1e %8.1e  : ", (int)data[key].nrt,data[key].rtpar[0],data[key].rtpar[1],data[key].rtpar[2],data[key].rtpar[3]); 
  } 

  //Calibrate t0
  if (useref && level==5){ 
    data[key].t0flag=true;
    data[key].t0=caldata_above.t0+data[key].reft0;
    data[key].t0err=caldata_above.t0err;
    data[key].t0off=data[key].t0-caldata_above.t0;
    data[key].t0fittype = 3;
    if (prnt) printf("T0 ** %7i  %3.2f%+3.2f=%3.2f", (int)data[key].ntres, caldata_above.t0, data[key].reft0, data[key].t0); 
  }
  else {
    if (enough_t0 && calt0){ 
      
      data[key].t0flag=true;
      //make a root histogram
      TH1F* tresHist = new TH1F("timeresidual","time residual",nbinstres,mintres,maxtres);
      for (int i=0;i<100;i++) {
	tresHist->SetBinContent(i+1,data[key].treshist[i]);
      }
      tresHist->SetEntries((int)data[key].ntres);
      if (isquiet) {
	tresHist->SetDirectory(0);
      }
      
      data[key].t0=data[key].oldt0 + FitTimeResidual(key,tresHist); //do the fit and modify t0
      if (data[key].lay==0 && data[key].stl<9) data[key].t0+=0.8; //short straw compensation
      data[key].t0off=data[key].t0-caldata_above.t0; //calculate t0 offset from level above
      if (data[key].t0<0) data[key].t0=0;
      if (prnt) printf("T0    %7i  %3.2f%+3.2f=%3.2f", (int)data[key].ntres, data[key].oldt0, data[key].t0-data[key].oldt0, data[key].t0); 

    }
    else {     
    //use data from level above
      data[key].t0=caldata_above.t0;
      data[key].t0err=caldata_above.t0err;
      data[key].t0off=data[key].t0-caldata_above.t0;
      data[key].t0fittype = 4;
      if (prnt) printf("T0 /\\ %7i  %3.2f%+3.2f=%3.2f", (int)data[key].ntres, caldata_above.t0, 0.0, data[key].t0); 
    }
  }

  if (prnt) cout << endl;

  return newdir;

}

int Calibrator::AddHit(string key, databundle d){
  
  int tresbin=Simple1dHist(mintres,maxtres,nbinstres,d.tres);
  int resbin=Simple1dHist(minres,maxres,nbinsres,d.res);
  int rtbin=Simple2dHist(mint,maxt,nbinst,minr,maxr,nbinsr,d.t,d.r);
  
  //if the first one
  if (data.find(key) == data.end()){

    caldata* hist=(caldata*)malloc(sizeof(caldata));

    if (not hist){
      cout << "OUT OF MEMORY!" << endl;
      return -1;
    }

    for (int i=0;i<640;i++) {
      hist->rthist[i]=0;
      if (i<100) {
	hist->treshist[i]=0;
	hist->reshist[i]=0;
      }
    }
    hist->treshist[tresbin]=d.weight;
    hist->reshist[resbin]=1;
    hist->rthist[rtbin]=1;
    if (level>0) hist->det=d.det; else hist->det=-3;
    if (level>1) hist->lay=d.lay; else hist->lay=-3;
    if (level>2) hist->mod=d.mod; else hist->mod=-3;
    if (level>3) hist->brd=d.brd; else hist->brd=-3;
    if (level>4) hist->chp=d.chp; else hist->chp=-3;
    if (level>5) hist->stw=d.stw; else hist->stw=-3;
    if (level>2) hist->stl=d.stl; else hist->stl=-3;
    if (level>5) hist->sid=d.sid; else hist->sid=-3;
    hist->res=0;
    hist->reserr=0;
    hist->tres=0;
    hist->t0=0;
    if (level==5) hist->reft0=d.rt0; else hist->reft0=0;
    hist->t0off=0;
    hist->t0err=0;
    hist->oldt0=d.t0;
    hist->ntres=1;
    hist->nres=1;
    hist->nrt=1;
    hist->nhits=1;
    hist->t0fittype=0;
    
    hist->x=d.x;
    hist->y=d.y;
    hist->z=d.z;

    hist->rtflag=false;
    hist->t0flag=false;
    hist->calflag=false;

    data[key]=*hist;

    //printf("%30s %f first\n",key.data(),hist.nhits);

  }
  else {

    //increment histogram bins
    if (tresbin>0){ 
      data[key].treshist[tresbin]=data[key].treshist[tresbin]+d.weight;
      data[key].ntres++;
    }
    if (resbin>0){ 
      data[key].reshist[resbin]++;
      data[key].nres++;
    }
    if (rtbin>0){ 
      data[key].rthist[rtbin]++;
      data[key].nrt++;
    }
    
    //update old t0
    data[key].oldt0 = data[key].oldt0*((data[key].nhits-1)/data[key].nhits)+d.t0/data[key].nhits;

    data[key].x = data[key].x*((data[key].nhits-1)/data[key].nhits)+d.x/data[key].nhits;
    data[key].y = data[key].y*((data[key].nhits-1)/data[key].nhits)+d.y/data[key].nhits;
    data[key].z = data[key].z*((data[key].nhits-1)/data[key].nhits)+d.z/data[key].nhits;

    //increment hit counts
    data[key].nhits++;
  }    

  return 0;
}

void Calibrator::WriteStat(TDirectory* dir){
  
  dir->cd();

  TNtuple* stattup = new TNtuple(Form("%stuple",name.data()),"statistics","det:lay:mod:brd:chp:rtflag:t0flag:t0:oldt0:rt0:dt0:t0offset:ftype:nhits:nt0:nrt:res:dres:tres:x:y:z");
  for(map<string,caldata>::iterator ihist=data.begin(); ihist!=data.end(); ++ihist){
    if ((ihist->second).calflag) {
      float const ntvar[22]={
	(ihist->second).det,
	(ihist->second).lay,
	(ihist->second).mod,
	(ihist->second).brd,
	(ihist->second).chp,
	(int)(ihist->second).rtflag,	
	(int)(ihist->second).t0flag,	
	(ihist->second).t0,
	(ihist->second).oldt0,
	(ihist->second).reft0,
	(ihist->second).t0err,
	(ihist->second).t0off,
	(ihist->second).t0fittype,
	(ihist->second).nhits,
	(ihist->second).ntres,
	(ihist->second).nrt,
	(ihist->second).res,
	(ihist->second).reserr,
	(ihist->second).tres,
	(ihist->second).x,
	(ihist->second).y,
	(ihist->second).z
      };      
      stattup->Fill(ntvar);
    }
      
  }

  stattup->Write();
  stattup->Delete();
}


//dictionary 
class zero{public: int z;};
class BDstraw{public: map<string,zero> s;};
class BDchip{public: map<string,BDstraw> c;};
class BDboard{public: map<string,BDchip> b;};
class BDmodule{public: map<string,BDboard> m;};
class BDlayer{public: map<string,BDmodule> l;};
class BDdetector{public: map<string,BDlayer> d;};
class BDTRT{public: map<string,BDdetector> t;};
BDTRT trt; 

string TkeyBD,DkeyBD,LkeyBD,MkeyBD,BkeyBD,CkeyBD,SkeyBD;


bool IncludedLevels(string opt0, int* levinc){

  string popts[7]{};

  string opt1=opt0.substr(0,opt0.rfind('_'));
  string opt2=opt1.substr(0,opt1.rfind('_'));
  string opt3=opt2.substr(0,opt2.rfind('_'));
  string opt4=opt3.substr(0,opt3.rfind('_'));
  string opt5=opt4.substr(0,opt4.rfind('_'));
  string opt6=opt5.substr(0,opt5.rfind('_'));

  popts[6]=opt0.replace(0,opt1.size(),"");
  popts[5]=opt1.replace(0,opt2.size(),"");
  popts[4]=opt2.replace(0,opt3.size(),"");
  popts[3]=opt3.replace(0,opt4.size(),"");
  popts[2]=opt4.replace(0,opt5.size(),"");
  popts[1]=opt5.replace(0,opt6.size(),"");
  popts[0]=opt6;

  bool accept=true;
  int var;
  for (int i=0;i<7;i++){    
    string slevinc = popts[i].substr(1,popts[i].size());
    if (slevinc.find('*')!=string::npos || slevinc.find(',')!=string::npos || slevinc.find('-')!=string::npos) levinc[i]=-3;
    else {
      if(EOF == sscanf( (popts[i].substr(1,popts[i].size())).data() , "%d", &var));
      levinc[i]=var;
    }
  }
  
  return accept;
}


string SubLev(string pat, int lev){  
  
  string sublev;

  for (int i=0;i<lev;i++){
    pat.replace(pat.find('_'),1,"");
    sublev = pat.substr(0,pat.find('_'));
    pat.replace(0,pat.find('_'),"");
  }
  return sublev;
}


bool IsSubLev(const string &key, int lev, const string &sublev){  
  
  string sl=sublev;

  if (sl.compare(SubLev(key,lev))==0) return true;

   while(sl.compare(sl.substr(sl.find(',')+1))!=0){
     if (SubLev(key,lev).compare(sl.substr(0,sl.find(',')))==0) return true;
     sl=sl.substr(sl.find(',')+1);
     if (SubLev(key,lev).compare(sl.substr(0,sl.find(',')))==0) return true;
   }
  return false;

}

void MakeBDKeys(int bec, int low, int mod, int brd, int chp, int strawid){
  //make map keys for the board-chip view

  char ckey[100];
  sprintf(ckey,"_%i_%i_%i_%i_%i_%i",bec,low,mod,brd,chp,strawid); SkeyBD=string(ckey);
  sprintf(ckey,"_%i_%i_%i_%i_%i",bec,low,mod,brd,chp); CkeyBD=string(ckey);
  sprintf(ckey,"_%i_%i_%i_%i",bec,low,mod,brd); BkeyBD=string(ckey);
  sprintf(ckey,"_%i_%i_%i",bec,low,mod); MkeyBD=string(ckey);
  sprintf(ckey,"_%i_%i",bec,low); LkeyBD=string(ckey);
  sprintf(ckey,"_%i",bec); DkeyBD=string(ckey);
  sprintf(ckey,"_all"); TkeyBD=string(ckey);
}

int main(int argc, char *argv[]){

  int minrt=0,mint0=0,nevents=-1;

  string selection=string(argv[1]);
  string options=string(argv[2]);
  sscanf(argv[3],"%i",&minrt);
  sscanf(argv[4],"%i",&mint0);
  sscanf(argv[7],"%i",&nevents);

  string outfile="calibout";
  string rtrel=argv[5]{};
  string infile=argv[6]{};

  cout << endl;
  cout << "INPUT FILE       : " << infile << endl; 
  cout << "OUTPUT FILE      : " << outfile << endl; 
  cout << "SELECTION STRING : " << selection << endl; 
  cout << "OPTION STRING    : " << options << endl; 
  cout << "MIN STATISTICS   : RT=" << minrt << ", T0=" << mint0 << endl; 
  cout << endl;

  databundle d;

  Calibrator TRT(0,"TRT",mint0,minrt,rtrel);
  Calibrator Detector(1,"Detector",mint0,minrt,rtrel);
  Calibrator Layer(2,"Layer",mint0,minrt,rtrel);
  Calibrator Module(3,"Module",mint0,minrt,rtrel);
  Calibrator Board(4,"Board",mint0,minrt,rtrel);
  Calibrator Chip(5,"Chip",mint0,minrt,rtrel);
  Calibrator Straw(6,"Straw",mint0,minrt,rtrel);
  
  float run{},evt{},trk{},det{},lay{},mod{},stl{},stw{};
  float brd{},chp{},sid{},locx{},locy{},locz{};
  float x{},y{},z{},r{},dr{},t{},rtrack{},ttrack{},t0{},ephase{};

  int rbrd{}, rchp{}, rdet{}, dum{};
  float rt0{};
  char rkey[10]{};
  map<string,float> reft0map;
  ifstream t0ref("finedelays.txt",ios::in);
  for(int iref=0;iref<208;iref++){
    t0ref >> rbrd >> rchp >> rdet >> dum >> rt0;
    reft0map[string(Form("_%i_%i_%i",rdet,rbrd,rchp))]=rt0;
  }
  t0ref.close();

  int levinc[10];
  IncludedLevels(selection,levinc);

  bool isdines = rtrel.find("dines")!=string::npos;
  int rtint{};
  if (isdines) rtint=2;
  else rtint=0;

  TFile* ntfile=new TFile(infile.data());
  if (!(ntfile->IsZombie())){
    TNtuple* hittuple=(TNtuple*)ntfile->Get("ntuple");
    
    
    hittuple->SetBranchAddress("run",&run);
    hittuple->SetBranchAddress("evt",&evt);
    hittuple->SetBranchAddress("trk",&trk);
    hittuple->SetBranchAddress("det",&det);
    hittuple->SetBranchAddress("lay",&lay);
    hittuple->SetBranchAddress("mod",&mod);
    hittuple->SetBranchAddress("stl",&stl);
    hittuple->SetBranchAddress("stw",&stw);
    hittuple->SetBranchAddress("brd",&brd);
    hittuple->SetBranchAddress("chp",&chp);
    hittuple->SetBranchAddress("sid",&sid);
    hittuple->SetBranchAddress("locx",&locx);
    hittuple->SetBranchAddress("locy",&locy);
    hittuple->SetBranchAddress("locz",&locz);
    hittuple->SetBranchAddress("x",&x);
    hittuple->SetBranchAddress("y",&y);
    hittuple->SetBranchAddress("z",&z);
    hittuple->SetBranchAddress("r",&r);
    hittuple->SetBranchAddress("dr",&dr);
    hittuple->SetBranchAddress("t",&t);
    hittuple->SetBranchAddress("rtrack",&rtrack);
    hittuple->SetBranchAddress("ttrack",&ttrack);
    hittuple->SetBranchAddress("t0",&t0);
    hittuple->SetBranchAddress("ephase",&ephase);
      
    int ievt{};

    map<string,epdata> ephasemap{};
    //map<string,epdata> ephasemap2;

    if (nevents==-1) nevents = hittuple->GetEntries();

    //calculate event phase;
    cout << "CALCULATING TRACK DATA!" << endl;
    for (ievt=0;ievt<nevents;ievt++){

      if (ievt%100000==99999) cout << ievt+1 << " HITS READ! " << endl; 

      hittuple->GetEntry(ievt);

      string epkey=string(Form("_%i_%i_%i",(int)run,(int)evt,(int)trk));
      
      if (ephasemap.find(epkey) == ephasemap.end()){
        ephasemap[epkey].run=run;
        ephasemap[epkey].event=evt;
        ephasemap[epkey].track=trk;
        ephasemap[epkey].ephase1=(t+ephase)-t0-ttrack;
        ephasemap[epkey].ephase2=ephase;
        ephasemap[epkey].nhits=1.0;
        ephasemap[epkey].t0=t0;
        ephasemap[epkey].traw=t;
      }
      else {
        ephasemap[epkey].ephase1+=(t+ephase)-t0-ttrack;
        ephasemap[epkey].t0+=t0;
        ephasemap[epkey].traw+=t;
        ephasemap[epkey].nhits++;
      }
    }


    TFile* hfile = new TFile("histograms.root","RECREATE");
    map<string,TH1F*> T0Histmap{};
    map<string,TH2F*> RtHistmap{};

    //main loop
    cout << "MAIN LOOP!" << endl;
    for (ievt=0;ievt<nevents;ievt++){

      if (ievt%100000==99999) cout << ievt+1 << " HITS READ! " << endl; 

      hittuple->GetEntry(ievt);

      if (((int)det==levinc[1] || levinc[1]==-3) &&
	  ((int)lay==levinc[2] || levinc[2]==-3) &&
	  ((int)mod==levinc[3] || levinc[3]==-3) &&
	  ((int)brd==levinc[4] || levinc[4]==-3) &&
	  ((int)chp==levinc[5] || levinc[5]==-3) &&
	  ((int)sid==levinc[6] || levinc[6]==-3)){ 
	
	//Build map keys for Board-Chip view
	MakeBDKeys((int)det,(int)lay,(int)mod,(int)brd,(int)chp,(int)sid);
	
	//Make the sub level hierachy dictionary
	trt.t[TkeyBD].d[DkeyBD].l[LkeyBD].m[MkeyBD].b[BkeyBD].c[CkeyBD].s[SkeyBD].z=0;
	
	d.det=(int)det; 
	d.lay=(int)lay; 
	d.mod=(int)mod; 
	d.brd=(int)brd; 
	d.chp=(int)chp; 
	d.stl=(int)stl; 
	d.stw=(int)stw; 
	d.sid=(int)sid; 
	d.ievt=ievt; 
	d.tres=t-ttrack-t0; 
	//d.weight=1/dr;
        d.weight=1;
	d.res=r-rtrack; 
	d.t=t-t0; 
	d.r=fabs(rtrack);
	d.t0=t0;
	d.rt0=reft0map[string(Form("_%i_%i_%i",d.det,d.brd,d.chp))];
	d.x=x;
	d.y=y;
	d.z=z;
	
 	TRT.AddHit(TkeyBD,d);
 	Detector.AddHit(DkeyBD,d);
 	Layer.AddHit(LkeyBD,d);
 	Module.AddHit(MkeyBD,d);    
 	Board.AddHit(BkeyBD,d);    
 	Chip.AddHit(CkeyBD,d);    
 	Straw.AddHit(SkeyBD,d);   
	
	if (T0Histmap.find(CkeyBD) == T0Histmap.end()) T0Histmap[CkeyBD]= new TH1F(("T0" + CkeyBD).data(),("T0" + CkeyBD).data(),100,-25,25);
	else T0Histmap[CkeyBD]->Fill(t-ttrack-t0,1/dr);

	if (RtHistmap.find(CkeyBD) == RtHistmap.end()) RtHistmap[CkeyBD]= new TH2F(("RT" + CkeyBD).data(),("RT" + CkeyBD).data(),32,-10,80,20,0,2);
	else RtHistmap[CkeyBD]->Fill(t-t0,fabs(rtrack));

      }
    }    

    hfile->Write();
    hfile->Close();
    
    //for (map<string,caldata>::iterator it = TRT.data.begin(); it != TRT.data.end(); it++) cout << it->first << " " << (it->second).entries << endl;
    //for (map<string,caldata>::itergoogle mapsator id = Detector.data.begin(); id != Detector.data.end(); id++) cout << id->first << " " << (id->second).entries << endl;
    
    TFile* file = new TFile(Form("%s%s",outfile.data(),".root"),"RECREATE");
    ofstream rtcalfile(Form("%s_rt.txt",outfile.data()),ios::out);
    ofstream t0calfile(Form("%s_t0.txt",outfile.data()),ios::out);

    caldata startdata;
    startdata.t0=20.0;

    for (map<string,BDdetector>::iterator it = trt.t.begin(); it != trt.t.end(); ++it){
      TDirectory* dir1 = TRT.Calibrate(file,it->first,SubLev(options,1),startdata);
      if (SubLev(options,1).find('F')!=string::npos && TRT.data[it->first].t0flag) t0calfile << Form("-3 -1 -1 -1 -1 : %e %e",TRT.data[it->first].t0,TRT.data[it->first].t0err) << endl;
      if (SubLev(options,1).find('F')!=string::npos && TRT.data[it->first].rtflag) rtcalfile << Form("-3 -1 -1 -1 -1 : %i %e %e %e %e",rtint,TRT.data[it->first].rtpar[0],TRT.data[it->first].rtpar[1],TRT.data[it->first].rtpar[2],TRT.data[it->first].rtpar[3]) << endl;
      
      for (map<string,BDlayer>::iterator id = (it->second.d).begin() ; id != (it->second.d).end(); ++id){
	if(SubLev(selection,2).compare("-")==0) {
	  // create tracktuple only once (on top level)
	  file->cd();
	  TNtuple* tracktup = new TNtuple("tracktuple","track data","run:evt:track:ep1:ep2:nhits:t0:traw");
	  for (map<string,epdata>::iterator iep = ephasemap.begin(); iep != ephasemap.end(); ++iep){
	    tracktup->Fill(iep->second.run, iep->second.event, iep->second.track,  iep->second.ephase1/iep->second.nhits, iep->second.ephase2, iep->second.nhits, iep->second.t0/iep->second.nhits, iep->second.traw/iep->second.nhits);
	  }
	  break; //only calibrate to this level
	}
	if(SubLev(selection,2).compare("*")!=0 && !IsSubLev(id->first,1,SubLev(selection,2))) continue; //skip calibration if not the right id, if * go ahead!
	TDirectory* dir2 = Detector.Calibrate(dir1,id->first,SubLev(options,2),TRT.data[it->first]);
	if (SubLev(options,2).find('F')!=string::npos && Detector.data[id->first].t0flag) t0calfile << Form("%i -1 -1 -1 -1 : %e %e",Detector.data[id->first].det,Detector.data[id->first].t0,Detector.data[id->first].t0err) << endl;
	if (SubLev(options,2).find('F')!=string::npos && Detector.data[id->first].rtflag) rtcalfile << Form("%i -1 -1 -1 -1 : %i %e %e %e %e",Detector.data[id->first].det,rtint,Detector.data[id->first].rtpar[0],Detector.data[id->first].rtpar[1],Detector.data[id->first].rtpar[2],Detector.data[id->first].rtpar[3]) << endl;
	
	for (map<string,BDmodule>::iterator il = (id->second.l).begin(); il != (id->second.l).end(); ++il){
	  if(SubLev(selection,3).compare("-")==0) break;
	  if(SubLev(selection,3).compare("*")!=0 && !IsSubLev(il->first,2,SubLev(selection,3))) continue;
	  TDirectory* dir3 = Layer.Calibrate(dir2,il->first,SubLev(options,3),Detector.data[id->first]);
	  if (SubLev(options,3).find('F')!=string::npos && Layer.data[il->first].t0flag) t0calfile << Form("%i %i -1 -1 -1 : %e %e",Layer.data[il->first].det,Layer.data[il->first].lay,Layer.data[il->first].t0,Layer.data[il->first].t0err) << endl;
	  if (SubLev(options,3).find('F')!=string::npos && Layer.data[il->first].rtflag) rtcalfile << Form("%i %i -1 -1 -1 : %i %e %e %e %e",Layer.data[il->first].det,Layer.data[il->first].lay,rtint,Layer.data[il->first].rtpar[0],Layer.data[il->first].rtpar[1],Layer.data[il->first].rtpar[2],Layer.data[il->first].rtpar[3]) << endl;
	  
	  for (map<string,BDboard>::iterator im = (il->second.m).begin(); im != (il->second.m).end(); ++im){
	    if(SubLev(selection,4).compare("-")==0) break;
	    if(SubLev(selection,4).compare("*")!=0 && !IsSubLev(im->first,3,SubLev(selection,4))) continue;
	    TDirectory* dir4 = Module.Calibrate(dir3,im->first,SubLev(options,4),Layer.data[il->first]);
	    if (SubLev(options,4).find('F')!=string::npos && Module.data[im->first].t0flag) t0calfile << Form("%i %i %i -1 -1 : %e %e",Module.data[im->first].det,Module.data[im->first].lay,Module.data[im->first].mod,Module.data[im->first].t0,Module.data[im->first].t0err) << endl;
	    if (SubLev(options,4).find('F')!=string::npos && Module.data[im->first].rtflag) rtcalfile << Form("%i %i %i -1 -1 : %i %e %e %e %e",Module.data[im->first].det,Module.data[im->first].lay,Module.data[im->first].mod,rtint,Module.data[im->first].rtpar[0],Module.data[im->first].rtpar[1],Module.data[im->first].rtpar[2],Module.data[im->first].rtpar[3]) << endl;
	    
 	    for (map<string,BDchip>::iterator ib = (im->second.b).begin(); ib != (im->second.b).end(); ++ib){
	      if(SubLev(selection,5).compare("-")==0) break;
	      if(SubLev(selection,5).compare("*")!=0 && !IsSubLev(ib->first,4,SubLev(selection,5))) continue;
	      TDirectory* dir5 = Board.Calibrate(dir4,ib->first,SubLev(options,5),Module.data[im->first]);
	      
 	      for (map<string,BDstraw>::iterator ic = (ib->second.c).begin(); ic != (ib->second.c).end(); ++ic){
		if(SubLev(selection,6).compare("-")==0) break;
		if(SubLev(selection,6).compare("*")!=0 && !IsSubLev(ic->first,5,SubLev(selection,6))) continue;

		if ((ic->first).size()<50) {
		  
		  TDirectory* dir6 = Chip.Calibrate(dir5,ic->first,SubLev(options,6),Board.data[ib->first]);
		  
		  for (map<string,zero>::iterator is = (ic->second.s).begin(); is != (ic->second.s).end(); ++is){
		    if(SubLev(selection,7).compare("-")==0) break;
		    if(SubLev(selection,7).compare("*")!=0 && !IsSubLev(is->first,6,SubLev(selection,7))) continue;
		    TDirectory* dir7 = Straw.Calibrate(dir6,is->first,SubLev(options,7),Chip.data[ic->first]);
		    if (SubLev(options,7).find('F')!=string::npos && (Chip.data[ic->first].t0flag || Board.data[ib->first].t0flag)) {
		      t0calfile << Form("%i %i %i %i %i : %e %e",Straw.data[is->first].det,Straw.data[is->first].lay,Straw.data[is->first].mod,Straw.data[is->first].stl,Straw.data[is->first].stw,Straw.data[is->first].t0,Straw.data[is->first].t0err) << endl;
		    }
		    if (SubLev(options,7).find('F')!=string::npos && (Chip.data[ic->first].rtflag || Board.data[ib->first].rtflag)) 
		      rtcalfile << Form("%i %i %i %i %i : %i %e %e %e %e",Straw.data[is->first].det,Straw.data[is->first].lay,Straw.data[is->first].mod,Straw.data[is->first].stl,Straw.data[is->first].stw,rtint,Straw.data[is->first].rtpar[0],Straw.data[is->first].rtpar[1],Straw.data[is->first].rtpar[2],Straw.data[is->first].rtpar[3]) << endl;
		    
		  }
		  
		}
		else cout << "BAD KEY .. SKIPPING CHIP!" << endl;

 	      }
 	    }
	  }	  
	}
      }      
    }

    
    if ((SubLev(options,1).find('F')!=string::npos)) TRT.WriteStat(file);
    if ((SubLev(options,2).find('F')!=string::npos)) Detector.WriteStat(file);
    if ((SubLev(options,3).find('F')!=string::npos)) Layer.WriteStat(file);
    if ((SubLev(options,4).find('F')!=string::npos)) Module.WriteStat(file);
    if ((SubLev(options,5).find('F')!=string::npos)) Board.WriteStat(file);
    if ((SubLev(options,6).find('F')!=string::npos)) Chip.WriteStat(file);
    if ((SubLev(options,7).find('F')!=string::npos)) Straw.WriteStat(file);

    file->Write();
    file->Close();
    file->Delete();

    rtcalfile.close();
    t0calfile.close();

    exit(0);
    //return 0;

  }
}

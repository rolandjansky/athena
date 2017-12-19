/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

   NAME: Calibrator.cxx    
PACKAGE: TRTCalibTools 

AUTHORS: Johan Lundquist  
CREATED: 27-03-2009 

PURPOSE: Class for calibrating a TRT sub-level
          
********************************************************************/

#include "Calibrator.h"

caldata::caldata(){
  res = -999; 
  resMean = -999;
  reserr = -999;
  tres = -999;
  tresMean = -999;
  t0 = -999;
  t0err = -999;
  reft0  = -999; 
  t0off  = -999; 
  rtt0  = -999; 
  nhits = -999;
  x  = -999;
  y  = -999;
  z  = -999;
  oldt02  = -999;
  sumt0  = -999;
  sumx  = -999;
  sumy  = -999;
  sumz  = -999;
  calflag  = -999;
  rtflag  = -999;
  t0flag  = -999;
  m_treshist  = nullptr;
  reshist  = nullptr;
  rthist  = nullptr;
  det = -999;
  lay = -999;
  mod = -999;
  brd = -999;
  chp = -999;
  stl = -999;
  stw = -999;
  sid = -999;
  ntres = -999;
  nrt = -999;
  nres = -999;
  t0fittype = -999;
  rtgraph = nullptr;
}

caldata::~caldata(){
  delete[] m_treshist;
  delete[] reshist;
  delete[] rthist;

}

caldata::caldata(bool makehist, int nbinst, int nbinsr){
  res = -999; 
  resMean = -999;
  reserr = -999;
  tres = -999;
  tresMean = -999;
  t0 = -999;
  t0err = -999;
  reft0  = -999; 
  t0off  = -999; 
  rtt0  = -999; 
  nhits = -999;
  x  = -999;
  y  = -999;
  z  = -999;
  oldt02  = -999;
  sumt0  = -999;
  sumx  = -999;
  sumy  = -999;
  sumz  = -999;
  calflag  = -999;
  rtflag  = -999;
  t0flag  = -999;
  m_treshist  = nullptr;
  reshist  = nullptr;
  rthist  = nullptr;
  det = -999;
  lay = -999;
  mod = -999;
  brd = -999;
  chp = -999;
  stl = -999;
  stw = -999;
  sid = -999;
  ntres = -999;
  nrt = -999;
  nres = -999;
  t0fittype= -999;
  rtgraph = nullptr;

  if (makehist) {
    if (m_treshist) delete m_treshist;
    m_treshist = new float[100];
  }
  if (makehist) {
    if (reshist) delete reshist;
    reshist = new float[100];
  }
  if (makehist) {
    if (rthist) delete rthist;
    rthist = new float[nbinsr*nbinst+200];
  }
}

RtGraph::RtGraph(TH2F* rtHist, int binvar, const char* binlabel, bool pflag, TDirectory* dir){

  int fitresult;
  //bool t_binning = binvar==0;
  //bool r_binning = binvar==1;

  npoints = binvar==0 ? rtHist->GetNbinsX() : rtHist->GetNbinsY() ;


  m_mean = -20.0; 
  m_t = -20.0;
  m_d = -20.0;
  m_et = -20.0;
  m_ed = -20.0;
  m_ff =  nullptr ;

  
  hslizes = new TH1D*[npoints];
  m_btype = new bintype[npoints];
  m_tv = new float[npoints];
  m_dv = new float[npoints];
  m_etv = new float[npoints];
  m_edv = new float[npoints];
  m_rightsig = new float[npoints];
  m_leftsig = new float[npoints];
  m_leftval = new float[npoints];
  m_rightval = new float[npoints];
  m_maxbin = new int[npoints];
  m_maxval = new float[npoints];
  
  m_ipoint=0;
  mintime=999;
  m_mindistance=0;
  
  if (pflag){
    TDirectory* binhistdir = dir->mkdir("binhist");
    binhistdir->cd();
  }

  // check and classify the bin-histograms
  for (int i=0;i<npoints;i++) {
    
    m_chname = string(Form("%s slize_%i",binlabel,i));
    m_chtit = binvar==0 ? string(Form("bin %i : %4.2f < %s < %4.2f  [ns]",i,rtHist->GetXaxis()->GetBinLowEdge(i+1),binlabel,rtHist->GetXaxis()->GetBinUpEdge(i+1))) :
                        string(Form("bin %i : %4.2f < %s < %4.2f  [mm]",i,rtHist->GetYaxis()->GetBinLowEdge(i+1),binlabel,rtHist->GetYaxis()->GetBinUpEdge(i+1))) ;
    hslizes[i] = binvar==0 ? rtHist->ProjectionY(m_chname.data(),i+1,i+1) :
                             rtHist->ProjectionX(m_chname.data(),i+1,i+1);
    hslizes[i]->SetTitle(m_chtit.data());
    if (!pflag){
      hslizes[i]->SetDirectory(0);
    }
    
    m_maxbin[i]=1; m_maxval[i]=0;
    for (int j=1;j<=hslizes[i]->GetNbinsX();j++) {
      float val=hslizes[i]->GetBinContent(j);
      if (val>m_maxval[i]){
        m_maxval[i]=val;
        m_maxbin[i]=j;
        m_leftval[i] = hslizes[i]->GetBinContent(j-1);
        m_rightval[i] = hslizes[i]->GetBinContent(j+1);
      }
    }
    
    //m_rightsig[i] = m_rightval[i]==0 || m_maxbin[i]==20 ? 100 : sqrt(m_maxval[i])/m_maxval[i] + sqrt(m_rightval[i])/m_rightval[i];
    m_rightsig[i] = m_rightval[i]==0 ? 100 : sqrt(m_maxval[i])/m_maxval[i] + sqrt(m_rightval[i])/m_rightval[i];
    m_leftsig[i]  = m_leftval[i]==0 ? 100 : sqrt(m_maxval[i])/m_maxval[i] + sqrt(m_leftval[i])/m_leftval[i];
    
    m_btype[i]=EMPTY;
    //if (m_rightsig[i]<0.1 && m_leftsig[i]<0.1){
    //if (m_rightsig[i]<0.5 && m_leftsig[i]<0.5){
    if(true){
      m_btype[i]=GOOD;
      m_btype[i-1]=GOOD;
    } 
    else{
      if (hslizes[i]->GetEntries()>0){
        if (i<(float)hslizes[i]->GetNbinsX()/2)  m_btype[i]=LOW;
        else m_btype[i]=HIGH;
      }
    }
    printf("%s ... %8f %8i %4i %8f %8f %4i\n", m_chtit.data(), (float)hslizes[i]->GetEntries()/(float)rtHist->GetEntries(), (int)hslizes[i]->GetEntries(), m_maxbin[i], m_leftsig[i], m_rightsig[i], m_btype[i]);
  }
  
  float frmin=0,frmax=0;
  for (int i=0;i<npoints;i++) {
    
    // prepare initial fit parameters   
    m_mean=hslizes[i]->GetBinCenter(m_maxbin[i]);
    if (m_btype[i]==LOW) {frmin = hslizes[i]->GetBinCenter(1);        frmax = hslizes[i]->GetBinCenter(m_maxbin[i]+4);}
    if (m_btype[i]==GOOD) {frmin = hslizes[i]->GetBinCenter(m_maxbin[i]-3); frmax = hslizes[i]->GetBinCenter(m_maxbin[i]+3);}
    if (m_btype[i]==HIGH) {frmin = hslizes[i]->GetBinCenter(m_maxbin[i]-3); frmax = hslizes[i]->GetBinCenter(m_maxbin[i]+4);}
    
    m_ff = new TF1("dtfitfunc","gaus");
    
    m_ff->SetRange(frmin,frmax);
    
    //     m_ff->SetParameter(0,hslizes[i]->GetMaximum()); 
    //     m_ff->SetParLimits(0,0.5*hslizes[i]->GetMaximum(),1000000);
    //     m_ff->SetParameter(1,hslizes[i]->GetBinCenter(m_maxbin[i])); 
    //     m_ff->SetParLimits(1,frmin,frmax);
    //     m_ff->SetParameter(2,5);
    //     m_ff->SetParLimits(2,-0.5,0.5);
    
    //cout << i << " " << m_maxbin[i] << " " << m_maxval[i] << " " << m_mean-width << " " << m_mean+width << " " << m_mean << endl;
    
    m_t = binvar==0 ? rtHist->GetXaxis()->GetBinCenter(i+1) : rtHist->GetYaxis()->GetBinCenter(i+1);
    m_et = binvar==0 ? rtHist->GetXaxis()->GetBinWidth(1)/sqrt(12.0) : rtHist->GetYaxis()->GetBinWidth(1)/sqrt(12.0);
    m_d = 0;
    m_ed = 0;
    
    if (m_btype[i]==LOW){
      m_d =  0;
      m_ed = 0;
    }
    if (m_btype[i]==HIGH){
      m_d =  2;
      m_ed = 0;
    }
    if (m_btype[i]==EMPTY){
      m_d =  0;
      m_ed = 0;
    }
    if (m_btype[i]==GOOD){
      if (m_btype[i-1]==GOOD){
        fitresult=hslizes[i]->Fit("dtfitfunc","QR");
        m_ff->SetRange(m_mean-1.0*m_ff->GetParameter(2), m_mean+1.0*m_ff->GetParameter(2));
        fitresult=hslizes[i]->Fit("dtfitfunc","QR");
        m_d =  m_ff->GetParameter(1);
        m_ed = m_ff->GetParError(1);
        cout << fitresult << " " << m_ed << endl;
      }
      else{
        m_d =  0;
        m_ed = 0;
      }
    }

    //cout << i << " " << m_ipoint << " " << m_t << " " << m_d << " " << m_et << " " << m_ed << " " << m_btype[i] << endl;

    //if (m_btype[i]==GOOD && m_d>=0.0 && m_d<=2.0){
    if (m_btype[i]==GOOD && m_ed!=0){
      m_tv[m_ipoint]=m_t;
      m_dv[m_ipoint]=m_d;
      m_etv[m_ipoint]=m_et;
      m_edv[m_ipoint]=m_ed;
      m_ipoint++;
      if (binvar==0) {
        rval.push_back(m_d);
        tval.push_back(m_t);
      }
      else {
        rval.push_back(m_t);
        tval.push_back(m_d);
      }
    }

    
    if(m_d>=m_mindistance && m_t<mintime) mintime = m_t; 
  }  

  rtgr = binvar==0 ? new TGraphErrors(m_ipoint,m_tv,m_dv,m_etv,m_edv) :  new TGraphErrors(m_ipoint,m_dv,m_tv,m_edv,m_etv);
  rtgr->SetTitle((string("binning in ") + binlabel).data());
  trgr = binvar==0 ? new TGraphErrors(m_ipoint,m_dv,m_tv,m_edv,m_etv) :  new TGraphErrors(m_ipoint,m_tv,m_dv,m_etv,m_edv);
  trgr->SetTitle((string("binning in ") + binlabel).data());

  rtgr->SetMarkerStyle(1) ;
  rtgr->SetMarkerColor(2) ;
  rtgr->SetLineColor(2) ;
  rtgr->SetName("rtgraph") ;
  trgr->SetMarkerStyle(1) ;
  trgr->SetMarkerColor(2) ;
  trgr->SetLineColor(2) ;
  trgr->SetName("trgraph") ;

  dir->cd();




}


RtGraph::~RtGraph(){  

 delete hslizes   ;
 delete m_btype     ;
 delete m_tv        ;
 delete m_dv        ;
 delete m_etv       ;
 delete m_edv       ;
 delete m_rightsig  ;
 delete m_leftsig   ;
 delete m_leftval   ;
 delete m_rightval  ;
 delete m_maxbin    ;
 delete m_maxval    ;
}


double pol3deg(double *x, double *par) {
  double r = x[0];
  double t = par[0]+r*(par[1]+(r*(par[2]+r*par[3])));
  return t;
}

double pol3deg2(double *x, double *par) {
  double t = x[0];
  double r = par[1]*(t-par[0]) + par[2]*(t-par[0])*(t-par[0]) + par[3]*(t-par[0])*(t-par[0])*(t-par[0]);
  return r;
}

/*
double pol3_dines(double *x, double *par) {
  double m_t = x[0];
  double rp = par[0]+m_t*(par[1]+(m_t*(par[2]+m_t*par[3])));
  return rp;
}

double pol3simple_dines(double *x, double *par) {
  double m_t = x[0];
  double tp = m_t-par[1];// then tp = par[2]*rp+par[3]*rp*rp*rp 
  double b = par[3]/(par[2]*par[2]*par[2]*par[2]);
  double rp = tp/par[2]-b*tp*tp*tp;// an assumption that holds within +-1ns
  return rp;
}
*/

double trrel_dines(double *x, double *par){
  double rmin0 = par[0];
  double rho = par[1];
  double v = par[2];
  double t_const = par[3];
  double r = x[0];
  double t = t_const + 2*rho/v*asin(sqrt(rmin0*rmin0*(1-0.25*r*r)+r*r)/(2*rho));
  return t;
}

double rtrel_dines(double *x, double*par){
  double rmin0 = par[0];
  double rho = par[1];
  double v = par[2];
  double t_const = par[3];
  double t = x[0];
  double r_squared = (4*rho*rho*sin(v*(t-t_const)/(2*rho))-rmin0*rmin0)/(1-0.25*rmin0*rmin0);
  double r=r_squared>0 ? sqrt(r_squared) : 0.0;
  return r;
}

/*
double trrel_dines(double *x, double *par) {
  double r = x[0];
  double rp = sqrt(r*r+par[0]*par[0]);
  double m_t = par[1]+rp*par[2]+rp*rp*par[3];
  return m_t;
}

double rtrel_dines(double *x, double *par) {
  double m_t=x[0];

  double a = par[1]-par[2]*fabs(par[0]);
  double b = par[2]*par[0]*par[2]*par[0];
  double c = par[2]*par[2];
  double m_d = par[3];

  double A = m_d*m_d;
  double B = -c-2*(m_t-a)*m_d;
  double C = (m_t-a)*(m_t-a)-b;
  
  double r;
 
  if (A>0){    
    if ((-B-sqrt(B*B-4*A*C))/(2*A)>0) r = sqrt((-B-sqrt(B*B-4*A*C))/(2*A));
    else r = 0;
  }
  else{
    if (C/fabs(B)>0) r = sqrt(C/fabs(B));
    else r = 0;
  }
  //if (C/fabs(B)-C*C*A*A/fabs(B*B*B)>0) r = sqrt(C/fabs(B) - C*C*A*A/fabs(B*B*B));
  //if (C/fabs(B)-C*C*A*A/fabs(B*B*B)+C*C*C*A*A*A/(2*fabs(B*B*B*B*B))>0) r = sqrt(C/fabs(B) - C*C*A*A/fabs(B*B*B) + C*C*C*A*A*A/(2*fabs(B*B*B*B*B)));
  
  return r;
}
*/

Calibrator::Calibrator(){
  level =-10;
  m_name ="None"; 
  m_rtbinning ="None";
  m_minrtstat =-10;
  m_mint0stat =-10;
  m_t0shift =-100.;
  m_mint=-5;
  m_maxt=50;
  m_nbinst=55;

  m_minr=0;
  m_maxr=2;
  m_nbinsr=100;

  m_mintres=-10;
  m_maxtres=10;
  m_nbinstres=100;

  m_minres=-0.6;
  m_maxres=0.6;
  m_nbinsres=100;

  m_isdines = false;
  dort=false;
  dot0=false;
  dores=false;
  nort=false;
  not0=false;
  usebref=false;
  bequiet=false;
  printlog=false;
  printt0=false;
  printrt=false;
  usep0=false;
  floatp3=false;
  selection.insert(-3);
  useshortstraws=true;
  m_ntreshits=0;
  m_nreshits=0;
  m_nrthits=0;
  m_nhits=0;





}

Calibrator::Calibrator(int lev, string nme, int mint0, int minrt, string rtr, string rtb, float t0sft){
  level=lev;
  m_name=nme; 
  m_rtbinning=rtb;
  m_minrtstat=minrt;
  m_mint0stat=mint0;
  m_t0shift=t0sft;

  m_mint=-5;
  m_maxt=50;
  m_nbinst=55;

  m_minr=0;
  m_maxr=2;
  m_nbinsr=100;

  m_mintres=-10;
  m_maxtres=10;
  m_nbinstres=100;

  m_minres=-0.6;
  m_maxres=0.6;
  m_nbinsres=100;

  m_isdines = rtr.find("dines")!=string::npos;
  dort=false;
  dot0=false;
  dores=false;
  nort=false;
  not0=false;
  usebref=false;
  bequiet=false;
  printlog=false;
  printt0=false;
  printrt=false;
  usep0=false;
  floatp3=false;
  selection.insert(-3);
  useshortstraws=true;
  m_ntreshits=0;
  m_nreshits=0;
  m_nrthits=0;
  m_nhits=0;
}

Calibrator::~Calibrator(){
}

int Calibrator::Simple1dHist(float min, float max, int nbins, float value){
  if ( (value<min) || (value>max) ) return -1;
  int binno=(int)(nbins*((value-min)/(max-min)));
  return binno;
}

int Calibrator::Simple2dHist(float minx, float maxx, int nbinsx, float miny, float maxy, int nbinsy, float valuex, float valuey){
  if ( (valuex<minx) || (valuex>maxx) ||  (valuey<miny) || (valuey>maxy) ) return -1;
  int binnox=(int)(nbinsx*((valuex-minx)/(maxx-minx)));
  int binnoy=(int)(nbinsy*((valuey-miny)/(maxy-miny)));
  return binnoy*nbinsx+binnox;
}

float Calibrator::AccumulativeMean(float n, float oldmean, float newvalue){
    return oldmean*((n-1)/n)+newvalue/n;
}

bool Calibrator::HasKey(string key){
/* std::map<std::string,caldata>::iterator i = data.begin();

   cout << key << endl;
   for ( ; i != data.end(); ++i ){
   printf("# ");
   cout << i->first << endl;
   }
*/
  return data.find(key) != data.end();
}

bool Calibrator::CheckSelection(int level){
  //if (selection.find(level)!=selection.end() || selection.find(-3)!=selection.end() || selection.find(-4)!=selection.end()) return true;
  if (selection.find(level)!=selection.end() || selection.find(-3)!=selection.end()) return true;
  else return false;
}

bool Calibrator::Skip(){
  if (selection.find(-4)!=selection.end()) return true;
  else return false;
}

string Calibrator::PrintInfo(){
  string yn[2]={"NO","YES"};
  string info = string(Form("CONFIGURATION %-16s: dort=%-3s, dot0=%-3s, dores=%-3s, selection=",m_name.data(),yn[dort].data(),yn[dot0].data(),yn[dores].data()));
  for (std::set<int>::iterator isel = selection.begin(); isel != selection.end(); isel++) {
    if (*isel==-3) info += string("all"); 
    else if (*isel==-4) info += string("none"); 
    else info += string(Form("%2i,",*isel)); 
  } 
  return info;
}

string Calibrator::PrintStat(){
  string info = string(Form("STATISTICS %16s: nunits=%8i, nhits=%9i, hits/unit=%11.1f", m_name.data(), (int)data.size(), m_nhits, (float)m_nhits/(float)data.size() ));
  return info;
}

string Calibrator::GetOptString(){
  string optstr="";
  if (dort) optstr = optstr + "R";
  if (dot0) optstr = optstr + "T";
  if (printlog) optstr = optstr + "P";
  if (printt0) optstr = optstr + "F";
  if (printrt) optstr = optstr + "G";
  if (bequiet) optstr = optstr + "Q";
  if (usebref) optstr = optstr + "B";
  if (useshortstraws) optstr = optstr + "S";
  if (usep0 && dort) optstr = optstr + "0";
  if (floatp3 && dort) optstr = optstr + "3";
  if (!(dort || dot0 || usebref || bequiet || printlog || printt0 || printrt)) optstr = optstr + "N";
  
  return optstr;
}

int Calibrator::GetNDirs(){
  return data.size();
}

string Calibrator::GetSelString(){
  string selstr="";
  if (selection.find(-3)!=selection.end()) selstr="*";
  else if (selection.find(-4)!=selection.end()) selstr="-";
  else for (std::set<int>::iterator isel=selection.begin(); isel!=selection.end(); isel++) selstr += string(Form("%i",*isel));
  return selstr;
}

string Calibrator::GetBinnedRt(string key){

  int nbins=data[key].rtgraph->tval.size();
  double maxt=data[key].rtgraph->tval[nbins-1];
  double mint=data[key].rtgraph->tval[0];
  //double binwidth=(maxt-mint)/(nbins-1);

  string brt = "";
  brt += Form("%f %f %i ",mint,maxt,nbins);

  for (int ip=0; ip<nbins;ip++){
    brt += Form("%f ",data[key].rtgraph->rval[ip]);
    //cout << data[key].rtgraph->rval[ip] << endl;
  }
  return brt;
}

int Calibrator::UpdateOldConstants(){

  string line;
  ifstream oldconstfile("calib_constants_in.txt");
  string key;
  float t0;
  
  if (oldconstfile.is_open())
    {
      while (!oldconstfile.eof() )
      {
        oldconstfile >> key >> t0;
        if (data.find(key) != data.end()){
          cout << "UPDATED OLD T0: " << key << " " << data[key].oldt02 << " -> " << t0 << endl; 
          data[key].oldt02=t0;
        }
      }
      oldconstfile.close();
      return 0;
    }  
  else {
    cout << "NO OLD T0S FILE FOUND. USING AVERAGE VALUES!" << endl;
    return -1;
  } 
}

float Calibrator::FitRt(string key, string opt, TH2F* rtHist, TDirectory* dir){
  
  //float mintime=999.0;
  //float m_mindistance = 0.0; //0.1 
  //int npoints=rtHist->GetNbinsY();
  //char m_chname[100], m_chtit[100];
  //float tbinsize=3.125;// width=2.5*tbinsize, m_mean;
  //int m_maxbin;
  //float m_maxval;
  float rtpars[4];

  //TGraphErrors* rtgr = new TGraphErrors(npoints);
  //TGraphErrors* trgr = new TGraphErrors(npoints);

  //create r-m_t and m_t-r graphs
  RtGraph* rtg = m_rtbinning.find("t")==string::npos ? new RtGraph(rtHist,1,string("abs(rtrack)").data(),!bequiet,dir) : new RtGraph(rtHist,0,string("t-t0").data(),!bequiet,dir);

  TF1 dtfitfunc("dtfitfunc","gaus(0)");

  gStyle->SetOptFit(1111);
  gStyle->SetPalette(1);

  // select type of R-m_t relation
  TF1 *trfunc,*rtfunc,*oldrtfunc;
  if (m_isdines){
    trfunc = new TF1("trfunc",trrel_dines,0,3,4);
    rtfunc = new TF1("rtfunc",rtrel_dines,rtg->mintime,200,4);
    oldrtfunc = new TF1("oldrtfunc",rtrel_dines,rtg->mintime,200,4);
    trfunc->SetParameters(0.3, 1.0, 0.05, -3.);
    double rmin0Limits[2]={0.0,2.0};
    double rhoLimits[2]={0.0,10.};
    double vLimits[2]={0.0,1.0};
    trfunc->SetParLimits(0,rmin0Limits[0],rmin0Limits[1]);
    trfunc->SetParLimits(1,rhoLimits[0],rhoLimits[1]);
    trfunc->SetParLimits(2,vLimits[0],vLimits[1]);
    rtg->trgr->SetTitle("Dines' R-t sqrt(..)");
  }
  else {
    trfunc = new TF1("trfunc",pol3deg,0,3,4);
    rtfunc = new TF1("rtfunc",pol3deg,rtg->mintime,200,4);
    oldrtfunc = new TF1("oldrtfunc",pol3deg,rtg->mintime,200,4);
    trfunc->SetParameters(0.0, 0.0, 0.0, 0.0);
    rtg->trgr->SetTitle("Polynomial R-t");
  }

  // m_t-r relation
  trfunc->SetRange(0,2);
  if (opt.find("3")==string::npos) trfunc->FixParameter(3,0);
  trfunc->SetLineColor(4) ;
  rtg->trgr->Fit(trfunc,"QR"); // always fit the m_t-r relation
  //if (opt.find("Q")==string::npos) rtg->trgr->Write();
  if (!bequiet) rtg->trgr->Write();

  //float trpars[4]={trfunc->GetParameter(0),trfunc->GetParameter(1),trfunc->GetParameter(2),trfunc->GetParameter(3)};

  // r-m_t relation
  rtfunc->SetRange(0,45);
  if (opt.find("3")==string::npos) rtfunc->FixParameter(3,0);
  rtfunc->SetLineColor(4) ;
  if (m_isdines) { 
    // if Dines' R-m_t relation just use the parameters from the m_t-r relation
    rtfunc->SetParameters(trfunc->GetParameter(0),trfunc->GetParameter(1),trfunc->GetParameter(2),trfunc->GetParameter(3));
    //(rtg->rtgr->GetListOfFunctions())->Add(rtfunc->Clone());
    //rtfunc->SetParameters(0.000000e+00, 6.269950e-02, -3.370054e-04, -1.244642e-07);
    //rtg->rtgr->Fit(rtfunc,"QR");
    
    //(rtg->rtgr->GetListOfFunctions())->Add(rtfunc);
  }
  else { // else do a fit
    rtfunc->SetParameters(0.000000e+00, 6.269950e-02, -3.370054e-04, -1.244642e-07);
    rtg->rtgr->Fit(rtfunc,"QR"); //fit Rt first time
    for (int ipnt=0; ipnt<rtg->rtgr->GetN(); ipnt++){ //calculate m_t-errors
      rtg->rtgr->SetPointError(ipnt , rtg->rtgr->GetEY()[ipnt]/rtfunc->Derivative(rtg->rtgr->GetX()[ipnt]) , rtg->rtgr->GetEY()[ipnt]);
    }
    rtg->rtgr->Fit(rtfunc,"QR"); //fit again

  }
  //if (opt.find("Q")==string::npos)  rtg->rtgr->Write();
  if (!bequiet)  rtg->rtgr->Write();

  // old r-m_t relation
  oldrtfunc->SetRange(0,45);
  oldrtfunc->SetLineColor(1) ;
  oldrtfunc->SetLineStyle(2) ;
  oldrtfunc->SetLineWidth(1) ;
  oldrtfunc->SetParameters(data[key].oldrtpar[0],data[key].oldrtpar[1],data[key].oldrtpar[2],data[key].oldrtpar[3]);


  //cout << data[key].oldrtpar[0] << endl;
  //cout << data[key].oldrtpar[1] << endl;
  //cout << data[key].oldrtpar[2] << endl;
  //cout << data[key].oldrtpar[3] << endl;

  //if (opt.find("Q")==string::npos) oldrtfunc->Write();
  if (!bequiet) oldrtfunc->Write();

  rtpars[0] = rtfunc->GetParameter(0);
  rtpars[1] = rtfunc->GetParameter(1);
  rtpars[2] = rtfunc->GetParameter(2);
  rtpars[3] = rtfunc->GetParameter(3);

  //get the t0 from the tr relation
  float tdrift = 20;
  //if (m_isdines) tdrift = trfunc->GetParameter(1); 
  //else         tdrift = trfunc->GetParameter(0); 

  if (!m_isdines) {
    float rdrift = 0.0;
    int ntries = 0;
    float precision = 0.0001;
    int maxtries = 500;
    float drdt;
    float driftradius = rtpars[0]+tdrift*(rtpars[1]+tdrift*(rtpars[2]));
    float residual = fabs(rdrift) - driftradius;
    while (fabs(residual) > precision) {
      
      drdt = rtpars[1]+tdrift*(2*rtpars[2]);
      tdrift = tdrift + residual/drdt;
      
      driftradius = rtpars[0]+tdrift*(rtpars[1]+tdrift*(rtpars[2]));
      residual = rdrift - driftradius;
      
      ntries = ntries + 1;
      //cout << "RES " << residual << " " << tdrift << endl;
      if (ntries>maxtries){
        break;
      }
    }
  }
  
  if (opt.find("0")==string::npos) {
    if (m_isdines){
      data[key].rtpar[0] = rtpars[0];
      data[key].rtpar[1] = rtpars[1];
      data[key].rtpar[2] = rtpars[2];
      data[key].rtpar[3] = 0;
    } else {
      data[key].rtpar[0] = 0;
      data[key].rtpar[1] = rtpars[1] + 2*tdrift*rtpars[2] + 3*tdrift*tdrift*rtpars[3];
      data[key].rtpar[2] = rtpars[2] + 3*tdrift*rtpars[3];
      data[key].rtpar[3] = rtpars[3];
    }
  }
  else {
    data[key].rtpar[0] = rtpars[0];
    data[key].rtpar[1] = rtpars[1];
    data[key].rtpar[2] = rtpars[2];
    data[key].rtpar[3] = rtpars[3];
  }
  
  //cout << rtpars[0] << "==>" << data[key].rtpar[0] << endl;
  //cout << rtpars[1] << "==>" << data[key].rtpar[1] << endl;
  //cout << rtpars[2] << "==>" << data[key].rtpar[2] << endl;
  //cout << rtpars[3] << "==>" << data[key].rtpar[3] << endl;

  //m_rtgraphs[key]=rtg;
  data[key].rtgraph=rtg;

  delete rtfunc;
  delete oldrtfunc;
  delete trfunc;

  //return tdrift;
  return 0.0;
  //return rtpars[0];
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


  data[key].tresMean =  0;
  float fitmean=tresHist->GetBinCenter(maxbin);  
  if (fabs(fitmean)>5.0){ 
//    data[key].t0    = mean;
    data[key].tresMean =  mean;
    data[key].t0err = rms;
    data[key].t0fittype = 1;
    return mean;
  }

  float fitrms=rms;
  if (fitrms>5.0) fitrms=5.0;
  
  tresHist->Fit("gaus","QR","",mean-rms,mean+rms);
  if (tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2) < m_mintres ||
      tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2) > m_maxtres) {
    data[key].tresMean =  mean;
    data[key].t0err = rms;
    data[key].t0fittype = 1;
    return mean;
  }

  tresHist->Fit("gaus","QR","",tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2),tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2));
  if (tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2) < m_mintres ||
      tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2) > m_maxtres) {
    data[key].tres = tresHist->GetFunction("gaus")->GetParameter(2);
    data[key].tresMean = tresHist->GetFunction("gaus")->GetParameter(1);
    data[key].t0err = tresHist->GetFunction("gaus")->GetParError(1);
    data[key].t0fittype = 2;
    return tresHist->GetFunction("gaus")->GetParameter(1);
  }

  tresHist->Fit("gaus","QR","",tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2),tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2));
  if (tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2) < m_mintres ||
      tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2) > m_maxtres) {
    data[key].tres = tresHist->GetFunction("gaus")->GetParameter(2);
    data[key].tresMean = tresHist->GetFunction("gaus")->GetParameter(1);
    data[key].t0err = tresHist->GetFunction("gaus")->GetParError(1);
    data[key].t0fittype = 2;
    return tresHist->GetFunction("gaus")->GetParameter(1);
  }

  tresHist->Fit("gaus","QR","",tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2),tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2));
  if (tresHist->GetFunction("gaus")->GetParameter(1) - tresHist->GetFunction("gaus")->GetParameter(2) < m_mintres ||
      tresHist->GetFunction("gaus")->GetParameter(1) + tresHist->GetFunction("gaus")->GetParameter(2) > m_maxtres) {
    data[key].tresMean =  mean;
    data[key].t0err = rms;
    data[key].t0fittype = 1;
    return mean;
  }

  //tresHist->Fit("gaus","QR","",fitmean-fitrms,fitmean+fitrms);
  //tresHist->Fit("gaus","QR","",mean-fitrms,mean+fitrms);

  data[key].tres = tresHist->GetFunction("gaus")->GetParameter(2);
  data[key].tresMean = tresHist->GetFunction("gaus")->GetParameter(1);
  data[key].t0err = tresHist->GetFunction("gaus")->GetParError(1);
  data[key].t0fittype = 2;
 
  // protection against wrong fits:
  if ( fabs(tresHist->GetFunction("gaus")->GetParameter(2))>10 || fabs(tresHist->GetFunction("gaus")->GetParameter(1))>10   ) {
    
    data[key].tres = tresHist->GetRMS();
    data[key].tresMean =  tresHist->GetMean();
    data[key].t0err = tresHist->GetRMS();
    data[key].t0fittype = 6;
    return tresHist->GetMean(); 
  }
  
  
  gStyle->SetOptFit(1111);
  
  return tresHist->GetFunction("gaus")->GetParameter(1);
  }


float Calibrator::FitResidual(string key, TH1F* resHist){

  float mean = resHist->GetMean();
  //float rms = resHist->GetRMS();

  resHist->Fit("gaus","QRI","",mean-0.3,mean+0.3);
  resHist->Fit("gaus","QRI","",resHist->GetFunction("gaus")->GetParameter(1) - 1.5*resHist->GetFunction("gaus")->GetParameter(2),resHist->GetFunction("gaus")->GetParameter(1) + 1.5*resHist->GetFunction("gaus")->GetParameter(2));
  resHist->Fit("gaus","QRI","",resHist->GetFunction("gaus")->GetParameter(1) - 1.5*resHist->GetFunction("gaus")->GetParameter(2),resHist->GetFunction("gaus")->GetParameter(1) + 1.5*resHist->GetFunction("gaus")->GetParameter(2));
  resHist->Fit("gaus","QRI","",resHist->GetFunction("gaus")->GetParameter(1) - 1.5*resHist->GetFunction("gaus")->GetParameter(2),resHist->GetFunction("gaus")->GetParameter(1) + 1.5*resHist->GetFunction("gaus")->GetParameter(2));

  gStyle->SetOptFit(1111);

  data[key].res=resHist->GetFunction("gaus")->GetParameter(2);
  data[key].resMean=resHist->GetFunction("gaus")->GetParameter(1);
  data[key].reserr=resHist->GetFunction("gaus")->GetParError(2);

  return resHist->GetFunction("gaus")->GetParameter(2);

}

TDirectory* Calibrator::Calibrate(TDirectory* dir, string key, string opt, caldata * caldata_above){

  //set some bool flags
  bool calrt=opt.find("R")!=string::npos;
  bool calt0=opt.find("T")!=string::npos;
  bool donothing=opt.find("N")!=string::npos;
  //bool isquiet=opt.find("Q")!=string::npos;
  bool prnt=opt.find("P")!=string::npos;
  bool useref=opt.find("B")!=string::npos;
  bool useshortstw=opt.find("S")!=string::npos;
  
  if (donothing) return dir;

  data[key].calflag=true; 

  bool enough_t0 = data[key].ntres>=m_mint0stat;
  bool enough_rt = data[key].nrt>=m_minrtstat;

  // TDirectory* newdir;
  if ((enough_rt && calrt) || (enough_t0 && calt0)) {   
    m_hdirs[key] = dir->mkdir(Form("%s%s",m_name.data(),key.data()));
    m_hdirs[key]->cd();
  }
  else m_hdirs[key]=dir;
  
      
  //Fit also the residual if an rt or t0 calibration is made
  //if ((int)data[key].nres>50 && level<5) {    
  if ((int)data[key].nres>50 && ((enough_rt && calrt) || (enough_t0 && calt0))) {    
  //if ((int)data[key].nres>50) {    

    m_resHists[key] = new TH1F("residual","residual",m_nbinsres,m_minres,m_maxres);
    //m_resHists[key] = new TH1F("residual","residual",m_nbinsres,m_minres,m_maxres);
    for (int i=0;i<100;i++) {
      m_resHists[key]->SetBinContent(i+1,data[key].reshist[i]);
    }
    m_resHists[key]->SetEntries((int)data[key].nres);
    if (bequiet) m_resHists[key]->SetDirectory(0);                                 
    
    FitResidual(key,m_resHists[key]);

  }
  

  if (prnt) printf("TRTCalibrator: %8s %-14s: ",m_name.data(),key.data()); 

  //Calibrate r-m_t
  if (nort){
    //use old data
    data[key].rtflag=true;
    //data[key].rtt0=0;
    data[key].rtt0=caldata_above->rtt0;
    data[key].rtgraph=caldata_above->rtgraph;
    for (int i=0;i<4;i++) data[key].rtpar[i]=data[key].oldrtpar[i];
    if (prnt) printf("RT << %7i (%8.1e) %8.1e %8.1e %8.1e, %3.2f  : ", data[key].nrt,data[key].rtpar[0],data[key].rtpar[1],data[key].rtpar[2],data[key].rtpar[3], data[key].rtt0); 
  }
  else{
    //do fit
    if ((calrt && enough_rt) || level==0) {
      data[key].rtflag=true;
      m_rtHists[key] = new TH2F("rt-relation","rt-relation",m_nbinst,m_mint,m_maxt,m_nbinsr,m_minr,m_maxr); //make a root histogram
      for (int i=0;i<m_nbinst;i++) {
        for (int j=0;j<m_nbinsr;j++) {
          m_rtHists[key]->SetBinContent(i+1,j+1,data[key].rthist[i+m_nbinst*j]);
        }
      }
      m_rtHists[key]->SetEntries(data[key].nrt);
      if (bequiet) {
        m_rtHists[key]->SetDirectory(0);
      }
      data[key].rtt0=FitRt(key,opt,m_rtHists[key],m_hdirs[key]); //do the fit
      //if (prnt) printf("RT    %7i (%8.1e) %8.1e %8.1e %8.1e, %3.2f  : ", data[key].nrt, data[key].rtpar[0], data[key].rtpar[1], data[key].rtpar[2], data[key]. rtpar[3], data[key].rtt0);     
      if (prnt) printf("RT    %7i (%8.1e) %8.1e %8.1e %8.1e, %3.2f  : ", data[key].nrt, data[key].rtpar[0], data[key].rtpar[1], data[key].rtpar[2], data[key]. rtpar[3], data[key].rtt0);     
    }
    else{
      //use data from level above
      data[key].rtgraph=caldata_above->rtgraph;
      data[key].rtt0=caldata_above->rtt0;
      for (int i=0;i<4;i++) data[key].rtpar[i]=caldata_above->rtpar[i];
      if (prnt) printf("RT /\\ %7i (%8.1e) %8.1e %8.1e %8.1e, %3.2f  : ", data[key].nrt,data[key].rtpar[0],data[key].rtpar[1],data[key].rtpar[2],data[key].rtpar[3], data[key].rtt0); 
    }
  }


  //Calibrate t0
  if (not0){ 
    //use old data
    data[key].t0flag=true;
    data[key].t0=data[key].oldt02 + data[key].rtt0;
    data[key].t0err=0;
    data[key].t0off=0;
    data[key].t0fittype = 5;
    if (prnt) printf("T0 << %7i  %05.2f%+05.2f%+05.2f=%05.2f", data[key].ntres, data[key].oldt02, 0.0, data[key].rtt0, data[key].t0);       
  }
  else{
    if (useref && level==5){
      //use chip reference values 
      data[key].t0flag=true;
      data[key].t0=caldata_above->t0 + data[key].reft0 + data[key].rtt0;
      data[key].t0err=caldata_above->t0err;
      data[key].t0off=data[key].t0-caldata_above->t0;
      data[key].t0fittype = 3;
      if (prnt) printf("T0 ** %7i  %05.2f%+05.2f%+05.2f=%05.2f", data[key].ntres, caldata_above->t0, data[key].reft0, data[key].rtt0, data[key].t0); 
    }
    else {
      //do fit
      if ((calt0 && enough_t0) || level==0) {
        data[key].t0flag=true;
        m_tresHists[key] = new TH1F("timeresidual","time residual",m_nbinstres,m_mintres,m_maxtres); //make a root histogram

        for (int i=0;i<100;i++) {
          m_tresHists[key]->SetBinContent(i+1,data[key].m_treshist[i]);
        }
        m_tresHists[key]->SetEntries(data[key].ntres);
        if (bequiet) {
          m_tresHists[key]->SetDirectory(0);
        }      

        data[key].t0=data[key].oldt02 + FitTimeResidual(key,m_tresHists[key]) + data[key].rtt0 + m_t0shift; //do the fit and modify t0
        data[key].t0off=data[key].t0-caldata_above->t0; //calculate t0 offset from level above
        if (data[key].t0<0) data[key].t0=0;
        if (prnt) printf("T0    %7i  %05.2f%+05.2f%+05.2f%+05.2f=%05.2f", data[key].ntres, data[key].oldt02, data[key].t0-data[key].oldt02-data[key].rtt0, data[key].rtt0, m_t0shift, data[key].t0); 


      }
      //use data from level above
      else { 
        //data[key].t0=caldata_above->t0;
        //TEMP FIX to dont destroy right T0s
        if (data[key].oldt02 + (caldata_above->t0 - caldata_above->oldt02)  >0)    data[key].t0=data[key].oldt02 + (caldata_above->t0 - caldata_above->oldt02);
        else data[key].t0= 0;
        //TEMP FIX to dont destroy right T0s
        //if (level != 6 )  data[key].t0=data[key].oldt02;
        //if (level == 6 ) {
        //data[key].t0=caldata_above->t0;
        //if  (caldata_above->ntres< m_mint0stat )  data[key].t0=data[key].oldt02;
        //}
      //add the short straw correction here. In this way, the shift is only done when contants at STRAW level come from level above.
        if ((level == 6 && useshortstw) && fabs(data[key].det)<2  && (data[key].lay==0 && data[key].stl<9) )         data[key].t0=caldata_above->t0-0.75; 
        data[key].t0err=caldata_above->t0err;
        data[key].t0off=data[key].t0-caldata_above->t0 + data[key].rtt0;;
        data[key].t0fittype = 4;
//      if (prnt) printf("T0 /\\ %7i  %05.2f%+05.2f%+05.2f=%05.2f", data[key].ntres, caldata_above->t0, 0.0, 0.0, data[key].t0); 
      if (prnt) printf("T0 /\\ %7i  %05.2f%+05.2f%+05.2f=%05.2f", data[key].ntres, caldata_above->t0, caldata_above->oldt02, data[key].oldt02 ,  data[key].t0); 
      }
    }
  }


  if (prnt && !bequiet) cout << " H";

  if (prnt) cout << endl;


  return m_hdirs[key];

}


int Calibrator::AddHit(string key, databundle d, int* binhist, bool makehist){
  
  int tresbin=Simple1dHist(m_mintres,m_maxtres,m_nbinstres,d.tres);
  int resbin=Simple1dHist(m_minres,m_maxres,m_nbinsres,d.res);
  int rtbin=Simple2dHist(m_mint,m_maxt,m_nbinst,m_minr,m_maxr,m_nbinsr,d.t,d.r);
  int npop;
  int ibin;
  int value;
  
  //if the it is the first hit or histogram
  if (data.find(key) == data.end()){
    
    //create a new histogram object
    caldata* hist=new caldata(makehist,m_nbinst,m_nbinsr);    
 
    //out of memory?
    if (hist == nullptr){
      cout << "OUT OF MEMORY!" << endl;
      return -1;
    }

    hist->ntres=0;
    hist->nres=0;
    hist->nrt=0;
    hist->sumt0=0;

    //zero histogram bins
    for (int i=0;i<m_nbinsr*m_nbinst+200;i++) {
      if (makehist) hist->rthist[i]=0;
      if (i<100) {
        if (makehist) hist->m_treshist[i]=0;
        if (makehist) hist->reshist[i]=0;
      }
    }

    if (binhist==nullptr){ //if it is a hit
      
      if (tresbin>=0){ 
        if (makehist) hist->m_treshist[tresbin]=d.weight; //add value to bin
        hist->ntres=1; //set bin content to 1
        m_nhits=1;
      }
      if (resbin>=0){ 
        if (makehist) hist->reshist[resbin]=1;
        hist->nres=1;
      }
      if (rtbin>=0){ 
        if (makehist) hist->rthist[rtbin]=1;
        hist->nrt=1;
      }

      hist->sumt0=d.t0;
    }
    else { //if it is a histogram

      npop=binhist[0]; //the number of populated (non-zero) bins
      
      for(int ipop=2;ipop<2*npop+2;ipop=ipop+2) { //loop over the data

        ibin=binhist[ipop]; //bin number
        value=binhist[ipop+1]; //value

        if (ibin<100) { //timeresidual histogram
          m_ntreshits+=value;
          hist->ntres+=value;
          m_nhits+=value;
          if (makehist) hist->m_treshist[ibin]=(float)value;
          hist->sumt0+=d.t0*value;
        }
        else if (ibin>=100 && ibin<200) { //residual histogram
          m_nreshits+=value;
          hist->nres+=value;
          if (makehist) hist->reshist[ibin-100]=(float)value;
        }
        else { //rt histogram
          m_nrthits+=value;
          hist->nrt+=value;
          if (makehist) hist->rthist[ibin-200]=(float)value;
        }

      }    
    }

    //initialize parameters
    if (level>0) hist->det=d.det; else hist->det=-3; //
    if (level>1) hist->lay=d.lay; else hist->lay=-3;
    if (level>2) hist->mod=d.mod; else hist->mod=-3;
    if (level>3) hist->brd=d.brd; else hist->brd=-3;
    if (level>4) hist->chp=d.chp; else hist->chp=-3;
    if (level>5) hist->stw=d.stw; else hist->stw=-3;
    if (level>2) hist->stl=d.stl; else hist->stl=-3;
    if (level>5) hist->sid=d.sid; else hist->sid=-3;
    hist->res=0;
    hist->resMean=0;
    hist->reserr=0;
    hist->tres=0;
    hist->tresMean=0;
    hist->t0=0;
    if (level==5) hist->reft0=d.rt0; else hist->reft0=0;
    hist->t0off=0;
    hist->t0err=0;
    hist->nhits=1;
    hist->t0fittype=0;
    
    hist->x=d.x; //straw x coordinate
    hist->y=d.y; //straw y coordinate
    hist->z=d.z; //straw z coordinate

    hist->oldt02=d.t0; //straw old t0 value
    hist->oldrtpar[0]=d.rtpar[0];
    hist->oldrtpar[1]=d.rtpar[1];
    hist->oldrtpar[2]=d.rtpar[2];
    hist->oldrtpar[3]=d.rtpar[3];

    hist->sumx=0;
    hist->sumy=0;
    hist->sumz=0;

    hist->rtflag=false;
    hist->t0flag=false;
    hist->calflag=false;


    for (unsigned int i =0; i < 4; i++){
      hist->rtpar[i]=-10.0;
    }

    data[key]=*hist; //save the histogram in the map 

    delete hist;

    data[key].oldt02 = AccumulativeMean(data[key].nhits, data[key].oldt02, d.t0); //update old t0 m_mean value

    data[key].nhits++; // increment m_nhits

    return 1;
  }
  else { //if not the first hit or histogram

    //increment histogram bins
    if (binhist==nullptr){
      
      if (tresbin>=0){
        if (makehist) data[key].m_treshist[tresbin]=data[key].m_treshist[tresbin]+d.weight;
        data[key].ntres++;
        m_nhits=1;
      }
      if (resbin>=0){
        if (makehist) data[key].reshist[resbin]++;
        data[key].nres++;
      }
      if (rtbin>=0){
        if (makehist) data[key].rthist[rtbin]++;
        data[key].nrt++;
      }

      data[key].sumt0+=d.t0;

    }

    else {

      npop=binhist[0];

      for(int ipop=2;ipop<2*npop+2;ipop=ipop+2) {

        ibin=binhist[ipop];
        value=binhist[ipop+1];

        if (ibin<100) {
          m_ntreshits+=value;
          m_nhits+=value;
          data[key].ntres+=value;
          if (makehist) data[key].m_treshist[ibin]+=(float)value;
          data[key].sumt0+=d.t0*value;
        }
        else if (ibin>=100 && ibin<200) {
          m_nreshits+=value;
          data[key].nres+=value;
          if (makehist) data[key].reshist[ibin-100]+=(float)value;
        }
        else {
          m_nrthits+=value;
          data[key].nrt+=value;
          if (makehist) data[key].rthist[ibin-200]+=(float)value;
        }

      }    
      
    }

        
    data[key].sumx+=d.x;
    data[key].sumy+=d.y;
    data[key].sumz+=d.z;
    data[key].oldt02 = AccumulativeMean(data[key].nhits, data[key].oldt02, d.t0);
    data[key].oldrtpar[0] = AccumulativeMean(data[key].nhits, data[key].oldrtpar[0], d.rtpar[0]);
    data[key].oldrtpar[1] = AccumulativeMean(data[key].nhits, data[key].oldrtpar[1], d.rtpar[1]);
    data[key].oldrtpar[2] = AccumulativeMean(data[key].nhits, data[key].oldrtpar[2], d.rtpar[2]);
    data[key].oldrtpar[3] = AccumulativeMean(data[key].nhits, data[key].oldrtpar[3], d.rtpar[3]);
    data[key].x = AccumulativeMean(data[key].nhits, data[key].x, d.x);
    data[key].y = AccumulativeMean(data[key].nhits, data[key].y, d.y);
    data[key].z = AccumulativeMean(data[key].nhits, data[key].z, d.z);

    data[key].nhits++; //increment hit counts

    return 0;
  }    


}

void Calibrator::WriteStat(TDirectory* dir){
  dir->cd();
  TNtuple* stattup = new TNtuple(Form("%stuple",m_name.data()),"statistics","det:lay:mod:brd:chp:sid:stl:stw:rtflag:t0flag:t0:oldt0:rt0:dt0:t0offset:ftype:nhits:nt0:nrt:res:resMean:dres:tres:tresMean:x:y:z");
  for(map<string,caldata>::iterator ihist=data.begin(); ihist!=data.end(); ++ihist){
    if ((ihist->second).calflag) {
      float const ntvar[27]={
        float((ihist->second).det),
        float((ihist->second).lay),
        float((ihist->second).mod),
        float((ihist->second).brd),
        float((ihist->second).chp),
        float((ihist->second).sid),
        float((ihist->second).stl),
        float((ihist->second).stw),
        float((int)(ihist->second).rtflag),
        float((int)(ihist->second).t0flag),
        (ihist->second).t0,
        float((ihist->second).oldt02),
        //oldt0(ihist->first),
        (ihist->second).reft0,
        (ihist->second).t0err,
        (ihist->second).t0off,
        float((ihist->second).t0fittype),
        (ihist->second).nhits,
        (float)(ihist->second).ntres,
        (float)(ihist->second).nrt,
        (float)(ihist->second).res,
        (float)(ihist->second).resMean,
        (ihist->second).reserr,
        (ihist->second).tres,
        (ihist->second).tresMean,
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

void Calibrator::DumpConstants(){

  //ofstream dumpfile(Form("calib_constants_out.txt",level),ios::out | ios::app);
  ofstream dumpfile( "calib_constants_out.txt", ios::out | ios::app);

  for(map<string,caldata>::iterator ihist=data.begin(); ihist!=data.end(); ihist++){
    
    dumpfile << ihist->first << " " << (ihist->second).t0 << " " << endl;
    
  }
  
  dumpfile.close();
  
}

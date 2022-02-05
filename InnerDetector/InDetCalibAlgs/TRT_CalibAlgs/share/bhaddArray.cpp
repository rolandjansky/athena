/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <TMath.h>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <map>
#include <vector>
#include <TFile.h>
#include <TNtuple.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TVectorD.h>
using namespace std;

  int g_ntbins=55	;
  int g_nrbins=100	;
  int g_nhistbins=g_ntbins*g_nrbins+200;	





int Simple1dHist(float min, float max, int nbins, float value)
{
  if ( value<min | value>max ) return -1;
  int binno=(int)(nbins*((value-min)/(max-min)));
  return binno;
}

int Simple2dHist(float minx, float maxx, int nbinsx, float miny, float maxy, int nbinsy, float valuex, float valuey)
{
  if ( valuex<minx | valuex>maxx |  valuey<miny | valuey>maxy ) return -1;
  int binnox=(int)(nbinsx*((valuex-minx)/(maxx-minx)));
  int binnoy=(int)(nbinsy*((valuey-miny)/(maxy-miny)));
  return binnoy*nbinsx+binnox;
}

void process_mem_usage(double& vm_usage, double& resident_set)
{
  using std::ios_base;
  using std::ifstream;
  using std::string;

  vm_usage     = 0.0;
  resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat",ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
	      >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
	      >> utime >> stime >> cutime >> cstime >> priority >> nice
	      >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  vm_usage     = vsize / 1024.0;
  resident_set = rss * page_size_kb;
}


//=========================================

class CalHist{
public: 
  CalHist();
  ~CalHist();
  int IncreaseBin(short,unsigned short);
//  void Print(int);
  void GetArray(int*,int);
  CalHist(const CalHist &) = delete;
  CalHist & operator =(const CalHist &) = delete;  int maxvalue{};
private:
  unsigned short* hist{};
};

CalHist::CalHist(){
  maxvalue=0;
  hist = new unsigned short[g_nhistbins];
  for (int j= 0; j<g_nhistbins ; j++){
	hist[j] = 0;
   }
}



CalHist::~CalHist(){}

int CalHist::IncreaseBin(short bin, unsigned short value){
	if (bin > g_nhistbins) return 0;
	hist[bin] = hist[bin] + value;
	return 1;
}


void CalHist::GetArray(int* array, int nbins){
  for (int ibin=0; ibin<=nbins; ibin++){
	if (ibin < g_nhistbins) array[ibin] = hist[ibin];
    else array[ibin]=0;
  }
}



class CompBHist{

public:
  CompBHist(int,int*,int,int);
  ~CompBHist();
  CompBHist(const CompBHist &) = delete;
  CompBHist & operator=(const CompBHist &) = delete;
  int Print();
  void Write(ofstream*);
  int GetStat(int);
  int* hist{};
  int id{}
  int npop{};
};

CompBHist::CompBHist(int sid, int* uchist, int ntbins, int nrbins){
  id=sid;
  npop=0;
  int tmp[2][ntbins*nrbins+200];
  for (int ibin=0;ibin<ntbins*nrbins+200;ibin++){
    if (uchist[ibin]>0){
      tmp[0][npop]=ibin;
      tmp[1][npop]=uchist[ibin];
      npop++;
    }
  }
  
  hist=new int[npop*2+2];
  hist[0]=npop;
  hist[npop*2+1]=id;
  for (int ipop=0;ipop<npop;ipop++){
    hist[ipop*2+1]=tmp[0][ipop];
    hist[ipop*2+2]=tmp[1][ipop];
  }
}

CompBHist::~CompBHist(){
delete[] hist;
}

int CompBHist::Print(){
  //for(int ipop=0;ipop<1;ipop++) cout << hist[ipop] << " ";
  cout << npop << " " << id << " ";
  for(int ipop=0;ipop<2*npop+2;ipop++) cout << hist[ipop] << " ";
  cout << endl;
  return 0;
}

void CompBHist::Write(ofstream* file){
  file->write((char*)hist,(npop*2+2)*sizeof(int));
}

int CompBHist::GetStat(int val){
 
  int stat[3];
  stat[0]=0;
  stat[1]=0;
  stat[2]=0;
 
  for(int ipop=0;ipop<2*npop;ipop=ipop+2) {
    if (hist[ipop]<100) stat[0]+=hist[ipop+1];
    else if (hist[ipop]>=100 && hist[ipop]<200) stat[1]+=hist[ipop+1];
    else stat[2]+=hist[ipop+1];    
  }    

  return stat[val];

}

//=========================================

class trackdata{
public:
  float run,event,track,nhits,epnew,epold,t0,t,ttrack,theta,phi,pt,d0,trackres,trackresMean;
};

struct calibpars{
  float t0;
  float dt0;
};

map<string,calibpars> readoldpars() {
  string line;
  ifstream oldconstfile("precision_constants.txt");
  int det,lay,mod,stl,stw,dum;
  float t0,dt0;
  map<string,calibpars> oldparsmap;
  char key[100];

  if (oldconstfile.is_open())
    {
      while (! oldconstfile.eof() )
	{
	  getline(oldconstfile,line);
	  if (line.size()>25 && line.size()<55){
	    sscanf(line.data(),"%d %d %d %d %d : %e %e",&det,&lay,&mod,&stl,&stw,&t0,&dt0);
	    if (det>=-3 && lay>-1 && mod>-1 && stl>-1 && stw>-1) {
	      sprintf(key,"_%i_%i_%i_%i_%i",det,lay,mod,stl,stw); 
	      oldparsmap[string(key)].t0=t0; oldparsmap[string(key)].dt0=dt0;
	    }
	  }
	}
      oldconstfile.close();
    }
 
  else cout << "Unable to open precision t0 file!" << endl; 

  return oldparsmap;
}

int dump_tracktuple(map<string,trackdata> *trackmap){

  TFile* ttfile = new TFile("tracktuple.root","UPDATE");
  TNtuple* tracktup = new TNtuple("tracktuple","track data","run:evt:track:epnew:epold:nhits:t0:t:ttrack:theta:phi:d0:pt:trackres:trackresMean");
  for (std::map<std::string,trackdata>::iterator iep = trackmap->begin(); iep != trackmap->end(); ++iep){
    tracktup->Fill(iep->second.run, iep->second.event, iep->second.track,  iep->second.epnew/iep->second.nhits, iep->second.epold, iep->second.nhits, iep->second.t0/iep->second.nhits, iep->second.t/iep->second.nhits, iep->second.ttrack/iep->second.nhits ,  iep->second.theta,  iep->second.phi,  iep->second.d0,  iep->second.pt, (iep->second.trackres/iep->second.nhits),(iep->second.trackresMean/iep->second.nhits) );
  }
  ttfile->Write();
  ttfile->Close();

}

int dump_hists(map<int,CalHist*> *histmap,int ntbins, int nrbins, int ntres, char* fname, int fileno){

  int temparray[ntbins*nrbins+200];
  int maxnpop=0;
  int totnpop=0;

  ofstream ofile (Form("%s.part%i",fname,fileno), ios::out | ios::binary | ios::app);
  //ofstream ofilestat (Form("%s.stat.%i",fname,fileno), ios::out);

  for (map<int,CalHist*>::iterator it = histmap->begin(); it != histmap->end(); ++it){	
    
    
    it->second->GetArray(temparray,ntbins*nrbins+200);
    CompBHist* cbhist=new CompBHist(it->first,temparray,ntbins,nrbins);
    //cbhist->Print();
    cbhist->Write(&ofile);
    if (cbhist->npop>maxnpop) maxnpop=cbhist->npop;
    totnpop+=cbhist->npop;

    delete cbhist;


  }

  //ofilestat << "TRESHITS " << ntres << endl;

  cout << "DUMPING " << histmap->size() << " HISTOGRAMS, MAX NONZERO BINS: " << maxnpop << ", AVERAGE NONZERO BINS: " << (float)totnpop/histmap->size() << endl;
  
  ofile.close();
  //ofilestat.close();

  return 0;

}

//=========================================

int main(int argc, char *argv[]){

  
  float ptcutforerrors = 2;
  int who = RUSAGE_SELF;
  struct rusage usage;
  struct sysinfo sinfo;
  int ret;
  double vm, rss;

  float run,evt,trk,det,lay,mod,stl,stw,brd,chp,fsid,locx,locy,locz,x,y,z,r,dr,t,rtrack,ttrack,drtrack,t0,ephase,theta,phi,pt,d0,trackres,ToT,HT,qoverp;
  int sid; pt=0; d0=0; phi=0; theta=0; trackres=1;
  map<int,CalHist*> histmap;
  map<string,trackdata> trackmap;

  int ntbins=g_ntbins,nrbins=g_nrbins,ntresbins=100,nresbins=100,nhistbins,maxvalue=0;
  //int ntbins=64,nrbins=64,ntresbins=100,nresbins=100,nhistbins,maxvalue=0;
  double minr=0,maxr=2,mint=-5,maxt=50,mintres=-10,maxtres=10,minres=-0.6,maxres=0.6;
  //double minr=0,maxr=2,mint=-10,maxt=80,mintres=-25,maxtres=25,minres=-1.0,maxres=1.0;

  //int ntbins=64,nrbins=64,ntresbins=100,nresbins=100,nhistbins,maxvalue=0;
  //double minr=0,maxr=2,mint=-10,maxt=80,mintres=-25,maxtres=25,minres=-1.0,maxres=1.0;

  //sscanf (argv[1],"%i",&ntbins);
  //sscanf (argv[2],"%i",&nrbins);
  nhistbins=ntbins*nrbins+200;

  int npop;
  char header[50];
  int histdata[nhistbins];

  int nhits=0,nhisthits=0,nhists=0;
  int ntres=0,nres=0,nrt=0;
  int nhistsadd=0,nhitsadd=0,nbinsadd=0,nhiststmp=0;

  int nfiles=argc-2;
  cout << "PROCESSING " << nfiles << " FILE(S)" << endl << endl;

  ofstream ofilestat ((string(argv[1])+".stat").data(), ios::out);
  
  TH1F* residual_ref = new TH1F("residual_-2_0_0_1","residual_-2_0_0_1",nresbins,minres,maxres);
  TH1F* timeresidual_ref = new TH1F("timeresidual_1_0_0_0_1","timeresidual_1_0_0_0_1",ntresbins,mintres,maxtres);
  TH2F* rt_ref = new TH2F("rt_-1_2","rt_-1_2",ntbins,mint,maxt,nrbins,minr,maxr);
  TH2F* rt_ref2 = new TH2F("rt_-1","rt_-1",ntbins,mint,maxt,nrbins,minr,maxr);

  TH2F* reshist_trt = new TH2F("residual_trt","residual_trt",ntbins,mint,maxt,100,-0.4,0.4);
  TH2F* reshist_bar = new TH2F("residual_bar","residual_bar",ntbins,mint,maxt,100,-0.4,0.4);
  TH2F* reshist1 = new TH2F("residual_1","residual_1",ntbins,mint,maxt,100,-0.4,0.4);
  TH2F* reshist2 = new TH2F("residual_-1","residual_-1",ntbins,mint,maxt,100,-0.4,0.4);
  TH2F* reshist3 = new TH2F("residual_2","residual_2",ntbins,mint,maxt,100,-0.4,0.4);
  TH2F* reshist4 = new TH2F("residual_-2","residual_-2",ntbins,mint,maxt,100,-0.4,0.4);

  TH2F* treshist_trt = new TH2F("timeresidual_trt","timeresidual_trt",50,mintres,maxtres,nrbins,minr,maxr);
  TH2F* treshist_bar = new TH2F("timeresidual_bar","timeresidual_bar",50,mintres,maxtres,nrbins,minr,maxr);
  TH2F* treshist1 = new TH2F("timeresidual_1","timeresidual_1",50,mintres,maxtres,nrbins,minr,maxr);
  TH2F* treshist2 = new TH2F("timeresidual_-1","timeresidual_-1",50,mintres,maxtres,nrbins,minr,maxr);
  TH2F* treshist3 = new TH2F("timeresidual_2","timeresidual_2",50,mintres,maxtres,nrbins,minr,maxr);
  TH2F* treshist4 = new TH2F("timeresidual_-2","timeresidual_-2",50,mintres,maxtres,nrbins,minr,maxr);

  map<string,calibpars> pt0map=readoldpars();


  //Histograms to do error study: Barrel/Ecs:

  // Pt dependent:
  
  // Pull All hits:
  TH2F* pull_trt = new TH2F("pull_trt","pull_TRT_allhits", 	16, 0, 16 ,200,-4,4);
  TH2F* pull_ba =  new TH2F("pull_ba" ,"pull_BarrelA_allhits", 	16, 0, 16 ,200,-4,4);
  TH2F* pull_bc =  new TH2F("pull_bc" ,"pull_BarrelC_allhits",	16, 0, 16, 200,-4,4);
  TH2F* pull_ea =  new TH2F("pull_ea" ,"pull_EndcapA_allhits",	16, 0, 16, 200,-4,4);
  TH2F* pull_ec =  new TH2F("pull_ec" ,"pull_EndcapC_allhits",	16, 0, 16, 200,-4,4);
  
  // Residual All hits:
  TH2F* residual_trt = new TH2F("residual_trt","residual_TRT_allhits",    16, 0, 16 ,200,-2,2);
  TH2F* residual_ba =  new TH2F("residual_ba" ,"residual_BarrelA_allhits",16, 0, 16 ,200,-2,2);
  TH2F* residual_bc =  new TH2F("residual_bc" ,"residual_BarrelC_allhits",16, 0, 16 ,200,-2,2);
  TH2F* residual_ea =  new TH2F("residual_ea" ,"residual_EndcapA_allhits",16, 0, 16 ,200,-2,2);
  TH2F* residual_ec =  new TH2F("residual_ec" ,"residual_EndcapC_allhits",16, 0, 16 ,200,-2,2);
  
  // Time Residual All hits:
  TH2F* tresidual_trt = new TH2F("tresidual_trt","tresidual_TRT_allhits",    16, 0, 16 ,200,-20,20);
  TH2F* tresidual_ba =  new TH2F("tresidual_ba" ,"tresidual_BarrelA_allhits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_bc =  new TH2F("tresidual_bc" ,"tresidual_BarrelC_allhits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_ea =  new TH2F("tresidual_ea" ,"tresidual_EndcapA_allhits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_ec =  new TH2F("tresidual_ec" ,"tresidual_EndcapC_allhits",16, 0, 16 ,200,-20,20);
  
  // Pull Precision hits:
  TH2F* pull_trtP = new TH2F("pull_trtP","pull_TRT_Phits",    16, 0, 16 ,200,-4,4);
  TH2F* pull_baP =  new TH2F("pull_baP" ,"pull_BarrelA_Phits",16, 0, 16 ,200,-4,4);
  TH2F* pull_bcP =  new TH2F("pull_bcP" ,"pull_BarrelC_Phits",16, 0, 16 ,200,-4,4);
  TH2F* pull_eaP =  new TH2F("pull_eaP" ,"pull_EndcapA_Phits",16, 0, 16 ,200,-4,4);
  TH2F* pull_ecP =  new TH2F("pull_ecP" ,"pull_EndcapC_Phits",16, 0, 16 ,200,-4,4);
  // Residaul Precision hits:
  TH2F* residual_trtP = new TH2F("residual_trtP","residual_TRT_Phits",    16, 0, 16 ,200,-2,2);
  TH2F* residual_baP =  new TH2F("residual_baP" ,"residual_BarrelA_Phits",16, 0, 16 ,200,-2,2);
  TH2F* residual_bcP =  new TH2F("residual_bcP" ,"residual_BarrelC_Phits",16, 0, 16 ,200,-2,2);
  TH2F* residual_eaP =  new TH2F("residual_eaP" ,"residual_EndcapA_Phits",16, 0, 16 ,200,-2,2);
  TH2F* residual_ecP =  new TH2F("residual_ecP" ,"residual_EndcapC_Phits",16, 0, 16 ,200,-2,2);
  // Residaul Precision hits:
  TH2F* tresidual_trtP = new TH2F("tresidual_trtP","residual_TRT_Phits",    16, 0, 16 ,200,-20,20);
  TH2F* tresidual_baP =  new TH2F("tresidual_baP" ,"residual_BarrelA_Phits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_bcP =  new TH2F("tresidual_bcP" ,"residual_BarrelC_Phits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_eaP =  new TH2F("tresidual_eaP" ,"residual_EndcapA_Phits",16, 0, 16 ,200,-20,20);
  TH2F* tresidual_ecP =  new TH2F("tresidual_ecP" ,"residual_EndcapC_Phits",16, 0, 16 ,200,-20,20);
  // Track Errors :
  TH2F* trackerrors_trtP = new TH2F("trackerrors_trt","trackerrors_vs_pt_trt"	 ,16,0,16,200   , 0, 0.2);
  TH2F* trackerrors_baP =  new TH2F("trackerrors_ba", "trackerrors_vs_pt_BarrelA",16,0,16,200,0,.2);
  TH2F* trackerrors_bcP =  new TH2F("trackerrors_bc", "trackerrors_vs_pt_BarrelC",16,0,16,200,0,.2);
  TH2F* trackerrors_eaP =  new TH2F("trackerrors_ea", "trackerrors_vs_pt_EndcapA",16,0,16,200,0,.2);
  TH2F* trackerrors_ecP =  new TH2F("trackerrors_ec", "trackerrors_vs_pt_EndcapC",16,0,16,200,0,.2);
  
  //for (map<string,calibpars>::iterator iot=pt0map.begin(); iot!=pt0map.end(); iot++) {
  
  // Time Bin Dependent:
  
  int ntbins2 = 30;
  float tmin = -10;
  float tmax = 75 ;
  
  // TH2 to Store the Pull vs width:
  TH2F* pull_vs_tb_trt = new TH2F("pull_vs_tb_trt","pull_vs_timebin_trt"	,	 ntbins2, tmin , tmax,200,-4,4);
  TH2F* pull_vs_tb_ba  = new TH2F("pull_vs_tb_ba","pull_vs_timebin_BarrelA"	,	 ntbins2, tmin , tmax,200,-4,4);
  TH2F* pull_vs_tb_bc  = new TH2F("pull_vs_tb_bc","pull_vs_timebin_BarrelC"	,	 ntbins2, tmin , tmax,200,-4,4);
  TH2F* pull_vs_tb_ea  = new TH2F("pull_vs_tb_ea","pull_vs_timebin_EndcapA"	,	 ntbins2, tmin , tmax,200,-4,4);
  TH2F* pull_vs_tb_ec  = new TH2F("pull_vs_tb_ec","pull_vs_timebin_EndcapC"	,	 ntbins2, tmin , tmax,200,-4,4);
  
  // TH2 to Store the ErrorsUsed vs width:
  TH2F* errors_vs_tb_trt = new TH2F("errors_vs_tb_trt","errors_vs_timebin_trt"	,	ntbins2, tmin , tmax ,200,0,0.4);
  TH2F* errors_vs_tb_ba =  new TH2F("errors_vs_tb_ba","errors_vs_timebin_BarrelA",	ntbins2, tmin , tmax ,200,0,0.4);
  TH2F* errors_vs_tb_bc =  new TH2F("errors_vs_tb_bc","errors_vs_timebin_BarrelC",	ntbins2, tmin , tmax ,200,0,0.4);
  TH2F* errors_vs_tb_ea =  new TH2F("errors_vs_tb_ea","errors_vs_timebin_EndcapA",	ntbins2, tmin , tmax ,200,0,0.4);
  TH2F* errors_vs_tb_ec =  new TH2F("errors_vs_tb_ec","errors_vs_timebin_EndcapC",	ntbins2, tmin , tmax ,200,0,0.4);
  
  // TH2 to Store the ErrorsUsed vs width:
  TH2F* residual_vs_tb_trt = new TH2F("residual_vs_tb_trt","residual_vs_timebin_trt",		ntbins2, tmin , tmax,	200,-2,.2);
  TH2F* residual_vs_tb_ba =  new TH2F("residual_vs_tb_ba", "residual_vs_timebin_BarrelA",	ntbins2, tmin , tmax,	200,-2,.2);
  TH2F* residual_vs_tb_bc =  new TH2F("residual_vs_tb_bc", "residual_vs_timebin_BarrelC",	ntbins2, tmin , tmax,	200,-2,.2);
  TH2F* residual_vs_tb_ea =  new TH2F("residual_vs_tb_ea", "residual_vs_timebin_EndcapA",	ntbins2, tmin , tmax,	200,-2,.2);
  TH2F* residual_vs_tb_ec =  new TH2F("residual_vs_tb_ec", "residual_vs_timebin_EndcapC",	ntbins2, tmin , tmax,	200,-2,.2);
  
  // TH2 to Store the Track Errors vs width:
  TH2F* trackerrors_vs_tb_trt = new TH2F("trackerrors_vs_tb_trt","trackerrors_vs_timebin_trt",		ntbins2, tmin , tmax,200   , 0, 0.2);
  TH2F* trackerrors_vs_tb_ba =  new TH2F("trackerrors_vs_tb_ba","trackerrors_vs_timebin_BarrelA",	ntbins2, tmin , tmax,200,0,.2);
  TH2F* trackerrors_vs_tb_bc =  new TH2F("trackerrors_vs_tb_bc","trackerrors_vs_timebin_BarrelC",	ntbins2, tmin , tmax,200,0,.2);
  TH2F* trackerrors_vs_tb_ea =  new TH2F("trackerrors_vs_tb_ea","trackerrors_vs_timebin_EndcapA",	ntbins2, tmin , tmax,200,0,.2);
  TH2F* trackerrors_vs_tb_ec =  new TH2F("trackerrors_vs_tb_ec","trackerrors_vs_timebin_EndcapC",	ntbins2, tmin , tmax,200,0,.2);
 



	// Histograms to store the ToT Corrections, TIME RESIDUAL:
  TH2F* tres_vs_ToT_trt = new TH2F("tres_vs_ToT_trt","tres_vs_ToT_trt",		24, -0.5 , 23.5 , 200   , -20, 20);
  TH2F* tres_vs_ToT_ba = new TH2F("tres_vs_ToT_ba","tres_vs_ToT_ba",		24, -0.5 , 23.5 , 200   , -20, 20);
  TH2F* tres_vs_ToT_bc = new TH2F("tres_vs_ToT_bc","tres_vs_ToT_bc",		24, -0.5 , 23.5 , 200   , -20, 20);
  TH2F* tres_vs_ToT_ea = new TH2F("tres_vs_ToT_ea","tres_vs_ToT_ea",		24, -0.5 , 23.5 , 200   , -20, 20);
  TH2F* tres_vs_ToT_ec = new TH2F("tres_vs_ToT_ec","tres_vs_ToT_ec",		24, -0.5 , 23.5 , 200   , -20, 20);

	// Residual:
  TH2F* res_vs_ToT_trt = new TH2F("res_vs_ToT_trt","res_vs_ToT_trt",		24, -0.5 , 23.5 , 200   , -2, 2);
  TH2F* res_vs_ToT_ba = new TH2F("res_vs_ToT_ba","res_vs_ToT_ba",		24, -0.5 , 23.5 , 200   , -2, 2);
  TH2F* res_vs_ToT_bc = new TH2F("res_vs_ToT_bc","res_vs_ToT_bc",		24, -0.5 , 23.5 , 200   , -2, 2);
  TH2F* res_vs_ToT_ea = new TH2F("res_vs_ToT_ea","res_vs_ToT_ea",		24, -0.5 , 23.5 , 200   , -2, 2);
  TH2F* res_vs_ToT_ec = new TH2F("res_vs_ToT_ec","res_vs_ToT_ec",		24, -0.5 , 23.5 , 200   , -2, 2);


	// Histograms to store the HT Corrections, TIME RESIDUAL:
  TH2F* tres_vs_HT_trt = new TH2F("tres_vs_HT_trt","tres_vs_HT_trt",		2, -0.5 , 1.5 , 200   , -20, 20);
  TH2F* tres_vs_HT_ba = new TH2F("tres_vs_HT_ba","tres_vs_HT_ba",		2, -0.5 , 1.5 , 200   , -20, 20);
  TH2F* tres_vs_HT_bc = new TH2F("tres_vs_HT_bc","tres_vs_HT_bc",		2, -0.5 , 1.5 , 200   , -20, 20);
  TH2F* tres_vs_HT_ea = new TH2F("tres_vs_HT_ea","tres_vs_HT_ea",		2, -0.5 , 1.5 , 200   , -20, 20);
  TH2F* tres_vs_HT_ec = new TH2F("tres_vs_HT_ec","tres_vs_HT_ec",		2, -0.5 , 1.5 , 200   , -20, 20);

	// Residual:
  TH2F* res_vs_HT_trt = new TH2F("res_vs_HT_trt","res_vs_HT_trt",		2, -0.5 , 1.5 , 200   , -2, 2);
  TH2F* res_vs_HT_ba = new TH2F("res_vs_HT_ba","res_vs_HT_ba",			2, -0.5 , 1.5 , 200   , -2, 2);
  TH2F* res_vs_HT_bc = new TH2F("res_vs_HT_bc","res_vs_HT_bc",			2, -0.5 , 1.5 , 200   , -2, 2);
  TH2F* res_vs_HT_ea = new TH2F("res_vs_HT_ea","res_vs_HT_ea",			2, -0.5 , 1.5 , 200   , -2, 2);
  TH2F* res_vs_HT_ec = new TH2F("res_vs_HT_ec","res_vs_HT_ec",			2, -0.5 , 1.5 , 200   , -2, 2);



	// Histograms to store the sin(theta)/p Corrections, TIME RESIDUAL:
  TH2F* tres_vs_SinOverP_trt = new TH2F("tres_vs_SinOverP_trt","tres_vs_SinOverP_trt",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_SinOverP_ba = new TH2F("tres_vs_SinOverP_ba","tres_vs_SinOverP_ba",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_SinOverP_bc = new TH2F("tres_vs_SinOverP_bc","tres_vs_SinOverP_bc",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_SinOverP_ea = new TH2F("tres_vs_SinOverP_ea","tres_vs_SinOverP_ea",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_SinOverP_ec = new TH2F("tres_vs_SinOverP_ec","tres_vs_SinOverP_ec",		24, 0. , 1.,  200   , -20., 20.);

	// Residual:
  TH2F* res_vs_SinOverP_trt = new TH2F("res_vs_SinOverP_trt","res_vs_SinOverP_trt",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_SinOverP_ba = new TH2F("res_vs_SinOverP_ba","res_vs_SinOverP_ba",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_SinOverP_bc = new TH2F("res_vs_SinOverP_bc","res_vs_SinOverP_bc",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_SinOverP_ea = new TH2F("res_vs_SinOverP_ea","res_vs_SinOverP_ea",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_SinOverP_ec = new TH2F("res_vs_SinOverP_ec","res_vs_SinOverP_ec",		24, 0. , 1.,  200   , -2., 2.);


	// Histograms to store the sin(theta)/p Corrections, TIME RESIDUAL:
  TH2F* tres_vs_CosOverP_trt = new TH2F("tres_vs_CosOverP_trt","tres_vs_CosOverP_trt",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_CosOverP_ba = new TH2F("tres_vs_CosOverP_ba","tres_vs_CosOverP_ba",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_CosOverP_bc = new TH2F("tres_vs_CosOverP_bc","tres_vs_CosOverP_bc",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_CosOverP_ea = new TH2F("tres_vs_CosOverP_ea","tres_vs_CosOverP_ea",		24, 0. , 1.,  200   , -20., 20.);
  TH2F* tres_vs_CosOverP_ec = new TH2F("tres_vs_CosOverP_ec","tres_vs_CosOverP_ec",		24, 0. , 1.,  200   , -20., 20.);

	// Residual:
  TH2F* res_vs_CosOverP_trt = new TH2F("res_vs_CosOverP_trt","res_vs_CosOverP_trt",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_CosOverP_ba = new TH2F("res_vs_CosOverP_ba","res_vs_CosOverP_ba",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_CosOverP_bc = new TH2F("res_vs_CosOverP_bc","res_vs_CosOverP_bc",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_CosOverP_ea = new TH2F("res_vs_CosOverP_ea","res_vs_CosOverP_ea",		24, 0. , 1.,  200   , -2., 2.);
  TH2F* res_vs_CosOverP_ec = new TH2F("res_vs_CosOverP_ec","res_vs_CosOverP_ec",		24, 0. , 1.,  200   , -2., 2.);



 
  //for (map<string,calibpars>::iterator iot=pt0map.begin(); iot!=pt0map.end(); iot++) {
  //  cout << iot->second.t0 << " "  << iot->second.dt0 <<  endl;
  //}
  
  
  //*********************************************************************
  //  FILE LOOP
  for (int ifiles=0;ifiles<nfiles;ifiles++){
  //*********************************************************************
    
    bool isntuple=false;
    char filetype[5];
    ifstream myFile (argv[ifiles+2], ios::in | ios::binary);
    if(myFile.is_open()){
      myFile.read ((char*)filetype, 4); filetype[4]=0;
      if (strcmp(filetype,"root")==0) isntuple=true;
      //if (strcmp(filetype,"bina")==0) return 3;
      myFile.close();
    }
    else {
      cout << "WRONG INPUT FILE!" << endl;
      return -1;
    }
    
    
    //*********************************************************************
    //  SCANNING ROOT FILE     
    if (isntuple) {
    //*********************************************************************
      
      nhistsadd=0;
      //nhitsadd=0;
      //nhists=0;
      
      TFile* ntfile=new TFile(argv[ifiles+2]);
      TNtuple* hittuple=(TNtuple*)ntfile->Get("ntuple");
      
      hittuple->SetBranchAddress("run",&run);
      hittuple->SetBranchAddress("evt",&evt);
      hittuple->SetBranchAddress("trk",&trk);
      hittuple->SetBranchAddress("det",&det);
      hittuple->SetBranchAddress("lay",&lay);
      hittuple->SetBranchAddress("mod",&mod);
      hittuple->SetBranchAddress("brd",&brd);
      hittuple->SetBranchAddress("chp",&chp);
      hittuple->SetBranchAddress("sid",&fsid);
      hittuple->SetBranchAddress("stl",&stl);
      hittuple->SetBranchAddress("stw",&stw);
      hittuple->SetBranchAddress("r",&r);
      hittuple->SetBranchAddress("dr",&dr);
      hittuple->SetBranchAddress("dr",&dr);
      hittuple->SetBranchAddress("t",&t);
      hittuple->SetBranchAddress("t0",&t0);
      //hittuple->SetBranchAddress("rtrack",&rtrack);
      //hittuple->SetBranchAddress("ttrack",&ttrack);
      hittuple->SetBranchAddress("rtrackunbias",&rtrack);
      hittuple->SetBranchAddress("drrtrackunbias",&drtrack);
      hittuple->SetBranchAddress("ttrackunbias",&ttrack);
      hittuple->SetBranchAddress("ephase",&ephase);

      if( hittuple->GetListOfBranches()->FindObject("theta") ){
	hittuple->SetBranchAddress("theta",&theta);
      }
      if( hittuple->GetListOfBranches()->FindObject("phi") ){
	hittuple->SetBranchAddress("phi",&phi);
      }
      if( hittuple->GetListOfBranches()->FindObject("d0") ){
	hittuple->SetBranchAddress("d0",&d0);
      }
      if( hittuple->GetListOfBranches()->FindObject("pt") ){
	hittuple->SetBranchAddress("pt",&pt);
      }
      if( hittuple->GetListOfBranches()->FindObject("ToT") ){
	hittuple->SetBranchAddress("ToT",&ToT);
      }
	else ToT = 0;
      if( hittuple->GetListOfBranches()->FindObject("HT") ){
	hittuple->SetBranchAddress("HT",&HT);
      }
	else HT = 0;
      if( hittuple->GetListOfBranches()->FindObject("qoverp") ){
	hittuple->SetBranchAddress("qoverp",&qoverp);
      }
	else qoverp = 0;
      
      int nevents;
      int npt0=0;
      float diffpt0=0;
      nevents = hittuple->GetEntries();

      cout << "SCANNING ROOT FILE " << argv[ifiles+2] << " (" << nevents << " HITS)" << endl;


      //*********************************************************************
      //  EVENT LOOP
      for (int ievt=0;ievt<nevents;ievt++){
      //for (int ievt=25000000;ievt<nevents;ievt++){
      //for (int ievt=0;ievt<2000100;ievt++){
      //for (int ievt=0;ievt<1000;ievt++){
      //*********************************************************************

	hittuple->GetEntry(ievt);

	if (dr<1.0){ //event selection
	  //if (true){ //event selection
	  
	  nhits++;
	  
	  string trkkey=string(Form("_%i_%i_%i",(int)run,(int)evt,(int)trk));	
	  
	  string pt0key=string(Form("_%i_%i_%i_%i_%i",(int)det,(int)lay,(int)mod,(int)stl,(int)stw));
	  if (pt0map.find(pt0key) != pt0map.end()) {
	    diffpt0+=fabs(t0-pt0map[string(Form("_%i_%i_%i_%i_%i",(int)det,(int)lay,(int)mod,(int)stl,(int)stw))].t0);
	    t0=pt0map[string(Form("_%i_%i_%i_%i_%i",(int)det,(int)lay,(int)mod,(int)stl,(int)stw))].t0;
	    npt0++;
	    //cout << string(Form("_%i_%i_%i_%i_%i",(int)det,(int)lay,(int)mod,(int)stl,(int)stw)) << " " << t0 << " -> " << pt0map[string(Form("_%i_%i_%i_%i_%i",(int)det,(int)lay,(int)mod,(int)stl,(int)stw))].t0 << endl;
	  }
	  
	  if (trackmap.find(trkkey) == trackmap.end()){
	    trackmap[trkkey].run=run;
	    trackmap[trkkey].event=evt;
	    trackmap[trkkey].track=trk;
	    trackmap[trkkey].epnew=(t+ephase)-t0-ttrack;
	    trackmap[trkkey].epold=ephase;
	    trackmap[trkkey].nhits=1.0;
	    trackmap[trkkey].t=t;
	    trackmap[trkkey].ttrack=ttrack;
	    trackmap[trkkey].t0=t0;
	    trackmap[trkkey].theta=theta;
	    trackmap[trkkey].phi=phi;
	    trackmap[trkkey].d0=d0;
	    trackmap[trkkey].pt=pt;
	    trackmap[trkkey].trackres=(r-rtrack)*(r-rtrack);
	    trackmap[trkkey].trackresMean=(r-rtrack);
	  }
	  else {
	    trackmap[trkkey].epnew+=(t+ephase)-t0-ttrack;
	    trackmap[trkkey].t+=t;
	    trackmap[trkkey].ttrack+=ttrack;
	    trackmap[trkkey].t0+=t0;
	    trackmap[trkkey].nhits++;
	    trackmap[trkkey].trackres+= (r-rtrack)*(r-rtrack);
	    trackmap[trkkey].trackresMean+=(r-rtrack);
	  }
	  sid=(int)fsid;
	  
	  short tresbin=Simple1dHist(mintres,maxtres,ntresbins,t-ttrack-t0+ephase);
	  short rresbin=Simple1dHist(minres,maxres,nresbins,r-rtrack);
	  short trbin=Simple2dHist(mint,maxt,ntbins,minr,maxr,nrbins,t-t0+ephase,fabs(rtrack));
	  
	  if ((int)det==-2 && (int)lay==0 && (int)mod==0 && (int)brd==1) residual_ref->Fill(r-rtrack);
	  if ((int)det==1 && (int)lay==0 && (int)mod==0 && (int)brd==0 && (int)chp==1) timeresidual_ref->Fill(t-ttrack-t0+ephase);
	  if ((int)det==-1 && (int)lay==2) rt_ref->Fill(t-t0+ephase,fabs(rtrack));
          if ((int)det==-1) rt_ref2->Fill(t-t0+ephase,fabs(rtrack));
	  	  
	  nhisthits++;
	  
	  if(histmap.find(sid) == histmap.end()){
	    nhistsadd++;
	    nhists++;
	    histmap[sid]=new CalHist();
	  } 
	  if (tresbin>=0){ 
	    nbinsadd += histmap[sid]->IncreaseBin(tresbin,1);
	    ntres++;
	    nhitsadd++;
	  }
	  if (rresbin>=0){ 
	    nbinsadd += histmap[sid]->IncreaseBin(rresbin+100,1);
	    nres++;
	  }
	  if (trbin>=0){
	    nbinsadd += histmap[sid]->IncreaseBin(trbin+200,1);
	    nrt++;
	  }
	  
	  if (histmap[sid]->maxvalue > maxvalue) maxvalue=histmap[sid]->maxvalue;
	  
	  if (nhits%1000000==0 | ievt==nevents-1){ 
	    process_mem_usage(vm, rss);
	    //cppcheck-suppress invalidPrintfArgType_uint
	    printf("%8i HITS READ, %8i HISTOGRAMS (%8lu ADDED), %8i BINS ADDED, MAXVALUE: %3i, VM: %8.0f, RSS: %8.0f\n",nhits,nhists,histmap.size()-nhiststmp,nbinsadd,maxvalue,vm,rss);
	    nhiststmp=histmap.size();
	    nbinsadd=0;
	  }      
	
	  
	  reshist_trt->Fill(t-t0,fabs(r)-fabs(rtrack));
	  if ((int)det==-2) reshist1->Fill(t-t0+ephase,fabs(r)-fabs(rtrack));
	  if ((int)det==-1) reshist2->Fill(t-t0+ephase,fabs(r)-fabs(rtrack));
	  if ((int)det==1) reshist3->Fill(t-t0+ephase,fabs(r)-fabs(rtrack));
	  if ((int)det==2) reshist4->Fill(t-t0+ephase,fabs(r)-fabs(rtrack));
	  if (abs((int)det)==1) reshist_bar->Fill(t-t0+ephase,fabs(r)-fabs(rtrack));
	  
	  treshist_trt->Fill(t-t0-ttrack,fabs(rtrack));
	  if ((int)det==-2) treshist1->Fill(t-t0-ttrack+ephase,fabs(rtrack));
	  if ((int)det==-1) treshist2->Fill(t-t0-ttrack+ephase,fabs(rtrack));
	  if ((int)det==1) treshist3->Fill(t-t0-ttrack+ephase,fabs(rtrack));
	  if ((int)det==2) treshist4->Fill(t-t0-ttrack+ephase,fabs(rtrack));
	  if (abs((int)det)==1) treshist_bar->Fill(t-t0-ttrack+ephase,fabs(rtrack));
	  

	  float residual 	= r-rtrack; 
	  float timebin = int((t-t0 + ephase)/3.125 ); 
	  float time=t-t0 + ephase; 
	  float pull	= (r-rtrack)/TMath::Sqrt( dr*dr + drtrack*drtrack ) ; 
	  float tresidual  = t - t0 - ttrack + ephase;

		ToT = ToT/3.125;


	  float sinoverp 	= 0.0;
	  float cosoverp	= 0.0;
		if (pt != 0.0) {
			sinoverp = 1000. * sin(theta)*sin(theta) / pt ; 
			cosoverp = 1000. * cos(theta)*cos(theta) /pt ; 
		}


	  float ppt = pt/1000.;
	  if (ppt>16) ppt = 15.5;


	  tres_vs_ToT_trt 	->Fill( ToT, tresidual);
	  res_vs_ToT_trt 	->Fill( ToT, residual);
	  tres_vs_HT_trt 	->Fill( HT, tresidual);
	  res_vs_HT_trt 	->Fill( HT, residual);
	  tres_vs_SinOverP_trt 	->Fill( sinoverp, tresidual);
	  res_vs_SinOverP_trt 	->Fill( sinoverp, residual);
	  tres_vs_CosOverP_trt 	->Fill( cosoverp, tresidual);
	  res_vs_CosOverP_trt 	->Fill( cosoverp, residual);


	  pull_trtP		->Fill( ppt, pull   	 );
	  residual_trtP		->Fill(	ppt, residual );	
	  tresidual_trtP	->Fill(	ppt, tresidual );	
	  trackerrors_trtP	->Fill( ppt, drtrack  );
	  if (ppt>ptcutforerrors)		pull_vs_tb_trt		->Fill( time, pull	);
	  if (ppt>ptcutforerrors) 	errors_vs_tb_trt	->Fill(	time, dr	);
	  if (ppt>ptcutforerrors) 	trackerrors_vs_tb_trt	->Fill(	time, drtrack);
	  if (ppt>ptcutforerrors) 	residual_vs_tb_trt	->Fill(	time, residual);

	  if ((int)det==-2) {
	    tres_vs_ToT_ea 	->Fill( ToT, tresidual);
	    res_vs_ToT_ea 	->Fill( ToT, residual);
	    tres_vs_HT_ea 	->Fill( HT, tresidual);
	    res_vs_HT_ea 	->Fill( HT, residual);
	    tres_vs_SinOverP_ea	->Fill( sinoverp, tresidual);
	    res_vs_SinOverP_ea 	->Fill( sinoverp, residual);
	    tres_vs_CosOverP_ea	->Fill( cosoverp, tresidual);
	    res_vs_CosOverP_ea 	->Fill( cosoverp, residual);

	    pull_eaP		->Fill( ppt, pull   	 );
	    residual_eaP		->Fill(	ppt, residual );	
	    tresidual_eaP		->Fill(	ppt, tresidual );	
	    trackerrors_eaP		->Fill( ppt, drtrack  );
	    if (ppt>ptcutforerrors) 	pull_vs_tb_ea		->Fill( time, pull	);
	    if (ppt>ptcutforerrors) 	errors_vs_tb_ea		->Fill(	time, dr	);
	    if (ppt>ptcutforerrors) 	trackerrors_vs_tb_ea 	->Fill(	time, drtrack);
	    if (ppt>ptcutforerrors) 	residual_vs_tb_ea	->Fill(	time, residual);
	  }
	  if ((int)det==2) {
	    tres_vs_ToT_ec 	->Fill( ToT, tresidual);
	    res_vs_ToT_ec 	->Fill( ToT, residual);
	    tres_vs_HT_ec 	->Fill( HT, tresidual);
	    res_vs_HT_ec 	->Fill( HT, residual);
	    tres_vs_SinOverP_ec	->Fill( sinoverp, tresidual);
	    res_vs_SinOverP_ec 	->Fill( sinoverp, residual);
	    tres_vs_CosOverP_ec	->Fill( cosoverp, tresidual);
	    res_vs_CosOverP_ec 	->Fill( cosoverp, residual);

	    pull_ecP		->Fill( ppt, pull   	 );
	    residual_ecP		->Fill(	ppt, residual );	
	    tresidual_ecP		->Fill(	ppt, tresidual );	
	    trackerrors_ecP		->Fill( ppt, drtrack  );
	    if (ppt>ptcutforerrors) 	pull_vs_tb_ec		->Fill( time, pull	);
	    if (ppt>ptcutforerrors) 	errors_vs_tb_ec		->Fill(	time, dr	);
	    if (ppt>ptcutforerrors) 	trackerrors_vs_tb_ec 	->Fill(	time, drtrack);
	    if (ppt>ptcutforerrors) 	residual_vs_tb_ec	->Fill(	time, residual);
	  }
	  if ((int)det==-1) {
	    tres_vs_ToT_ba 	->Fill( ToT, tresidual);
	    res_vs_ToT_ba 	->Fill( ToT, residual);
	    tres_vs_HT_ba 	->Fill( HT, tresidual);
	    res_vs_HT_ba 	->Fill( HT, residual);
	    tres_vs_SinOverP_ba	->Fill( sinoverp, tresidual);
	    res_vs_SinOverP_ba 	->Fill( sinoverp, residual);
	    tres_vs_CosOverP_ba	->Fill( cosoverp, tresidual);
	    res_vs_CosOverP_ba 	->Fill( cosoverp, residual);

	    pull_baP		->Fill( ppt, pull   	 );
	    residual_baP		->Fill(	ppt, residual );	
	    tresidual_baP		->Fill(	ppt, tresidual );	
	    trackerrors_baP		->Fill( ppt, drtrack  );
	    if (ppt>ptcutforerrors) 	pull_vs_tb_ba		->Fill( time, pull	);
	    if (ppt>ptcutforerrors) 	errors_vs_tb_ba		->Fill(	time, dr	);
	    if (ppt>ptcutforerrors) 	trackerrors_vs_tb_ba 	->Fill(	time, drtrack);
	    if (ppt>ptcutforerrors) 	residual_vs_tb_ba	->Fill(	time, residual);
	  }
	  if ((int)det==1) {
	    tres_vs_ToT_bc 	->Fill( ToT, tresidual);
	    res_vs_ToT_bc 	->Fill( ToT, residual);
	    tres_vs_HT_bc 	->Fill( HT, tresidual);
	    res_vs_HT_bc 	->Fill( HT, residual);
	    tres_vs_SinOverP_bc	->Fill( sinoverp, tresidual);
	    res_vs_SinOverP_bc 	->Fill( sinoverp, residual);
	    tres_vs_CosOverP_bc	->Fill( cosoverp, tresidual);
	    res_vs_CosOverP_bc 	->Fill( cosoverp, residual);

	    pull_bcP		->Fill( ppt, pull   	 );
	    residual_bcP		->Fill(	ppt, residual );	
	    tresidual_bcP		->Fill(	ppt, tresidual );	
	    trackerrors_bcP		->Fill( ppt, drtrack  );
	    if (ppt>ptcutforerrors) 	pull_vs_tb_bc		->Fill( time, pull	);
	    if (ppt>ptcutforerrors) 	errors_vs_tb_bc		->Fill(	time, dr	);
	    if (ppt>ptcutforerrors) 	trackerrors_vs_tb_bc 	->Fill(	time, drtrack);
	    if (ppt>ptcutforerrors) 	residual_vs_tb_bc	->Fill(	time, residual);
	  }
	} // ... if event selection

	float ppt = pt/1000.;
	if (ppt>16) ppt = 15.5;
	float residual  = r-rtrack;
	float tresidual  = t - t0 - ttrack + ephase;
	float pull      = (r-rtrack)/TMath::Sqrt( dr*dr + drtrack*drtrack ) ;
	pull_trt               ->Fill( ppt ,pull     );
	residual_trt           ->Fill( ppt, residual );
	tresidual_trt           ->Fill( ppt, residual );
	if ((int)det==-2) {
	  pull_ea                ->Fill( ppt, pull     );
	  tresidual_ea            ->Fill( ppt, tresidual );
	}
	if ((int)det==2) {
	  pull_ec                ->Fill( ppt, pull     );
	  residual_ec            ->Fill( ppt, residual );
	  tresidual_ec            ->Fill( ppt, tresidual );
	}
	if ((int)det==-1) {
	  pull_ba                ->Fill( ppt, pull     );
	  residual_ba            ->Fill( ppt, residual );
	  tresidual_ba            ->Fill( ppt, tresidual );
	}
	if ((int)det==1) {
	  pull_bc                ->Fill( ppt, pull     );
	  residual_bc            ->Fill( ppt, residual );
	  tresidual_bc            ->Fill( ppt, tresidual );
	}

      } // END OF EVENT LOOP
      
      printf("REPLACED %d T0S WITH PRECICION VALUES (MEAN DIFF=%f)\n",npt0,diffpt0/float(npt0));
      
      if (ifiles==nfiles-1){
	
	TFile* ttfile = new TFile("tracktuple.root","UPDATE");
	
	TDirectory* trtdir = ttfile->mkdir("TRT_all");
	TDirectory* binhist =trtdir->mkdir("reshists");
	TDirectory* detdir1 =trtdir->mkdir("Detector_-2");
	TDirectory* binhist1 =detdir1->mkdir("reshists");
	TDirectory* detdir2 =trtdir->mkdir("Detector_-1");
	TDirectory* binhist2 =detdir2->mkdir("reshists");
	TDirectory* detdir3 =trtdir->mkdir("Detector_1");
	TDirectory* binhist3 =detdir3->mkdir("reshists");
	TDirectory* detdir4 =trtdir->mkdir("Detector_2");
	TDirectory* binhist4 =detdir4->mkdir("reshists");
	TDirectory* detdir5 =trtdir->mkdir("WholeBarrel_1");
	TDirectory* binhist5 =detdir5->mkdir("reshists");

	TDirectory* errordir = ttfile->mkdir("Errors");
	TDirectory* corrdir = ttfile->mkdir("Correction");
	
	int npointsX =  reshist1->GetNbinsX();
	int npointsY =  treshist1->GetNbinsY();
	TH1D** hslizesX = new TH1D*[npointsX];
	TH1D** hslizesY = new TH1D*[npointsY];
	
	TVectorD tbins(npointsX);
	TVectorD rbins(npointsY);
	
	string chname;
	for (int i=0;i<npointsX;i++) {

	  tbins(i)=reshist1->GetXaxis()->GetBinCenter(i+1);

	  binhist->cd();
	  chname = string(Form("res_tbin%i_trt",i));
	  hslizesX[i] = reshist_trt->ProjectionY(chname.data(),i+1,i+1);
	  binhist1->cd();
	  chname = string(Form("res_tbin%i_-2",i));
	  hslizesX[i] = reshist1->ProjectionY(chname.data(),i+1,i+1);
	  binhist2->cd();
	  chname = string(Form("res_tbin%i_-1",i));
	  hslizesX[i] = reshist2->ProjectionY(chname.data(),i+1,i+1);
	  binhist3->cd();
	  chname = string(Form("res_tbin%i_1",i));
	  hslizesX[i] = reshist3->ProjectionY(chname.data(),i+1,i+1);
	  binhist4->cd();
	  chname = string(Form("res_tbin%i_2",i));
	  hslizesX[i] = reshist4->ProjectionY(chname.data(),i+1,i+1);
	  binhist5->cd();
	  chname = string(Form("res_tbin%i_bar",i));
	  hslizesX[i] = reshist_bar->ProjectionY(chname.data(),i+1,i+1);
	}
	
	for (int i=0;i<npointsY;i++) {

	  rbins(i)=treshist1->GetYaxis()->GetBinCenter(i+1);

	  binhist->cd();
	  chname = string(Form("tres_rbin%i_trt",i));
	  hslizesY[i] = treshist_trt->ProjectionX(chname.data(),i+1,i+1);
	  binhist1->cd();
	  chname = string(Form("tres_rbin%i_-2",i));
	  hslizesY[i] = treshist1->ProjectionX(chname.data(),i+1,i+1);
	  binhist2->cd();
	  chname = string(Form("tres_rbin%i_-1",i));
	  hslizesY[i] = treshist1->ProjectionX(chname.data(),i+1,i+1);
	  binhist3->cd();
	  chname = string(Form("tres_rbin%i_1",i));
	  hslizesY[i] = treshist3->ProjectionX(chname.data(),i+1,i+1);
	  binhist4->cd();
	  chname = string(Form("tres_rbin%i_2",i));
	  hslizesY[i] = treshist4->ProjectionX(chname.data(),i+1,i+1);
	  binhist5->cd();
	  chname = string(Form("tres_rbin%i_bar",i));
	  hslizesY[i] = treshist_bar->ProjectionX(chname.data(),i+1,i+1);
	}
	
	ttfile->cd();	
	tbins.Write("tbins");
	rbins.Write("rbins");
		
	trtdir->cd();	
	residual_ref->Write();
	timeresidual_ref->Write();
	rt_ref->Write();
        rt_ref2->Write();

	binhist->cd(); reshist_trt->Write();
	binhist5->cd(); reshist_bar->Write();
	binhist1->cd(); reshist1->Write();
	binhist2->cd(); reshist2->Write();
	binhist3->cd(); reshist3->Write();
	binhist4->cd(); reshist4->Write();
	binhist->cd(); treshist_trt->Write();
	binhist5->cd(); treshist_bar->Write();
	binhist1->cd(); treshist1->Write();
	binhist2->cd(); treshist2->Write();
	binhist3->cd(); treshist3->Write();
	binhist4->cd(); treshist4->Write();

	// Save stuff for errors:
	errordir->cd();
	
  	pull_trt->Write();
  	pull_ba ->Write();
  	pull_bc->Write();
  	pull_ea->Write();
  	pull_ec->Write();

  	residual_trt->Write();
  	residual_ba->Write();
  	residual_bc->Write();
  	residual_ea->Write();
  	residual_ec->Write();
    
  	tresidual_trt->Write();
  	tresidual_ba->Write();
  	tresidual_bc->Write();
  	tresidual_ea->Write();
  	tresidual_ec->Write();
    
  	pull_trtP ->Write();
  	pull_baP->Write();
  	pull_bcP->Write();
  	pull_eaP->Write();
  	pull_ecP->Write();

  	residual_trtP->Write();
  	residual_baP->Write();
  	residual_bcP->Write();
  	residual_eaP->Write();
  	residual_ecP->Write();

  	tresidual_trtP->Write();
  	tresidual_baP->Write();
  	tresidual_bcP->Write();
  	tresidual_eaP->Write();
  	tresidual_ecP->Write();

  	pull_vs_tb_trt->Write();
  	pull_vs_tb_trt->Write();
  	pull_vs_tb_ba->Write();
  	pull_vs_tb_bc->Write();
  	pull_vs_tb_ea->Write();
  	pull_vs_tb_ec->Write();

  	errors_vs_tb_trt->Write();
  	errors_vs_tb_ba->Write();
  	errors_vs_tb_bc->Write();
  	errors_vs_tb_ea->Write();
  	errors_vs_tb_ec->Write();

  	residual_vs_tb_trt->Write();
  	residual_vs_tb_ba->Write();
  	residual_vs_tb_bc->Write();
  	residual_vs_tb_ea->Write();
  	residual_vs_tb_ec->Write();

  	trackerrors_trtP->Write();
  	trackerrors_baP->Write();
  	trackerrors_bcP->Write();
  	trackerrors_eaP->Write();
  	trackerrors_ecP->Write();

  	trackerrors_vs_tb_trt->Write();
  	trackerrors_vs_tb_ba->Write();
  	trackerrors_vs_tb_bc->Write();
  	trackerrors_vs_tb_ea->Write();
  	trackerrors_vs_tb_ec->Write();


	// Save HT correction and ToT 	
	corrdir->cd();

	tres_vs_ToT_trt->Write() ;
	tres_vs_ToT_ba->Write() ;
	tres_vs_ToT_bc->Write() ;
	tres_vs_ToT_ea->Write() ;
	tres_vs_ToT_ec->Write() ;

	res_vs_ToT_trt->Write() ;
	res_vs_ToT_ba->Write() ;
	res_vs_ToT_bc->Write() ;
	res_vs_ToT_ea->Write() ;
	res_vs_ToT_ec->Write() ;


	tres_vs_HT_trt->Write();
	tres_vs_HT_ba->Write() ;
	tres_vs_HT_bc->Write() ;
	tres_vs_HT_ea->Write() ;
	tres_vs_HT_ec->Write() ;

	res_vs_HT_trt->Write();
	res_vs_HT_ba->Write() ;
	res_vs_HT_bc->Write() ;
	res_vs_HT_ea->Write() ;
	res_vs_HT_ec->Write() ;

	tres_vs_SinOverP_trt->Write();
	tres_vs_SinOverP_ba->Write() ;
	tres_vs_SinOverP_bc->Write() ;
	tres_vs_SinOverP_ea->Write() ;
	tres_vs_SinOverP_ec->Write() ;

	res_vs_SinOverP_trt->Write();
	res_vs_SinOverP_ba->Write() ;
	res_vs_SinOverP_bc->Write() ;
	res_vs_SinOverP_ea->Write() ;
	res_vs_SinOverP_ec->Write() ;

	tres_vs_CosOverP_trt->Write();
	tres_vs_CosOverP_ba->Write() ;
	tres_vs_CosOverP_bc->Write() ;
	tres_vs_CosOverP_ea->Write() ;
	tres_vs_CosOverP_ec->Write() ;

	res_vs_CosOverP_trt->Write();
	res_vs_CosOverP_ba->Write() ;
	res_vs_CosOverP_bc->Write() ;
	res_vs_CosOverP_ea->Write() ;
	res_vs_CosOverP_ec->Write() ;




	ttfile->Write();
	ttfile->Close();
      }
      
      
    //*********************************************************************
    //  SCANNING BINARY FILE     
    } else {
    //*********************************************************************
      
      cout << "SCANNING BINARY HISTOGRAM FILE " << argv[ifiles+2] << endl;
      
      nhistsadd=0;
      nhitsadd=0;
      nhists=0;

      ifstream * ifile = new ifstream(argv[ifiles+2], ios::in | ios::binary);
      
      //histogram loop
      while(true){
	
	//reading histogram
	ifile->read ((char*)&npop,sizeof(int));
	if (ifile->eof()) break;
	int* chist=new int[2*npop];
	if (npop>0) ifile->read ((char*)chist, sizeof(int)*2*npop);
	ifile->read ((char*)&sid,sizeof(int));

	
	if(histmap.find(sid) == histmap.end()){ //create histogram if seen for the first time
	  //cppcheck-suppress stlFindInsert
	  histmap[sid] = new CalHist();
	  nhistsadd++;
	}    
	
	for(int ipop=0;ipop<2*npop;ipop=ipop+2) {
	  histmap[sid]->IncreaseBin((short)chist[ipop],(unsigned short)chist[ipop+1]); //increase the bins 
	  if (chist[ipop]<100) { ntres+=chist[ipop+1]; nhitsadd+=chist[ipop+1]; } //increase hit counters
	  else if (chist[ipop]>=100 && chist[ipop]<200) nres+=chist[ipop+1];
	  else nrt+=chist[ipop+1];
	}
	
	nhists++;

        if (nhists%10000==0){
	  process_mem_usage(vm, rss);
	  printf("%i HISTOGRAMS READ! %i HISTOGRAMS ADDED! %i BINS ADDED! MAXVALUE: %i VM: %0f RSS: %0f\n",nhists,nhistsadd,nbinsadd,maxvalue,vm,rss);
	}

	delete chist;

      }
      
      delete ifile;
      
      cout << nhists << " HISTOGRAMS SACNNED" << endl;
      cout << nhistsadd << " HISTOGRAMS ADDED" << endl;

    }// end of if (root or binary histograms)
  }// end of file loop 

  cout << endl;
  cout << "TOT HISTOGRAMS............. " << nhists << endl;
  cout << "TOT HITS IN TIMERES HIST .. " << ntres << " (" << 100*float(ntres)/float(nhits) << "%)" << endl;
  cout << "TOT HITS IN RRES HIST ..... " << nres  << " (" << 100*float(nres)/float(nhits) << "%)" << endl;
  cout << "TOT HITS IN RT HIST ....... " << nrt   << " (" << 100*float(nrt)/float(nhits) << "%)" << endl;
  cout << endl;

  ofilestat << "TRESHITS " << ntres << endl;
  ofilestat.close();

  dump_hists(&histmap, ntbins, nrbins, ntres, argv[1], 0) ;
  dump_tracktuple(&trackmap);

  return 0;

}

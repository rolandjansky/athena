/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonFixedId.h"
#include "Tools.h"

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TAxis.h"
#include "TH1.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TSystem.h"

#include <sstream>
#include <iostream>
#include <cmath>

#include <map>

using namespace std;
using namespace MuonCalib;

const int  NITER_MAX=20;

bool getTotalChi2(int iter, TTree* tree,
		  std::map<long long int,int*>& ntracksMap,
		  std::map<long long int,int*>& ndofMap,
		  std::map<long long int,double*>& chi2Map);

void help(const char *argv0) {
    
  cerr << "Usage: " << argv0 << " [options] " << endl
       << "Options:" << endl
       << " -o :\tOutput path"  << endl
       << " -i :\tInput files" << endl
       << " -r :\tRootname" << endl
       << " -e :\tDraw errors" << endl
       << " -c :\tCombine chambers in a tower" << endl
       << " -l :\tL1 alignment" << endl
       << " -h :\tShow this message and quit"  << endl
       << endl;
  exit(1);
}

int main(int argc, char** argv) {

  //----------------------------
  //-- Parse the command line --
  //---------------------------- 
  
  extern char *optarg;
  char c;
  TString inputdir("");
  TString outputdir("");
  TString rootname("");
  bool DRAWERR     = false;
  bool COMBINE     = false;
  bool DOALLPLOTS  = false;
  bool L1ALIGNMENT = false;
  bool INITMISAL   = false;

  while((c = getopt(argc, argv, "-heclo:i:r:")) !=-1){
    switch(c){
    case 'o': 
      outputdir = optarg;
      break;
    case 'i':
      inputdir = optarg;
      break;
    case 'r':
      rootname = optarg;
      break;
    case 'e':
      DRAWERR = true;
      break;
    case 'c':
      COMBINE = true;
      break;
    case 'l':
      L1ALIGNMENT = true;
      break;
    default:
      cerr << "Help or unknown argument!" << endl;
      help(argv[0]);
      break; 
    }
  }
  if(rootname==""){
    cerr << "no rootname passed, exiting..." << endl;
    exit(1);
  } 
  if(inputdir==""){
    inputdir = rootname;
  } 
  if(outputdir==""){
    outputdir = inputdir;
  }
 
  //---------
  //-- Run --
  //--------- 
  
  Tools::setStyle();
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetTitleYOffset(1.65);
  
  double tsize=0.05;
  gStyle->SetTextSize(tsize);
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");

  // int font=42;
  //  gStyle->SetTextFont(font);
  //   gStyle->SetLabelFont(font,"x");
  //   gStyle->SetTitleFont(font,"x");
  //   gStyle->SetLabelFont(font,"y");
  //   gStyle->SetTitleFont(font,"y")

  //  std::string directory="";
  //   if (argc>1) {
  //     directory=string(argv[1]);
  //   }
  //   else {
  //     cout<<"please give input directory"<<endl;
  //     exit(3);
  //   }
  //   std::cout<<"using input directory "<<directory<<std::endl;

  //const bool DRAWERR = 0;
  //const bool COMBINE = 0; 

  string ytitles[6]={"#Deltas","#Deltaz","#Deltat",
                     "#Delta#alpha(s)","#Delta#beta(z)", "#Delta#gamma(t)"};
  string units[6]={" [mm]"," [mm]"," [mm]"," [mrad]"," [mrad]"," [mrad]"};
  if (L1ALIGNMENT) {
    ytitles[0]="#Deltax";
    ytitles[1]="#Deltay";
    ytitles[2]="#Deltaz";
    ytitles[3]="#Delta#alpha(x)";
    ytitles[4]="#Delta#beta(y)";
    ytitles[5]="#Delta#gamma(z)";
  }

  bool activeParam[6] = {1,1,1,1,1,1}; 
  //bool activeParam[6] = {1,1,1,0,0,0}; 

  map<long long int,double**> yArrMap;
  map<long long int,double**> yErrMap;
  map<long long int,double*>  minMap;
  map<long long int,double*>  maxMap;

  map<long long int,double*>  chi2Map;
  map<long long int,int*>     nDoFMap;
  map<long long int,int*>     nTracksMap;

  double* xArr = new double[NITER_MAX+1];
  double* xErr = new double[NITER_MAX+1];
  int niter=0;

  void *directory = gSystem->OpenDirectory(inputdir);
  if (!directory){
    cerr << "input directory does not exist, exiting..." << endl;
    exit(1);
  } 
  directory = gSystem->OpenDirectory(outputdir);
  if (!directory){
    cerr << "create output directory" << endl;
    int status = gSystem->mkdir(outputdir,false);
    if(status!=0){
      cerr << "could not create output directory, exiting..." << endl;
      exit(1);
    }
  } 

  TString iterdir = inputdir+"/iter0";
  directory = gSystem->OpenDirectory(inputdir+"/iter0");
  bool multiplejobs = directory ? true : false; 


  //----------------------------
  //-- Get info about iterations
  //----------------------------
  bool lastfile=false;
  for (int iter=0;iter<NITER_MAX+1;iter++) {
    
    int fileindex = (iter==0) ? 0 : iter-1;
    TString filename;
    if(multiplejobs){
      TString iterdir = inputdir+Form("/iter%i/",fileindex);
      directory = gSystem->OpenDirectory(iterdir);
      filename = iterdir;
      if(!directory) break;
    }
    else{
      filename = inputdir+"/";
    }
    
    filename += "MuonAlign_"+rootname+"_iter";
    filename += Form("%i",fileindex);
       
    if(multiplejobs) filename += "_Solve";
    filename += ".root";

    //stringstream filename;
    //filename<<inputdir<<"/MuonAlign_"<<rootname<<"_iter";
    //int fileindex = (iter==0) ? iter+1 : iter;
    //filename<<int(fileindex);
    
    TFile* file=new TFile(filename,"READ");
    std::cout<<"got TFile "<<filename<<", getting tree"<<std::endl;
    TTree* tree=(TTree*)file->Get("alignParMDT");

    if(!tree){ 
      if(iter==0){
        cout << "-> no tree to run on in file "<<filename<<", exit..." << endl;
        exit(1);
      }
      else{
        cout << "-> don't worry: still running fine" << endl;
        break;
      }
    }
    
    TString basefilename;
    if(multiplejobs){
      TString iterdir = inputdir+Form("/iter%i/",iter);
      directory = gSystem->OpenDirectory(iterdir);
      basefilename = iterdir;
      if (!directory) {
	cerr << "iteration: " << iter << " does not exist..." << endl;
	lastfile=true;
      } 
    }
    else{
      basefilename = inputdir+"/";  
    }
    
    basefilename += "MuonAlign_"+rootname+Form("_iter%i",fileindex);
    TString muonalign_filename = basefilename;
    if(multiplejobs)   
      muonalign_filename += "_index*";  
    muonalign_filename += ".root";
    
    TChain* muonaligntree=new TChain("alignParMDT");
    std::cout<<"adding to alignPar chain: "<<muonalign_filename <<std::endl;
    Int_t nfilesadded=muonaligntree->Add(muonalign_filename);

    if (nfilesadded>0) {
      bool success=getTotalChi2(iter,muonaligntree,nTracksMap,nDoFMap,chi2Map);
      if (!success) {
	cout<<"didn't get total chi2, continuing"<<endl;
	continue;
      }
    }
    niter++;

    xArr[iter]=(double)(iter);
    xErr[iter]=0.;

    long long int chamberId;
    int    nDoF;
    double chi2;
    double init_tras, init_traz, init_trat;
    double init_rots, init_rotz, init_rott;
    double err_tras, err_traz, err_trat;
    double err_rots, err_rotz, err_rott;
    double tras,traz,trat,rots,rotz,rott;

    tree->SetBranchAddress("chamberId",  &chamberId);
    tree->SetBranchAddress("nDoF",       &nDoF);
    tree->SetBranchAddress("trackChi2",  &chi2);
    tree->SetBranchAddress("delta_tras", &tras);
    tree->SetBranchAddress("delta_traz", &traz);
    tree->SetBranchAddress("delta_trat", &trat);
    tree->SetBranchAddress("delta_rots", &rots);
    tree->SetBranchAddress("delta_rotz", &rotz);
    tree->SetBranchAddress("delta_rott", &rott);
    tree->SetBranchAddress("init_tras",  &init_tras);
    tree->SetBranchAddress("init_traz",  &init_traz);
    tree->SetBranchAddress("init_trat",  &init_trat);
    tree->SetBranchAddress("init_rots",  &init_rots);
    tree->SetBranchAddress("init_rotz",  &init_rotz);
    tree->SetBranchAddress("init_rott",  &init_rott);
    tree->SetBranchAddress("err_tras",   &err_tras);
    tree->SetBranchAddress("err_traz",   &err_traz);
    tree->SetBranchAddress("err_trat",   &err_trat);
    tree->SetBranchAddress("err_rots",   &err_rots);
    tree->SetBranchAddress("err_rotz",   &err_rotz);
    tree->SetBranchAddress("err_rott",   &err_rott);

    for (int ientry=0;ientry<(int)tree->GetEntries();ientry++) {

      tree->GetEntry(ientry);
      
      if(ientry==0){
        if(tras==0 && err_tras==0) activeParam[0]=false;
        if(traz==0 && err_traz==0) activeParam[1]=false;
        if(trat==0 && err_trat==0) activeParam[2]=false;
        if(rots==0 && err_rots==0) activeParam[3]=false;
        if(rotz==0 && err_rotz==0) activeParam[4]=false;
        if(rott==0 && err_rott==0) activeParam[5]=false;
      }
            
      double** yArr(0);
      double** yErr(0);
      double*  min(0);
      double*  max(0);
      double*  chi2Arr(0);
      double*  nDoFArr(0);

      if(iter==0) {
        yArr=new double*[6];  
        yArr[0]=new double[NITER_MAX+1];
        yArr[1]=new double[NITER_MAX+1];
        yArr[2]=new double[NITER_MAX+1];
        yArr[3]=new double[NITER_MAX+1];
        yArr[4]=new double[NITER_MAX+1];
        yArr[5]=new double[NITER_MAX+1];

        yErr=new double*[6];  
        yErr[0]=new double[NITER_MAX+1];
        yErr[1]=new double[NITER_MAX+1];
        yErr[2]=new double[NITER_MAX+1];
        yErr[3]=new double[NITER_MAX+1];
        yErr[4]=new double[NITER_MAX+1];
        yErr[5]=new double[NITER_MAX+1];

	nDoFArr=new double[NITER_MAX+1];
	chi2Arr   =new double[NITER_MAX+1];
          
        min=new double[6];
        max=new double[6];
          
        for (int i=0;i<6;i++) {
          yErr[i][0]=0.;
          min[i]=1.e6;
          max[i]=-1.e6;
        }
          
        yArrMap[chamberId]    = yArr;
        yErrMap[chamberId]    = yErr;
        minMap[chamberId]     = min;
        maxMap[chamberId]     = max;

      }
      else{
        yArr = yArrMap[chamberId];
        yErr = yErrMap[chamberId];
        min  = minMap[chamberId];
        max  = maxMap[chamberId];
      }
      //if (chamberId!=240131072)  continue; //EIL2A1 
      
      if(iter==0){
	cout<<"iter 0, init_trat="<<init_trat<<endl;
        yArr[0][iter]=init_tras;
        yArr[1][iter]=init_traz;
        yArr[2][iter]=init_trat;
        yArr[3][iter]=init_rots*1000;
        yArr[4][iter]=init_rotz*1000;
        yArr[5][iter]=init_rott*1000;
	if (L1ALIGNMENT && INITMISAL) {
	  yArr[0][0]+= 2.;
	  yArr[1][0]+= 1.;
	  yArr[2][0]+=- .5;
	  yArr[3][0]+= .01;
	  yArr[4][0]+=-.03;
	  yArr[5][0]+=- .2;	  
	}
      }
      else{
	cout<<"iter "<<iter<<", init_trat="<<init_trat<<endl;
	cout<<"init_trat+delta="<<yArr[0][iter-1]+trat<<endl;
	yArr[0][iter]= yArr[0][iter-1] + tras;
	yArr[1][iter]= yArr[1][iter-1] + traz;
	yArr[2][iter]= yArr[2][iter-1] + trat;
	yArr[3][iter]= yArr[3][iter-1] + rots*1000.;
	yArr[4][iter]= yArr[4][iter-1] + rotz*1000.;
	yArr[5][iter]= yArr[5][iter-1] + rott*1000.;  
	if (L1ALIGNMENT && INITMISAL && iter==2) {
	  yArr[0][2]-= 2.;
	  yArr[1][2]-= 1.;
	  yArr[2][2]-=- .5;
	  yArr[3][2]-= .01;
	  yArr[4][2]-=-.03;
	  yArr[5][2]-=- .2;	  
	}
	/*	
                yArr[0][iter]= tras;
                yArr[1][iter]= traz;
                yArr[2][iter]= trat;
                yArr[3][iter]= rots*1000.;
                yArr[4][iter]= rotz*1000.;
                yArr[5][iter]= rott*1000.; */ 
	yErr[0][iter]= err_tras;
	yErr[1][iter]= err_traz;
	yErr[2][iter]= err_trat;
	yErr[3][iter]= err_rots*1000.*1.75;
	yErr[4][iter]= err_rotz*1000.*1.7;
	yErr[5][iter]= err_rott*1000.*2.44;
      }
      //chi2Arr[iter]=chi2;
      //nTracksArr[iter]=nTracks;
      //cout<<"tracks/chi2="<<nTracks<<"/"<<chi2<<endl;

      for (int i=0;i<6;i++) {

        if(!activeParam[i]) continue; 
          
        if(!DRAWERR){
          if (yArr[i][iter]<min[i]) min[i]=yArr[i][iter];
          if (yArr[i][iter]>max[i]) max[i]=yArr[i][iter];
        }
        else{
          if ((yArr[i][iter]-yErr[i][iter])<min[i]) min[i]=yArr[i][iter]-yErr[i][iter];
          if ((yArr[i][iter]+yErr[i][iter])>max[i]) max[i]=yArr[i][iter]+yErr[i][iter];
        }
      }
    }

    delete file;
    file=0;
  }
 
  //-------------
  //-- Make plots
  //-------------

  MuonFixedId* mid=new MuonFixedId(0);
  map<long long int,double**>::const_iterator yArrIt    = yArrMap.begin(); 
  map<long long int,double**>::const_iterator yArrEnd   = yArrMap.end();
  map<long long int,double**>::const_iterator yErrIt    = yErrMap.begin(); 
  map<long long int,double*>::const_iterator  minIt     = minMap.begin(); 
  map<long long int,double*>::const_iterator  maxIt     = maxMap.begin(); 
  
  const int NITER = niter-1;

  map<string,vector<TGraph**>*> graphMap;
  map<string,vector<string> >   graphNamesMap;
  map<string,double*>   graphMinMap;
  map<string,double*>   graphMaxMap;

  for (; yArrIt!=yArrEnd; yArrIt++,yErrIt++,minIt++,maxIt++) {

    int chamberId = yArrIt->first;
    mid->setMuonIdentifier(chamberId);
    string chamberName;
    if (chamberId>2) chamberName = Tools::getChamberName(mid);
    else {
      if (chamberId==0) chamberName="endcapA";
      else if (chamberId==1) chamberName="endcapC";
      else if (chamberId==2) chamberName="barrel";
    }
      
    double** yArr = yArrIt->second;
    double** yErr = yErrIt->second;
    double*  min  = minIt->second;
    double*  max  = maxIt->second;

    TCanvas* canv=new TCanvas("canv"+TString(chamberName),"canv");
    //canv->SetFillColor(10);
    canv->Divide(3,2);
    TGraph** gr     =new TGraph*[6];      
    TH1F**  hist=new TH1F*[6];
    double value[6]={0.,0.,0.,0.,0.,0.};
    if(chamberName=="EIL2A1"){
      value[0] = 2.155;
      value[1] = .546;
      value[2] = -.283;
      value[3] = -.445;
      value[4] = 2.23;
      value[5] = .517;
    }
    
    //if plots will be combined, plot difference to nominal position
    if(COMBINE){
      for (int i=0;i<6;i++) {
        for (int iter=0;iter<niter;iter++){
          yArr[i][iter] -= value[i];
        }
        min[i] -= value[i];
        max[i] -= value[i];
        value[i] = 0.;
      }
    }

    for (int i=0;i<6;i++) {
      for (int iter=0;iter<niter;iter++){
	cout<<"yArr["<<i<<"]["<<iter<<"]="<<yArr[i][iter]<<endl;
      }
    }

    //string names[6]={"S", "Z", "T", "rot_S", "rot_Z", "rot_T"};
    TLine** line=new TLine*[6];
    for (int i=0;i<6;i++) {
      line[i]=new TLine(xArr[0],value[i],xArr[NITER],value[i]);
      if (value[i]<min[i]) min[i]=value[i];
      if (value[i]>max[i]) max[i]=value[i];
    }
      
    for (int i=0;i<6;i++) {
      //for (int i=2;i<3;i++) {
      if(!activeParam[i]) continue; 
      canv->cd(i+1);
           
      double currentmin=(min[i]<0.)?1.1*min[i]:.8*min[i];
      double currentmax=(max[i]>0.)?1.1*max[i]:.8*max[i];
      hist[i]=canv->cd(i+1)->DrawFrame(xArr[0],currentmin,xArr[NITER],currentmax);   
      string title = chamberName;// + " - " + names[i];
      hist[i]->SetTitle(title.c_str());
      hist[i]->GetXaxis()->SetTitle("Iterations");
          
      string ytitle = ytitles[i]+ units[i];
      hist[i]->GetYaxis()->SetTitle(ytitle.c_str());

      canv->cd(i+1)->Modified();
          
      if(!DRAWERR) gr[i]=new TGraph      (NITER+1,xArr,yArr[i]);
      else         gr[i]=new TGraphErrors(NITER+1,xArr,yArr[i],xErr,yErr[i]);
          
      gr[i]->SetMarkerSize(0.8);
      gr[i]->SetMarkerStyle(8);
      gr[i]->SetLineWidth(2);
      gr[i]->Draw("PLsame");
         
      line[i]->SetLineColor(kRed);
      line[i]->Draw("same");

 
    }
    stringstream canvname;
    canvname<<outputdir<<"/iterations_"<<rootname;
    if((int)yArrMap.size()>1){
      canvname<<"_"<<chamberName;
    }          
    canvname<<".eps";
    canv->Print(canvname.str().c_str());

    if(COMBINE) {
      string towerName = chamberName;
      towerName.replace(1,1,"");
      if(!graphMap[towerName]){
        graphMap[towerName] = new vector<TGraph**>(0);
        graphNamesMap[towerName] = vector<string>(0);
        graphMinMap[towerName] = min;
        graphMaxMap[towerName] = max;
      }
      else{
        double* grMin = graphMinMap[towerName];
        double* grMax = graphMaxMap[towerName];
        for (int i=0;i<6;i++) {
          if(min[i]<grMin[i]) grMin[i]=min[i];
          if(max[i]>grMax[i]) grMax[i]=max[i];
        } 
      }
      graphMap[towerName]->push_back(gr);
      graphNamesMap[towerName].push_back(chamberName);
    }

    if (!COMBINE) delete [] gr;
    delete [] line;
    
    delete canv;
  }//for yArrIt
  
  
  //---------------
  // Make more plots
  //---------------
  
  if(DOALLPLOTS){
    yArrIt = yArrMap.begin(); 
    map<long long int,double*> ::const_iterator chi2It    = chi2Map.begin(); 
    map<long long int,int*> ::const_iterator    nTracksIt = nTracksMap.begin(); 
    map<long long int,int*> ::const_iterator    nDoFIt    = nDoFMap.begin(); 
    
    for (; yArrIt!=yArrEnd; yArrIt++,nTracksIt++,chi2It++,nDoFIt++) {
      int chamberId = yArrIt->first;
      mid->setMuonIdentifier(chamberId);
      string chamberName;
      if (chamberId>2) chamberName = Tools::getChamberName(mid);
      else {
        if (chamberId==0) chamberName="endcapA";
        else if (chamberId==1) chamberName="endcapC";
        else if (chamberId==2) chamberName="barrel";
      }
      int*  nDoFArr= nDoFIt->second;
      double* dDoFArr=new double[NITER+1];
      for (int iter=0;iter<niter;iter++) {
        dDoFArr[iter]=(double)nDoFArr[iter];
      }
      
      double* totalchi2Arr = chi2It->second;
      double* chi2dofArr = new double[NITER+1];
      for (int iter=0;iter<niter;iter++) {
        if (nDoFArr[iter]>0) { 
          chi2dofArr[iter]=totalchi2Arr[iter]/(double)nDoFArr[iter];
          cout<<"chi2dofArr["<<iter<<"]="<<chi2dofArr[iter]<<endl;
          cout<<"totalchi2Arr="<<totalchi2Arr[iter]<<", nDoF="<<nDoFArr[iter]<<endl;
        }
      }
    
      TCanvas* canvnTrks =new TCanvas("canv"+TString(chamberName)+"_ntrks","canv");
      int* ntracksArr=nTracksIt->second;
      double* dtracksArr=new double[NITER+1];
      for (int iter=0;iter<niter;iter++) {
        dtracksArr[iter]=(double)ntracksArr[iter];
      }
      TGraph* grNTrks=new TGraph(niter-1,xArr,dtracksArr);
      grNTrks->SetMarkerSize(0.8);
      grNTrks->SetMarkerStyle(8);
      grNTrks->SetLineWidth(2);
      grNTrks->Draw("APL");
      grNTrks->GetXaxis()->SetTitle("iterations");
      grNTrks->GetYaxis()->SetTitle("number tracks");
      stringstream canvname;
      canvname<<outputdir<<"/iterations_ntrks_"<<rootname;
      if((int)yArrMap.size()>1){
        canvname<<"_"<<chamberName;
      }          
      canvname<<".eps";
      canvnTrks->Print(canvname.str().c_str());
      
      TCanvas* canvChi2=new TCanvas("canv"+TString(chamberName)+"_chi2","canv");
      
      TGraph* grChi2 =new TGraph(niter-1,xArr,chi2dofArr);
      grChi2->SetMarkerSize(0.8);
      grChi2->SetMarkerStyle(8);
      grChi2->SetLineWidth(2);
      grChi2->Draw("APL");
      grChi2->GetXaxis()->SetTitle("iterations");
      grChi2->GetYaxis()->SetTitle("track #chi^{2}/NDoF");
      stringstream canvnamechi2;
      canvnamechi2<<outputdir<<"/iterations_chi2_"<<rootname;
      if((int)yArrMap.size()>1){
        canvnamechi2<<"_"<<chamberName;
      }          
      canvnamechi2<<".eps";
      canvChi2->Print(canvnamechi2.str().c_str());    
    }
  } 
  
  if (!COMBINE) return 0;

  map<string,vector<TGraph**>*>::const_iterator graphIt  = graphMap.begin();
  map<string,vector<TGraph**>*>::const_iterator graphEnd = graphMap.end();
  map<string,vector<string> >::const_iterator graphNamesIt = graphNamesMap.begin();
  map<string,double*>::const_iterator graphMinIt = graphMinMap.begin();
  map<string,double*>::const_iterator graphMaxIt = graphMaxMap.begin();
  
  TLine* line = new TLine(xArr[0],0,xArr[NITER],0);
  line->SetLineColor(kRed);
  //line->SetLineColor(14);
  //line->SetLineStyle(2);
  //int markerstyles[3] = {8,4,21};
  int markerstyles[3] = {8,22,21};

  for (; graphIt!=graphEnd; graphIt++,graphMinIt++,graphMaxIt++,graphNamesIt++) {
    
    string name = graphIt->first;
    vector<TGraph**>* graphs=graphIt->second;
    
    TH1F**  hist=new TH1F*[6];
    TCanvas* canv=new TCanvas("canv"+TString(name),"canv");
    double shift = 0.009;
    TLegend* leg = new TLegend(0.1+shift, 0.67, 0.25+shift, 0.82);
    //TLegend* leg = new TLegend(0.1, 0.65, 0.2, 0.85);
    canv->Divide(3,2);
    
    for(unsigned int iGr=0;iGr<graphs->size();iGr++){
      for (int i=1;i<6;i++) {
	if(!activeParam[i]) continue; 
        canv->cd(i+1);
	
	if(iGr==0){
	  double min = graphMinIt->second[i];
	  double max = graphMaxIt->second[i];
	  double currentmin=(min<0.)?1.1*min:.8*min;
	  double currentmax=(max>0.)?1.1*max:.8*max;
	  hist[i]=canv->cd(i+1)->DrawFrame(xArr[0],currentmin,xArr[NITER],currentmax);   
	  hist[i]->GetXaxis()->SetTitle("Iterations");
	  string ytitle = ytitles[i]+ units[i];
	  hist[i]->GetYaxis()->SetTitle(ytitle.c_str());
          line->Draw("same");
	}
	
	TGraph* gr = graphs->at(iGr)[i];
        int color = iGr==0 ? iGr+1 : iGr+2;
	gr->SetLineColor(color);
	gr->SetMarkerColor(color);
	gr->SetMarkerStyle(markerstyles[iGr%3]);
	gr->Draw("PLsame");
	
	if(i==1){
	  leg->AddEntry(gr,graphNamesIt->second[iGr].c_str(),"PL"); 
	}
      }
      
      if(iGr==graphs->size()-1){
        canv->cd(0);
	leg->SetBorderSize(0);
	leg->SetFillColor(0);
	leg->Draw();
	
	stringstream canvname;
	canvname<<outputdir<<"/iterations_"<<rootname;
	canvname<<"_"<<name<<"combined";
	canvname<<".eps";
	canv->Print(canvname.str().c_str());
      }
    }
    delete [] hist;
    delete canv;
    delete leg;
    for(unsigned int iGr=0;iGr<graphs->size();iGr++){
      for (int i=1;i<6;i++) {
        if(!activeParam[i]) continue; 
        delete graphs->at(iGr)[i];
      }
    }
    
  }
  delete line;
    
  return 0;
  
}

//________________________________________________________________________
bool getTotalChi2(int iter, TTree* tree, 
		  std::map<long long int,int*>& ntracksMap, 
		  std::map<long long int,int*>& ndofMap, 
		  std::map<long long int, double*>& totalchi2Map)
{
  long long int chamberId;
  int nDoF,nTracks;
  double chi2;

  tree->SetBranchAddress("chamberId",  &chamberId);
  tree->SetBranchAddress("nTracks",    &nTracks);
  tree->SetBranchAddress("nDoF",       &nDoF);
  tree->SetBranchAddress("trackChi2",  &chi2);

  if (tree->GetEntries()<1) return false;

  for (int ientry=0;ientry<(int)tree->GetEntries();ientry++) {
    tree->GetEntry(ientry);

    std::map<long long int,int*>::const_iterator it=ndofMap.find(chamberId);
    if (it==ndofMap.end()) {
      ntracksMap[chamberId]  =new int   [NITER_MAX];
      ndofMap[chamberId]     =new int   [NITER_MAX];
      totalchi2Map[chamberId]=new double[NITER_MAX];
      for (int i=0;i<NITER_MAX;i++) {
	ntracksMap[chamberId][i]=0;
	ndofMap[chamberId][i]=0;
	totalchi2Map[chamberId][i]=0.;
      }
      ntracksMap  [chamberId][iter]=nTracks;
      ndofMap     [chamberId][iter]=nDoF;
      cout<<"chi2="<<chi2<<endl;
      totalchi2Map[chamberId][iter]=chi2;
      cout<<" totalchi2Map["<<chamberId<<"]["<<iter<<"]="<<totalchi2Map[chamberId][iter]<<endl;
    }
    else {
      ntracksMap  [chamberId][iter]+=nTracks;
      ndofMap     [chamberId][iter]+=nDoF;
      totalchi2Map[chamberId][iter]+=chi2;
      cout<<"+totalchi2Map["<<chamberId<<"]["<<iter<<"]="<<totalchi2Map[chamberId][iter]<<endl;
    }
  }

  return true;
}

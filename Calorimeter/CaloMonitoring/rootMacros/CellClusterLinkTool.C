/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

 //////////////////////////////////////////////////////////////////////////////////////
 // CellClusterLinkTool.C : a standalone DQ monitoring tool by using cluster         //
 //                         and cell plots                                           //
 // Author: FENG TIAN (Columbia University)                                          //
 // Email : Feng.Tian@cern.ch                                                        //
 // usage: 									     //
 //   1. compile : 								     //
 //      g++ `root-config --cflags --glibs` -o CellClusterLink CellClusterLinkTool.C //
 //   2. run :           							     //
 //      ./CellClusterLinkTool Run# tag stream data10_7TeV(or data10_hi)             //
 //////////////////////////////////////////////////////////////////////////////////////

#include<string>
#include<iostream>
#include"TString.h"
#include"TFile.h"
#include"TList.h"
#include"TH2F.h"
#include"TCollection.h"
#include"CellClusterLinkTool.h"
#include<math.h>
#include<fstream>
#include<algorithm>
#include<stdio.h>
#include<list>
using namespace std;
bool mySortfunc(clusterbin i,clusterbin j){
   return (i.m_value > j.m_value);
}
string TranslatePartition(int partition){
   switch(partition){
    case EMBA:  return "EMBA"; break;
    case EMECA: return "EMECA";break;
    case HECA:  return "HECA"; break;
    case FCALA: return "FCALA";break;
    case EMBC:  return "EMBC"; break;
    case EMECC: return "EMECC";break;
    case HECC:  return "HECC"; break;
    case FCALC: return "FCALC";break;
   }
}
string TranslateLayer(int layer){
 switch(layer){
  case 0: return "Presampler"; break;
  case 1: return "Sampling1"; break;
  case 2: return "Sampling2"; break;
  case 3: return "Sampling3"; break;
 }
}


void find_n(string name,int& partition,int& layer){
 size_t foundb;
 size_t founde;
 foundb=name.find_first_of("_");
 founde=name.find_last_of("_");
 string extract_name = name.substr(foundb+1,founde-foundb-1);
 int Namelength=extract_name.length();
 if(extract_name.find("EMB")!=string::npos){
     if(extract_name[Namelength-1]=='A') partition=EMBA;
     else if(extract_name[Namelength-1]=='C') partition = EMBC;
 }
 if(extract_name.find("HEC")!=string::npos){
     if(extract_name[Namelength-1]=='A') partition=HECA;
     else if(extract_name[Namelength-1]=='C')
             partition = HECC;
 }
if(extract_name.find("EMEC")!=string::npos){
     if(extract_name[Namelength-1]=='A') partition=EMECA;
     else if(extract_name[Namelength-1]=='C') partition = EMECC;
}
if(extract_name.find("FCAL")!=string::npos){
     if(extract_name[Namelength-1]=='A') partition=FCALA;
     else if(extract_name[Namelength-1]=='C')  partition = FCALC;
}
switch(extract_name[Namelength-2]){
   case 'P': layer = 0; break;
   case '0': layer = 0; break;
   case '1': layer = 1; break;
   case '2': layer = 2; break;
   case '3': layer = 3; break;
}
}
void FindStripMedian(vector<double> onestrip_tmp,vector<double>& stripsMedian){
  double median=0;
   sort(onestrip_tmp.begin(),onestrip_tmp.end());
   int index1=onestrip_tmp.size()/4;
   int index2=onestrip_tmp.size()/2;
   int index3=3*onestrip_tmp.size()/4;
   median = (onestrip_tmp[index1]+onestrip_tmp[index2]+onestrip_tmp[index3])/3.0;
   stripsMedian.push_back(median);
}

void ProcessByBinsDiffFromStripMedian(TH2F*& histo,vector<clusterbin>& problematicClusters){
 std::string histoName = histo->GetName();
 for(int part=CaloB;part<NbCaloPart;part++){
  std::vector<double> stripsMedian;
  std::vector<double> stripsAvg;
  std::vector<double> stripsVariance;
  double maxInMap=0;
  int range0=histo->GetXaxis()->FindBin(m_range[part][0]);
  int range1=histo->GetXaxis()->FindBin(m_range[part][1]);
  int range2=0;
  int range3=histo->GetYaxis()->GetNbins();
  for ( int i=range0 ; i <=range1 ; ++i ) {
    std::vector<double> onestrip;
    double stripSum=0;
    for ( int j=range2 ; j <=range3; ++j ) {
      float binvalue = histo->GetBinContent(i,j);
      onestrip.push_back(binvalue);
      stripSum += binvalue;
      if(binvalue > maxInMap) {
       maxInMap = binvalue;
      }
    }
    stripsAvg.push_back(stripSum/onestrip.size());
    FindStripMedian(onestrip,stripsMedian);
  }
  for ( int i = range0; i <= range1; ++i ) {
   float sumdiff2=0;
   int counter=0;
   for ( int j = range2; j <= range3; ++j ) {
     double binvalue = histo->GetBinContent(i,j);
     double diff=binvalue-stripsAvg[i-range0];
     sumdiff2 +=pow(diff,2);
     counter++;
   }
   double variance=-1;
   if(counter!=0) variance = sumdiff2 / counter ;
   stripsVariance.push_back(variance);
  }

  std::vector<clusterbin> redbins;
  for ( int k = range0; k <= range1; ++k ) {
    for ( int l = range2; l <= range3; ++l ) {   
      double binvalue = histo->GetBinContent(k,l);
      double strip_median = stripsMedian[k-range0];  
      if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]==0) continue; // skip empty strip    
      else if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]!=0
              && stripsAvg[k-range0]!=0) strip_median = stripsAvg[k-range0];
           else if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]!=0
                    && stripsAvg[k-range0]==0) continue;
      double outstandingRatio=0;
      if(fabs(strip_median) > 0.00001 )
         outstandingRatio= binvalue/strip_median;
      else continue;
      double eta = histo->GetXaxis()->GetBinCenter(k);
      double phi = histo->GetYaxis()->GetBinCenter(l);
      clusterbin onebin;
      onebin.m_eta = eta;
      onebin.m_phi = phi;
      onebin.m_value = binvalue;
      onebin.m_OSRatio = outstandingRatio;
      onebin.m_nx = 0;
      onebin.m_ny = 0;
      if(fabs(outstandingRatio) > RedThreshold_cluster) {
       redbins.push_back(onebin);
     }
    }
  }

  std::sort(redbins.begin(),redbins.end(),mySortfunc);
for(unsigned int i=0;i<redbins.size();i++){
   if(redbins[i].m_value / maxInMap < 0.15 ) continue;
   bool match=false;
   clusterbin onebin;
   onebin.m_eta = redbins[i].m_eta;
   onebin.m_phi = redbins[i].m_phi;
   onebin.m_value = redbins[i].m_value;
   onebin.m_level = redbins[i].m_OSRatio>6*RedThreshold_cluster? "**":"*" ; 
   problematicClusters.push_back(onebin);
 }
}
}
void ProcessByBinsDiffFromStripMedian(TH2F*& h1 ,TH2F*& h2){
  if(h1==NULL || h2==NULL ) return;
  std::vector<double> stripsMedian;
  std::vector<double> stripsAvg;
  std::vector<double> stripsVariance;
  double maxInMap=0;
  int range0=0;
  int range1=h1->GetXaxis()->GetNbins();
  int range2=0;
  int range3=h1->GetYaxis()->GetNbins();
  for ( int i=range0 ; i <=range1 ; ++i ) {
    std::vector<double> onestrip;
    double stripSum=0;
    for ( int j=range2 ; j <=range3; ++j ) {
      float binvalue = h1->GetBinContent(i,j);
      onestrip.push_back(binvalue);
      stripSum += binvalue;
      if(binvalue > maxInMap) {
       maxInMap = binvalue;
      }
    }
    stripsAvg.push_back(stripSum/onestrip.size());
    FindStripMedian(onestrip,stripsMedian);
  }
  for ( int i = range0; i <= range1; ++i ) {
   float sumdiff2=0;
   int counter=0;
   for ( int j = range2; j <= range3; ++j ) {
     double binvalue = h1->GetBinContent(i,j);
     double diff=binvalue-stripsAvg[i-range0];
     sumdiff2 +=pow(diff,2);
     counter++;
   }
   double variance=-1;
   if(counter!=0) variance = sumdiff2 / counter ;
   stripsVariance.push_back(variance);
  }

  std::vector<clusterbin> redbins;
  for ( int k = range0; k <= range1; ++k ) {
    for ( int l = range2; l <= range3; ++l ) {   
      double binvalue = h1->GetBinContent(k,l);
      double strip_median = stripsMedian[k-range0];  
      if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]==0) continue; // skip empty strip    
      else if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]!=0
              && stripsAvg[k-range0]!=0) strip_median = stripsAvg[k-range0];
           else if(stripsMedian[k-range0]==0 && stripsVariance[k-range0]!=0
                    && stripsAvg[k-range0]==0) continue;
      double outstandingRatio=0;
      if(fabs(strip_median) > 0.00001 )
         outstandingRatio= binvalue/strip_median;
      else continue;
      double eta = h1->GetXaxis()->GetBinCenter(k);
      double phi = h1->GetYaxis()->GetBinCenter(l);
      clusterbin onebin;
      onebin.m_eta = eta;
      onebin.m_phi = phi;
      onebin.m_value = binvalue;
      onebin.m_OSRatio = outstandingRatio; 
      onebin.m_nx = 0;
      onebin.m_ny = 0;
      if(fabs(outstandingRatio) > RedThreshold_cell) {
       redbins.push_back(onebin);
     }
    }
  }
  std::sort(redbins.begin(),redbins.end(),mySortfunc);
for(unsigned int i=0;i<redbins.size();i++){
   if(redbins[i].m_value / maxInMap < 0.15 ) continue;
   bool match=false;
   clusterbin onebin;
   onebin.m_eta = redbins[i].m_eta;
   onebin.m_phi = redbins[i].m_phi;
   onebin.m_value = redbins[i].m_value;
   h2->Fill(onebin.m_eta,onebin.m_phi,onebin.m_value);
 }
}
void ProcessInWindow(int MODE,TH2F*& h,TH2F*& h2,int ix,int iy,int ix_total,int iy_total,vector<cellbin>& problemCellbinSummary){
  string name = h->GetName();
//  cout<<"processing... "<<name<<endl;
  int partition=-1;
  int layer=-1;
  find_n(name,partition,layer); 
  int startX;
  int endX;
  int startY;
  int endY;
  if(MODE==M_5_5){
    startX = ix - 2;
    endX = ix+2;
    startY = iy -2;
    endY = iy +2;
  }
  else if(MODE==M_3_5){
  	  startX = ix -1;
  	  endX = ix +1;
   	  startY = iy -2;
          endY = iy +2;
        }
  for(int i=startX;i<=endX;i++){
   for(int j=startY;j<=endY;j++){
     double binvalue = h->GetBinContent(i,j);
     if(binvalue>0){
       cellbin onebin;
       onebin.m_eta = h->GetXaxis()->GetBinCenter(i);
       onebin.m_phi = h->GetYaxis()->GetBinCenter(j);
       onebin.m_value = binvalue;
       onebin.m_partition = partition;
       onebin.m_layer = layer;
       onebin.m_name = name;
       onebin.m_level = "normal";
       onebin.m_nx = 0;
       onebin.m_ny = 0;
       onebin.m_side = 0;
       onebin.m_OSRatio = 0;
       if(h2!=NULL) onebin.m_level = h2->GetBinContent(i,j)>0?"hot":"noisy";
       bool flag = false;
       for(int k=0;k<problemCellbinSummary.size();k++){
        if( onebin.m_eta==problemCellbinSummary[k].m_eta 
          && onebin.m_phi==problemCellbinSummary[k].m_phi
          && onebin.m_partition==problemCellbinSummary[k].m_partition
          && onebin.m_layer==problemCellbinSummary[k].m_layer 
          )
         flag=true;
       }
       if(!flag) problemCellbinSummary.push_back(onebin);
     }
   }
  }
}
void boundaryProcess(int MODE,TH2F*& h,TH2F* h2,int ix,int iy,int ix_total,int iy_total,vector<cellbin>& problemCellbinSummary){
 string name = h->GetName();
 int partition=-1;
 int layer=-1;
 find_n(name,partition,layer);
 int startX=0;
 int endX=0;
 int startY=0;
 int endY=0;
 if(MODE==M_5_5){
  startX=ix-2;
  endX=ix+2;
  startY=iy-2;
  endY=iy+2;
  if(ix>ix_total-2) endX=ix_total;
  if(ix<=2)  startX=1;
  if(iy>iy_total-2) endY=iy_total;
  if(iy<=2)  startY=1;
 } 
 else if(MODE==M_3_5){
        startX=ix-1;
        endX=ix+1;
        startY=iy-2;
        endY=iy+2;
	if(ix>ix_total-1)  endX=ix_total;
        if(ix<=1) startX=1;
        if(iy>iy_total-2) endY=iy_total;
        if(iy<=2) startY=1;
      }
  for(int i=startX;i<=endX;i++){
   for(int j=startY;j<=endY;j++){
     double binvalue = h->GetBinContent(i,j);
     if(binvalue>0){
       cellbin onebin;   
       onebin.m_eta = h->GetXaxis()->GetBinCenter(i);
       onebin.m_phi = h->GetYaxis()->GetBinCenter(j);  
       onebin.m_value = binvalue;
       onebin.m_partition = partition;
       onebin.m_layer = layer;
       onebin.m_name = name;
       onebin.m_level = "noisy";
       onebin.m_nx = 0;
       onebin.m_ny = 0;
       onebin.m_side = 0;
       onebin.m_OSRatio = 0;
       if(h2!=NULL) onebin.m_level = h2->GetBinContent(i,j)>0?"hot":"noisy";
       bool flag = false;
       for(int k=0;k<problemCellbinSummary.size();k++){
        if( onebin.m_eta==problemCellbinSummary[k].m_eta
          && onebin.m_phi==problemCellbinSummary[k].m_phi
          && onebin.m_partition==problemCellbinSummary[k].m_partition
          && onebin.m_layer==problemCellbinSummary[k].m_layer
          )
         flag=true;       
       }
       if(!flag) problemCellbinSummary.push_back(onebin);
     }
   }
  }
}
void LinkClusterAndCell(clusterbin clb,TH2F*& h,TH2F*& h2,int MODE,vector<cellbin>& problemCellbinSummary){
 if(h==NULL) return;
 double eta = clb.m_eta;
 double phi = clb.m_phi; 
 int ix = h->GetXaxis()->FindBin(eta);
 int iy = h->GetYaxis()->FindBin(phi);
 int ix_total=h->GetXaxis()->GetNbins();
 int iy_total=h->GetYaxis()->GetNbins();
 if(ix==0 || ix>ix_total || iy==0 || iy>iy_total) return;
 if( (MODE==M_5_5 && (ix > ix_total-2 || ix <=2 || iy > iy_total-2 || iy <= 2))
   ||(MODE==M_3_5 && (ix > ix_total-1 || ix <=1 || iy > iy_total-2 || iy <= 2)) )
  boundaryProcess(MODE,h,h2,ix,iy,ix_total,iy_total,problemCellbinSummary);
 else{
  ProcessInWindow(MODE,h,h2,ix,iy,ix_total,iy_total,problemCellbinSummary);
 }
}
void AdjustLayer(string& layer,string& partition){

 if(layer.compare("Presampler")==0 && (partition.compare("HECA")==0||partition.compare("HECC")==0) ){
   layer="Sampling0";
 }

}
void Maketag(string& info,cellbin bin,int index,string& webadd,list<string>& myweblist){
 char tmp[500];
 string partition = TranslatePartition(bin.m_partition);
 string layer = TranslateLayer(bin.m_layer);
 sprintf(tmp,"  %i.[",index);
 info=tmp;
 info+=bin.m_level;
 sprintf(tmp,"](%0.3f,%0.3f)[%e]",bin.m_eta,bin.m_phi,bin.m_value);
 info+=tmp;
 info+=partition;
 info+=",";
 info+=layer;
 info+="\n";
 AdjustLayer(layer,partition);
 char web[500];
 sprintf(web,webadd.c_str(),partition.c_str(),layer.c_str(),bin.m_name.c_str());
 myweblist.push_back(web);
 myweblist.unique();

}
void Output(const clusterbin& clb,vector<cellbin> problemCellbinSummary,int initial_pos,string& webadd,list<string>& myweblist){
  if(problemCellbinSummary.size()==initial_pos) return;
  for(int i=initial_pos;i<problemCellbinSummary.size();i++){
    string info;
    Maketag(info,problemCellbinSummary[i],i+1,webadd,myweblist);
    outputfile<<"<p>"<<info<<"</p>"<<endl;
  }
}
void setupStyleAndMessage(){
 outputfile<<"<head><br>"<<endl;
 outputfile<<"<style type=\"text/css\">"<<endl; outputfile<<"body {background-color: #F2F5A9}"<<endl;
 outputfile<<"p.no1 {background-color: blue;padding: 20px;}"<<endl; outputfile<<"p.no2 {background-color: gray; padding: 20px;}"<<endl;
 outputfile<<"</style>"<<endl;
 outputfile<<"<body><br>"<<endl;
 outputfile<<"<p align=\"center\"><font size=\"6\"> Remember to  export STAGE_SVCCLASS=atlcal "<<endl;
 outputfile<<"<br> and log on https://atlasdqm.cern.ch/webdisplay/ first to make sure you have the certificate "<<endl;
 outputfile<<"<br> usage: ./CellClusterLink 162347 f287_m585 physics_CosmicCalo data10_7TeV "<<endl;
 outputfile<<"<br> [**]: OSRatio > "<<6*RedThreshold_cluster<<endl;
 outputfile<<"<br> [*] : OSRatio > "<<RedThreshold_cluster<<endl;
 outputfile<<"<br> hot : the cell bins appear both in occupancy plots and average energy plots"<<endl;
outputfile<<"<br> noisy: the cell bins appear only in occupancy plots</p>"<<endl;
 outputfile<<"<hr>"<<endl;
 
}
int main(int narg,char* argv[]){
 if(narg!=5){
   argv[1]=(char*)"166466";
   argv[2]=(char*)"x46_m616";
   argv[3]=(char*)"physics_CosmicCalo";
   argv[4]=(char*)"data10_7TeV";
 } 
 char outfileName[50];
 sprintf(outfileName,"log_%s.html",argv[1]);
// outputfile.open("log.html");
 outputfile.open(outfileName);
 setupStyleAndMessage();

TString nameFile="/castor/cern.ch/grid/atlas/tzero/prod1/perm/"+string(argv[4])+"/"+string(argv[3])+"/0"+ string(argv[1])+"/"+string(argv[4])+".00"+string(argv[1])+"."+string(argv[3])+".merge.HIST."+string(argv[2])+"/"+string(argv[4])+".00"+string(argv[1])+"."+string(argv[3])+".merge.HIST."+string(argv[2])+"._0001.1";
//TString nameFile="/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_hi/express_express/0"+ string(argv[1])+"/data10_hi.00"+string(argv[1])+".express_express.merge.HIST."+string(argv[2])+"/data10_hi.00"+string(argv[1])+".express_express.merge.HIST."+string(argv[2])+"._0001.1";
TString ClusterDir="run_"+string(argv[1])+"/CaloMonitoring/ClusterMon/CaloCalTopoClusterNoTrigSel/2d_Rates/";
 TString CellOccDir="run_"+string(argv[1])+"/CaloMonitoring/LArCellMon_NoTrigSel/2d_Occupancy/";
 TString CellAvgDir="run_"+string(argv[1])+"/CaloMonitoring/LArCellMon_NoTrigSel/2d_AvgEnergy/";
// TFile* myFile=new TFile(nameFile);
 TFile* myFile=TFile::Open(nameFile);
 myFile->cd(ClusterDir);
 TList* listofkeys=gDirectory->GetListOfKeys();
 TIter myiter(listofkeys);
// get cluster hit plots
 int NPlots_cluster_occ = sizeof(ClusterPlotsNames_occ)/sizeof(*ClusterPlotsNames_occ);
 for(int i=0;i<NPlots_cluster_occ;i++){
  h2_cluster_occ[i]=(TH2F*)gDirectory->FindObjectAny(ClusterPlotsNames_occ[i].c_str());
 }
// get cluster avg plots if necessary
 int NPlots_cluster_avg = sizeof(ClusterPlotsNames_avg)/sizeof(*ClusterPlotsNames_avg);
 if(NPlots_cluster_avg>0){
 ClusterDir="run_"+string(argv[1])+"/CaloMonitoring/ClusterMon/CaloCalTopoClusterNoTrigSel/2d_AvEnergy/";
 myFile->cd(ClusterDir);
  for(int i=0;i<NPlots_cluster_avg;i++){
   h2_cluster_avg[i]=(TH2F*)gDirectory->FindObjectAny(ClusterPlotsNames_avg[i].c_str());
  }
 }
 vector<TH2F*> h_clusterMap;
// h_clusterMap.push_back(h_clusterHitMap5GeV);
 for(int i=0;i<NPlots_cluster_occ;i++){
   h_clusterMap.push_back(h2_cluster_occ[i]);
 }
 for(int i=0;i<NPlots_cluster_avg;i++){
   h_clusterMap.push_back(h2_cluster_avg[i]);
 }
 myFile->cd(CellOccDir);
 int NPlots = sizeof(TargetNames)/sizeof(*TargetNames);
 for(int i=0;i<NPlots;i++){
   TH2F* h2_tmp=(TH2F*)gDirectory->FindObjectAny(TargetNames[i].c_str());
   if(h2_tmp==NULL) {outputfile<<"<p>ERROR: missing plot--"<<TargetNames[i]<<"</p>"<<endl;continue;}
   int partition=-1;
   int layer=-1;
   find_n(TargetNames[i],partition,layer);
   h2_occ[partition][layer]=h2_tmp;
   h2_cellStatus_Summary[partition][layer]=(TH2F*)h2_tmp->Clone();
   h2_cellStatus_Summary[partition][layer]->Reset();
}
 int NPlots_avg= sizeof(TargetNames_Avg)/sizeof(*TargetNames_Avg);
 if(NPlots_avg > 0){
   myFile->cd(CellAvgDir);
   for(int i=0;i<NPlots_avg-1;i++){
    TH2F* h2_tmp=(TH2F*)gDirectory->FindObjectAny(TargetNames_Avg[i].c_str());
    if(h2_tmp==NULL) {outputfile<<"<p>ERROR: missing plot--"<<TargetNames_Avg[i]<<"</p>"<<endl;continue;}
    int partition=-1;
    int layer=-1;
    find_n(TargetNames_Avg[i],partition,layer);
    h2_avgEnergy[partition][layer]=h2_tmp;
    h2_cellStatus_Summary_avgEnergy[partition][layer]=(TH2F*)h2_tmp->Clone();
    h2_cellStatus_Summary_avgEnergy[partition][layer]->Reset();
   }
 }
vector<clusterbin> problematicClusters;
vector<cellbin>    problemCellbinSummary;
outputfile<<"<p align=\"center\"><font face=\"verdana\">Your command is "<<argv[0]<<" "<<argv[1]<<" "<<argv[2]<<" "<<argv[3]<<" "<<argv[4]<<"</font></p>"<<endl;
outputfile<<"<p align=\"center\" ><em>";
for(int i=0;i<h_clusterMap.size();i++){
 ProcessByBinsDiffFromStripMedian(h_clusterMap[i],problematicClusters);
 outputfile<<"Cluster Plots used:"<<h_clusterMap[i]->GetTitle()<<endl;
 outputfile<<"<br>"<<endl;
}
outputfile<<"</em></p>"<<endl;
// process all the cell plots and write bin status to h2_cellplotStatus_Summay
for(int i=EMBA;i<NbOfPartition;i++){
 for(int j=Presampler;j<NbOflayers;j++){
   ProcessByBinsDiffFromStripMedian(h2_occ[i][j],h2_cellStatus_Summary[i][j]);
   ProcessByBinsDiffFromStripMedian(h2_avgEnergy[i][j],h2_cellStatus_Summary_avgEnergy[i][j]);
 }
}
if(problematicClusters.size()!=0){
   for (const clusterbin& bin : problematicClusters) {
    outputfile<<"<p class=\"no2\"><big>==>["<<bin.m_level<<"]("<<bin.m_eta<<","<<bin.m_phi<<")["<<bin.m_value<<"]"<<"</big></p>"<<endl;
    list<string> myweblist;
    for(int i=EMBA;i<NbOfPartition;i++){
     for(int j=Presampler;j<NbOflayers;j++){
      int initial_pos = problemCellbinSummary.size();
      LinkClusterAndCell(bin,h2_cellStatus_Summary[i][j],h2_cellStatus_Summary_avgEnergy[i][j],MODE,problemCellbinSummary);
      string webAdd;
      webAdd ="https://atlasdqm.cern.ch/webdisplay/tier0/1/"+string(argv[3])+"/run_"+string(argv[1])+"/run/LAr/%s/Noise/Single_Cells/%s/%s.png";
      Output(bin,problemCellbinSummary,initial_pos,webAdd,myweblist);
     }
    }
    for (const std::string& s : myweblist) {
      outputfile<<"<img src=\""<<s<<"\"/>"<<endl;
    }
    outputfile<<"<hr>"<<endl;
  }
}
return 0;
}

